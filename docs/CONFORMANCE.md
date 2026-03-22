# Stage 3 Conformance Harness

## Overview

The conformance harness verifies that generated router/bindings produced by `katana_gen`
match the OpenAPI contract on real HTTP input. It exercises routing, `Content-Type` and
`Accept` negotiation, JSON parsing, schema validation, handler dispatch, and RFC 7807
error responses without starting the server runtime.

Current implementation lives in:

- `test/conformance/fixtures/petstore_minimal.json`
- `test/conformance/generated/*.hpp`
- `test/conformance/test_conformance.cpp`
- `test/CMakeLists.txt` target `conformance_tests`

## Architecture

```text
petstore_minimal.json --> katana_gen --> generated/*.hpp
                                          |
                                          v
                               test_conformance.cpp
                                          |
                                          v
                                 generated_router
                                          |
                                          v
                            HttpHandlerHarness.run_raw()
```

## Design Choices

1. No reactor startup. The tests dispatch raw HTTP requests through the generated router.
2. No external dependencies. The harness uses only existing test support and committed generated code.
3. Stub handler only. The handler returns minimal responses and records calls so the tests verify framework behaviour, not domain logic.
4. Portable target. `conformance_tests` runs the same way on Linux and WSL.
5. Pre-generated code is committed. The fixture spec is the source of truth, but CI/test runs do not regenerate headers on the fly.

## Current Coverage

The committed harness currently covers 15 end-to-end scenarios on the `petstore_minimal` fixture:

1. `GET /pets` binds query, header, and cookie parameters.
2. `GET /pets` defaults to JSON when `Accept` is absent.
3. Missing required header returns `400`.
4. Invalid query parameter returns `400`.
5. `Accept: application/cbor` returns `501` before handler dispatch.
6. `POST /pets` with valid JSON returns `201`.
7. Malformed JSON body returns `400`.
8. Validation failure returns `400`.
9. Unsupported `Content-Type` returns `415`.
10. `Content-Type: application/cbor` returns `501` before handler dispatch.
11. `GET /pets/{petId}` binds path parameter correctly.
12. Invalid path parameter returns `400`.
13. `DELETE /pets/{petId}` returns `204`.
14. Unknown route returns `404`.
15. Method mismatch returns `405` and `Allow`.

The fixture intentionally covers:

- path, query, header, and cookie parameters;
- JSON success path;
- non-JSON request/response codec stub paths;
- validation and parser failures;
- generated route-table semantics for `404` and `405`.

## Directory Layout

```text
test/conformance/
├── fixtures/
│   └── petstore_minimal.json
├── generated/
│   ├── generated_dtos.hpp
│   ├── generated_handlers.hpp
│   ├── generated_json.hpp
│   ├── generated_router_bindings.hpp
│   ├── generated_routes.hpp
│   └── generated_validators.hpp
└── test_conformance.cpp
```

## Running

```bash
cmake --build build/debug-local --target conformance_tests -j
ctest --test-dir build/debug-local -R conformance_tests --output-on-failure
```

For a direct binary run:

```bash
./build/debug-local/test/conformance_tests
```

## Notes

- `conformance_tests` has IPO/LTO disabled in CMake. This avoids a GCC LTO internal compiler error in `bench-local` caused by the large generated include graph. It does not affect runtime binaries.
- The harness currently uses one committed fixture spec plus explicit C++ test cases. A future data-driven runner can be added later if needed.
- CBOR and MessagePack remain Stage 3 stubs. The conformance suite verifies that those paths fail predictably with `501 Not Implemented`.

## Definition of Done

- [x] Generated router/bindings are exercised on real HTTP requests.
- [x] No server startup or external services are required.
- [x] Negotiation, validation, and error semantics are covered end-to-end.
- [x] Generated route-table behaviour for `404` and `405` is covered.
- [x] Fixture spec and generated headers are committed together.
- [x] Linux/WSL-friendly single target exists: `conformance_tests`.
