# Codegen Examples

Current `katana_gen` examples that build through CMake and regenerate code from OpenAPI during the build.

## Active examples

- `compute_api/` — CPU load on parse/validate/serialize (`/compute/sum`).
- `validation_api/` — format and range validation (`/user/register`).
- `text_api/` — several endpoints with enum/transform logic.
- `benchmark_api/` — a broad API for codegen benchmarks (CRUD + compute + validation).
- `task_api/` — a complex spec for stress-testing the generator.

`products_api/` is kept only as archived reference code and is not considered a supported example:
- not wired into CMake;
- not part of the benchmark pipeline;
- has no current OpenAPI/codegen pipeline.

## Build

```bash
cmake --preset bench
cmake --build --preset bench --target compute_api validation_api text_api benchmark_api task_api
```

Binaries show up in `build/bench/examples/codegen/*/`.

## Generation via the tools

In each active example, `CMakeLists.txt` calls `katana_gen openapi ... --emit all --inline-naming operation` and writes the files into a local `generated/`.

Manual regeneration (from the project root):

```bash
./build/bench/katana_gen openapi -i examples/codegen/compute_api/api.yaml -o examples/codegen/compute_api/generated --emit all --inline-naming operation
```

Same for the other `api.yaml` files.
