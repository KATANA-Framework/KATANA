\# Stage 3 Conformance Harness



\## Overview



The conformance harness verifies that \*\*generated router/bindings\*\* produced by

`katana\_gen` conform to the OpenAPI contract. It tests the full HTTP pipeline:

routing, Content-Type/Accept negotiation, JSON parsing, schema validation,

handler dispatch, and error semantics — all without external dependencies or a

running server.



\## Architecture



```

conformance\_api.yaml ──► katana\_gen ──► generated/\*.hpp

&#x20;                                             │

&#x20;                                             ▼

fixtures/\*.json ──► test\_conformance.cpp ──► generated\_router

&#x20;                        │                       │

&#x20;                        │               router\_handler adapter

&#x20;                        │                       │

&#x20;                        ▼                       ▼

&#x20;                   assertions ◄── HttpHandlerHarness.run\_raw()

```



\*\*Key design decisions:\*\*



1\. \*\*No reactor startup\*\* — uses `HttpHandlerHarness` + `router\_handler` to

&#x20;  dispatch raw HTTP request strings through the generated router. Tests run in

&#x20;  milliseconds.

2\. \*\*No external dependencies\*\* — only the existing gtest-compatible harness,

&#x20;  `katana\_core`, and generated code.

3\. \*\*Stub handler\*\* — returns minimal valid responses. Tests verify \*framework\*

&#x20;  behaviour (negotiation, validation, error format), not business logic.

4\. \*\*Portable\*\* — same CMake target works on Linux and WSL. No fork/bind/listen.



\## Gap Analysis (Before This Harness)



| Area | Before | After |

|------|--------|-------|

| Valid request end-to-end | Codegen unit tests verify generated code text | Tests run generated code against real HTTP input |

| Content-Type / Accept negotiation | Only tested in router unit tests | Tested through generated bindings |

| Validation constraint errors | Validator unit tests with synthetic structs | Tested via JSON → parse → validate → error |

| Problem details format | `test\_problem.cpp` tests `to\_json()` | Verified end-to-end in HTTP response bodies |

| 404 / 405 dispatch | `test\_router.cpp` with manual route tables | Verified through generated route tables |



\## Fixture Format



Each scenario is a JSON file in `test/conformance/fixtures/`:



```json

{

&#x20; "name": "valid\_create\_item",

&#x20; "description": "POST /items with valid JSON body returns 200",

&#x20; "request": {

&#x20;   "method": "POST",

&#x20;   "uri": "/items",

&#x20;   "headers": {

&#x20;     "Content-Type": "application/json",

&#x20;     "Accept": "application/json"

&#x20;   },

&#x20;   "body": "{\\"name\\":\\"widget\\",\\"quantity\\":10}"

&#x20; },

&#x20; "expected": {

&#x20;   "status": 200,

&#x20;   "content\_type": "application/json",

&#x20;   "body\_contains": "widget"

&#x20; }

}

```



Fixtures are documentation — each one maps to a named C++ TEST\_F case.



\## Directory Layout



```

test/conformance/

├── conformance\_api.yaml          # OpenAPI fixture spec

├── test\_conformance.cpp          # C++ test runner (21 tests)

├── generated/                    # Pre-generated code (committed)

│   ├── generated\_dtos.hpp

│   ├── generated\_handlers.hpp

│   ├── generated\_json.hpp

│   ├── generated\_router\_bindings.hpp

│   ├── generated\_routes.hpp

│   └── generated\_validators.hpp

└── fixtures/                     # Scenario descriptions (JSON)

&#x20;   ├── valid\_create\_item.json

&#x20;   ├── valid\_create\_item\_with\_optional.json

&#x20;   ├── valid\_list\_items.json

&#x20;   ├── valid\_get\_item.json

&#x20;   ├── valid\_echo.json

&#x20;   ├── no\_accept\_defaults\_ok.json

&#x20;   ├── missing\_required\_field.json

&#x20;   ├── echo\_empty\_message.json

&#x20;   ├── invalid\_body\_json.json

&#x20;   ├── constraint\_violation\_quantity.json

&#x20;   ├── invalid\_email\_format.json

&#x20;   ├── invalid\_path\_param.json

&#x20;   ├── invalid\_query\_param.json

&#x20;   ├── unsupported\_content\_type.json

&#x20;   ├── missing\_content\_type.json

&#x20;   ├── unacceptable\_accept.json

&#x20;   ├── route\_not\_found.json

&#x20;   └── method\_not\_allowed.json

```



\## Test Matrix (First Wave)



| # | Scenario | Category | Expected |

|---|----------|----------|----------|

| 1 | Valid POST /items | Valid request | 200 + JSON body |

| 2 | Valid POST /items with optional fields | Valid request | 200 |

| 3 | Valid GET /items | Valid request (no body) | 200 |

| 4 | Valid GET /items/42 | Valid request (path param) | 200 |

| 5 | Valid POST /echo | Valid request | 200 + echo body |

| 6 | POST /items without Accept | Valid (Accept defaults) | 200 |

| 7 | POST /items missing required field | Missing required param | 400 parse error |

| 8 | POST /echo empty message | Constraint violation (minLength) | 400 + field name |

| 9 | POST /items malformed JSON | Invalid body | 400 + "invalid request body" |

| 10 | POST /items quantity > max | Constraint violation | 400 + field name |

| 11 | POST /items invalid email | Format violation | 400 + field name |

| 12 | GET /items/abc (non-integer) | Invalid path param | 400 + "id" |

| 13 | GET /items?limit=abc | Invalid query param | 400 + "limit" |

| 14 | POST /items Content-Type: xml | Unsupported Content-Type | 415 |

| 15 | POST /items no Content-Type | Missing Content-Type | 415 |

| 16 | POST /items Accept: text/xml | Unacceptable Accept | 406 |

| 17 | Response Content-Type check | Response media type | application/json |

| 18 | 415 error body format | Problem details | has "status" + "title" |

| 19 | Validation error format | Problem details | has field + "status" |

| 20 | GET /nonexistent | Route not found | 404 |

| 21 | DELETE /items | Method not allowed | 405 |



\## Running



```bash

\# From build directory:

cmake --preset debug   # or: cmake .. -DCMAKE\_BUILD\_TYPE=Debug -DENABLE\_TESTING=ON

make conformance\_tests -j$(nproc)

./test/conformance\_tests



\# Or via ctest:

ctest -R conformance

```



\## Rollout Order



1\. \*\*Wave 1 (this PR):\*\* 21 canonical scenarios covering all categories from the

&#x20;  issue requirements. Generated code is pre-committed.

2\. \*\*Wave 2:\*\* Add fixture-driven test runner that reads `fixtures/\*.json` at

&#x20;  runtime and generates test cases dynamically (data-driven).

3\. \*\*Wave 3:\*\* Add coverage for multi-media-type negotiation (JSON + CBOR) when

&#x20;  media type registry lands.

4\. \*\*Wave 4:\*\* CI integration with automatic re-generation from spec changes.



\## Definition of Done



\- \[x] Fixture format defined and documented

\- \[x] Test architecture uses existing test support (HttpHandlerHarness, router\_handler)

\- \[x] No external dependencies beyond what is in the repo

\- \[x] All 7 canonical scenario categories covered (valid, missing required, invalid,

&#x20;     Content-Type, Accept, response media type, problem details/error semantics)

\- \[x] Tests run identically on Linux and WSL (no fork/bind/listen)

\- \[x] Directory structure and naming convention documented

\- \[x] Conformance tests linked to generated artifacts via `generated/` directory

\- \[x] All 21 tests pass
