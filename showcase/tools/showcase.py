#!/usr/bin/env python3
# Copyright (c) Ian Pike
# Copyright (c) CCMath contributors
#
# CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
# See LICENSE for more information.
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
"""Run showcase accuracy, performance, and compile-time measurements."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))

from paths import REPORTS, SHOWCASE, find_build_dir, find_executable, gcem_include_dir


def run_cmd(cmd: list[str]) -> int:
    print("+", " ".join(cmd))
    return subprocess.run(cmd, check=False).returncode


def main() -> int:
    parser = argparse.ArgumentParser(description="Run showcase comparison suite")
    parser.add_argument("--build-dir", type=Path, default=None, help="Override auto-detected build directory")
    parser.add_argument("--skip-accuracy", action="store_true")
    parser.add_argument("--skip-perf", action="store_true")
    parser.add_argument("--skip-compile", action="store_true")
    args = parser.parse_args()

    build_dir = find_build_dir(args.build_dir)
    if build_dir is None:
        print("showcase: no build directory found. Run ./showcase/run build first.", file=sys.stderr)
        return 1

    print(f"showcase build_dir={build_dir}")

    REPORTS.mkdir(parents=True, exist_ok=True)
    (REPORTS / "perf").mkdir(parents=True, exist_ok=True)
    (REPORTS / "accuracy").mkdir(parents=True, exist_ok=True)
    (REPORTS / "compile").mkdir(parents=True, exist_ok=True)

    exit_code = 0
    summary: dict = {"build_dir": str(build_dir), "functions": ["sqrt", "sin"], "perf": {}, "accuracy": {}, "compile": {}}

    if not args.skip_perf:
        for fn, target in [("sqrt", "showcase_bench_sqrt"), ("sin", "showcase_bench_sin")]:
            exe = find_executable(build_dir, target)
            if exe is None:
                print(f"missing perf executable: {target}")
                exit_code = 1
                continue
            out_json = REPORTS / "perf" / f"{fn}.json"
            code = run_cmd([str(exe), f"--benchmark_out={out_json}", "--benchmark_out_format=json"])
            exit_code |= code
            if out_json.exists():
                summary["perf"][fn] = json.loads(out_json.read_text())

    if not args.skip_accuracy:
        exe = find_executable(build_dir, "showcase_accuracy")
        if exe is None:
            print("missing showcase_accuracy executable")
            exit_code = 1
        else:
            for fn in ("sqrt", "sin"):
                out_base = REPORTS / "accuracy" / fn
                code = run_cmd([str(exe), f"--function={fn}", f"--log-output={out_base}.json"])
                exit_code |= code
                summary["accuracy"][fn] = {"log_prefix": str(out_base)}

    if not args.skip_compile:
        compile_json = REPORTS / "compile" / "compile.json"
        script = SHOWCASE / "tools" / "measure_compile.py"
        compile_cmd = [sys.executable, str(script), "--json-out", str(compile_json)]
        gcem = gcem_include_dir(build_dir)
        if gcem is not None:
            compile_cmd.extend(["--gcem-include", str(gcem)])
        code = run_cmd(compile_cmd)
        exit_code |= code
        if compile_json.exists():
            summary["compile"] = json.loads(compile_json.read_text())

    summary_path = REPORTS / "summary.json"
    summary_path.write_text(json.dumps(summary, indent=2) + "\n")
    print(f"wrote {summary_path}")
    return exit_code


if __name__ == "__main__":
    sys.exit(main())
