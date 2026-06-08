#!/usr/bin/env bash
# Install the pinned clang-format release for Linux CI format checks.

set -euo pipefail

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
version="$(tr -d '[:space:]' < "${repo_root}/tools/clang_format_version")"
cache_dir="${HOME}/.cache/ccmath/llvm-${version}"
archive="LLVM-${version}-Linux-X64.tar.xz"
archive_path="${cache_dir}/${archive}"
extract_dir="${cache_dir}/LLVM-${version}-Linux-X64"
url="https://github.com/llvm/llvm-project/releases/download/llvmorg-${version}/${archive}"

mkdir -p "${cache_dir}"
bash "${repo_root}/tools/download_with_retry.sh" "${url}" "${archive_path}"

if [[ ! -x "${extract_dir}/bin/clang-format" ]]; then
    rm -rf "${extract_dir}"
    tar -xf "${archive_path}" -C "${cache_dir}"
fi

sudo ln -sf "${extract_dir}/bin/clang-format" /usr/local/bin/clang-format
clang-format --version

if ! clang-format --version | grep -Fq "${version}"; then
    echo "error: clang-format version mismatch, expected ${version}" >&2
    exit 1
fi
