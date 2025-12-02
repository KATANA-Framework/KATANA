FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive
WORKDIR /workspace

RUN apt-get update && \
    apt-get install -y --no-recommends \
        build-essential \
        cmake \
        ninja-build \
        git \
        curl \
        pkg-config \
        python3 \
        python3-pip \
        ca-certificates \
        libssl-dev \
        zlib1g-dev && \
    rm -rf /var/lib/apt/lists/*

# Expect repository to be mounted to /workspace.
# Bench harness builds with ENABLE_BENCHMARKS=ON and includes router_benchmark.
CMD ["bash", "-lc", "cd /workspace && ./run_benchmarks.sh"]
