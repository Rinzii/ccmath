#!/usr/bin/env bash
# Copyright (c) Ian Pike
# Copyright (c) CCMath contributors
#
# CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
# See LICENSE for more information.
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# Run any asmlab command inside the cross-arch Linux container. This is the path for
# gcc codegen, Linux-accurate x86, and the docker-only ARM arches (armv8.2-a). The
# repo is mounted read-write at /ccmath so outputs land in out/asmlab/ as usual.
#
# Usage:
#   docker/run.sh build                         # build the image only
#   docker/run.sh asmlab report pow_impl --arch armv8.2-a
#   docker/run.sh emit sqrt --arch x86-64-v3 --compiler gcc
#   docker/run.sh shell                         # interactive shell in the container
#
# The first token after run.sh selects the action: build | shell | <script>. When it
# is a known asmlab script name (asmlab, emit, analyze) it is invoked via python3;
# otherwise the remaining args run verbatim in the container.

set -euo pipefail

here="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
root="$(cd "${here}/../../.." && pwd)"
image="${CCMATH_ASMLAB_IMAGE:-ccmath-asmlab:local}"
platform="${CCMATH_ASMLAB_PLATFORM:-linux/amd64}"

cli=""
for c in docker podman; do
    if command -v "$c" >/dev/null 2>&1; then cli="$c"; break; fi
done
if [[ -z "${cli}" ]]; then
    echo "no docker/podman CLI found. Install Docker Desktop, Colima, or Podman." >&2
    exit 2
fi

build_image() {
    echo ">> building ${image} (${platform})" >&2
    "${cli}" build --platform "${platform}" -t "${image}" -f "${here}/Dockerfile" "${here}"
}

image_exists() {
    "${cli}" image inspect "${image}" >/dev/null 2>&1
}

run_in() {
    "${cli}" run --rm -it \
        --platform "${platform}" \
        -v "${root}:/ccmath:rw" \
        -w /ccmath \
        "${image}" \
        bash -lc "$*"
}

action="${1:-shell}"
shift || true

case "${action}" in
    build)
        build_image
        ;;
    shell)
        image_exists || build_image
        run_in "bash"
        ;;
    asmlab|emit|analyze|gate|verify|validate|view)
        image_exists || build_image
        if [[ "${action}" == "gate" ]]; then
            run_in "CCMATH_ASMLAB_DOCKER=1 bash tools/asmlab/scripts/accuracy_gate.sh $*"
        elif [[ "${action}" == "bench" ]]; then
            run_in "CCMATH_ASMLAB_DOCKER=1 bash tools/asmlab/scripts/bench.sh $*"
        else
            run_in "CCMATH_ASMLAB_DOCKER=1 python3 tools/asmlab/scripts/${action}.py $*"
        fi
        ;;
    bench)
        image_exists || build_image
        run_in "CCMATH_ASMLAB_DOCKER=1 bash tools/asmlab/scripts/bench.sh $*"
        ;;
    *)
        image_exists || build_image
        run_in "${action} $*"
        ;;
esac
