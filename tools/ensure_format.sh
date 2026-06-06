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

STYLE_ARGS=(-style=file -fallback-style=none)

if ! find include -name '*.hpp' -print -quit | grep -q .; then
    echo "No header files found under include/."
    exit 0
fi

if find include -name '*.hpp' | xargs clang-format --dry-run --Werror "${STYLE_ARGS[@]}"; then
    echo "Formatting is up to date."
    exit 0
fi

echo "Formatting required. Applying clang-format..."
find include -name '*.hpp' | xargs clang-format -i "${STYLE_ARGS[@]}"
echo "Formatting applied."
