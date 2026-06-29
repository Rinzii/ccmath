#!/usr/bin/env python3
# Copyright (c) Ian Pike
# Copyright (c) CCMath contributors
#
# CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
# See LICENSE for more information.
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
"""Smoke and full freshness checks for pow proof artifacts."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from pathlib import Path
from typing import Any

from proof_reporting import utc_timestamp, write_json_report


PROOF_DIR = Path(__file__).resolve().parent


def _parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Run combined pow proof freshness checks.")
    parser.add_argument("--smoke-only", action="store_true", help="Run smoke checks without full Gappa runs.")
    parser.add_argument("--full", action="store_true", help="Run full proof scripts including Gappa.")
    parser.add_argument(
        "--require-tools",
        action="store_true",
        help="Require gappa and sollya on PATH.",
    )
    parser.add_argument(
        "--aspirational-ulp",
        action="store_true",
        help="Gappa exp ULP checks at 0.5 and 1.5 only.",
    )
    parser.add_argument("--report", type=Path, help="Write combined freshness JSON report.")
    return parser.parse_args()


def _run_script(
    name: str,
    *,
    smoke_only: bool,
    full: bool,
    require_tools: bool,
    aspirational_ulp: bool,
) -> dict[str, Any]:
    cmd = [sys.executable, str(PROOF_DIR / name)]
    if smoke_only:
        cmd.append("--smoke-only")
    if full:
        cmd.append("--full")
    if require_tools:
        cmd.append("--require-tools")
    if aspirational_ulp:
        cmd.append("--aspirational-ulp")
    completed = subprocess.run(cmd, text=True, capture_output=True, check=False)
    return {
        "script": name,
        "command": cmd,
        "returncode": completed.returncode,
        "stdout": completed.stdout,
        "stderr": completed.stderr,
    }


def main() -> int:
    args = _parse_args()
    smoke_only = args.smoke_only and not args.full and not args.aspirational_ulp

    results = [
        _run_script(
            "pow_double_proof.py",
            smoke_only=smoke_only,
            full=args.full,
            require_tools=args.require_tools,
            aspirational_ulp=args.aspirational_ulp,
        ),
        _run_script(
            "pow_float_proof.py",
            smoke_only=smoke_only,
            full=args.full,
            require_tools=args.require_tools,
            aspirational_ulp=args.aspirational_ulp,
        ),
    ]

    if args.aspirational_ulp:
        mode = "aspirational-ulp"
    elif args.full:
        mode = "full"
    elif smoke_only:
        mode = "smoke-only"
    else:
        mode = "default"

    report: dict[str, Any] = {
        "generated_at_utc": utc_timestamp(),
        "mode": mode,
        "proof_scope": "generic_non_fma_kernel",
        "results": results,
        "status": "success" if all(item["returncode"] == 0 for item in results) else "failure",
    }

    if args.report is not None:
        write_json_report(args.report, report)

    for item in results:
        if item["stdout"]:
            print(item["stdout"], end="" if item["stdout"].endswith("\n") else "\n")
        if item["stderr"]:
            print(item["stderr"], file=sys.stderr, end="" if item["stderr"].endswith("\n") else "\n")

    return 0 if report["status"] == "success" else 1


if __name__ == "__main__":
    sys.exit(main())
