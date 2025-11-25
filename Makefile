# Convenience targets for local dev. Uses CMake presets from CMakePresets.json.

PRESET ?= debug
BUILD_DIR := build/$(PRESET)

.PHONY: help configure build test format lint bench fuzz profile clean

help:
	@echo "Common targets:"
	@echo "  make build PRESET=debug|release|asan|tsan|ubsan|io_uring-debug|io_uring-release"
	@echo "  make test  PRESET=debug (runs ctest with the chosen preset)"
	@echo "  make format (clang-format + cmake-format via pre-commit if available)"
	@echo "  make lint   (pre-commit run --all-files)"
	@echo "  make bench  (Release benchmarks preset + run performance_benchmark)"
	@echo "  make fuzz   (build+run HTTP parser fuzz target)"
	@echo "  make profile (Release simple_benchmark)"
	@echo "  make clean"

configure:
	cmake --preset $(PRESET)

build: configure
	cmake --build --preset $(PRESET)

test: build
	ctest --preset $(PRESET) --output-on-failure

format:
	@if command -v pre-commit >/dev/null 2>&1; then \
		pre-commit run clang-format cmake-format --all-files; \
	else \
		echo "pre-commit not found, running clang-format directly"; \
		find katana/core examples test benchmark -type f \( -name '*.c' -o -name '*.cc' -o -name '*.cpp' -o -name '*.cxx' -o -name '*.h' -o -name '*.hh' -o -name '*.hpp' \) -print0 | xargs -0 -r clang-format -i; \
	fi

lint:
	@if command -v pre-commit >/dev/null 2>&1; then \
		pre-commit run --all-files; \
	else \
		echo "pre-commit not installed; install with: pip install pre-commit && pre-commit install" && exit 1; \
	fi

bench:
	cmake --preset bench
	cmake --build --preset bench
	./build/bench/benchmark/performance_benchmark

fuzz:
	cmake --preset fuzz
	cmake --build --preset fuzz
	./build/fuzz/test/fuzz/http_parser_fuzz -max_total_time=60 -max_len=8192

profile:
	cmake --preset release
	cmake --build --preset release
	./build/release/benchmark/simple_benchmark

clean:
	rm -rf build
