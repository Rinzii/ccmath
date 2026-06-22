#!/usr/bin/env bash
set -euo pipefail

usage() {
    echo "usage: $0 [cmake-build-dir]" >&2
    echo "  Copies CCMath headers into integration/vendored-consumer/third_party/ccmath." >&2
    echo "  version.hpp is taken from the CMake build tree." >&2
}

if [[ "${1:-}" == "-h" || "${1:-}" == "--help" ]]; then
    usage
    exit 0
fi

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
consumer_dir="${repo_root}/integration/vendored-consumer"
vendor_root="${consumer_dir}/third_party/ccmath"
cmake_build="${1:-${repo_root}/out/build/ninja/ninja-clang-debug}"
version_header="${cmake_build}/include/ccmath/version.hpp"

if [[ ! -f "${version_header}" ]]; then
    echo "error: missing ${version_header}" >&2
    echo "Configure CCMath with CMake first so version.hpp is generated." >&2
    exit 1
fi

if [[ ! -d "${repo_root}/include/ccmath" ]]; then
    echo "error: missing ${repo_root}/include/ccmath" >&2
    exit 1
fi

rm -rf "${vendor_root}"
mkdir -p "${vendor_root}/include/ccmath"

cp -R "${repo_root}/include/ccmath/." "${vendor_root}/include/ccmath/"
cp "${version_header}" "${vendor_root}/include/ccmath/version.hpp"

echo "Prepared vendored CCMath tree at ${vendor_root}"
