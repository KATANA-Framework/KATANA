# HTTP Framework Comparison Suite

This directory contains small benchmark targets for the maintained KATANA E2E
scenarios:

- `GET /` using `test/load/scripts/hello_pipeline.lua`
- `POST /compute/sum` using `test/load/scripts/compute_sum_pipeline.lua`

The goal is not feature parity with each framework. The goal is to benchmark the
same hot paths and payloads that KATANA already measures in stages 9-12.

## What Was Compared

The comparison matrix covers:

- KATANA
- Rust `actix-web`
- Rust `axum`
- Rust `ntex`
- C++ `Drogon`
- Python `FastAPI`

Every server was benchmarked only on the two maintained HTTP paths:

- `GET /` returning `Hello, World!`
- `POST /compute/sum` returning the sum of a JSON number array

This keeps the comparison focused on the same user-visible scenarios that
KATANA already tracks in its own benchmark stages.

## How It Was Compared

The runs were performed with the same benchmark inputs and runner logic used by
KATANA:

- same Lua scripts:
  - `test/load/scripts/hello_pipeline.lua`
  - `test/load/scripts/compute_sum_pipeline.lua`
- same wrk shape:
  - canonical: `t=4`, `c=512`, `10s`
  - peak: `t=4`, `c=512`, `5s`
- same worker count for every server: `4`
- same aggregation policy: `median of 3`
- sequential execution only:
  - only one server was running during a measurement
  - no parallel framework runs

The runner for this suite is [run_framework_benchmarks.py](C:/Users/Ya/OneDrive/Desktop/KATANA/scripts/run_framework_benchmarks.py),
which reuses the same wrk parsing approach as
[run_benchmarks.py](C:/Users/Ya/OneDrive/Desktop/KATANA/scripts/run_benchmarks.py).

## Where It Was Compared

The comparison environment used for the recorded runs was:

- Ubuntu 24.04 VM
- framework servers running inside the VM
- `wrk` also running inside the same VM

That means the benchmark is fair at the HTTP scenario level, but it is still a
same-host measurement, not a full cross-machine network-stack benchmark. For
final latency claims, rerun from a separate load-generator machine against the
VM IP.

## Latest Snapshot

The current comparison snapshot is shown below.

![HTTP framework comparison table](../table.jpg)

## Layout

- `actix_web/` - Rust `actix-web` server
- `axum/` - Rust `axum` server
- `ntex/` - Rust `ntex` server
- `drogon/` - C++ Drogon server
- `fastapi/` - Python FastAPI server
- `targets.example.json` - ready-to-edit benchmark target matrix

Each server intentionally implements only the two endpoints used by the current
wrk stages:

- `GET /` -> `Hello, World!`
- `POST /compute/sum` -> JSON number equal to the sum of the input array

For `/compute/sum`, every implementation enforces the same request shape that
the KATANA OpenAPI example uses:

- JSON array of numbers
- minimum 1 item
- maximum 1024 items

## Why a Separate Folder

The comparison servers are isolated on purpose:

- no changes to KATANA runtime or main CMake graph
- no foreign framework dependencies added to the main build
- easy to copy or deploy only the comparison suite onto a Linux host

## Recommended Methodology

If you want real network-stack numbers, do not run wrk on the same machine as
the server. Run the framework servers on the Linux VM and run
`scripts/run_framework_benchmarks.py` from another machine against the VM IP.

That preserves the existing wrk scenarios while adding a real TCP path between
client and server.

## Prerequisites on the Linux Server

Use a machine with enough free disk for multiple toolchains. In practice, plan
for at least:

- 2 GB free disk
- Rust toolchain for `actix-web`, `axum`, and `ntex`
- Python 3 + venv/pip for FastAPI
- Drogon installed and discoverable by CMake for the C++ target
- `wrk` on the machine that generates load

## Example Build and Run Commands

### actix-web

```bash
cd comparisons/http_frameworks/actix_web
cargo build --release
PORT=19080 BENCH_WORKERS=4 ./target/release/katana_comparison_actix
```

### axum

```bash
cd comparisons/http_frameworks/axum
cargo build --release
PORT=19081 BENCH_WORKERS=4 ./target/release/katana_comparison_axum
```

### Drogon

```bash
cd comparisons/http_frameworks/drogon
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
PORT=19082 BENCH_WORKERS=4 ./build/katana_comparison_drogon
```

### ntex

```bash
cd comparisons/http_frameworks/ntex
cargo build --release
PORT=19084 BENCH_WORKERS=4 ./target/release/katana_comparison_ntex
```

### FastAPI

```bash
cd comparisons/http_frameworks/fastapi
python3 -m venv .venv
. .venv/bin/activate
pip install -r requirements.txt
PORT=19083 BENCH_WORKERS=4 python3 app.py
```

## Running the Shared Benchmarks

Use the example target file as a starting point:

```bash
python3 scripts/run_framework_benchmarks.py \
  --targets-file comparisons/http_frameworks/targets.example.json \
  --repeats 5 \
  --aggregation median \
  --output benchmark_results/framework_comparison
```

The runner reuses the same wrk Lua scripts and the same wrk-output parsing logic
as `scripts/run_benchmarks.py`.

For a 1:1 comparison with KATANA stages 9-12, use `BENCH_WORKERS=4`.

## Notes About KATANA in the Same Matrix

KATANA currently exposes the maintained benchmark endpoints via two binaries:

- `hello_world_server`
- `compute_api`

Because of that, the example target file uses `scenario_urls` for KATANA and a
single `base_url` for the comparison servers.
