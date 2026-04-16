#!/usr/bin/env python3
"""
Build and benchmark the Stage 4 SQL comparison targets sequentially.

This runner keeps only one framework process alive at a time to reduce cross-run
noise and reuses the shared wrk scenarios already maintained in the repository.
"""

from __future__ import annotations

import argparse
import contextlib
import json
import os
import shutil
import socket
import subprocess
import sys
import time
import urllib.request
from dataclasses import dataclass
from datetime import datetime
from pathlib import Path
from typing import Any, Dict, List, Optional

SCRIPT_DIR = Path(__file__).resolve().parent
REPO_ROOT = SCRIPT_DIR.parent
if str(SCRIPT_DIR) not in sys.path:
    sys.path.insert(0, str(SCRIPT_DIR))

import run_benchmarks as katana_bench
import run_framework_benchmarks as framework_bench


@dataclass(frozen=True)
class FrameworkConfig:
    name: str
    port: int
    workdir: Path
    build_steps: List[List[str]]
    command: List[str]
    env: Dict[str, str]


DEFAULT_FRAMEWORKS = ("katana", "actix-web", "axum", "drogon", "ntex", "fastapi")
DEFAULT_SCENARIOS = ("sql-read-heavy", "sql-mixed-crud")
DEFAULT_SEED_COUNT = 4096
SQL_CLEANUP = [
    "ALTER TABLE katana_stage4_items RESET (autovacuum_enabled, toast.autovacuum_enabled)",
    "TRUNCATE katana_stage4_items RESTART IDENTITY",
]


def discover_katana_build_dir() -> Path:
    build_dir = katana_bench.BUILD_DIR
    if not (build_dir / "CMakeCache.txt").exists():
        raise FileNotFoundError(f"KATANA build directory not found: {build_dir}")
    return build_dir


def resolve_postgres_dsn(cli_value: Optional[str]) -> str:
    if cli_value:
        return cli_value
    for key in ("KATANA_BENCHMARK_API_POSTGRES_DSN", "KATANA_TEST_POSTGRES_DSN"):
        value = os.environ.get(key)
        if value:
            return value
    raise ValueError(
        "PostgreSQL DSN is required. Use --postgres-dsn or set "
        "KATANA_BENCHMARK_API_POSTGRES_DSN/KATANA_TEST_POSTGRES_DSN."
    )


def framework_configs(
    *,
    workers: int,
    postgres_dsn: str,
    seed_count: int,
    jobs: int,
) -> Dict[str, FrameworkConfig]:
    katana_build_dir = discover_katana_build_dir()
    katana_binary = katana_build_dir / "benchmark" / "benchmark_api_sql_server"

    shared_env = {
        "PORT": "{port}",
        "BENCH_WORKERS": str(workers),
        "KATANA_BENCHMARK_API_POSTGRES_DSN": postgres_dsn,
        "KATANA_BENCHMARK_API_BOOTSTRAP": "1",
        "KATANA_BENCHMARK_API_RESET": "1",
        "KATANA_BENCHMARK_API_DISABLE_AUTOVACUUM": "1",
        "KATANA_BENCHMARK_API_SEED_COUNT": str(seed_count),
        "KATANA_BENCHMARK_API_WORKERS": str(workers),
        "KATANA_BENCHMARK_API_EXECUTORS": str(workers),
    }

    comparison_root = REPO_ROOT / "comparisons" / "http_frameworks"
    return {
        "katana": FrameworkConfig(
            name="katana",
            port=19090,
            workdir=REPO_ROOT,
            build_steps=[
                [
                    "cmake",
                    "--build",
                    str(katana_build_dir),
                    "--target",
                    "benchmark_api_sql_server",
                    "-j",
                    str(jobs),
                ]
            ],
            command=[str(katana_binary)],
            env=shared_env,
        ),
        "actix-web": FrameworkConfig(
            name="actix-web",
            port=19080,
            workdir=comparison_root / "actix_web",
            build_steps=[["cargo", "build", "--release"]],
            command=["./target/release/katana_comparison_actix"],
            env=shared_env,
        ),
        "axum": FrameworkConfig(
            name="axum",
            port=19091,
            workdir=comparison_root / "axum",
            build_steps=[["cargo", "build", "--release"]],
            command=["./target/release/katana_comparison_axum"],
            env=shared_env,
        ),
        "drogon": FrameworkConfig(
            name="drogon",
            port=19092,
            workdir=comparison_root / "drogon",
            build_steps=[
                ["cmake", "-S", ".", "-B", "build", "-DCMAKE_BUILD_TYPE=Release"],
                ["cmake", "--build", "build", "-j", str(jobs)],
            ],
            command=["./build/katana_comparison_drogon"],
            env=shared_env,
        ),
        "ntex": FrameworkConfig(
            name="ntex",
            port=19084,
            workdir=comparison_root / "ntex",
            build_steps=[["cargo", "build", "--release"]],
            command=["./target/release/katana_comparison_ntex"],
            env=shared_env,
        ),
        "fastapi": FrameworkConfig(
            name="fastapi",
            port=19093,
            workdir=comparison_root / "fastapi",
            build_steps=[
                ["python3", "-m", "venv", ".venv"],
                [
                    "./.venv/bin/pip",
                    "install",
                    "-r",
                    "requirements.txt",
                ],
            ],
            command=["./.venv/bin/python3", "app.py"],
            env={**shared_env, "PYTHONUNBUFFERED": "1"},
        ),
    }


def run_command(cmd: List[str], *, cwd: Path, env: Optional[Dict[str, str]] = None) -> None:
    print(f"[build] {' '.join(cmd)}")
    subprocess.run(cmd, cwd=cwd, check=True, env=env)


def port_is_open(host: str, port: int) -> bool:
    with contextlib.suppress(OSError):
        with socket.create_connection((host, port), timeout=0.2):
            return True
    return False


def wait_for_health(url: str, timeout_s: float) -> bool:
    deadline = time.time() + timeout_s
    while time.time() < deadline:
        try:
            with urllib.request.urlopen(url, timeout=0.5) as response:
                if 200 <= response.status < 300:
                    return True
        except Exception:
            time.sleep(0.1)
    return False


def start_server(
    config: FrameworkConfig,
    *,
    port: int,
    logs_dir: Path,
) -> subprocess.Popen[str]:
    env = dict(os.environ)
    for key, value in config.env.items():
        env[key] = value.format(port=port)

    if port_is_open("127.0.0.1", port):
        raise RuntimeError(f"port {port} is already in use before starting {config.name}")

    log_path = logs_dir / f"{config.name}.log"
    log_file = log_path.open("w", encoding="utf-8")
    try:
        proc = subprocess.Popen(
            config.command,
            cwd=config.workdir,
            env=env,
            stdout=log_file,
            stderr=subprocess.STDOUT,
            text=True,
        )
    except Exception:
        log_file.close()
        raise

    if wait_for_health(f"http://127.0.0.1:{port}/health", timeout_s=15.0):
        return proc

    proc.terminate()
    with contextlib.suppress(Exception):
        proc.wait(timeout=5)
    log_file.close()
    raise RuntimeError(f"{config.name} failed to become ready on port {port}")


def stop_server(proc: Optional[subprocess.Popen[str]]) -> None:
    if proc is None:
        return
    with contextlib.suppress(Exception):
        proc.terminate()
        proc.wait(timeout=5)
        return
    with contextlib.suppress(Exception):
        proc.kill()
        proc.wait(timeout=2)


def cleanup_sql(postgres_dsn: str, env: Dict[str, str]) -> Optional[str]:
    psql_binary = shutil.which("psql")
    if not psql_binary:
        return "psql not found in PATH"
    for sql in SQL_CLEANUP:
        proc = subprocess.run(
            [psql_binary, postgres_dsn, "-v", "ON_ERROR_STOP=1", "-Atc", sql],
            capture_output=True,
            text=True,
            timeout=15,
            env=env,
        )
        if proc.returncode != 0:
            return proc.stderr.strip() or proc.stdout.strip() or "unknown psql cleanup failure"
    return None


def generate_markdown(report: Dict[str, Any]) -> str:
    lines = [
        "# Stage 4 SQL Framework Matrix",
        "",
        f"> Generated at: {report['generated_at']}",
        f"> Workers: {report['workers']}",
        f"> Repeats: {report['repeats']}",
        f"> Aggregation: {report['aggregation_mode']}",
        f"> Warmup runs: {report['warmup_runs']}",
        f"> Inter-run cooldown: {report['inter_run_cooldown_sec']}",
        f"> Seed count: {report['seed_count']}",
        f"> Cooldown seconds: {report['cooldown_sec']}",
        "",
        "Targets were built and benchmarked sequentially. Only one framework",
        "server was alive during each measurement window.",
        "Each scenario restarts its server and SQL fixture to reduce cross-scenario noise.",
        "",
    ]

    for scenario_id in report["scenario_order"]:
        scenario = framework_bench.SCENARIOS[scenario_id]
        lines.extend(
            [
                f"## {scenario['name']}",
                "",
                "| Target | Throughput | Avg us | P50 us | P95 us | P99 us | Errors |",
                "|--------|------------|--------|--------|--------|--------|--------|",
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
                f"| {item['target']} | "
                f"{bench.get('throughput', 0.0):.2f} req/sec | "
                f"{(bench.get('avg_latency_us') or 0.0):.3f} | "
                f"{(bench.get('latency_p50_us') or 0.0):.3f} | "
                f"{(bench.get('latency_p95_us') or 0.0):.3f} | "
                f"{(bench.get('latency_p99_us') or 0.0):.3f} | "
                f"{bench.get('errors', 0)} |"
            )
        if not rows:
            lines.append("| - | - | - | - | - | - | - |")
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
        description=(
            "Sequential Stage 4 SQL benchmark matrix for Katana, Actix Web, Axum, Drogon, "
            "ntex and FastAPI."
        )
    )
    parser.add_argument(
        "--framework",
        action="append",
        choices=list(DEFAULT_FRAMEWORKS),
        help="Subset of frameworks to run. Can be passed multiple times.",
    )
    parser.add_argument(
        "--scenario",
        action="append",
        choices=list(DEFAULT_SCENARIOS),
        help="Subset of Stage 4 scenarios to run. Can be passed multiple times.",
    )
    parser.add_argument("--postgres-dsn", help="PostgreSQL DSN for the Stage 4 fixture.")
    parser.add_argument("--workers", type=int, default=4, help="Worker count for every target.")
    parser.add_argument("--seed-count", type=int, default=DEFAULT_SEED_COUNT)
    parser.add_argument("--repeats", type=int, default=3)
    parser.add_argument("--warmup-runs", type=int, default=1)
    parser.add_argument(
        "--aggregation",
        choices=["best", "median", "mean"],
        default="median",
    )
    parser.add_argument(
        "--inter-run-cooldown-sec",
        type=float,
        default=1.0,
        help="Sleep between warmup/measured wrk runs for lower noise (default: 1.0).",
    )
    parser.add_argument("--cooldown-sec", type=float, default=2.0)
    parser.add_argument(
        "--output",
        type=Path,
        default=REPO_ROOT / "benchmark_results" / "stage4_framework_matrix",
    )
    parser.add_argument("--jobs", type=int, default=max(1, os.cpu_count() or 1))
    parser.add_argument("--skip-build", action="store_true")
    parser.add_argument("--json", action="store_true", help="Print final JSON report to stdout.")
    args = parser.parse_args()

    if args.repeats < 1:
        print("Error: --repeats must be >= 1")
        return 1
    if args.warmup_runs < 0:
        print("Error: --warmup-runs must be >= 0")
        return 1
    if args.workers < 1:
        print("Error: --workers must be >= 1")
        return 1
    if args.inter_run_cooldown_sec < 0:
        print("Error: --inter-run-cooldown-sec must be >= 0")
        return 1

    try:
        postgres_dsn = resolve_postgres_dsn(args.postgres_dsn)
        configs = framework_configs(
            workers=args.workers,
            postgres_dsn=postgres_dsn,
            seed_count=args.seed_count,
            jobs=args.jobs,
        )
    except Exception as exc:
        print(f"Error: {exc}")
        return 1

    framework_order = args.framework or list(DEFAULT_FRAMEWORKS)
    scenario_order = args.scenario or list(DEFAULT_SCENARIOS)
    missing = [name for name in framework_order if name not in configs]
    if missing:
        print(f"Error: unknown frameworks: {', '.join(missing)}")
        return 1

    args.output.mkdir(parents=True, exist_ok=True)
    logs_dir = args.output / "logs"
    logs_dir.mkdir(parents=True, exist_ok=True)

    if not args.skip_build:
        for name in framework_order:
            config = configs[name]
            for cmd in config.build_steps:
                run_command(cmd, cwd=config.workdir)

    wrk_binary = shutil.which(os.environ.get("KATANA_WRK_BIN", "wrk"))
    if not wrk_binary:
        print("Error: wrk not found in PATH")
        return 1

    results: List[Dict[str, Any]] = []
    started = time.perf_counter()
    for name in framework_order:
        config = configs[name]
        env = dict(os.environ)
        for key, value in config.env.items():
            env[key] = value.format(port=config.port)

        target = framework_bench.TargetConfig(
            name=name,
            base_url=f"http://127.0.0.1:{config.port}/",
            scenario_urls={},
        )
        for scenario_id in scenario_order:
            proc: Optional[subprocess.Popen[str]] = None
            try:
                print(f"\n[target] starting {name} on :{config.port} for {scenario_id}")
                proc = start_server(config, port=config.port, logs_dir=logs_dir)
                scenario = framework_bench.SCENARIOS[scenario_id]
                results.append(
                    framework_bench.run_target_scenario(
                        wrk_binary=wrk_binary,
                        target=target,
                        scenario_id=scenario_id,
                        scenario=scenario,
                        repeats=args.repeats,
                        aggregation_mode=args.aggregation,
                        include_runs=False,
                        warmup_runs=args.warmup_runs,
                        inter_run_cooldown_sec=args.inter_run_cooldown_sec,
                    )
                )
            finally:
                stop_server(proc)
                cleanup_error = cleanup_sql(postgres_dsn, env)
                if cleanup_error:
                    results.append(
                        {
                            "target": name,
                            "scenario_id": "__cleanup__",
                            "scenario_name": "cleanup",
                            "url": None,
                            "config": {},
                            "run_count": 0,
                            "duration_ms": 0,
                            "success": False,
                            "error_message": cleanup_error,
                            "benchmark": None,
                            "benchmark_stats": {},
                            "benchmark_runs": [],
                        }
                    )
                time.sleep(max(0.0, args.cooldown_sec))

    report = {
        "generated_at": datetime.now().strftime("%Y-%m-%d %H:%M:%S"),
        "workers": args.workers,
        "seed_count": args.seed_count,
        "repeats": args.repeats,
        "aggregation_mode": args.aggregation,
        "warmup_runs": args.warmup_runs,
        "inter_run_cooldown_sec": args.inter_run_cooldown_sec,
        "cooldown_sec": args.cooldown_sec,
        "total_duration_ms": int((time.perf_counter() - started) * 1000),
        "framework_order": framework_order,
        "scenario_order": scenario_order,
        "results": results,
    }

    json_path = args.output / "stage4_framework_matrix.json"
    md_path = args.output / "STAGE4_FRAMEWORK_MATRIX.md"
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
