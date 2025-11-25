#!/usr/bin/env python3

from __future__ import annotations

import re
import subprocess
from pathlib import Path
from typing import Dict, List, Optional, Tuple

ROOT = Path(__file__).resolve().parent.parent
README_PATH = ROOT / "README.md"
BENCHMARK_PATH = ROOT / "BENCHMARK_RESULTS.md"

BENCH_START = "<!-- BENCH_SUMMARY_START -->"
BENCH_END = "<!-- BENCH_SUMMARY_END -->"
LATEST_START = "<!-- LATEST_UPDATE_START -->"
LATEST_END = "<!-- LATEST_UPDATE_END -->"


def parse_benchmark_results(path: Path) -> Tuple[Optional[str], Dict[str, Dict[str, Tuple[float, str]]]]:
    if not path.exists():
        raise FileNotFoundError(f"Benchmark report not found: {path}")

    text = path.read_text(encoding="utf-8")
    generated_match = re.search(r"^Generated:\s*(.+)$", text, re.MULTILINE)
    generated = generated_match.group(1).strip() if generated_match else None

    results: Dict[str, Dict[str, Tuple[float, str]]] = {}
    current_category: Optional[str] = None
    line_re = re.compile(
        r"^\|\s*(?P<name>[^|]+?)\s*\|\s*(?P<value>-?[\d.]+)\s*\|\s*(?P<unit>[^|]+?)\s*\|"
    )

    for line in text.splitlines():
        if line.startswith("## "):
            current_category = line[3:].strip()
            continue

        match = line_re.match(line)
        if not match or not current_category:
            continue

        try:
            value = float(match.group("value"))
        except ValueError:
            continue

        name = match.group("name").strip()
        unit = match.group("unit").strip()
        results.setdefault(current_category, {})[name] = (value, unit)

    return generated, results


def fmt_value(value: float) -> str:
    text = f"{value:.3f}".rstrip("0").rstrip(".")
    return text or "0"


def get_metric(results: Dict[str, Dict[str, Tuple[float, str]]], category: str, name: str) -> Optional[Tuple[float, str]]:
    return results.get(category, {}).get(name)


def build_bench_lines(generated: Optional[str], results: Dict[str, Dict[str, Tuple[float, str]]]) -> List[str]:
    lines: List[str] = []

    if generated:
        lines.append(f"* Отчёт сгенерирован {generated}.")

    ka_thr = get_metric(results, "Core Performance", "Keep-alive throughput")
    ka_p99 = get_metric(results, "Core Performance", "Latency p99")
    ka_samples = get_metric(results, "Core Performance", "Latency samples")
    if ka_thr or ka_p99 or ka_samples:
        parts: List[str] = []
        if ka_thr:
            parts.append(f"throughput {fmt_value(ka_thr[0])} {ka_thr[1]}")
        if ka_p99:
            parts.append(f"p99 {fmt_value(ka_p99[0])} {ka_p99[1]}")
        if ka_samples:
            parts.append(f"{fmt_value(ka_samples[0])} samples")
        lines.append(f"* Keep-alive: {', '.join(parts)}.")

    scale_128 = get_metric(results, "Scalability", "128 concurrent connections")
    threads_8 = get_metric(results, "Scalability", "Throughput with 8 threads")
    if scale_128 or threads_8:
        parts = []
        if scale_128:
            parts.append(f"128 коннектов — {fmt_value(scale_128[0])} {scale_128[1]}")
        if threads_8:
            parts.append(f"8 потоков — {fmt_value(threads_8[0])} {threads_8[1]}")
        lines.append(f"* Масштабирование: {'; '.join(parts)}.")

    sustained = get_metric(results, "Stability", "Sustained throughput")
    total = get_metric(results, "Stability", "Total requests")
    if sustained or total:
        parts = []
        if sustained:
            parts.append(f"sustained {fmt_value(sustained[0])} {sustained[1]}")
        if total:
            parts.append(f"всего {fmt_value(total[0])} {total[1]}")
        lines.append(f"* Устойчивость: {', '.join(parts)}.")

    if not lines:
        lines.append("* Нет данных о бенчмарках.")

    return lines


def latest_commit_line(repo_root: Path) -> Optional[str]:
    try:
        output = subprocess.check_output(
            [
                "git",
                "-C",
                str(repo_root),
                "log",
                "-1",
                "--date=format:%d.%m %H:%M",
                "--format=%ad — %s (%h)",
            ],
            text=True,
        )
    except (subprocess.CalledProcessError, FileNotFoundError):
        return None

    return output.strip() or None


def replace_block(text: str, start: str, end: str, lines: List[str]) -> str:
    pattern = re.compile(re.escape(start) + r"(.*?)" + re.escape(end), re.DOTALL)
    if not pattern.search(text):
        raise ValueError(f"Markers {start} / {end} not found in README.md")

    replacement = f"{start}\n" + "\n".join(lines) + f"\n{end}"
    return pattern.sub(replacement, text, count=1)


def main() -> None:
    generated, results = parse_benchmark_results(BENCHMARK_PATH)
    bench_lines = build_bench_lines(generated, results)

    commit_line = latest_commit_line(ROOT)
    latest_lines = [f"* {commit_line}"] if commit_line else ["* Не удалось получить информацию о последнем коммите."]

    content = README_PATH.read_text(encoding="utf-8")
    content = replace_block(content, BENCH_START, BENCH_END, bench_lines)
    content = replace_block(content, LATEST_START, LATEST_END, latest_lines)
    README_PATH.write_text(content, encoding="utf-8")


if __name__ == "__main__":
    main()
