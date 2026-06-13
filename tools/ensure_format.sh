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

if ! find include -name '*.hpp' -print -quit | grep -q .; then
    echo "No header files found under include/."
    exit 0
fi

while IFS= read -r -d '' file; do
    if ! "${CLANG_FORMAT}" --dry-run --Werror "${STYLE_ARGS[@]}" "${file}"; then
        echo "Formatting check failed: ${file}"
        echo "Run tools/apply_format.sh locally or fix headers manually."
        exit 1
    fi
done < <(find include -name '*.hpp' -print0)

echo "Formatting is up to date."
