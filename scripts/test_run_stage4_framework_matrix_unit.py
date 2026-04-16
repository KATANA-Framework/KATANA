import importlib.util
import pathlib
import sys
import unittest
from unittest import mock


SCRIPT_PATH = pathlib.Path(__file__).with_name("run_stage4_framework_matrix.py")
SPEC = importlib.util.spec_from_file_location("run_stage4_framework_matrix_module", SCRIPT_PATH)
matrix = importlib.util.module_from_spec(SPEC)
assert SPEC.loader is not None
sys.modules[SPEC.name] = matrix
SPEC.loader.exec_module(matrix)


class Stage4FrameworkMatrixTests(unittest.TestCase):
    def test_resolve_postgres_dsn_prefers_cli(self):
        self.assertEqual(matrix.resolve_postgres_dsn("postgres://cli"), "postgres://cli")

    def test_resolve_postgres_dsn_falls_back_to_env(self):
        with mock.patch.dict("os.environ", {"KATANA_TEST_POSTGRES_DSN": "postgres://env"}, clear=True):
            self.assertEqual(matrix.resolve_postgres_dsn(None), "postgres://env")

    def test_framework_configs_include_all_expected_targets(self):
        fake_build_dir = pathlib.Path("/tmp/katana-build")
        with mock.patch.object(matrix, "discover_katana_build_dir", return_value=fake_build_dir):
            configs = matrix.framework_configs(
                workers=4,
                postgres_dsn="postgres://bench",
                seed_count=2048,
                jobs=8,
            )

        self.assertEqual(set(configs), {"katana", "actix-web", "axum", "drogon", "ntex", "fastapi"})
        self.assertEqual(configs["actix-web"].port, 19080)
        self.assertTrue(configs["actix-web"].command[-1].endswith("katana_comparison_actix"))
        self.assertEqual(configs["katana"].port, 19090)
        self.assertTrue(configs["axum"].command[-1].endswith("katana_comparison_axum"))
        self.assertEqual(configs["ntex"].port, 19084)
        self.assertTrue(configs["ntex"].command[-1].endswith("katana_comparison_ntex"))
        self.assertEqual(configs["fastapi"].command[-1], "app.py")


if __name__ == "__main__":
    unittest.main()
