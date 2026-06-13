#!/usr/bin/env python3
# Copyright (c) Ian Pike
# Copyright (c) CCMath contributors
#
# CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
# See LICENSE for more information.
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
"""Measure compile-time cost and object footprint for showcase backends."""

from __future__ import annotations

import argparse
import json
import os
import shutil
import subprocess
import sys
import time
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SHOWCASE = ROOT / "showcase"
CONFIG = json.loads((SHOWCASE / "config" / "functions.json").read_text())
TEMPLATE = (SHOWCASE / "common" / "compile" / "probe_template.cpp.in").read_text()

CCMATH_PATHS = {
    "sqrt": [
        ("ccmath/public_default", '#include "ccmath/math/power/sqrt.hpp"', "ccm::sqrt({x})"),
        ("ccmath/generic_gen", '#include "ccmath/internal/math/generic/func/power/sqrt_gen.hpp"', "ccm::gen::sqrt_gen<double>({x})"),
        ("ccmath/runtime_rt", '#include "ccmath/internal/math/runtime/func/power/sqrt_rt.hpp"', "ccm::rt::sqrt_rt<double>({x})"),
        ("ccmath/runtime_simd", '#include "ccmath/internal/math/runtime/simd/func/catalog.hpp"\n#include "ccmath/internal/math/runtime/func/rt_dispatch.hpp"', "ccm::rt::simd_impl::sqrt_simd_impl({x})"),
        ("ccmath/runtime_builtin", '#include <cmath>', "__builtin_sqrt({x})"),
    ],
    "sin": [
        ("ccmath/public_default", '#include "ccmath/math/trig/sin.hpp"', "ccm::sin({x})"),
        ("ccmath/generic_gen", '#include "ccmath/internal/math/generic/func/trig/sin_gen.hpp"', "ccm::gen::sin_gen({x})"),
        ("ccmath/runtime_rt", '#include "ccmath/internal/math/runtime/func/trig/sin_rt.hpp"', "ccm::rt::sin_rt({x})"),
        ("ccmath/runtime_builtin", '#include <cmath>', "__builtin_sin({x})"),
    ],
}

GCEM_BACKENDS = {
    "sqrt": ('gcem', '#include "gcem.hpp"', "gcem::sqrt({x})"),
    "sin": ('gcem', '#include "gcem.hpp"', "gcem::sin({x})"),
}


def cxx_compiler() -> str:
    return os.environ.get("CXX", "clang++")


def render_probe(function: str, label: str, include_line: str, expr: str, inputs: list[float]) -> str:
    cases = []
    for i, value in enumerate(inputs):
        cases.append(f"  static_assert({expr.format(x=repr(value))} == {expr.format(x=repr(value))});")
    include_block = include_line
    return (
        TEMPLATE.replace("@@INCLUDE_BLOCK@@", include_block)
        .replace("@@CONSTEXPR_CASES@@", "\n".join(cases))
    )


def object_text_bytes(obj_path: Path) -> int | None:
    size_tool = shutil.which("llvm-size") or shutil.which("size")
    if size_tool is None or not obj_path.exists():
        return None
    proc = subprocess.run([size_tool, "-A", str(obj_path)], capture_output=True, text=True, check=False)
    if proc.returncode != 0:
        return None
    for line in proc.stdout.splitlines():
        parts = line.split()
        if len(parts) >= 2 and parts[0] == ".text":
            try:
                return int(parts[1]) * (16 if "llvm-size" in size_tool else 1)
            except ValueError:
                return int(parts[1])
    return None


def compile_probe(src: Path, obj: Path, include_dirs: list[Path], extra_flags: list[str]) -> dict:
    cmd = [
        cxx_compiler(),
        "-std=c++20",
        "-O2",
        "-c",
        *[f"-I{d}" for d in include_dirs],
        *extra_flags,
        "-o",
        str(obj),
        str(src),
    ]
    start = time.perf_counter()
    proc = subprocess.run(cmd, capture_output=True, text=True, check=False)
    elapsed = time.perf_counter() - start
    result = {
        "compile_seconds": elapsed,
        "compile_ok": proc.returncode == 0,
        "object_bytes": obj.stat().st_size if obj.exists() else 0,
        "text_section_bytes": object_text_bytes(obj),
        "command": " ".join(cmd),
    }
    if proc.returncode != 0:
        result["stderr"] = proc.stderr[-4000:]
    return result


def run(function: str, out_dir: Path, gcem_include: Path | None) -> list[dict]:
    inputs = CONFIG[function]["compile_probe_inputs"]
    include_dirs = [ROOT / "include", ROOT / "showcase" / "common"]
    extra_flags: list[str] = []
    if gcem_include is not None:
        include_dirs.append(gcem_include)

    rows: list[dict] = []
    out_dir.mkdir(parents=True, exist_ok=True)

    for label, include_line, expr in CCMATH_PATHS[function]:
        src = out_dir / f"{function}_{label.replace('/', '_')}.cpp"
        obj = out_dir / f"{function}_{label.replace('/', '_')}.o"
        src.write_text(render_probe(function, label, include_line, expr, inputs))
        metrics = compile_probe(src, obj, include_dirs, extra_flags)
        rows.append({"function": function, "backend": label, **metrics})

    if gcem_include is not None:
        label, include_line, expr = GCEM_BACKENDS[function]
        src = out_dir / f"{function}_{label}.cpp"
        obj = out_dir / f"{function}_{label}.o"
        src.write_text(render_probe(function, label, include_line, expr, inputs))
        metrics = compile_probe(src, obj, include_dirs, extra_flags)
        rows.append({"function": function, "backend": label, **metrics})

    return rows


def main() -> int:
    parser = argparse.ArgumentParser(description="Measure showcase compile probes")
    parser.add_argument("--function", choices=["sqrt", "sin", "all"], default="all")
    parser.add_argument("--out-dir", type=Path, default=SHOWCASE / "compile_time" / "probes")
    parser.add_argument("--json-out", type=Path, default=SHOWCASE / "reports" / "compile" / "compile.json")
    parser.add_argument("--gcem-include", type=Path, default=None)
    parser.add_argument("--build-dir", type=Path, default=None, help="Used to locate GCEM headers when --gcem-include is omitted")
    args = parser.parse_args()

    sys.path.insert(0, str(Path(__file__).resolve().parent))
    from paths import find_build_dir, gcem_include_dir

    gcem_include = args.gcem_include
    if gcem_include is None:
        gcem_include = gcem_include_dir(find_build_dir(args.build_dir))

    functions = ["sqrt", "sin"] if args.function == "all" else [args.function]
    all_rows: list[dict] = []
    for fn in functions:
        all_rows.extend(run(fn, args.out_dir / fn, gcem_include))

    args.json_out.parent.mkdir(parents=True, exist_ok=True)
    args.json_out.write_text(json.dumps(all_rows, indent=2) + "\n")
    print(f"wrote {args.json_out} rows={len(all_rows)}")
    return 0 if all(row.get("compile_ok", False) for row in all_rows) else 1


if __name__ == "__main__":
    sys.exit(main())
