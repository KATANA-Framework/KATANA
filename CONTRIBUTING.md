# Contributing

Thanks for helping develop KATANA. When adding features, keep the documentation in sync with the code and don't promise things that aren't implemented yet.

## Environment

- Dependencies: CMake ≥ 3.20, Ninja, GCC ≥ 12 or Clang ≥ 16. The io_uring presets need `liburing-dev`.
- Configuration via `CMakePresets.json`:
  - Debug: `cmake --preset debug && cmake --build --preset debug`
  - Sanitizers: `--preset asan|tsan|ubsan`
  - io_uring: `--preset io_uring-debug` / `io_uring-release`
  - Examples/benchmarks: `--preset examples` / `bench`
- Tests: `ctest --preset debug`, or `ctest --test-dir build/<preset> -j`. The suite uses a lightweight gtest-compatible harness in `test/gtest/`. Running with `-j` is fine: the codegen tests are hermetic and use per-process temp dirs.
- Fuzzing: `cmake --preset fuzz && cmake --build --preset fuzz && ./build/fuzz/test/fuzz/http_parser_fuzz -max_total_time=60`.

There are Makefile shortcuts for the common flows: `make build`, `make test`, `make ci`, `make format`, `make lint`.

**Build note (GCC 15 / CMake 3.31):** add `-DCMAKE_CXX_SCAN_FOR_MODULES=OFF` to the configure step. The project uses no C++20 modules, and CMake's module scanning breaks with that toolchain.

## Quality and style

- Formatting: `clang-format` per `.clang-format`. Use `pre-commit install` (see `.pre-commit-config.yaml`) before committing.
- Static analysis: `.clang-tidy` is enabled; after configuring you can run `clang-tidy -p build/debug <files>`.
- Avoid unjustified global state and raw `new/delete`; prefer RAII and `std::expected`/`monotonic_arena`.

## Testing

- Unit and integration tests live in `test/unit` and `test/integration`.
- When adding new subsystems, add fixture-based tests, and keep minimal reproductions for regressions.

## Documentation

- README/ARCHITECTURE describe the target architecture. If functionality is added, or conversely not yet implemented, record that in the relevant sections.
- Document new CLI flags, build options, and test parameters in the README or a separate file under `docs/`.

## PR Checklist

- [ ] `cmake --build --preset debug`
- [ ] `ctest --preset debug`
- [ ] `pre-commit run --all-files`
- [ ] Relevant .md files updated (especially when actual functionality changes)
