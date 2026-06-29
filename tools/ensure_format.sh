#!/bin/bash
#
# Copyright (c) Ian Pike
# Copyright (c) CCMath contributors
#
# CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
# See LICENSE for more information.
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

set -euo pipefail

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
expected_version="$(tr -d '[:space:]' < "${repo_root}/tools/clang_format_version")"
CLANG_FORMAT="${CLANG_FORMAT:-clang-format}"
STYLE_ARGS=(-style=file -fallback-style=none)

if ! "${CLANG_FORMAT}" --version | grep -Fq "${expected_version}"; then
    echo "clang-format version mismatch: expected ${expected_version}" >&2
    "${CLANG_FORMAT}" --version >&2
    exit 1
fi

cd "${repo_root}"

if ! git rev-parse --is-inside-work-tree > /dev/null 2>&1; then
    echo "Not inside a git work tree, cannot enumerate sources." >&2
    exit 1
fi

# Every tracked C++ source, excluding build glue, vendored code, and docs demos.
source_pathspec=('*.hpp' '*.cpp' '*.h' ':!:docs/**' ':!:thirdparty/**' ':!:cmake/**')

checked=0
while IFS= read -r -d '' file; do
    checked=1
    if ! "${CLANG_FORMAT}" --dry-run --Werror "${STYLE_ARGS[@]}" "${file}"; then
        echo "Formatting check failed: ${file}"
        echo "Run tools/apply_format.sh locally or fix the file manually."
        exit 1
    fi
done < <(git ls-files -z -- "${source_pathspec[@]}")

if [ "${checked}" -eq 0 ]; then
    echo "No source files found."
    exit 0
fi

echo "Formatting is up to date."
