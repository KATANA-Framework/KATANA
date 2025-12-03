#!/usr/bin/env python3

import subprocess
import re
import sys
import time
import signal
from datetime import datetime
from pathlib import Path

class BenchmarkCollector:
    def __init__(self):
        self.results = {}
        self.build_dir = Path(__file__).parent / "build"
        self.server_proc = None

    def start_server(self):
        server_path = self.build_dir / "hello_world_server"
        if not server_path.exists():
            print(f"Warning: hello_world_server not found at {server_path}")
            return False

        try:
            self.server_proc = subprocess.Popen([str(server_path)],
                                                stdout=subprocess.DEVNULL,
                                                stderr=subprocess.DEVNULL)
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

    def run_benchmark(self, name, needs_server=False):
        bench_path = self.build_dir / "benchmark" / name
        if not bench_path.exists():
            print(f"Warning: {name} not found, skipping...")
            return None

        if needs_server and not self.start_server():
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
                            f.write(f"| {display_name} | {value:.3f} | {unit} |\n")
                    else:
                        value, unit = metrics
                        f.write(f"| {bench_name} | {value:.3f} | {unit} |\n")

                if i < len(categories) - 1:
                    f.write("\n")

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
        output = collector.run_benchmark(bench_name, needs_server=needs_server)

        if output:
            if parser == collector.parse_simple_benchmark:
                parser(output)
            else:
                parser(output, category)
            print(f"✓ {bench_name} completed\n")
        else:
            print(f"✗ {bench_name} failed\n")

    output_file = Path(__file__).parent / "BENCHMARK_RESULTS.md"
    collector.generate_markdown(output_file)

    print(f"\nReport generated: {output_file}")
    print(f"Total categories: {len(collector.results)}")

if __name__ == "__main__":
    main()
