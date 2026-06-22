#!/usr/bin/env python3
# Copyright (c) Ian Pike
# Copyright (c) CCMath contributors
#
# CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
# See LICENSE for more information.
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
"""CLI helpers for showcase/run."""

from __future__ import annotations

import sys

from paths import find_build_dir, find_executable, gcem_include_dir


def main() -> int:
    if len(sys.argv) < 2:
        return 1

    command = sys.argv[1]
    if command == "build-dir":
        build_dir = find_build_dir()
        if build_dir is None:
            return 1
        print(build_dir)
        return 0

    if command == "exe":
        if len(sys.argv) != 3:
            return 1
        build_dir = find_build_dir()
        if build_dir is None:
            return 1
        exe = find_executable(build_dir, sys.argv[2])
        if exe is None:
            return 1
        print(exe)
        return 0

    if command == "gcem-include":
        build_dir = find_build_dir()
        include_dir = gcem_include_dir(build_dir)
        if include_dir is None:
            return 1
        print(include_dir)
        return 0

    return 1


if __name__ == "__main__":
    sys.exit(main())
