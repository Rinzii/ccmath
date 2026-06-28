#!/bin/bash
# Apply clang-format to every tracked C++ source, excluding build glue, vendored code, and docs demos.

set -euo pipefail

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
CLANG_FORMAT="${CLANG_FORMAT:-clang-format}"
STYLE_ARGS=(-style=file -fallback-style=none)

cd "${repo_root}"

if ! git rev-parse --is-inside-work-tree > /dev/null 2>&1; then
    echo "Not inside a git work tree, cannot enumerate sources." >&2
    exit 1
fi

source_pathspec=('*.hpp' '*.cpp' '*.h' ':!:docs/**' ':!:thirdparty/**' ':!:cmake/**')

if [ -z "$(git ls-files -- "${source_pathspec[@]}" | head -n 1)" ]; then
    echo "No source files found."
    exit 0
fi

git ls-files -z -- "${source_pathspec[@]}" | xargs -0 "${CLANG_FORMAT}" -i "${STYLE_ARGS[@]}"
echo "Formatting applied."
