#!/usr/bin/env bash
# Install the pinned clang-tidy for Linux CI.
# It ships in the same LLVM release as clang-format, so this reuses that cache.

set -euo pipefail

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
version="$(tr -d '[:space:]' < "${repo_root}/tools/clang_tidy_version")"
cache_dir="${HOME}/.cache/ccmath/llvm-${version}"
archive="LLVM-${version}-Linux-X64.tar.xz"
archive_path="${cache_dir}/${archive}"
extract_dir="${cache_dir}/LLVM-${version}-Linux-X64"
url="https://github.com/llvm/llvm-project/releases/download/llvmorg-${version}/${archive}"

mkdir -p "${cache_dir}"

if [[ ! -x "${extract_dir}/bin/clang-tidy" ]]; then
    bash "${repo_root}/tools/download_with_retry.sh" "${url}" "${archive_path}"
    rm -rf "${extract_dir}"
    tar -xf "${archive_path}" -C "${cache_dir}"
fi

sudo ln -sf "${extract_dir}/bin/clang-tidy" /usr/local/bin/clang-tidy
clang-tidy --version

if ! clang-tidy --version | grep -Fq "${version}"; then
    echo "error: clang-tidy version mismatch, expected ${version}" >&2
    exit 1
fi
