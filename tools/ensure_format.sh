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

CLANG_FORMAT="${CLANG_FORMAT:-clang-format}"
STYLE_ARGS=(-style=file -fallback-style=none)

if ! find include -name '*.hpp' -print -quit | grep -q .; then
    echo "No header files found under include/."
    exit 0
fi

set +e
find include -name '*.hpp' | xargs "${CLANG_FORMAT}" --dry-run --Werror "${STYLE_ARGS[@]}"
check_status=$?
set -e

if [ "${check_status}" -eq 0 ]; then
    echo "Formatting is up to date."
    exit 0
fi

echo "Formatting required. Applying clang-format..."
find include -name '*.hpp' | xargs "${CLANG_FORMAT}" -i "${STYLE_ARGS[@]}"
echo "Formatting applied."
