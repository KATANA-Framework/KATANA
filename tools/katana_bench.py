#!/usr/bin/env python3
"""KATANA Unified Benchmark Runner v2.0

Usage:
    python3 tools/katana_bench.py --all
    python3 tools/katana_bench.py --stage 1
    python3 tools/katana_bench.py --stage 2
    python3 tools/katana_bench.py --stage 2 --compare benchmarks/baseline_stage2.json
    python3 tools/katana_bench.py --stage 2 --format json > results.json
"""

import argparse
import contextlib
import json
import os
import re
import subprocess
import time
import socket
import sys
from pathlib import Path
from typing import Dict, List, Optional, Tuple

# Benchmark stage definitions
STAGES = {
    "1": {
        "name": "Core Runtime",
        "binaries": [
            "performance_benchmark",
            "simple_benchmark",
            "mpsc_benchmark",
            "timer_benchmark",
            "headers_benchmark",
            "io_buffer_benchmark",
            "router_benchmark",
            "simd_whitespace_benchmark",
        ],
    },
    "2": {
        "name": "Codegen Quality",
        "binaries": [
            "codegen_quality_benchmark",
            "generated_json_benchmark",
            "serialize_benchmark",
            "json_parsing_benchmark",
            "json_benchmark",
            "generated_api_benchmark",
            "products_api_benchmark",
            "benchmark_api_codegen_benchmark",
        ],
    },
}

TIMEOUT_OVERRIDES = {
    "performance_benchmark": 300,
    "simple_benchmark": 180,
}


def find_build_dir() -> Path:
    """Find the benchmark build directory."""
    candidates = [
        Path("build/benchmark"),
        Path("build/debug/benchmark"),
        Path("build/release/benchmark"),
        Path("cmake-build-release/benchmark"),
        Path("cmake-build-debug/benchmark"),
    ]
    for c in candidates:
        if c.exists():
            return c
    # Try preset-based
    for d in Path(".").glob("build/*/benchmark"):
        if d.is_dir():
            return d
    print("ERROR: Cannot find benchmark build directory.", file=sys.stderr)
    print("Build with: cmake --preset debug && cmake --build --preset debug", file=sys.stderr)
    sys.exit(1)


def find_companion_binary(build_dir: Path, names: List[str]) -> Optional[Path]:
    """Find binary near benchmark directory (build/bench/*)."""
    roots = [build_dir.parent, build_dir.parent / "examples"]
    for root in roots:
        for name in names:
            p = root / name
            if p.exists():
                return p
    return None


def wait_for_port(host: str, port: int, timeout_s: float = 10.0) -> bool:
    """Wait until TCP port starts accepting connections."""
    deadline = time.monotonic() + timeout_s
    while time.monotonic() < deadline:
        try:
            with socket.create_connection((host, port), timeout=0.5):
                return True
        except OSError:
            time.sleep(0.2)
    return False


def start_server(server_binary: Path, port: int) -> Optional[subprocess.Popen]:
    """Start server and wait for readiness."""
    try:
        proc = subprocess.Popen(
            [str(server_binary), str(port)],
            stdout=subprocess.DEVNULL,
            stderr=subprocess.DEVNULL,
        )
    except Exception:
        return None

    if wait_for_port("127.0.0.1", port, timeout_s=10.0):
        return proc

    with contextlib.suppress(Exception):
        proc.terminate()
        proc.wait(timeout=2)
    return None


def run_benchmark(
    binary_path: Path,
    timeout: int = 120,
    args: Optional[List[str]] = None,
    env: Optional[Dict[str, str]] = None,
) -> Tuple[str, float]:
    """Run a single benchmark binary and return (output, elapsed_seconds)."""
    if not binary_path.exists():
        return f"SKIP: {binary_path.name} not found", 0.0

    start = time.monotonic()
    try:
        result = subprocess.run(
            [str(binary_path)] + (args or []),
            capture_output=True,
            text=True,
            timeout=timeout,
            env=env,
        )
        elapsed = time.monotonic() - start
        output = result.stdout
        if result.returncode != 0:
            output += f"\nEXIT CODE: {result.returncode}\n"
            if result.stderr:
                output += f"STDERR: {result.stderr}\n"
        return output, elapsed
    except subprocess.TimeoutExpired:
        return f"TIMEOUT after {timeout}s", time.monotonic() - start
    except Exception as e:
        return f"ERROR: {e}", 0.0


def sanitize_metric_key(raw: str) -> str:
    """Convert free-form metric labels to stable snake_case keys."""
    key = raw.strip().lower()
    key = re.sub(r"[%()]+", "", key)
    key = key.replace("/", "_per_")
    key = key.replace("-", "_")
    key = re.sub(r"\s+", "_", key)
    key = re.sub(r"[^a-z0-9_]", "", key)
    key = re.sub(r"_+", "_", key).strip("_")
    return key or "metric"


def section_key(name: str) -> str:
    """Normalize benchmark section title into a stable key."""
    return sanitize_metric_key(name)


def latency_to_ns(value: float, unit: str) -> float:
    """Convert ns/us/ms value to nanoseconds."""
    if unit == "ns":
        return value
    if unit == "us":
        return value * 1000.0
    if unit == "ms":
        return value * 1_000_000.0
    return value


def parse_benchmark_output(output: str) -> Dict:
    """Parse benchmark output for structured metrics."""
    metrics: Dict[str, Dict[str, float]] = {}

    # Pattern: name    X.XX ns    YYYYYY ops/sec
    # Or:     name    X.XX us    YYYYYY ops/sec
    pattern = re.compile(
        r"^\s+(.+?)\s+([\d.]+)\s+(ns|us)\s+([\d.]+)\s+ops/sec",
        re.MULTILINE,
    )
    for m in pattern.finditer(output):
        name = m.group(1).strip()
        value = float(m.group(2))
        unit = m.group(3)
        ops = float(m.group(4))
        if unit == "us":
            value *= 1000  # convert to ns
        metrics[name] = {
            "ns_per_op": value,
            "ops_per_sec": ops,
        }

    current_section = "summary"

    for raw_line in output.splitlines():
        line = raw_line.strip()
        if not line:
            continue

        # Section headers: === Name === or --- Name ---
        m = re.match(r"^(?:===|---)\s*(.+?)\s*(?:===|---)$", line)
        if m:
            current_section = section_key(m.group(1))
            metrics.setdefault(current_section, {})
            continue

        entry = metrics.setdefault(current_section, {})

        # Operations: 200000
        m = re.match(r"^Operations:\s*([\d.]+)$", line, re.IGNORECASE)
        if m:
            entry["operations"] = float(m.group(1))
            continue

        # Duration: 123.45 ms
        m = re.match(r"^Duration:\s*([\d.]+)\s*ms$", line, re.IGNORECASE)
        if m:
            entry["duration_ms"] = float(m.group(1))
            continue

        # Throughput: 123.45 ops/sec / 1.23 M ops/s
        m = re.match(
            r"^Throughput:\s*([\d.]+)\s*(M)?\s*ops/(?:sec|s)$",
            line,
            re.IGNORECASE,
        )
        if m:
            throughput = float(m.group(1))
            if m.group(2):
                throughput *= 1_000_000.0
            entry["throughput_ops_per_sec"] = throughput
            continue

        # Errors: 10
        m = re.match(r"^Errors:\s*(\d+)$", line, re.IGNORECASE)
        if m:
            entry["errors"] = float(m.group(1))
            continue

        # Latency pXX: value unit
        m = re.match(
            r"^Latency\s+p(\d+(?:\.\d+)?):\s*([\d.]+)\s*(ns|us|ms)$",
            line,
            re.IGNORECASE,
        )
        if m:
            p = sanitize_metric_key(f"latency_p{m.group(1)}")
            entry[f"{p}_ns"] = latency_to_ns(float(m.group(2)), m.group(3).lower())
            continue

        # pXX: value unit (supports piped format)
        for token in [t.strip() for t in line.split("|")]:
            p_match = re.match(
                r"^p(\d+(?:\.\d+)?):\s*([\d.]+)\s*(ns|us|ms)$",
                token,
                re.IGNORECASE,
            )
            if p_match:
                p = sanitize_metric_key(f"latency_p{p_match.group(1)}")
                entry[f"{p}_ns"] = latency_to_ns(
                    float(p_match.group(2)), p_match.group(3).lower()
                )
                continue

            # errors: X% (A/B)
            err_match = re.match(
                r"^errors:\s*([\d.]+)%\s*\((\d+)/(\d+)\)$",
                token,
                re.IGNORECASE,
            )
            if err_match:
                entry["error_rate_pct"] = float(err_match.group(1))
                entry["errors"] = float(err_match.group(2))
                entry["total_requests"] = float(err_match.group(3))

    return metrics


def metric_direction(metric_name: str) -> Optional[str]:
    """Return comparison direction: higher/lower is better."""
    name = metric_name.lower()

    higher_is_better = (
        "ops_per_sec",
        "throughput",
    )
    lower_is_better = (
        "ns_per_op",
        "latency",
        "duration",
        "errors",
        "error_rate",
        "timeout",
        "fail",
    )
    neutral = ("operations", "total_requests", "count")

    if any(k in name for k in neutral):
        return None
    if any(k in name for k in higher_is_better):
        return "higher"
    if any(k in name for k in lower_is_better):
        return "lower"
    return None


def compare_with_baseline(
    results: Dict, baseline: Dict, threshold: float = 0.03
) -> List[Dict]:
    """Compare metric dictionaries with baseline."""
    comparisons = []
    for bench_name, current_metrics in results.items():
        if bench_name not in baseline:
            continue
        baseline_metrics = baseline[bench_name]
        if not isinstance(current_metrics, dict) or not isinstance(baseline_metrics, dict):
            continue

        for metric_name, current in current_metrics.items():
            if metric_name not in baseline_metrics:
                continue
            base = baseline_metrics[metric_name]
            if not isinstance(current, (int, float)) or not isinstance(base, (int, float)):
                continue
            if base == 0:
                continue

            direction = metric_direction(metric_name)
            if not direction:
                continue

            if direction == "higher":
                delta = (float(current) - float(base)) / float(base)
            else:
                delta = (float(base) - float(current)) / float(base)

            status = (
                "improved"
                if delta > threshold
                else ("regressed" if delta < -threshold else "unchanged")
            )
            comparisons.append(
                {
                    "benchmark": bench_name,
                    "metric": metric_name,
                    "baseline": float(base),
                    "current": float(current),
                    "change_pct": delta * 100.0,
                    "status": status,
                    "direction": direction,
                }
            )
    return comparisons


def format_console(stage_name: str, binary: str, output: str, comparisons: List[Dict] = None):
    """Format results for console output."""
    print(f"\n[{binary}]")
    print(output)
    if comparisons:
        print("  Comparisons with baseline:")
        for c in comparisons:
            symbol = "+" if c["status"] != "regressed" else "!"
            print(
                f"    {symbol} {c['benchmark']}.{c['metric']}: "
                f"{c['change_pct']:+.1f}% "
                f"(base={c['baseline']:.3f}, cur={c['current']:.3f})"
            )


def main():
    parser = argparse.ArgumentParser(description="KATANA Benchmark Suite v2.0")
    parser.add_argument("--stage", choices=["1", "2", "e2e", "all"], default="all",
                        help="Benchmark stage to run")
    parser.add_argument("--scope", type=str, help="Run specific benchmark binary")
    parser.add_argument("--format", choices=["json", "md", "console"], default="console",
                        help="Output format")
    parser.add_argument("--compare", type=str, help="Baseline JSON file for comparison")
    parser.add_argument("--timeout", type=int, default=120,
                        help="Timeout per benchmark in seconds")
    parser.add_argument("--threshold", type=float, default=0.03,
                        help="Regression threshold (fraction, default: 0.03 = 3%)")
    args = parser.parse_args()

    build_dir = find_build_dir()
    print(f"KATANA Benchmark Suite v2.0", file=sys.stderr)
    print(f"Build directory: {build_dir}", file=sys.stderr)

    # Determine which stages to run
    if args.stage == "all":
        stages_to_run = ["1", "2"]
    else:
        stages_to_run = [args.stage]

    # Load baseline if provided
    baseline_data = {}
    if args.compare:
        try:
            with open(args.compare) as f:
                baseline_data = json.load(f).get("benchmarks", {})
        except Exception as e:
            print(f"WARNING: Cannot load baseline: {e}", file=sys.stderr)

    all_results = {}
    all_comparisons = []

    for stage_id in stages_to_run:
        if stage_id not in STAGES:
            print(f"WARNING: Unknown stage {stage_id}", file=sys.stderr)
            continue

        stage = STAGES[stage_id]
        print(f"\nRunning Stage {stage_id}: {stage['name']} benchmarks...",
              file=sys.stderr)

        binaries = stage["binaries"]
        if args.scope:
            binaries = [b for b in binaries if args.scope in b]

        for i, binary_name in enumerate(binaries, 1):
            binary_path = build_dir / binary_name
            print(f"  [{i}/{len(binaries)}] {binary_name}...", file=sys.stderr, end="", flush=True)

            effective_timeout = max(args.timeout, TIMEOUT_OVERRIDES.get(binary_name, args.timeout))
            run_args: Optional[List[str]] = None
            run_env: Optional[Dict[str, str]] = None
            server_proc: Optional[subprocess.Popen] = None

            if binary_name == "simple_benchmark":
                server_binary = find_companion_binary(
                    build_dir, ["hello_world_server", "hello_world_server.exe"]
                )
                if not server_binary:
                    output = "SKIP: hello_world_server not found for simple_benchmark"
                    elapsed = 0.0
                    metrics = {}
                    all_results[binary_name] = metrics
                    print(" skipped (server missing)", file=sys.stderr)
                    if args.format == "console":
                        format_console(stage["name"], binary_name, output, [])
                    continue

                server_proc = start_server(server_binary, 18080)
                if not server_proc:
                    output = "SKIP: failed to start hello_world_server on 18080"
                    elapsed = 0.0
                    metrics = {}
                    all_results[binary_name] = metrics
                    print(" skipped (server start failed)", file=sys.stderr)
                    if args.format == "console":
                        format_console(stage["name"], binary_name, output, [])
                    continue

                run_args = ["18080"]
                run_env = {**os.environ, "HELLO_PORT": "18080"}

            output, elapsed = run_benchmark(
                binary_path,
                effective_timeout,
                args=run_args,
                env=run_env,
            )
            metrics = parse_benchmark_output(output)
            all_results[binary_name] = metrics

            if server_proc is not None:
                with contextlib.suppress(Exception):
                    server_proc.terminate()
                    server_proc.wait(timeout=5)

            print(f" done ({elapsed:.1f}s)", file=sys.stderr)

            # Compare with baseline
            stage_baseline = baseline_data.get(binary_name, {})
            comparisons = compare_with_baseline(metrics, stage_baseline, args.threshold)
            all_comparisons.extend(comparisons)

            if args.format == "console":
                format_console(stage["name"], binary_name, output, comparisons)

    # Output
    if args.format == "json":
        output = {
            "version": "2.0",
            "timestamp": time.strftime("%Y-%m-%dT%H:%M:%SZ", time.gmtime()),
            "benchmarks": all_results,
        }
        print(json.dumps(output, indent=2))
    elif args.format == "md":
        print("# KATANA Benchmark Results\n")
        for binary_name, metrics in all_results.items():
            print(f"## {binary_name}\n")
            print("| Benchmark | Metric | Value |")
            print("|-----------|--------|-------|")
            for name, metric_map in metrics.items():
                for metric_name, value in metric_map.items():
                    if isinstance(value, (int, float)):
                        print(f"| {name} | {metric_name} | {value:.6g} |")
            print()

    # Summary
    improved = sum(1 for c in all_comparisons if c["status"] == "improved")
    regressed = sum(1 for c in all_comparisons if c["status"] == "regressed")
    unchanged = sum(1 for c in all_comparisons if c["status"] == "unchanged")

    if all_comparisons:
        print(f"\nSummary: {improved} improved, {regressed} regressed, {unchanged} unchanged",
              file=sys.stderr)
        if regressed > 0:
            print("WARNING: Performance regressions detected!", file=sys.stderr)
            sys.exit(1)


if __name__ == "__main__":
    main()
