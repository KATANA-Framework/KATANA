import importlib.util
import json
import pathlib
import sys
import tempfile
import unittest


SCRIPT_PATH = pathlib.Path(__file__).with_name("run_framework_benchmarks.py")
SPEC = importlib.util.spec_from_file_location("run_framework_benchmarks_module", SCRIPT_PATH)
framework_bench = importlib.util.module_from_spec(SPEC)
assert SPEC.loader is not None
sys.modules[SPEC.name] = framework_bench
SPEC.loader.exec_module(framework_bench)


class TargetParsingTests(unittest.TestCase):
    def test_parse_target_arg_normalizes_root_path(self):
        target = framework_bench.parse_target_arg("axum=http://192.168.0.104:19081")
        self.assertEqual(target.name, "axum")
        self.assertEqual(target.base_url, "http://192.168.0.104:19081/")

    def test_resolve_target_url_prefers_scenario_override(self):
        target = framework_bench.TargetConfig(
            name="katana",
            base_url=None,
            scenario_urls={
                "hello-keepalive": "http://192.168.0.104:18080/",
                "hello-canonical": "http://192.168.0.104:18080/",
                "compute-keepalive": "http://192.168.0.104:18081/",
                "compute-canonical": "http://192.168.0.104:18081/",
            },
        )

        self.assertEqual(
            framework_bench.resolve_target_url(target, "hello-keepalive"),
            "http://192.168.0.104:18080/",
        )
        self.assertEqual(
            framework_bench.resolve_target_url(target, "hello-canonical"),
            "http://192.168.0.104:18080/",
        )
        self.assertEqual(
            framework_bench.resolve_target_url(target, "compute-keepalive"),
            "http://192.168.0.104:18081/",
        )
        self.assertEqual(
            framework_bench.resolve_target_url(target, "compute-canonical"),
            "http://192.168.0.104:18081/",
        )

    def test_load_targets_file_accepts_base_url_and_scenario_urls(self):
        payload = {
            "targets": [
                {"name": "actix-web", "base_url": "http://192.168.0.104:19080"},
                {
                    "name": "katana",
                    "scenario_urls": {
                        "hello-keepalive": "http://192.168.0.104:18080/",
                        "hello-canonical": "http://192.168.0.104:18080/",
                        "compute-keepalive": "http://192.168.0.104:18081/",
                        "compute-canonical": "http://192.168.0.104:18081/",
                    },
                },
            ]
        }

        with tempfile.TemporaryDirectory() as tmpdir:
            path = pathlib.Path(tmpdir) / "targets.json"
            path.write_text(json.dumps(payload), encoding="utf-8")
            targets = framework_bench.load_targets_file(path)

        self.assertEqual(len(targets), 2)
        self.assertEqual(targets[0].base_url, "http://192.168.0.104:19080/")
        self.assertEqual(
            targets[1].scenario_urls["compute-canonical"],
            "http://192.168.0.104:18081/",
        )
        self.assertEqual(
            targets[1].scenario_urls["hello-keepalive"],
            "http://192.168.0.104:18080/",
        )


if __name__ == "__main__":
    unittest.main()
