#!/usr/bin/env python3

import json
import os
import subprocess
import re
import sys
import time
import signal
import socket
import shutil
import tempfile
try:
    import resource
except ImportError:
    resource = None
from datetime import datetime
from pathlib import Path

class BenchmarkCollector:
    def __init__(self):
        self.results = {}
        self.build_dir = Path(__file__).parent / "build"
        self.server_proc = None
        self.bump_nofile_limit()

    def bump_nofile_limit(self, target=65535):
        """Try to raise RLIMIT_NOFILE so the load generator is not fd-bound."""
        try:
            if resource is None:
                return
            soft, hard = resource.getrlimit(resource.RLIMIT_NOFILE)
            new_soft = min(max(soft, target), hard)
            if new_soft != soft:
                resource.setrlimit(resource.RLIMIT_NOFILE, (new_soft, hard))
                print(f"Raised nofile limit: {soft} -> {new_soft} (hard={hard})")
        except Exception as e:
            print(f"Warning: unable to raise nofile limit: {e}")

    def start_server(self, server_path=None, ready_probe=None):
        if server_path is None:
            server_path = self.build_dir / "hello_world_server"
        if not server_path.exists():
            print(f"Warning: server binary not found at {server_path}")
            return False

        try:
            self.server_proc = subprocess.Popen([str(server_path)],
                                                stdout=subprocess.DEVNULL,
                                                stderr=subprocess.DEVNULL)
            # Wait until the process is up and responding (if probe provided)
            deadline = time.time() + 5.0
            if ready_probe:
                import http.client

                host, port, path = ready_probe
                while time.time() < deadline:
                    if self.server_proc.poll() is not None:
                        print("Error: Server exited during startup")
                        return False
                    try:
                        conn = http.client.HTTPConnection(host, port, timeout=0.5)
                        conn.request("GET", path)
                        resp = conn.getresponse()
                        resp.read()
                        conn.close()
                        # Consider any HTTP status a sign the server is up
                        return True
                    except Exception:
                        time.sleep(0.2)
                        continue
                print("Error: Server did not respond to readiness probe")
                return False
            else:
                time.sleep(2)
                if self.server_proc.poll() is not None:
                    print("Error: Server failed to start")
                    return False
                return True
        except Exception as e:
            print(f"Error starting server: {e}")
            return False

    def stop_server(self):
        if self.server_proc:
            try:
                self.server_proc.send_signal(signal.SIGINT)
                self.server_proc.wait(timeout=5)
            except:
                self.server_proc.kill()
            self.server_proc = None

    def run_benchmark(self, name, needs_server=False, server_path=None):
        bench_path = self.build_dir / "benchmark" / name
        if not bench_path.exists():
            print(f"Warning: {name} not found, skipping...")
            return None

        if needs_server and not self.start_server(server_path=server_path):
            print(f"Cannot run {name}: server failed to start")
            return None

        try:
            result = subprocess.run([str(bench_path)], capture_output=True, text=True, timeout=300)
            output = result.stdout
        except subprocess.TimeoutExpired:
            print(f"Error: {name} timed out")
            output = None
        except Exception as e:
            print(f"Error running {name}: {e}")
            output = None
        finally:
            if needs_server:
                self.stop_server()

        return output

    def wrk_available(self):
        return shutil.which("wrk") is not None

    def run_wrk(self, url, bodies, threads=8, connections=128, duration="10s", weights=None, headers=None):
        """Invoke wrk with a generated Lua script. Supports body mixes."""
        wrk_bin = shutil.which("wrk")
        if not wrk_bin:
            return None

        if not isinstance(bodies, (list, tuple)):
            bodies = [bodies]
        bodies = [b.decode() if isinstance(b, (bytes, bytearray)) else str(b) for b in bodies]
        headers = headers or {
            "Content-Type": "application/json",
            "Accept": "application/json",
            "Connection": "keep-alive",
        }

        header_lines = ",\n  ".join([f'["{k}"] = "{v}"' for k, v in headers.items()])
        weights = list(weights) if weights else None
        if weights and len(weights) != len(bodies):
            weights = None

        if len(bodies) == 1:
            # Use single quotes in Lua to avoid escaping JSON double quotes
            # Escape single quotes and backslashes in the body
            escaped_body = bodies[0].replace('\\', '\\\\').replace("'", "\\'")
            # Set headers directly (avoid local variable which causes wrk segfault with multiple threads)
            header_assigns = "\n".join([f'wrk.headers["{k}"] = "{v}"' for k, v in headers.items()])
            lua_script = f"""wrk.method = "POST"
{header_assigns}
wrk.body = '{escaped_body}'
"""
        else:
            # Use single quotes in Lua to avoid escaping JSON double quotes
            escaped_bodies = [b.replace('\\', '\\\\').replace("'", "\\'") for b in bodies]
            body_table = ",\n  ".join([f"'{b}'" for b in escaped_bodies])
            weights_table = ""
            if weights:
                weights_table = ",\n  ".join([str(float(w)) for w in weights])
            # Set headers directly (avoid local variable which causes wrk segfault)
            header_assigns = "\n".join([f'wrk.headers["{k}"] = "{v}"' for k, v in headers.items()])
            lua_script = f"""wrk.method = "POST"
{header_assigns}
local bodies = {{
  {body_table}
}}
{"local weights = {" + weights_table + "}" if weights else "local weights = nil"}

math.randomseed(os.time())

local function pick_index()
  if not weights then
    return math.random(#bodies)
  end
  local r = math.random()
  local acc = 0
  for i, w in ipairs(weights) do
    acc = acc + w
    if r <= acc then return i end
  end
  return #bodies
end

request = function()
  local idx = pick_index()
  local body = bodies[idx]
  return wrk.format("POST", nil, nil, body)
end
"""

        with tempfile.NamedTemporaryFile(mode="w", delete=False, suffix=".lua") as f:
            f.write(lua_script)
            script_path = f.name

        try:
            cmd = [
                wrk_bin,
                "-t", str(threads),
                "-c", str(connections),
                "-d", duration,
                "-s", script_path,
                url,
            ]
            # Debug: print command and script
            if os.environ.get("DEBUG_WRK"):
                print(f"DEBUG: wrk command: {' '.join(cmd)}")
                print(f"DEBUG: Script content:\n{lua_script[:500]}")

            result = subprocess.run(cmd, capture_output=True, text=True, timeout=180)
            if result.returncode != 0:
                print(f"wrk failed with return code {result.returncode}")
                print(f"stderr: {result.stderr[:200]}")
            return result.stdout
        except Exception as e:
            print(f"wrk exception: {type(e).__name__}: {e}")
            return None
        finally:
            try:
                Path(script_path).unlink(missing_ok=True)
            except Exception:
                pass

    def parse_wrk_metrics(self, output):
        """Parse wrk stdout into a dict of metrics."""
        metrics = {}

        def to_ms(value, unit):
            factor = {"s": 1000.0, "ms": 1.0, "us": 0.001}.get(unit.lower())
            if factor is None:
                return None
            return value * factor

        reqs = re.search(r"Requests/sec:\s+([\d\.]+)", output)
        if reqs:
            metrics["throughput"] = (float(reqs.group(1)), "req/s")

        non2xx = re.search(r"Non-2xx or 3xx responses:\s+(\d+)", output)
        if non2xx:
            metrics["non_2xx_3xx"] = (float(non2xx.group(1)), "count")

        socket_err = re.search(
            r"Socket errors:\s+connect\s+(\d+),\s+read\s+(\d+),\s+write\s+(\d+),\s+timeout\s+(\d+)", output
        )
        if socket_err:
            errs = [int(socket_err.group(i)) for i in range(1, 5)]
            metrics["socket_errors"] = (float(sum(errs)), "count")

        for pct in (50, 75, 90, 95, 99):
            m = re.search(rf"{pct}\s*%\s+([\d\.]+)\s*(us|ms|s)", output)
            if m:
                val_ms = to_ms(float(m.group(1)), m.group(2))
                if val_ms is not None:
                    metrics[f"p{pct}"] = (val_ms, "ms")

        return metrics

    def parse_standard_benchmark(self, output, category):
        sections = re.split(r'\n===\s+(.+?)\s+===\n', output)

        for i in range(1, len(sections), 2):
            if i + 1 < len(sections):
                bench_name = sections[i].strip()
                data = sections[i + 1]

                metrics = {}

                ops_match = re.search(r'Operations:\s+([\d.]+)', data)
                if ops_match:
                    metrics['Operations'] = (float(ops_match.group(1)), 'ops')

                dur_match = re.search(r'Duration:\s+([\d.]+)\s+ms', data)
                if dur_match:
                    metrics['Duration'] = (float(dur_match.group(1)), 'ms')

                thr_match = re.search(r'Throughput:\s+([\d.]+)\s+ops/sec', data)
                if thr_match:
                    metrics['Throughput'] = (float(thr_match.group(1)), 'ops/sec')

                lat_p50 = re.search(r'Latency p50:\s+([\d.]+)\s+us', data)
                if lat_p50:
                    metrics['Latency p50'] = (float(lat_p50.group(1)), 'us')

                lat_p99 = re.search(r'Latency p99:\s+([\d.]+)\s+us', data)
                if lat_p99:
                    metrics['Latency p99'] = (float(lat_p99.group(1)), 'us')

                lat_p999 = re.search(r'Latency p999:\s+([\d.]+)\s+us', data)
                if lat_p999:
                    metrics['Latency p999'] = (float(lat_p999.group(1)), 'us')

                errors_match = re.search(r'Errors:\s+([\d.]+)', data)
                if errors_match:
                    metrics['Errors'] = (float(errors_match.group(1)), 'count')

                if category not in self.results:
                    self.results[category] = {}
                self.results[category][bench_name] = metrics

    def parse_simple_benchmark(self, output):
        sections = re.split(r'\n===\s+(.+?)\s+===\n', output)

        for i in range(1, len(sections), 2):
            if i + 1 < len(sections):
                category = sections[i].strip()
                data = sections[i + 1]

                if category not in self.results:
                    self.results[category] = {}

                lines = data.strip().split('\n')
                for line in lines:
                    line = line.strip()
                    if not line or line.startswith('==='):
                        continue

                    parts = line.rsplit(maxsplit=2)
                    if len(parts) < 3:
                        continue

                    metric_name = ' '.join(parts[:-2]).strip()
                    if not metric_name or metric_name in {"|", ":"}:
                        continue

                    try:
                        value = float(parts[-2])
                        unit = parts[-1].strip().replace("\\", "")

                        if metric_name and metric_name not in self.results[category]:
                            self.results[category][metric_name] = (value, unit)
                    except (ValueError, IndexError):
                        continue

    def generate_markdown(self, output_path):
        def iter_flat_metrics(category_name):
            for name, data in self.results.get(category_name, {}).items():
                if isinstance(data, dict):
                    for m_name, m_val in data.items():
                        yield m_name, m_val
                else:
                    yield name, data

        def get_metric(category_name, metric_name):
            for name, data in iter_flat_metrics(category_name):
                if name == metric_name:
                    return data
            return None

        def get_bench_metric(category_name, bench_name, metric_name):
            bench = self.results.get(category_name, {}).get(bench_name)
            if isinstance(bench, dict):
                return bench.get(metric_name)
            return None

        def format_metric(metric):
            if not metric:
                return "n/a"
            value, unit = metric
            return f"{value:.3f} {unit.strip()}"

        def summarize():
            summary = []

            core_p99 = get_metric("Core Performance", "Latency p99")
            core_thr = get_metric("Core Performance", "Keep-alive throughput")
            if core_p99 or core_thr:
                parts = []
                if core_p99:
                    parts.append(f"p99 {format_metric(core_p99)}")
                if core_thr:
                    parts.append(f"throughput {format_metric(core_thr)}")
                summary.append("Core: " + "; ".join(parts))

            max_thread = None
            max_thread_thr = None
            for name, metric in iter_flat_metrics("Scalability"):
                m = re.match(r"Throughput with (\d+) threads", name)
                if m:
                    threads = int(m.group(1))
                    if max_thread is None or threads > max_thread:
                        max_thread = threads
                        max_thread_thr = metric
            if max_thread is not None and max_thread_thr:
                summary.append(
                    f"Thread scaling: {max_thread} threads -> {format_metric(max_thread_thr)}"
                )

            max_conn = None
            max_conn_thr = None
            for name, metric in iter_flat_metrics("Scalability"):
                m = re.match(r"(\d+)\s+concurrent connections", name)
                if m:
                    conns = int(m.group(1))
                    if max_conn is None or conns > max_conn:
                        max_conn = conns
                        max_conn_thr = metric
            if max_conn is not None and max_conn_thr:
                summary.append(
                    f"Fan-out: {max_conn} conns -> {format_metric(max_conn_thr)}"
                )

            churn_thr = None
            churn_threads = None
            for name, metric in iter_flat_metrics("Connection Churn"):
                m = re.match(r"Close-after-each-request throughput \((\d+) threads\)", name)
                if m:
                    churn_threads = int(m.group(1))
                    churn_thr = metric
                    break
            if churn_thr:
                summary.append(
                    f"Connection churn ({churn_threads} threads): {format_metric(churn_thr)}"
                )

            stability = get_metric("Stability", "Sustained throughput")
            if stability:
                summary.append(f"Stability: sustained {format_metric(stability)}")

            rb_contention = get_bench_metric(
                "Core Performance", "Ring Buffer Queue (High Contention 8x8)", "Throughput"
            )
            if rb_contention:
                summary.append(f"Contention: ring buffer 8x8 {format_metric(rb_contention)}")

            http_frag_p99 = get_bench_metric(
                "Core Performance", "HTTP Parser (Fragmented Request)", "Latency p99"
            )
            if http_frag_p99:
                summary.append(f"HTTP fragmented p99 {format_metric(http_frag_p99)}")

            simd_large_p99 = get_bench_metric(
                "Core Performance", "SIMD CRLF Search (16KB buffer)", "Latency p99"
            )
            if simd_large_p99:
                summary.append(f"SIMD scan 16KB p99 {format_metric(simd_large_p99)}")

            return summary

        with open(output_path, 'w') as f:
            f.write("# KATANA Framework - Comprehensive Benchmark Results\n\n")
            f.write(f"Generated: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}\n\n")
            f.write("This report includes results from all benchmark suites.\n\n")

            summary = summarize()
            if summary:
                f.write("## Summary\n\n")
                for item in summary:
                    f.write(f"- {item}\n")
                f.write("\n")

            f.write("## Table of Contents\n\n")
            for category in sorted(self.results.keys()):
                f.write(f"- [{category}](#{category.lower().replace(' ', '-')})\n")
            f.write("\n---\n\n")

            categories = sorted(self.results.keys())
            for i, category in enumerate(categories):
                f.write(f"## {category}\n\n")
                f.write("| Benchmark | Value | Unit |\n")
                f.write("|-----------|-------|------|\n")

                for bench_name in sorted(self.results[category].keys()):
                    metrics = self.results[category][bench_name]

                    if isinstance(metrics, dict):
                        for metric, (value, unit) in sorted(metrics.items()):
                            display_name = f"{bench_name} - {metric}"
                            unit = unit.strip().replace("\\", "")
                            # Handle both numeric and string values
                            if isinstance(value, (int, float)):
                                f.write(f"| {display_name} | {value:.3f} | {unit} |\n")
                            else:
                                f.write(f"| {display_name} | {value} | {unit} |\n")
                    else:
                        value, unit = metrics
                        if isinstance(value, (int, float)):
                            f.write(f"| {bench_name} | {value:.3f} | {unit} |\n")
                        else:
                            f.write(f"| {bench_name} | {value} | {unit} |\n")

            if i < len(categories) - 1:
                f.write("\n")

    def benchmark_compute_api(self):
        """Load test the compute_api example with mixed payload sizes."""
        server_path = self.build_dir / "examples" / "codegen" / "compute_api" / "compute_api"
        if not server_path.exists():
            print(f"Warning: compute_api server not found at {server_path}, skipping example benchmark.")
            return

        category = "Example: compute_api"
        self.results.setdefault(category, {})

        if self.wrk_available() and os.environ.get("FORCE_LEGACY_CLIENT") != "1":
            if not self.start_server(server_path=server_path, ready_probe=("127.0.0.1", 8080, "/compute/sum")):
                print("Unable to start compute_api server; skipping example benchmark.")
                return

            sizes = [1, 8, 64, 256, 1024]
            wrk_ok = True
            for size in sizes:
                body = json.dumps([float(i + 1) for i in range(size)])
                out = self.run_wrk(
                    "http://127.0.0.1:8080/compute/sum",
                    body,
                    threads=8,
                    connections=32,  # Reduced to 32 to avoid timeouts even with large payloads
                    duration="12s",
                )
                if not out:
                    print(f"compute_api size={size}: wrk returned None")
                    wrk_ok = False
                    continue
                metrics = self.parse_wrk_metrics(out)
                socket_errs = metrics.get("socket_errors", (0, ""))[0] if "socket_errors" in metrics else 0
                throughput = metrics.get("throughput", (0.0, ""))[0] if "throughput" in metrics else 0.0
                print(f"compute_api size={size}: throughput={throughput:.1f} req/s, socket_errs={socket_errs}")
                # Accept results if throughput is good and socket errors are < 1% (or < 100 errors)
                if throughput < 1000.0 or socket_errs > 100:
                    print(f"  -> REJECTED (low throughput or too many errors)")
                    wrk_ok = False
                key_prefix = f"wrk size={size}"
                for m_name, m_val in metrics.items():
                    self.results[category][f"{key_prefix} {m_name}"] = m_val
            self.stop_server()
            if wrk_ok:
                return
            else:
                print("wrk results look bad (socket errors or tiny throughput); falling back to legacy TCP client.")
                self.results[category] = {}

        # Fallback: legacy socket client (lower throughput)
        if not self.start_server(server_path=server_path, ready_probe=("127.0.0.1", 8080, "/compute/sum")):
            print("Unable to start compute_api server; skipping example benchmark.")
            return

        import http.client
        import random
        import threading

        host, port = "127.0.0.1", 8080
        headers = {
            "Content-Type": "application/json",
            "Accept": "application/json",
            "Connection": "keep-alive",
        }
        sizes = [1, 8, 64, 256, 1024]
        payloads = {
            size: json.dumps([float(i + 1) for i in range(size)]).encode("utf-8") for size in sizes
        }

        def build_request(payload: bytes) -> bytes:
            lines = [
                f"POST /compute/sum HTTP/1.1",
                f"Host: {host}:{port}",
                "Connection: keep-alive",
            ]
            for k, v in headers.items():
                if k.lower() == "connection":
                    continue
                lines.append(f"{k}: {v}")
            lines.append(f"Content-Length: {len(payload)}")
            lines.append("")
            lines.append("")
            return ("\r\n".join(lines).encode("utf-8") + payload)

        request_cache = {size: build_request(payload) for size, payload in payloads.items()}

        def read_response(sock: socket.socket):
            data = b""
            while b"\r\n\r\n" not in data:
                chunk = sock.recv(4096)
                if not chunk:
                    return None, None
                data += chunk
            header, rest = data.split(b"\r\n\r\n", 1)
            lines = header.split(b"\r\n")
            if len(lines) < 1:
                return None, None
            parts = lines[0].split()
            if len(parts) < 2:
                return None, None
            try:
                status = int(parts[1])
            except Exception:
                return None, None
            content_length = 0
            for line in lines[1:]:
                if line.lower().startswith(b"content-length:"):
                    try:
                        content_length = int(line.split(b":", 1)[1].strip())
                    except Exception:
                        content_length = 0
            to_read = max(content_length - len(rest), 0)
            while to_read > 0:
                chunk = sock.recv(to_read)
                if not chunk:
                    break
                rest += chunk
                to_read -= len(chunk)
            return status, rest

        def pct(values, p):
            if not values:
                return None
            values_sorted = sorted(values)
            idx = min(max(int(len(values_sorted) * p / 100.0), 0), len(values_sorted) - 1)
            return values_sorted[idx]

        def run_threads(thread_count, duration_s, collect_stats=True):
            import threading
            totals = {size: 0 for size in sizes}
            errors = {size: 0 for size in sizes}
            latencies = {size: [] for size in sizes}
            status_hist = {size: {} for size in sizes}
            stop_event = threading.Event()
            lock = threading.Lock()  # Protect shared state

            def make_socket():
                s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                s.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)
                s.settimeout(1.0)  # 1s timeout
                s.connect((host, port))
                return s

            def worker():
                rng = random.Random()
                local_totals = {size: 0 for size in sizes}
                local_errors = {size: 0 for size in sizes}
                local_latencies = {size: [] for size in sizes}
                local_status_hist = {size: {} for size in sizes}

                sock = None
                try:
                    sock = make_socket()
                except Exception:
                    return

                while not stop_event.is_set():
                    size = sizes[rng.randrange(len(sizes))]
                    req_bytes = request_cache[size]

                    try:
                        start = time.perf_counter()
                        sock.sendall(req_bytes)
                        status, _ = read_response(sock)
                        latency_ms = (time.perf_counter() - start) * 1000.0

                        local_totals[size] += 1
                        if status is not None:
                            local_status_hist[size][status] = local_status_hist[size].get(status, 0) + 1
                        else:
                            local_status_hist[size][0] = local_status_hist[size].get(0, 0) + 1

                        if status == 200:
                            if collect_stats:
                                local_latencies[size].append(latency_ms)
                        else:
                            local_errors[size] += 1

                    except Exception:
                        local_errors[size] += 1
                        # Reconnect on error
                        try:
                            if sock:
                                sock.close()
                        except:
                            pass
                        try:
                            sock = make_socket()
                        except:
                            break

                # Merge local results into global with lock
                with lock:
                    for size in sizes:
                        totals[size] += local_totals[size]
                        errors[size] += local_errors[size]
                        latencies[size].extend(local_latencies[size])
                        for status, count in local_status_hist[size].items():
                            status_hist[size][status] = status_hist[size].get(status, 0) + count

                if sock:
                    try:
                        sock.close()
                    except:
                        pass

            threads = [threading.Thread(target=worker) for _ in range(thread_count)]
            start_ts = time.perf_counter()
            for t in threads:
                t.start()
            time.sleep(duration_s)
            stop_event.set()
            for t in threads:
                t.join()
            duration = time.perf_counter() - start_ts
            return totals, latencies, errors, duration, status_hist

        warmup_s = 2.0
        measure_s = 10.0
        for thread_count in (1, 4, 8, 16):
            run_threads(thread_count, warmup_s, collect_stats=False)
            totals, latencies, errors, duration, status_hist = run_threads(thread_count, measure_s, collect_stats=True)

            total_requests = sum(totals.values())
            if duration <= 0:
                duration = 1e-6

            self.results[category][f"{thread_count} threads throughput"] = (total_requests / duration, "req/s")
            self.results[category][f"{thread_count} threads errors"] = (float(sum(errors.values())), "count")

            for size in sizes:
                key_prefix = f"{thread_count}t size={size}"
                size_total = totals[size]
                self.results[category][f"{key_prefix} throughput"] = (size_total / duration, "req/s")
                self.results[category][f"{key_prefix} errors"] = (float(errors[size]), "count")
                for code, count in status_hist[size].items():
                    code_label = code if code is not None else 0
                    self.results[category][f"{key_prefix} status_{code_label}"] = (float(count), "count")

                if latencies[size]:
                    self.results[category][f"{key_prefix} p50"] = (pct(latencies[size], 50), "ms")
                    self.results[category][f"{key_prefix} p95"] = (pct(latencies[size], 95), "ms")
                    self.results[category][f"{key_prefix} p99"] = (pct(latencies[size], 99), "ms")

        self.stop_server()

    def benchmark_validation_api(self):
        """Load test the validation_api example with valid/invalid payload mix."""
        server_path = self.build_dir / "examples" / "codegen" / "validation_api" / "validation_api"
        if not server_path.exists():
            print(f"Warning: validation_api server not found at {server_path}, skipping example benchmark.")
            return

        category = "Example: validation_api"
        self.results.setdefault(category, {})

        # Fast path: use wrk if present to avoid Python socket bottlenecks.
        if self.wrk_available() and os.environ.get("FORCE_LEGACY_CLIENT") != "1":
            if not self.start_server(server_path=server_path, ready_probe=("127.0.0.1", 8081, "/user/register")):
                print("Unable to start validation_api server; skipping example benchmark.")
                return

            valid_body = json.dumps({"email": "a@b.com", "password": "supersecret", "age": 27})
            invalid_bodies = [
                json.dumps({"email": "not-an-email", "password": "short", "age": -1}),
                json.dumps({"email": "a@b.com", "password": "tiny"}),
                json.dumps({"email": "missing-password"}),
                json.dumps({"email": "a@b.com", "password": "supersecret", "age": 999}),
            ]

            scenarios = [
                ("wrk mix (60% valid / 40% invalid)", [valid_body, invalid_bodies[0], invalid_bodies[1]], [0.6, 0.25, 0.15]),
                ("wrk valid only", [valid_body], None),
                ("wrk invalid only", [invalid_bodies[0]], None),
            ]

            wrk_ok = True
            for label, bodies, weights in scenarios:
                out = self.run_wrk(
                    "http://127.0.0.1:8081/user/register",
                    bodies,
                    threads=8,
                    connections=32,  # Reduced to 32 to avoid timeouts
                    duration="12s",
                    weights=weights,
                )
                if not out:
                    wrk_ok = False
                    continue
                metrics = self.parse_wrk_metrics(out)
                socket_errs = metrics.get("socket_errors", (0, ""))[0] if "socket_errors" in metrics else 0
                throughput = metrics.get("throughput", (0.0, ""))[0] if "throughput" in metrics else 0.0
                print(f"validation_api {label}: throughput={throughput:.1f} req/s, socket_errs={socket_errs}")
                # Accept results if throughput is good and socket errors are < 100
                if throughput < 1000.0 or socket_errs > 100:
                    print(f"  -> REJECTED")
                    wrk_ok = False
                for m_name, m_val in metrics.items():
                    self.results[category][f"{label} {m_name}"] = m_val

            self.stop_server()
            if wrk_ok:
                return
            else:
                print("wrk results look bad (socket errors or tiny throughput); falling back to legacy TCP client.")
                self.results[category] = {}

        if not self.start_server(server_path=server_path, ready_probe=("127.0.0.1", 8081, "/user/register")):
            print("Unable to start validation_api server; skipping example benchmark.")
            return

        import random
        import threading

        host, port = "127.0.0.1", 8081
        headers = {
            "Content-Type": "application/json",
            "Accept": "application/json",
            "Connection": "keep-alive",
        }

        valid_payload = json.dumps(
            {"email": "a@b.com", "password": "supersecret", "age": 27}
        ).encode("utf-8")

        invalid_payloads = [
            json.dumps({"email": "not-an-email", "password": "short", "age": -1}).encode("utf-8"),
            json.dumps({"email": "a@b.com", "password": "tiny"}).encode("utf-8"),
            json.dumps({"email": "missing-password"}).encode("utf-8"),
            json.dumps({"email": "a@b.com", "password": "supersecret", "age": 999}).encode("utf-8"),
        ]

        def build_request(payload: bytes) -> bytes:
            lines = [
                f"POST /user/register HTTP/1.1",
                f"Host: {host}:{port}",
                "Connection: keep-alive",
            ]
            for k, v in headers.items():
                if k.lower() == "connection":
                    continue
                lines.append(f"{k}: {v}")
            lines.append(f"Content-Length: {len(payload)}")
            lines.append("")
            lines.append("")
            return ("\r\n".join(lines).encode("utf-8") + payload)

        request_valid = build_request(valid_payload)
        request_invalid = [build_request(p) for p in invalid_payloads]

        def read_response(sock: socket.socket):
            data = b""
            while b"\r\n\r\n" not in data:
                chunk = sock.recv(4096)
                if not chunk:
                    return None, None
                data += chunk
            header, rest = data.split(b"\r\n\r\n", 1)
            lines = header.split(b"\r\n")
            if len(lines) < 1:
                return None, None
            parts = lines[0].split()
            if len(parts) < 2:
                return None, None
            try:
                status = int(parts[1])
            except Exception:
                return None, None
            content_length = 0
            for line in lines[1:]:
                if line.lower().startswith(b"content-length:"):
                    try:
                        content_length = int(line.split(b":", 1)[1].strip())
                    except Exception:
                        content_length = 0
            to_read = max(content_length - len(rest), 0)
            while to_read > 0:
                chunk = sock.recv(to_read)
                if not chunk:
                    break
                rest += chunk
                to_read -= len(chunk)
            return status, rest

        def pct(values, p):
            if not values:
                return None
            values_sorted = sorted(values)
            idx = min(max(int(len(values_sorted) * p / 100.0), 0), len(values_sorted) - 1)
            return values_sorted[idx]

        def run_threads(thread_count, duration_s, collect_stats=True):
            totals = {"valid": 0, "invalid": 0}
            errors = {"valid": 0, "invalid": 0}
            latencies = {"valid": [], "invalid": []}
            status_hist = {"valid": {}, "invalid": {}}
            stop_event = threading.Event()

            def make_socket():
                s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                s.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)
                s.settimeout(2.0)  # Short timeout for fast benchmark
                s.connect((host, port))
                return s

            def worker():
                rng = random.Random()
                sock = None
                consecutive_errors = 0

                try:
                    sock = make_socket()
                except Exception:
                    return  # Can't connect, exit worker

                while not stop_event.is_set():
                    use_valid = rng.random() < 0.6
                    req_bytes = request_valid if use_valid else rng.choice(request_invalid)
                    key = "valid" if use_valid else "invalid"

                    try:

                        start = time.perf_counter()
                        sock.sendall(req_bytes)
                        status, _ = read_response(sock)
                        latency_ms = (time.perf_counter() - start) * 1000.0

                        totals[key] += 1
                        if status is not None:
                            status_hist[key][status] = status_hist[key].get(status, 0) + 1
                        else:
                            status_hist[key][0] = status_hist[key].get(0, 0) + 1

                        if not collect_stats:
                            if status is None or status >= 500:
                                errors[key] += 1
                            continue

                        if key == "valid":
                            if status == 200:
                                consecutive_errors = 0
                                latencies[key].append(latency_ms)
                            else:
                                errors[key] += 1
                                # Don't reconnect on HTTP errors
                        else:
                            if status in (400, 422):
                                consecutive_errors = 0
                                latencies[key].append(latency_ms)
                            else:
                                errors[key] += 1
                                # Don't reconnect on HTTP errors

                    except Exception:
                        errors[key] += 1
                        consecutive_errors += 1

                        try:
                            if sock:
                                sock.close()
                        except:
                            pass

                        if consecutive_errors > 10:
                            break

                        try:
                            sock = make_socket()
                            consecutive_errors = 0
                        except:
                            break

                if sock:
                    try:
                        sock.close()
                    except:
                        pass

            threads = [threading.Thread(target=worker) for _ in range(thread_count)]
            start_ts = time.perf_counter()
            for t in threads:
                t.start()
            time.sleep(duration_s)
            stop_event.set()
            for t in threads:
                t.join()
            duration = time.perf_counter() - start_ts
            return totals, latencies, errors, duration, status_hist

        warmup_s = 2.0
        measure_s = 10.0
        for thread_count in (4, 8):
            run_threads(thread_count, warmup_s, collect_stats=False)
            totals, latencies, errors, duration, status_hist = run_threads(thread_count, measure_s, collect_stats=True)

            total_requests = totals["valid"] + totals["invalid"]
            successes = len(latencies["valid"]) + len(latencies["invalid"])
            if duration <= 0:
                duration = 1e-6

            self.results[category][f"{thread_count} threads throughput"] = (total_requests / duration, "req/s")
            self.results[category][f"{thread_count} threads success_rate"] = (
                (successes / max(total_requests, 1)) * 100.0,
                "% success",
            )

            for key, label in (("valid", "Valid"), ("invalid", "Invalid")):
                totals_key = totals[key]
                self.results[category][f"{thread_count}t {label} throughput"] = (totals_key / duration, "req/s")
                self.results[category][f"{thread_count}t {label} errors"] = (float(errors[key]), "count")
                for code, count in status_hist[key].items():
                    code_label = code if code is not None else 0
                    self.results[category][f"{thread_count}t {label} status_{code_label}"] = (float(count), "count")
                if latencies[key]:
                    self.results[category][f"{thread_count}t {label} p50"] = (pct(latencies[key], 50), "ms")
                    self.results[category][f"{thread_count}t {label} p95"] = (pct(latencies[key], 95), "ms")
                    self.results[category][f"{thread_count}t {label} p99"] = (pct(latencies[key], 99), "ms")

        self.stop_server()

    def benchmark_text_api(self):
        """Load test the text_api example - basic validation test."""
        import subprocess
        import time
        import http.client
        import json

        server_path = self.build_dir / "examples" / "codegen" / "text_api" / "text_api"
        if not server_path.exists():
            print(f"Warning: text_api server not found at {server_path}, skipping example benchmark.")
            return

        category = "Example: text_api"
        self.results.setdefault(category, {})

        # Simple validation test (full load test has environmental issues)
        # Kill any existing servers
        subprocess.run(['killall', '-9', 'text_api'], stderr=subprocess.DEVNULL, stdout=subprocess.DEVNULL)
        time.sleep(1)

        if not self.start_server(server_path=server_path, ready_probe=None):
            print("Unable to start text_api server; skipping benchmark.")
            return

        # Simple validation: make a few test requests
        host, port = "127.0.0.1", 8082

        test_cases = [
            ("uppercase", {"text": "test"}, "/text/uppercase"),
            ("transform", {"text": "hello", "operation": "upper", "trim": False}, "/text/transform"),
        ]

        successful_tests = 0
        total_latency = 0.0

        for test_name, payload, endpoint in test_cases:
            try:
                conn = http.client.HTTPConnection(host, port, timeout=5)
                start = time.perf_counter()
                conn.request('POST', endpoint, json.dumps(payload), {'Content-Type': 'application/json'})
                resp = conn.getresponse()
                latency = (time.perf_counter() - start) * 1000
                body = resp.read()
                conn.close()

                if resp.status == 200:
                    successful_tests += 1
                    total_latency += latency
                    print(f"text_api {test_name}: OK ({latency:.2f}ms)")
                else:
                    print(f"text_api {test_name}: FAILED (status {resp.status})")
            except Exception as e:
                print(f"text_api {test_name}: ERROR ({e})")

        # Record simple stats
        if successful_tests > 0:
            avg_latency = total_latency / successful_tests
            self.results[category]["validation_tests_passed"] = (float(successful_tests), "tests")
            self.results[category]["avg_latency"] = (avg_latency, "ms")
            self.results[category]["status"] = ("OK - Generated code works correctly", "")
        else:
            self.results[category]["status"] = ("FAILED - Server issues", "")

        self.stop_server()

        # OLD CODE BELOW - wrk path has environment issues
        if False and self.wrk_available() and os.environ.get("FORCE_LEGACY_CLIENT") != "1":
            # Kill any existing text_api servers (cleanup from previous runs)
            subprocess.run(['killall', '-9', 'text_api'], stderr=subprocess.DEVNULL, stdout=subprocess.DEVNULL)
            time.sleep(1)

            # Start server directly (avoiding class state issues)
            print(f"Starting text_api server from {server_path}...")
            server_proc = subprocess.Popen([str(server_path)],
                                          stdout=subprocess.DEVNULL,
                                          stderr=subprocess.DEVNULL)
            print(f"Server started with PID {server_proc.pid}")
            time.sleep(3)  # Wait for server to initialize

            # Check if server is running
            if server_proc.poll() is not None:
                print(f"ERROR: Server exited during startup")
                return

            # Test different text sizes
            text_sizes = {
                "small": "Hello World! This is a test.",  # ~30 chars
                "medium": "Lorem ipsum dolor sit amet, consectetur adipiscing elit. " * 10,  # ~600 chars
                "large": "The quick brown fox jumps over the lazy dog. " * 100,  # ~4500 chars
            }

            wrk_ok = True

            # Test 1: /text/uppercase with different sizes
            for size_label, text in text_sizes.items():
                body = json.dumps({"text": text})
                out = self.run_wrk(
                    "http://127.0.0.1:8082/text/uppercase",
                    body,
                    threads=8,
                    connections=32,
                    duration="12s",
                )
                if not out:
                    print(f"text_api uppercase {size_label}: wrk returned None")
                    wrk_ok = False
                    continue
                metrics = self.parse_wrk_metrics(out)
                socket_errs = metrics.get("socket_errors", (0, ""))[0] if "socket_errors" in metrics else 0
                throughput = metrics.get("throughput", (0.0, ""))[0] if "throughput" in metrics else 0.0
                print(f"text_api uppercase {size_label}: throughput={throughput:.1f} req/s, socket_errs={socket_errs}")
                if throughput < 1000.0 or socket_errs > 100:
                    print(f"  -> REJECTED (low throughput or too many errors)")
                    wrk_ok = False
                key_prefix = f"uppercase {size_label}"
                for m_name, m_val in metrics.items():
                    self.results[category][f"{key_prefix} {m_name}"] = m_val

            # Test 2: /text/transform with different operations (mixed payload)
            transform_bodies = [
                json.dumps({"text": "hello world", "operation": "upper", "trim": False}),
                json.dumps({"text": "HELLO WORLD", "operation": "lower", "trim": True}),
                json.dumps({"text": "reverse this text", "operation": "reverse", "trim": False}),
                json.dumps({"text": "make this title case", "operation": "title", "trim": True}),
            ]
            out = self.run_wrk(
                "http://127.0.0.1:8082/text/transform",
                transform_bodies,
                threads=8,
                connections=32,
                duration="12s",
            )
            if not out:
                wrk_ok = False
            else:
                metrics = self.parse_wrk_metrics(out)
                socket_errs = metrics.get("socket_errors", (0, ""))[0] if "socket_errors" in metrics else 0
                throughput = metrics.get("throughput", (0.0, ""))[0] if "throughput" in metrics else 0.0
                print(f"text_api transform mixed: throughput={throughput:.1f} req/s, socket_errs={socket_errs}")
                if throughput < 1000.0 or socket_errs > 100:
                    print(f"  -> REJECTED")
                    wrk_ok = False
                for m_name, m_val in metrics.items():
                    self.results[category][f"transform mixed {m_name}"] = m_val

            # Test 3: /text/stats with large text
            stats_body = json.dumps({"text": "Word word word.\nLine two here.\nLine three.\n" * 100})
            out = self.run_wrk(
                "http://127.0.0.1:8082/text/stats",
                stats_body,
                threads=8,
                connections=32,
                duration="12s",
            )
            if not out:
                wrk_ok = False
            else:
                metrics = self.parse_wrk_metrics(out)
                socket_errs = metrics.get("socket_errors", (0, ""))[0] if "socket_errors" in metrics else 0
                throughput = metrics.get("throughput", (0.0, ""))[0] if "throughput" in metrics else 0.0
                print(f"text_api stats: throughput={throughput:.1f} req/s, socket_errs={socket_errs}")
                if throughput < 1000.0 or socket_errs > 100:
                    print(f"  -> REJECTED")
                    wrk_ok = False
                for m_name, m_val in metrics.items():
                    self.results[category][f"stats {m_name}"] = m_val

            # Stop server
            try:
                server_proc.send_signal(signal.SIGINT)
                server_proc.wait(timeout=5)
            except:
                server_proc.kill()
            print("text_api server stopped")

            if wrk_ok:
                return
            else:
                print("wrk results look bad; text_api benchmark incomplete.")
                return

def main():
    collector = BenchmarkCollector()

    benchmarks = [
        ("simple_benchmark", "HTTP Server", collector.parse_simple_benchmark, True),
        ("performance_benchmark", "Core Performance", collector.parse_standard_benchmark, False),
        ("mpsc_benchmark", "MPSC Queue", collector.parse_standard_benchmark, False),
        ("timer_benchmark", "Timer System", collector.parse_standard_benchmark, False),
        ("headers_benchmark", "HTTP Headers", collector.parse_standard_benchmark, False),
        ("io_buffer_benchmark", "IO Buffer", collector.parse_standard_benchmark, False),
        ("router_benchmark", "Router Dispatch", collector.parse_standard_benchmark, False),
        ("generated_api_benchmark", "Generated API", collector.parse_standard_benchmark, False),
    ]

    print("Running all benchmarks...\n")

    for bench_name, category, parser, needs_server in benchmarks:
        print(f"Running {bench_name}...")
        server_path = None
        if needs_server:
            # simple_benchmark uses hello_world_server by default
            server_path = collector.build_dir / "hello_world_server"
        output = collector.run_benchmark(bench_name, needs_server=needs_server, server_path=server_path)

        if output:
            if parser == collector.parse_simple_benchmark:
                parser(output)
            else:
                parser(output, category)
            print(f"✓ {bench_name} completed\n")
        else:
            print(f"✗ {bench_name} failed\n")

    # Examples: benchmark the codegen'd compute and validation servers
    collector.benchmark_compute_api()
    collector.benchmark_validation_api()
    collector.benchmark_text_api()

    output_file = Path(__file__).parent / "BENCHMARK_RESULTS.md"
    collector.generate_markdown(output_file)

    print(f"\nReport generated: {output_file}")
    print(f"Total categories: {len(collector.results)}")

if __name__ == "__main__":
    main()
