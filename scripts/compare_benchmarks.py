#!/usr/bin/env python3
"""
Benchmark Comparison Tool for KATANA Framework

Compares benchmark results against a baseline to detect performance regressions.
Returns non-zero exit code if regression exceeds threshold.

Usage:
    ./scripts/compare_benchmarks.py --baseline benchmarks/baseline.json --current results.json
    ./scripts/compare_benchmarks.py --baseline benchmarks/baseline.json --current results.json --threshold 5

Exit codes:
    0 - No regression detected
    1 - Regression detected (performance drop > threshold)
    2 - Error (file not found, parse error, etc.)
"""

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Optional


@dataclass
class BenchmarkResult:
    """Single benchmark metric result."""
    name: str
    value: float
    unit: str
    lower_is_better: bool = True  # For latency metrics
    higher_is_better: bool = False  # For throughput metrics


@dataclass
class ComparisonResult:
    """Result of comparing baseline vs current."""
    name: str
    baseline: float
    current: float
    unit: str
    change_percent: float
    is_regression: bool
    is_improvement: bool


def load_json(path: Path) -> dict:
    """Load and parse JSON file."""
    try:
        with open(path, 'r') as f:
            return json.load(f)
    except FileNotFoundError:
        print(f"Error: File not found: {path}", file=sys.stderr)
        sys.exit(2)
    except json.JSONDecodeError as e:
        print(f"Error: Failed to parse JSON: {path}: {e}", file=sys.stderr)
        sys.exit(2)


def is_latency_metric(name: str) -> bool:
    """Check if metric is a latency metric (lower is better)."""
    latency_keywords = ['latency', 'p50', 'p90', 'p95', 'p99', 'p999', 'duration', 'time']
    name_lower = name.lower()
    return any(kw in name_lower for kw in latency_keywords)


def is_throughput_metric(name: str) -> bool:
    """Check if metric is a throughput metric (higher is better)."""
    throughput_keywords = ['throughput', 'ops/sec', 'req/s', 'operations', 'rate']
    name_lower = name.lower()
    return any(kw in name_lower for kw in throughput_keywords)


def compare_metrics(
    baseline: dict,
    current: dict,
    threshold: float
) -> tuple[list[ComparisonResult], bool]:
    """
    Compare baseline and current benchmark results.

    Returns:
        Tuple of (comparison_results, has_regression)
    """
    results = []
    has_regression = False

    # Find common metrics
    baseline_metrics = set(baseline.keys())
    current_metrics = set(current.keys())
    common_metrics = baseline_metrics & current_metrics

    for metric_name in sorted(common_metrics):
        baseline_data = baseline[metric_name]
        current_data = current[metric_name]

        # Handle both simple values and structured data
        if isinstance(baseline_data, dict):
            baseline_value = baseline_data.get('value', baseline_data.get('mean', 0))
            unit = baseline_data.get('unit', '')
        else:
            baseline_value = float(baseline_data)
            unit = ''

        if isinstance(current_data, dict):
            current_value = current_data.get('value', current_data.get('mean', 0))
        else:
            current_value = float(current_data)

        # Calculate change
        if baseline_value == 0:
            change_percent = 0 if current_value == 0 else float('inf')
        else:
            change_percent = ((current_value - baseline_value) / baseline_value) * 100

        # Determine if this is a regression
        is_regression = False
        is_improvement = False

        if is_latency_metric(metric_name):
            # For latency, increase is bad
            if change_percent > threshold:
                is_regression = True
                has_regression = True
            elif change_percent < -threshold:
                is_improvement = True
        elif is_throughput_metric(metric_name):
            # For throughput, decrease is bad
            if change_percent < -threshold:
                is_regression = True
                has_regression = True
            elif change_percent > threshold:
                is_improvement = True
        else:
            # Default: assume lower is better
            if change_percent > threshold:
                is_regression = True
                has_regression = True

        results.append(ComparisonResult(
            name=metric_name,
            baseline=baseline_value,
            current=current_value,
            unit=unit,
            change_percent=change_percent,
            is_regression=is_regression,
            is_improvement=is_improvement
        ))

    return results, has_regression


def print_results(results: list[ComparisonResult], threshold: float) -> None:
    """Print comparison results in a formatted table."""
    print("\n" + "=" * 80)
    print("BENCHMARK COMPARISON RESULTS")
    print("=" * 80)
    print(f"Regression threshold: {threshold}%")
    print("-" * 80)
    print(f"{'Metric':<45} {'Baseline':>12} {'Current':>12} {'Change':>10}")
    print("-" * 80)

    regressions = []
    improvements = []

    for r in results:
        # Format change with color indicators
        if r.is_regression:
            change_str = f"⚠️  {r.change_percent:+.2f}%"
            regressions.append(r)
        elif r.is_improvement:
            change_str = f"✅ {r.change_percent:+.2f}%"
            improvements.append(r)
        else:
            change_str = f"   {r.change_percent:+.2f}%"

        unit_suffix = f" {r.unit}" if r.unit else ""
        print(f"{r.name:<45} {r.baseline:>12.3f}{unit_suffix} {r.current:>12.3f} {change_str}")

    print("-" * 80)

    # Summary
    print("\nSUMMARY:")
    print(f"  Total metrics compared: {len(results)}")
    print(f"  Improvements: {len(improvements)}")
    print(f"  Regressions: {len(regressions)}")

    if regressions:
        print("\n⚠️  REGRESSIONS DETECTED:")
        for r in regressions:
            print(f"  - {r.name}: {r.change_percent:+.2f}%")

    if improvements:
        print("\n✅ IMPROVEMENTS:")
        for r in improvements:
            print(f"  - {r.name}: {r.change_percent:+.2f}%")

    print("=" * 80)


def generate_json_report(
    results: list[ComparisonResult],
    has_regression: bool,
    threshold: float,
    output_path: Optional[Path] = None
) -> dict:
    """Generate JSON report of comparison results."""
    report = {
        "threshold_percent": threshold,
        "has_regression": has_regression,
        "summary": {
            "total_metrics": len(results),
            "regressions": sum(1 for r in results if r.is_regression),
            "improvements": sum(1 for r in results if r.is_improvement),
            "unchanged": sum(1 for r in results if not r.is_regression and not r.is_improvement)
        },
        "comparisons": [
            {
                "name": r.name,
                "baseline": r.baseline,
                "current": r.current,
                "unit": r.unit,
                "change_percent": round(r.change_percent, 3),
                "is_regression": r.is_regression,
                "is_improvement": r.is_improvement
            }
            for r in results
        ]
    }

    if output_path:
        with open(output_path, 'w') as f:
            json.dump(report, f, indent=2)
        print(f"\nReport saved to: {output_path}")

    return report


def main():
    parser = argparse.ArgumentParser(
        description="Compare benchmark results against baseline",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog=__doc__
    )
    parser.add_argument(
        '--baseline', '-b',
        type=Path,
        required=True,
        help='Path to baseline benchmark results JSON'
    )
    parser.add_argument(
        '--current', '-c',
        type=Path,
        required=True,
        help='Path to current benchmark results JSON'
    )
    parser.add_argument(
        '--threshold', '-t',
        type=float,
        default=3.0,
        help='Regression threshold percentage (default: 3.0)'
    )
    parser.add_argument(
        '--output', '-o',
        type=Path,
        help='Output JSON report path'
    )
    parser.add_argument(
        '--quiet', '-q',
        action='store_true',
        help='Only print errors and summary'
    )
    parser.add_argument(
        '--fail-on-regression',
        action='store_true',
        default=True,
        help='Exit with code 1 if regression detected (default: True)'
    )
    parser.add_argument(
        '--no-fail',
        action='store_true',
        help='Do not fail on regression (always exit 0)'
    )

    args = parser.parse_args()

    # Load data
    baseline = load_json(args.baseline)
    current = load_json(args.current)

    # Compare
    results, has_regression = compare_metrics(baseline, current, args.threshold)

    if not results:
        print("Warning: No common metrics found to compare", file=sys.stderr)
        sys.exit(2)

    # Print results
    if not args.quiet:
        print_results(results, args.threshold)

    # Generate report if requested
    if args.output:
        generate_json_report(results, has_regression, args.threshold, args.output)

    # Exit code
    if has_regression and args.fail_on_regression and not args.no_fail:
        print("\n❌ Performance regression detected!")
        sys.exit(1)
    else:
        if not has_regression:
            print("\n✅ No performance regression detected")
        sys.exit(0)


if __name__ == '__main__':
    main()
