import importlib.util
import pathlib
import sys
import unittest


SCRIPT_PATH = pathlib.Path(__file__).with_name("profile_e2e_instruction_perf.py")
SPEC = importlib.util.spec_from_file_location("profile_e2e_instruction_perf_module", SCRIPT_PATH)
profile_perf = importlib.util.module_from_spec(SPEC)
assert SPEC.loader is not None
sys.modules[SPEC.name] = profile_perf
SPEC.loader.exec_module(profile_perf)


class ExtractTopSymbolsTests(unittest.TestCase):
    def test_extract_top_symbols_skips_duplicates_and_unknowns(self):
        report = """
            37.11%  hello_world_server  hello_world_server  [.] katana::http::Server::run
            18.40%  hello_world_server  hello_world_server  [.] katana::http::Server::run
            11.02%  hello_world_server  hello_world_server  [.] katana::Router::dispatch
             4.50%  hello_world_server  [unknown]          [.] [unknown]
        """

        symbols = profile_perf.extract_top_symbols_from_report(report, max_count=4)

        self.assertEqual(
            symbols,
            [
                {"symbol": "katana::http::Server::run", "overhead_pct": 37.11},
                {"symbol": "katana::Router::dispatch", "overhead_pct": 11.02},
            ],
        )

    def test_sanitize_filename_collapses_non_ascii_and_punctuation(self):
        sanitized = profile_perf.sanitize_filename("katana::json::parse(std::span<const char>)")
        self.assertEqual(sanitized, "katana_json_parse_std_span_const_char")


if __name__ == "__main__":
    unittest.main()
