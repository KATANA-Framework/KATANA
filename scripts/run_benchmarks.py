#!/usr/bin/env python3
"""
Unified Benchmark Runner for KATANA Framework

This script runs all benchmarks in stages, collects results, and generates
structured output for documentation and CI integration.

Usage:
    ./scripts/run_benchmarks.py                    # Run microbenchmark stages 1-4,6-8,13
    ./scripts/run_benchmarks.py --stage 1          # Run only stage 1
    ./scripts/run_benchmarks.py --stage 1 2        # Run stages 1 and 2
    ./scripts/run_benchmarks.py --repeats 10       # 10 runs per stage
    ./scripts/run_benchmarks.py --aggregation best # Use best result for each benchmark
    ./scripts/run_benchmarks.py --stage-repeat 1=20 # Override repeats for stage 1
    ./scripts/run_benchmarks.py --include-e2e      # Include wrk-based E2E stages 9-12
    ./scripts/run_benchmarks.py --perf-stat        # Collect perf stat counters
    ./scripts/run_benchmarks.py --output results   # Save results to results/
    ./scripts/run_benchmarks.py --update-docs      # Update BENCHMARK_RESULTS.md
    ./scripts/run_benchmarks.py --json             # Output JSON to stdout

Stages:
    1. Core Runtime: Ring buffer, circular buffer, SIMD operations
    2. Codegen Quality: JSON parsing, escaping, object parsing
    3. Serialization: JSON string/array/object serialization
    4. Router: HTTP routing dispatch performance
    5. Legacy E2E: compute_api keep-alive scenario with latency percentiles
    6. Generated API Dispatch Benchmarks
    7. Generated API Mixed Workload Benchmarks
    8. Generated API Framework Path Benchmarks
    9-12. wrk-based E2E operating points (canonical + peak, hello + compute_api)
    13. SQL codegen parsing and generation benchmarks
    14. Generated SQL repository/runtime microbenchmarks
    15. PostgreSQL-backed SQL repository benchmarks
    16. Concurrent PostgreSQL repository throughput benchmarks
    17. wrk benchmark_api SQL read-heavy HTTP benchmark
    18. wrk benchmark_api SQL mixed CRUD HTTP benchmark
"""

from __future__ import annotations

import argparse
import contextlib
import json
import math
import os
import platform
import re
import shlex
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
RESULTS_MD_PATH = REPO_ROOT / "BENCHMARK_RESULTS.md"


def _is_wsl() -> bool:
    release = platform.release().lower()
    version = platform.version().lower()
    return "microsoft" in release or "microsoft" in version


def _discover_default_build_dir() -> Path:
    env_override = os.environ.get("KATANA_BENCH_BUILD_DIR")
    if env_override:
        return Path(env_override).expanduser().resolve()

    build_root = REPO_ROOT / "build"
    preferred_names = ["bench-local"]
    if _is_wsl():
        preferred_names.extend(["bench-wsl", "bench"])
    else:
        preferred_names.extend(["bench", "bench-wsl"])

    discovered: List[Path] = []
    if build_root.exists():
        discovered.extend(
            path for path in build_root.iterdir() if path.is_dir() and (path / "CMakeCache.txt").exists()
        )
    discovered.extend(
        path for path in REPO_ROOT.iterdir() if path.is_dir() and path.name.startswith("build") and (path / "CMakeCache.txt").exists()
    )
    if discovered:
        unique = {path.resolve() for path in discovered}

        def build_dir_rank(path: Path) -> tuple[int, float, str]:
            try:
                preferred_rank = preferred_names.index(path.name)
            except ValueError:
                preferred_rank = len(preferred_names)
            cache_path = path / "CMakeCache.txt"
            try:
                mtime = cache_path.stat().st_mtime
            except OSError:
                mtime = 0.0
            return (preferred_rank, -mtime, str(path))

        return min(unique, key=build_dir_rank)

    return build_root / ("bench-wsl" if _is_wsl() else "bench")


BUILD_DIR = _discover_default_build_dir()
BENCHMARK_DIR = BUILD_DIR / "benchmark"


@dataclass
class BenchmarkResult:
    """Single benchmark measurement."""

    name: str
    latency_ns: Optional[float] = None
    throughput: Optional[float] = None
    throughput_unit: str = "ops/sec"
    bytes_per_sec: Optional[float] = None
    operations: Optional[float] = None
    avg_latency_us: Optional[float] = None
    latency_p50_us: Optional[float] = None
    latency_p95_us: Optional[float] = None
    latency_p99_us: Optional[float] = None
    latency_p999_us: Optional[float] = None
    latency_max_us: Optional[float] = None
    tail_p95_ns: Optional[float] = None
    tail_p99_ns: Optional[float] = None
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
    description: Optional[str] = None
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
    config: Dict[str, Any] = field(default_factory=dict)


@dataclass
class WrkHttpInvocation:
    wrk_binary: str
    server_binary: Path
    port: int
    bench_workers: int
    env: Dict[str, str]
    wrk_env: Dict[str, str]
    script_path: Path
    benchmark_name: str
    server_target: str
    wrk_threads: int
    wrk_connections: int
    wrk_duration_sec: int
    wrk_warmup_sec: int
    wrk_url: str
    cleanup_sql: List[str] = field(default_factory=list)


@dataclass
class BenchmarkReport:
    """Complete benchmark report."""

    generated_at: str
    commit_sha: Optional[str] = None
    stages: List[StageResult] = field(default_factory=list)
    total_duration_ms: int = 0
    repeats: int = 10
    aggregation_mode: str = "median"
    stage_repeats: Dict[int, int] = field(default_factory=dict)
    environment: Dict[str, Any] = field(default_factory=dict)
    quality_summary: Optional[Dict[str, Any]] = None
    budget_summary: Optional[Dict[str, Any]] = None
    comparison_summary: Optional[Dict[str, Any]] = None


DEFAULT_REPEATS = 10
DEFAULT_STAGE_REPEATS = {
    1: 15,  # Core runtime includes highly concurrent / contention-sensitive tests.
    5: 5,   # E2E scenario is heavier, lower default is enough for trend tracking.
    9: 5,   # wrk-based network load tests are heavier than in-process microbenches.
    10: 5,
    11: 5,
    12: 5,
    16: 3,  # Concurrent DB throughput runs are heavier than in-process microbenches.
    17: 3,  # SQL HTTP wrk stages need repeats for usable latency/throughput medians.
    18: 3,
}


def default_sql_http_workers() -> int:
    env_override = os.environ.get("KATANA_SQL_HTTP_WORKERS")
    if env_override:
        with contextlib.suppress(ValueError):
            parsed = int(env_override)
            if parsed > 0:
                return parsed

    return min(max(1, os.cpu_count() or 1), 12)


def default_sql_http_peak_workers() -> int:
    env_override = os.environ.get("KATANA_SQL_HTTP_PEAK_WORKERS")
    if env_override:
        with contextlib.suppress(ValueError):
            parsed = int(env_override)
            if parsed > 0:
                return parsed

    logical = default_sql_http_workers()
    if logical >= 8:
        return min(logical + logical // 3, 16)
    return logical


def default_sql_bench_threads() -> int:
    env_override = os.environ.get("KATANA_SQL_BENCH_THREADS")
    if env_override:
        with contextlib.suppress(ValueError):
            parsed = int(env_override)
            if parsed > 0:
                return parsed

    return min(max(1, os.cpu_count() or 1), 12)


STAGE4_SQL_BUDGETS: Dict[int, Dict[str, Dict[str, Dict[str, float]]]] = {
    15: {
        "postgres repo get_user (1 row)": {
            "throughput": {"min": 12000.0},
            "latency_ns": {"max": 85000.0},
            "tail_p99_ns": {"max": 140000.0},
            "errors": {"max": 0.0},
        },
        "postgres repo list_users (128 rows)": {
            "throughput": {"min": 9000.0},
            "latency_ns": {"max": 120000.0},
            "tail_p99_ns": {"max": 170000.0},
            "errors": {"max": 0.0},
        },
        "postgres repo touch_user (exec)": {
            "throughput": {"min": 10500.0},
            "latency_ns": {"max": 100000.0},
            "tail_p99_ns": {"max": 120000.0},
            "errors": {"max": 0.0},
        },
        "postgres tx begin+touch+rollback": {
            "throughput": {"min": 5000.0},
            "latency_ns": {"max": 200000.0},
            "tail_p99_ns": {"max": 240000.0},
            "errors": {"max": 0.0},
        },
    },
    16: {
        "postgres concurrent get_user": {
            "throughput": {"min": 40000.0},
            "latency_ns": {"max": 26000.0},
            "errors": {"max": 0.0},
        },
        "postgres concurrent list_users": {
            "throughput": {"min": 27500.0},
            "latency_ns": {"max": 37000.0},
            "errors": {"max": 0.0},
        },
        "postgres concurrent touch_user": {
            "throughput": {"min": 2000.0},
            "latency_ns": {"max": 500000.0},
            "errors": {"max": 0.0},
        },
        "postgres concurrent touch_user tx32": {
            "throughput": {"min": 28000.0},
            "latency_ns": {"max": 40000.0},
            "errors": {"max": 0.0},
        },
        "postgres concurrent mixed workload": {
            "throughput": {"min": 9000.0},
            "latency_ns": {"max": 120000.0},
            "errors": {"max": 0.0},
        },
    },
    17: {
        "wrk benchmark_api SQL read-heavy": {
            "throughput": {"min": 24000.0},
            "latency_p50_us": {"max": 11000.0},
            "latency_p95_us": {"max": 19000.0},
            "latency_p99_us": {"max": 26000.0},
            "errors": {"max": 0.0},
        },
    },
    18: {
        "wrk benchmark_api SQL mixed CRUD": {
            "throughput": {"min": 14500.0},
            "latency_p50_us": {"max": 19000.0},
            "latency_p95_us": {"max": 32000.0},
            "latency_p99_us": {"max": 45000.0},
            "errors": {"max": 0.0},
        },
    },
}

STAGE4_SQL_BUDGET_NOTES = [
    "Stage 15-18 budgets are Stage 4 local sanity gates for generated SQL/runtime plus SQL HTTP CRUD.",
    "Use the runner default repeat policy for budget decisions; single-run wrk samples are too noisy for hard conclusions.",
    "HTTP budgets target the blocking libpq path as implemented today; they are not async-DB targets.",
    "Durable write paths remain commit/WAL-flush bound: compare touch_user vs touch_user tx32 before blaming codegen/runtime.",
    "Reference BENCHMARK_RESULTS.md numbers should be collected with cpu_governor=performance; local powersave runs are expected to be slower.",
]


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
        "name": "Legacy E2E HTTP Keep-Alive Benchmark",
        "kind": "e2e_keepalive",
        "description": "Legacy Python keep-alive scenario for compute_api; kept for compatibility only",
    },
    6: {
        "name": "Generated API Dispatch Benchmarks",
        "binary": "generated_api_benchmark",
        "description": "Generated router bindings, param extraction, parse and serialize on ready request objects",
    },
    7: {
        "name": "Generated API Mixed Workload Benchmarks",
        "binary": "benchmark_api_codegen_benchmark",
        "description": "Generated stack mixed valid/error workload on benchmark_api spec",
    },
    8: {
        "name": "Generated API Framework Path Benchmarks",
        "binary": "benchmark_api_framework_benchmark",
        "description": "Generated stack valid/error workload on benchmark_api spec with minimal handlers",
    },
    9: {
        "name": "HTTP Load Benchmark (hello_world_server canonical pipeline via wrk)",
        "kind": "wrk_http",
        "description": "Canonical low-latency pipelined GET profile for hello_world_server",
        "profile": "canonical",
        "benchmark_name": "wrk hello_world GET / depth10",
        "server_target": "hello_world_server",
        "server_candidates": [
            "hello_world_server",
            "examples/hello_world_server",
        ],
        "server_env": {"HELLO_PORT": "{port}", "KATANA_WORKERS": "{bench_workers}"},
        "bench_workers": 4,
        "port": 18080,
        "wrk_threads": 4,
        "wrk_connections": 512,
        "wrk_duration_sec": 10,
        "wrk_env": {"KATANA_PIPELINE_DEPTH": "10"},
        "wrk_script": "test/load/scripts/hello_pipeline.lua",
        "wrk_url": "http://127.0.0.1:{port}/",
    },
    10: {
        "name": "HTTP Load Benchmark (compute_api canonical pipeline via wrk)",
        "kind": "wrk_http",
        "description": "Canonical low-latency pipelined POST profile for compute_api",
        "profile": "canonical",
        "benchmark_name": "wrk compute_api POST /compute/sum depth10",
        "server_target": "compute_api",
        "server_candidates": [
            "examples/codegen/compute_api/compute_api",
            "examples/examples/codegen/compute_api/compute_api",
        ],
        "server_env": {"PORT": "{port}", "COMPUTE_PORT": "{port}", "KATANA_WORKERS": "{bench_workers}"},
        "bench_workers": 4,
        "port": 18081,
        "wrk_threads": 4,
        "wrk_connections": 512,
        "wrk_duration_sec": 10,
        "wrk_env": {"KATANA_PIPELINE_DEPTH": "10"},
        "wrk_script": "test/load/scripts/compute_sum_pipeline.lua",
        "wrk_url": "http://127.0.0.1:{port}/",
    },
    11: {
        "name": "HTTP Load Benchmark (hello_world_server peak pipeline via wrk)",
        "kind": "wrk_http",
        "description": "Peak-throughput pipelined GET profile for hello_world_server",
        "profile": "peak",
        "benchmark_name": "wrk hello_world GET / depth20",
        "server_target": "hello_world_server",
        "server_candidates": [
            "hello_world_server",
            "examples/hello_world_server",
        ],
        "server_env": {"HELLO_PORT": "{port}", "KATANA_WORKERS": "{bench_workers}"},
        "bench_workers": 4,
        "port": 18080,
        "wrk_threads": 4,
        "wrk_connections": 512,
        "wrk_duration_sec": 5,
        "wrk_env": {"KATANA_PIPELINE_DEPTH": "20"},
        "wrk_script": "test/load/scripts/hello_pipeline.lua",
        "wrk_url": "http://127.0.0.1:{port}/",
    },
    12: {
        "name": "HTTP Load Benchmark (compute_api peak pipeline via wrk)",
        "kind": "wrk_http",
        "description": "Peak-throughput pipelined POST profile for compute_api",
        "profile": "peak",
        "benchmark_name": "wrk compute_api POST /compute/sum depth40",
        "server_target": "compute_api",
        "server_candidates": [
            "examples/codegen/compute_api/compute_api",
            "examples/examples/codegen/compute_api/compute_api",
        ],
        "server_env": {"PORT": "{port}", "COMPUTE_PORT": "{port}", "KATANA_WORKERS": "{bench_workers}"},
        "bench_workers": 4,
        "port": 18081,
        "wrk_threads": 4,
        "wrk_connections": 512,
        "wrk_duration_sec": 5,
        "wrk_env": {"KATANA_PIPELINE_DEPTH": "40"},
        "wrk_script": "test/load/scripts/compute_sum_pipeline.lua",
        "wrk_url": "http://127.0.0.1:{port}/",
    },
    13: {
        "name": "SQL Codegen Benchmarks",
        "binary": "sql_codegen_benchmark",
        "description": "SQL catalog parsing plus generated model/repository emission cost",
    },
    14: {
        "name": "SQL Runtime Benchmarks",
        "binary": "sql_runtime_benchmark",
        "description": "Generated SQL repository overhead for point lookup, list mapping, and exec dispatch",
    },
    15: {
        "name": "SQL PostgreSQL Benchmarks",
        "binary": "sql_postgres_benchmark",
        "description": "PostgreSQL-backed generated repository benchmarks over live libpq/prepared statements",
    },
    16: {
        "name": "SQL PostgreSQL Throughput Benchmarks",
        "binary": "sql_postgres_throughput_benchmark",
        "description": "Concurrent PostgreSQL repository throughput over a multi-connection pool",
        "env": {
            "KATANA_SQL_BENCH_THREADS": str(default_sql_bench_threads()),
        },
    },
    17: {
        "name": "HTTP Load Benchmark (benchmark_api SQL read-heavy via wrk)",
        "kind": "wrk_http",
        "description": "Read-heavy generated HTTP path over benchmark_api SQL demo service",
        "profile": "canonical",
        "benchmark_name": "wrk benchmark_api SQL read-heavy",
        "server_target": "benchmark_api_sql_server",
        "server_candidates": [
            "benchmark/benchmark_api_sql_server",
        ],
        "server_env": {
            "PORT": "{port}",
            "KATANA_BENCHMARK_API_BOOTSTRAP": "1",
            "KATANA_BENCHMARK_API_RESET": "1",
            "KATANA_BENCHMARK_API_DISABLE_AUTOVACUUM": "1",
            "KATANA_BENCHMARK_API_SEED_COUNT": "4096",
            "KATANA_BENCHMARK_API_WORKERS": "{bench_workers}",
            "KATANA_BENCHMARK_API_EXECUTORS": "{bench_workers}",
        },
        "cleanup_sql": [
            "ALTER TABLE katana_stage4_items RESET (autovacuum_enabled, toast.autovacuum_enabled)",
        ],
        "bench_workers": default_sql_http_workers(),
        "port": 18086,
        "wrk_threads": 4,
        "wrk_connections": 256,
        "wrk_duration_sec": 10,
        "wrk_warmup_sec": 2,
        "wrk_env": {"KATANA_BENCHMARK_API_ITEM_COUNT": "4096"},
        "wrk_script": "test/load/scripts/benchmark_api_read_heavy.lua",
        "wrk_url": "http://127.0.0.1:{port}/items/1",
    },
    18: {
        "name": "HTTP Load Benchmark (benchmark_api SQL mixed CRUD via wrk)",
        "kind": "wrk_http",
        "description": "Mixed read/write generated HTTP path over benchmark_api SQL demo service",
        "profile": "peak",
        "benchmark_name": "wrk benchmark_api SQL mixed CRUD",
        "server_target": "benchmark_api_sql_server",
        "server_candidates": [
            "benchmark/benchmark_api_sql_server",
        ],
        "server_env": {
            "PORT": "{port}",
            "KATANA_BENCHMARK_API_BOOTSTRAP": "1",
            "KATANA_BENCHMARK_API_RESET": "1",
            "KATANA_BENCHMARK_API_DISABLE_AUTOVACUUM": "1",
            "KATANA_BENCHMARK_API_SEED_COUNT": "4096",
            "KATANA_BENCHMARK_API_WORKERS": "{bench_workers}",
            "KATANA_BENCHMARK_API_EXECUTORS": "{bench_workers}",
        },
        "cleanup_sql": [
            "ALTER TABLE katana_stage4_items RESET (autovacuum_enabled, toast.autovacuum_enabled)",
        ],
        "bench_workers": default_sql_http_peak_workers(),
        "port": 18087,
        "wrk_threads": 4,
        "wrk_connections": 256,
        "wrk_duration_sec": 10,
        "wrk_warmup_sec": 2,
        "wrk_env": {"KATANA_BENCHMARK_API_ITEM_COUNT": "4096"},
        "wrk_script": "test/load/scripts/benchmark_api_mixed_crud.lua",
        "wrk_url": "http://127.0.0.1:{port}/items/1",
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


def _safe_read_text(path: Path) -> Optional[str]:
    try:
        return path.read_text(encoding="utf-8").strip()
    except Exception:
        return None


def _run_capture(cmd: List[str], timeout: int = 3) -> Optional[str]:
    try:
        proc = subprocess.run(cmd, capture_output=True, text=True, timeout=timeout)
        if proc.returncode != 0:
            return None
        out = (proc.stdout or "").strip()
        return out or None
    except Exception:
        return None


def _cpu_governor_paths() -> List[Path]:
    root = Path("/sys/devices/system/cpu")
    return sorted(root.glob("cpu[0-9]*/cpufreq/scaling_governor"))


def _read_current_cpu_governor() -> Optional[str]:
    for path in _cpu_governor_paths():
        value = _safe_read_text(path)
        if value:
            return value
    return None


def _set_cpu_governor_via_sysfs(target: str, paths: List[Path]) -> Tuple[bool, Optional[str]]:
    try:
        for path in paths:
            path.write_text(f"{target}\n", encoding="utf-8")
        return True, None
    except Exception as exc:
        return False, str(exc)


def _set_cpu_governor_via_sudo(target: str) -> Tuple[bool, Optional[str]]:
    if shutil.which("sudo") is None:
        return False, "sudo is not available"

    shell_cmd = (
        "for f in /sys/devices/system/cpu/cpu*/cpufreq/scaling_governor; do "
        "[ -e \"$f\" ] || continue; "
        f"echo {shlex.quote(target)} > \"$f\"; "
        "done"
    )
    proc = subprocess.run(
        ["sudo", "-n", "sh", "-c", shell_cmd],
        capture_output=True,
        text=True,
        check=False,
    )
    if proc.returncode == 0:
        return True, None

    details = proc.stderr.strip() or proc.stdout.strip() or f"sudo exited with {proc.returncode}"
    return False, details


def set_cpu_governor(target: str) -> Tuple[bool, str]:
    paths = _cpu_governor_paths()
    if not paths:
        return False, "cpufreq sysfs is unavailable on this host"

    current = _read_current_cpu_governor()
    if current == target:
        return True, f"already {target}"

    if all(os.access(path, os.W_OK) for path in paths):
        ok, error = _set_cpu_governor_via_sysfs(target, paths)
        if ok:
            return True, f"set via sysfs ({len(paths)} CPUs)"
        return False, error or "failed to write governor via sysfs"

    ok, error = _set_cpu_governor_via_sudo(target)
    if ok:
        return True, "set via sudo"
    return False, error or "failed to change governor"


@contextlib.contextmanager
def managed_cpu_governor(target: str, restore: bool):
    if target == "keep":
        yield
        return

    previous = _read_current_cpu_governor()
    success, message = set_cpu_governor(target)
    if not success:
        raise RuntimeError(
            f"failed to set CPU governor to {target}: {message}. "
            "Set it manually or rerun after sudo authentication."
        )
    print(f"CPU governor: {target} ({message})")

    try:
        yield
    finally:
        if restore and previous and previous != target:
            restored, restore_message = set_cpu_governor(previous)
            if restored:
                print(f"CPU governor restored to {previous} ({restore_message})")
            else:
                print(
                    f"Warning: failed to restore CPU governor to {previous}: {restore_message}"
                )


def _extract_cmake_cache_vars(cache_path: Path, keys: List[str]) -> Dict[str, str]:
    values: Dict[str, str] = {}
    if not cache_path.exists():
        return values
    try:
        text = cache_path.read_text(encoding="utf-8", errors="ignore")
    except Exception:
        return values

    for line in text.splitlines():
        if line.startswith("//") or line.startswith("#") or "=" not in line or ":" not in line:
            continue
        key_type, value = line.split("=", 1)
        key, _ctype = key_type.split(":", 1)
        if key in keys:
            values[key] = value.strip()
    return values


def _physical_cores_linux() -> Optional[int]:
    cpuinfo = _safe_read_text(Path("/proc/cpuinfo"))
    if not cpuinfo:
        return None

    physical = set()
    proc_id = None
    core_id = None
    for raw in cpuinfo.splitlines():
        line = raw.strip()
        if not line:
            if proc_id is not None and core_id is not None:
                physical.add((proc_id, core_id))
            proc_id = None
            core_id = None
            continue
        if line.startswith("physical id"):
            proc_id = line.split(":", 1)[1].strip()
        elif line.startswith("core id"):
            core_id = line.split(":", 1)[1].strip()

    if proc_id is not None and core_id is not None:
        physical.add((proc_id, core_id))
    if not physical:
        return None
    return len(physical)


def default_bench_workers() -> int:
    env_override = os.environ.get("KATANA_BENCH_WORKERS")
    if env_override:
        with contextlib.suppress(ValueError):
            parsed = int(env_override)
            if parsed > 0:
                return parsed

    logical = max(1, os.cpu_count() or 1)
    if logical <= 6:
        return min(logical, 4)
    return min(logical, 8)


def _parse_data_rate_to_bytes(raw: str) -> Optional[float]:
    match = re.match(r"^\s*([\d.]+)\s*([KMG]?B)\s*$", raw)
    if not match:
        return None
    value = float(match.group(1))
    unit = match.group(2)
    scale = {
        "B": 1.0,
        "KB": 1024.0,
        "MB": 1024.0 * 1024.0,
        "GB": 1024.0 * 1024.0 * 1024.0,
    }
    factor = scale.get(unit)
    if factor is None:
        return None
    return value * factor


def collect_environment_metadata() -> Dict[str, Any]:
    env: Dict[str, Any] = {}
    uname = platform.uname()
    env["platform"] = uname.system
    env["platform_release"] = uname.release
    env["platform_version"] = uname.version
    env["machine"] = uname.machine
    env["hostname"] = uname.node
    env["python"] = platform.python_version()
    env["build_dir"] = str(BUILD_DIR)
    env["benchmark_dir"] = str(BENCHMARK_DIR)

    env["logical_cores"] = os.cpu_count()
    phys = _physical_cores_linux()
    if phys is not None:
        env["physical_cores"] = phys

    cpu_model = None
    cpuinfo = _safe_read_text(Path("/proc/cpuinfo"))
    if cpuinfo:
        for raw in cpuinfo.splitlines():
            if raw.lower().startswith("model name"):
                cpu_model = raw.split(":", 1)[1].strip()
                break
    if not cpu_model:
        cpu_model = platform.processor() or None
    if cpu_model:
        env["cpu_model"] = cpu_model

    gov = _safe_read_text(Path("/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor"))
    if gov:
        env["cpu_governor"] = gov
    no_turbo = _safe_read_text(Path("/sys/devices/system/cpu/intel_pstate/no_turbo"))
    if no_turbo is not None:
        env["intel_no_turbo"] = no_turbo

    if hasattr(os, "sched_getaffinity"):
        try:
            env["affinity_cpus"] = len(os.sched_getaffinity(0))
        except Exception:
            pass

    cache_vars = _extract_cmake_cache_vars(
        BUILD_DIR / "CMakeCache.txt",
        [
            "CMAKE_CXX_COMPILER",
            "CMAKE_CXX_COMPILER_VERSION",
            "CMAKE_CXX_FLAGS",
            "CMAKE_CXX_FLAGS_RELEASE",
            "CMAKE_BUILD_TYPE",
        ],
    )
    if cache_vars:
        env["cmake"] = cache_vars

    lscpu = _run_capture(["lscpu"], timeout=3)
    if lscpu:
        lines = []
        for line in lscpu.splitlines():
            if any(
                prefix in line
                for prefix in (
                    "Architecture:",
                    "CPU(s):",
                    "Model name:",
                    "Thread(s) per core:",
                    "Core(s) per socket:",
                    "Socket(s):",
                )
            ):
                lines.append(line.strip())
        if lines:
            env["lscpu_brief"] = lines

    wrk_binary = shutil.which(os.environ.get("KATANA_WRK_BIN", "wrk"))
    if wrk_binary:
        env["wrk_binary"] = wrk_binary

    return env


def _ci95_halfwidth(metric_stats: Dict[str, float]) -> Optional[float]:
    count = metric_stats.get("count")
    stddev = metric_stats.get("stddev")
    if not isinstance(count, (int, float)) or not isinstance(stddev, (int, float)):
        return None
    if count < 2:
        return None
    return 1.96 * float(stddev) / math.sqrt(float(count))


def summarize_quality(report: BenchmarkReport, cv_warn_pct: float) -> Dict[str, Any]:
    noisy: List[Dict[str, Any]] = []
    severe: List[Dict[str, Any]] = []

    tracked_metrics = {
        "throughput",
        "bytes_per_sec",
        "latency_ns",
        "avg_latency_us",
        "latency_p50_us",
        "latency_p95_us",
        "latency_p99_us",
        "latency_p999_us",
        "latency_max_us",
        "tail_p95_ns",
        "tail_p99_ns",
        "retries_per_op_total",
    }

    for stage in report.stages:
        for bench_name, metric_map in stage.benchmark_stats.items():
            for metric_name, stats in metric_map.items():
                if metric_name not in tracked_metrics:
                    continue
                cv = _cv_pct(stats)
                if cv is None:
                    continue
                item = {
                    "stage_id": stage.stage_id,
                    "stage_name": stage.stage_name,
                    "benchmark": bench_name,
                    "metric": metric_name,
                    "cv_pct": cv,
                }
                if cv > cv_warn_pct:
                    noisy.append(item)
                if cv > cv_warn_pct * 2.0:
                    severe.append(item)

    noisy.sort(key=lambda x: x["cv_pct"], reverse=True)
    severe.sort(key=lambda x: x["cv_pct"], reverse=True)

    has_e2e = any((s.config.get("kind") in {"wrk_http", "e2e_keepalive"}) and s.success for s in report.stages)
    return {
        "cv_warn_pct": cv_warn_pct,
        "noisy_metrics_count": len(noisy),
        "severe_noisy_metrics_count": len(severe),
        "top_noisy_metrics": noisy[:15],
        "e2e_included": has_e2e,
    }


def _budget_metric_unit(metric_name: str) -> str:
    if metric_name == "bytes_per_sec":
        return "bytes/sec"
    if metric_name.endswith("_ns"):
        return "ns"
    if metric_name.endswith("_us"):
        return "us"
    if metric_name == "errors":
        return "count"
    return ""


def evaluate_stage4_sql_budgets(report: BenchmarkReport) -> Optional[Dict[str, Any]]:
    applicable_stage_ids = {
        stage.stage_id for stage in report.stages if stage.stage_id in STAGE4_SQL_BUDGETS and stage.success
    }
    if not applicable_stage_ids:
        return None

    checks: List[Dict[str, Any]] = []
    missing: List[Dict[str, Any]] = []
    stage_map = {stage.stage_id: stage for stage in report.stages if stage.success}

    for stage_id in sorted(applicable_stage_ids):
        stage = stage_map.get(stage_id)
        if stage is None:
            continue
        by_name = {bench.name: bench for bench in stage.benchmarks}
        for bench_name, metric_rules in STAGE4_SQL_BUDGETS[stage_id].items():
            bench = by_name.get(bench_name)
            if bench is None:
                missing.append(
                    {
                        "stage_id": stage_id,
                        "benchmark": bench_name,
                        "metric": "*",
                        "reason": "benchmark_missing",
                    }
                )
                continue

            for metric_name, rule in metric_rules.items():
                value = getattr(bench, metric_name, None)
                if value is None:
                    missing.append(
                        {
                            "stage_id": stage_id,
                            "benchmark": bench_name,
                            "metric": metric_name,
                            "reason": "metric_missing",
                        }
                    )
                    continue

                min_value = rule.get("min")
                max_value = rule.get("max")
                if min_value is not None:
                    passed = float(value) >= float(min_value)
                    comparator = ">="
                    target = float(min_value)
                else:
                    passed = float(value) <= float(max_value)
                    comparator = "<="
                    target = float(max_value)

                checks.append(
                    {
                        "stage_id": stage_id,
                        "stage_name": stage.stage_name,
                        "benchmark": bench_name,
                        "metric": metric_name,
                        "actual": float(value),
                        "target": target,
                        "comparator": comparator,
                        "unit": bench.throughput_unit if metric_name == "throughput" else _budget_metric_unit(metric_name),
                        "passed": passed,
                    }
                )

    failures = [item for item in checks if not item["passed"]]
    failures.sort(key=lambda item: (item["stage_id"], str(item["benchmark"]), str(item["metric"])))
    missing.sort(key=lambda item: (item["stage_id"], str(item["benchmark"]), str(item["metric"])))

    return {
        "name": "stage4_sql",
        "applicable_stage_ids": sorted(applicable_stage_ids),
        "checked_count": len(checks),
        "passed_count": len(checks) - len(failures),
        "failed_count": len(failures),
        "missing_count": len(missing),
        "failures": failures,
        "missing": missing[:25],
        "notes": STAGE4_SQL_BUDGET_NOTES,
    }


def ensure_build() -> bool:
    """Ensure benchmark binaries are built."""
    if not BUILD_DIR.exists():
        print(f"Build directory not found at {BUILD_DIR}. Running cmake configure...")
        configure_cmd: List[str]
        if BUILD_DIR == REPO_ROOT / "build" / "bench":
            configure_cmd = ["cmake", "--preset", "bench"]
        else:
            configure_cmd = [
                "cmake",
                "-S",
                str(REPO_ROOT),
                "-B",
                str(BUILD_DIR),
                "-G",
                "Ninja",
                "-DCMAKE_BUILD_TYPE=Release",
                "-DENABLE_BENCHMARKS=ON",
                "-DENABLE_EXAMPLES=ON",
            ]
        result = subprocess.run(
            configure_cmd,
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
        print("Build failed.")
        if result.stdout.strip():
            print(result.stdout)
        if result.stderr.strip():
            print(result.stderr)
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
        r"^\s+(.+?)\s+([\d.]+)\s+(ns|us)\s+([\d.]+)\s+ops/sec(?:\s+([\d.]+)\s+bytes/sec)?(?:\s+p95:\s+([\d.]+)\s+(ns|us)\s+p99:\s+([\d.]+)\s+(ns|us))?\s*$",
        re.MULTILINE,
    )

    for match in pattern.finditer(output):
        name = match.group(1).strip()
        latency = float(match.group(2))
        unit = match.group(3)
        throughput = float(match.group(4))
        bytes_per_sec = float(match.group(5)) if match.group(5) else None
        tail_p95_ns = float(match.group(6)) if match.group(6) else None
        tail_p95_unit = match.group(7) if match.group(7) else None
        tail_p99_ns = float(match.group(8)) if match.group(8) else None
        tail_p99_unit = match.group(9) if match.group(9) else None

        # Convert to ns if in us
        if unit == "us":
            latency *= 1000
        if tail_p95_ns is not None and tail_p95_unit == "us":
            tail_p95_ns *= 1000
        if tail_p99_ns is not None and tail_p99_unit == "us":
            tail_p99_ns *= 1000

        results.append(
            BenchmarkResult(
                name=name,
                latency_ns=latency,
                throughput=throughput,
                bytes_per_sec=bytes_per_sec,
                tail_p95_ns=tail_p95_ns,
                tail_p99_ns=tail_p99_ns,
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


def parse_benchmark_api_codegen_output(output: str) -> List[BenchmarkResult]:
    """Parse output from benchmark_api_codegen_benchmark."""
    results = []

    header_pattern = re.compile(
        r"^\s+(.+?)\s+([\d.]+)\s+(ns|us)\s+([\d.]+)\s+ops/sec\s*$",
        re.MULTILINE,
    )
    metrics_pattern = re.compile(
        r"p50:\s+([\d.]+)\s+us\s+\|\s+p95:\s+([\d.]+)\s+us\s+\|\s+p99:\s+([\d.]+)\s+us\s+\|\s+p999:\s+([\d.]+)\s+us\s+\|\s+errors:\s+[\d.]+%\s+\((\d+)/(\d+)\)"
    )

    lines = output.splitlines()
    i = 0
    while i < len(lines):
        header_match = header_pattern.match(lines[i])
        if not header_match:
            i += 1
            continue

        name = header_match.group(1).strip()
        latency = float(header_match.group(2))
        unit = header_match.group(3)
        throughput = float(header_match.group(4))
        latency_ns = latency * 1000.0 if unit == "us" else latency

        result = BenchmarkResult(name=name, latency_ns=latency_ns, throughput=throughput)

        if i + 1 < len(lines):
            metrics_match = metrics_pattern.search(lines[i + 1])
            if metrics_match:
                result.latency_p50_us = float(metrics_match.group(1))
                result.latency_p95_us = float(metrics_match.group(2))
                result.latency_p99_us = float(metrics_match.group(3))
                result.latency_p999_us = float(metrics_match.group(4))
                result.errors = int(metrics_match.group(5))
                result.operations = float(metrics_match.group(6))
                i += 1

        results.append(result)
        i += 1

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
    if binary_name == "generated_api_benchmark":
        return parse_router_output(output)
    if binary_name == "benchmark_api_codegen_benchmark":
        return parse_benchmark_api_codegen_output(output)
    if binary_name == "benchmark_api_framework_benchmark":
        return parse_benchmark_api_codegen_output(output)
    if binary_name == "sql_codegen_benchmark":
        return parse_codegen_quality_output(output)
    if binary_name == "sql_runtime_benchmark":
        return parse_codegen_quality_output(output)
    if binary_name == "sql_postgres_benchmark":
        return parse_codegen_quality_output(output)
    if binary_name == "sql_postgres_throughput_benchmark":
        return parse_codegen_quality_output(output)
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
    pos = clamped * (len(sorted_values) - 1)
    lo = int(math.floor(pos))
    hi = int(math.ceil(pos))
    if lo == hi:
        return sorted_values[lo]
    frac = pos - lo
    return sorted_values[lo] * (1.0 - frac) + sorted_values[hi] * frac


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
        "avg_latency_us": b.avg_latency_us,
        "latency_p50_us": b.latency_p50_us,
        "latency_p95_us": b.latency_p95_us,
        "latency_p99_us": b.latency_p99_us,
        "latency_p999_us": b.latency_p999_us,
        "latency_max_us": b.latency_max_us,
        "tail_p95_ns": b.tail_p95_ns,
        "tail_p99_ns": b.tail_p99_ns,
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


def aggregate_benchmark_result(
    candidates: List[BenchmarkResult], aggregation_mode: str = "median"
) -> BenchmarkResult:
    """Aggregate each metric independently for a more stable summary result."""
    if not candidates:
        raise ValueError("aggregate_benchmark_result requires at least one candidate")

    mode = aggregation_mode.lower()
    if mode == "best":
        return choose_representative_result(candidates, aggregation_mode=aggregation_mode)

    stats = build_benchmark_stats(candidates)
    base = candidates[0]
    out = BenchmarkResult(
        name=base.name,
        throughput_unit=base.throughput_unit,
        thread_producers=base.thread_producers,
        thread_consumers=base.thread_consumers,
        thread_hw=base.thread_hw,
    )

    metric_map = [
        ("throughput", "throughput"),
        ("bytes_per_sec", "bytes_per_sec"),
        ("operations", "operations"),
        ("latency_ns", "latency_ns"),
        ("avg_latency_us", "avg_latency_us"),
        ("latency_p50_us", "latency_p50_us"),
        ("latency_p95_us", "latency_p95_us"),
        ("latency_p99_us", "latency_p99_us"),
        ("latency_p999_us", "latency_p999_us"),
        ("latency_max_us", "latency_max_us"),
        ("tail_p95_ns", "tail_p95_ns"),
        ("tail_p99_ns", "tail_p99_ns"),
        ("producer_retries", "producer_retries"),
        ("consumer_retries", "consumer_retries"),
        ("retries_per_op_push", "retries_per_op_push"),
        ("retries_per_op_pop", "retries_per_op_pop"),
        ("retries_per_op_total", "retries_per_op_total"),
        ("thread_oversubscription", "thread_oversubscription"),
        ("errors", "errors"),
    ]

    stat_key = "mean" if mode == "mean" else "p50"
    for attr, metric_name in metric_map:
        metric_stats = stats.get(metric_name)
        if not metric_stats:
            continue
        value = metric_stats.get(stat_key)
        if value is None:
            continue
        if attr == "errors":
            setattr(out, attr, int(round(value)))
        else:
            setattr(out, attr, float(value))

    return out


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

    avg_latency_us_values = [float(c.avg_latency_us) for c in candidates if c.avg_latency_us is not None]
    if avg_latency_us_values:
        stats["avg_latency_us"] = _metric_stats(avg_latency_us_values)

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

    tail_p95_values = [float(c.tail_p95_ns) for c in candidates if c.tail_p95_ns is not None]
    if tail_p95_values:
        stats["tail_p95_ns"] = _metric_stats(tail_p95_values)

    tail_p99_values = [float(c.tail_p99_ns) for c in candidates if c.tail_p99_ns is not None]
    if tail_p99_values:
        stats["tail_p99_ns"] = _metric_stats(tail_p99_values)

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
        stats_by_name[name] = build_benchmark_stats(candidates)
        merged.append(aggregate_benchmark_result(candidates, aggregation_mode=aggregation_mode))
        if include_runs:
            runs_by_name[name] = [_benchmark_to_dict(c) for c in candidates]

    return merged, stats_by_name, runs_by_name


def _resolve_binary_candidates(candidates: List[str]) -> List[Path]:
    resolved: List[Path] = []
    seen: set[str] = set()
    for raw in candidates:
        candidate = Path(raw)
        probe_list = []
        if candidate.is_absolute():
            probe_list.append(candidate)
        else:
            probe_list.extend([BUILD_DIR / candidate, REPO_ROOT / candidate])
        for probe in probe_list:
            key = str(probe)
            if key in seen:
                continue
            seen.add(key)
            resolved.append(probe)
    return resolved


def _find_binary(candidates: List[Path]) -> Optional[Path]:
    for candidate in candidates:
        if candidate.exists() and candidate.is_file():
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


def _is_port_in_use(host: str, port: int) -> bool:
    try:
        with socket.create_connection((host, port), timeout=0.2):
            return True
    except OSError:
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


def _prepare_wrk_http_invocation(stage: Dict[str, Any]) -> Tuple[Optional[WrkHttpInvocation], Optional[str]]:
    wrk_binary = shutil.which(os.environ.get("KATANA_WRK_BIN", "wrk"))
    if not wrk_binary:
        return None, "wrk not found in PATH"

    server_candidates = _resolve_binary_candidates(stage["server_candidates"])
    server_binary = _find_binary(server_candidates)
    if server_binary is None:
        return None, f"{stage['server_target']} binary not found"

    port = int(stage["port"])
    bench_workers = int(stage.get("bench_workers", default_bench_workers()))
    env = dict(os.environ)
    for key, value in stage.get("server_env", {}).items():
        env[key] = value.format(port=port, bench_workers=bench_workers)

    wrk_env = dict(env)
    for key, value in stage.get("wrk_env", {}).items():
        wrk_env[key] = value.format(port=port, bench_workers=bench_workers)

    return (
        WrkHttpInvocation(
            wrk_binary=wrk_binary,
            server_binary=server_binary,
            port=port,
            bench_workers=bench_workers,
            env=env,
            wrk_env=wrk_env,
            script_path=REPO_ROOT / stage["wrk_script"],
            benchmark_name=stage["benchmark_name"],
            server_target=stage["server_target"],
            wrk_threads=int(stage["wrk_threads"]),
            wrk_connections=int(stage["wrk_connections"]),
            wrk_duration_sec=int(stage["wrk_duration_sec"]),
            wrk_warmup_sec=int(stage.get("wrk_warmup_sec", 0)),
            wrk_url=stage["wrk_url"].format(port=port),
            cleanup_sql=list(stage.get("cleanup_sql", [])),
        ),
        None,
    )


def _build_wrk_http_command(
    invocation: WrkHttpInvocation,
    duration_sec: int,
    *,
    include_latency: bool,
) -> List[str]:
    cmd = [
        invocation.wrk_binary,
        "-t",
        str(invocation.wrk_threads),
        "-c",
        str(invocation.wrk_connections),
        "-d",
        f"{duration_sec}s",
        "-s",
        str(invocation.script_path),
        invocation.wrk_url,
    ]
    if include_latency:
        cmd.insert(7, "--latency")
    return cmd


def _start_wrk_http_server(invocation: WrkHttpInvocation) -> Tuple[Optional[subprocess.Popen[str]], Optional[str]]:
    if _is_port_in_use("127.0.0.1", invocation.port):
        return None, f"port {invocation.port} already in use before starting {invocation.server_target}"

    server_proc = _start_server_process(invocation.server_binary, env=invocation.env)
    if _wait_for_port("127.0.0.1", invocation.port, timeout_s=10.0):
        return server_proc, None

    details = ""
    if server_proc.poll() is not None and server_proc.stderr is not None:
        with contextlib.suppress(Exception):
            details = server_proc.stderr.read().strip()
    _stop_server_process(server_proc)
    suffix = f": {details}" if details else ""
    return None, f"{invocation.server_target} failed to listen on {invocation.port}{suffix}"


def _run_wrk_http_subprocess(
    invocation: WrkHttpInvocation,
    *,
    duration_sec: int,
    include_latency: bool,
    mode: str,
) -> Tuple[Optional[subprocess.CompletedProcess[str]], Optional[str]]:
    proc = subprocess.run(
        _build_wrk_http_command(invocation, duration_sec, include_latency=include_latency),
        capture_output=True,
        text=True,
        timeout=max(30 if include_latency else 15, duration_sec + 15 if include_latency else duration_sec + 10),
        env=invocation.wrk_env,
    )
    if proc.returncode == 0:
        return proc, None
    details = proc.stderr.strip() or proc.stdout.strip()
    return None, f"wrk {mode} failed: {details}"


def _run_wrk_http_warmup(invocation: WrkHttpInvocation) -> Optional[str]:
    if invocation.wrk_warmup_sec <= 0:
        return None
    _, error = _run_wrk_http_subprocess(
        invocation,
        duration_sec=invocation.wrk_warmup_sec,
        include_latency=False,
        mode="warmup",
    )
    return error


def _run_wrk_http_measurement(invocation: WrkHttpInvocation) -> Tuple[BenchmarkResult, Optional[str]]:
    proc, error = _run_wrk_http_subprocess(
        invocation,
        duration_sec=invocation.wrk_duration_sec,
        include_latency=True,
        mode="run",
    )
    if error:
        return (
            BenchmarkResult(
                name=invocation.benchmark_name,
                throughput=0.0,
                throughput_unit="req/sec",
                errors=0,
            ),
            error,
        )

    assert proc is not None
    parsed = _parse_wrk_output(proc.stdout, invocation.benchmark_name)
    if parsed is None:
        return (
            BenchmarkResult(
                name=invocation.benchmark_name,
                throughput=0.0,
                throughput_unit="req/sec",
                errors=0,
            ),
            "failed to parse wrk output",
        )
    return parsed, None


def _run_wrk_http_cleanup(invocation: WrkHttpInvocation) -> Optional[str]:
    if not invocation.cleanup_sql:
        return None

    psql_binary = shutil.which("psql")
    if not psql_binary:
        return "psql not found in PATH for PostgreSQL cleanup"

    dsn = invocation.env.get("KATANA_BENCHMARK_API_POSTGRES_DSN") or invocation.env.get(
        "KATANA_TEST_POSTGRES_DSN"
    )
    if not dsn:
        return "PostgreSQL cleanup requested but DSN is not set"

    for sql in invocation.cleanup_sql:
        proc = subprocess.run(
            [psql_binary, dsn, "-v", "ON_ERROR_STOP=1", "-Atc", sql],
            capture_output=True,
            text=True,
            timeout=15,
            env=invocation.env,
        )
        if proc.returncode != 0:
            details = proc.stderr.strip() or proc.stdout.strip()
            return f"PostgreSQL cleanup failed: {details}"
    return None


def _parse_wrk_time_to_us(raw: str) -> Optional[float]:
    match = re.match(r"^\s*([\d.]+)\s*(us|ms|s)\s*$", raw)
    if not match:
        return None
    value = float(match.group(1))
    unit = match.group(2)
    if unit == "us":
        return value
    if unit == "ms":
        return value * 1000.0
    if unit == "s":
        return value * 1_000_000.0
    return None


def _parse_wrk_output(output: str, benchmark_name: str) -> Optional[BenchmarkResult]:
    throughput = None
    operations = None
    bytes_per_sec = None
    avg_latency_us = None
    latency_p50_us = None
    latency_p95_us = None
    latency_p99_us = None
    latency_p999_us = None
    latency_max_us = None
    errors = 0

    for line in output.splitlines():
        stripped = line.strip()
        if stripped.startswith("Requests/sec:"):
            match = re.search(r"Requests/sec:\s*([\d.]+)", stripped)
            if match:
                throughput = float(match.group(1))
        elif stripped.startswith("Transfer/sec:"):
            match = re.search(r"Transfer/sec:\s*([\d.]+\s*[KMG]?B)", stripped)
            if match:
                bytes_per_sec = _parse_data_rate_to_bytes(match.group(1))
        elif stripped.startswith("Latency"):
            match = re.search(r"Latency\s+([\d.]+\s*(?:us|ms|s))", stripped)
            if match:
                avg_latency_us = _parse_wrk_time_to_us(match.group(1))
        elif "requests in" in stripped:
            match = re.search(r"([\d.]+)\s+requests in", stripped)
            if match:
                operations = float(match.group(1))
        elif stripped.startswith("Socket errors:"):
            match = re.search(
                r"connect\s+(\d+),\s*read\s+(\d+),\s*write\s+(\d+),\s*timeout\s+(\d+)",
                stripped,
            )
            if match:
                errors = sum(int(match.group(i)) for i in range(1, 5))
        elif stripped.startswith("KATANA_LATENCY_P50_US:"):
            latency_p50_us = float(stripped.split(":", 1)[1].strip())
        elif stripped.startswith("KATANA_LATENCY_P95_US:"):
            latency_p95_us = float(stripped.split(":", 1)[1].strip())
        elif stripped.startswith("KATANA_LATENCY_P99_US:"):
            latency_p99_us = float(stripped.split(":", 1)[1].strip())
        elif stripped.startswith("KATANA_LATENCY_P999_US:"):
            latency_p999_us = float(stripped.split(":", 1)[1].strip())
        elif stripped.startswith("KATANA_LATENCY_MAX_US:"):
            latency_max_us = float(stripped.split(":", 1)[1].strip())
        elif stripped.startswith("KATANA_ERRORS:"):
            errors = int(float(stripped.split(":", 1)[1].strip()))

    if throughput is None:
        return None

    return BenchmarkResult(
        name=benchmark_name,
        throughput=throughput,
        throughput_unit="req/sec",
        operations=operations,
        avg_latency_us=avg_latency_us,
        bytes_per_sec=bytes_per_sec,
        latency_p50_us=latency_p50_us,
        latency_p95_us=latency_p95_us,
        latency_p99_us=latency_p99_us,
        latency_p999_us=latency_p999_us,
        latency_max_us=latency_max_us,
        errors=errors,
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
    env = {
        **os.environ,
        "PORT": str(port),
        "COMPUTE_PORT": str(port),
        "KATANA_WORKERS": os.environ.get("KATANA_BENCH_WORKERS", "8"),
    }

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
        description=STAGES[stage_id]["description"],
        aggregation_mode=aggregation_mode,
        config={
            "kind": "e2e_keepalive",
            "connections": e2e_connections,
            "requests_per_connection": e2e_requests_per_connection,
            "port": e2e_port,
        },
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


def run_wrk_http_once(stage: Dict[str, Any]) -> Tuple[BenchmarkResult, Optional[str]]:
    invocation, error = _prepare_wrk_http_invocation(stage)
    if invocation is None:
        return (
            BenchmarkResult(
                name=stage["benchmark_name"],
                throughput=0.0,
                throughput_unit="req/sec",
                errors=0,
            ),
            error,
        )

    server_proc: Optional[subprocess.Popen[str]] = None
    measured_result = BenchmarkResult(
        name=stage["benchmark_name"],
        throughput=0.0,
        throughput_unit="req/sec",
        errors=0,
    )
    run_error: Optional[str] = None
    try:
        server_proc, error = _start_wrk_http_server(invocation)
        if error:
            run_error = error
        else:
            error = _run_wrk_http_warmup(invocation)
            if error:
                run_error = error
            else:
                measured_result, run_error = _run_wrk_http_measurement(invocation)
    finally:
        _stop_server_process(server_proc)
        cleanup_error = _run_wrk_http_cleanup(invocation)
        if cleanup_error and run_error is None:
            run_error = cleanup_error

    return measured_result, run_error


def run_wrk_http_stage(
    stage_id: int,
    stage_name: str,
    stage: Dict[str, Any],
    repeats: int,
    aggregation_mode: str,
    include_runs: bool,
    reuse_server: bool,
) -> StageResult:
    result = StageResult(
        stage_id=stage_id,
        stage_name=stage_name,
        description=stage.get("description"),
        aggregation_mode=aggregation_mode,
        config={
            "kind": "wrk_http",
            "profile": stage.get("profile"),
            "server_target": stage.get("server_target"),
            "port": stage.get("port"),
            "wrk_threads": stage.get("wrk_threads"),
            "wrk_connections": stage.get("wrk_connections"),
            "wrk_duration_sec": stage.get("wrk_duration_sec"),
            "wrk_warmup_sec": stage.get("wrk_warmup_sec"),
            "wrk_script": stage.get("wrk_script"),
            "wrk_url": stage.get("wrk_url"),
            "bench_workers": stage.get("bench_workers", default_bench_workers()),
            "pipeline_depth": stage.get("wrk_env", {}).get("KATANA_PIPELINE_DEPTH"),
            "server_lifecycle": "reused" if reuse_server else "per_run",
        },
    )

    print(f"\n[Stage {stage_id}] Running {stage_name} ({aggregation_mode}-of-{repeats})...")
    run_results: List[List[BenchmarkResult]] = []
    run_errors: List[str] = []
    started = time.perf_counter()

    if not reuse_server:
        for run_idx in range(repeats):
            print(f"  Run {run_idx + 1}/{repeats}...")
            run_result, error = run_wrk_http_once(stage)
            if error:
                run_errors.append(f"run {run_idx + 1}: {error}")
                continue
            run_results.append([run_result])
    else:
        invocation, error = _prepare_wrk_http_invocation(stage)
        if error or invocation is None:
            run_errors.append(f"setup: {error or 'failed to prepare wrk stage'}")
        else:
            server_proc: Optional[subprocess.Popen[str]] = None
            try:
                server_proc, error = _start_wrk_http_server(invocation)
                if error:
                    run_errors.append(f"setup: {error}")
                else:
                    error = _run_wrk_http_warmup(invocation)
                    if error:
                        run_errors.append(f"warmup: {error}")
                    else:
                        for run_idx in range(repeats):
                            print(f"  Run {run_idx + 1}/{repeats}...")
                            run_result, error = _run_wrk_http_measurement(invocation)
                            if error:
                                run_errors.append(f"run {run_idx + 1}: {error}")
                                continue
                            run_results.append([run_result])
            finally:
                _stop_server_process(server_proc)
                cleanup_error = _run_wrk_http_cleanup(invocation)
                if cleanup_error:
                    run_errors.append(f"cleanup: {cleanup_error}")

    result.duration_ms = int((time.perf_counter() - started) * 1000)
    if run_errors:
        if all(_is_network_restricted_error(err) or "wrk not found" in err.lower() for err in run_errors):
            result.success = True
            result.error_message = "skipped: load tool or listener unavailable in environment"
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
    if "latency" in lowered or lowered.startswith("tail_") or "errors" in lowered or "retry" in lowered:
        return "lower"
    return None


WRK_COMPARISON_METRICS = {"throughput", "errors"}
WRK_POLICY_NOTE = "wrk_http compares throughput/errors only and requires repeated runs on both current and baseline."


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
        stage_kind = stage.get("config", {}).get("kind")
        run_count = int(stage.get("run_count") or 0)

        by_name = {b.get("name"): b for b in stage.get("benchmarks", []) if b.get("name")}
        by_stats = stage.get("benchmark_stats", {})
        for bench_name, bench in by_name.items():
            stats = by_stats.get(bench_name, {})
            for metric_name in [
                "throughput",
                "bytes_per_sec",
                "latency_ns",
                "avg_latency_us",
                "latency_p50_us",
                "latency_p95_us",
                "latency_p99_us",
                "latency_p999_us",
                "latency_max_us",
                "tail_p95_ns",
                "tail_p99_ns",
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
                    sample_count = int(metric_stats.get("count", 0) or 0)
                else:
                    sample_count = 0

                if value is None:
                    raw = bench.get(metric_name)
                    if isinstance(raw, (int, float)):
                        value = float(raw)
                        if sample_count == 0:
                            sample_count = 1

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
                    "stage_kind": stage_kind,
                    "run_count": run_count,
                    "sample_count": sample_count,
                }
    return metrics


def _comparison_policy_reason(
    current_metric: Dict[str, Any],
    baseline_metric: Dict[str, Any],
) -> Optional[str]:
    if current_metric.get("stage_kind") != "wrk_http" and baseline_metric.get("stage_kind") != "wrk_http":
        return None

    metric_name = str(current_metric.get("metric") or baseline_metric.get("metric") or "")
    if metric_name not in WRK_COMPARISON_METRICS:
        return "wrk_informational_only"

    current_samples = int(current_metric.get("sample_count") or 0)
    baseline_samples = int(baseline_metric.get("sample_count") or 0)
    if current_samples < 2 or baseline_samples < 2:
        return "wrk_requires_repeats"

    return None


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
            "policy_skipped": [],
            "policy_skipped_count": 0,
            "policy_skipped_breakdown": {},
            "policy_notes": [WRK_POLICY_NOTE],
        }
    common_keys = sorted(set(current_metrics.keys()) & set(baseline_metrics.keys()))

    regressions: List[Dict[str, Any]] = []
    noisy_regressions: List[Dict[str, Any]] = []
    improvements: List[Dict[str, Any]] = []
    unchanged: List[Dict[str, Any]] = []
    policy_skipped: List[Dict[str, Any]] = []

    for key in common_keys:
        cur = current_metrics[key]
        base = baseline_metrics[key]
        skip_reason = _comparison_policy_reason(cur, base)
        if skip_reason is not None:
            policy_skipped.append(
                {
                    "key": key,
                    "stage_id": cur["stage_id"],
                    "benchmark": cur["benchmark"],
                    "metric": cur["metric"],
                    "reason": skip_reason,
                    "current_sample_count": cur.get("sample_count"),
                    "baseline_sample_count": base.get("sample_count"),
                }
            )
            continue

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
    policy_skipped.sort(key=lambda x: (str(x["reason"]), str(x["key"])))

    policy_skipped_breakdown: Dict[str, int] = {}
    for item in policy_skipped:
        reason = str(item["reason"])
        policy_skipped_breakdown[reason] = policy_skipped_breakdown.get(reason, 0) + 1

    compared_metrics = len(regressions) + len(noisy_regressions) + len(improvements) + len(unchanged)

    return {
        "baseline_loaded": True,
        "threshold_pct": threshold_pct,
        "cv_threshold_pct": cv_threshold_pct,
        "compared_metrics": compared_metrics,
        "regressions": regressions,
        "noisy_regressions": noisy_regressions,
        "improvements": improvements,
        "unchanged_count": len(unchanged),
        "hard_regressions_count": len(regressions),
        "noisy_regressions_count": len(noisy_regressions),
        "improvements_count": len(improvements),
        "policy_skipped": policy_skipped,
        "policy_skipped_count": len(policy_skipped),
        "policy_skipped_breakdown": policy_skipped_breakdown,
        "policy_notes": [WRK_POLICY_NOTE],
    }


def run_stage(
    stage_id: int,
    repeats: int = 10,
    aggregation_mode: str = "median",
    include_runs: bool = False,
    stage_warmup: bool = True,
    perf_stat: bool = False,
    perf_events: Optional[List[str]] = None,
    e2e_connections: int = 16,
    e2e_requests_per_connection: int = 200,
    e2e_port: int = 18082,
    wrk_reuse_server: bool = False,
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
    if stage.get("kind") == "wrk_http":
        return run_wrk_http_stage(
            stage_id=stage_id,
            stage_name=stage["name"],
            stage=stage,
            repeats=repeats,
            aggregation_mode=aggregation_mode,
            include_runs=include_runs,
            reuse_server=wrk_reuse_server,
        )

    binary_name = stage["binary"]
    binary_path = BENCHMARK_DIR / binary_name

    result = StageResult(
        stage_id=stage_id,
        stage_name=stage["name"],
        description=stage.get("description"),
        run_count=0,
        aggregation_mode=aggregation_mode,
        config={
            "kind": "microbenchmark",
            "binary": binary_name,
            "binary_path": str(binary_path),
            "env": stage.get("env"),
        },
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
    stage_env = dict(os.environ)
    for key, value in stage.get("env", {}).items():
        stage_env[key] = str(value)

    def _skip_message(stdout: str, stderr: str) -> Optional[str]:
        combined = "\n".join(part for part in [stdout.strip(), stderr.strip()] if part).lower()
        if "[skip]" not in combined and "skipping" not in combined:
            return None
        for source in (stdout, stderr):
            for line in source.splitlines():
                if "[skip]" in line.lower() or "skipping" in line.lower():
                    return line.strip()
        return "benchmark self-reported skip"

    if stage_warmup:
        try:
            warmup_proc = subprocess.run(
                [str(binary_path)],
                capture_output=True,
                text=True,
                timeout=300,
                env=stage_env,
            )
            if warmup_proc.returncode != 0:
                print(f"  Warmup failed (ignored): exit code {warmup_proc.returncode}")
        except Exception as exc:
            print(f"  Warmup failed (ignored): {exc}")

    for run_idx in range(repeats):
        print(f"  Run {run_idx + 1}/{repeats}...")
        try:
            proc = subprocess.run(
                [str(binary_path)],
                capture_output=True,
                text=True,
                timeout=300,
                env=stage_env,
            )
            if proc.returncode != 0:
                run_errors.append(f"run {run_idx + 1}: exit code {proc.returncode}")
                continue

            skip_message = _skip_message(proc.stdout, proc.stderr)
            if skip_message is not None:
                result.success = True
                result.error_message = f"skipped: {skip_message}"
                result.duration_ms = int((time.perf_counter() - started) * 1000)
                print(f"  Skipped: {result.error_message}")
                return result

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


def _is_network_stage(stage: StageResult) -> bool:
    return stage.config.get("kind") in {"wrk_http", "e2e_keepalive"}


def _render_stage_config(config: Dict[str, Any]) -> List[str]:
    rows: List[str] = []
    for key in sorted(config.keys()):
        value = config[key]
        if value is None:
            continue
        rows.append(f"| {key} | {value} |")
    return rows


def generate_markdown(report: BenchmarkReport) -> str:
    """Generate BENCHMARK_RESULTS.md content from report."""
    lines = [
        "# KATANA Benchmark Results",
        "",
        f"> Last updated: {report.generated_at}",
    ]

    if report.commit_sha:
        lines.append(f"> Commit: {report.commit_sha}")

    quality = report.quality_summary or {}
    budget = report.budget_summary or {}
    noisy_count = int(quality.get("noisy_metrics_count", 0))
    severe_noisy_count = int(quality.get("severe_noisy_metrics_count", 0))
    cv_warn = float(quality.get("cv_warn_pct", 20.0))
    e2e_included = bool(quality.get("e2e_included", False))
    budget_failed = int(budget.get("failed_count", 0))
    budget_checked = int(budget.get("checked_count", 0))
    stability_verdict = "stable enough for trend tracking"
    if noisy_count > 0:
        stability_verdict = "contains noisy metrics, inspect before trusting small deltas"

    lines.extend([
        "",
        "## Summary",
        "",
        f"- Stability verdict: {stability_verdict}",
        f"- Noisy metrics (CV > {cv_warn:.1f}%): {noisy_count}",
        f"- Severely noisy metrics (CV > {cv_warn * 2.0:.1f}%): {severe_noisy_count}",
        f"- Budget gate failures: {budget_failed} / {budget_checked}" if budget_checked > 0 else "- Budget gates evaluated: no",
        f"- E2E network stages included: {'yes' if e2e_included else 'no'}",
        "",
        f"> **Note**: Results shown use {_aggregation_title(report.aggregation_mode)} "
        f"aggregation across {_repeat_policy_text(report)}. Concurrent benchmarks are highly "
        "sensitive to system load and thread scheduling.",
        "",
        "---",
        "",
    ])

    if report.environment:
        lines.extend(["## Environment", ""])
        lines.extend(["| Key | Value |", "|-----|-------|"])
        for key in sorted(report.environment.keys()):
            value = report.environment[key]
            if isinstance(value, dict):
                rendered = "; ".join(f"{k}={v}" for k, v in sorted(value.items()))
            elif isinstance(value, list):
                rendered = "; ".join(str(v) for v in value)
            else:
                rendered = str(value)
            lines.append(f"| {key} | {rendered} |")
        lines.extend(["", "---", ""])

    if report.quality_summary:
        lines.extend(["## Quality Gates", ""])
        lines.append(f"- CV warning threshold: {cv_warn:.2f}%")
        lines.append(f"- Noisy metrics: {noisy_count}")
        lines.append(f"- Severely noisy metrics: {severe_noisy_count}")
        lines.append("")

        top_noisy = quality.get("top_noisy_metrics", [])
        if top_noisy:
            lines.extend(
                [
                    "| Top Noisy Metric | CV |",
                    "|------------------|----|",
                ]
            )
            for item in top_noisy[:10]:
                label = (
                    f"stage{item.get('stage_id')}.{item.get('benchmark')}.{item.get('metric')}"
                )
                lines.append(f"| {label} | {item.get('cv_pct', 0.0):.2f}% |")
            lines.append("")
        lines.extend(["---", ""])

    if report.budget_summary:
        budget = report.budget_summary
        lines.extend(["## Budget Gates", ""])
        lines.append(f"- Budget preset: {budget.get('name', 'unknown')}")
        lines.append(f"- Checked metrics: {budget.get('checked_count', 0)}")
        lines.append(f"- Failed metrics: {budget.get('failed_count', 0)}")
        lines.append(f"- Missing metrics: {budget.get('missing_count', 0)}")
        lines.append("")
        for note in budget.get("notes", []):
            lines.append(f"- Note: {note}")
        if budget.get("notes"):
            lines.append("")

        failures = budget.get("failures", [])
        if failures:
            lines.extend([
                "| Stage | Benchmark | Metric | Target | Actual |",
                "|-------|-----------|--------|--------|--------|",
            ])
            for item in failures[:20]:
                unit = f" {item.get('unit')}" if item.get("unit") else ""
                lines.append(
                    f"| {item.get('stage_id')} | {item.get('benchmark')} | {item.get('metric')} | "
                    f"{item.get('comparator')} {item.get('target'):.3f}{unit} | "
                    f"{item.get('actual'):.3f}{unit} |"
                )
            lines.append("")
        lines.extend(["---", ""])

    network_stages = [stage for stage in report.stages if stage.success and _is_network_stage(stage) and stage.benchmarks]
    if network_stages:
        lines.extend(["## E2E Summary", ""])
        lines.extend([
            "> **Note**: For pipeline stages, `wrk` latency metrics reflect one scripted batch/request() call,",
            "> not isolated single-request service time. Throughput and error-free completion are the primary",
            "> comparable signals across different pipeline depths.",
            "",
        ])
        lines.extend([
            "| Stage | Profile | Benchmark | Workers | wrk t/c | Depth | Duration | Throughput | Avg Latency | p50 | p95 | p99 | Data Rate | Errors |",
            "|-------|---------|-----------|---------|---------|-------|----------|------------|-------------|-----|-----|-----|-----------|--------|",
        ])
        for stage in network_stages:
            bench = stage.benchmarks[0]
            profile = stage.config.get("profile", stage.config.get("kind", "-"))
            workers = stage.config.get("bench_workers", "-")
            wrk_tc = f"{stage.config.get('wrk_threads', '-')}/{stage.config.get('wrk_connections', '-')}"
            depth = stage.config.get("pipeline_depth", "-")
            duration = stage.config.get("wrk_duration_sec")
            duration_s = f"{duration}s" if duration is not None else "-"
            throughput = (
                f"{format_throughput(bench.throughput)} {bench.throughput_unit}"
                if bench.throughput is not None
                else "-"
            )
            avg_latency = f"{bench.avg_latency_us:.3f} us" if bench.avg_latency_us is not None else "-"
            p50 = f"{bench.latency_p50_us:.3f} us" if bench.latency_p50_us is not None else "-"
            p95 = f"{bench.latency_p95_us:.3f} us" if bench.latency_p95_us is not None else "-"
            p99 = f"{bench.latency_p99_us:.3f} us" if bench.latency_p99_us is not None else "-"
            data_rate = format_data_rate(bench.bytes_per_sec) if bench.bytes_per_sec is not None else "-"
            lines.append(
                f"| {stage.stage_id} | {profile} | {bench.name} | {workers} | {wrk_tc} | {depth} | {duration_s} | "
                f"{throughput} | {avg_latency} | {p50} | {p95} | {p99} | {data_rate} | {bench.errors} |"
            )
        lines.extend(["", "---", ""])

    for stage in report.stages:
        if not stage.success:
            lines.extend(
                [
                    f"## {stage.stage_name}",
                    "",
                    f"**Error**: {stage.error_message}",
                    "",
                    "---",
                    "",
                ]
            )
            continue

        lines.extend([
            f"## {stage.stage_name}",
            "",
            f"_Runs: {stage.run_count} | Aggregation: {stage.aggregation_mode}_",
            "",
        ])

        if stage.description:
            lines.extend([stage.description, ""])

        config_rows = _render_stage_config(stage.config)
        if config_rows:
            lines.extend(["| Config | Value |", "|--------|-------|"])
            lines.extend(config_rows)
            lines.append("")

        if stage.error_message and not stage.benchmarks:
            lines.extend([f"_Note: {stage.error_message}_", "", "---", ""])
            continue

        has_latency = any(b.latency_ns is not None for b in stage.benchmarks)
        has_percentiles = any(
            b.latency_p50_us is not None
            or b.latency_p95_us is not None
            or b.latency_p99_us is not None
            or b.latency_p999_us is not None
            or b.latency_max_us is not None
            for b in stage.benchmarks
        )
        has_tail_latency = any(
            b.tail_p95_ns is not None or b.tail_p99_ns is not None for b in stage.benchmarks
        )
        has_data_rate = any(b.bytes_per_sec is not None for b in stage.benchmarks)
        has_retry_total = any(b.retries_per_op_total is not None for b in stage.benchmarks)

        if has_percentiles:
            latency_cols: List[Tuple[str, str]] = []
            for field_name, label in [
                ("avg_latency_us", "Latency avg"),
                ("latency_p50_us", "Latency p50"),
                ("latency_p95_us", "Latency p95"),
                ("latency_p99_us", "Latency p99"),
                ("latency_p999_us", "Latency p999"),
                ("latency_max_us", "Latency max"),
            ]:
                if any(getattr(b, field_name) is not None for b in stage.benchmarks):
                    latency_cols.append((field_name, label))

            headers = ["Benchmark", "Throughput"]
            if has_data_rate:
                headers.append("Data Rate")
            headers.extend([label for _, label in latency_cols])
            if has_retry_total:
                headers.append("Retries/op")
            headers.append("Errors")
            lines.append("| " + " | ".join(headers) + " |")
            lines.append("|" + "|".join("-" * (len(h) + 2) for h in headers) + "|")

            for b in stage.benchmarks:
                throughput = (
                    f"{format_throughput(b.throughput)} {b.throughput_unit}"
                    if b.throughput is not None
                    else "-"
                )
                row = [b.name, throughput]
                if has_data_rate:
                    row.append(format_data_rate(b.bytes_per_sec) if b.bytes_per_sec is not None else "-")
                for field_name, _label in latency_cols:
                    value = getattr(b, field_name)
                    row.append(f"{value:.3f} us" if value is not None else "-")
                retry_total = (
                    f"{b.retries_per_op_total:.3f}" if b.retries_per_op_total is not None else "-"
                )
                if has_retry_total:
                    row.append(retry_total)
                row.append(str(b.errors))
                lines.append("| " + " | ".join(row) + " |")
        elif has_latency:
            headers = ["Benchmark", "Latency", "Throughput"]
            if has_tail_latency:
                headers.extend(["Tail p95", "Tail p99"])
            if has_data_rate:
                headers.append("Data Rate")
            lines.append("| " + " | ".join(headers) + " |")
            lines.append("|" + "|".join("-" * (len(h) + 2) for h in headers) + "|")
            for b in stage.benchmarks:
                latency = f"{b.latency_ns:.1f} ns" if b.latency_ns is not None else "-"
                throughput = (
                    f"{format_throughput(b.throughput)} {b.throughput_unit}"
                    if b.throughput is not None
                    else "-"
                )
                row = [b.name, latency, throughput]
                if has_tail_latency:
                    row.append(f"{b.tail_p95_ns:.1f} ns" if b.tail_p95_ns is not None else "-")
                    row.append(f"{b.tail_p99_ns:.1f} ns" if b.tail_p99_ns is not None else "-")
                if has_data_rate:
                    row.append(format_data_rate(b.bytes_per_sec) if b.bytes_per_sec is not None else "-")
                lines.append("| " + " | ".join(row) + " |")
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
                    ci95 = _ci95_halfwidth(throughput_stats)
                    min_v = throughput_stats.get("min", 0.0)
                    p50_v = throughput_stats.get("p50", 0.0)
                    p95_v = throughput_stats.get("p95", 0.0)
                    max_v = throughput_stats.get("max", 0.0)

                    stability_rows.append(
                        f"| {b.name} | {format_throughput(mean)} | {format_throughput(stddev)} | {cv:.2f}% | "
                        f"{format_throughput(ci95) if ci95 is not None else '-'} | "
                        f"{format_throughput(min_v)} | {format_throughput(p50_v)} | {format_throughput(p95_v)} | "
                        f"{format_throughput(max_v)} |"
                    )

                latency_metric_name = ""
                latency_stats: Dict[str, float] = {}
                for metric_key, label in [
                    ("avg_latency_us", "avg us"),
                    ("latency_p99_us", "p99 us"),
                    ("latency_p95_us", "p95 us"),
                    ("latency_p999_us", "p999 us"),
                    ("latency_p50_us", "p50 us"),
                    ("tail_p99_ns", "tail p99 ns"),
                    ("tail_p95_ns", "tail p95 ns"),
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
                    lat_ci95 = _ci95_halfwidth(latency_stats)
                    lat_p50 = latency_stats.get("p50", 0.0)
                    lat_p95 = latency_stats.get("p95", 0.0)
                    lat_max = latency_stats.get("max", 0.0)
                    latency_rows.append(
                        f"| {b.name} | {latency_metric_name} | {lat_mean:.3f} | {lat_stddev:.3f} | "
                        f"{lat_cv:.2f}% | "
                        f"{lat_ci95:.3f} | {lat_p50:.3f} | {lat_p95:.3f} | {lat_max:.3f} |"
                    )

                retry_stats = stage.benchmark_stats.get(b.name, {}).get("retries_per_op_total", {})
                if int(retry_stats.get("count", 0)) >= 2:
                    r_mean = retry_stats.get("mean", 0.0)
                    r_stddev = retry_stats.get("stddev", 0.0)
                    r_cv = (r_stddev / r_mean * 100.0) if r_mean > 0 else 0.0
                    r_ci95 = _ci95_halfwidth(retry_stats)
                    r_p50 = retry_stats.get("p50", 0.0)
                    r_p95 = retry_stats.get("p95", 0.0)
                    r_max = retry_stats.get("max", 0.0)
                    retry_rows.append(
                        f"| {b.name} | {r_mean:.3f} | {r_stddev:.3f} | {r_cv:.2f}% | "
                        f"{r_ci95:.3f} | "
                        f"{r_p50:.3f} | {r_p95:.3f} | {r_max:.3f} |"
                    )

                data_rate_stats = stage.benchmark_stats.get(b.name, {}).get("bytes_per_sec", {})
                if int(data_rate_stats.get("count", 0)) >= 2:
                    d_mean = data_rate_stats.get("mean", 0.0)
                    d_stddev = data_rate_stats.get("stddev", 0.0)
                    d_cv = (d_stddev / d_mean * 100.0) if d_mean > 0 else 0.0
                    d_ci95 = _ci95_halfwidth(data_rate_stats)
                    d_min = data_rate_stats.get("min", 0.0)
                    d_p50 = data_rate_stats.get("p50", 0.0)
                    d_p95 = data_rate_stats.get("p95", 0.0)
                    d_max = data_rate_stats.get("max", 0.0)
                    data_rate_rows.append(
                        f"| {b.name} | {format_data_rate(d_mean)} | {format_data_rate(d_stddev)} | {d_cv:.2f}% | "
                        f"{format_data_rate(d_ci95) if d_ci95 is not None else '-'} | "
                        f"{format_data_rate(d_min)} | {format_data_rate(d_p50)} | {format_data_rate(d_p95)} | "
                        f"{format_data_rate(d_max)} |"
                    )

            if stability_rows:
                lines.extend([
                    "",
                    "### Throughput Stability (Across Repeated Runs)",
                    "",
                    "| Benchmark | Mean | Stddev | CV | 95% CI | Min | p50 | p95 | Max |",
                    "|-----------|------|--------|----|--------|-----|-----|-----|-----|",
                ])
                lines.extend(stability_rows)

            if latency_rows:
                lines.extend([
                    "",
                    "### Latency Stability (Across Repeated Runs)",
                    "",
                    "| Benchmark | Metric | Mean | Stddev | CV | 95% CI | p50 | p95 | Max |",
                    "|-----------|--------|------|--------|----|--------|-----|-----|-----|",
                ])
                lines.extend(latency_rows)

            if retry_rows:
                lines.extend([
                    "",
                    "### Contention Stability (Retries/op)",
                    "",
                    "| Benchmark | Mean | Stddev | CV | 95% CI | p50 | p95 | Max |",
                    "|-----------|------|--------|----|--------|-----|-----|-----|",
                ])
                lines.extend(retry_rows)

            if data_rate_rows:
                lines.extend([
                    "",
                    "### Data Throughput Stability (bytes/sec)",
                    "",
                    "| Benchmark | Mean | Stddev | CV | 95% CI | Min | p50 | p95 | Max |",
                    "|-----------|------|--------|----|--------|-----|-----|-----|-----|",
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
                f"- Policy-skipped metrics: {summary.get('policy_skipped_count', 0)}",
                f"- Thresholds: delta={summary.get('threshold_pct', 0):.2f}% | CV gate={summary.get('cv_threshold_pct', 0):.2f}%",
                "",
            ])
            for note in summary.get("policy_notes", []):
                lines.append(f"- Policy: {note}")
            if summary.get("policy_notes"):
                lines.append("")

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

            skipped = summary.get("policy_skipped", [])[:10]
            if skipped:
                lines.extend([
                    "| Policy-Skipped Metric | Reason | Current Samples | Baseline Samples |",
                    "|-----------------------|--------|-----------------|------------------|",
                ])
                for item in skipped:
                    lines.append(
                        f"| {item.get('key')} | {item.get('reason')} | "
                        f"{item.get('current_sample_count', '-')} | {item.get('baseline_sample_count', '-')} |"
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
        "# Run full micro + wrk-based E2E pipeline",
        "./scripts/run_benchmarks.py --include-e2e",
        "",
        "# Recommended repeat policy with explicit quality gate",
        "./scripts/run_benchmarks.py --aggregation median --stage-repeat 1=20 --cv-threshold-pct 15",
        "",
        "# Run Stage 4 SQL budgets as a CI gate",
        "./scripts/run_benchmarks.py --stage 15 16 17 18 --fail-on-budget",
        "",
        "# Diagnose steady-state wrk behavior without per-sample server restarts",
        "./scripts/run_benchmarks.py --stage 17 18 --wrk-reuse-server",
        "",
        "# Point the runner at a custom build tree or wrk binary",
        "KATANA_BENCH_BUILD_DIR=build/bench-wsl KATANA_WRK_BIN=wrk ./scripts/run_benchmarks.py --include-e2e",
        "",
        "# Collect perf counters (requires perf permissions)",
        "./scripts/run_benchmarks.py --perf-stat",
        "",
        "# Compare against a baseline",
        "./scripts/run_benchmarks.py --compare benchmarks/baseline.json --fail-on-regression",
        "",
        "# Run specific stages",
        "./scripts/run_benchmarks.py --stage 1 2 9 10",
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
        "environment": report.environment,
        "quality_summary": report.quality_summary,
        "budget_summary": report.budget_summary,
        "comparison_summary": report.comparison_summary,
        "stages": [
            {
                "stage_id": s.stage_id,
                "stage_name": s.stage_name,
                "description": s.description,
                "duration_ms": s.duration_ms,
                "success": s.success,
                "error_message": s.error_message,
                "run_count": s.run_count,
                "aggregation_mode": s.aggregation_mode,
                "config": s.config,
                "benchmarks": [
                    {
                        "name": b.name,
                        "latency_ns": b.latency_ns,
                        "throughput": b.throughput,
                        "throughput_unit": b.throughput_unit,
                        "bytes_per_sec": b.bytes_per_sec,
                        "operations": b.operations,
                        "avg_latency_us": b.avg_latency_us,
                        "latency_p50_us": b.latency_p50_us,
                        "latency_p95_us": b.latency_p95_us,
                        "latency_p99_us": b.latency_p99_us,
                        "latency_p999_us": b.latency_p999_us,
                        "latency_max_us": b.latency_max_us,
                        "tail_p95_ns": b.tail_p95_ns,
                        "tail_p99_ns": b.tail_p99_ns,
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
    print(f"  Policy-skipped metrics: {summary.get('policy_skipped_count', 0)}")
    for note in summary.get("policy_notes", []):
        print(f"  Policy: {note}")

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

    skipped = summary.get("policy_skipped", [])
    if skipped:
        print("  Top policy-skipped metrics:")
        for item in skipped[:10]:
            print(
                f"    - {item['key']}: {item.get('reason')} "
                f"(samples {item.get('current_sample_count', '-')}/{item.get('baseline_sample_count', '-')})"
            )


def print_budget_summary(summary: Optional[Dict[str, Any]]) -> None:
    if not summary:
        return

    print("\nBudget gates:")
    print(f"  Preset: {summary.get('name', 'unknown')}")
    print(f"  Checked metrics: {summary.get('checked_count', 0)}")
    print(f"  Failed metrics: {summary.get('failed_count', 0)}")
    print(f"  Missing metrics: {summary.get('missing_count', 0)}")

    failures = summary.get("failures", [])
    if failures:
        print("  Failures:")
        for item in failures[:20]:
            unit = f" {item.get('unit')}" if item.get("unit") else ""
            print(
                f"    - stage {item.get('stage_id')} / {item.get('benchmark')} / {item.get('metric')}: "
                f"{item.get('actual'):.3f}{unit} (target {item.get('comparator')} {item.get('target'):.3f}{unit})"
            )


def main():
    global BUILD_DIR, BENCHMARK_DIR

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
        help="Run only specific stages. If not specified, runs stages 1-4, 6-8, and 13.",
    )
    parser.add_argument(
        "--include-e2e",
        action="store_true",
        help="Include wrk-based E2E stages 9-12 when --stage is not set.",
    )
    parser.add_argument(
        "--include-legacy-e2e",
        action="store_true",
        help="Also include legacy stage 5 keep-alive benchmark when --stage is not set.",
    )
    parser.add_argument(
        "--build-dir",
        type=Path,
        help="Explicit build directory override (defaults to auto-detected bench or bench-wsl tree).",
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
        "--no-stage-warmup",
        action="store_true",
        help="Disable unmeasured warmup launch before each non-E2E stage.",
    )
    parser.add_argument(
        "--wrk-reuse-server",
        action="store_true",
        help=(
            "Reuse one live server per wrk_http stage instead of restarting per sample. "
            "Diagnostic steady-state mode only; default cold-start gate semantics stay unchanged."
        ),
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
        "--auto-compare-baseline",
        type=Path,
        default=REPO_ROOT / "benchmarks" / "baseline.json",
        help=(
            "Auto-compare against this baseline when --compare is not provided "
            "(default: benchmarks/baseline.json)."
        ),
    )
    parser.add_argument(
        "--no-auto-compare-baseline",
        action="store_true",
        help="Disable baseline auto-compare fallback.",
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
        "--cv-warn-pct",
        type=float,
        default=20.0,
        help="Warning threshold for noisy metrics in report quality section (default: 20.0).",
    )
    parser.add_argument(
        "--fail-on-regression",
        action="store_true",
        help="Return non-zero when hard regressions are detected in --compare mode.",
    )
    parser.add_argument(
        "--fail-on-budget",
        action="store_true",
        help="Return non-zero when applicable Stage 4 SQL budget checks fail.",
    )
    parser.add_argument(
        "--cpu-governor",
        choices=["keep", "performance", "powersave"],
        default="keep",
        help=(
            "Attempt to switch Linux cpufreq governor before running benchmarks "
            "(default: keep current governor)."
        ),
    )
    parser.add_argument(
        "--no-restore-cpu-governor",
        action="store_true",
        help="Do not restore the previous CPU governor after the run.",
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

    if args.build_dir:
        BUILD_DIR = args.build_dir.expanduser().resolve()
        BENCHMARK_DIR = BUILD_DIR / "benchmark"

    if args.list_stages:
        print("Available benchmark stages:")
        for stage_id, stage in STAGES.items():
            print(f"  {stage_id}. {stage['name']}")
            if "binary" in stage:
                print(f"     Binary: {stage['binary']}")
            if stage.get("kind") == "wrk_http":
                depth = stage.get("wrk_env", {}).get("KATANA_PIPELINE_DEPTH", "-")
                print(
                    "     "
                    f"profile={stage.get('profile', '-')}, workers={stage.get('bench_workers', '-')}, "
                    f"wrk={stage.get('wrk_threads', '-')}/{stage.get('wrk_connections', '-')}, depth={depth}"
                )
            print(f"     {stage['description']}")
        return 0

    if args.repeats is not None and args.repeats < 1:
        print("Error: --repeats must be >= 1")
        return 1
    if args.e2e_connections < 1 or args.e2e_requests_per_connection < 1:
        print("Error: --e2e-connections and --e2e-requests-per-connection must be >= 1")
        return 1
    if args.cv_threshold_pct <= 0 or args.cv_warn_pct <= 0:
        print("Error: --cv-threshold-pct and --cv-warn-pct must be > 0")
        return 1

    if args.stage:
        stages_to_run = args.stage
    else:
        stages_to_run = [1, 2, 3, 4, 6, 7, 8, 13]
        if args.include_e2e:
            stages_to_run.extend([9, 10, 11, 12])
        if args.include_legacy_e2e:
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

    try:
        with managed_cpu_governor(
            args.cpu_governor,
            restore=not args.no_restore_cpu_governor,
        ):
            if not args.no_build:
                if not ensure_build():
                    print("Error: Failed to build benchmarks")
                    return 1

            print(f"\n{'='*60}")
            print("KATANA Benchmark Runner")
            print(f"{'='*60}")
            print(f"Build dir: {BUILD_DIR}")
            print(f"Stages to run: {stages_to_run}")
            print(f"Default repeats: {base_repeats}")
            print(
                "Stage repeats: "
                + ", ".join(f"{sid}={effective_repeats_by_stage[sid]}" for sid in stages_to_run)
            )
            print(f"Aggregation mode: {args.aggregation}")
            print(f"Stage warmup: {'disabled' if args.no_stage_warmup else 'enabled'}")
            print(
                "wrk server lifecycle: "
                + ("reused per stage (steady-state diagnostic)" if args.wrk_reuse_server else "restart per sample")
            )
            print(f"perf stat: {'enabled' if args.perf_stat else 'disabled'}")

            report = BenchmarkReport(
                generated_at=datetime.now().strftime("%Y-%m-%d %H:%M:%S"),
                commit_sha=get_git_sha(),
                repeats=base_repeats,
                aggregation_mode=args.aggregation,
                stage_repeats={k: v for k, v in sorted(effective_repeats_by_stage.items())},
                environment=collect_environment_metadata(),
            )

            started = time.perf_counter()
            for stage_id in stages_to_run:
                result = run_stage(
                    stage_id,
                    repeats=effective_repeats_by_stage[stage_id],
                    aggregation_mode=args.aggregation,
                    include_runs=args.include_runs,
                    stage_warmup=not args.no_stage_warmup,
                    perf_stat=args.perf_stat,
                    perf_events=perf_events,
                    e2e_connections=args.e2e_connections,
                    e2e_requests_per_connection=args.e2e_requests_per_connection,
                    e2e_port=args.e2e_port,
                    wrk_reuse_server=args.wrk_reuse_server,
                )
                report.stages.append(result)
            report.total_duration_ms = int((time.perf_counter() - started) * 1000)

            compare_path: Optional[Path] = args.compare
            if compare_path is None and not args.no_auto_compare_baseline:
                candidate = args.auto_compare_baseline
                if candidate.exists():
                    compare_path = candidate
                    print(f"\nAuto compare baseline detected: {compare_path}")

            if compare_path:
                if not compare_path.exists():
                    print(f"Error: Baseline file not found: {compare_path}")
                    return 1
                try:
                    with open(compare_path, "r") as f:
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

            report.quality_summary = summarize_quality(report, cv_warn_pct=args.cv_warn_pct)
            report.budget_summary = evaluate_stage4_sql_budgets(report)
            print_budget_summary(report.budget_summary)

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
            quality = report.quality_summary or {}
            print(
                f"\nQuality: noisy_metrics={quality.get('noisy_metrics_count', 0)} "
                f"(CV>{quality.get('cv_warn_pct', args.cv_warn_pct):.1f}%)"
            )
            budget = report.budget_summary or {}
            if budget:
                print(
                    "Budget gates: "
                    f"failed={budget.get('failed_count', 0)} / checked={budget.get('checked_count', 0)}"
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
            budget_failures = 0
            if report.budget_summary:
                budget_failures = int(report.budget_summary.get("failed_count", 0))
            if args.fail_on_regression and hard_regressions > 0:
                return 1
            if args.fail_on_budget and budget_failures > 0:
                return 1
            return 0 if all_passed else 1
    except RuntimeError as exc:
        print(f"Error: {exc}")
        return 1


if __name__ == "__main__":
    sys.exit(main())
