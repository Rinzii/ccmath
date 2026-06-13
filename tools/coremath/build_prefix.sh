#!/usr/bin/env bash
# Build a minimal CORE-MATH pow/powf oracle prefix for rigorous validation.
#
# Usage: tools/coremath/build_prefix.sh <install-prefix>
#
# The prefix layout matches cmake/config/FindCoreMath.cmake discovery:
#   <prefix>/include/crmath.h
#   <prefix>/lib/libcoremath_oracle.a

set -euo pipefail

prefix="${1:?usage: build_prefix.sh <install-prefix>}"
repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
work_dir="${CCMATH_COREMATH_BUILD_DIR:-${TMPDIR:-/tmp}/ccmath-coremath-build-$$}"
source_dir="${work_dir}/core-math-src"
build_dir="${work_dir}/build"

cleanup() {
    if [[ "${CCMATH_COREMATH_KEEP_BUILD:-}" != "1" ]]; then
        rm -rf "${work_dir}"
    fi
}
trap cleanup EXIT

mkdir -p "${work_dir}"
if [[ ! -f "${source_dir}/src/binary64/pow/pow.c" ]]; then
    git clone --depth 1 https://github.com/mockingbirdnest/core-math.git "${source_dir}"
fi

cmake -S "${repo_root}/cmake/coremath" -B "${build_dir}" \
    -DCORE_MATH_SOURCE_DIR="${source_dir}" \
    -DCMAKE_INSTALL_PREFIX="${prefix}" \
    -DCMAKE_BUILD_TYPE=Release

cmake --build "${build_dir}" --parallel
cmake --install "${build_dir}"

echo "CORE-MATH oracle prefix installed to ${prefix}"
