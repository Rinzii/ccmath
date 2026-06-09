#!/usr/bin/env python3
# Copyright (c) Ian Pike
# Copyright (c) CCMath contributors
#
# CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
# See LICENSE for more information.
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
"""Shared helpers for pow proof scripts."""

from __future__ import annotations

import argparse
import hashlib
import json
import math
import platform
import re
import shlex
import subprocess
import sys
from datetime import datetime, timezone
from fractions import Fraction
from pathlib import Path
from typing import Any


def sha256_bytes(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def sha256_text(text: str) -> str:
    return sha256_bytes(text.encode("utf-8"))


def sha256_file(path: Path) -> str:
    return sha256_bytes(path.read_bytes())


def command_string(cmd: list[str]) -> str:
    return " ".join(shlex.quote(part) for part in cmd)


def run_command(cmd: list[str], *, input_text: str | None = None) -> dict[str, Any]:
    result: dict[str, Any] = {
        "command": cmd,
        "command_string": command_string(cmd),
        "stdin_sha256": sha256_text(input_text) if input_text is not None else None,
    }

    try:
        completed = subprocess.run(
            cmd,
            input=input_text,
            text=True,
            capture_output=True,
            check=False,
        )
    except FileNotFoundError:
        result.update(
            {
                "available": False,
                "returncode": None,
                "stdout": "",
                "stderr": "",
            }
        )
        return result

    result.update(
        {
            "available": True,
            "returncode": completed.returncode,
            "stdout": completed.stdout,
            "stderr": completed.stderr,
        }
    )
    return result


def utc_timestamp() -> str:
    return datetime.now(timezone.utc).replace(microsecond=0).isoformat()


PROOF_TOOL_NAMES = ("sollya", "gappa")


def missing_proof_tools() -> list[str]:
    return [name for name in PROOF_TOOL_NAMES if not run_command([name, "--version"])["available"]]


def format_missing_tools(missing: list[str]) -> str:
    return ", ".join(missing)


def write_json_report(path: Path, payload: dict[str, Any]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(payload, indent=2, sort_keys=True) + "\n", encoding="utf-8")


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def strip_cpp_comments(text: str) -> str:
    return re.sub(r"//.*", "", text)


def extract_brace_array(text: str, name: str) -> list[str]:
    match = re.search(rf"\b{name}\b\s*=\s*\{{(.*?)\}};", text, re.DOTALL)
    if match is None:
        raise RuntimeError(f"Could not locate array {name}")
    body = strip_cpp_comments(match.group(1))
    return [token.strip() for token in body.split(",") if token.strip()]


def extract_regex_array(text: str, pattern: str) -> list[str]:
    match = re.search(rf"{pattern}\s*=\s*\{{(.*?)\}};", text, re.DOTALL)
    if match is None:
        raise RuntimeError(f"Could not locate array matching: {pattern}")
    body = strip_cpp_comments(match.group(1))
    return [token.strip() for token in body.split(",") if token.strip()]


def parse_gappa_bound(path: Path, var: str) -> Fraction:
    text = read_text(path)
    match = re.search(rf"{re.escape(var)}\s+in\s+\[-(\d+)b(-?\d+),\s*\1b\2\]", text)
    if match is None:
        raise RuntimeError(f"Could not find assertion for '{var}' in {path.name}")
    mantissa, exponent = int(match.group(1)), int(match.group(2))
    return Fraction(mantissa) * Fraction(2) ** exponent


def run_gappa(path: Path) -> dict[str, Any]:
    result = run_command(["gappa", str(path)])
    if not result["available"]:
        raise RuntimeError("Gappa is not installed or not in PATH")
    if result["returncode"] != 0:
        raise RuntimeError(f"Gappa proof FAILED for {path.name}:\n{result['stderr'].strip()}")
    return result


def fraction_hex(frac: Fraction) -> str:
    return float(frac).hex()


def sollya_upper_bound(hex_str: str) -> Fraction:
    value = float.fromhex(hex_str.strip())
    return Fraction.from_float(math.nextafter(value, math.inf))


def check_fast_math_disabled() -> None:
    probe = "#include <iostream>\nint main(){#if defined(__FAST_MATH__)\\nstd::cout<<\\\"on\\\";\\n#else\\nstd::cout<<\\\"off\\\";\\n#endif\\nreturn 0;\\n}"
    result = run_command(["c++", "-x", "c++", "-", "-o", "/tmp/ccmath-proof-fast-math-probe"], input_text=probe)
    if not result["available"]:
        return
    run_result = run_command(["/tmp/ccmath-proof-fast-math-probe"])
    if run_result.get("stdout", "").strip() == "on":
        raise RuntimeError("__FAST_MATH__ is active in proof-mode compile probe")


def proof_base_report(
    *,
    root: Path,
    script_path: Path,
    hashed_paths: list[Path],
    args: argparse.Namespace,
) -> dict[str, Any]:
    return {
        "generated_at_utc": utc_timestamp(),
        "script": str(script_path.relative_to(root)),
        "invocation": [sys.executable, str(script_path), *sys.argv[1:]],
        "proof_scope": "generic_non_fma_kernel",
        "path_configuration": "generic_runtime_kernel_not_public_api",
        "environment": {
            "python": sys.version,
            "platform": platform.platform(),
            "machine": platform.machine(),
        },
        "tool_versions": {
            "gappa": run_command(["gappa", "--version"]),
            "sollya": run_command(["sollya", "--version"]),
        },
        "source_hashes": {
            str(path.relative_to(root)): sha256_file(path) for path in hashed_paths
        },
        "report_path": str(args.report) if args.report is not None else None,
    }


def maybe_write_report(args: argparse.Namespace, report: dict[str, Any]) -> None:
    if args.report is not None:
        write_json_report(args.report, report)


def add_require_tools_arg(parser: argparse.ArgumentParser) -> None:
    parser.add_argument(
        "--require-tools",
        action="store_true",
        help="Require gappa and sollya on PATH.",
    )


def add_aspirational_ulp_arg(parser: argparse.ArgumentParser) -> None:
    parser.add_argument(
        "--aspirational-ulp",
        action="store_true",
        help="Gappa exp ULP checks at 0.5 and 1.5 only.",
    )


def raise_if_required_tools(args: argparse.Namespace, missing: list[str]) -> None:
    if not missing or not getattr(args, "require_tools", False):
        return
    if "sollya" in missing:
        raise RuntimeError("Sollya is not installed or not in PATH")
    raise RuntimeError("Gappa is not installed or not in PATH")


def skip_if_tools_missing(
    args: argparse.Namespace,
    report: dict[str, Any],
    *,
    skip_label: str,
) -> bool:
    missing = missing_proof_tools()
    if not missing:
        return False
    raise_if_required_tools(args, missing)
    print(
        f"WARNING: {format_missing_tools(missing)} not on PATH, skipping {skip_label}",
        file=sys.stderr,
    )
    report["status"] = "skipped"
    report["skipped_tools"] = missing
    report["exit_status"] = 0
    return True


def finalize_proof_smoke(args: argparse.Namespace, report: dict[str, Any], *, title: str) -> int:
    missing = missing_proof_tools()
    raise_if_required_tools(args, missing)
    if missing:
        print(
            f"WARNING: {format_missing_tools(missing)} not on PATH, skipping tool checks",
            file=sys.stderr,
        )
        report["status"] = "skipped"
        report["skipped_tools"] = missing
    else:
        report["status"] = "success"
    report["exit_status"] = 0
    report["smoke_only"] = True
    maybe_write_report(args, report)
    print(title)
    print("  source parse               : PASS")
    for tool in PROOF_TOOL_NAMES:
        if tool in missing:
            print(f"  {tool} presence            : SKIP")
        else:
            print(f"  {tool} presence            : PASS")
    print("  fast_math probe            : PASS")
    print("  proof scope                : generic_non_fma_kernel")
    if args.report is not None:
        print(f"  json report                : {args.report}")
    return 0


BLOCKING_ULP_POLICY_DOUBLE = Fraction(4)
ASPIRATIONAL_ULP_TIERS: tuple[tuple[str, Fraction], ...] = (
    ("0.5 ULP", Fraction(1, 2)),
    ("1.5 ULP", Fraction(3, 2)),
)


def aspirational_ulp_results(ulp_bound: Fraction) -> list[dict[str, Any]]:
    entries: list[dict[str, Any]] = []
    for label, threshold in ASPIRATIONAL_ULP_TIERS:
        passed = ulp_bound < threshold
        entries.append(
            {
                "tier": label,
                "threshold": float(threshold),
                "ulp_bound": float(ulp_bound),
                "passed": passed,
            }
        )
    return entries


def _format_ulp_bound(value: float) -> str:
    if abs(value) < 0.01:
        return f"{value:.2e}"
    return f"{value:.4f}"


def emit_aspirational_ulp(entries: list[dict[str, Any]], *, phase: str) -> None:
    for entry in entries:
        label = entry["tier"]
        ulp_text = _format_ulp_bound(entry["ulp_bound"])
        status = "PASS" if entry["passed"] else "MISS"
        print(f"  aspirational {label:<10} : {status} ({ulp_text})")
        if not entry["passed"]:
            print(
                f"WARNING: {phase} exp bound {ulp_text} exceeds aspirational {label}",
                file=sys.stderr,
            )
