# asmlab cross-arch analysis image (linux/amd64).
#
# Provides Linux-accurate codegen with both clang and gcc, the LLVM analysis tools,
# an aarch64 cross toolchain (so the docker-only ARM arches in the matrix emit), and
# the optional throughput models. Build with docker/run.sh, which mounts the repo at /ccmath.
FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update -qq \
    && apt-get install -yqq --no-install-recommends \
        ca-certificates \
        clang \
        cmake \
        curl \
        g++ \
        g++-aarch64-linux-gnu \
        git \
        llvm \
        ninja-build \
        pkg-config \
        python3 \
        python3-pip \
    && rm -rf /var/lib/apt/lists/*

# OSACA is optional but cheap to add here so the container is self-contained.
RUN pip3 install --no-cache-dir --break-system-packages osaca || true

WORKDIR /ccmath
