#!/bin/bash
# Apply clang-format to all headers under include/.

set -euo pipefail

CLANG_FORMAT="${CLANG_FORMAT:-clang-format}"
STYLE_ARGS=(-style=file -fallback-style=none)

if ! find include -name '*.hpp' -print -quit | grep -q .; then
    echo "No header files found under include/."
    exit 0
fi

find include -name '*.hpp' -print0 | xargs -0 "${CLANG_FORMAT}" -i "${STYLE_ARGS[@]}"
echo "Formatting applied."
