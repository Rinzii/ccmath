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
# Run clang-tidy over a compile database using the project .clang-tidy config.
# Most checks are advisory, but cast style is enforced: any C-style cast fails the run.
# Vendored and generated code (the build tree, _deps, thirdparty, integration) is not linted.
# Add // NOLINT(google-readability-casting) if a cast is genuinely required.

set -euo pipefail

db="${1:-${CCM_COMPILE_DB:-build/compile_commands.json}}"
repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
CLANG_TIDY="${CLANG_TIDY:-clang-tidy}"
header_filter="${repo_root}/(include|tests|fuzzing|benchmark|benchmarks|tools)/"
# The public front-facing API under ext/ and math/ is exempt from clang-tidy.
exclude_public="${repo_root}/include/ccmath/(ext|math)/"

if [[ ! -f "${db}" ]]; then
    echo "No compile database at '${db}'."
    exit 0
fi

db_dir="$(cd "$(dirname "${db}")" && pwd)"

cast_violations=0

while IFS= read -r -d '' tu; do
    out="$("${CLANG_TIDY}" -p "${db_dir}" "${tu}" --quiet --header-filter="${header_filter}" \
        --exclude-header-filter="${exclude_public}" 2>&1 || true)"
    if [[ -n "${out}" ]]; then
        printf '%s\n' "${out}"
    fi
    if printf '%s' "${out}" | grep -q 'C-style casts are discouraged'; then
        cast_violations=$((cast_violations + 1))
    fi
done < <(python3 - "${db}" "${repo_root}" <<'PY'
import json, os, sys
db_path, root = sys.argv[1], os.path.realpath(sys.argv[2])
# The compile database lives in the build tree, which also holds _deps and generated code.
db_dir = os.path.dirname(os.path.realpath(db_path))


def is_vendored_or_generated(p):
    if p.startswith(db_dir + os.sep):
        return True
    for seg in ("_deps", "thirdparty", "integration"):
        if (os.sep + seg + os.sep) in p:
            return True
    return False


seen = set()
for e in json.load(open(db_path)):
    p = os.path.realpath(os.path.join(e.get("directory", ""), e["file"]))
    if not p.startswith(root + os.sep) or is_vendored_or_generated(p) or p in seen:
        continue
    seen.add(p)
    sys.stdout.write(p + "\0")
PY
)

if [[ "${cast_violations}" -gt 0 ]]; then
    echo
    echo "C-style casts found in ${cast_violations} translation unit(s)."
    echo "Use static_cast/bit_cast, or // NOLINT(google-readability-casting) if absolutely required."
    exit 1
fi

echo "clang-tidy run complete. No C-style casts."
