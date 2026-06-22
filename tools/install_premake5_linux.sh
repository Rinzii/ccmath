#!/usr/bin/env bash
# Install the official premake5 Linux binary (v5.0.0-beta8).
#
# Usage: tools/install_premake5_linux.sh [install-dir]
#
# install-dir defaults to <repo>/out/tools/premake and receives the premake5
# executable directly. Pass /usr/local/bin when installing system-wide.
#
# Environment:
#   PREMAKE_CACHE_DIR  optional directory for the downloaded tarball reuse

set -euo pipefail

if [[ "$(uname -s)" != Linux ]]; then
    echo "error: tools/install_premake5_linux.sh supports Linux only" >&2
    exit 1
fi

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
install_dir="${1:-${repo_root}/out/tools/premake}"
premake_url="https://github.com/premake/premake-core/releases/download/v5.0.0-beta8/premake-5.0.0-beta8-linux.tar.gz"
tarball_name="premake-5.0.0-beta8-linux.tar.gz"
staging="$(mktemp -d)"
trap 'rm -rf "${staging}"' EXIT

mkdir -p "${install_dir}"

if [[ -x "${install_dir}/premake5" ]]; then
    "${install_dir}/premake5" --version
    exit 0
fi

cache_dir="${PREMAKE_CACHE_DIR:-${staging}}"
mkdir -p "${cache_dir}"
tarball="${cache_dir}/${tarball_name}"

bash "${repo_root}/tools/download_with_retry.sh" "${premake_url}" "${tarball}"
tar -xzf "${tarball}" -C "${install_dir}" premake5
chmod +x "${install_dir}/premake5"

echo "Installed premake5 to ${install_dir}/premake5"
"${install_dir}/premake5" --version
