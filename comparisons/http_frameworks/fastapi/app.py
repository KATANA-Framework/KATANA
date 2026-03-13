import os

from fastapi import FastAPI, HTTPException
from fastapi.responses import PlainTextResponse
import uvicorn


DEFAULT_PORT = 8080
DEFAULT_WORKERS = 4
MAX_ITEMS = 1024


def read_port() -> int:
    for key in ("PORT", "HELLO_PORT", "COMPUTE_PORT"):
        value = os.getenv(key)
        if value:
            try:
                port = int(value)
            except ValueError:
                continue
            if 0 < port < 65536:
                return port
    return DEFAULT_PORT


def read_workers() -> int:
    for key in ("BENCH_WORKERS", "KATANA_WORKERS", "WORKERS"):
        value = os.getenv(key)
        if value:
            try:
                workers = int(value)
            except ValueError:
                continue
            if workers > 0:
                return workers
    return DEFAULT_WORKERS


app = FastAPI(docs_url=None, redoc_url=None, openapi_url=None)


@app.get("/", response_class=PlainTextResponse)
async def hello() -> str:
    return "Hello, World!"


@app.post("/compute/sum")
async def compute_sum(nums: list[float]) -> float:
    if not 1 <= len(nums) <= MAX_ITEMS:
        raise HTTPException(status_code=400, detail="payload must contain 1..=1024 numbers")
    return float(sum(nums))


if __name__ == "__main__":
    port = read_port()
    workers = read_workers()
    print(f"fastapi comparison server listening on :{port} with {workers} workers")
    uvicorn.run(
        "app:app",
        host="0.0.0.0",
        port=port,
        workers=workers,
        access_log=False,
        log_level="warning",
    )
