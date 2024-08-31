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

status=0
while read f
do
    echo checking $f
    clang-tidy $f -p=out/clang-tidy
    ret=$?
    if [ $ret -ne 0 ]; then
        status=1
    fi
done <<< $(find include/ccmath -name "*.hpp")
exit $status
