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
import json
import os
import re
import subprocess
import sys
import time
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
            "serialize_benchmark",
            "json_parsing_benchmark",
            "json_benchmark",
            "generated_api_benchmark",
            "products_api_benchmark",
        ],
    },
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


def run_benchmark(binary_path: Path, timeout: int = 120) -> Tuple[str, float]:
    """Run a single benchmark binary and return (output, elapsed_seconds)."""
    if not binary_path.exists():
        return f"SKIP: {binary_path.name} not found", 0.0

    start = time.monotonic()
    try:
        result = subprocess.run(
            [str(binary_path)],
            capture_output=True,
            text=True,
            timeout=timeout,
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


def parse_benchmark_output(output: str) -> Dict:
    """Parse benchmark output for key metrics."""
    metrics = {}
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
    return metrics


def compare_with_baseline(
    results: Dict, baseline: Dict, threshold: float = 0.03
) -> List[Dict]:
    """Compare results with baseline. Returns list of comparisons."""
    comparisons = []
    for name, current in results.items():
        if name in baseline:
            base = baseline[name]
            base_ns = base.get("ns_per_op", 0)
            curr_ns = current.get("ns_per_op", 0)
            if base_ns > 0:
                change_pct = (base_ns - curr_ns) / base_ns
                status = "improved" if change_pct > threshold else (
                    "regressed" if change_pct < -threshold else "unchanged"
                )
                comparisons.append({
                    "name": name,
                    "baseline_ns": base_ns,
                    "current_ns": curr_ns,
                    "change_pct": change_pct * 100,
                    "status": status,
                })
    return comparisons


def format_console(stage_name: str, binary: str, output: str, comparisons: List[Dict] = None):
    """Format results for console output."""
    print(f"\n[{binary}]")
    print(output)
    if comparisons:
        print("  Comparisons with baseline:")
        for c in comparisons:
            symbol = "+" if c["status"] != "regressed" else "!"
            print(f"    {symbol} {c['name']}: {c['change_pct']:+.1f}%")


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

            output, elapsed = run_benchmark(binary_path, args.timeout)
            metrics = parse_benchmark_output(output)
            all_results[binary_name] = metrics

            print(f" done ({elapsed:.1f}s)", file=sys.stderr)

            # Compare with baseline
            stage_baseline = baseline_data.get(binary_name, {})
            comparisons = compare_with_baseline(metrics, stage_baseline)
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
            print("| Benchmark | Latency | Throughput |")
            print("|-----------|---------|------------|")
            for name, m in metrics.items():
                ns = m["ns_per_op"]
                lat = f"{ns:.1f} ns" if ns < 1000 else f"{ns/1000:.2f} us"
                print(f"| {name} | {lat} | {m['ops_per_sec']:.0f} ops/sec |")
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
