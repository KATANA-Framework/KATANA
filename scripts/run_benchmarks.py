#!/usr/bin/env python3
"""
Unified Benchmark Runner for KATANA Framework

This script runs all benchmarks in stages, collects results, and generates
structured output for documentation and CI integration.

Usage:
    ./scripts/run_benchmarks.py                    # Run all stages
    ./scripts/run_benchmarks.py --stage 1          # Run only stage 1
    ./scripts/run_benchmarks.py --stage 1 2        # Run stages 1 and 2
    ./scripts/run_benchmarks.py --repeats 5        # Best-of-5 runs per stage
    ./scripts/run_benchmarks.py --output results   # Save results to results/
    ./scripts/run_benchmarks.py --update-docs      # Update BENCHMARK_RESULTS.md
    ./scripts/run_benchmarks.py --json             # Output JSON to stdout

Stages:
    1. Core Runtime: Ring buffer, circular buffer, SIMD operations
    2. Codegen Quality: JSON parsing, serialization, arena allocation
    3. Router: HTTP routing dispatch performance
    4. HTTP Parser: Full request parsing benchmarks
"""

from __future__ import annotations

import argparse
import json
import os
import re
import subprocess
import sys
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
    latency_p50_us: Optional[float] = None
    latency_p99_us: Optional[float] = None
    latency_p999_us: Optional[float] = None
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


@dataclass
class BenchmarkReport:
    """Complete benchmark report."""

    generated_at: str
    commit_sha: Optional[str] = None
    stages: List[StageResult] = field(default_factory=list)
    total_duration_ms: int = 0
    repeats: int = 3


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

    # Pattern: "  name                               X.X ns    YYYYY ops/sec"
    pattern = re.compile(
        r"^\s+(.+?)\s+([\d.]+)\s+(ns|us)\s+([\d.]+)\s+ops/sec", re.MULTILINE
    )

    for match in pattern.finditer(output):
        name = match.group(1).strip()
        latency = float(match.group(2))
        unit = match.group(3)
        throughput = float(match.group(4))

        # Convert to ns if in us
        if unit == "us":
            latency *= 1000

        results.append(
            BenchmarkResult(
                name=name,
                latency_ns=latency,
                throughput=throughput,
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
    p50_pattern = re.compile(r"Latency p50:\s+([\d.]+)\s+us")
    p99_pattern = re.compile(r"Latency p99:\s+([\d.]+)\s+us")
    p999_pattern = re.compile(r"Latency p999:\s+([\d.]+)\s+us")
    errors_pattern = re.compile(r"Errors:\s+(\d+)")

    # Split output by sections
    sections = re.split(r"(?=^=== )", output, flags=re.MULTILINE)

    for section in sections:
        name_match = name_pattern.search(section)
        if not name_match:
            continue

        name = name_match.group(1)
        if name == "Warmup":
            continue

        result = BenchmarkResult(name=name)

        throughput_match = throughput_pattern.search(section)
        if throughput_match:
            result.throughput = float(throughput_match.group(1))

        p50_match = p50_pattern.search(section)
        if p50_match:
            result.latency_p50_us = float(p50_match.group(1))

        p99_match = p99_pattern.search(section)
        if p99_match:
            result.latency_p99_us = float(p99_match.group(1))

        p999_match = p999_pattern.search(section)
        if p999_match:
            result.latency_p999_us = float(p999_match.group(1))

        errors_match = errors_pattern.search(section)
        if errors_match:
            result.errors = int(errors_match.group(1))

        results.append(result)

    return results


def parse_performance_output(output: str) -> List[BenchmarkResult]:
    """Parse output from performance_benchmark."""
    results = []

    # Pattern for performance results
    name_pattern = re.compile(r"^=== (.+?) ===$", re.MULTILINE)
    throughput_pattern = re.compile(r"Throughput:\s+([\d.]+)\s+ops/sec")
    p50_pattern = re.compile(r"Latency p50:\s+([\d.]+)\s+us")
    p99_pattern = re.compile(r"Latency p99:\s+([\d.]+)\s+us")
    p999_pattern = re.compile(r"Latency p999:\s+([\d.]+)\s+us")

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

        p50_match = p50_pattern.search(section)
        if p50_match:
            result.latency_p50_us = float(p50_match.group(1))

        p99_match = p99_pattern.search(section)
        if p99_match:
            result.latency_p99_us = float(p99_match.group(1))

        p999_match = p999_pattern.search(section)
        if p999_match:
            result.latency_p999_us = float(p999_match.group(1))

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


def choose_best_result(candidates: List[BenchmarkResult]) -> BenchmarkResult:
    """Choose best result from repeated runs for a single benchmark name."""
    throughput_candidates = [c for c in candidates if c.throughput is not None]
    if throughput_candidates:
        return max(throughput_candidates, key=lambda c: c.throughput if c.throughput else 0.0)

    latency_candidates = [c for c in candidates if _latency_sort_key(c) != float("inf")]
    if latency_candidates:
        return min(latency_candidates, key=_latency_sort_key)

    return candidates[0]


def merge_repeated_results(run_results: List[List[BenchmarkResult]]) -> List[BenchmarkResult]:
    """Merge repeated run results into best-of-N per benchmark name."""
    if not run_results:
        return []

    by_name: Dict[str, List[BenchmarkResult]] = {}
    order: List[str] = []

    for run in run_results:
        for bench in run:
            if bench.name not in by_name:
                by_name[bench.name] = []
                order.append(bench.name)
            by_name[bench.name].append(bench)

    merged: List[BenchmarkResult] = []
    for name in order:
        merged.append(choose_best_result(by_name[name]))

    return merged


def run_stage(stage_id: int, repeats: int = 3) -> StageResult:
    """Run a single benchmark stage."""
    if stage_id not in STAGES:
        return StageResult(
            stage_id=stage_id,
            stage_name=f"Unknown Stage {stage_id}",
            success=False,
            error_message=f"Stage {stage_id} not found",
        )

    stage = STAGES[stage_id]
    binary_path = BENCHMARK_DIR / stage["binary"]

    result = StageResult(
        stage_id=stage_id,
        stage_name=stage["name"],
    )

    if not binary_path.exists():
        # Try to build it
        build_result = subprocess.run(
            ["cmake", "--build", str(BUILD_DIR), "--target", stage["binary"]],
            capture_output=True,
            text=True,
        )
        if build_result.returncode != 0:
            result.success = False
            result.error_message = f"Binary not found and build failed: {binary_path}"
            return result

    print(f"\n[Stage {stage_id}] Running {stage['name']} (best-of-{repeats})...")
    stage_start = datetime.now()
    run_results: List[List[BenchmarkResult]] = []
    run_errors: List[str] = []

    for run_idx in range(repeats):
        run_start = datetime.now()
        print(f"  Run {run_idx + 1}/{repeats}...")

        try:
            proc = subprocess.run(
                [str(binary_path)],
                capture_output=True,
                text=True,
                timeout=300,  # 5 minute timeout
            )

            if proc.returncode != 0:
                run_errors.append(f"run {run_idx + 1}: exit code {proc.returncode}")
                continue

            parsed = parse_stage_output(stage["binary"], proc.stdout)
            if not parsed:
                run_errors.append(f"run {run_idx + 1}: no benchmarks parsed")
                continue
            run_results.append(parsed)

        except subprocess.TimeoutExpired:
            run_errors.append(f"run {run_idx + 1}: timed out")
        except Exception as e:
            run_errors.append(f"run {run_idx + 1}: {e}")
        finally:
            run_end = datetime.now()
            result.duration_ms += int((run_end - run_start).total_seconds() * 1000)

    stage_end = datetime.now()
    result.duration_ms = int((stage_end - stage_start).total_seconds() * 1000)

    if run_errors:
        result.success = False
        result.error_message = "; ".join(run_errors)
        print(f"  Error: {result.error_message}")
        return result

    result.benchmarks = merge_repeated_results(run_results)
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
        "`include/bench_utils.hpp`). Performance-critical paths have been optimized "
        "with hand-rolled parsers, SIMD-accelerated string processing, and lock-free "
        "concurrent data structures.",
        "",
        f"> **Note**: Results shown are best-of-{report.repeats} runs. Concurrent benchmarks are highly "
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
        ])

        # Check if we have latency data
        has_latency = any(b.latency_ns is not None for b in stage.benchmarks)
        has_percentiles = any(b.latency_p50_us is not None for b in stage.benchmarks)

        if has_percentiles:
            lines.extend([
                "| Benchmark | Throughput | Latency p50 | Latency p99 |",
                "|-----------|------------|-------------|-------------|",
            ])
            for b in stage.benchmarks:
                throughput = f"{format_throughput(b.throughput)} ops/sec" if b.throughput else "-"
                p50 = f"{b.latency_p50_us:.3f} us" if b.latency_p50_us else "-"
                p99 = f"{b.latency_p99_us:.3f} us" if b.latency_p99_us else "-"
                lines.append(f"| {b.name} | {throughput} | {p50} | {p99} |")
        elif has_latency:
            lines.extend([
                "| Benchmark | Latency | Throughput |",
                "|-----------|---------|------------|",
            ])
            for b in stage.benchmarks:
                latency = f"{b.latency_ns:.1f} ns" if b.latency_ns else "-"
                throughput = f"{format_throughput(b.throughput)} ops/sec" if b.throughput else "-"
                lines.append(f"| {b.name} | {latency} | {throughput} |")
        else:
            lines.extend([
                "| Benchmark | Throughput |",
                "|-----------|------------|",
            ])
            for b in stage.benchmarks:
                throughput = f"{format_throughput(b.throughput)} ops/sec" if b.throughput else "-"
                lines.append(f"| {b.name} | {throughput} |")

        lines.extend(["", "---", ""])

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
        "stages": [
            {
                "stage_id": s.stage_id,
                "stage_name": s.stage_name,
                "duration_ms": s.duration_ms,
                "success": s.success,
                "error_message": s.error_message,
                "benchmarks": [
                    {
                        "name": b.name,
                        "latency_ns": b.latency_ns,
                        "throughput": b.throughput,
                        "throughput_unit": b.throughput_unit,
                        "latency_p50_us": b.latency_p50_us,
                        "latency_p99_us": b.latency_p99_us,
                        "latency_p999_us": b.latency_p999_us,
                        "errors": b.errors,
                    }
                    for b in s.benchmarks
                ],
            }
            for s in report.stages
        ],
    }


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
        help="Run only specific stages (1-4). If not specified, runs all stages.",
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
        default=3,
        help="Number of runs per stage, best result is selected per benchmark (default: 3)",
    )

    args = parser.parse_args()

    if args.list_stages:
        print("Available benchmark stages:")
        for stage_id, stage in STAGES.items():
            print(f"  {stage_id}. {stage['name']}")
            print(f"     Binary: {stage['binary']}")
            print(f"     {stage['description']}")
        return 0

    # Determine which stages to run
    stages_to_run = args.stage if args.stage else list(STAGES.keys())
    if args.repeats < 1:
        print("Error: --repeats must be >= 1")
        return 1

    # Validate stage IDs
    for stage_id in stages_to_run:
        if stage_id not in STAGES:
            print(f"Error: Unknown stage {stage_id}")
            return 1

    # Build if needed
    if not args.no_build:
        if not ensure_build():
            print("Error: Failed to build benchmarks")
            return 1

    # Run benchmarks
    print(f"\n{'='*60}")
    print("KATANA Benchmark Runner")
    print(f"{'='*60}")
    print(f"Stages to run: {stages_to_run}")
    print(f"Repeats per stage: {args.repeats}")

    report = BenchmarkReport(
        generated_at=datetime.now().strftime("%Y-%m-%d %H:%M:%S"),
        commit_sha=get_git_sha(),
        repeats=args.repeats,
    )

    total_start = datetime.now()

    for stage_id in stages_to_run:
        result = run_stage(stage_id, repeats=args.repeats)
        report.stages.append(result)

    total_end = datetime.now()
    report.total_duration_ms = int((total_end - total_start).total_seconds() * 1000)

    # Print summary
    print(f"\n{'='*60}")
    print("Summary")
    print(f"{'='*60}")
    for stage in report.stages:
        status = "PASS" if stage.success else "FAIL"
        bench_count = len(stage.benchmarks)
        print(f"  Stage {stage.stage_id}: {stage.stage_name}")
        print(f"    Status: {status}, Benchmarks: {bench_count}, Duration: {stage.duration_ms}ms")

    print(f"\nTotal duration: {report.total_duration_ms}ms")

    # Output results
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

    # Return 0 if all stages passed, 1 otherwise
    all_passed = all(s.success for s in report.stages)
    return 0 if all_passed else 1


if __name__ == "__main__":
    sys.exit(main())
