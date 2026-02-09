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
import subprocess
import sys
import time
import http.client
import signal
import shutil
import tempfile
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


class BenchmarkRunner:
    """Unified benchmark runner for KATANA framework"""

    def __init__(self, build_dir: Path, verbose: bool = False):
        self.build_dir = build_dir
        self.verbose = verbose
        self.results: List[BenchmarkResult] = []
        self.servers: List[subprocess.Popen] = []
        self.start_time = time.time()

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

    def log(self, message: str, level: str = "INFO"):
        """Log message with timestamp"""
        if self.verbose or level != "DEBUG":
            timestamp = datetime.now().strftime("%H:%M:%S")
            print(f"[{timestamp}] [{level}] {message}", flush=True)

    def run_command(
        self, cmd: List[str], timeout: int = 60, capture: bool = True
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
                )
                return result.returncode, result.stdout, result.stderr
            else:
                result = subprocess.run(cmd, timeout=timeout, check=False)
                return result.returncode, "", ""
        except subprocess.TimeoutExpired:
            return -1, "", f"Command timed out after {timeout}s"
        except Exception as e:
            return -1, "", str(e)

    def start_server(
        self, binary: Path, port: int, ready_check_path: str = "/"
    ) -> Optional[subprocess.Popen]:
        """Start a server and wait for it to be ready"""
        if not binary.exists():
            self.log(f"Server binary not found: {binary}", "WARNING")
            return None

        try:
            # Start server in background
            proc = subprocess.Popen(
                [str(binary), str(port)],
                stdout=subprocess.DEVNULL,
                stderr=subprocess.DEVNULL,
            )

            # Wait for server to be ready
            deadline = time.time() + 10.0
            while time.time() < deadline:
                if proc.poll() is not None:
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
            return None

        except Exception as e:
            self.log(f"Failed to start server {binary}: {e}", "ERROR")
            return None

    def stop_servers(self):
        """Stop all running servers"""
        for proc in self.servers:
            try:
                proc.send_signal(signal.SIGINT)
                proc.wait(timeout=5)
            except Exception:
                proc.kill()
        self.servers.clear()

    def run_benchmark_binary(self, name: str, binary: Path, category: str) -> BenchmarkResult:
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

        start = time.time()
        returncode, stdout, stderr = self.run_command([str(binary)], timeout=120)
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
        metrics = {}

        # Common patterns in benchmark output
        for line in output.split("\n"):
            line = line.strip()

            # Throughput patterns
            if "ops/sec" in line or "req/sec" in line:
                try:
                    parts = line.split()
                    value_idx = next(i for i, p in enumerate(parts) if "/" in p)
                    value = float(parts[value_idx - 1])
                    name = "_".join(parts[:value_idx - 1]).lower()
                    metrics[f"{name}_throughput"] = value
                except (ValueError, StopIteration, IndexError):
                    pass

            # Latency patterns
            if " us" in line or " ms" in line:
                try:
                    parts = line.split()
                    for i, part in enumerate(parts):
                        if part in ["us", "ms"]:
                            value = float(parts[i - 1])
                            if part == "ms":
                                value *= 1000  # Convert to us
                            name = "_".join(parts[:i - 1]).lower().replace(":", "")
                            metrics[f"{name}_latency_us"] = value
                except (ValueError, IndexError):
                    pass

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
            result = self.run_benchmark_binary(name, binary, category)
            self.results.append(result)

        # Codegen benchmarks
        self.log("\n==> Running Codegen Quality Benchmarks")
        codegen_benchmarks = [
            ("codegen_quality_benchmark", "codegen"),
            ("generated_json_benchmark", "codegen"),
            ("generated_api_benchmark", "codegen"),
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
        task_api_binary = self.build_dir.parent / "examples" / "examples" / "codegen" / "task_api" / "task_api"

        if task_api_binary.exists():
            server = self.start_server(task_api_binary, 18081, "/health")
            if server:
                time.sleep(1)  # Let server stabilize
                edge_case_results = self.test_task_api_edge_cases(18081)
                self.results.extend(edge_case_results)
                self.log(f"Completed {len(edge_case_results)} edge case tests")
            else:
                self.log("Failed to start task_api server, skipping edge case tests", "WARNING")
        else:
            self.log(f"task_api binary not found at {task_api_binary}, skipping", "WARNING")

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
        try:
            commit = (
                subprocess.check_output(["git", "rev-parse", "--short", "HEAD"])
                .decode()
                .strip()
            )
        except Exception:
            commit = "unknown"

        try:
            branch = (
                subprocess.check_output(["git", "rev-parse", "--abbrev-ref", "HEAD"])
                .decode()
                .strip()
            )
        except Exception:
            branch = "unknown"

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
            performance_regressions=[],  # TODO: Compare with baseline
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
        "--fail-on-error",
        action="store_true",
        help="Exit with non-zero code if any benchmark fails",
    )

    args = parser.parse_args()

    # Create output directory
    args.output.parent.mkdir(parents=True, exist_ok=True)

    # Run benchmarks
    runner = BenchmarkRunner(args.build_dir, verbose=args.verbose)
    report = runner.run_all_benchmarks()

    # Print summary
    runner.print_summary(report)

    # Save report
    runner.save_report(report, args.output)

    # Exit with appropriate code
    if args.fail_on_error and report.failed > 0:
        sys.exit(1)

    sys.exit(0)


if __name__ == "__main__":
    main()
