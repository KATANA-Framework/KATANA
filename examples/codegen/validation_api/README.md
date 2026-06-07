# Validation API (codegen)

A validator microservice: POST `/user/register` checks email/password/age and immediately responds `"ok"` without any business logic.

## What it demonstrates
- Compile-time DTO + validators (required, nullable, regex/format/email, minLength/maxLength, integer ranges)
- Zero-copy JSON → arena DTO
- Problem Details (422/400) on schema violations
- Optional semantics: `age` can be absent or `null`

## Build and run
```bash
cmake --preset examples
cmake --build --preset examples --target validation_api
./build/examples/examples/codegen/validation_api/validation_api  # PORT=8081 by default
```

## Example request
```bash
curl -X POST http://localhost:8081/user/register \
  -H 'Content-Type: application/json' \
  -d '{"email":"a@b.com","password":"supersecret","age":21}'
# => "ok"
```

An invalid email or a short password automatically yields 400/422 with a detailed `detail.path`.

## Load testing
Scenario: a mix of valid/invalid requests under 4–8 threads (5 s warmup → 5 s load).
- Metrics: success_rate, p50/p95/p99 for valid requests and for the reject path
- Used in `generate_benchmark_report.py` and the docker benchmark
