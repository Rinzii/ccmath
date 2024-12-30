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

echo "Beginning linting..."

status=0
while read -r f
do
    clang-tidy "$f" -p=out/clang-tidy --quiet 2>/dev/null
    ret=$?
    if [ $ret -ne 0 ]; then
        echo "Error in $f"
        status=1
    fi
done <<< "$(find include/ccmath -name "*.hpp")"

echo "Done linting."
exit $status
