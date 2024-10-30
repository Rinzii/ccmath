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

for /r "include\ccmath" %%f in (*.hpp) do (
    echo Checking %%f
    clang-tidy %%f -p=out/clang-tidy
    if errorlevel 1 set status=1
)

exit /b %status%
