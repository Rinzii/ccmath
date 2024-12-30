@echo off

REM
REM Copyright (c) Ian Pike
REM Copyright (c) CCMath contributors
REM
REM CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
REM See LICENSE for more information.
REM
REM SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
REM

set status=0

echo Beginning linting...
for /r "include\ccmath" %%f in (*.hpp) do (
    clang-tidy %%f -p=out/clang-tidy --quiet 2>nul
    if errorlevel 1 (
        echo Error in %%f
        set status=1
    )
)
echo Linting complete.

exit /b %status%
