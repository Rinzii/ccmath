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
# Check the whole repo for C-style casts using a compile database.
# This covers the .cpp files that only build with their real flags.
# Make the database with cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON and pass it as
# CCM_CAST_COMPILE_DB or the first argument.
# Add // NOLINT(google-readability-casting) if a cast is unavoidable.

set -euo pipefail

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
expected_version="$(tr -d '[:space:]' < "${repo_root}/tools/clang_tidy_version")"
CLANG_TIDY="${CLANG_TIDY:-clang-tidy}"
db="${CCM_CAST_COMPILE_DB:-build/compile_commands.json}"

cstyle_checks='-*,google-readability-casting,cppcoreguidelines-pro-type-cstyle-cast'
header_filter="${repo_root}/(include|tests|fuzzing|benchmark|benchmarks|tools)/"
# The public front-facing API under ext/ and math/ is exempt from clang-tidy.
exclude_public="${repo_root}/include/ccmath/(ext|math)/"

if [[ ! -f "${db}" ]]; then
    echo "No compile database at '${db}'." >&2
    echo "Configure a build with -DCMAKE_EXPORT_COMPILE_COMMANDS=ON and set CCM_CAST_COMPILE_DB." >&2
    exit 1
fi

if ! "${CLANG_TIDY}" --version | grep -Fq "${expected_version}"; then
    echo "${CLANG_TIDY} version mismatch: expected ${expected_version}" >&2
    "${CLANG_TIDY}" --version >&2
    exit 1
fi

db_dir="$(cd "$(dirname "${db}")" && pwd)"

violations=0

while IFS= read -r -d '' tu; do
    out="$("${CLANG_TIDY}" -p "${db_dir}" "${tu}" --quiet \
        --checks="${cstyle_checks}" --header-filter="${header_filter}" \
        --exclude-header-filter="${exclude_public}" 2>&1 || true)"
    if printf '%s' "${out}" | grep -q 'C-style casts are discouraged'; then
        echo "C-style cast (forbidden): ${tu}"
        printf '%s\n' "${out}" | grep -E 'warning: C-style casts' || true
        violations=$((violations + 1))
    fi
done < <(python3 - "${db}" "${repo_root}" <<'PY'
import json, os, sys
db_path, root = sys.argv[1], os.path.realpath(sys.argv[2])
with open(db_path) as f:
    entries = json.load(f)
seen = set()
for e in entries:
    p = os.path.realpath(os.path.join(e.get("directory", ""), e["file"]))
    if p.startswith(root + os.sep) and p not in seen:
        seen.add(p)
        sys.stdout.write(p + "\0")
PY
)

if [[ "${violations}" -gt 0 ]]; then
    echo
    echo "C-style casts found in ${violations} translation unit(s)."
    echo "Use static_cast/bit_cast, or // NOLINT(google-readability-casting) if absolutely required."
    exit 1
fi

echo "No C-style casts in the compiled translation units."
