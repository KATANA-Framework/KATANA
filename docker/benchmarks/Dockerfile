FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive
WORKDIR /workspace

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
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
CMD ["bash", "-lc", "cd /workspace && ./run_benchmarks.sh"]
