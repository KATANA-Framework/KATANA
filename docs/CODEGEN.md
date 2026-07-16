# KATANA code generator (`katana_gen`)

The CLI generates DTOs, validators, JSON (de)serialization, router bindings, and handler interfaces for the core architecture (router, arena allocators, zero-copy I/O).

## How to invoke

```bash
katana_gen openapi -i test_api.yaml -o ./generated --emit all --alloc pmr
```

Flags:
- `--emit dto|validator|serdes|router|handler|all` — what to generate (default `all`).
- `--serdes server|client|all` — which directions of parse/serialize/validate to emit
  (default `server`). A server parses+validates request types and serializes response
  types; the mirror halves are dead code and are pruned. `client` is the mirror image
  (for round-trip test harnesses); `all` keeps both directions for every schema.
  Schemas no operation references stay in both directions, so a schema-library spec
  (`paths: {}`) keeps full serdes.
- `--alloc pmr|std` — pick `pmr` for arenas and a zero-alloc hot path.
- `--layer flat|layered` — layer style (flat by default).
- `--dump-ast` — write out `openapi_ast.json`.
- `--strict` — fail on any spec error.

Artifacts:
- `generated_dtos.hpp` — DTOs/enums (arena-aware with `--alloc pmr`).
- `generated_validators.hpp` — required/enum checks.
- `generated_json.hpp` — JSON parsing/serialization.
- `generated_routes.hpp` — compile-time route metadata.
- `generated_handlers.hpp` — `api_handler`, optional `async_api_handler`, and `async_api_handler_base` for async-first services.
- `generated_router_bindings.hpp` — a static router wired to the handler.

### Quick start
```bash
katana_gen openapi -i benchmark/test_api.yaml -o benchmark/generated --emit all --alloc pmr
cmake --build build --target generated_api_benchmark
```

## Performance and architecture

- With `--alloc pmr` the code uses `katana::monotonic_arena`; keep one arena per request and reuse it.
- Bindings return a **stateless/static router** — created once, no per-request allocations.
- DTOs/parser are heap-free with `arena_string`/`arena_vector`; use `pmr` everywhere on the hot path.
- Path parameters are `string_view`/primitives — don't copy them.
- In handlers, build the response with precomputed headers and `serialize_into`, reusing the buffer.
- If an endpoint needs to finish after returning from the initial dispatch frame, implement `*_async(..., katana::http::async_response_writer out)` and derive from `async_api_handler_base` instead of writing deferred glue by hand in every route handler.

## Regenerating for benchmarks

The `openapi_benchmark` and `generated_api_benchmark` benchmarks expect files in `benchmark/generated`. To refresh them:

```bash
katana_gen openapi -i benchmark/test_api.yaml -o benchmark/generated --emit all --alloc pmr
cmake --build build --target generated_api_benchmark openapi_benchmark
```

## Notes for contributors

- Keep new templates allocator-independent: arena only under `--alloc pmr`.
- Don't concatenate `std::string` on the hot path — use stack `to_chars` and header prefixes.
- Don't create a router per request — keep it static (the way it's generated).
- Validation is simple required/enum checks; no `std::regex` and no heavy dependencies.
