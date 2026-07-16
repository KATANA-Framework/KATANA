#!/usr/bin/env python3
"""
Cross-framework benchmark orchestrator (backend host side).

Runs on the machine that hosts the framework servers. For every
(framework, scenario) pair it:

  1. starts the framework server on --port (one server at a time),
  2. waits for readiness via GET /health,
  3. runs one unmeasured warmup pass with wrk on the remote load box,
  4. runs --repeats measured wrk passes (--latency), while sampling the
     backend process group locally:
       - memory via `ps -o rss=,vsz=` (per process group, summed)
       - CPU via /proc/<pid>/stat utime+stime deltas (per process group)
       - a `top -b -n 1` snapshot mid-run kept in the raw logs
  5. stops the server and aggregates the median across repeats.

wrk itself runs on the remote load generator over SSH so the HTTP path is a
real network path. Lua scripts must be present on the load box (use
--sync-scripts once to copy them).

Fairness policy encoded here:
  - identical wrk parameters, scripts, warmup and repeat counts per framework
  - identical worker counts (BENCH_WORKERS/KATANA_WORKERS) for every server
  - identical PostgreSQL DSN, seed count (4096) and pool sizing formula
  - servers restart (with DB reset + reseed) before the mutating
    db-insert-returning scenario so every framework inserts into an
    identically sized table
  - scenario order is fixed and identical for every framework
"""

from __future__ import annotations

import argparse
import json
import os
import shlex
import signal
import statistics
import subprocess
import sys
import threading
import time
from dataclasses import dataclass, field
from datetime import datetime, timezone
from pathlib import Path
from typing import Any, Dict, List, Optional

SCRIPT_DIR = Path(__file__).resolve().parent
REPO_ROOT = SCRIPT_DIR.parent.parent
WRK_SCRIPTS_DIR = REPO_ROOT / "test" / "load" / "scripts"

DEFAULT_SEED_COUNT = 4096

# set from --backend-taskset / --wrk-taskset / --wrk-threads
BACKEND_TASKSET = ""
WRK_TASKSET = ""
WRK_THREADS_OVERRIDE = 0


@dataclass(frozen=True)
class Scenario:
    id: str
    name: str
    wrk_script: str
    threads: int
    connections: int
    duration_sec: int
    needs_db: bool
    mutates_db: bool
    pipeline_depth: int = 0


SCENARIOS: List[Scenario] = [
    Scenario("json-small", "JSON small object (GET /json)", "json_get.lua", 4, 512, 30, False, False),
    Scenario("echo-json", "JSON echo transform (POST /echo)", "echo_post.lua", 4, 512, 30, False, False),
    Scenario("stats-128", "JSON stats 128 doubles (POST /compute/stats)", "stats_post.lua", 4, 512, 30, False, False),
    Scenario("json-pipe-10", "JSON pipelined depth 10 (GET /json)", "json_pipeline.lua", 4, 512, 30, False, False, 10),
    Scenario("json-pipe-20", "JSON pipelined depth 20 (GET /json)", "json_pipeline.lua", 4, 512, 30, False, False, 20),
    Scenario("db-point-select", "DB point SELECT + JSON (GET /items/{id})", "db_point_select.lua", 4, 256, 30, True, False),
    Scenario("db-list", "DB list SELECT 20 rows + JSON (GET /items)", "db_list.lua", 4, 256, 30, True, False),
    Scenario("db-insert-returning", "DB INSERT+RETURNING + JSON (POST /items)", "db_insert_returning.lua", 4, 256, 30, True, True),
]


@dataclass(frozen=True)
class Framework:
    id: str
    # command resolved relative to REPO_ROOT; {repo} placeholder supported
    command: List[str]
    cwd: str = "."
    extra_env: Dict[str, str] = field(default_factory=dict)


FRAMEWORKS: List[Framework] = [
    Framework(
        "katana",
        ["{repo}/build/bench/examples/codegen/benchmark_api/benchmark_api"],
    ),
    Framework(
        "actix-web",
        ["{repo}/comparisons/http_frameworks/actix_web/target/release/katana_comparison_actix"],
    ),
    Framework(
        "axum",
        ["{repo}/comparisons/http_frameworks/axum/target/release/katana_comparison_axum"],
    ),
    Framework(
        "ntex",
        ["{repo}/comparisons/http_frameworks/ntex/target/release/katana_comparison_ntex"],
    ),
    Framework(
        "drogon",
        ["{repo}/comparisons/http_frameworks/drogon/build/katana_comparison_drogon"],
    ),
    Framework(
        "fastapi",
        ["{repo}/comparisons/http_frameworks/fastapi/.venv/bin/python", "app.py"],
        cwd="comparisons/http_frameworks/fastapi",
    ),
]


def log(message: str) -> None:
    print(f"[{datetime.now().strftime('%H:%M:%S')}] {message}", flush=True)


def run(cmd: List[str], **kwargs: Any) -> subprocess.CompletedProcess:
    return subprocess.run(cmd, capture_output=True, text=True, **kwargs)


# --------------------------------------------------------------------------
# local sampling of the backend process group
# --------------------------------------------------------------------------


def listener_pids(port: int) -> set:
    """PIDs of processes listening on the port (via ss -tlnp)."""
    import re

    result = run(["ss", "-tlnp", f"sport = :{port}"])
    return {int(match) for match in re.findall(r"pid=(\d+)", result.stdout)}


def pgid_pids(pgid: int) -> List[int]:
    result = run(["ps", "-eo", "pid=,pgid="])
    pids: List[int] = []
    for line in result.stdout.splitlines():
        parts = line.split()
        if len(parts) == 2 and parts[1].isdigit() and int(parts[1]) == pgid:
            pids.append(int(parts[0]))
    return pids


def sample_memory_kb(pgid: int) -> Dict[str, int]:
    """Memory via `ps -o rss=,vsz=` summed over the process group (KiB)."""
    rss_total = 0
    vsz_total = 0
    for pid in pgid_pids(pgid):
        result = run(["ps", "-o", "rss=,vsz=", "-p", str(pid)])
        parts = result.stdout.split()
        if len(parts) == 2:
            rss_total += int(parts[0])
            vsz_total += int(parts[1])
    return {"rss_kb": rss_total, "vsz_kb": vsz_total}


def proc_group_cpu_ticks(pgid: int) -> int:
    total = 0
    for pid in pgid_pids(pgid):
        try:
            stat = Path(f"/proc/{pid}/stat").read_text()
        except OSError:
            continue
        fields = stat.rsplit(")", 1)[1].split()
        utime, stime = int(fields[11]), int(fields[12])
        total += utime + stime
    return total


class ResourceSampler:
    """Samples backend memory + CPU while a wrk pass is running."""

    def __init__(self, pgid: int, raw_log: Path, interval_sec: float = 2.0):
        self.pgid = pgid
        self.raw_log = raw_log
        self.interval = interval_sec
        self.samples: List[Dict[str, Any]] = []
        self._stop = threading.Event()
        self._thread = threading.Thread(target=self._loop, daemon=True)
        self._clk_tck = os.sysconf("SC_CLK_TCK")
        self._ncpu = os.cpu_count() or 1

    def _loop(self) -> None:
        prev_ticks = proc_group_cpu_ticks(self.pgid)
        prev_time = time.monotonic()
        top_taken = False
        started = time.monotonic()
        while not self._stop.wait(self.interval):
            now = time.monotonic()
            ticks = proc_group_cpu_ticks(self.pgid)
            memory = sample_memory_kb(self.pgid)
            elapsed = now - prev_time
            cpu_pct = 0.0
            if elapsed > 0:
                cpu_pct = 100.0 * (ticks - prev_ticks) / self._clk_tck / elapsed
            self.samples.append(
                {
                    "t": round(now - started, 1),
                    "cpu_pct_of_one_core": round(cpu_pct, 1),
                    "cpu_pct_of_all_cores": round(cpu_pct / self._ncpu, 2),
                    **memory,
                }
            )
            prev_ticks, prev_time = ticks, now
            # one `top -b -n 1` snapshot mid-run, as a raw-evidence artifact
            if not top_taken and now - started > 10:
                top_taken = True
                top = run(["top", "-b", "-n", "1"])
                with self.raw_log.open("a") as fh:
                    fh.write("\n===== top -b -n 1 (mid-run) =====\n")
                    fh.write(top.stdout[:8000])
                    fh.write("\n===== end top =====\n")

    def start(self) -> None:
        self._thread.start()

    def stop(self) -> Dict[str, Any]:
        self._stop.set()
        self._thread.join(timeout=10)
        if not self.samples:
            return {}
        rss_values = [s["rss_kb"] for s in self.samples]
        cpu_values = [s["cpu_pct_of_one_core"] for s in self.samples if s["t"] > 2]
        return {
            "rss_kb_peak": max(rss_values),
            "rss_kb_median": int(statistics.median(rss_values)),
            "vsz_kb_peak": max(s["vsz_kb"] for s in self.samples),
            "cpu_pct_one_core_median": round(statistics.median(cpu_values), 1) if cpu_values else None,
            "cpu_pct_one_core_peak": round(max(cpu_values), 1) if cpu_values else None,
            "samples": self.samples,
        }


# --------------------------------------------------------------------------
# remote wrk
# --------------------------------------------------------------------------


def parse_wrk_output(output: str) -> Dict[str, Any]:
    import re

    metrics: Dict[str, Any] = {
        "rps": None,
        "latency_p50_us": None,
        "latency_p90_us": None,
        "latency_p95_us": None,
        "latency_p99_us": None,
        "latency_p999_us": None,
        "latency_max_us": None,
        "errors_status": 0,
        "errors_timeout": 0,
        "errors_total": 0,
        "non_2xx": 0,
        "requests": None,
    }
    for line in output.splitlines():
        stripped = line.strip()
        if stripped.startswith("Requests/sec:"):
            metrics["rps"] = float(stripped.split(":", 1)[1])
        elif "requests in" in stripped:
            match = re.search(r"([\d.]+)\s+requests in", stripped)
            if match:
                metrics["requests"] = float(match.group(1))
        elif stripped.startswith("Non-2xx or 3xx responses:"):
            metrics["non_2xx"] = int(stripped.split(":", 1)[1])
        elif stripped.startswith("KATANA_LATENCY_P50_US:"):
            metrics["latency_p50_us"] = float(stripped.split(":", 1)[1])
        elif stripped.startswith("KATANA_LATENCY_P90_US:"):
            metrics["latency_p90_us"] = float(stripped.split(":", 1)[1])
        elif stripped.startswith("KATANA_LATENCY_P95_US:"):
            metrics["latency_p95_us"] = float(stripped.split(":", 1)[1])
        elif stripped.startswith("KATANA_LATENCY_P99_US:"):
            metrics["latency_p99_us"] = float(stripped.split(":", 1)[1])
        elif stripped.startswith("KATANA_LATENCY_P999_US:"):
            metrics["latency_p999_us"] = float(stripped.split(":", 1)[1])
        elif stripped.startswith("KATANA_LATENCY_MAX_US:"):
            metrics["latency_max_us"] = float(stripped.split(":", 1)[1])
        elif stripped.startswith("KATANA_ERRORS_STATUS:"):
            metrics["errors_status"] = int(float(stripped.split(":", 1)[1]))
        elif stripped.startswith("KATANA_ERRORS_TIMEOUT:"):
            metrics["errors_timeout"] = int(float(stripped.split(":", 1)[1]))
        elif stripped.startswith("KATANA_ERRORS:"):
            metrics["errors_total"] = int(float(stripped.split(":", 1)[1]))
    return metrics


def run_remote_wrk(
    ssh_target: str,
    remote_scripts_dir: str,
    scenario: Scenario,
    target_url: str,
    duration_override: Optional[int],
    item_count: int,
) -> str:
    duration = duration_override or scenario.duration_sec
    wrk_prefix = ""
    if ssh_target == "local" and WRK_TASKSET:
        wrk_prefix = f"taskset -c {WRK_TASKSET} "
    threads = WRK_THREADS_OVERRIDE or scenario.threads
    depth_env = f"KATANA_PIPELINE_DEPTH={scenario.pipeline_depth} " if scenario.pipeline_depth else ""
    remote_cmd = (
        f"{depth_env}KATANA_BENCHMARK_API_ITEM_COUNT={item_count} "
        f"{wrk_prefix}wrk -t{threads} -c{scenario.connections} -d{duration}s --latency "
        f"-s {shlex.quote(remote_scripts_dir + '/' + scenario.wrk_script)} "
        f"{shlex.quote(target_url)}"
    )
    if ssh_target == "local":
        result = run(["bash", "-c", remote_cmd], timeout=duration + 60)
    else:
        result = run(
            ["ssh", "-o", "BatchMode=yes", ssh_target, remote_cmd],
            timeout=duration + 60,
        )
    if result.returncode != 0:
        raise RuntimeError(
            f"remote wrk failed (rc={result.returncode}): {result.stderr.strip()[:500]}"
        )
    return result.stdout


# --------------------------------------------------------------------------
# backend lifecycle
# --------------------------------------------------------------------------


class Backend:
    def __init__(self, framework: Framework, port: int, workers: int, dsn: str, log_path: Path):
        self.framework = framework
        self.port = port
        self.workers = workers
        self.dsn = dsn
        self.log_path = log_path
        self.process: Optional[subprocess.Popen] = None

    def env(self) -> Dict[str, str]:
        env = dict(os.environ)
        pool_size = max(4, min(self.workers * 4, 64))
        env.update(
            {
                "PORT": str(self.port),
                "HELLO_PORT": str(self.port),
                "COMPUTE_PORT": str(self.port),
                "BENCH_WORKERS": str(self.workers),
                "KATANA_WORKERS": str(self.workers),
                "KATANA_BENCHMARK_API_POSTGRES_DSN": self.dsn,
                "KATANA_TEST_POSTGRES_DSN": self.dsn,
                "KATANA_BENCHMARK_API_SEED_COUNT": str(DEFAULT_SEED_COUNT),
                "KATANA_BENCHMARK_API_RESET": "1",
                "KATANA_BENCHMARK_API_BOOTSTRAP": "1",
                # same connection budget for KATANA's SQL executors as the
                # deadpool/psycopg/drogon pools
                "KATANA_BENCHMARK_API_EXECUTORS": str(pool_size),
            }
        )
        env.update(self.framework.extra_env)
        return env

    def start(self) -> None:
        # A stale listener would silently serve every probe and every wrk pass
        # in place of the framework under test — refuse to start over one.
        stale = listener_pids(self.port)
        if stale:
            raise RuntimeError(
                f"port {self.port} is already in use by pids {sorted(stale)}; "
                "kill the stale listener before benchmarking"
            )
        command = [part.format(repo=str(REPO_ROOT)) for part in self.framework.command]
        if BACKEND_TASKSET:
            command = ["taskset", "-c", BACKEND_TASKSET, *command]
        cwd = REPO_ROOT / self.framework.cwd
        log_file = self.log_path.open("a")
        log_file.write(f"\n===== start {datetime.now().isoformat()} :: {' '.join(command)}\n")
        self.process = subprocess.Popen(
            command,
            cwd=cwd,
            env=self.env(),
            stdout=log_file,
            stderr=subprocess.STDOUT,
            start_new_session=True,  # own process group for sampling/teardown
        )

    @property
    def pgid(self) -> int:
        assert self.process is not None
        return os.getpgid(self.process.pid)

    def wait_ready(self, timeout_sec: int = 90) -> None:
        deadline = time.monotonic() + timeout_sec
        url = f"http://127.0.0.1:{self.port}/health"
        while time.monotonic() < deadline:
            if self.process and self.process.poll() is not None:
                raise RuntimeError(
                    f"{self.framework.id} exited during startup "
                    f"(rc={self.process.returncode}); see {self.log_path}"
                )
            probe = run(["curl", "-s", "-o", "/dev/null", "-w", "%{http_code}", "--max-time", "2", url])
            if probe.stdout.strip() == "200":
                # verify the /health answer came from OUR process group, not a
                # stale listener sharing the port via SO_REUSEPORT
                listeners = listener_pids(self.port)
                ours = set(pgid_pids(self.pgid))
                if not listeners:
                    raise RuntimeError(
                        f"{self.framework.id}: health answered but no listener found on :{self.port}"
                    )
                if not listeners.issubset(ours):
                    raise RuntimeError(
                        f"{self.framework.id}: foreign listener(s) {sorted(listeners - ours)} "
                        f"on :{self.port} (our pgid {self.pgid}) — measurements would be bogus"
                    )
                return
            time.sleep(1.0)
        raise RuntimeError(f"{self.framework.id} did not become ready in {timeout_sec}s")

    def stop(self) -> None:
        if self.process is None:
            return
        try:
            os.killpg(self.pgid, signal.SIGTERM)
        except ProcessLookupError:
            pass
        try:
            self.process.wait(timeout=15)
        except subprocess.TimeoutExpired:
            try:
                os.killpg(self.pgid, signal.SIGKILL)
            except ProcessLookupError:
                pass
            self.process.wait(timeout=10)
        self.process = None
        # wait until the port is actually released; escalate if needed
        deadline = time.monotonic() + 20
        while time.monotonic() < deadline:
            remaining = listener_pids(self.port)
            if not remaining:
                time.sleep(1.0)
                return
            for pid in remaining:
                try:
                    os.kill(pid, signal.SIGKILL)
                except (ProcessLookupError, PermissionError):
                    pass
            time.sleep(1.0)
        raise RuntimeError(f"port {self.port} still busy after stopping {self.framework.id}")


# --------------------------------------------------------------------------
# orchestration
# --------------------------------------------------------------------------


def median_or_none(values: List[Optional[float]]) -> Optional[float]:
    clean = [v for v in values if v is not None]
    return statistics.median(clean) if clean else None


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--ssh-target", required=True, help="load generator, e.g. root@1.2.3.4")
    parser.add_argument("--target-url", required=True, help="URL the load box uses, e.g. http://white-ip:8085")
    parser.add_argument("--port", type=int, default=8085)
    parser.add_argument("--workers", type=int, default=os.cpu_count())
    parser.add_argument("--dsn", required=True, help="PostgreSQL DSN shared by all backends")
    parser.add_argument("--frameworks", default=",".join(f.id for f in FRAMEWORKS))
    parser.add_argument("--scenarios", default=",".join(s.id for s in SCENARIOS))
    parser.add_argument("--repeats", type=int, default=3)
    parser.add_argument("--warmup-sec", type=int, default=10)
    parser.add_argument("--duration-sec", type=int, default=None, help="override scenario duration")
    parser.add_argument("--remote-scripts-dir", default="/root/katana-bench/scripts")
    parser.add_argument("--sync-scripts", action="store_true", help="scp lua scripts to the load box and exit")
    parser.add_argument("--output", default=str(SCRIPT_DIR / "results"))
    parser.add_argument("--backend-taskset", default="", help="pin backend processes, e.g. 0-3")
    parser.add_argument("--wrk-taskset", default="", help="pin local wrk (only with --ssh-target local), e.g. 4,5,10,11")
    parser.add_argument("--wrk-threads", type=int, default=0, help="override wrk thread count (e.g. 2 for a 2-vCPU load box)")
    args = parser.parse_args()

    global BACKEND_TASKSET, WRK_TASKSET, WRK_THREADS_OVERRIDE
    BACKEND_TASKSET = args.backend_taskset
    WRK_TASKSET = args.wrk_taskset
    WRK_THREADS_OVERRIDE = args.wrk_threads

    if args.sync_scripts:
        scripts = [str(WRK_SCRIPTS_DIR / s.wrk_script) for s in SCENARIOS]
        if args.ssh_target == "local":
            run(["mkdir", "-p", args.remote_scripts_dir])
            result = run(["cp", *scripts, args.remote_scripts_dir + "/"])
        else:
            run(["ssh", "-o", "BatchMode=yes", args.ssh_target, f"mkdir -p {shlex.quote(args.remote_scripts_dir)}"])
            result = run(["scp", "-q", *scripts, f"{args.ssh_target}:{args.remote_scripts_dir}/"])
        print(result.stderr or "scripts synced")
        return result.returncode

    selected_frameworks = [f for f in FRAMEWORKS if f.id in set(args.frameworks.split(","))]
    selected_scenarios = [s for s in SCENARIOS if s.id in set(args.scenarios.split(","))]

    out_dir = Path(args.output) / datetime.now().strftime("%Y-%m-%d_%H%M%S")
    raw_dir = out_dir / "raw"
    raw_dir.mkdir(parents=True, exist_ok=True)

    all_results: List[Dict[str, Any]] = []

    for framework in selected_frameworks:
        log(f"=== framework: {framework.id} ===")
        server_log = raw_dir / f"{framework.id}.server.log"
        backend = Backend(framework, args.port, args.workers, args.dsn, server_log)
        backend.start()
        try:
            backend.wait_ready()
            log(f"{framework.id} ready on :{args.port}")

            for scenario in selected_scenarios:
                if scenario.mutates_db:
                    # restart with a fresh, identically-seeded table before the
                    # mutating scenario so every framework inserts into the
                    # same table size
                    log(f"{framework.id}: restarting for fresh DB before {scenario.id}")
                    backend.stop()
                    backend.start()
                    backend.wait_ready()

                log(f"{framework.id} :: {scenario.id} :: warmup {args.warmup_sec}s")
                run_remote_wrk(
                    args.ssh_target, args.remote_scripts_dir, scenario,
                    args.target_url, args.warmup_sec, DEFAULT_SEED_COUNT,
                )

                repeats: List[Dict[str, Any]] = []
                for attempt in range(1, args.repeats + 1):
                    raw_log = raw_dir / f"{framework.id}.{scenario.id}.run{attempt}.log"
                    sampler = ResourceSampler(backend.pgid, raw_log)
                    sampler.start()
                    try:
                        output = run_remote_wrk(
                            args.ssh_target, args.remote_scripts_dir, scenario,
                            args.target_url, args.duration_sec, DEFAULT_SEED_COUNT,
                        )
                    finally:
                        resources = sampler.stop()
                    raw_log.write_text(output + "\n" + raw_log.read_text() if raw_log.exists() else output)
                    metrics = parse_wrk_output(output)
                    metrics["resources"] = resources
                    repeats.append(metrics)
                    log(
                        f"{framework.id} :: {scenario.id} :: run {attempt}/{args.repeats}: "
                        f"{metrics['rps']:.0f} rps, p50 {metrics['latency_p50_us']}us, "
                        f"p99 {metrics['latency_p99_us']}us, errors {metrics['errors_total']}"
                    )

                summary = {
                    "framework": framework.id,
                    "scenario": scenario.id,
                    "scenario_name": scenario.name,
                    "wrk": {
                        "threads": scenario.threads,
                        "connections": scenario.connections,
                        "duration_sec": args.duration_sec or scenario.duration_sec,
                        "script": scenario.wrk_script,
                        "repeats": args.repeats,
                        "warmup_sec": args.warmup_sec,
                    },
                    "median": {
                        "rps": median_or_none([r["rps"] for r in repeats]),
                        "latency_p50_us": median_or_none([r["latency_p50_us"] for r in repeats]),
                        "latency_p90_us": median_or_none([r["latency_p90_us"] for r in repeats]),
                        "latency_p95_us": median_or_none([r["latency_p95_us"] for r in repeats]),
                        "latency_p99_us": median_or_none([r["latency_p99_us"] for r in repeats]),
                        "rss_kb_peak": median_or_none(
                            [r["resources"].get("rss_kb_peak") for r in repeats if r.get("resources")]
                        ),
                        "cpu_pct_one_core_median": median_or_none(
                            [r["resources"].get("cpu_pct_one_core_median") for r in repeats if r.get("resources")]
                        ),
                        "errors_total": sum(r["errors_total"] for r in repeats),
                        "errors_status": sum(r["errors_status"] for r in repeats),
                        "errors_timeout": sum(r["errors_timeout"] for r in repeats),
                        "non_2xx": sum(r["non_2xx"] for r in repeats),
                    },
                    "runs": repeats,
                }
                all_results.append(summary)
                (out_dir / "results.json").write_text(json.dumps(all_results, indent=2))
        finally:
            backend.stop()
            log(f"{framework.id} stopped")

    # CSV summary
    csv_lines = [
        "framework,scenario,rps,p50_us,p90_us,p99_us,rss_peak_kb,cpu_pct_one_core,errors,non_2xx"
    ]
    for result in all_results:
        median = result["median"]
        csv_lines.append(
            ",".join(
                str(x if x is not None else "")
                for x in [
                    result["framework"],
                    result["scenario"],
                    median["rps"],
                    median["latency_p50_us"],
                    median["latency_p90_us"],
                    median["latency_p99_us"],
                    median["rss_kb_peak"],
                    median["cpu_pct_one_core_median"],
                    median["errors_total"],
                    median["non_2xx"],
                ]
            )
        )
    (out_dir / "summary.csv").write_text("\n".join(csv_lines) + "\n")
    log(f"done; results in {out_dir}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
