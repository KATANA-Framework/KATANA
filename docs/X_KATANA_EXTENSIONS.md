\# x-katana-\\\* Extensions — Stage 3 Contract Revision



> Ревизия extension contract: что реально поддерживается, что только парсится,

> что остаётся декларативным, а что пока нельзя обещать в docs.



\---



\## 1. Inventory



Все `x-katana-\*` расширения, найденные в кодовой базе:



| Extension | Файл / Контекст |

|---|---|

| `x-katana-cache` | AST (`openapi\_ast.hpp:149`), loader (`openapi\_loader.cpp:1239`), codegen (`router\_generator.cpp:1209`), README, tutorial YAML |

| `x-katana-alloc` | AST (`openapi\_ast.hpp:150`), loader (`openapi\_loader.cpp:1249`), codegen (`router\_generator.cpp:1212`), README |

| `x-katana-rate-limit` | AST (`openapi\_ast.hpp:151`), loader (`openapi\_loader.cpp:1259`), codegen (`router\_generator.cpp:1215`), README |

| `x-katana-idempotency` | README Stage 5 roadmap only (`README.md:683`); \*\*нет в коде\*\* |



Не найдены в коде и не документированы как отдельные расширения:

`x-katana-auth`, `x-katana-log`, `x-katana-retry`, `x-katana-timeout`,

`x-katana-circuit-breaker`, `x-katana-validate`.



\---



\## 2. Truth Table



| Extension | Parser (loader) | AST field | Codegen output | Runtime effect | Test coverage | Docs status | Stage 3 label |

|---|---|---|---|---|---|---|---|

| `x-katana-cache` | \*\*yes\*\* — string or bool | `operation::x\_katana\_cache` | comment `// @cache:` | \*\*none\*\* | \*\*none\*\* | mentioned in README, example YAML | \*\*declarative\*\* |

| `x-katana-alloc` | \*\*yes\*\* — string or number | `operation::x\_katana\_alloc` | comment `// @alloc:` | \*\*none\*\* | \*\*none\*\* | mentioned in README feature list | \*\*declarative\*\* |

| `x-katana-rate-limit` | \*\*yes\*\* — string only | `operation::x\_katana\_rate\_limit` | comment `// @rate-limit:` | \*\*none\*\* | \*\*none\*\* | mentioned in README feature list | \*\*declarative\*\* |

| `x-katana-idempotency` | \*\*no\*\* | \*\*no\*\* | \*\*no\*\* | \*\*none\*\* | \*\*none\*\* | mentioned in README Stage 5 | \*\*not supported\*\* |



\### Legend



\- \*\*supported\*\* — parsed, stored in AST, affects codegen \*\*and\*\* runtime behaviour.

\- \*\*partial\*\* — parsed and stored, affects codegen but not runtime.

\- \*\*declarative\*\* — parsed and stored in AST, emitted only as comments in generated code; no runtime effect.

\- \*\*not supported\*\* — not implemented in any layer; exists only as a roadmap mention.



\---



\## 3. Concrete Mismatches Between Code and Docs



\### 3.1. README YAML example uses object format for `x-katana-cache`



\*\*README.md:785–786\*\* shows:



```yaml

x-katana-cache:

&#x20; ttl: 10s

```



But the loader (`openapi\_loader.cpp:1239–1248`) only accepts \*\*string\*\* (`"10s"`) or \*\*boolean\*\* (`true`/`false`).

An object value (`{ttl: 10s}`) is \*\*silently skipped\*\* (`cur.skip\_value()`).



\*\*Impact\*\*: users who follow the tutorial will get no cache annotation in generated code.



\### 3.2. README claims `x-katana-\*` are "done" (✅)



\*\*README.md:41\*\* marks `x-katana-\* extensions` with ✅, and \*\*README.md:238\*\* repeats

`✅ x-katana-\* extensions (cache, alloc, rate-limit)`.



This implies the extensions have meaningful effect. In reality:

\- they are \*\*parsed and stored\*\* (loader + AST) ✅

\- they are \*\*emitted as code comments\*\* (codegen) ✅

\- they have \*\*no runtime enforcement\*\* ❌

\- they have \*\*no test coverage\*\* ❌



The ✅ is misleading without qualification.



\### 3.3. README section 340–348 promises more than what exists



\*\*README.md:340–348\*\* describes extensions including:



\- `выбор сериализатора (zero-copy / dom)` — no corresponding `x-katana-\*` extension in code

\- `кэш (TTL, stale-while-revalidate, ключи инвалидации)` — only simple string/bool parsed; no stale-while-revalidate or invalidation keys

\- `rate limiting и idempotency` — idempotency not implemented at all

\- `требования консистентности и дедлайны` — no corresponding extension in code



\### 3.4. OPENAPI.md marks extension spec as TODO



\*\*docs/OPENAPI.md:760\*\* correctly lists the specification as a pending task:

`- \[ ] чёткая спецификация поддерживаемых x-katana-\* extensions`



This is consistent with the actual state, but conflicts with README's ✅ claims.



\---



\## 4. Recommended Contract Text



\### For README.md — feature list (replace current ✅ line)



> `x-katana-\* extensions` — declarative annotations parsed from OpenAPI spec

> and emitted as comments in generated code. \*\*No runtime enforcement in

> current version.\*\* Supported annotations: `x-katana-cache` (string),

> `x-katana-alloc` (string | number), `x-katana-rate-limit` (string).

> Runtime enforcement planned for Stage 5.



\### For README.md — extensions section (replace lines 340–348)



> ### Расширения OpenAPI (`x-katana-\*`)

>

> Не меняют стандарт, добавляют \*\*декларативные\*\* аннотации на уровне

> operation. На текущем этапе (Stage 2–3) аннотации:

>

> - \*\*парсятся\*\* loader'ом из OpenAPI JSON/YAML,

> - \*\*сохраняются\*\* в AST (`operation` struct),

> - \*\*выводятся\*\* в generated код как комментарии (`// @cache:`, `// @alloc:`, `// @rate-limit:`).

>

> \*\*Runtime enforcement\*\* (фактическое применение кэша, rate limit, аллокации)

> запланировано на Stage 5.

>

> | Extension | Accepted values | Example |

> |---|---|---|

> | `x-katana-cache` | string (`"300s"`, `"5m"`) or `true`/`false` | `x-katana-cache: "5m"` |

> | `x-katana-alloc` | string (`"pool"`, `"arena"`) or number (`4096`) | `x-katana-alloc: "pool"` |

> | `x-katana-rate-limit` | string (`"100/s"`, `"1000/m"`) | `x-katana-rate-limit: "100/s"` |

>

> Расширения, упомянутые в roadmap, но ещё не реализованные:

> `x-katana-idempotency` (Stage 5).



\### For docs/OPENAPI.md — new section (before roadmap)



> ## x-katana-\\\* Extensions

>

> Katana поддерживает vendor extensions с префиксом `x-katana-` на уровне

> OpenAPI operation. Расширения парсятся loader'ом и передаются в AST, но на

> текущем этапе (Stage 3) \*\*не влияют на runtime behaviour\*\* — они

> декларативны.

>

> ### Поддерживаемые расширения

>

> | Extension | Value type | Loader support | Codegen output | Runtime |

> |---|---|---|---|---|

> | `x-katana-cache` | `string` / `boolean` | ✅ | comment | Stage 5 |

> | `x-katana-alloc` | `string` / `number` | ✅ | comment | Stage 5 |

> | `x-katana-rate-limit` | `string` | ✅ | comment | Stage 5 |

>

> \*\*Object values\*\* (e.g. `x-katana-cache: { ttl: 10s }`) are \*\*not supported\*\*

> and will be silently ignored.



\---



\## 5. Extension Schema Normalization Recommendations



\### 5.1. Adopt string-only values (current state is acceptable)



Current parser accepts string for all three extensions, plus boolean for cache

and number for alloc. This is a reasonable minimal contract. \*\*No schema change

needed at Stage 3.\*\*



\### 5.2. When Stage 5 adds runtime enforcement, consider:



\- \*\*Structured object values\*\* (e.g. `{ttl: "5m", stale-while-revalidate: "30s"}`)

&#x20; will require loader changes. Either:

&#x20; - (a) extend the parser to handle objects, or

&#x20; - (b) use a compact string DSL (e.g. `"5m;swr=30s"`)

\- \*\*Validation\*\*: add loader warnings for unrecognized values.

\- \*\*Enum for `x-katana-alloc`\*\*: limit to known values (`arena`, `pool`, `heap`, or numeric buffer size).



\### 5.3. Register extensions in a central enum/table



To prevent typos and make future extensions discoverable, consider adding

a compile-time registry of known extension names (even if just a comment

or constexpr array in `openapi\_ast.hpp`).



\---



\## 6. Follow-Up Tasks (Stage 3 scope only)



\- \[ ] Fix README YAML example: change `x-katana-cache: { ttl: 10s }` → `x-katana-cache: "10s"` to match actual parser

\- \[ ] Qualify README feature list: clarify that extensions are declarative (parsed + comment-only)

\- \[ ] Update README extensions section (lines 340–348) with accurate contract table

\- \[ ] Add `x-katana-\*` section to `docs/OPENAPI.md` with value format specification

\- \[ ] Mark `docs/OPENAPI.md:760` checkbox as done after above changes

\- \[ ] Add basic loader tests for x-katana-\* extension parsing (string, bool, number, object-skip)

\- \[ ] Add codegen tests verifying comment emission for extensions

\- \[ ] (\*\*Out of scope reminder\*\*) Runtime enforcement is Stage 5 — do not implement here
