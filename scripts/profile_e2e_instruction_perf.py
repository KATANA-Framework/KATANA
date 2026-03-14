#!/usr/bin/env python3
"""
Instruction-level perf capture for the maintained E2E wrk scenarios.

This script builds the example servers with debug symbols, runs the canonical
Hello World and Compute API wrk workloads, and collects:

- perf stat counters scoped to the server PID
- perf record samples scoped to the server PID
- perf report and perf script text dumps
- perf annotate output for the hottest user-space symbols
- a JSON manifest plus a markdown summary

The resulting bundle is suitable for deeper manual analysis or for handing off
to multiple agents that each focus on a different slice of the data.
"""

from __future__ import annotations

import argparse
import json
import os
import re
import shlex
import shutil
import socket
import subprocess
import sys
import time
from dataclasses import dataclass
from datetime import datetime, timezone
from pathlib import Path
from typing import Any, Dict, Iterable, List, Optional


REPO_ROOT = Path(__file__).resolve().parent.parent
DEFAULT_BUILD_DIR = REPO_ROOT / "build" / "perf-e2e"
DEFAULT_RESULTS_ROOT = REPO_ROOT / "profiling_results"
DEFAULT_STAT_EVENTS = (
    "task-clock:u,context-switches,cpu-migrations,page-faults,"
    "minor-faults,major-faults"
)
DEFAULT_SAMPLE_EVENT = "task-clock:u"
DEFAULT_SAMPLE_PERIOD = 2_000_000
DEFAULT_ANNOTATE_TOP = 8
DEFAULT_WARMUP_SEC = 2
DEFAULT_SERVER_WAIT_SEC = 15.0


@dataclass(frozen=True)
class Scenario:
    id: str
    display_name: str
    server_target: str
    server_candidates: tuple[str, ...]
    server_env: Dict[str, str]
    port: int
    wrk_script: str
    wrk_threads: int
    wrk_connections: int
    wrk_duration_sec: int
    pipeline_depth: str


SCENARIOS: Dict[str, Scenario] = {
    "hello-canonical": Scenario(
        id="hello-canonical",
        display_name="Hello World Canonical Pipeline",
        server_target="hello_world_server",
        server_candidates=(
            "hello_world_server",
            "examples/hello_world_server",
            "examples/examples/hello_world_server",
        ),
        server_env={"HELLO_PORT": "{port}", "KATANA_WORKERS": "{bench_workers}"},
        port=18080,
        wrk_script="test/load/scripts/hello_pipeline.lua",
        wrk_threads=4,
        wrk_connections=512,
        wrk_duration_sec=10,
        pipeline_depth="10",
    ),
    "compute-canonical": Scenario(
        id="compute-canonical",
        display_name="Compute API Canonical Pipeline",
        server_target="compute_api",
        server_candidates=(
            "compute_api",
            "examples/codegen/compute_api/compute_api",
            "examples/examples/codegen/compute_api/compute_api",
        ),
        server_env={
            "PORT": "{port}",
            "COMPUTE_PORT": "{port}",
            "KATANA_WORKERS": "{bench_workers}",
        },
        port=18081,
        wrk_script="test/load/scripts/compute_sum_pipeline.lua",
        wrk_threads=4,
        wrk_connections=512,
        wrk_duration_sec=10,
        pipeline_depth="10",
    ),
}


REPORT_SYMBOL_RE = re.compile(
    r"^\s*(?P<overhead>\d+(?:\.\d+)?)%\s+.*?\[\.\]\s+(?P<symbol>.+?)\s*$"
)


def utc_now_slug() -> str:
    return datetime.now(timezone.utc).strftime("%Y%m%dT%H%M%SZ")


def format_cmd(cmd: Iterable[str]) -> str:
    return " ".join(shlex.quote(part) for part in cmd)


def run_command(
    cmd: List[str],
    *,
    cwd: Optional[Path] = None,
    env: Optional[Dict[str, str]] = None,
    stdin_text: Optional[str] = None,
    stdout_path: Optional[Path] = None,
    stderr_path: Optional[Path] = None,
    timeout: Optional[int] = None,
    check: bool = True,
) -> subprocess.CompletedProcess[str]:
    stdout_target: Any = subprocess.PIPE
    stderr_target: Any = subprocess.PIPE
    stdout_handle = None
    stderr_handle = None
    if stdout_path:
        stdout_handle = stdout_path.open("w", encoding="utf-8")
        stdout_target = stdout_handle
    if stderr_path:
        stderr_handle = stderr_path.open("w", encoding="utf-8")
        stderr_target = stderr_handle
    try:
        proc = subprocess.run(
            cmd,
            cwd=str(cwd) if cwd else None,
            env=env,
            input=stdin_text,
            text=True,
            stdout=stdout_target,
            stderr=stderr_target,
            timeout=timeout,
            check=False,
        )
    finally:
        if stdout_handle:
            stdout_handle.close()
        if stderr_handle:
            stderr_handle.close()

    if check and proc.returncode != 0:
        stdout = (proc.stdout or "").strip()
        stderr = (proc.stderr or "").strip()
        raise RuntimeError(
            f"command failed ({proc.returncode}): {format_cmd(cmd)}\nstdout:\n{stdout}\nstderr:\n{stderr}"
        )
    return proc


def sudo_command(base_cmd: List[str], sudo_password: Optional[str]) -> tuple[List[str], Optional[str]]:
    if sudo_password:
        return ["sudo", "-S", *base_cmd], f"{sudo_password}\n"
    return ["sudo", "-n", *base_cmd], None


def find_binary(build_dir: Path, candidates: Iterable[str]) -> Optional[Path]:
    for candidate in candidates:
        binary = build_dir / candidate
        if binary.exists():
            return binary
    return None


def wait_for_port(host: str, port: int, timeout_s: float) -> bool:
    deadline = time.monotonic() + timeout_s
    while time.monotonic() < deadline:
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
            sock.settimeout(0.25)
            try:
                sock.connect((host, port))
                return True
            except OSError:
                time.sleep(0.1)
    return False


def build_examples(build_dir: Path, *, cmake_bin: str, ninja_bin: str) -> None:
    configure_cmd = [
        cmake_bin,
        "-S",
        str(REPO_ROOT),
        "-B",
        str(build_dir),
        "-G",
        "Ninja",
        "-DCMAKE_BUILD_TYPE=RelWithDebInfo",
        "-DENABLE_EXAMPLES=ON",
        "-DCMAKE_C_FLAGS_RELWITHDEBINFO=-O2 -g -DNDEBUG -fno-omit-frame-pointer",
        "-DCMAKE_CXX_FLAGS_RELWITHDEBINFO=-O2 -g -DNDEBUG -fno-omit-frame-pointer",
    ]
    run_command(configure_cmd, cwd=REPO_ROOT)

    build_cmd = [
        cmake_bin,
        "--build",
        str(build_dir),
        "--target",
        "hello_world_server",
        "compute_api",
        "--parallel",
    ]
    env = dict(os.environ)
    env.setdefault("CMAKE_MAKE_PROGRAM", ninja_bin)
    run_command(build_cmd, cwd=REPO_ROOT, env=env)


def start_server(binary: Path, env: Dict[str, str], logs_dir: Path) -> subprocess.Popen[str]:
    stdout_path = logs_dir / "server.stdout.log"
    stderr_path = logs_dir / "server.stderr.log"
    stdout_handle = stdout_path.open("w", encoding="utf-8")
    stderr_handle = stderr_path.open("w", encoding="utf-8")
    try:
        proc = subprocess.Popen(
            [str(binary)],
            cwd=str(REPO_ROOT),
            env=env,
            text=True,
            stdout=stdout_handle,
            stderr=stderr_handle,
        )
    except Exception:
        stdout_handle.close()
        stderr_handle.close()
        raise
    proc._katana_stdout_handle = stdout_handle  # type: ignore[attr-defined]
    proc._katana_stderr_handle = stderr_handle  # type: ignore[attr-defined]
    return proc


def stop_server(proc: Optional[subprocess.Popen[str]]) -> None:
    if proc is None:
        return
    if proc.poll() is None:
        proc.terminate()
        try:
            proc.wait(timeout=5)
        except subprocess.TimeoutExpired:
            proc.kill()
            proc.wait(timeout=5)
    stdout_handle = getattr(proc, "_katana_stdout_handle", None)
    stderr_handle = getattr(proc, "_katana_stderr_handle", None)
    if stdout_handle:
        stdout_handle.close()
    if stderr_handle:
        stderr_handle.close()


def run_wrk(
    scenario: Scenario,
    *,
    wrk_bin: str,
    env: Dict[str, str],
    output_path: Path,
    extra_duration_sec: int = 0,
) -> subprocess.CompletedProcess[str]:
    wrk_env = dict(env)
    wrk_env["KATANA_PIPELINE_DEPTH"] = scenario.pipeline_depth
    cmd = [
        wrk_bin,
        "-t",
        str(scenario.wrk_threads),
        "-c",
        str(scenario.wrk_connections),
        "-d",
        f"{scenario.wrk_duration_sec + extra_duration_sec}s",
        "--latency",
        "-s",
        str(REPO_ROOT / scenario.wrk_script),
        f"http://127.0.0.1:{scenario.port}/",
    ]
    proc = run_command(
        cmd,
        cwd=REPO_ROOT,
        env=wrk_env,
        stdout_path=output_path,
        timeout=max(30, scenario.wrk_duration_sec + extra_duration_sec + 20),
    )
    return proc


def extract_top_symbols_from_report(report_text: str, max_count: int) -> List[Dict[str, Any]]:
    top_symbols: List[Dict[str, Any]] = []
    seen: set[str] = set()
    for line in report_text.splitlines():
        match = REPORT_SYMBOL_RE.match(line)
        if not match:
            continue
        symbol = match.group("symbol").strip()
        symbol = re.sub(r"\s+-\s+-\s*$", "", symbol).rstrip()
        if not symbol or symbol in seen or symbol.startswith("__") or symbol == "[unknown]":
            continue
        seen.add(symbol)
        top_symbols.append(
            {
                "symbol": symbol,
                "overhead_pct": float(match.group("overhead")),
            }
        )
        if len(top_symbols) >= max_count:
            break
    return top_symbols


def perf_text_report(
    perf_bin: str,
    perf_data: Path,
    output_path: Path,
    *,
    sudo_password: Optional[str],
    extra_args: Optional[List[str]] = None,
) -> str:
    base_cmd = [
        perf_bin,
        "report",
        "-f",
        "-i",
        str(perf_data),
        "--stdio",
        "--no-children",
        "--percent-limit",
        "0",
    ]
    if extra_args:
        base_cmd.extend(extra_args)
    cmd, stdin_text = sudo_command(base_cmd, sudo_password)
    proc = run_command(cmd, stdin_text=stdin_text, check=True)
    output_path.write_text(proc.stdout or "", encoding="utf-8")
    return proc.stdout or ""


def perf_annotate_symbol(
    perf_bin: str,
    perf_data: Path,
    symbol: str,
    output_path: Path,
    *,
    sudo_password: Optional[str],
) -> None:
    base_cmd = [
        perf_bin,
        "annotate",
        "-f",
        "-i",
        str(perf_data),
        "--stdio",
        "--symbol",
        symbol,
    ]
    cmd, stdin_text = sudo_command(base_cmd, sudo_password)
    proc = run_command(cmd, stdin_text=stdin_text, check=False)
    output_path.write_text((proc.stdout or "") + (proc.stderr or ""), encoding="utf-8")


def collect_perf_script(
    perf_bin: str,
    perf_data: Path,
    output_path: Path,
    *,
    sudo_password: Optional[str],
) -> None:
    base_cmd = [
        perf_bin,
        "script",
        "-f",
        "-i",
        str(perf_data),
        "-F",
        "comm,pid,tid,time,period,event,ip,sym,dso",
    ]
    cmd, stdin_text = sudo_command(base_cmd, sudo_password)
    proc = run_command(cmd, stdin_text=stdin_text)
    output_path.write_text(proc.stdout or "", encoding="utf-8")


def chown_artifacts(paths: Iterable[Path], *, sudo_password: Optional[str], user: str) -> None:
    existing = [str(path) for path in paths if path.exists()]
    if not existing or not sudo_password:
        return
    base_cmd = ["chown", f"{user}:{user}", *existing]
    cmd, stdin_text = sudo_command(base_cmd, sudo_password)
    run_command(cmd, stdin_text=stdin_text)


def capture_perf_stat(
    scenario: Scenario,
    scenario_dir: Path,
    *,
    binary: Path,
    wrk_bin: str,
    perf_bin: str,
    sudo_password: Optional[str],
    stat_events: str,
    bench_workers: int,
) -> Dict[str, Any]:
    run_dir = scenario_dir / "stat_pass"
    run_dir.mkdir(parents=True, exist_ok=True)
    env = dict(os.environ)
    for key, value in scenario.server_env.items():
        env[key] = value.format(port=scenario.port, bench_workers=bench_workers)
    env["KATANA_WORKERS"] = str(bench_workers)

    server_proc = start_server(binary, env, run_dir)
    profiler_proc: Optional[subprocess.Popen[str]] = None
    profiler_stderr = (run_dir / "perf_stat.stderr.log").open("w", encoding="utf-8")
    try:
        if not wait_for_port("127.0.0.1", scenario.port, DEFAULT_SERVER_WAIT_SEC):
            raise RuntimeError(f"{scenario.display_name}: server failed to listen on {scenario.port}")

        warmup_output = run_dir / "warmup_wrk.txt"
        run_wrk(scenario, wrk_bin=wrk_bin, env=env, output_path=warmup_output, extra_duration_sec=-8)

        stat_path = run_dir / "perf_stat.csv"
        timeout_sec = scenario.wrk_duration_sec + 2
        perf_cmd = [
            "timeout",
            "--signal",
            "INT",
            f"{timeout_sec}s",
            perf_bin,
            "stat",
            "-x,",
            "-e",
            stat_events,
            "-p",
            str(server_proc.pid),
            "-o",
            str(stat_path),
        ]
        sudo_cmd, stdin_text = sudo_command(perf_cmd, sudo_password)
        profiler_proc = subprocess.Popen(
            sudo_cmd,
            cwd=str(REPO_ROOT),
            env=env,
            stdin=subprocess.PIPE if stdin_text is not None else None,
            stdout=subprocess.DEVNULL,
            stderr=profiler_stderr,
            text=True,
        )
        if stdin_text and profiler_proc.stdin:
            profiler_proc.stdin.write(stdin_text)
            profiler_proc.stdin.flush()
            profiler_proc.stdin.close()

        time.sleep(1.0)
        wrk_output = run_dir / "wrk_output.txt"
        run_wrk(scenario, wrk_bin=wrk_bin, env=env, output_path=wrk_output)
        profiler_proc.wait(timeout=timeout_sec + 10)

        chown_artifacts([stat_path], sudo_password=sudo_password, user=env.get("USER", ""))
        return {
            "perf_stat_csv": str(stat_path.relative_to(scenario_dir.parent)),
            "wrk_output": str(wrk_output.relative_to(scenario_dir.parent)),
        }
    finally:
        profiler_stderr.close()
        if profiler_proc and profiler_proc.poll() is None:
            profiler_proc.terminate()
            profiler_proc.wait(timeout=5)
        stop_server(server_proc)


def capture_perf_record(
    scenario: Scenario,
    scenario_dir: Path,
    *,
    binary: Path,
    wrk_bin: str,
    perf_bin: str,
    sudo_password: Optional[str],
    sample_event: str,
    sample_period: int,
    annotate_top: int,
    bench_workers: int,
) -> Dict[str, Any]:
    run_dir = scenario_dir / "record_pass"
    annotate_dir = run_dir / "annotate"
    annotate_dir.mkdir(parents=True, exist_ok=True)

    env = dict(os.environ)
    for key, value in scenario.server_env.items():
        env[key] = value.format(port=scenario.port, bench_workers=bench_workers)
    env["KATANA_WORKERS"] = str(bench_workers)

    perf_data = run_dir / "perf.data"
    server_proc = start_server(binary, env, run_dir)
    profiler_proc: Optional[subprocess.Popen[str]] = None
    profiler_stderr = (run_dir / "perf_record.stderr.log").open("w", encoding="utf-8")
    try:
        if not wait_for_port("127.0.0.1", scenario.port, DEFAULT_SERVER_WAIT_SEC):
            raise RuntimeError(f"{scenario.display_name}: server failed to listen on {scenario.port}")

        warmup_output = run_dir / "warmup_wrk.txt"
        run_wrk(scenario, wrk_bin=wrk_bin, env=env, output_path=warmup_output, extra_duration_sec=-8)

        timeout_sec = scenario.wrk_duration_sec + 2
        perf_cmd = [
            "timeout",
            "--signal",
            "INT",
            f"{timeout_sec}s",
            perf_bin,
            "record",
            "-e",
            sample_event,
            "-c",
            str(sample_period),
            "-g",
            "--call-graph",
            "fp",
            "-p",
            str(server_proc.pid),
            "-o",
            str(perf_data),
        ]
        sudo_cmd, stdin_text = sudo_command(perf_cmd, sudo_password)
        profiler_proc = subprocess.Popen(
            sudo_cmd,
            cwd=str(REPO_ROOT),
            env=env,
            stdin=subprocess.PIPE if stdin_text is not None else None,
            stdout=subprocess.DEVNULL,
            stderr=profiler_stderr,
            text=True,
        )
        if stdin_text and profiler_proc.stdin:
            profiler_proc.stdin.write(stdin_text)
            profiler_proc.stdin.flush()
            profiler_proc.stdin.close()

        time.sleep(1.0)
        wrk_output = run_dir / "wrk_output.txt"
        run_wrk(scenario, wrk_bin=wrk_bin, env=env, output_path=wrk_output)
        profiler_proc.wait(timeout=timeout_sec + 10)

        perf_sidecar = perf_data.with_name("perf.data.old")
        chown_artifacts([perf_data, perf_sidecar], sudo_password=sudo_password, user=env.get("USER", ""))

        report_path = run_dir / "perf_report.txt"
        report_text = perf_text_report(
            perf_bin,
            perf_data,
            report_path,
            sudo_password=sudo_password,
            extra_args=["-F", "overhead,symbol"],
        )
        collect_perf_script(perf_bin, perf_data, run_dir / "perf_script.txt", sudo_password=sudo_password)

        top_symbols = extract_top_symbols_from_report(report_text, annotate_top)
        for idx, entry in enumerate(top_symbols, start=1):
            annotate_path = annotate_dir / f"{idx:02d}_{sanitize_filename(entry['symbol'])}.txt"
            perf_annotate_symbol(
                perf_bin,
                perf_data,
                entry["symbol"],
                annotate_path,
                sudo_password=sudo_password,
            )

        top_symbols_path = run_dir / "top_symbols.json"
        top_symbols_path.write_text(json.dumps(top_symbols, indent=2), encoding="utf-8")
        return {
            "perf_data": str(perf_data.relative_to(scenario_dir.parent)),
            "perf_report": str(report_path.relative_to(scenario_dir.parent)),
            "perf_script": str((run_dir / "perf_script.txt").relative_to(scenario_dir.parent)),
            "top_symbols": str(top_symbols_path.relative_to(scenario_dir.parent)),
            "annotate_dir": str(annotate_dir.relative_to(scenario_dir.parent)),
            "wrk_output": str(wrk_output.relative_to(scenario_dir.parent)),
        }
    finally:
        profiler_stderr.close()
        if profiler_proc and profiler_proc.poll() is None:
            profiler_proc.terminate()
            profiler_proc.wait(timeout=5)
        stop_server(server_proc)


def sanitize_filename(value: str) -> str:
    cleaned = re.sub(r"[^A-Za-z0-9._-]+", "_", value).strip("._")
    return cleaned[:96] or "symbol"


def collect_system_metadata(perf_bin: str, wrk_bin: str) -> Dict[str, Any]:
    git_sha = run_command(["git", "rev-parse", "HEAD"], cwd=REPO_ROOT).stdout.strip()
    uname = run_command(["uname", "-a"]).stdout.strip()
    perf_version = run_command([perf_bin, "--version"]).stdout.strip()
    wrk_version = run_command([wrk_bin, "--version"], check=False)
    return {
        "git_sha": git_sha,
        "uname": uname,
        "perf_version": perf_version,
        "wrk_version": (wrk_version.stdout or wrk_version.stderr or "").strip(),
        "captured_at_utc": datetime.now(timezone.utc).isoformat(),
    }


def write_summary(results_dir: Path, manifest: Dict[str, Any]) -> None:
    lines = [
        "# E2E Instruction-Level perf Bundle",
        "",
        f"- Git SHA: `{manifest['system']['git_sha']}`",
        f"- Captured at UTC: `{manifest['system']['captured_at_utc']}`",
        f"- perf: `{manifest['system']['perf_version']}`",
        f"- wrk: `{manifest['system']['wrk_version']}`",
        "",
        "## Scenarios",
        "",
    ]
    for scenario_id, result in manifest["scenarios"].items():
        lines.append(f"### {scenario_id}")
        lines.append("")
        lines.append(f"- Server binary: `{result['server_binary']}`")
        lines.append(f"- perf stat: `{result['stat_pass']['perf_stat_csv']}`")
        lines.append(f"- perf report: `{result['record_pass']['perf_report']}`")
        lines.append(f"- perf script: `{result['record_pass']['perf_script']}`")
        lines.append(f"- annotate dir: `{result['record_pass']['annotate_dir']}`")
        top_symbols = result["record_pass"]["top_symbols_entries"]
        if top_symbols:
            lines.append("- Hottest user-space symbols:")
            for entry in top_symbols:
                lines.append(f"  - `{entry['symbol']}` ({entry['overhead_pct']:.2f}%)")
        lines.append("")
    (results_dir / "SUMMARY.md").write_text("\n".join(lines), encoding="utf-8")


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--scenario",
        action="append",
        choices=sorted(SCENARIOS),
        help="Scenario to capture. Defaults to both canonical E2E scenarios.",
    )
    parser.add_argument(
        "--build-dir",
        type=Path,
        default=DEFAULT_BUILD_DIR,
        help=f"Build directory (default: {DEFAULT_BUILD_DIR}).",
    )
    parser.add_argument(
        "--output-dir",
        type=Path,
        default=None,
        help="Results directory. Defaults to profiling_results/e2e_instruction_perf_<timestamp>.",
    )
    parser.add_argument("--wrk-bin", default=os.environ.get("KATANA_WRK_BIN", "wrk"))
    parser.add_argument("--perf-bin", default=os.environ.get("KATANA_PERF_BIN", "perf"))
    parser.add_argument("--cmake-bin", default=os.environ.get("KATANA_CMAKE_BIN", "cmake"))
    parser.add_argument("--ninja-bin", default=os.environ.get("KATANA_NINJA_BIN", "ninja"))
    parser.add_argument(
        "--bench-workers",
        type=int,
        default=int(os.environ.get("KATANA_BENCH_WORKERS", "4")),
        help="Worker count passed to the server processes.",
    )
    parser.add_argument(
        "--stat-events",
        default=DEFAULT_STAT_EVENTS,
        help=f"perf stat events list (default: {DEFAULT_STAT_EVENTS}).",
    )
    parser.add_argument(
        "--sample-event",
        default=DEFAULT_SAMPLE_EVENT,
        help=f"perf record event used for annotate/report (default: {DEFAULT_SAMPLE_EVENT}).",
    )
    parser.add_argument(
        "--sample-period",
        type=int,
        default=DEFAULT_SAMPLE_PERIOD,
        help=f"perf record sampling period (default: {DEFAULT_SAMPLE_PERIOD}).",
    )
    parser.add_argument(
        "--annotate-top",
        type=int,
        default=DEFAULT_ANNOTATE_TOP,
        help=f"Number of hottest symbols to annotate (default: {DEFAULT_ANNOTATE_TOP}).",
    )
    parser.add_argument(
        "--sudo-password-env",
        default="PERF_SUDO_PASSWORD",
        help="Environment variable name that holds the sudo password.",
    )
    parser.add_argument(
        "--skip-build",
        action="store_true",
        help="Skip cmake configure/build and reuse an existing build tree.",
    )
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    scenario_ids = args.scenario or ["hello-canonical", "compute-canonical"]
    output_dir = args.output_dir or (DEFAULT_RESULTS_ROOT / f"e2e_instruction_perf_{utc_now_slug()}")
    output_dir.mkdir(parents=True, exist_ok=True)

    sudo_password = os.environ.get(args.sudo_password_env)
    if shutil.which(args.wrk_bin) is None:
        raise SystemExit(f"wrk not found: {args.wrk_bin}")
    if shutil.which(args.perf_bin) is None:
        raise SystemExit(f"perf not found: {args.perf_bin}")
    if shutil.which(args.cmake_bin) is None:
        raise SystemExit(f"cmake not found: {args.cmake_bin}")

    if not args.skip_build:
        build_examples(args.build_dir, cmake_bin=args.cmake_bin, ninja_bin=args.ninja_bin)

    manifest: Dict[str, Any] = {
        "results_dir": str(output_dir),
        "build_dir": str(args.build_dir),
        "arguments": {
            "scenario_ids": scenario_ids,
            "bench_workers": args.bench_workers,
            "stat_events": args.stat_events,
            "sample_event": args.sample_event,
            "sample_period": args.sample_period,
            "annotate_top": args.annotate_top,
        },
        "system": collect_system_metadata(args.perf_bin, args.wrk_bin),
        "scenarios": {},
    }

    for scenario_id in scenario_ids:
        scenario = SCENARIOS[scenario_id]
        scenario_dir = output_dir / scenario.id
        scenario_dir.mkdir(parents=True, exist_ok=True)
        binary = find_binary(args.build_dir, scenario.server_candidates)
        if binary is None:
            raise SystemExit(f"{scenario.id}: server binary not found under {args.build_dir}")

        stat_result = capture_perf_stat(
            scenario,
            scenario_dir,
            binary=binary,
            wrk_bin=args.wrk_bin,
            perf_bin=args.perf_bin,
            sudo_password=sudo_password,
            stat_events=args.stat_events,
            bench_workers=args.bench_workers,
        )
        record_result = capture_perf_record(
            scenario,
            scenario_dir,
            binary=binary,
            wrk_bin=args.wrk_bin,
            perf_bin=args.perf_bin,
            sudo_password=sudo_password,
            sample_event=args.sample_event,
            sample_period=args.sample_period,
            annotate_top=args.annotate_top,
            bench_workers=args.bench_workers,
        )
        top_symbols_entries = json.loads((output_dir / record_result["top_symbols"]).read_text(encoding="utf-8"))
        manifest["scenarios"][scenario.id] = {
            "display_name": scenario.display_name,
            "server_binary": str(binary),
            "stat_pass": stat_result,
            "record_pass": {
                **record_result,
                "top_symbols_entries": top_symbols_entries,
            },
        }

    (output_dir / "manifest.json").write_text(json.dumps(manifest, indent=2), encoding="utf-8")
    write_summary(output_dir, manifest)
    print(str(output_dir))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
