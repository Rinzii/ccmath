#!/usr/bin/env python3
# Copyright (c) Ian Pike
# Copyright (c) CCMath contributors
#
# CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
# See LICENSE for more information.
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
"""Shared paths for showcase tools and the showcase/run entrypoint."""

from __future__ import annotations

import os
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SHOWCASE = ROOT / "showcase"
REPORTS = SHOWCASE / "reports"

CLION_BUILD_DIR = ROOT / "out" / "clang-showcase"
NINJA_BUILD_DIR = ROOT / "out" / "build" / "ninja" / "ninja-clang-release-showcase"

CLION_PRESET = "clion-clang-release-showcase"
NINJA_PRESET = "ninja-clang-release-showcase"


def build_dir_candidates() -> list[Path]:
    explicit = os.environ.get("CCMATH_SHOWCASE_BUILD_DIR")
    rows: list[Path] = []
    if explicit:
        rows.append(Path(explicit))
    rows.extend([CLION_BUILD_DIR, NINJA_BUILD_DIR])
    return rows


def find_build_dir(explicit: Path | None = None) -> Path | None:
    if explicit is not None:
        return explicit if explicit.is_dir() else None

    for candidate in build_dir_candidates():
        if candidate.is_dir() and find_executable(candidate, "showcase_bench_sqrt") is not None:
            return candidate

    for name in ("showcase_bench_sqrt", "showcase_accuracy"):
        for match in sorted(ROOT.glob(f"out/**/{name}")):
            cur = match.parent
            for _ in range(8):
                if (cur / "CMakeCache.txt").is_file():
                    return cur
                cur = cur.parent
    return None


def find_executable(build_dir: Path, name: str) -> Path | None:
    for candidate in [
        build_dir / "showcase" / "performance" / name,
        build_dir / "showcase" / "performance" / "Release" / name,
        build_dir / "showcase" / "accuracy" / name,
        build_dir / "showcase" / "accuracy" / "Release" / name,
    ]:
        if candidate.exists():
            return candidate
    matches = list(build_dir.rglob(name))
    return matches[0] if matches else None


def gcem_include_dir(build_dir: Path | None) -> Path | None:
    if build_dir is None:
        return None
    candidate = build_dir / "_deps" / "gcem-src" / "include"
    return candidate if candidate.is_dir() else None
