FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    ninja-build \
    pkg-config \
    python3 \
    # perf runtime deps (for host-provided perf)
    libnuma1 \
    libunwind8 \
    libbabeltrace1 \
    libbabeltrace-ctf1 \
    libpfm4 \
    libtraceevent1 \
    libcap2 \
    libelf1 \
    libdw1 \
    liblzma5 \
    libzstd1 \
    libbz2-1.0 \
    libpcre2-8-0 \
    libssl3 \
    libslang2 \
    libperl5.38 \
    libpython3.12 \
    libpython3.12-minimal \
    # build helpers
    libdw-dev \
    binutils-dev \
    git \
    curl \
    && rm -rf /var/lib/apt/lists/*

# Install FlameGraph scripts
RUN git clone https://github.com/brendangregg/FlameGraph.git /opt/FlameGraph
ENV PATH="/opt/FlameGraph:${PATH}"

WORKDIR /workspace

COPY docker/profiling/entrypoint.sh /usr/local/bin/entrypoint.sh
RUN chmod +x /usr/local/bin/entrypoint.sh

ENTRYPOINT ["/usr/local/bin/entrypoint.sh"]
