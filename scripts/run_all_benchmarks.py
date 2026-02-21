#!/usr/bin/env python3
"""
Comprehensive Benchmark Automation Script for KATANA Framework

This script provides a unified, automated benchmark system that:
- Runs all framework benchmarks with detailed measurements
- Tests edge cases and boundary conditions
- Generates comprehensive reports
- Validates performance against baselines
- Can be used in CI/CD and pre-commit hooks

Created for issue #5: benchmarks run and try codegen
"""

import argparse
import json
import os
import re
import subprocess
import sys
import time
import http.client
import signal
from datetime import datetime
from pathlib import Path
from typing import Dict, List, Optional, Tuple, Any
from dataclasses import dataclass, asdict


@dataclass
class BenchmarkResult:
    """Single benchmark result with metadata"""

    name: str
    category: str
    status: str  # success, failed, skipped
    duration_seconds: float
    metrics: Dict[str, Any]
    edge_cases_tested: List[str]
    timestamp: str
    error: Optional[str] = None


@dataclass
class BenchmarkReport:
    """Complete benchmark report"""

    timestamp: str
    commit_hash: str
    branch: str
    total_benchmarks: int
    successful: int
    failed: int
    skipped: int
    total_duration_seconds: float
    results: List[BenchmarkResult]
    edge_cases_summary: Dict[str, int]
    performance_regressions: List[str]
    category_stats: Dict[str, Any]
    core_stats: Dict[str, Any]
    regression_summary: Dict[str, Any]
    normalized_metrics: Dict[str, float]
    baseline_path: Optional[str]
    regression_threshold_pct: float


class BenchmarkRunner:
    """Unified benchmark runner for KATANA framework"""

    def __init__(
        self,
        build_dir: Path,
        verbose: bool = False,
        baseline_path: Optional[Path] = None,
        regression_threshold_pct: float = 5.0,
    ):
        self.build_dir = build_dir
        self.verbose = verbose
        self.baseline_path = baseline_path
        self.regression_threshold_pct = regression_threshold_pct
        self.results: List[BenchmarkResult] = []
        self.servers: List[subprocess.Popen] = []
        self.start_time = time.time()
        self.last_server_error: Optional[str] = None

        # Benchmark categories
        self.categories = {
            "core": "Core runtime components (reactor, HTTP parser, arena)",
            "codegen": "Code generation quality and performance",
            "router": "HTTP routing and dispatching",
            "serialization": "JSON serialization/deserialization",
            "integration": "End-to-end service benchmarks",
            "edge_cases": "Boundary conditions and edge cases",
        }

        # Edge case scenarios to test
        self.edge_cases = {
            "empty_input": "Empty request bodies and strings",
            "max_length": "Maximum allowed string/array lengths",
            "min_value": "Minimum boundary values",
            "max_value": "Maximum boundary values",
            "unicode": "Unicode and special characters",
            "null_handling": "Null and optional field handling",
            "concurrent": "High concurrency scenarios",
            "large_payload": "Large request/response payloads",
            "invalid_input": "Malformed and invalid requests",
            "stress": "Sustained high load",
        }

        self.benchmark_timeouts = {
            "performance_benchmark": 300,
            "simple_benchmark": 180,
        }

    def log(self, message: str, level: str = "INFO"):
        """Log message with timestamp"""
        if self.verbose or level != "DEBUG":
            timestamp = datetime.now().strftime("%H:%M:%S")
            print(f"[{timestamp}] [{level}] {message}", flush=True)

    @staticmethod
    def sanitize_metric_key(raw: str) -> str:
        """Normalize metric names to stable snake_case keys."""
        key = raw.strip().lower()
        key = key.replace("/", "_per_")
        key = key.replace("%", "pct")
        key = key.replace("-", "_")
        key = re.sub(r"\s+", "_", key)
        key = re.sub(r"[^a-z0-9_]", "", key)
        key = re.sub(r"_+", "_", key).strip("_")
        return key or "metric"

    @staticmethod
    def metric_direction(metric_name: str) -> Optional[str]:
        """
        Return metric direction:
        - 'higher': higher values are better
        - 'lower': lower values are better
        - None: neutral metric, skip regression checks
        """
        name = metric_name.lower()

        if "throughput" in name or "ops_per_sec" in name or "req_per_sec" in name:
            return "higher"
        if (
            "latency" in name
            or "duration" in name
            or "error_rate" in name
            or name.endswith("_errors")
            or ".errors" in name
            or ".error" in name
        ):
            return "lower"
        if "operations" in name or "total_requests" in name:
            return None
        return None

    @staticmethod
    def percentile(values: List[float], pct: float) -> float:
        """Compute percentile for already-sorted values."""
        if not values:
            return 0.0
        clamped = min(1.0, max(0.0, pct))
        idx = int(clamped * (len(values) - 1))
        return float(values[idx])

    def summarize_numeric(self, values: List[float]) -> Dict[str, float]:
        """Return compact summary stats for a numeric vector."""
        if not values:
            return {"count": 0}

        ordered = sorted(float(v) for v in values)
        count = len(ordered)
        total = sum(ordered)
        return {
            "count": count,
            "min": ordered[0],
            "max": ordered[-1],
            "mean": total / count,
            "p50": self.percentile(ordered, 0.50),
            "p95": self.percentile(ordered, 0.95),
            "p99": self.percentile(ordered, 0.99),
            "sum": total,
        }

    def normalized_metrics(self) -> Dict[str, float]:
        """Flatten all numeric per-benchmark metrics into one comparable map."""
        flat: Dict[str, float] = {}

        for result in self.results:
            prefix = self.sanitize_metric_key(result.name)
            flat[f"{prefix}.duration_seconds"] = float(result.duration_seconds)

            if result.status == "failed":
                flat[f"{prefix}.failed"] = 1.0
            elif result.status == "skipped":
                flat[f"{prefix}.skipped"] = 1.0
            else:
                flat[f"{prefix}.failed"] = 0.0
                flat[f"{prefix}.skipped"] = 0.0

            for metric_name, value in result.metrics.items():
                if isinstance(value, (int, float)):
                    key = self.sanitize_metric_key(metric_name)
                    flat[f"{prefix}.{key}"] = float(value)

        return flat

    def build_category_stats(self) -> Dict[str, Any]:
        """Build category-level aggregates across benchmark results."""
        stats: Dict[str, Any] = {}
        categories = sorted({r.category for r in self.results} | set(self.categories.keys()))

        for category in categories:
            cat_results = [r for r in self.results if r.category == category]
            successful = [r for r in cat_results if r.status == "success"]

            throughputs: List[float] = []
            latencies_ns: List[float] = []
            durations_ms: List[float] = []
            error_rates: List[float] = []
            errors: List[float] = []

            for result in successful:
                for key, value in result.metrics.items():
                    if not isinstance(value, (int, float)):
                        continue
                    key_lower = key.lower()
                    numeric = float(value)
                    if "ops_per_sec" in key_lower or "throughput" in key_lower:
                        throughputs.append(numeric)
                    if "latency" in key_lower and key_lower.endswith("_ns"):
                        latencies_ns.append(numeric)
                    if key_lower.endswith("duration_ms"):
                        durations_ms.append(numeric)
                    if "error_rate" in key_lower:
                        error_rates.append(numeric)
                    if key_lower == "errors" or key_lower.endswith("_errors"):
                        errors.append(numeric)

            total = len(cat_results)
            success_count = len(successful)
            failed_count = sum(1 for r in cat_results if r.status == "failed")
            skipped_count = sum(1 for r in cat_results if r.status == "skipped")

            stats[category] = {
                "benchmarks_total": total,
                "benchmarks_success": success_count,
                "benchmarks_failed": failed_count,
                "benchmarks_skipped": skipped_count,
                "success_rate_pct": (100.0 * success_count / total) if total else 0.0,
                "total_duration_seconds": sum(r.duration_seconds for r in cat_results),
                "mean_duration_seconds": (
                    sum(r.duration_seconds for r in cat_results) / total if total else 0.0
                ),
                "throughput_ops_per_sec": self.summarize_numeric(throughputs),
                "latency_ns": self.summarize_numeric(latencies_ns),
                "duration_ms": self.summarize_numeric(durations_ms),
                "error_rate_pct": self.summarize_numeric(error_rates),
                "errors": self.summarize_numeric(errors),
            }

        return stats

    def build_core_stats(self, category_stats: Dict[str, Any]) -> Dict[str, Any]:
        """Build enriched core-only stats used in summaries and dashboards."""
        core = dict(category_stats.get("core", {}))
        core_results = [r for r in self.results if r.category == "core" and r.status == "success"]

        top_throughput: List[Tuple[str, float]] = []
        slowest_latency_p99: List[Tuple[str, float]] = []

        for result in core_results:
            for key, value in result.metrics.items():
                if not isinstance(value, (int, float)):
                    continue
                metric_name = key.lower()
                metric_value = float(value)
                if "throughput" in metric_name or metric_name.endswith("ops_per_sec"):
                    top_throughput.append((f"{result.name}.{key}", metric_value))
                if "latency_p99_ns" in metric_name:
                    slowest_latency_p99.append((f"{result.name}.{key}", metric_value))

        top_throughput.sort(key=lambda item: item[1], reverse=True)
        slowest_latency_p99.sort(key=lambda item: item[1], reverse=True)

        core["top_throughput_ops_per_sec"] = [
            {"metric": name, "value": value} for name, value in top_throughput[:10]
        ]
        core["worst_p99_latency_ns"] = [
            {"metric": name, "value": value} for name, value in slowest_latency_p99[:10]
        ]
        return core

    def load_baseline_metrics(self) -> Dict[str, float]:
        """Load flattened baseline metrics."""
        if not self.baseline_path:
            return {}

        if not self.baseline_path.exists():
            self.log(f"Baseline not found: {self.baseline_path}", "WARNING")
            return {}

        try:
            with open(self.baseline_path, "r") as f:
                data = json.load(f)
        except Exception as e:
            self.log(f"Failed to load baseline {self.baseline_path}: {e}", "WARNING")
            return {}

        payload = data.get("metrics", data) if isinstance(data, dict) else {}
        if not isinstance(payload, dict):
            return {}

        baseline: Dict[str, float] = {}
        for key, value in payload.items():
            if isinstance(value, (int, float)):
                baseline[str(key)] = float(value)
        return baseline

    def compare_with_baseline(
        self,
        current_metrics: Dict[str, float],
    ) -> Tuple[List[str], Dict[str, Any]]:
        """Compare current metrics with baseline and return regressions + summary."""
        baseline_metrics = self.load_baseline_metrics()
        threshold = float(self.regression_threshold_pct)

        if not baseline_metrics:
            return [], {
                "baseline_loaded": False,
                "baseline_path": str(self.baseline_path) if self.baseline_path else None,
                "threshold_pct": threshold,
                "compared_metrics": 0,
                "regressions": 0,
                "improvements": 0,
                "unchanged": 0,
                "details": [],
            }

        details: List[Dict[str, Any]] = []
        regressions: List[Tuple[str, float, float, float]] = []
        improvements = 0
        unchanged = 0

        for key in sorted(set(current_metrics.keys()) & set(baseline_metrics.keys())):
            direction = self.metric_direction(key)
            if direction is None:
                continue

            baseline = baseline_metrics[key]
            current = current_metrics[key]
            if baseline == 0.0:
                continue

            delta_pct = ((current - baseline) / abs(baseline)) * 100.0

            if direction == "higher":
                is_regression = delta_pct < -threshold
                is_improvement = delta_pct > threshold
            else:
                is_regression = delta_pct > threshold
                is_improvement = delta_pct < -threshold

            status = "unchanged"
            if is_regression:
                status = "regressed"
                regressions.append((key, delta_pct, baseline, current))
            elif is_improvement:
                status = "improved"
                improvements += 1
            else:
                unchanged += 1

            details.append(
                {
                    "metric": key,
                    "direction": direction,
                    "baseline": baseline,
                    "current": current,
                    "change_pct": round(delta_pct, 3),
                    "status": status,
                }
            )

        regressions.sort(
            key=lambda row: abs(row[1]),
            reverse=True,
        )
        regression_messages = [
            f"{name}: {change:+.2f}% (baseline={base:.6g}, current={cur:.6g})"
            for name, change, base, cur in regressions
        ]

        summary = {
            "baseline_loaded": True,
            "baseline_path": str(self.baseline_path),
            "threshold_pct": threshold,
            "compared_metrics": len(details),
            "regressions": len(regressions),
            "improvements": improvements,
            "unchanged": unchanged,
            "details": details,
        }
        return regression_messages, summary

    def write_baseline(self, output_path: Path, metrics: Dict[str, float]) -> None:
        """Write flattened metrics as new baseline."""
        baseline = {
            "generated_at": datetime.now().isoformat(),
            "commit": self.safe_git_value(["git", "rev-parse", "--short", "HEAD"]),
            "branch": self.safe_git_value(["git", "rev-parse", "--abbrev-ref", "HEAD"]),
            "metrics": metrics,
        }
        output_path.parent.mkdir(parents=True, exist_ok=True)
        with open(output_path, "w") as f:
            json.dump(baseline, f, indent=2)
        self.log(f"Baseline updated: {output_path}")

    def safe_git_value(self, cmd: List[str]) -> str:
        """Read git metadata without raising."""
        try:
            return subprocess.check_output(cmd).decode().strip()
        except Exception:
            return "unknown"

    def run_command(
        self,
        cmd: List[str],
        timeout: int = 60,
        capture: bool = True,
        env: Optional[Dict[str, str]] = None,
    ) -> Tuple[int, str, str]:
        """Run command and return exit code, stdout, stderr"""
        try:
            if capture:
                result = subprocess.run(
                    cmd,
                    capture_output=True,
                    text=True,
                    timeout=timeout,
                    check=False,
                    env=env,
                )
                return result.returncode, result.stdout, result.stderr
            else:
                result = subprocess.run(cmd, timeout=timeout, check=False, env=env)
                return result.returncode, "", ""
        except subprocess.TimeoutExpired:
            return -1, "", f"Command timed out after {timeout}s"
        except Exception as e:
            return -1, "", str(e)

    def find_binary(self, relative_paths: List[Path]) -> Optional[Path]:
        """Find first existing binary from candidate paths."""
        for rel in relative_paths:
            candidate = self.build_dir / rel
            if candidate.exists():
                return candidate

        # Also support callers that pass paths relative to build root parent
        for rel in relative_paths:
            candidate = self.build_dir.parent / rel
            if candidate.exists():
                return candidate

        return None

    def start_server(
        self, binary: Path, port: int, ready_check_path: str = "/"
    ) -> Optional[subprocess.Popen]:
        """Start a server and wait for it to be ready"""
        self.last_server_error = None
        if not binary.exists():
            self.log(f"Server binary not found: {binary}", "WARNING")
            return None

        try:
            # Start server in background
            proc = subprocess.Popen(
                [str(binary), str(port)],
                stdout=subprocess.DEVNULL,
                stderr=subprocess.PIPE,
                text=True,
            )

            # Wait for server to be ready
            deadline = time.time() + 10.0
            while time.time() < deadline:
                if proc.poll() is not None:
                    stderr_tail = ""
                    try:
                        _, stderr_tail = proc.communicate(timeout=0.2)
                    except Exception:
                        stderr_tail = ""
                    self.last_server_error = stderr_tail.strip() or "server exited during startup"
                    self.log(f"Server exited during startup: {binary}", "ERROR")
                    return None

                try:
                    conn = http.client.HTTPConnection("localhost", port, timeout=1.0)
                    conn.request("GET", ready_check_path)
                    resp = conn.getresponse()
                    resp.read()
                    conn.close()
                    self.log(f"Server ready on port {port}: {binary.name}", "DEBUG")
                    self.servers.append(proc)
                    return proc
                except Exception:
                    time.sleep(0.2)

            self.log(f"Server did not become ready: {binary}", "ERROR")
            proc.terminate()
            try:
                _, stderr_tail = proc.communicate(timeout=0.2)
                self.last_server_error = stderr_tail.strip() or "server did not become ready"
            except Exception:
                self.last_server_error = "server did not become ready"
            return None

        except Exception as e:
            self.last_server_error = str(e)
            self.log(f"Failed to start server {binary}: {e}", "ERROR")
            return None

    @staticmethod
    def is_network_restricted_error(message: Optional[str]) -> bool:
        """Detect sandbox/network restrictions that are not product regressions."""
        if not message:
            return False
        lowered = message.lower()
        return (
            "operation not permitted" in lowered
            or "permission denied" in lowered
            or "address already in use" in lowered
            or "failed to start listeners" in lowered
        )

    def stop_servers(self):
        """Stop all running servers"""
        for proc in self.servers:
            try:
                proc.send_signal(signal.SIGINT)
                proc.wait(timeout=5)
            except Exception:
                proc.kill()
        self.servers.clear()

    def run_benchmark_binary(
        self,
        name: str,
        binary: Path,
        category: str,
        args: Optional[List[str]] = None,
        timeout: Optional[int] = None,
        env: Optional[Dict[str, str]] = None,
    ) -> BenchmarkResult:
        """Run a single benchmark binary"""
        self.log(f"Running {name}...", "INFO")

        if not binary.exists():
            return BenchmarkResult(
                name=name,
                category=category,
                status="skipped",
                duration_seconds=0.0,
                metrics={},
                edge_cases_tested=[],
                timestamp=datetime.now().isoformat(),
                error="Binary not found",
            )

        cmd = [str(binary)] + (args or [])
        effective_timeout = timeout if timeout is not None else self.benchmark_timeouts.get(name, 120)

        start = time.time()
        returncode, stdout, stderr = self.run_command(cmd, timeout=effective_timeout, env=env)
        duration = time.time() - start

        if returncode != 0:
            return BenchmarkResult(
                name=name,
                category=category,
                status="failed",
                duration_seconds=duration,
                metrics={},
                edge_cases_tested=[],
                timestamp=datetime.now().isoformat(),
                error=f"Exit code {returncode}: {stderr[:200]}",
            )

        # Parse metrics from output
        metrics = self.parse_benchmark_output(stdout)

        return BenchmarkResult(
            name=name,
            category=category,
            status="success",
            duration_seconds=duration,
            metrics=metrics,
            edge_cases_tested=["basic_functionality"],
            timestamp=datetime.now().isoformat(),
        )

    def parse_benchmark_output(self, output: str) -> Dict[str, Any]:
        """Parse benchmark output to extract metrics"""
        metrics: Dict[str, Any] = {}

        table_pattern = re.compile(
            r"^\s+(.+?)\s+([\d.]+)\s+(ns|us)\s+([\d.]+)\s+ops/sec$",
            re.IGNORECASE,
        )

        for raw_line in output.split("\n"):
            line = raw_line.strip()
            if not line:
                continue

            # Standard table rows: <name> <latency> <unit> <throughput> ops/sec
            m = table_pattern.match(raw_line)
            if m:
                name = self.sanitize_metric_key(m.group(1))
                latency = float(m.group(2))
                unit = m.group(3).lower()
                throughput = float(m.group(4))
                latency_ns = latency if unit == "ns" else latency * 1000.0
                metrics[f"{name}_ns_per_op"] = latency_ns
                metrics[f"{name}_ops_per_sec"] = throughput
                continue

            # Summary rows from performance benchmark:
            # <name> : <value> ops/sec
            m = re.match(r"^(.+?)\s*:\s*([\d.]+)\s+ops/sec$", line, re.IGNORECASE)
            if m:
                metric_name = self.sanitize_metric_key(m.group(1))
                metrics[f"{metric_name}_ops_per_sec"] = float(m.group(2))
                continue

            # Throughput: <value> [M] ops/sec
            m = re.match(
                r"^Throughput:\s*([\d.]+)\s*(M)?\s*ops/(?:sec|s)$",
                line,
                re.IGNORECASE,
            )
            if m:
                value = float(m.group(1))
                if m.group(2):
                    value *= 1_000_000.0
                metrics["throughput_ops_per_sec"] = value
                continue

            # Duration: <value> ms
            m = re.match(r"^Duration:\s*([\d.]+)\s*ms$", line, re.IGNORECASE)
            if m:
                metrics["duration_ms"] = float(m.group(1))
                continue

            # Operations / Errors
            m = re.match(r"^Operations:\s*([\d.]+)$", line, re.IGNORECASE)
            if m:
                metrics["operations"] = float(m.group(1))
                continue

            m = re.match(r"^Errors:\s*(\d+)$", line, re.IGNORECASE)
            if m:
                metrics["errors"] = float(m.group(1))
                continue

            # Latency pXX: value unit
            m = re.match(
                r"^Latency\s+p(\d+(?:\.\d+)?):\s*([\d.]+)\s*(ns|us|ms)$",
                line,
                re.IGNORECASE,
            )
            if m:
                p = m.group(1).replace(".", "_")
                value = float(m.group(2))
                unit = m.group(3).lower()
                if unit == "us":
                    value *= 1000.0
                elif unit == "ms":
                    value *= 1_000_000.0
                metrics[f"latency_p{p}_ns"] = value
                continue

            # Compact percentile line:
            # p50: ... | p95: ... | p99: ... | p999: ... | errors: X% (A/B)
            for token in [t.strip() for t in line.split("|")]:
                p_match = re.match(
                    r"^p(\d+(?:\.\d+)?):\s*([\d.]+)\s*(ns|us|ms)$",
                    token,
                    re.IGNORECASE,
                )
                if p_match:
                    p = p_match.group(1).replace(".", "_")
                    value = float(p_match.group(2))
                    unit = p_match.group(3).lower()
                    if unit == "us":
                        value *= 1000.0
                    elif unit == "ms":
                        value *= 1_000_000.0
                    metrics[f"latency_p{p}_ns"] = value
                    continue

                err_match = re.match(
                    r"^errors:\s*([\d.]+)%\s*\((\d+)/(\d+)\)$",
                    token,
                    re.IGNORECASE,
                )
                if err_match:
                    metrics["error_rate_pct"] = float(err_match.group(1))
                    metrics["errors"] = float(err_match.group(2))
                    metrics["total_requests"] = float(err_match.group(3))

        return metrics

    def test_task_api_edge_cases(self, port: int = 18081) -> List[BenchmarkResult]:
        """Test task API with comprehensive edge cases"""
        results = []
        base_url = f"localhost:{port}"

        edge_case_tests = [
            # Minimum values
            {
                "name": "task_api_min_title",
                "edge_case": "min_value",
                "method": "POST",
                "path": "/tasks",
                "body": '{"title":"A","priority":1}',
                "expect_status": 201,
            },
            # Maximum values
            {
                "name": "task_api_max_title",
                "edge_case": "max_value",
                "method": "POST",
                "path": "/tasks",
                "body": '{"title":"' + "X" * 200 + '","priority":5}',
                "expect_status": 201,
            },
            # Maximum description
            {
                "name": "task_api_max_description",
                "edge_case": "max_length",
                "method": "POST",
                "path": "/tasks",
                "body": '{"title":"Test","description":"' + "X" * 2000 + '","priority":3}',
                "expect_status": 201,
            },
            # Maximum tags
            {
                "name": "task_api_max_tags",
                "edge_case": "max_value",
                "method": "POST",
                "path": "/tasks",
                "body": '{"title":"Test","priority":3,"tags":['
                + ",".join(f'"tag{i}"' for i in range(20))
                + "]}",
                "expect_status": 201,
            },
            # Null optional fields
            {
                "name": "task_api_null_fields",
                "edge_case": "null_handling",
                "method": "POST",
                "path": "/tasks",
                "body": '{"title":"Test","priority":3,"assignee_id":null,"due_date":null}',
                "expect_status": 201,
            },
            # Batch minimum
            {
                "name": "task_api_batch_min",
                "edge_case": "min_value",
                "method": "POST",
                "path": "/tasks/batch",
                "body": '{"tasks":[{"title":"Task1","priority":1}]}',
                "expect_status": 200,
            },
            # Batch maximum
            {
                "name": "task_api_batch_max",
                "edge_case": "max_value",
                "method": "POST",
                "path": "/tasks/batch",
                "body": '{"tasks":['
                + ",".join('{"title":"T","priority":1}' for _ in range(100))
                + "]}",
                "expect_status": 200,
            },
            # Query parameters edge cases
            {
                "name": "task_api_query_max_limit",
                "edge_case": "max_value",
                "method": "GET",
                "path": "/tasks?limit=100&offset=0",
                "body": None,
                "expect_status": 200,
            },
            # Search with all filters
            {
                "name": "task_api_search_complex",
                "edge_case": "stress",
                "method": "POST",
                "path": "/tasks/search",
                "body": '{"statuses":["pending","in_progress","completed","cancelled"],"min_priority":1,"max_priority":5}',
                "expect_status": 200,
            },
            # Invalid: title too long
            {
                "name": "task_api_invalid_title_length",
                "edge_case": "invalid_input",
                "method": "POST",
                "path": "/tasks",
                "body": '{"title":"' + "X" * 201 + '","priority":3}',
                "expect_status": 400,
            },
            # Invalid: priority out of range
            {
                "name": "task_api_invalid_priority",
                "edge_case": "invalid_input",
                "method": "POST",
                "path": "/tasks",
                "body": '{"title":"Test","priority":6}',
                "expect_status": 400,
            },
        ]

        for test in edge_case_tests:
            start = time.time()
            try:
                conn = http.client.HTTPConnection(base_url, timeout=5.0)
                if test["body"]:
                    conn.request(
                        test["method"],
                        test["path"],
                        body=test["body"],
                        headers={"Content-Type": "application/json"},
                    )
                else:
                    conn.request(test["method"], test["path"])

                resp = conn.getresponse()
                body = resp.read()
                conn.close()

                duration = time.time() - start
                success = resp.status == test["expect_status"]

                results.append(
                    BenchmarkResult(
                        name=test["name"],
                        category="edge_cases",
                        status="success" if success else "failed",
                        duration_seconds=duration,
                        metrics={
                            "http_status": resp.status,
                            "expected_status": test["expect_status"],
                            "response_size": len(body),
                            "latency_ms": duration * 1000,
                        },
                        edge_cases_tested=[test["edge_case"]],
                        timestamp=datetime.now().isoformat(),
                        error=None
                        if success
                        else f"Expected {test['expect_status']}, got {resp.status}",
                    )
                )

            except Exception as e:
                results.append(
                    BenchmarkResult(
                        name=test["name"],
                        category="edge_cases",
                        status="failed",
                        duration_seconds=time.time() - start,
                        metrics={},
                        edge_cases_tested=[test["edge_case"]],
                        timestamp=datetime.now().isoformat(),
                        error=str(e),
                    )
                )

        return results

    def run_all_benchmarks(self) -> BenchmarkReport:
        """Run all benchmarks and generate comprehensive report"""
        self.log("=" * 60)
        self.log("KATANA Comprehensive Benchmark Suite")
        self.log("=" * 60)

        # Core runtime benchmarks
        self.log("\n==> Running Core Runtime Benchmarks")
        core_benchmarks = [
            ("simple_benchmark", "core"),
            ("performance_benchmark", "core"),
            ("mpsc_benchmark", "core"),
            ("timer_benchmark", "core"),
            ("io_buffer_benchmark", "core"),
            ("headers_benchmark", "core"),
        ]

        for name, category in core_benchmarks:
            binary = self.build_dir / "benchmark" / name

            if name == "simple_benchmark":
                server_binary = self.find_binary(
                    [
                        Path("hello_world_server"),
                        Path("examples/hello_world_server"),
                    ]
                )
                if not server_binary:
                    self.log("hello_world_server binary not found, skipping simple_benchmark", "WARNING")
                    self.results.append(
                        BenchmarkResult(
                            name=name,
                            category=category,
                            status="skipped",
                            duration_seconds=0.0,
                            metrics={},
                            edge_cases_tested=[],
                            timestamp=datetime.now().isoformat(),
                            error="hello_world_server not found",
                        )
                    )
                    continue

                server = self.start_server(server_binary, 18080, "/")
                if not server:
                    error_message = "hello_world_server failed to start"
                    status = "failed"
                    if self.is_network_restricted_error(self.last_server_error):
                        status = "skipped"
                        error_message = (
                            "hello_world_server skipped: environment blocks listener startup"
                        )
                    elif self.last_server_error:
                        error_message = f"hello_world_server failed to start: {self.last_server_error}"
                    self.results.append(
                        BenchmarkResult(
                            name=name,
                            category=category,
                            status=status,
                            duration_seconds=0.0,
                            metrics={},
                            edge_cases_tested=[],
                            timestamp=datetime.now().isoformat(),
                            error=error_message,
                        )
                    )
                    continue

                result = self.run_benchmark_binary(
                    name,
                    binary,
                    category,
                    args=["18080"],
                    env={**os.environ, "HELLO_PORT": "18080"},
                )
                self.results.append(result)
                self.stop_servers()
                continue

            result = self.run_benchmark_binary(name, binary, category)
            self.results.append(result)

        # Codegen benchmarks
        self.log("\n==> Running Codegen Quality Benchmarks")
        codegen_benchmarks = [
            ("codegen_quality_benchmark", "codegen"),
            ("generated_json_benchmark", "codegen"),
            ("generated_api_benchmark", "codegen"),
            ("products_api_benchmark", "codegen"),
            ("benchmark_api_codegen_benchmark", "codegen"),
        ]

        for name, category in codegen_benchmarks:
            binary = self.build_dir / "benchmark" / name
            result = self.run_benchmark_binary(name, binary, category)
            self.results.append(result)

        # Router benchmarks
        self.log("\n==> Running Router Benchmarks")
        result = self.run_benchmark_binary(
            "router_benchmark", self.build_dir / "benchmark" / "router_benchmark", "router"
        )
        self.results.append(result)

        # Serialization benchmarks
        self.log("\n==> Running Serialization Benchmarks")
        serialize_benchmarks = [
            ("serialize_benchmark", "serialization"),
            ("json_benchmark", "serialization"),
            ("json_parsing_benchmark", "serialization"),
        ]

        for name, category in serialize_benchmarks:
            binary = self.build_dir / "benchmark" / name
            result = self.run_benchmark_binary(name, binary, category)
            self.results.append(result)

        # Integration benchmarks - Task API edge cases
        self.log("\n==> Running Task API Integration Benchmarks")
        task_api_binary = self.find_binary(
            [
                Path("examples/codegen/task_api/task_api"),
                Path("examples/examples/codegen/task_api/task_api"),
            ]
        )

        if task_api_binary and task_api_binary.exists():
            server = self.start_server(task_api_binary, 18081, "/health")
            if server:
                time.sleep(1)  # Let server stabilize
                edge_case_results = self.test_task_api_edge_cases(18081)
                self.results.extend(edge_case_results)
                self.log(f"Completed {len(edge_case_results)} edge case tests")
            else:
                if self.is_network_restricted_error(self.last_server_error):
                    self.log(
                        "task_api edge cases skipped: environment blocks listener startup",
                        "WARNING",
                    )
                else:
                    self.log("Failed to start task_api server, skipping edge case tests", "WARNING")
        else:
            self.log("task_api binary not found in expected build locations, skipping", "WARNING")

        # Stop all servers
        self.stop_servers()

        # Generate report
        return self.generate_report()

    def generate_report(self) -> BenchmarkReport:
        """Generate comprehensive benchmark report"""
        total_duration = time.time() - self.start_time

        successful = sum(1 for r in self.results if r.status == "success")
        failed = sum(1 for r in self.results if r.status == "failed")
        skipped = sum(1 for r in self.results if r.status == "skipped")

        # Count edge cases tested
        edge_cases_summary = {}
        for result in self.results:
            for edge_case in result.edge_cases_tested:
                edge_cases_summary[edge_case] = edge_cases_summary.get(edge_case, 0) + 1

        # Get git info
        commit = self.safe_git_value(["git", "rev-parse", "--short", "HEAD"])
        branch = self.safe_git_value(["git", "rev-parse", "--abbrev-ref", "HEAD"])

        category_stats = self.build_category_stats()
        core_stats = self.build_core_stats(category_stats)
        normalized_metrics = self.normalized_metrics()
        performance_regressions, regression_summary = self.compare_with_baseline(normalized_metrics)

        report = BenchmarkReport(
            timestamp=datetime.now().isoformat(),
            commit_hash=commit,
            branch=branch,
            total_benchmarks=len(self.results),
            successful=successful,
            failed=failed,
            skipped=skipped,
            total_duration_seconds=total_duration,
            results=self.results,
            edge_cases_summary=edge_cases_summary,
            performance_regressions=performance_regressions,
            category_stats=category_stats,
            core_stats=core_stats,
            regression_summary=regression_summary,
            normalized_metrics=normalized_metrics,
            baseline_path=str(self.baseline_path) if self.baseline_path else None,
            regression_threshold_pct=self.regression_threshold_pct,
        )

        return report

    def print_summary(self, report: BenchmarkReport):
        """Print human-readable summary"""
        self.log("\n" + "=" * 60)
        self.log("BENCHMARK SUMMARY")
        self.log("=" * 60)
        self.log(f"Timestamp: {report.timestamp}")
        self.log(f"Commit: {report.commit_hash}")
        self.log(f"Branch: {report.branch}")
        self.log(f"Duration: {report.total_duration_seconds:.2f}s")
        self.log("")
        self.log(f"Total Benchmarks: {report.total_benchmarks}")
        self.log(f"  ✓ Successful: {report.successful}")
        self.log(f"  ✗ Failed: {report.failed}")
        self.log(f"  ○ Skipped: {report.skipped}")
        self.log("")

        if report.core_stats:
            self.log("Core Stats:")
            self.log(
                f"  Success rate: {report.core_stats.get('success_rate_pct', 0.0):.1f}% "
                f"({report.core_stats.get('benchmarks_success', 0)}/"
                f"{report.core_stats.get('benchmarks_total', 0)})"
            )
            throughput_stats = report.core_stats.get("throughput_ops_per_sec", {})
            latency_stats = report.core_stats.get("latency_ns", {})
            if throughput_stats.get("count", 0) > 0:
                self.log(
                    f"  Throughput metrics: {int(throughput_stats['count'])}, "
                    f"mean={throughput_stats.get('mean', 0.0):.2f} ops/s, "
                    f"p50={throughput_stats.get('p50', 0.0):.2f} ops/s, "
                    f"max={throughput_stats.get('max', 0.0):.2f} ops/s"
                )
            if latency_stats.get("count", 0) > 0:
                self.log(
                    f"  Latency metrics: {int(latency_stats['count'])}, "
                    f"p50={latency_stats.get('p50', 0.0):.2f} ns, "
                    f"p99={latency_stats.get('p99', 0.0):.2f} ns, "
                    f"max={latency_stats.get('max', 0.0):.2f} ns"
                )
            self.log("")

        if report.edge_cases_summary:
            self.log("Edge Cases Tested:")
            for edge_case, count in sorted(report.edge_cases_summary.items()):
                self.log(f"  {edge_case}: {count} tests")
            self.log("")

        if report.failed > 0:
            self.log("Failed Benchmarks:")
            for result in report.results:
                if result.status == "failed":
                    self.log(f"  ✗ {result.name}: {result.error}")
            self.log("")

        regression_summary = report.regression_summary or {}
        if regression_summary.get("baseline_loaded"):
            self.log("Baseline Comparison:")
            self.log(f"  Compared metrics: {regression_summary.get('compared_metrics', 0)}")
            self.log(f"  Regressions: {regression_summary.get('regressions', 0)}")
            self.log(f"  Improvements: {regression_summary.get('improvements', 0)}")
            self.log(f"  Threshold: {regression_summary.get('threshold_pct', 0.0):.2f}%")
            if report.performance_regressions:
                self.log("  Top regressions:")
                for item in report.performance_regressions[:5]:
                    self.log(f"    - {item}")
            self.log("")
        elif self.baseline_path:
            self.log(
                f"Baseline comparison skipped: baseline file not found ({self.baseline_path})",
                "WARNING",
            )
            self.log("")

        self.log("=" * 60)

    def save_report(self, report: BenchmarkReport, output_file: Path):
        """Save report to JSON file"""
        with open(output_file, "w") as f:
            json.dump(asdict(report), f, indent=2)
        self.log(f"Report saved to: {output_file}")


def main():
    parser = argparse.ArgumentParser(
        description="Comprehensive benchmark automation for KATANA framework"
    )
    parser.add_argument(
        "--build-dir",
        type=Path,
        default=Path("build/bench"),
        help="Build directory containing benchmarks",
    )
    parser.add_argument(
        "--output",
        type=Path,
        default=Path("benchmark_results/report.json"),
        help="Output file for JSON report",
    )
    parser.add_argument(
        "--verbose", "-v", action="store_true", help="Enable verbose logging"
    )
    parser.add_argument(
        "--baseline",
        type=Path,
        default=Path("benchmarks/baseline_comprehensive.json"),
        help="Baseline JSON for regression detection (default: benchmarks/baseline_comprehensive.json)",
    )
    parser.add_argument(
        "--regression-threshold-pct",
        type=float,
        default=5.0,
        help="Regression threshold in percent (default: 5.0)",
    )
    parser.add_argument(
        "--fail-on-regression",
        action="store_true",
        help="Exit with non-zero code when regressions exceed threshold",
    )
    parser.add_argument(
        "--update-baseline",
        action="store_true",
        help="Write current normalized metrics to --baseline file",
    )
    parser.add_argument(
        "--fail-on-error",
        action="store_true",
        help="Exit with non-zero code if any benchmark fails",
    )

    args = parser.parse_args()

    # Create output directory
    args.output.parent.mkdir(parents=True, exist_ok=True)

    # Run benchmarks
    runner = BenchmarkRunner(
        args.build_dir,
        verbose=args.verbose,
        baseline_path=args.baseline,
        regression_threshold_pct=args.regression_threshold_pct,
    )
    report = runner.run_all_benchmarks()

    # Print summary
    runner.print_summary(report)

    # Save report
    runner.save_report(report, args.output)

    if args.update_baseline:
        runner.write_baseline(args.baseline, report.normalized_metrics)

    # Exit with appropriate code
    if args.fail_on_error and report.failed > 0:
        sys.exit(1)

    regression_summary = report.regression_summary or {}
    if (
        args.fail_on_regression
        and regression_summary.get("baseline_loaded")
        and regression_summary.get("regressions", 0) > 0
    ):
        sys.exit(1)

    sys.exit(0)


if __name__ == "__main__":
    main()
