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
# Check that the internal headers use no C-style casts.
# The public front-facing API under ext/ and math/ is exempt from clang-tidy.
# clang-tidy finds them. Add // NOLINT(google-readability-casting) if one is unavoidable.
# Headers that do not parse on their own are skipped. The .cpp files are checked by
# ensure_cast_style_repo.sh.

set -euo pipefail

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
expected_version="$(tr -d '[:space:]' < "${repo_root}/tools/clang_tidy_version")"
CLANG_TIDY="${CLANG_TIDY:-clang-tidy}"

cstyle_checks='-*,google-readability-casting,cppcoreguidelines-pro-type-cstyle-cast'
compile_args=(-std=c++17 -Iinclude -xc++ -Wno-pragma-once-outside-header)

if ! "${CLANG_TIDY}" --version | grep -Fq "${expected_version}"; then
    echo "${CLANG_TIDY} version mismatch: expected ${expected_version}" >&2
    "${CLANG_TIDY}" --version >&2
    exit 1
fi

if ! find include/ccmath/internal -name '*.hpp' -print -quit | grep -q .; then
    echo "No header files found under include/ccmath/internal."
    exit 0
fi

violations=0
skipped=0

while IFS= read -r -d '' file; do
    tidy_out="$("${CLANG_TIDY}" "${file}" --quiet --checks="${cstyle_checks}" -- "${compile_args[@]}" 2>&1 || true)"

    # Skip headers that do not parse on their own.
    if printf '%s' "${tidy_out}" | grep -q 'clang-diagnostic-error'; then
        echo "Skipped (does not parse standalone): ${file}"
        skipped=$((skipped + 1))
        continue
    fi

    if printf '%s' "${tidy_out}" | grep -q 'C-style casts are discouraged'; then
        echo "C-style cast (forbidden): ${file}"
        printf '%s\n' "${tidy_out}" | grep -E 'warning: C-style casts' || true
        violations=$((violations + 1))
    fi
done < <(find include/ccmath/internal -name '*.hpp' -print0)

[[ "${skipped}" -gt 0 ]] && echo && echo "Skipped ${skipped} header(s) that do not parse standalone (see above)."

if [[ "${violations}" -gt 0 ]]; then
    echo
    echo "C-style casts found in ${violations} file(s)."
    echo "Use static_cast/bit_cast, or // NOLINT(google-readability-casting) if absolutely required."
    exit 1
fi

echo "Cast style is up to date."
