#!/usr/bin/env bash
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
TEST_BUILD_DIR="${KATANA_SQL_TEST_BUILD_DIR:-/tmp/katana-stage4-build}"
BENCH_BUILD_DIR="${KATANA_SQL_BENCH_BUILD_DIR:-/tmp/katana-stage4-bench}"
COMPOSE_FILE="${REPO_ROOT}/docker/sql/docker-compose.yml"
COMPOSE_PROJECT="katana-stage4-sql"
POSTGRES_DSN="host=127.0.0.1 port=55432 dbname=katana_stage4 user=katana password=katana sslmode=disable"

cleanup() {
    docker compose -f "${COMPOSE_FILE}" -p "${COMPOSE_PROJECT}" down -v >/dev/null 2>&1 || true
}

trap cleanup EXIT

cmake -S "${REPO_ROOT}" -B "${TEST_BUILD_DIR}" -G Ninja \
    -DCMAKE_BUILD_TYPE=Debug \
    -DENABLE_TESTING=ON \
    -DCMAKE_CXX_SCAN_FOR_MODULES=OFF

cmake -S "${REPO_ROOT}" -B "${BENCH_BUILD_DIR}" -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DENABLE_BENCHMARKS=ON \
    -DENABLE_TESTING=OFF \
    -DCMAKE_CXX_SCAN_FOR_MODULES=OFF

cmake --build "${TEST_BUILD_DIR}" --target sql_integration_tests -j4
cmake --build "${BENCH_BUILD_DIR}" --target sql_postgres_benchmark sql_postgres_throughput_benchmark benchmark_api_sql_server -j4

docker compose -f "${COMPOSE_FILE}" -p "${COMPOSE_PROJECT}" up -d

for _ in $(seq 1 30); do
    if docker compose -f "${COMPOSE_FILE}" -p "${COMPOSE_PROJECT}" exec -T postgres \
        pg_isready -U katana -d katana_stage4 >/dev/null 2>&1; then
        break
    fi
    sleep 1
done

docker compose -f "${COMPOSE_FILE}" -p "${COMPOSE_PROJECT}" exec -T postgres \
    pg_isready -U katana -d katana_stage4 >/dev/null

echo "[sql-smoke] running sql_integration_tests"
KATANA_TEST_POSTGRES_DSN="${POSTGRES_DSN}" \
    "${TEST_BUILD_DIR}/test/sql_integration_tests"

echo "[sql-smoke] running sql_postgres_benchmark"
KATANA_TEST_POSTGRES_DSN="${POSTGRES_DSN}" \
    "${BENCH_BUILD_DIR}/benchmark/sql_postgres_benchmark"

echo "[sql-smoke] running sql_postgres_throughput_benchmark"
KATANA_TEST_POSTGRES_DSN="${POSTGRES_DSN}" \
    "${BENCH_BUILD_DIR}/benchmark/sql_postgres_throughput_benchmark"

echo "[sql-smoke] running stages 15, 16, 17 and 18 via benchmark runner"
KATANA_TEST_POSTGRES_DSN="${POSTGRES_DSN}" \
    python3 "${REPO_ROOT}/scripts/run_benchmarks.py" \
        --build-dir "${BENCH_BUILD_DIR}" \
        --no-build \
        --stage 15 16 17 18 \
        --stage-repeat 17=1 \
        --stage-repeat 18=1 \
        --json
