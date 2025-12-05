# Convenience targets for local dev and CI. Uses CMake presets from CMakePresets.json.

PRESET ?= debug
BUILD_DIR := build/$(PRESET)
DOCKER ?= docker
DOCKER_BENCH_IMAGE ?= katana-bench-epoll
DOCKER_PROFILE_IMAGE ?= katana-profile
PERF_PATH ?= $(HOME)/src/WSL2-Linux-Kernel/tools/perf/perf

.PHONY: help configure build test format lint bench bench-report fuzz profile clean \
	docker-bench-image docker-bench docker-profile-image docker-profile

help:
	@echo "Common targets:"
	@echo "  make build PRESET=debug|release|asan|tsan|ubsan|io_uring-debug|io_uring-release"
	@echo "  make test  PRESET=debug (runs ctest with the chosen preset)"
	@echo "  make format (clang-format + cmake-format via pre-commit if available)"
	@echo "  make lint   (pre-commit run --all-files)"
	@echo "  make bench  (Release benchmarks preset + run performance_benchmark)"
	@echo "  make bench-report (full run_benchmarks.sh: builds, runs all benches, generates BENCHMARK_RESULTS.md)"
	@echo "  make fuzz   (build+run HTTP parser fuzz target)"
	@echo "  make profile (Release simple_benchmark)"
	@echo "  make docker-bench-image / docker-bench (build & run benchmark container)"
	@echo "  make docker-profile-image / docker-profile (build & run perf container; PERF_PATH overrides host perf)"
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

bench-report:
	./run_benchmarks.sh

fuzz:
	cmake --preset fuzz
	cmake --build --preset fuzz
	./build/fuzz/test/fuzz/http_parser_fuzz -max_total_time=60 -max_len=8192

profile:
	cmake --preset release
	cmake --build --preset release
	./build/release/benchmark/simple_benchmark

docker-bench-image:
	$(DOCKER) build -f docker/benchmarks/Dockerfile.epoll -t $(DOCKER_BENCH_IMAGE) .

docker-bench: docker-bench-image
	$(DOCKER) run --rm -v "$$(pwd)":/workspace $(DOCKER_BENCH_IMAGE)

docker-profile-image:
	$(DOCKER) build -f docker/profiling/Dockerfile.profile -t $(DOCKER_PROFILE_IMAGE) .

docker-profile: docker-profile-image
	$(DOCKER) run --rm -v "$$(pwd)":/workspace -e HOST_PERF="$(PERF_PATH)" $(DOCKER_PROFILE_IMAGE)

clean:
	rm -rf build
