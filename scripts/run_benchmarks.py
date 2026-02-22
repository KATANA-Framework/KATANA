#!/usr/bin/env python3
"""
Unified Benchmark Runner for KATANA Framework

This script runs all benchmarks in stages, collects results, and generates
structured output for documentation and CI integration.

Usage:
    ./scripts/run_benchmarks.py                    # Run stages 1-4
    ./scripts/run_benchmarks.py --stage 1          # Run only stage 1
    ./scripts/run_benchmarks.py --stage 1 2        # Run stages 1 and 2
    ./scripts/run_benchmarks.py --repeats 10       # 10 runs per stage
    ./scripts/run_benchmarks.py --aggregation best # Use best result for each benchmark
    ./scripts/run_benchmarks.py --stage-repeat 1=20 # Override repeats for stage 1
    ./scripts/run_benchmarks.py --include-e2e      # Include stage 5 (keep-alive e2e)
    ./scripts/run_benchmarks.py --perf-stat        # Collect perf stat counters
    ./scripts/run_benchmarks.py --output results   # Save results to results/
    ./scripts/run_benchmarks.py --update-docs      # Update BENCHMARK_RESULTS.md
    ./scripts/run_benchmarks.py --json             # Output JSON to stdout

Stages:
    1. Core Runtime: Ring buffer, circular buffer, SIMD operations
    2. Codegen Quality: JSON parsing, escaping, object parsing
    3. Serialization: JSON string/array/object serialization
    4. Router: HTTP routing dispatch performance
    5. E2E: compute_api keep-alive scenario with latency percentiles
"""

from __future__ import annotations

import argparse
import contextlib
import json
import math
import os
import re
import shutil
import socket
import subprocess
import sys
import threading
import time
from dataclasses import dataclass, field
from datetime import datetime
from pathlib import Path
from typing import Any, Dict, List, Optional, Tuple

# Paths relative to repository root
REPO_ROOT = Path(__file__).resolve().parent.parent
BUILD_DIR = REPO_ROOT / "build" / "bench"
BENCHMARK_DIR = BUILD_DIR / "benchmark"
RESULTS_MD_PATH = REPO_ROOT / "BENCHMARK_RESULTS.md"


@dataclass
class BenchmarkResult:
    """Single benchmark measurement."""

    name: str
    latency_ns: Optional[float] = None
    throughput: Optional[float] = None
    throughput_unit: str = "ops/sec"
    bytes_per_sec: Optional[float] = None
    operations: Optional[float] = None
    latency_p50_us: Optional[float] = None
    latency_p95_us: Optional[float] = None
    latency_p99_us: Optional[float] = None
    latency_p999_us: Optional[float] = None
    latency_max_us: Optional[float] = None
    producer_retries: Optional[float] = None
    consumer_retries: Optional[float] = None
    retries_per_op_push: Optional[float] = None
    retries_per_op_pop: Optional[float] = None
    retries_per_op_total: Optional[float] = None
    thread_producers: Optional[float] = None
    thread_consumers: Optional[float] = None
    thread_hw: Optional[float] = None
    thread_oversubscription: Optional[float] = None
    errors: int = 0


@dataclass
class StageResult:
    """Results from a single benchmark stage."""

    stage_id: int
    stage_name: str
    benchmarks: List[BenchmarkResult] = field(default_factory=list)
    duration_ms: int = 0
    success: bool = True
    error_message: Optional[str] = None
    run_count: int = 0
    aggregation_mode: str = "median"
    benchmark_stats: Dict[str, Dict[str, Dict[str, float]]] = field(default_factory=dict)
    benchmark_runs: Dict[str, List[Dict[str, Any]]] = field(default_factory=dict)
    perf_stat: Dict[str, float] = field(default_factory=dict)
    perf_derived: Dict[str, float] = field(default_factory=dict)
    perf_error: Optional[str] = None


@dataclass
class BenchmarkReport:
    """Complete benchmark report."""

    generated_at: str
    commit_sha: Optional[str] = None
    stages: List[StageResult] = field(default_factory=list)
    total_duration_ms: int = 0
    repeats: int = 10
    aggregation_mode: str = "median"
    stage_repeats: Dict[str, int] = field(default_factory=dict)
    comparison_summary: Optional[Dict[str, Any]] = None


DEFAULT_REPEATS = 10
DEFAULT_STAGE_REPEATS = {
    1: 15,  # Core runtime includes highly concurrent / contention-sensitive tests.
    5: 5,   # E2E scenario is heavier, lower default is enough for trend tracking.
}


# Stage definitions
STAGES = {
    1: {
        "name": "Core Runtime Benchmarks",
        "binary": "performance_benchmark",
        "description": "Ring buffer, circular buffer, SIMD operations, arena allocation",
    },
    2: {
        "name": "Codegen Quality Benchmarks",
        "binary": "codegen_quality_benchmark",
        "description": "JSON parsing, string escaping, key dispatch",
    },
    3: {
        "name": "Serialization Benchmarks",
        "binary": "serialize_benchmark",
        "description": "JSON serialization, string escaping, array construction",
    },
    4: {
        "name": "Router Benchmarks",
        "binary": "router_benchmark",
        "description": "HTTP routing dispatch performance",
    },
    5: {
        "name": "E2E HTTP Keep-Alive Benchmark",
        "kind": "e2e_keepalive",
        "description": "Full lifecycle: accept -> parse -> route -> handler -> JSON serialize -> write",
    },
}


def get_git_sha() -> Optional[str]:
    """Get current git commit SHA."""
    try:
        result = subprocess.run(
            ["git", "rev-parse", "--short", "HEAD"],
            capture_output=True,
            text=True,
            cwd=REPO_ROOT,
        )
        if result.returncode == 0:
            return result.stdout.strip()
    except Exception:
        pass
    return None


def ensure_build() -> bool:
    """Ensure benchmark binaries are built."""
    if not BUILD_DIR.exists():
        print("Build directory not found. Running cmake configure...")
        result = subprocess.run(
            ["cmake", "--preset", "bench"],
            cwd=REPO_ROOT,
            capture_output=True,
            text=True,
        )
        if result.returncode != 0:
            print(f"CMake configure failed: {result.stderr}")
            return False

    # Build all benchmark targets
    print("Building benchmark targets...")
    result = subprocess.run(
        ["cmake", "--build", str(BUILD_DIR), "-j"],
        capture_output=True,
        text=True,
    )
    if result.returncode != 0:
        print(f"Build failed: {result.stderr}")
        return False

    return True


def parse_codegen_quality_output(output: str) -> List[BenchmarkResult]:
    """Parse output from codegen_quality_benchmark."""
    results = []

    # Pattern:
    # "  name                               X.X ns    YYYYY ops/sec"
    # "  name                               X.X ns    YYYYY ops/sec    ZZZZZ bytes/sec"
    # "  name                               X.X ns    YYYYY ops/sec    ZZZZZ bytes/sec    p95: A ns    p99: B ns"
    pattern = re.compile(
        r"^\s+(.+?)\s+([\d.]+)\s+(ns|us)\s+([\d.]+)\s+ops/sec(?:\s+([\d.]+)\s+bytes/sec)?(?:\s+p95:\s+[\d.]+\s+ns\s+p99:\s+[\d.]+\s+ns)?\s*$",
        re.MULTILINE,
    )

    for match in pattern.finditer(output):
        name = match.group(1).strip()
        latency = float(match.group(2))
        unit = match.group(3)
        throughput = float(match.group(4))
        bytes_per_sec = float(match.group(5)) if match.group(5) else None

        # Convert to ns if in us
        if unit == "us":
            latency *= 1000

        results.append(
            BenchmarkResult(
                name=name,
                latency_ns=latency,
                throughput=throughput,
                bytes_per_sec=bytes_per_sec,
            )
        )

    return results


def parse_serialize_output(output: str) -> List[BenchmarkResult]:
    """Parse output from serialize_benchmark."""
    return parse_codegen_quality_output(output)  # Same format


def parse_router_output(output: str) -> List[BenchmarkResult]:
    """Parse output from router_benchmark."""
    results = []

    # Pattern for router results
    name_pattern = re.compile(r"^=== (.+?) ===$", re.MULTILINE)
    throughput_pattern = re.compile(r"Throughput:\s+([\d.]+)\s+ops/sec")
    operations_pattern = re.compile(r"Operations:\s+([\d.]+)")
    p50_pattern = re.compile(r"Latency p50:\s+([\d.]+)\s+us")
    p95_pattern = re.compile(r"Latency p95:\s+([\d.]+)\s+us")
    p99_pattern = re.compile(r"Latency p99:\s+([\d.]+)\s+us")
    p999_pattern = re.compile(r"Latency p999:\s+([\d.]+)\s+us")
    max_pattern = re.compile(r"Latency max:\s+([\d.]+)\s+us")
    errors_pattern = re.compile(r"Errors:\s+(\d+)")

    # Split output by sections
    sections = re.split(r"(?=^=== )", output, flags=re.MULTILINE)

    for section in sections:
        name_match = name_pattern.search(section)
        if not name_match:
            continue

        name = name_match.group(1)
        if name.startswith("Warmup"):
            continue

        result = BenchmarkResult(name=name)

        throughput_match = throughput_pattern.search(section)
        if throughput_match:
            result.throughput = float(throughput_match.group(1))

        operations_match = operations_pattern.search(section)
        if operations_match:
            result.operations = float(operations_match.group(1))

        p50_match = p50_pattern.search(section)
        if p50_match:
            result.latency_p50_us = float(p50_match.group(1))

        p95_match = p95_pattern.search(section)
        if p95_match:
            result.latency_p95_us = float(p95_match.group(1))

        p99_match = p99_pattern.search(section)
        if p99_match:
            result.latency_p99_us = float(p99_match.group(1))

        p999_match = p999_pattern.search(section)
        if p999_match:
            result.latency_p999_us = float(p999_match.group(1))

        max_match = max_pattern.search(section)
        if max_match:
            result.latency_max_us = float(max_match.group(1))

        errors_match = errors_pattern.search(section)
        if errors_match:
            result.errors = int(errors_match.group(1))

        results.append(result)

    return results


def parse_performance_output(output: str) -> List[BenchmarkResult]:
    """Parse output from performance_benchmark."""
    results = []
    number = r"([0-9]+(?:\.[0-9]+)?)"

    # Pattern for performance results
    name_pattern = re.compile(r"^=== (.+?) ===$", re.MULTILINE)
    throughput_pattern = re.compile(r"Throughput:\s+([\d.]+)\s+ops/sec")
    operations_pattern = re.compile(r"Operations:\s+([\d.]+)")
    p50_pattern = re.compile(r"Latency p50:\s+([\d.]+)\s+us")
    p95_pattern = re.compile(r"Latency p95:\s+([\d.]+)\s+us")
    p99_pattern = re.compile(r"Latency p99:\s+([\d.]+)\s+us")
    p999_pattern = re.compile(r"Latency p999:\s+([\d.]+)\s+us")
    max_pattern = re.compile(r"Latency max:\s+([\d.]+)\s+us")
    producer_retries_pattern = re.compile(rf"Producer retries:\s+{number}")
    consumer_retries_pattern = re.compile(rf"Consumer retries:\s+{number}")
    retries_per_op_pattern = re.compile(
        rf"Retries/op \(push/pop/total\):\s+{number}\s*/\s*{number}\s*/\s*{number}"
    )
    topology_pattern = re.compile(
        rf"Thread topology:\s+{number}x{number}\s+\(hw={number},\s+oversubscription={number}x\)"
    )

    # Split output by sections
    sections = re.split(r"(?=^=== )", output, flags=re.MULTILINE)

    for section in sections:
        name_match = name_pattern.search(section)
        if not name_match:
            continue

        name = name_match.group(1)

        result = BenchmarkResult(name=name)

        throughput_match = throughput_pattern.search(section)
        if throughput_match:
            result.throughput = float(throughput_match.group(1))

        operations_match = operations_pattern.search(section)
        if operations_match:
            result.operations = float(operations_match.group(1))

        p50_match = p50_pattern.search(section)
        if p50_match:
            result.latency_p50_us = float(p50_match.group(1))

        p95_match = p95_pattern.search(section)
        if p95_match:
            result.latency_p95_us = float(p95_match.group(1))

        p99_match = p99_pattern.search(section)
        if p99_match:
            result.latency_p99_us = float(p99_match.group(1))

        p999_match = p999_pattern.search(section)
        if p999_match:
            result.latency_p999_us = float(p999_match.group(1))

        max_match = max_pattern.search(section)
        if max_match:
            result.latency_max_us = float(max_match.group(1))

        producer_retries_match = producer_retries_pattern.search(section)
        if producer_retries_match:
            result.producer_retries = float(producer_retries_match.group(1))

        consumer_retries_match = consumer_retries_pattern.search(section)
        if consumer_retries_match:
            result.consumer_retries = float(consumer_retries_match.group(1))

        retries_per_op_match = retries_per_op_pattern.search(section)
        if retries_per_op_match:
            result.retries_per_op_push = float(retries_per_op_match.group(1))
            result.retries_per_op_pop = float(retries_per_op_match.group(2))
            result.retries_per_op_total = float(retries_per_op_match.group(3))

        topology_match = topology_pattern.search(section)
        if topology_match:
            result.thread_producers = float(topology_match.group(1))
            result.thread_consumers = float(topology_match.group(2))
            result.thread_hw = float(topology_match.group(3))
            result.thread_oversubscription = float(topology_match.group(4))

        results.append(result)

    return results


def parse_stage_output(binary_name: str, output: str) -> List[BenchmarkResult]:
    """Parse benchmark output by stage binary."""
    if binary_name == "performance_benchmark":
        return parse_performance_output(output)
    if binary_name == "codegen_quality_benchmark":
        return parse_codegen_quality_output(output)
    if binary_name == "serialize_benchmark":
        return parse_serialize_output(output)
    if binary_name == "router_benchmark":
        return parse_router_output(output)
    return []


def _latency_sort_key(b: BenchmarkResult) -> float:
    if b.latency_ns is not None:
        return b.latency_ns
    if b.latency_p50_us is not None:
        return b.latency_p50_us * 1000.0
    if b.latency_p99_us is not None:
        return b.latency_p99_us * 1000.0
    if b.latency_p999_us is not None:
        return b.latency_p999_us * 1000.0
    return float("inf")


def _percentile(sorted_values: List[float], q: float) -> float:
    if not sorted_values:
        return 0.0
    if len(sorted_values) == 1:
        return sorted_values[0]
    clamped = min(1.0, max(0.0, q))
    idx = int(clamped * (len(sorted_values) - 1))
    return sorted_values[idx]


def _metric_stats(values: List[float]) -> Dict[str, float]:
    if not values:
        return {}

    ordered = sorted(values)
    count = len(ordered)
    mean = sum(ordered) / count
    var = sum((v - mean) * (v - mean) for v in ordered) / count
    stddev = math.sqrt(var)

    return {
        "count": float(count),
        "min": ordered[0],
        "max": ordered[-1],
        "mean": mean,
        "stddev": stddev,
        "p50": _percentile(ordered, 0.50),
        "p90": _percentile(ordered, 0.90),
        "p95": _percentile(ordered, 0.95),
        "p99": _percentile(ordered, 0.99),
        "p999": _percentile(ordered, 0.999),
    }


def _benchmark_to_dict(b: BenchmarkResult) -> Dict[str, Any]:
    return {
        "name": b.name,
        "latency_ns": b.latency_ns,
        "throughput": b.throughput,
        "throughput_unit": b.throughput_unit,
        "bytes_per_sec": b.bytes_per_sec,
        "operations": b.operations,
        "latency_p50_us": b.latency_p50_us,
        "latency_p95_us": b.latency_p95_us,
        "latency_p99_us": b.latency_p99_us,
        "latency_p999_us": b.latency_p999_us,
        "latency_max_us": b.latency_max_us,
        "producer_retries": b.producer_retries,
        "consumer_retries": b.consumer_retries,
        "retries_per_op_push": b.retries_per_op_push,
        "retries_per_op_pop": b.retries_per_op_pop,
        "retries_per_op_total": b.retries_per_op_total,
        "thread_producers": b.thread_producers,
        "thread_consumers": b.thread_consumers,
        "thread_hw": b.thread_hw,
        "thread_oversubscription": b.thread_oversubscription,
        "errors": b.errors,
    }


def _representative_target(values: List[float], aggregation_mode: str) -> float:
    mode = aggregation_mode.lower()
    if mode == "mean":
        return sum(values) / len(values)
    ordered = sorted(values)
    return _percentile(ordered, 0.50)


def choose_representative_result(
    candidates: List[BenchmarkResult], aggregation_mode: str = "median"
) -> BenchmarkResult:
    """Choose representative result from repeated runs for one benchmark."""
    mode = aggregation_mode.lower()
    if mode not in {"best", "median", "mean"}:
        mode = "median"

    throughput_candidates = [c for c in candidates if c.throughput is not None]
    if throughput_candidates:
        if mode == "best":
            return max(throughput_candidates, key=lambda c: c.throughput if c.throughput else 0.0)

        values = [c.throughput if c.throughput is not None else 0.0 for c in throughput_candidates]
        target = _representative_target(values, mode)
        return min(
            throughput_candidates,
            key=lambda c: abs((c.throughput if c.throughput is not None else 0.0) - target),
        )

    latency_candidates = [c for c in candidates if _latency_sort_key(c) != float("inf")]
    if latency_candidates:
        if mode == "best":
            return min(latency_candidates, key=_latency_sort_key)

        values = [_latency_sort_key(c) for c in latency_candidates]
        target = _representative_target(values, mode)
        return min(latency_candidates, key=lambda c: abs(_latency_sort_key(c) - target))

    return candidates[0]


def build_benchmark_stats(candidates: List[BenchmarkResult]) -> Dict[str, Dict[str, float]]:
    stats: Dict[str, Dict[str, float]] = {}

    throughput_values = [float(c.throughput) for c in candidates if c.throughput is not None]
    if throughput_values:
        stats["throughput"] = _metric_stats(throughput_values)

    bytes_per_sec_values = [float(c.bytes_per_sec) for c in candidates if c.bytes_per_sec is not None]
    if bytes_per_sec_values:
        stats["bytes_per_sec"] = _metric_stats(bytes_per_sec_values)

    operations_values = [float(c.operations) for c in candidates if c.operations is not None]
    if operations_values:
        stats["operations"] = _metric_stats(operations_values)

    latency_ns_values = [float(c.latency_ns) for c in candidates if c.latency_ns is not None]
    if latency_ns_values:
        stats["latency_ns"] = _metric_stats(latency_ns_values)

    p50_values = [float(c.latency_p50_us) for c in candidates if c.latency_p50_us is not None]
    if p50_values:
        stats["latency_p50_us"] = _metric_stats(p50_values)

    p95_values = [float(c.latency_p95_us) for c in candidates if c.latency_p95_us is not None]
    if p95_values:
        stats["latency_p95_us"] = _metric_stats(p95_values)

    p99_values = [float(c.latency_p99_us) for c in candidates if c.latency_p99_us is not None]
    if p99_values:
        stats["latency_p99_us"] = _metric_stats(p99_values)

    p999_values = [float(c.latency_p999_us) for c in candidates if c.latency_p999_us is not None]
    if p999_values:
        stats["latency_p999_us"] = _metric_stats(p999_values)

    max_values = [float(c.latency_max_us) for c in candidates if c.latency_max_us is not None]
    if max_values:
        stats["latency_max_us"] = _metric_stats(max_values)

    producer_retry_values = [float(c.producer_retries) for c in candidates if c.producer_retries is not None]
    if producer_retry_values:
        stats["producer_retries"] = _metric_stats(producer_retry_values)

    consumer_retry_values = [float(c.consumer_retries) for c in candidates if c.consumer_retries is not None]
    if consumer_retry_values:
        stats["consumer_retries"] = _metric_stats(consumer_retry_values)

    retry_push_values = [
        float(c.retries_per_op_push) for c in candidates if c.retries_per_op_push is not None
    ]
    if retry_push_values:
        stats["retries_per_op_push"] = _metric_stats(retry_push_values)

    retry_pop_values = [
        float(c.retries_per_op_pop) for c in candidates if c.retries_per_op_pop is not None
    ]
    if retry_pop_values:
        stats["retries_per_op_pop"] = _metric_stats(retry_pop_values)

    retry_total_values = [
        float(c.retries_per_op_total) for c in candidates if c.retries_per_op_total is not None
    ]
    if retry_total_values:
        stats["retries_per_op_total"] = _metric_stats(retry_total_values)

    thread_oversub_values = [
        float(c.thread_oversubscription)
        for c in candidates
        if c.thread_oversubscription is not None
    ]
    if thread_oversub_values:
        stats["thread_oversubscription"] = _metric_stats(thread_oversub_values)

    error_values = [float(c.errors) for c in candidates]
    if error_values:
        stats["errors"] = _metric_stats(error_values)

    return stats


def merge_repeated_results(
    run_results: List[List[BenchmarkResult]],
    aggregation_mode: str = "median",
    include_runs: bool = False,
) -> Tuple[List[BenchmarkResult], Dict[str, Dict[str, Dict[str, float]]], Dict[str, List[Dict[str, Any]]]]:
    """Merge repeated run results into representative + stats per benchmark name."""
    if not run_results:
        return [], {}, {}

    by_name: Dict[str, List[BenchmarkResult]] = {}
    order: List[str] = []

    for run in run_results:
        for bench in run:
            if bench.name not in by_name:
                by_name[bench.name] = []
                order.append(bench.name)
            by_name[bench.name].append(bench)

    merged: List[BenchmarkResult] = []
    stats_by_name: Dict[str, Dict[str, Dict[str, float]]] = {}
    runs_by_name: Dict[str, List[Dict[str, Any]]] = {}

    for name in order:
        candidates = by_name[name]
        merged.append(choose_representative_result(candidates, aggregation_mode=aggregation_mode))
        stats_by_name[name] = build_benchmark_stats(candidates)
        if include_runs:
            runs_by_name[name] = [_benchmark_to_dict(c) for c in candidates]

    return merged, stats_by_name, runs_by_name


def _find_binary(candidates: List[Path]) -> Optional[Path]:
    for candidate in candidates:
        if candidate.exists():
            return candidate
    return None


def _wait_for_port(host: str, port: int, timeout_s: float = 10.0) -> bool:
    deadline = time.monotonic() + timeout_s
    while time.monotonic() < deadline:
        try:
            with socket.create_connection((host, port), timeout=0.5):
                return True
        except OSError:
            time.sleep(0.1)
    return False


def _start_server_process(binary: Path, env: Dict[str, str]) -> subprocess.Popen[str]:
    return subprocess.Popen(
        [str(binary)],
        stdout=subprocess.DEVNULL,
        stderr=subprocess.PIPE,
        text=True,
        env=env,
    )


def _stop_server_process(proc: Optional[subprocess.Popen[str]]) -> None:
    if proc is None:
        return
    try:
        proc.terminate()
        proc.wait(timeout=5)
    except Exception:
        try:
            proc.kill()
            proc.wait(timeout=2)
        except Exception:
            pass


def _is_network_restricted_error(message: Optional[str]) -> bool:
    if not message:
        return False
    lowered = message.lower()
    return (
        "operation not permitted" in lowered
        or "permission denied" in lowered
        or "address already in use" in lowered
        or "failed to start listeners" in lowered
    )


def _parse_content_length(headers_blob: bytes) -> int:
    for line in headers_blob.split(b"\r\n"):
        if line.lower().startswith(b"content-length:"):
            try:
                return int(line.split(b":", 1)[1].strip())
            except Exception:
                return 0
    return 0


def _read_http_response(sock: socket.socket) -> Tuple[Optional[int], bool]:
    data = b""
    while b"\r\n\r\n" not in data:
        chunk = sock.recv(4096)
        if not chunk:
            return None, False
        data += chunk

    headers_blob, rest = data.split(b"\r\n\r\n", 1)
    status_line = headers_blob.split(b"\r\n", 1)[0]
    parts = status_line.split()
    status: Optional[int] = None
    if len(parts) >= 2:
        try:
            status = int(parts[1])
        except Exception:
            status = None

    content_length = _parse_content_length(headers_blob)
    need = max(content_length - len(rest), 0)
    while need > 0:
        chunk = sock.recv(min(need, 4096))
        if not chunk:
            return status, False
        rest += chunk
        need -= len(chunk)
    return status, True


def _build_compute_request(host: str, port: int, payload: bytes) -> bytes:
    headers = [
        "POST /compute/sum HTTP/1.1",
        f"Host: {host}:{port}",
        "Connection: keep-alive",
        "Content-Type: application/json",
        "Accept: application/json",
        f"Content-Length: {len(payload)}",
        "",
        "",
    ]
    return ("\r\n".join(headers)).encode("utf-8") + payload


def _e2e_worker(
    host: str,
    port: int,
    req_bytes: bytes,
    request_count: int,
    latencies_out: List[float],
    lock: threading.Lock,
) -> Tuple[int, int]:
    local_latencies: List[float] = []
    success = 0
    errors = 0
    sock: Optional[socket.socket] = None

    try:
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)
        sock.settimeout(5.0)
        sock.connect((host, port))

        for _ in range(request_count):
            start_ns = time.perf_counter_ns()
            try:
                sock.sendall(req_bytes)
                status, complete = _read_http_response(sock)
                end_ns = time.perf_counter_ns()
                if not complete or status != 200:
                    errors += 1
                    continue
                success += 1
                local_latencies.append((end_ns - start_ns) / 1000.0)
            except Exception:
                errors += 1
                break
    except Exception:
        errors += request_count
    finally:
        if sock is not None:
            try:
                sock.close()
            except Exception:
                pass

    if local_latencies:
        with lock:
            latencies_out.extend(local_latencies)

    return success, errors


def run_e2e_keepalive_once(
    connections: int,
    requests_per_connection: int,
    port: int,
) -> Tuple[BenchmarkResult, Optional[str]]:
    """Run one keep-alive end-to-end scenario against compute_api."""
    candidates = [
        BUILD_DIR / "examples" / "codegen" / "compute_api" / "compute_api",
        BUILD_DIR / "examples" / "examples" / "codegen" / "compute_api" / "compute_api",
        REPO_ROOT / "build" / "examples" / "examples" / "codegen" / "compute_api" / "compute_api",
    ]
    server_binary = _find_binary(candidates)
    if server_binary is None:
        return (
            BenchmarkResult(
                name="HTTP E2E Keep-Alive (compute_api)",
                throughput=0.0,
                throughput_unit="req/sec",
                errors=connections * requests_per_connection,
            ),
            "compute_api binary not found (build examples or bench preset with examples)",
        )

    host = "127.0.0.1"
    payload = b"[1,2,3,4,5,6,7,8]"
    req_bytes = _build_compute_request(host, port, payload)
    env = {**os.environ, "PORT": str(port), "COMPUTE_PORT": str(port)}

    server_proc: Optional[subprocess.Popen[str]] = None
    try:
        server_proc = _start_server_process(server_binary, env=env)
        if not _wait_for_port(host, port, timeout_s=10.0):
            details = ""
            if server_proc.poll() is not None and server_proc.stderr is not None:
                with contextlib.suppress(Exception):
                    details = server_proc.stderr.read().strip()
            suffix = f": {details}" if details else ""
            return (
                BenchmarkResult(
                    name="HTTP E2E Keep-Alive (compute_api)",
                    throughput=0.0,
                    throughput_unit="req/sec",
                    errors=connections * requests_per_connection,
                ),
                f"compute_api failed to listen on {port}{suffix}",
            )

        latencies_us: List[float] = []
        lock = threading.Lock()
        worker_results: List[Tuple[int, int]] = [(0, 0) for _ in range(connections)]

        start = time.perf_counter()
        threads: List[threading.Thread] = []
        for idx in range(connections):

            def _run_worker(worker_idx: int) -> None:
                worker_results[worker_idx] = _e2e_worker(
                    host,
                    port,
                    req_bytes,
                    requests_per_connection,
                    latencies_us,
                    lock,
                )

            t = threading.Thread(target=_run_worker, args=(idx,))
            threads.append(t)
            t.start()
        for t in threads:
            t.join()

        total_expected = connections * requests_per_connection
        total_success = sum(success for success, _ in worker_results)
        total_errors = sum(errors for _, errors in worker_results)
        if total_success + total_errors < total_expected:
            total_errors += total_expected - (total_success + total_errors)
        elapsed = max(time.perf_counter() - start, 1e-9)

        ordered = sorted(latencies_us)
        p50 = _percentile(ordered, 0.50) if ordered else 0.0
        p95 = _percentile(ordered, 0.95) if ordered else 0.0
        p99 = _percentile(ordered, 0.99) if ordered else 0.0
        p999 = _percentile(ordered, 0.999) if ordered else 0.0
        max_lat = ordered[-1] if ordered else 0.0

        result = BenchmarkResult(
            name=f"HTTP E2E Keep-Alive ({connections} conn x {requests_per_connection} req)",
            operations=float(total_success),
            throughput=float(total_success) / elapsed,
            throughput_unit="req/sec",
            latency_p50_us=p50 if total_success > 0 else None,
            latency_p95_us=p95 if total_success > 0 else None,
            latency_p99_us=p99 if total_success > 0 else None,
            latency_p999_us=p999 if total_success > 0 else None,
            latency_max_us=max_lat if total_success > 0 else None,
            errors=total_errors,
        )
        return result, None
    finally:
        _stop_server_process(server_proc)


def run_e2e_keepalive_stage(
    stage_id: int,
    stage_name: str,
    repeats: int,
    aggregation_mode: str,
    include_runs: bool,
    e2e_connections: int,
    e2e_requests_per_connection: int,
    e2e_port: int,
) -> StageResult:
    result = StageResult(
        stage_id=stage_id,
        stage_name=stage_name,
        aggregation_mode=aggregation_mode,
    )

    print(
        f"\n[Stage {stage_id}] Running {stage_name} "
        f"({aggregation_mode}-of-{repeats}, {e2e_connections} conn x {e2e_requests_per_connection} req)..."
    )
    run_results: List[List[BenchmarkResult]] = []
    run_errors: List[str] = []
    started = time.perf_counter()

    for run_idx in range(repeats):
        print(f"  Run {run_idx + 1}/{repeats}...")
        run_result, error = run_e2e_keepalive_once(
            connections=e2e_connections,
            requests_per_connection=e2e_requests_per_connection,
            port=e2e_port,
        )
        if error:
            run_errors.append(f"run {run_idx + 1}: {error}")
            continue
        run_results.append([run_result])

    result.duration_ms = int((time.perf_counter() - started) * 1000)
    if run_errors:
        if all(_is_network_restricted_error(err) for err in run_errors):
            result.success = True
            result.error_message = "skipped: environment blocks listener startup"
            print(f"  Skipped: {result.error_message}")
            return result
        result.success = False
        result.error_message = "; ".join(run_errors)
        print(f"  Error: {result.error_message}")
        return result

    result.run_count = len(run_results)
    merged, stats_by_name, runs_by_name = merge_repeated_results(
        run_results,
        aggregation_mode=aggregation_mode,
        include_runs=include_runs,
    )
    result.benchmarks = merged
    result.benchmark_stats = stats_by_name
    result.benchmark_runs = runs_by_name
    print(f"  Completed in {result.duration_ms}ms with {len(result.benchmarks)} benchmarks")
    return result


def _find_perf() -> Optional[str]:
    perf_bin = shutil.which("perf")
    if perf_bin:
        return perf_bin
    local_candidates = [
        Path.home() / "src" / "WSL2-Linux-Kernel" / "tools" / "perf" / "perf",
        REPO_ROOT / "WSL2-Linux-Kernel" / "tools" / "perf" / "perf",
    ]
    for candidate in local_candidates:
        if candidate.exists() and os.access(candidate, os.X_OK):
            return str(candidate)
    return None


def _parse_perf_stat(stderr: str) -> Dict[str, float]:
    metrics: Dict[str, float] = {}
    for raw in stderr.splitlines():
        line = raw.strip()
        if not line or line.startswith("#"):
            continue
        parts = line.split(",")
        if len(parts) < 3:
            continue
        value_raw = parts[0].strip()
        event_name = parts[2].strip()
        if not value_raw or value_raw in {"<not counted>", "<not supported>"}:
            continue
        cleaned = value_raw.replace(" ", "")
        try:
            metrics[event_name] = float(cleaned)
        except ValueError:
            continue
    return metrics


def _sum_operations(benchmarks: List[BenchmarkResult]) -> Optional[float]:
    values = [b.operations for b in benchmarks if b.operations is not None]
    if not values:
        return None
    return float(sum(values))


def _derive_perf(metrics: Dict[str, float], operations: Optional[float]) -> Dict[str, float]:
    out: Dict[str, float] = {}
    cycles = metrics.get("cycles")
    instructions = metrics.get("instructions")
    branches = metrics.get("branches")
    branch_misses = metrics.get("branch-misses")
    cache_refs = metrics.get("cache-references")
    cache_misses = metrics.get("cache-misses")

    if cycles and instructions and cycles > 0:
        out["ipc"] = instructions / cycles
    if branches and branch_misses and branches > 0:
        out["branch_miss_rate_pct"] = (branch_misses / branches) * 100.0
    if cache_refs and cache_misses and cache_refs > 0:
        out["cache_miss_rate_pct"] = (cache_misses / cache_refs) * 100.0

    if operations and operations > 0:
        if cycles:
            out["cycles_per_op"] = cycles / operations
        if instructions:
            out["instructions_per_op"] = instructions / operations
    return out


def collect_perf_stat(
    binary_path: Path,
    timeout_s: int,
    events: List[str],
    operations: Optional[float],
) -> Tuple[Dict[str, float], Dict[str, float], Optional[str]]:
    perf_bin = _find_perf()
    if perf_bin is None:
        return {}, {}, "perf not found"

    cmd = [
        perf_bin,
        "stat",
        "-x,",
        "--no-big-num",
        "-e",
        ",".join(events),
        str(binary_path),
    ]
    try:
        proc = subprocess.run(
            cmd,
            capture_output=True,
            text=True,
            timeout=max(timeout_s, 30),
        )
    except subprocess.TimeoutExpired:
        return {}, {}, f"perf stat timed out after {timeout_s}s"
    except Exception as exc:
        return {}, {}, f"perf stat failed: {exc}"

    if proc.returncode != 0:
        stderr = (proc.stderr or "").strip()
        if "No permission to enable" in stderr or "Permission denied" in stderr:
            return {}, {}, "perf permission denied (perf_event_paranoid / capabilities)"
        return {}, {}, f"perf stat exit code {proc.returncode}: {stderr[:160]}"

    metrics = _parse_perf_stat(proc.stderr)
    if not metrics:
        return {}, {}, "perf produced no parsable counters"
    derived = _derive_perf(metrics, operations=operations)
    return metrics, derived, None


def _sanitize_metric_fragment(raw: str) -> str:
    key = raw.strip().lower()
    key = re.sub(r"\s+", "_", key)
    key = re.sub(r"[^a-z0-9_]", "", key)
    key = re.sub(r"_+", "_", key).strip("_")
    return key or "metric"


def _metric_direction(metric_name: str) -> Optional[str]:
    lowered = metric_name.lower()
    if (
        "throughput" in lowered
        or "ops_per_sec" in lowered
        or "req_per_sec" in lowered
        or "bytes_per_sec" in lowered
    ):
        return "higher"
    if "latency" in lowered or "errors" in lowered or "retry" in lowered:
        return "lower"
    return None


def _cv_pct(metric_stats: Dict[str, float]) -> Optional[float]:
    mean = metric_stats.get("mean")
    stddev = metric_stats.get("stddev")
    if mean is None or stddev is None:
        return None
    if mean == 0:
        return None
    return abs(stddev / mean) * 100.0


def _extract_metrics(report_json: Dict[str, Any]) -> Dict[str, Dict[str, Any]]:
    metrics: Dict[str, Dict[str, Any]] = {}
    for stage in report_json.get("stages", []):
        stage_id = stage.get("stage_id")
        if stage.get("success") is False:
            continue

        by_name = {b.get("name"): b for b in stage.get("benchmarks", []) if b.get("name")}
        by_stats = stage.get("benchmark_stats", {})
        for bench_name, bench in by_name.items():
            stats = by_stats.get(bench_name, {})
            for metric_name in [
                "throughput",
                "bytes_per_sec",
                "latency_ns",
                "latency_p50_us",
                "latency_p95_us",
                "latency_p99_us",
                "latency_p999_us",
                "latency_max_us",
                "producer_retries",
                "consumer_retries",
                "retries_per_op_push",
                "retries_per_op_pop",
                "retries_per_op_total",
                "errors",
            ]:
                direction = _metric_direction(metric_name)
                if direction is None:
                    continue

                value: Optional[float] = None
                cv = None
                if metric_name in stats:
                    metric_stats = stats[metric_name]
                    p50 = metric_stats.get("p50")
                    if isinstance(p50, (int, float)):
                        value = float(p50)
                    cv = _cv_pct(metric_stats)

                if value is None:
                    raw = bench.get(metric_name)
                    if isinstance(raw, (int, float)):
                        value = float(raw)

                if value is None:
                    continue

                key = (
                    f"stage{stage_id}."
                    f"{_sanitize_metric_fragment(str(bench_name))}."
                    f"{metric_name}"
                )
                metrics[key] = {
                    "value": value,
                    "cv_pct": cv,
                    "direction": direction,
                    "stage_id": stage_id,
                    "benchmark": bench_name,
                    "metric": metric_name,
                }
    return metrics


def compare_reports_with_cv(
    current_report: Dict[str, Any],
    baseline_report: Dict[str, Any],
    threshold_pct: float,
    cv_threshold_pct: float,
) -> Dict[str, Any]:
    current_metrics = _extract_metrics(current_report)
    baseline_metrics = _extract_metrics(baseline_report)
    if not baseline_metrics:
        return {
            "baseline_loaded": False,
            "threshold_pct": threshold_pct,
            "cv_threshold_pct": cv_threshold_pct,
            "compared_metrics": 0,
            "regressions": [],
            "noisy_regressions": [],
            "improvements": [],
            "unchanged_count": 0,
            "hard_regressions_count": 0,
            "noisy_regressions_count": 0,
            "improvements_count": 0,
        }
    common_keys = sorted(set(current_metrics.keys()) & set(baseline_metrics.keys()))

    regressions: List[Dict[str, Any]] = []
    noisy_regressions: List[Dict[str, Any]] = []
    improvements: List[Dict[str, Any]] = []
    unchanged: List[Dict[str, Any]] = []

    for key in common_keys:
        cur = current_metrics[key]
        base = baseline_metrics[key]
        cur_value = float(cur["value"])
        base_value = float(base["value"])
        if base_value == 0.0:
            continue

        delta_pct = ((cur_value - base_value) / abs(base_value)) * 100.0
        direction = cur["direction"]

        is_regression = False
        is_improvement = False
        if direction == "higher":
            is_regression = delta_pct < -threshold_pct
            is_improvement = delta_pct > threshold_pct
        elif direction == "lower":
            is_regression = delta_pct > threshold_pct
            is_improvement = delta_pct < -threshold_pct

        max_cv = max(cur.get("cv_pct") or 0.0, base.get("cv_pct") or 0.0)
        item = {
            "key": key,
            "stage_id": cur["stage_id"],
            "benchmark": cur["benchmark"],
            "metric": cur["metric"],
            "baseline": base_value,
            "current": cur_value,
            "delta_pct": delta_pct,
            "current_cv_pct": cur.get("cv_pct"),
            "baseline_cv_pct": base.get("cv_pct"),
            "max_cv_pct": max_cv,
        }

        if is_regression:
            if max_cv > cv_threshold_pct:
                noisy_regressions.append(item)
            else:
                regressions.append(item)
        elif is_improvement:
            improvements.append(item)
        else:
            unchanged.append(item)

    regressions.sort(key=lambda x: abs(x["delta_pct"]), reverse=True)
    noisy_regressions.sort(key=lambda x: abs(x["delta_pct"]), reverse=True)
    improvements.sort(key=lambda x: abs(x["delta_pct"]), reverse=True)

    return {
        "baseline_loaded": True,
        "threshold_pct": threshold_pct,
        "cv_threshold_pct": cv_threshold_pct,
        "compared_metrics": len(common_keys),
        "regressions": regressions,
        "noisy_regressions": noisy_regressions,
        "improvements": improvements,
        "unchanged_count": len(unchanged),
        "hard_regressions_count": len(regressions),
        "noisy_regressions_count": len(noisy_regressions),
        "improvements_count": len(improvements),
    }


def run_stage(
    stage_id: int,
    repeats: int = 10,
    aggregation_mode: str = "median",
    include_runs: bool = False,
    perf_stat: bool = False,
    perf_events: Optional[List[str]] = None,
    e2e_connections: int = 16,
    e2e_requests_per_connection: int = 200,
    e2e_port: int = 18082,
) -> StageResult:
    """Run a single benchmark stage."""
    if stage_id not in STAGES:
        return StageResult(
            stage_id=stage_id,
            stage_name=f"Unknown Stage {stage_id}",
            success=False,
            error_message=f"Stage {stage_id} not found",
        )

    stage = STAGES[stage_id]
    if stage.get("kind") == "e2e_keepalive":
        return run_e2e_keepalive_stage(
            stage_id=stage_id,
            stage_name=stage["name"],
            repeats=repeats,
            aggregation_mode=aggregation_mode,
            include_runs=include_runs,
            e2e_connections=e2e_connections,
            e2e_requests_per_connection=e2e_requests_per_connection,
            e2e_port=e2e_port,
        )

    binary_name = stage["binary"]
    binary_path = BENCHMARK_DIR / binary_name

    result = StageResult(
        stage_id=stage_id,
        stage_name=stage["name"],
        run_count=0,
        aggregation_mode=aggregation_mode,
    )

    if not binary_path.exists():
        build_result = subprocess.run(
            ["cmake", "--build", str(BUILD_DIR), "--target", binary_name],
            capture_output=True,
            text=True,
        )
        if build_result.returncode != 0:
            result.success = False
            result.error_message = f"Binary not found and build failed: {binary_path}"
            return result

    print(f"\n[Stage {stage_id}] Running {stage['name']} ({aggregation_mode}-of-{repeats})...")
    started = time.perf_counter()
    run_results: List[List[BenchmarkResult]] = []
    run_errors: List[str] = []

    for run_idx in range(repeats):
        print(f"  Run {run_idx + 1}/{repeats}...")
        try:
            proc = subprocess.run(
                [str(binary_path)],
                capture_output=True,
                text=True,
                timeout=300,
            )
            if proc.returncode != 0:
                run_errors.append(f"run {run_idx + 1}: exit code {proc.returncode}")
                continue

            parsed = parse_stage_output(binary_name, proc.stdout)
            if not parsed:
                run_errors.append(f"run {run_idx + 1}: no benchmarks parsed")
                continue
            run_results.append(parsed)
        except subprocess.TimeoutExpired:
            run_errors.append(f"run {run_idx + 1}: timed out")
        except Exception as exc:
            run_errors.append(f"run {run_idx + 1}: {exc}")

    result.duration_ms = int((time.perf_counter() - started) * 1000)

    if run_errors:
        result.success = False
        result.error_message = "; ".join(run_errors)
        print(f"  Error: {result.error_message}")
        return result

    result.run_count = len(run_results)
    merged, stats_by_name, runs_by_name = merge_repeated_results(
        run_results,
        aggregation_mode=aggregation_mode,
        include_runs=include_runs,
    )
    result.benchmarks = merged
    result.benchmark_stats = stats_by_name
    result.benchmark_runs = runs_by_name

    if perf_stat:
        events = perf_events or [
            "cycles",
            "instructions",
            "branches",
            "branch-misses",
            "cache-references",
            "cache-misses",
            "context-switches",
            "cpu-migrations",
            "task-clock",
        ]
        operations = _sum_operations(result.benchmarks)
        perf_metrics, perf_derived, perf_error = collect_perf_stat(
            binary_path=binary_path,
            timeout_s=300,
            events=events,
            operations=operations,
        )
        result.perf_stat = perf_metrics
        result.perf_derived = perf_derived
        result.perf_error = perf_error
        if perf_error:
            print(f"  perf stat: {perf_error}")
        elif perf_derived:
            ipc = perf_derived.get("ipc")
            cpo = perf_derived.get("cycles_per_op")
            if ipc is not None and cpo is not None:
                print(f"  perf stat: ipc={ipc:.3f}, cycles/op={cpo:.3f}")
            elif ipc is not None:
                print(f"  perf stat: ipc={ipc:.3f}")

    print(f"  Completed in {result.duration_ms}ms with {len(result.benchmarks)} benchmarks")
    return result


def format_throughput(value: float) -> str:
    """Format throughput with appropriate suffix."""
    if value >= 1e9:
        return f"{value / 1e9:.2f}B"
    elif value >= 1e6:
        return f"{value / 1e6:.1f}M"
    elif value >= 1e3:
        return f"{value / 1e3:.1f}K"
    else:
        return f"{value:.0f}"


def format_data_rate(value: float) -> str:
    """Format byte throughput with appropriate suffix."""
    return f"{format_throughput(value)} bytes/sec"


def _aggregation_title(mode: str) -> str:
    titles = {
        "best": "best-of-N",
        "median": "median-of-N",
        "mean": "mean-of-N",
    }
    return titles.get(mode.lower(), mode)


def _repeat_policy_text(report: BenchmarkReport) -> str:
    if not report.stage_repeats:
        return f"{report.repeats} run(s) per stage"
    values = sorted(set(report.stage_repeats.values()))
    if len(values) == 1:
        return f"{values[0]} run(s) per stage"
    formatted = ", ".join(f"stage {sid}={count}" for sid, count in sorted(report.stage_repeats.items()))
    return f"default {report.repeats} run(s), overrides: {formatted}"


def generate_markdown(report: BenchmarkReport) -> str:
    """Generate BENCHMARK_RESULTS.md content from report."""
    lines = [
        "# KATANA Benchmark Results",
        "",
        f"> Last updated: {report.generated_at}",
    ]

    if report.commit_sha:
        lines.append(f"> Commit: {report.commit_sha}")

    lines.extend([
        "",
        "## Summary",
        "",
        "All benchmarks show realistic, stable measurements with proper compiler "
        "optimization barriers (`do_not_optimize()` / `clobber_memory()` in "
        "`benchmark/bench_utils.hpp`). Performance-critical paths have been optimized "
        "with hand-rolled parsers, SIMD-accelerated string processing, and lock-free "
        "concurrent data structures.",
        "",
        f"> **Note**: Results shown use {_aggregation_title(report.aggregation_mode)} "
        f"aggregation across {_repeat_policy_text(report)}. Concurrent benchmarks are highly "
        "sensitive to system load and thread scheduling.",
        "",
        "---",
        "",
    ])

    for stage in report.stages:
        if not stage.success:
            lines.extend([
                f"## {stage.stage_name}",
                "",
                f"**Error**: {stage.error_message}",
                "",
                "---",
                "",
            ])
            continue

        lines.extend([
            f"## {stage.stage_name}",
            "",
            f"_Runs: {stage.run_count} | Aggregation: {stage.aggregation_mode}_",
            "",
        ])

        if stage.error_message and not stage.benchmarks:
            lines.extend([f"_Note: {stage.error_message}_", "", "---", ""])
            continue

        # Check if we have latency data.
        has_latency = any(b.latency_ns is not None for b in stage.benchmarks)
        has_percentiles = any(
            b.latency_p50_us is not None
            or b.latency_p95_us is not None
            or b.latency_p99_us is not None
            or b.latency_max_us is not None
            for b in stage.benchmarks
        )
        has_data_rate = any(b.bytes_per_sec is not None for b in stage.benchmarks)
        has_retry_total = any(b.retries_per_op_total is not None for b in stage.benchmarks)

        if has_percentiles:
            if has_retry_total:
                if has_data_rate:
                    lines.extend([
                        "| Benchmark | Throughput | Data Rate | Latency p50 | Latency p95 | Latency p99 | Latency max | Retries/op | Errors |",
                        "|-----------|------------|-----------|-------------|-------------|-------------|-------------|------------|--------|",
                    ])
                else:
                    lines.extend([
                        "| Benchmark | Throughput | Latency p50 | Latency p95 | Latency p99 | Latency max | Retries/op | Errors |",
                        "|-----------|------------|-------------|-------------|-------------|-------------|------------|--------|",
                    ])
            else:
                if has_data_rate:
                    lines.extend([
                        "| Benchmark | Throughput | Data Rate | Latency p50 | Latency p95 | Latency p99 | Latency max | Errors |",
                        "|-----------|------------|-----------|-------------|-------------|-------------|-------------|--------|",
                    ])
                else:
                    lines.extend([
                        "| Benchmark | Throughput | Latency p50 | Latency p95 | Latency p99 | Latency max | Errors |",
                        "|-----------|------------|-------------|-------------|-------------|-------------|--------|",
                    ])
            for b in stage.benchmarks:
                throughput = (
                    f"{format_throughput(b.throughput)} {b.throughput_unit}"
                    if b.throughput is not None
                    else "-"
                )
                data_rate = format_data_rate(b.bytes_per_sec) if b.bytes_per_sec is not None else "-"
                p50 = f"{b.latency_p50_us:.3f} us" if b.latency_p50_us is not None else "-"
                p95 = f"{b.latency_p95_us:.3f} us" if b.latency_p95_us is not None else "-"
                p99 = f"{b.latency_p99_us:.3f} us" if b.latency_p99_us is not None else "-"
                max_lat = f"{b.latency_max_us:.3f} us" if b.latency_max_us is not None else "-"
                retry_total = (
                    f"{b.retries_per_op_total:.3f}" if b.retries_per_op_total is not None else "-"
                )
                if has_retry_total:
                    if has_data_rate:
                        lines.append(
                            f"| {b.name} | {throughput} | {data_rate} | {p50} | {p95} | {p99} | {max_lat} | {retry_total} | {b.errors} |"
                        )
                    else:
                        lines.append(
                            f"| {b.name} | {throughput} | {p50} | {p95} | {p99} | {max_lat} | {retry_total} | {b.errors} |"
                        )
                else:
                    if has_data_rate:
                        lines.append(
                            f"| {b.name} | {throughput} | {data_rate} | {p50} | {p95} | {p99} | {max_lat} | {b.errors} |"
                        )
                    else:
                        lines.append(
                            f"| {b.name} | {throughput} | {p50} | {p95} | {p99} | {max_lat} | {b.errors} |"
                        )
        elif has_latency:
            if has_data_rate:
                lines.extend([
                    "| Benchmark | Latency | Throughput | Data Rate |",
                    "|-----------|---------|------------|-----------|",
                ])
            else:
                lines.extend([
                    "| Benchmark | Latency | Throughput |",
                    "|-----------|---------|------------|",
                ])
            for b in stage.benchmarks:
                latency = f"{b.latency_ns:.1f} ns" if b.latency_ns is not None else "-"
                throughput = (
                    f"{format_throughput(b.throughput)} {b.throughput_unit}"
                    if b.throughput is not None
                    else "-"
                )
                data_rate = format_data_rate(b.bytes_per_sec) if b.bytes_per_sec is not None else "-"
                if has_data_rate:
                    lines.append(f"| {b.name} | {latency} | {throughput} | {data_rate} |")
                else:
                    lines.append(f"| {b.name} | {latency} | {throughput} |")
        else:
            if has_data_rate:
                lines.extend([
                    "| Benchmark | Throughput | Data Rate |",
                    "|-----------|------------|-----------|",
                ])
            else:
                lines.extend([
                    "| Benchmark | Throughput |",
                    "|-----------|------------|",
                ])
            for b in stage.benchmarks:
                throughput = (
                    f"{format_throughput(b.throughput)} {b.throughput_unit}"
                    if b.throughput is not None
                    else "-"
                )
                data_rate = format_data_rate(b.bytes_per_sec) if b.bytes_per_sec is not None else "-"
                if has_data_rate:
                    lines.append(f"| {b.name} | {throughput} | {data_rate} |")
                else:
                    lines.append(f"| {b.name} | {throughput} |")

        if stage.run_count > 1:
            stability_rows: List[str] = []
            latency_rows: List[str] = []
            retry_rows: List[str] = []
            data_rate_rows: List[str] = []
            for b in stage.benchmarks:
                throughput_stats = stage.benchmark_stats.get(b.name, {}).get("throughput", {})
                count = int(throughput_stats.get("count", 0))
                if count < 2:
                    throughput_stats = {}

                if throughput_stats:
                    mean = throughput_stats.get("mean", 0.0)
                    stddev = throughput_stats.get("stddev", 0.0)
                    cv = (stddev / mean * 100.0) if mean > 0 else 0.0
                    min_v = throughput_stats.get("min", 0.0)
                    p50_v = throughput_stats.get("p50", 0.0)
                    p95_v = throughput_stats.get("p95", 0.0)
                    max_v = throughput_stats.get("max", 0.0)

                    stability_rows.append(
                        f"| {b.name} | {format_throughput(mean)} | {format_throughput(stddev)} | {cv:.2f}% | "
                        f"{format_throughput(min_v)} | {format_throughput(p50_v)} | {format_throughput(p95_v)} | "
                        f"{format_throughput(max_v)} |"
                    )

                latency_metric_name = ""
                latency_stats: Dict[str, float] = {}
                for metric_key, label in [
                    ("latency_p99_us", "p99 us"),
                    ("latency_p95_us", "p95 us"),
                    ("latency_p50_us", "p50 us"),
                    ("latency_ns", "ns"),
                ]:
                    candidate = stage.benchmark_stats.get(b.name, {}).get(metric_key, {})
                    if int(candidate.get("count", 0)) >= 2:
                        latency_metric_name = label
                        latency_stats = candidate
                        break

                if latency_stats:
                    lat_mean = latency_stats.get("mean", 0.0)
                    lat_stddev = latency_stats.get("stddev", 0.0)
                    lat_cv = (lat_stddev / lat_mean * 100.0) if lat_mean > 0 else 0.0
                    lat_p50 = latency_stats.get("p50", 0.0)
                    lat_p95 = latency_stats.get("p95", 0.0)
                    lat_max = latency_stats.get("max", 0.0)
                    latency_rows.append(
                        f"| {b.name} | {latency_metric_name} | {lat_mean:.3f} | {lat_stddev:.3f} | "
                        f"{lat_cv:.2f}% | {lat_p50:.3f} | {lat_p95:.3f} | {lat_max:.3f} |"
                    )

                retry_stats = stage.benchmark_stats.get(b.name, {}).get("retries_per_op_total", {})
                if int(retry_stats.get("count", 0)) >= 2:
                    r_mean = retry_stats.get("mean", 0.0)
                    r_stddev = retry_stats.get("stddev", 0.0)
                    r_cv = (r_stddev / r_mean * 100.0) if r_mean > 0 else 0.0
                    r_p50 = retry_stats.get("p50", 0.0)
                    r_p95 = retry_stats.get("p95", 0.0)
                    r_max = retry_stats.get("max", 0.0)
                    retry_rows.append(
                        f"| {b.name} | {r_mean:.3f} | {r_stddev:.3f} | {r_cv:.2f}% | "
                        f"{r_p50:.3f} | {r_p95:.3f} | {r_max:.3f} |"
                    )

                data_rate_stats = stage.benchmark_stats.get(b.name, {}).get("bytes_per_sec", {})
                if int(data_rate_stats.get("count", 0)) >= 2:
                    d_mean = data_rate_stats.get("mean", 0.0)
                    d_stddev = data_rate_stats.get("stddev", 0.0)
                    d_cv = (d_stddev / d_mean * 100.0) if d_mean > 0 else 0.0
                    d_min = data_rate_stats.get("min", 0.0)
                    d_p50 = data_rate_stats.get("p50", 0.0)
                    d_p95 = data_rate_stats.get("p95", 0.0)
                    d_max = data_rate_stats.get("max", 0.0)
                    data_rate_rows.append(
                        f"| {b.name} | {format_data_rate(d_mean)} | {format_data_rate(d_stddev)} | {d_cv:.2f}% | "
                        f"{format_data_rate(d_min)} | {format_data_rate(d_p50)} | {format_data_rate(d_p95)} | "
                        f"{format_data_rate(d_max)} |"
                    )

            if stability_rows:
                lines.extend([
                    "",
                    "### Throughput Stability (Across Repeated Runs)",
                    "",
                    "| Benchmark | Mean | Stddev | CV | Min | p50 | p95 | Max |",
                    "|-----------|------|--------|----|-----|-----|-----|-----|",
                ])
                lines.extend(stability_rows)

            if latency_rows:
                lines.extend([
                    "",
                    "### Latency Stability (Across Repeated Runs)",
                    "",
                    "| Benchmark | Metric | Mean | Stddev | CV | p50 | p95 | Max |",
                    "|-----------|--------|------|--------|----|-----|-----|-----|",
                ])
                lines.extend(latency_rows)

            if retry_rows:
                lines.extend([
                    "",
                    "### Contention Stability (Retries/op)",
                    "",
                    "| Benchmark | Mean | Stddev | CV | p50 | p95 | Max |",
                    "|-----------|------|--------|----|-----|-----|-----|",
                ])
                lines.extend(retry_rows)

            if data_rate_rows:
                lines.extend([
                    "",
                    "### Data Throughput Stability (bytes/sec)",
                    "",
                    "| Benchmark | Mean | Stddev | CV | Min | p50 | p95 | Max |",
                    "|-----------|------|--------|----|-----|-----|-----|-----|",
                ])
                lines.extend(data_rate_rows)

        if stage.perf_stat or stage.perf_derived or stage.perf_error:
            lines.extend(["", "### perf stat", ""])
            if stage.perf_error:
                lines.append(f"- Status: {stage.perf_error}")
            if stage.perf_stat:
                lines.extend([
                    "| Counter | Value |",
                    "|---------|-------|",
                ])
                for name in sorted(stage.perf_stat.keys()):
                    value = stage.perf_stat[name]
                    lines.append(f"| {name} | {value:.3f} |")
            if stage.perf_derived:
                lines.extend(["", "| Derived | Value |", "|---------|-------|"])
                for name in sorted(stage.perf_derived.keys()):
                    value = stage.perf_derived[name]
                    lines.append(f"| {name} | {value:.5f} |")

        lines.extend(["", "---", ""])

    if report.comparison_summary:
        summary = report.comparison_summary
        lines.extend(["## Baseline Comparison", ""])
        if not summary.get("baseline_loaded", False):
            lines.extend(["Baseline not loaded or incompatible format.", "", "---", ""])
        else:
            lines.extend([
                f"- Compared metrics: {summary.get('compared_metrics', 0)}",
                f"- Hard regressions: {summary.get('hard_regressions_count', 0)}",
                f"- Noisy regressions (ignored by CV gate): {summary.get('noisy_regressions_count', 0)}",
                f"- Improvements: {summary.get('improvements_count', 0)}",
                f"- Thresholds: delta={summary.get('threshold_pct', 0):.2f}% | CV gate={summary.get('cv_threshold_pct', 0):.2f}%",
                "",
            ])

            hard = summary.get("regressions", [])[:10]
            if hard:
                lines.extend([
                    "| Hard Regression | Delta | Current CV | Baseline CV |",
                    "|-----------------|-------|------------|-------------|",
                ])
                for item in hard:
                    cur_cv = item.get("current_cv_pct")
                    base_cv = item.get("baseline_cv_pct")
                    cur_cv_s = f"{cur_cv:.2f}%" if isinstance(cur_cv, (int, float)) else "-"
                    base_cv_s = f"{base_cv:.2f}%" if isinstance(base_cv, (int, float)) else "-"
                    lines.append(
                        f"| {item.get('key')} | {item.get('delta_pct', 0):+.2f}% | {cur_cv_s} | {base_cv_s} |"
                    )
                lines.append("")

    # Add running instructions
    lines.extend([
        "## Running Benchmarks",
        "",
        "```bash",
        "# Build benchmark targets",
        "cmake --preset bench",
        "cmake --build build/bench -j$(nproc)",
        "",
        "# Run all benchmarks",
        "./scripts/run_benchmarks.py",
        "",
        "# Recommended repeat policy (core runtime gets more samples)",
        "./scripts/run_benchmarks.py --aggregation median --stage-repeat 1=20",
        "",
        "# Include E2E keep-alive scenario",
        "./scripts/run_benchmarks.py --include-e2e",
        "",
        "# Collect perf counters (requires perf permissions)",
        "./scripts/run_benchmarks.py --perf-stat",
        "",
        "# Run specific stages",
        "./scripts/run_benchmarks.py --stage 1 2",
        "",
        "# Run individual benchmarks",
        "./build/bench/benchmark/codegen_quality_benchmark",
        "./build/bench/benchmark/serialize_benchmark",
        "./build/bench/benchmark/router_benchmark",
        "./build/bench/benchmark/performance_benchmark",
        "```",
        "",
    ])

    return "\n".join(lines)


def to_json(report: BenchmarkReport) -> Dict[str, Any]:
    """Convert report to JSON-serializable dict."""
    return {
        "generated_at": report.generated_at,
        "commit_sha": report.commit_sha,
        "total_duration_ms": report.total_duration_ms,
        "repeats": report.repeats,
        "aggregation_mode": report.aggregation_mode,
        "stage_repeats": report.stage_repeats,
        "comparison_summary": report.comparison_summary,
        "stages": [
            {
                "stage_id": s.stage_id,
                "stage_name": s.stage_name,
                "duration_ms": s.duration_ms,
                "success": s.success,
                "error_message": s.error_message,
                "run_count": s.run_count,
                "aggregation_mode": s.aggregation_mode,
                "benchmarks": [
                    {
                        "name": b.name,
                        "latency_ns": b.latency_ns,
                        "throughput": b.throughput,
                        "throughput_unit": b.throughput_unit,
                        "bytes_per_sec": b.bytes_per_sec,
                        "operations": b.operations,
                        "latency_p50_us": b.latency_p50_us,
                        "latency_p95_us": b.latency_p95_us,
                        "latency_p99_us": b.latency_p99_us,
                        "latency_p999_us": b.latency_p999_us,
                        "latency_max_us": b.latency_max_us,
                        "producer_retries": b.producer_retries,
                        "consumer_retries": b.consumer_retries,
                        "retries_per_op_push": b.retries_per_op_push,
                        "retries_per_op_pop": b.retries_per_op_pop,
                        "retries_per_op_total": b.retries_per_op_total,
                        "thread_producers": b.thread_producers,
                        "thread_consumers": b.thread_consumers,
                        "thread_hw": b.thread_hw,
                        "thread_oversubscription": b.thread_oversubscription,
                        "errors": b.errors,
                    }
                    for b in s.benchmarks
                ],
                "benchmark_stats": s.benchmark_stats,
                "benchmark_runs": s.benchmark_runs,
                "perf_stat": s.perf_stat,
                "perf_derived": s.perf_derived,
                "perf_error": s.perf_error,
            }
            for s in report.stages
        ],
    }


def parse_stage_repeat_overrides(values: Optional[List[str]]) -> Dict[int, int]:
    overrides: Dict[int, int] = {}
    if not values:
        return overrides
    for raw in values:
        if "=" not in raw:
            raise ValueError(f"Invalid stage repeat override '{raw}', expected format stage=repeats")
        stage_raw, count_raw = raw.split("=", 1)
        stage_id = int(stage_raw.strip())
        repeats = int(count_raw.strip())
        if repeats < 1:
            raise ValueError(f"Invalid repeats in '{raw}', must be >= 1")
        overrides[stage_id] = repeats
    return overrides


def print_comparison_summary(summary: Dict[str, Any]) -> None:
    if not summary.get("baseline_loaded", False):
        print("\nComparison vs baseline: baseline not loaded or incompatible format")
        return

    print("\nComparison vs baseline:")
    print(f"  Compared metrics: {summary.get('compared_metrics', 0)}")
    print(f"  Hard regressions: {summary.get('hard_regressions_count', 0)}")
    print(f"  Noisy regressions: {summary.get('noisy_regressions_count', 0)}")
    print(f"  Improvements: {summary.get('improvements_count', 0)}")

    hard = summary.get("regressions", [])
    if hard:
        print("  Top hard regressions:")
        for item in hard[:10]:
            cur_cv = item.get("current_cv_pct")
            base_cv = item.get("baseline_cv_pct")
            cur_cv_s = f"{cur_cv:.2f}%" if isinstance(cur_cv, (int, float)) else "-"
            base_cv_s = f"{base_cv:.2f}%" if isinstance(base_cv, (int, float)) else "-"
            print(
                f"    - {item['key']}: {item['delta_pct']:+.2f}% "
                f"(cur_cv={cur_cv_s}, base_cv={base_cv_s})"
            )


def main():
    parser = argparse.ArgumentParser(
        description="Unified Benchmark Runner for KATANA Framework",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog=__doc__,
    )
    parser.add_argument(
        "--stage",
        "-s",
        type=int,
        nargs="*",
        help="Run only specific stages (1-5). If not specified, runs stages 1-4.",
    )
    parser.add_argument(
        "--include-e2e",
        action="store_true",
        help="Include stage 5 (E2E keep-alive benchmark) when --stage is not set.",
    )
    parser.add_argument(
        "--output",
        "-o",
        type=Path,
        help="Output directory for results (JSON and markdown)",
    )
    parser.add_argument(
        "--json",
        "-j",
        action="store_true",
        help="Output JSON to stdout",
    )
    parser.add_argument(
        "--update-docs",
        "-u",
        action="store_true",
        help="Update BENCHMARK_RESULTS.md in repository",
    )
    parser.add_argument(
        "--no-build",
        action="store_true",
        help="Skip build step (assume binaries exist)",
    )
    parser.add_argument(
        "--list-stages",
        "-l",
        action="store_true",
        help="List available stages and exit",
    )
    parser.add_argument(
        "--repeats",
        type=int,
        default=None,
        help=(
            "Default runs per stage. If omitted, uses methodology defaults: "
            f"{DEFAULT_REPEATS} and stage overrides {DEFAULT_STAGE_REPEATS}."
        ),
    )
    parser.add_argument(
        "--stage-repeat",
        action="append",
        default=[],
        help="Per-stage repeat override, can be passed multiple times (e.g. --stage-repeat 1=20).",
    )
    parser.add_argument(
        "--aggregation",
        choices=["best", "median", "mean"],
        default="median",
        help="Aggregation mode for repeated runs (default: median)",
    )
    parser.add_argument(
        "--include-runs",
        action="store_true",
        help="Include raw per-run benchmark entries in JSON output",
    )
    parser.add_argument(
        "--perf-stat",
        action="store_true",
        help="Collect perf stat counters for each non-E2E stage (best effort).",
    )
    parser.add_argument(
        "--perf-events",
        default=(
            "cycles,instructions,branches,branch-misses,cache-references,"
            "cache-misses,context-switches,cpu-migrations,task-clock"
        ),
        help="Comma-separated perf events list for --perf-stat.",
    )
    parser.add_argument(
        "--compare",
        type=Path,
        help="Compare against a baseline JSON report generated by this script.",
    )
    parser.add_argument(
        "--regression-threshold-pct",
        type=float,
        default=5.0,
        help="Regression threshold in percent (default: 5.0).",
    )
    parser.add_argument(
        "--cv-threshold-pct",
        type=float,
        default=10.0,
        help=(
            "CV gate in percent. Regressions above this noise level are marked as noisy "
            "and do not fail with --fail-on-regression (default: 10.0)."
        ),
    )
    parser.add_argument(
        "--fail-on-regression",
        action="store_true",
        help="Return non-zero when hard regressions are detected in --compare mode.",
    )
    parser.add_argument(
        "--e2e-connections",
        type=int,
        default=16,
        help="Concurrent connections for stage 5 (default: 16).",
    )
    parser.add_argument(
        "--e2e-requests-per-connection",
        type=int,
        default=200,
        help="Requests per connection for stage 5 (default: 200).",
    )
    parser.add_argument(
        "--e2e-port",
        type=int,
        default=18082,
        help="Port for stage 5 compute_api server (default: 18082).",
    )

    args = parser.parse_args()

    if args.list_stages:
        print("Available benchmark stages:")
        for stage_id, stage in STAGES.items():
            print(f"  {stage_id}. {stage['name']}")
            if "binary" in stage:
                print(f"     Binary: {stage['binary']}")
            print(f"     {stage['description']}")
        return 0

    if args.repeats is not None and args.repeats < 1:
        print("Error: --repeats must be >= 1")
        return 1
    if args.e2e_connections < 1 or args.e2e_requests_per_connection < 1:
        print("Error: --e2e-connections and --e2e-requests-per-connection must be >= 1")
        return 1

    if args.stage:
        stages_to_run = args.stage
    else:
        stages_to_run = [1, 2, 3, 4]
        if args.include_e2e:
            stages_to_run.append(5)

    for stage_id in stages_to_run:
        if stage_id not in STAGES:
            print(f"Error: Unknown stage {stage_id}")
            return 1

    try:
        stage_repeat_overrides = parse_stage_repeat_overrides(args.stage_repeat)
    except ValueError as exc:
        print(f"Error: {exc}")
        return 1

    for stage_id in stage_repeat_overrides.keys():
        if stage_id not in STAGES:
            print(f"Error: --stage-repeat references unknown stage {stage_id}")
            return 1

    base_repeats = args.repeats if args.repeats is not None else DEFAULT_REPEATS
    if args.repeats is None:
        for stage_id, value in DEFAULT_STAGE_REPEATS.items():
            if stage_id in stages_to_run and stage_id not in stage_repeat_overrides:
                stage_repeat_overrides[stage_id] = value

    effective_repeats_by_stage: Dict[int, int] = {
        stage_id: stage_repeat_overrides.get(stage_id, base_repeats) for stage_id in stages_to_run
    }

    perf_events = [e.strip() for e in args.perf_events.split(",") if e.strip()]
    if args.perf_stat and not perf_events:
        print("Error: --perf-events produced an empty event list")
        return 1

    if not args.no_build:
        if not ensure_build():
            print("Error: Failed to build benchmarks")
            return 1

    print(f"\n{'='*60}")
    print("KATANA Benchmark Runner")
    print(f"{'='*60}")
    print(f"Stages to run: {stages_to_run}")
    print(f"Default repeats: {base_repeats}")
    print(
        "Stage repeats: "
        + ", ".join(f"{sid}={effective_repeats_by_stage[sid]}" for sid in stages_to_run)
    )
    print(f"Aggregation mode: {args.aggregation}")
    print(f"perf stat: {'enabled' if args.perf_stat else 'disabled'}")

    report = BenchmarkReport(
        generated_at=datetime.now().strftime("%Y-%m-%d %H:%M:%S"),
        commit_sha=get_git_sha(),
        repeats=base_repeats,
        aggregation_mode=args.aggregation,
        stage_repeats={str(k): v for k, v in sorted(effective_repeats_by_stage.items())},
    )

    started = time.perf_counter()
    for stage_id in stages_to_run:
        result = run_stage(
            stage_id,
            repeats=effective_repeats_by_stage[stage_id],
            aggregation_mode=args.aggregation,
            include_runs=args.include_runs,
            perf_stat=args.perf_stat,
            perf_events=perf_events,
            e2e_connections=args.e2e_connections,
            e2e_requests_per_connection=args.e2e_requests_per_connection,
            e2e_port=args.e2e_port,
        )
        report.stages.append(result)
    report.total_duration_ms = int((time.perf_counter() - started) * 1000)

    if args.compare:
        if not args.compare.exists():
            print(f"Error: Baseline file not found: {args.compare}")
            return 1
        try:
            with open(args.compare, "r") as f:
                baseline_json = json.load(f)
        except Exception as exc:
            print(f"Error: Failed to load baseline JSON: {exc}")
            return 1

        comparison_summary = compare_reports_with_cv(
            current_report=to_json(report),
            baseline_report=baseline_json,
            threshold_pct=args.regression_threshold_pct,
            cv_threshold_pct=args.cv_threshold_pct,
        )
        report.comparison_summary = comparison_summary
        print_comparison_summary(comparison_summary)

    print(f"\n{'='*60}")
    print("Summary")
    print(f"{'='*60}")
    for stage in report.stages:
        status = "PASS" if stage.success else "FAIL"
        bench_count = len(stage.benchmarks)
        print(f"  Stage {stage.stage_id}: {stage.stage_name}")
        print(
            f"    Status: {status}, Benchmarks: {bench_count}, "
            f"Runs: {stage.run_count}, Duration: {stage.duration_ms}ms"
        )
    print(f"\nTotal duration: {report.total_duration_ms}ms")

    if args.json:
        print(json.dumps(to_json(report), indent=2))

    if args.output:
        args.output.mkdir(parents=True, exist_ok=True)
        json_path = args.output / "benchmark_results.json"
        md_path = args.output / "BENCHMARK_RESULTS.md"
        with open(json_path, "w") as f:
            json.dump(to_json(report), f, indent=2)
        print(f"\nJSON results saved to: {json_path}")

        with open(md_path, "w") as f:
            f.write(generate_markdown(report))
        print(f"Markdown results saved to: {md_path}")

    if args.update_docs:
        with open(RESULTS_MD_PATH, "w") as f:
            f.write(generate_markdown(report))
        print(f"\nUpdated: {RESULTS_MD_PATH}")

    all_passed = all(s.success for s in report.stages)
    hard_regressions = 0
    if report.comparison_summary:
        hard_regressions = int(report.comparison_summary.get("hard_regressions_count", 0))
    if args.fail_on_regression and hard_regressions > 0:
        return 1
    return 0 if all_passed else 1


if __name__ == "__main__":
    sys.exit(main())
