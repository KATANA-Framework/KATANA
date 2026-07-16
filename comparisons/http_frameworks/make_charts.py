#!/usr/bin/env python3
"""Render report charts (RPS + p99 small multiples) from a results dataset.

Usage: make_charts.py <dataset.json> <out_dir> <title_suffix>
"""

import json
import sys
from pathlib import Path

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt

# color follows the framework (entity), fixed across every chart
FRAMEWORK_COLORS = {
    "katana": "#2a78d6",
    "actix-web": "#008300",
    "axum": "#e87ba4",
    "ntex": "#eda100",
    "drogon": "#1baf7a",
    "fastapi": "#eb6834",
}
FRAMEWORK_ORDER = ["katana", "actix-web", "axum", "ntex", "drogon", "fastapi"]
SCENARIO_ORDER = [
    "json-small", "echo-json", "stats-128", "json-pipe-10",
    "json-pipe-20", "db-point-select", "db-list", "db-insert-returning",
]

TEXT_PRIMARY = "#1a1a19"
TEXT_SECONDARY = "#5f5e58"
SURFACE = "#fcfcfb"
GRID = "#e8e7e2"


def human(value: float) -> str:
    if value >= 1_000_000:
        return f"{value / 1_000_000:.2f}M"
    if value >= 10_000:
        return f"{value / 1000:.0f}k"
    if value >= 1_000:
        return f"{value / 1000:.1f}k"
    return f"{value:.0f}"


def small_multiples(data, metric_key, metric_label, fname, out_dir, suffix, unit_ms=False):
    fig, axes = plt.subplots(2, 4, figsize=(16, 7.5), facecolor=SURFACE)
    fig.suptitle(f"{metric_label} — {suffix}", fontsize=15, color=TEXT_PRIMARY, fontweight="bold")

    for ax, scenario in zip(axes.flat, SCENARIO_ORDER):
        rows = {r["framework"]: r for r in data if r["scenario"] == scenario}
        frameworks = [f for f in FRAMEWORK_ORDER if f in rows]
        values, colors, labels = [], [], []
        for framework in frameworks:
            raw = rows[framework]["median"].get(metric_key)
            value = (raw or 0) / (1000 if unit_ms else 1)
            values.append(value)
            colors.append(FRAMEWORK_COLORS[framework])
            labels.append("n/a" if not raw else human(value))

        ax.set_facecolor(SURFACE)
        bars = ax.barh(range(len(frameworks)), values, color=colors, height=0.62)
        ax.set_yticks(range(len(frameworks)))
        ax.set_yticklabels(frameworks, fontsize=9, color=TEXT_PRIMARY)
        ax.invert_yaxis()
        ax.set_title(scenario, fontsize=11, color=TEXT_PRIMARY)
        ax.tick_params(axis="x", labelsize=8, colors=TEXT_SECONDARY)
        ax.grid(axis="x", color=GRID, linewidth=0.8)
        ax.set_axisbelow(True)
        for spine in ("top", "right", "left"):
            ax.spines[spine].set_visible(False)
        ax.spines["bottom"].set_color(GRID)
        span = max(values) if values else 1
        for bar, label in zip(bars, labels):
            ax.text(
                bar.get_width() + span * 0.02,
                bar.get_y() + bar.get_height() / 2,
                label,
                va="center", fontsize=8.5, color=TEXT_PRIMARY,
            )
        ax.set_xlim(0, span * 1.22 if span else 1)

    fig.tight_layout(rect=(0, 0, 1, 0.95))
    out = Path(out_dir) / fname
    fig.savefig(out, dpi=130, facecolor=SURFACE)
    plt.close(fig)
    print("wrote", out)


def main() -> None:
    dataset, out_dir, suffix = sys.argv[1], sys.argv[2], sys.argv[3]
    Path(out_dir).mkdir(parents=True, exist_ok=True)
    data = json.load(open(dataset))
    small_multiples(data, "rps", "Throughput, requests/sec (median of 3, higher is better)",
                    f"rps_{Path(dataset).stem}.png", out_dir, suffix)
    small_multiples(data, "latency_p99_us", "Latency p99, ms (median of 3, lower is better; n/a = wrk artifact)",
                    f"p99_{Path(dataset).stem}.png", out_dir, suffix, unit_ms=True)


if __name__ == "__main__":
    main()
