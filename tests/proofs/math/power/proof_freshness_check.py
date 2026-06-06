#!/usr/bin/env python3

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
    parser.add_argument("--report", type=Path, help="Write combined freshness JSON report.")
    return parser.parse_args()


def _run_script(name: str, smoke_only: bool, full: bool) -> dict[str, Any]:
    cmd = [sys.executable, str(PROOF_DIR / name)]
    if smoke_only:
        cmd.append("--smoke-only")
    if full:
        cmd.append("--full")
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
    smoke_only = args.smoke_only and not args.full

    results = [
        _run_script("pow_double_proof.py", smoke_only, args.full),
        _run_script("pow_float_proof.py", smoke_only, args.full),
    ]

    report: dict[str, Any] = {
        "generated_at_utc": utc_timestamp(),
        "mode": "full" if args.full else ("smoke-only" if smoke_only else "default"),
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
