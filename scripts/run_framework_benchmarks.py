#!/usr/bin/env python3
"""
Run the maintained wrk scenarios against arbitrary HTTP targets.

The targets are expected to expose the same hot paths as the current KATANA E2E
benchmarks:

- GET /
- POST /compute/sum with a JSON array body

This runner intentionally reuses the wrk Lua scripts and wrk-output parsing
logic from scripts/run_benchmarks.py so the comparison format stays aligned.
"""

from __future__ import annotations

import argparse
import json
import os
import shutil
import subprocess
import sys
import time
from dataclasses import dataclass
from datetime import datetime
from pathlib import Path
from typing import Any, Dict, List, Optional
from urllib.parse import urlsplit, urlunsplit

SCRIPT_DIR = Path(__file__).resolve().parent
REPO_ROOT = SCRIPT_DIR.parent
if str(SCRIPT_DIR) not in sys.path:
    sys.path.insert(0, str(SCRIPT_DIR))

import run_benchmarks as katana_bench


SCENARIOS: Dict[str, Dict[str, Any]] = {
    "hello-keepalive": {
        "name": "Hello World Keep-Alive",
        "benchmark_name": "wrk hello_world GET / depth1",
        "wrk_script": "test/load/scripts/hello_pipeline.lua",
        "wrk_threads": 4,
        "wrk_connections": 512,
        "wrk_duration_sec": 10,
        "pipeline_depth": "1",
        "mode": "keepalive",
    },
    "compute-keepalive": {
        "name": "Compute API Keep-Alive",
        "benchmark_name": "wrk compute_api POST /compute/sum depth1",
        "wrk_script": "test/load/scripts/compute_sum_pipeline.lua",
        "wrk_threads": 4,
        "wrk_connections": 512,
        "wrk_duration_sec": 10,
        "pipeline_depth": "1",
        "mode": "keepalive",
    },
    "hello-canonical": {
        "name": "Hello World Canonical Pipeline",
        "benchmark_name": "wrk hello_world GET / depth10",
        "wrk_script": "test/load/scripts/hello_pipeline.lua",
        "wrk_threads": 4,
        "wrk_connections": 512,
        "wrk_duration_sec": 10,
        "pipeline_depth": "10",
        "mode": "pipeline",
    },
    "compute-canonical": {
        "name": "Compute API Canonical Pipeline",
        "benchmark_name": "wrk compute_api POST /compute/sum depth10",
        "wrk_script": "test/load/scripts/compute_sum_pipeline.lua",
        "wrk_threads": 4,
        "wrk_connections": 512,
        "wrk_duration_sec": 10,
        "pipeline_depth": "10",
        "mode": "pipeline",
    },
    "hello-peak": {
        "name": "Hello World Peak Pipeline",
        "benchmark_name": "wrk hello_world GET / depth20",
        "wrk_script": "test/load/scripts/hello_pipeline.lua",
        "wrk_threads": 4,
        "wrk_connections": 512,
        "wrk_duration_sec": 5,
        "pipeline_depth": "20",
        "mode": "pipeline",
    },
    "compute-peak": {
        "name": "Compute API Peak Pipeline",
        "benchmark_name": "wrk compute_api POST /compute/sum depth40",
        "wrk_script": "test/load/scripts/compute_sum_pipeline.lua",
        "wrk_threads": 4,
        "wrk_connections": 512,
        "wrk_duration_sec": 5,
        "pipeline_depth": "40",
        "mode": "pipeline",
    },
}


@dataclass(frozen=True)
class TargetConfig:
    name: str
    base_url: Optional[str]
    scenario_urls: Dict[str, str]


def normalize_base_url(raw: str) -> str:
    value = raw.strip()
    if not value:
        raise ValueError("target URL must not be empty")

    parsed = urlsplit(value)
    if parsed.scheme not in {"http", "https"} or not parsed.netloc:
        raise ValueError(f"invalid target URL: {raw}")
    if parsed.query or parsed.fragment:
        raise ValueError(f"target URL must not include query or fragment: {raw}")

    path = parsed.path or "/"
    return urlunsplit((parsed.scheme, parsed.netloc, path, "", ""))


def parse_target_arg(raw: str) -> TargetConfig:
    if "=" not in raw:
        raise ValueError(f"invalid --target value '{raw}', expected name=url")
    name, url = raw.split("=", 1)
    name = name.strip()
    if not name:
        raise ValueError(f"invalid --target value '{raw}', target name is empty")
    return TargetConfig(name=name, base_url=normalize_base_url(url), scenario_urls={})


def load_targets_file(path: Path) -> List[TargetConfig]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    items = payload.get("targets")
    if not isinstance(items, list):
        raise ValueError("targets file must contain a top-level 'targets' array")

    targets: List[TargetConfig] = []
    for item in items:
        if not isinstance(item, dict):
            raise ValueError("every target entry must be an object")
        name = str(item.get("name") or "").strip()
        if not name:
            raise ValueError("target entry is missing a non-empty 'name'")

        raw_base_url = item.get("base_url")
        base_url = normalize_base_url(raw_base_url) if raw_base_url else None

        raw_scenario_urls = item.get("scenario_urls", {})
        if not isinstance(raw_scenario_urls, dict):
            raise ValueError(f"target '{name}' has non-object scenario_urls")

        scenario_urls: Dict[str, str] = {}
        for scenario_id, url in raw_scenario_urls.items():
            if scenario_id not in SCENARIOS:
                raise ValueError(f"target '{name}' references unknown scenario '{scenario_id}'")
            scenario_urls[scenario_id] = normalize_base_url(str(url))

        if base_url is None and not scenario_urls:
            raise ValueError(f"target '{name}' must define base_url or scenario_urls")

        targets.append(TargetConfig(name=name, base_url=base_url, scenario_urls=scenario_urls))

    return targets


def collect_targets(args: argparse.Namespace) -> List[TargetConfig]:
    targets: List[TargetConfig] = []

    if args.targets_file:
        targets.extend(load_targets_file(args.targets_file))
    for raw in args.target or []:
        targets.append(parse_target_arg(raw))

    if not targets:
        raise ValueError("no targets configured; use --target or --targets-file")

    seen: set[str] = set()
    for target in targets:
        if target.name in seen:
            raise ValueError(f"duplicate target name '{target.name}'")
        seen.add(target.name)
    return targets


def resolve_target_url(target: TargetConfig, scenario_id: str) -> str:
    if scenario_id in target.scenario_urls:
        return target.scenario_urls[scenario_id]
    if target.base_url:
        return target.base_url
    raise ValueError(
        f"target '{target.name}' has no URL for scenario '{scenario_id}'"
    )


def benchmark_to_dict(benchmark: katana_bench.BenchmarkResult) -> Dict[str, Any]:
    return {
        "name": benchmark.name,
        "latency_ns": benchmark.latency_ns,
        "throughput": benchmark.throughput,
        "throughput_unit": benchmark.throughput_unit,
        "bytes_per_sec": benchmark.bytes_per_sec,
        "operations": benchmark.operations,
        "avg_latency_us": benchmark.avg_latency_us,
        "latency_p50_us": benchmark.latency_p50_us,
        "latency_p95_us": benchmark.latency_p95_us,
        "latency_p99_us": benchmark.latency_p99_us,
        "latency_p999_us": benchmark.latency_p999_us,
        "latency_max_us": benchmark.latency_max_us,
        "tail_p95_ns": benchmark.tail_p95_ns,
        "tail_p99_ns": benchmark.tail_p99_ns,
        "producer_retries": benchmark.producer_retries,
        "consumer_retries": benchmark.consumer_retries,
        "retries_per_op_push": benchmark.retries_per_op_push,
        "retries_per_op_pop": benchmark.retries_per_op_pop,
        "retries_per_op_total": benchmark.retries_per_op_total,
        "thread_producers": benchmark.thread_producers,
        "thread_consumers": benchmark.thread_consumers,
        "thread_hw": benchmark.thread_hw,
        "thread_oversubscription": benchmark.thread_oversubscription,
        "errors": benchmark.errors,
    }


def run_wrk_once(
    *,
    wrk_binary: str,
    target: TargetConfig,
    scenario_id: str,
    scenario: Dict[str, Any],
) -> tuple[Optional[katana_bench.BenchmarkResult], Optional[str]]:
    url = resolve_target_url(target, scenario_id)
    script_path = REPO_ROOT / scenario["wrk_script"]

    env = dict(os.environ)
    env["KATANA_PIPELINE_DEPTH"] = scenario["pipeline_depth"]

    cmd = [
        wrk_binary,
        "-t",
        str(scenario["wrk_threads"]),
        "-c",
        str(scenario["wrk_connections"]),
        "-d",
        f"{scenario['wrk_duration_sec']}s",
        "--latency",
        "-s",
        str(script_path),
        url,
    ]
    proc = subprocess.run(
        cmd,
        capture_output=True,
        text=True,
        timeout=max(30, int(scenario["wrk_duration_sec"]) + 15),
        env=env,
    )
    if proc.returncode != 0:
        details = proc.stderr.strip() or proc.stdout.strip() or "unknown wrk failure"
        return None, f"wrk failed: {details}"

    benchmark_name = f"{target.name} :: {scenario['benchmark_name']}"
    parsed = katana_bench._parse_wrk_output(proc.stdout, benchmark_name)
    if parsed is None:
        return None, "failed to parse wrk output"
    return parsed, None


def run_target_scenario(
    *,
    wrk_binary: str,
    target: TargetConfig,
    scenario_id: str,
    scenario: Dict[str, Any],
    repeats: int,
    aggregation_mode: str,
    include_runs: bool,
    warmup_runs: int,
) -> Dict[str, Any]:
    started = time.perf_counter()
    run_results: List[List[katana_bench.BenchmarkResult]] = []
    run_errors: List[str] = []

    try:
        url = resolve_target_url(target, scenario_id)
    except ValueError as exc:
        return {
            "target": target.name,
            "scenario_id": scenario_id,
            "scenario_name": scenario["name"],
            "url": None,
            "config": {
                "wrk_threads": scenario["wrk_threads"],
                "wrk_connections": scenario["wrk_connections"],
                "wrk_duration_sec": scenario["wrk_duration_sec"],
                "wrk_script": scenario["wrk_script"],
                "pipeline_depth": scenario["pipeline_depth"],
                "mode": scenario["mode"],
            },
            "run_count": 0,
            "duration_ms": 0,
            "success": False,
            "error_message": str(exc),
            "benchmark": None,
            "benchmark_stats": {},
            "benchmark_runs": [],
        }

    print(f"\n[{target.name}] {scenario_id} ({aggregation_mode}-of-{repeats}, url={url})")
    for warmup_idx in range(warmup_runs):
        print(f"  Warmup {warmup_idx + 1}/{warmup_runs}...")
        _benchmark, error = run_wrk_once(
            wrk_binary=wrk_binary,
            target=target,
            scenario_id=scenario_id,
            scenario=scenario,
        )
        if error:
            run_errors.append(f"warmup {warmup_idx + 1}: {error}")
            break

    if not run_errors:
        for run_idx in range(repeats):
            print(f"  Run {run_idx + 1}/{repeats}...")
            benchmark, error = run_wrk_once(
                wrk_binary=wrk_binary,
                target=target,
                scenario_id=scenario_id,
                scenario=scenario,
            )
            if error:
                run_errors.append(f"run {run_idx + 1}: {error}")
                continue
            assert benchmark is not None
            run_results.append([benchmark])

    duration_ms = int((time.perf_counter() - started) * 1000)
    report_entry: Dict[str, Any] = {
        "target": target.name,
        "scenario_id": scenario_id,
        "scenario_name": scenario["name"],
        "url": url,
        "config": {
            "wrk_threads": scenario["wrk_threads"],
            "wrk_connections": scenario["wrk_connections"],
            "wrk_duration_sec": scenario["wrk_duration_sec"],
            "wrk_script": scenario["wrk_script"],
            "pipeline_depth": scenario["pipeline_depth"],
            "mode": scenario["mode"],
        },
        "run_count": len(run_results),
        "duration_ms": duration_ms,
        "success": not run_errors,
        "error_message": "; ".join(run_errors) if run_errors else None,
        "benchmark": None,
        "benchmark_stats": {},
        "benchmark_runs": [],
    }

    if run_errors:
        print(f"  Error: {report_entry['error_message']}")
        return report_entry

    merged, stats_by_name, runs_by_name = katana_bench.merge_repeated_results(
        run_results,
        aggregation_mode=aggregation_mode,
        include_runs=include_runs,
    )
    benchmark = merged[0]
    report_entry["benchmark"] = benchmark_to_dict(benchmark)
    report_entry["benchmark_stats"] = stats_by_name.get(benchmark.name, {})
    report_entry["benchmark_runs"] = runs_by_name.get(benchmark.name, [])
    print(
        "  Completed: "
        f"{benchmark.throughput:.2f} req/sec, p95={benchmark.latency_p95_us or 0.0:.3f} us"
    )
    return report_entry


def generate_markdown(report: Dict[str, Any]) -> str:
    lines = [
        "# Framework Benchmark Comparison",
        "",
        f"> Generated at: {report['generated_at']}",
        f"> Repeats: {report['repeats']}",
        f"> Aggregation: {report['aggregation_mode']}",
        f"> Warmup runs: {report['warmup_runs']}",
        f"> wrk binary: {report['wrk_binary']}",
        "",
        "This report reuses the same wrk Lua scripts and wrk-output parser as",
        "`scripts/run_benchmarks.py`.",
        "",
        "Keep-alive scenarios use the shared Lua scripts with `KATANA_PIPELINE_DEPTH=1`.",
        "Pipeline scenarios intentionally batch multiple HTTP/1.1 requests per write and",
        "should be read as a pipelining stress test, not a generic API ranking.",
        "",
    ]

    lines.extend(
        [
            "## Targets",
            "",
            "| Target | Base URL | Scenario-specific URLs |",
            "|--------|----------|------------------------|",
        ]
    )
    for target in report["targets"]:
        lines.append(
            f"| {target['name']} | {target.get('base_url') or '-'} | "
            f"{', '.join(sorted(target.get('scenario_urls', {}).keys())) or '-'} |"
        )
    lines.append("")

    for scenario_id in report["scenario_order"]:
        scenario_name = SCENARIOS[scenario_id]["name"]
        lines.extend(
            [
                f"## {scenario_name}",
                "",
                "| Target | URL | Mode | Depth | Throughput | Avg us | P50 us | P95 us | P99 us | Errors |",
                "|--------|-----|------|-------|------------|--------|--------|--------|--------|--------|",
            ]
        )
        rows = [
            item
            for item in report["results"]
            if item["scenario_id"] == scenario_id and item["success"] and item["benchmark"]
        ]
        rows.sort(
            key=lambda item: float(item["benchmark"].get("throughput") or 0.0),
            reverse=True,
        )
        for item in rows:
            bench = item["benchmark"]
            lines.append(
                f"| {item['target']} | {item['url']} | "
                f"{item['config'].get('mode', '-')} | "
                f"{item['config'].get('pipeline_depth', '-')} | "
                f"{bench.get('throughput', 0.0):.2f} req/sec | "
                f"{(bench.get('avg_latency_us') or 0.0):.3f} | "
                f"{(bench.get('latency_p50_us') or 0.0):.3f} | "
                f"{(bench.get('latency_p95_us') or 0.0):.3f} | "
                f"{(bench.get('latency_p99_us') or 0.0):.3f} | "
                f"{bench.get('errors', 0)} |"
            )
        if not rows:
            lines.append("| - | - | - | - | - | - | - | - | - | - |")
        lines.append("")

        failed = [
            item
            for item in report["results"]
            if item["scenario_id"] == scenario_id and not item["success"]
        ]
        if failed:
            lines.extend(["### Failures", ""])
            for item in failed:
                lines.append(f"- `{item['target']}`: {item['error_message']}")
            lines.append("")

    return "\n".join(lines)


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Benchmark arbitrary HTTP targets with the maintained wrk scenarios."
    )
    parser.add_argument(
        "--target",
        action="append",
        help="Inline target definition in the form name=http://host:port/ . Can be passed multiple times.",
    )
    parser.add_argument(
        "--targets-file",
        type=Path,
        help="JSON file describing one or more targets. See comparisons/http_frameworks/targets.example.json.",
    )
    parser.add_argument(
        "--scenario",
        nargs="*",
        default=None,
        help=f"Subset of scenarios to run. Available: {', '.join(SCENARIOS.keys())}",
    )
    parser.add_argument(
        "--repeats",
        type=int,
        default=3,
        help="Runs per target/scenario pair (default: 3).",
    )
    parser.add_argument(
        "--aggregation",
        choices=["best", "median", "mean"],
        default="median",
        help="Aggregation mode for repeated runs (default: median).",
    )
    parser.add_argument(
        "--include-runs",
        action="store_true",
        help="Include raw per-run values in the JSON output.",
    )
    parser.add_argument(
        "--warmup-runs",
        type=int,
        default=1,
        help="Unreported warmup runs before each target/scenario pair (default: 1).",
    )
    parser.add_argument(
        "--output",
        type=Path,
        default=REPO_ROOT / "benchmark_results" / "framework_comparison",
        help="Output directory for JSON and Markdown reports.",
    )
    parser.add_argument(
        "--json",
        action="store_true",
        help="Print JSON report to stdout as well.",
    )
    args = parser.parse_args()

    if args.repeats < 1:
        print("Error: --repeats must be >= 1")
        return 1
    if args.warmup_runs < 0:
        print("Error: --warmup-runs must be >= 0")
        return 1

    wrk_binary = shutil.which(os.environ.get("KATANA_WRK_BIN", "wrk"))
    if not wrk_binary:
        print("Error: wrk not found in PATH")
        return 1

    try:
        targets = collect_targets(args)
    except ValueError as exc:
        print(f"Error: {exc}")
        return 1

    scenario_order = args.scenario or list(SCENARIOS.keys())
    for scenario_id in scenario_order:
        if scenario_id not in SCENARIOS:
            print(f"Error: unknown scenario '{scenario_id}'")
            return 1

    results: List[Dict[str, Any]] = []
    started = time.perf_counter()
    for target in targets:
        for scenario_id in scenario_order:
            results.append(
                run_target_scenario(
                    wrk_binary=wrk_binary,
                    target=target,
                    scenario_id=scenario_id,
                    scenario=SCENARIOS[scenario_id],
                    repeats=args.repeats,
                    aggregation_mode=args.aggregation,
                    include_runs=args.include_runs,
                    warmup_runs=args.warmup_runs,
                )
            )

    report = {
        "generated_at": datetime.now().strftime("%Y-%m-%d %H:%M:%S"),
        "repeats": args.repeats,
        "aggregation_mode": args.aggregation,
        "warmup_runs": args.warmup_runs,
        "wrk_binary": wrk_binary,
        "total_duration_ms": int((time.perf_counter() - started) * 1000),
        "scenario_order": scenario_order,
        "scenarios": {scenario_id: SCENARIOS[scenario_id] for scenario_id in scenario_order},
        "targets": [
            {
                "name": target.name,
                "base_url": target.base_url,
                "scenario_urls": target.scenario_urls,
            }
            for target in targets
        ],
        "results": results,
    }

    args.output.mkdir(parents=True, exist_ok=True)
    json_path = args.output / "framework_comparison.json"
    md_path = args.output / "FRAMEWORK_COMPARISON.md"
    json_path.write_text(json.dumps(report, indent=2), encoding="utf-8")
    md_path.write_text(generate_markdown(report), encoding="utf-8")

    print(f"\nJSON report: {json_path}")
    print(f"Markdown report: {md_path}")
    print(f"Total duration: {report['total_duration_ms']}ms")

    if args.json:
        print(json.dumps(report, indent=2))

    return 0 if all(item["success"] for item in results) else 1


if __name__ == "__main__":
    raise SystemExit(main())
