#!/bin/bash

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