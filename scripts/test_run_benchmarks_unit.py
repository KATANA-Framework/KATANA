import importlib.util
import pathlib
import sys
import unittest


SCRIPT_PATH = pathlib.Path(__file__).with_name("run_benchmarks.py")
SPEC = importlib.util.spec_from_file_location("run_benchmarks_module", SCRIPT_PATH)
run_benchmarks = importlib.util.module_from_spec(SPEC)
assert SPEC.loader is not None
sys.modules[SPEC.name] = run_benchmarks
SPEC.loader.exec_module(run_benchmarks)


def make_stage(
    *,
    kind: str,
    run_count: int,
    benchmark_name: str,
    benchmark_values: dict,
    benchmark_stats: dict,
    stage_id: int = 10,
):
    return {
        "stage_id": stage_id,
        "stage_name": f"stage-{stage_id}",
        "success": True,
        "run_count": run_count,
        "config": {"kind": kind},
        "benchmarks": [
            {
                "name": benchmark_name,
                **benchmark_values,
            }
        ],
        "benchmark_stats": {
            benchmark_name: benchmark_stats,
        },
    }


def make_report(stage: dict) -> dict:
    return {"stages": [stage]}


class CompareReportsWithCvTests(unittest.TestCase):
    def test_wrk_latency_regression_is_policy_skipped(self):
        benchmark_name = "wrk compute_api POST /compute/sum depth10"
        baseline = make_report(
            make_stage(
                kind="wrk_http",
                run_count=5,
                benchmark_name=benchmark_name,
                benchmark_values={"throughput": 1200.0, "latency_p95_us": 5000.0, "errors": 0},
                benchmark_stats={
                    "throughput": {"count": 5, "p50": 1200.0, "mean": 1200.0, "stddev": 10.0},
                    "latency_p95_us": {"count": 5, "p50": 5000.0, "mean": 5000.0, "stddev": 200.0},
                    "errors": {"count": 5, "p50": 0.0, "mean": 0.0, "stddev": 0.0},
                },
            )
        )
        current = make_report(
            make_stage(
                kind="wrk_http",
                run_count=5,
                benchmark_name=benchmark_name,
                benchmark_values={"throughput": 1200.0, "latency_p95_us": 900000.0, "errors": 0},
                benchmark_stats={
                    "throughput": {"count": 5, "p50": 1200.0, "mean": 1200.0, "stddev": 12.0},
                    "latency_p95_us": {"count": 5, "p50": 900000.0, "mean": 900000.0, "stddev": 200000.0},
                    "errors": {"count": 5, "p50": 0.0, "mean": 0.0, "stddev": 0.0},
                },
            )
        )

        summary = run_benchmarks.compare_reports_with_cv(
            current_report=current,
            baseline_report=baseline,
            threshold_pct=5.0,
            cv_threshold_pct=10.0,
        )

        self.assertEqual(summary["hard_regressions_count"], 0)
        self.assertEqual(summary["noisy_regressions_count"], 0)
        self.assertEqual(summary["compared_metrics"], 1)
        self.assertGreater(summary["policy_skipped_count"], 0)
        self.assertTrue(
            any(item["reason"] == "wrk_informational_only" for item in summary["policy_skipped"])
        )

    def test_wrk_single_run_throughput_regression_is_skipped(self):
        benchmark_name = "wrk compute_api POST /compute/sum depth10"
        baseline = make_report(
            make_stage(
                kind="wrk_http",
                run_count=5,
                benchmark_name=benchmark_name,
                benchmark_values={"throughput": 1200.0},
                benchmark_stats={
                    "throughput": {"count": 5, "p50": 1200.0, "mean": 1200.0, "stddev": 10.0},
                },
            )
        )
        current = make_report(
            make_stage(
                kind="wrk_http",
                run_count=1,
                benchmark_name=benchmark_name,
                benchmark_values={"throughput": 900.0},
                benchmark_stats={
                    "throughput": {"count": 1, "p50": 900.0, "mean": 900.0, "stddev": 0.0},
                },
            )
        )

        summary = run_benchmarks.compare_reports_with_cv(
            current_report=current,
            baseline_report=baseline,
            threshold_pct=5.0,
            cv_threshold_pct=10.0,
        )

        self.assertEqual(summary["hard_regressions_count"], 0)
        self.assertEqual(summary["compared_metrics"], 0)
        self.assertEqual(summary["policy_skipped_count"], 1)
        self.assertEqual(summary["policy_skipped"][0]["reason"], "wrk_requires_repeats")

    def test_wrk_repeated_throughput_regression_is_hard_failure(self):
        benchmark_name = "wrk compute_api POST /compute/sum depth10"
        baseline = make_report(
            make_stage(
                kind="wrk_http",
                run_count=5,
                benchmark_name=benchmark_name,
                benchmark_values={"throughput": 1200.0},
                benchmark_stats={
                    "throughput": {"count": 5, "p50": 1200.0, "mean": 1200.0, "stddev": 10.0},
                },
            )
        )
        current = make_report(
            make_stage(
                kind="wrk_http",
                run_count=5,
                benchmark_name=benchmark_name,
                benchmark_values={"throughput": 900.0},
                benchmark_stats={
                    "throughput": {"count": 5, "p50": 900.0, "mean": 900.0, "stddev": 8.0},
                },
            )
        )

        summary = run_benchmarks.compare_reports_with_cv(
            current_report=current,
            baseline_report=baseline,
            threshold_pct=5.0,
            cv_threshold_pct=10.0,
        )

        self.assertEqual(summary["hard_regressions_count"], 1)
        self.assertEqual(summary["policy_skipped_count"], 0)
        self.assertEqual(summary["regressions"][0]["metric"], "throughput")

    def test_microbenchmark_latency_regression_is_still_compared(self):
        benchmark_name = "Router dispatch (hits)"
        baseline = make_report(
            make_stage(
                kind="microbenchmark",
                run_count=5,
                benchmark_name=benchmark_name,
                benchmark_values={"latency_ns": 100.0},
                benchmark_stats={
                    "latency_ns": {"count": 5, "p50": 100.0, "mean": 100.0, "stddev": 1.0},
                },
                stage_id=4,
            )
        )
        current = make_report(
            make_stage(
                kind="microbenchmark",
                run_count=5,
                benchmark_name=benchmark_name,
                benchmark_values={"latency_ns": 130.0},
                benchmark_stats={
                    "latency_ns": {"count": 5, "p50": 130.0, "mean": 130.0, "stddev": 2.0},
                },
                stage_id=4,
            )
        )

        summary = run_benchmarks.compare_reports_with_cv(
            current_report=current,
            baseline_report=baseline,
            threshold_pct=5.0,
            cv_threshold_pct=10.0,
        )

        self.assertEqual(summary["hard_regressions_count"], 1)
        self.assertEqual(summary["policy_skipped_count"], 0)
        self.assertEqual(summary["regressions"][0]["metric"], "latency_ns")


if __name__ == "__main__":
    unittest.main()
