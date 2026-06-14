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
# Advisory only. Prints warnings and always exits 0.

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

while IFS= read -r -d '' tu; do
    "${CLANG_TIDY}" -p "${db_dir}" "${tu}" --quiet --header-filter="${header_filter}" \
        --exclude-header-filter="${exclude_public}" 2>&1 || true
done < <(python3 - "${db}" "${repo_root}" <<'PY'
import json, os, sys
db_path, root = sys.argv[1], os.path.realpath(sys.argv[2])
seen = set()
for e in json.load(open(db_path)):
    p = os.path.realpath(os.path.join(e.get("directory", ""), e["file"]))
    if p.startswith(root + os.sep) and p not in seen:
        seen.add(p)
        sys.stdout.write(p + "\0")
PY
)

echo "clang-tidy advisory run complete."
