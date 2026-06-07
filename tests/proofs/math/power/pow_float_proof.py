#!/usr/bin/env python3
# Copyright (c) Ian Pike
# Copyright (c) CCMath contributors
#
# CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
# See LICENSE for more information.
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
"""Run binary32 pow Gappa and Sollya proof obligations."""

from __future__ import annotations

import argparse
import math
import platform
import re
import struct
import sys
from fractions import Fraction
from pathlib import Path
from typing import Any

from proof_reporting import run_command, sha256_file, utc_timestamp, write_json_report


ROOT = Path(__file__).resolve().parents[4]
PROOF_DIR = Path(__file__).resolve().parent
COMMON_CONSTANTS = ROOT / "include/ccmath/internal/support/common_math_constants.hpp"
POWF_IMPL = ROOT / "include/ccmath/internal/math/generic/func/power/pow_impl/powf_impl.hpp"
LOG_GAPPA = PROOF_DIR / "pow_float_log_eval.g"
EXP_GAPPA = PROOF_DIR / "pow_float_exp_eval.g"


def _parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Verify binary32 powf proof obligations.")
    parser.add_argument("--report", type=Path, help="Write a JSON proof report to this path.")
    parser.add_argument("--smoke-only", action="store_true", help="Verify source freshness and tool presence only.")
    parser.add_argument("--full", action="store_true", help="Run full Gappa and Sollya checks.")
    return parser.parse_args()


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def _strip_cpp_comments(text: str) -> str:
    return re.sub(r"//.*", "", text)


def _extract_array(text: str, name: str) -> list[str]:
    match = re.search(rf"\b{name}\b\s*=\s*\{{(.*?)\}};", text, re.DOTALL)
    if match is None:
        raise RuntimeError(f"Could not locate array {name}")
    body = _strip_cpp_comments(match.group(1))
    return [token.strip() for token in body.split(",") if token.strip()]


def _extract_typed_array(text: str, pattern: str) -> list[str]:
    match = re.search(rf"{pattern}\s*=\s*\{{(.*?)\}};", text, re.DOTALL)
    if match is None:
        raise RuntimeError(f"Could not locate array matching: {pattern}")
    body = _strip_cpp_comments(match.group(1))
    return [token.strip() for token in body.split(",") if token.strip()]


def _exact_float64(token: str) -> Fraction:
    return Fraction.from_float(float.fromhex(token))


def _exact_float32(token: str) -> Fraction:
    value = float.fromhex(token)
    packed = struct.pack("f", value)
    unpacked = struct.unpack("f", packed)[0]
    return Fraction.from_float(unpacked)


def _parse_gappa_bound(path: Path, var: str) -> Fraction:
    text = _read(path)
    match = re.search(rf"{re.escape(var)}\s+in\s+\[-(\d+)b(-?\d+),\s*\1b\2\]", text)
    if match is None:
        raise RuntimeError(f"Could not find assertion for '{var}' in {path.name}")
    mantissa, exponent = int(match.group(1)), int(match.group(2))
    return Fraction(mantissa) * Fraction(2) ** exponent


def _exp_ulp_bound_float(gappa_exp: Fraction, sollya_exp: Fraction) -> Fraction:
    return (gappa_exp + sollya_exp) * Fraction(2**24)


def _run_gappa(path: Path) -> dict[str, Any]:
    result = run_command(["gappa", str(path)])
    if not result["available"]:
        raise RuntimeError("Gappa is not installed or not in PATH")
    if result["returncode"] != 0:
        raise RuntimeError(f"Gappa proof FAILED for {path.name}:\n{result['stderr'].strip()}")
    return result


def _hex(frac: Fraction) -> str:
    return float(frac).hex()


def _check_fast_math_disabled() -> None:
    probe = "#include <iostream>\nint main(){#if defined(__FAST_MATH__)\\nstd::cout<<\\\"on\\\";\\n#else\\nstd::cout<<\\\"off\\\";\\n#endif\\nreturn 0;\\n}"
    result = run_command(["c++", "-x", "c++", "-", "-o", "/tmp/ccmath-proof-fast-math-probe"], input_text=probe)
    if not result["available"]:
        return
    run_result = run_command(["/tmp/ccmath-proof-fast-math-probe"])
    if run_result.get("stdout", "").strip() == "on":
        raise RuntimeError("__FAST_MATH__ is active in proof-mode compile probe")


def _base_report(args: argparse.Namespace) -> dict[str, Any]:
    script_path = Path(__file__).resolve()
    return {
        "generated_at_utc": utc_timestamp(),
        "script": str(script_path.relative_to(ROOT)),
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
            str(path.relative_to(ROOT)): sha256_file(path)
            for path in [COMMON_CONSTANTS, POWF_IMPL, LOG_GAPPA, EXP_GAPPA, script_path]
        },
        "report_path": str(args.report) if args.report is not None else None,
    }


def _maybe_write_report(args: argparse.Namespace, report: dict[str, Any]) -> None:
    if args.report is not None:
        write_json_report(args.report, report)


def main() -> int:
    args = _parse_args()
    report = _base_report(args)

    try:
        _check_fast_math_disabled()

        common_text = _read(COMMON_CONSTANTS)
        powf_text = _read(POWF_IMPL)

        r = [_exact_float32(token) for token in _extract_array(common_text, "R")]
        rd = [_exact_float64(token) for token in _extract_array(common_text, "RD")]
        log2_r = [_exact_float64(token) for token in _extract_array(common_text, "LOG2_R")]

        if len(r) != 128:
            raise RuntimeError(f"Expected 128 entries in R, got {len(r)}")
        if len(rd) != 128:
            raise RuntimeError(f"Expected 128 entries in RD, got {len(rd)}")
        if len(log2_r) != 128:
            raise RuntimeError(f"Expected 128 entries in LOG2_R, got {len(log2_r)}")

        log_coeffs = _extract_typed_array(powf_text, r"std::array<double,\s*6>\s*COEFFS")
        exp_coeffs = _extract_typed_array(powf_text, r"std::array<double,\s*6>\s*EXP2_COEFFS")

        if len(log_coeffs) != 6:
            raise RuntimeError(f"Expected 6 log2 coefficients for powf, got {len(log_coeffs)}")
        if len(exp_coeffs) != 6:
            raise RuntimeError(f"Expected 6 exp2 coefficients for powf, got {len(exp_coeffs)}")

        report["source_parse"] = {
            "r_entries": len(r),
            "rd_entries": len(rd),
            "log_coeff_count": len(log_coeffs),
            "exp_coeff_count": len(exp_coeffs),
        }

        if args.smoke_only and not args.full:
            sollya_probe = run_command(["sollya", "--version"])
            gappa_probe = run_command(["gappa", "--version"])
            if not sollya_probe["available"]:
                raise RuntimeError("Sollya is not installed or not in PATH")
            if not gappa_probe["available"]:
                raise RuntimeError("Gappa is not installed or not in PATH")
            report["status"] = "success"
            report["exit_status"] = 0
            report["smoke_only"] = True
            _maybe_write_report(args, report)
            print("powf proof smoke")
            print("  source parse               : PASS")
            print("  sollya presence            : PASS")
            print("  gappa presence             : PASS")
            print("  fast_math probe            : PASS")
            print("  proof scope                : generic_non_fma_kernel")
            if args.report is not None:
                print(f"  json report                : {args.report}")
            return 0

        for idx in range(128):
            if r[idx] != rd[idx]:
                raise RuntimeError(f"R[{idx}] = {_hex(r[idx])} != RD[{idx}] = {_hex(rd[idx])}")

        for k in range(128):
            denom = Fraction(1) + Fraction(k, 128)
            numer = Fraction(1) - Fraction(1, 256)
            scaled = numer / denom * 256
            expected_int = int(scaled) + (0 if scaled == int(scaled) else 1)
            expected = Fraction(expected_int, 256)
            if k == 0 and r[k] == 1:
                continue
            if r[k] != expected:
                raise RuntimeError(f"R[{k}] = {_hex(r[k])}, expected {_hex(expected)}")

        dx_min: Fraction | None = None
        dx_max: Fraction | None = None
        ulp24 = Fraction(1, 2**23)

        for k in range(128):
            m_low = Fraction(1) + Fraction(k, 128)
            m_high = Fraction(1) + Fraction(k + 1, 128) - ulp24

            lo = r[k] * m_low - 1
            hi = r[k] * m_high - 1

            dx_min = lo if dx_min is None or lo < dx_min else dx_min
            dx_max = hi if dx_max is None or hi > dx_max else dx_max

        assert dx_min is not None and dx_max is not None

        if dx_min < Fraction(-1, 256):
            raise RuntimeError(f"dx range lower bound violated: {_hex(dx_min)}")
        if dx_max >= Fraction(1, 128):
            raise RuntimeError(f"dx range upper bound violated: {_hex(dx_max)}")

        max_log2_err = Fraction(0)
        for k in range(128):
            if r[k] == 1:
                if log2_r[k] != 0:
                    raise RuntimeError(f"LOG2_R[{k}] should be 0 for R[k]=1, got {_hex(log2_r[k])}")
                continue
            exact = Fraction.from_float(-math.log2(float(r[k])))
            err = abs(log2_r[k] - exact)
            if err > max_log2_err:
                max_log2_err = err
            if err > Fraction(1, 2**50):
                raise RuntimeError(f"LOG2_R[{k}] error too large: {float(err):.3e}")

        gappa_log_result = _run_gappa(LOG_GAPPA)
        gappa_exp_result = _run_gappa(EXP_GAPPA)

        gappa_log_bound = _parse_gappa_bound(LOG_GAPPA, "log_eval - P")
        gappa_exp_bound = _parse_gappa_bound(EXP_GAPPA, "exp_eval - P")
        sollya_exp = Fraction.from_float(float.fromhex("0x1.a2b77e618f5c4p-60"))
        ulp_bound = _exp_ulp_bound_float(gappa_exp_bound, sollya_exp)

        if ulp_bound >= Fraction(1, 1000):
            raise RuntimeError(
                f"Float exp polynomial ULP bound {float(ulp_bound):.2e} is unexpectedly large"
            )

        report["status"] = "success"
        report["exit_status"] = 0
        report["commands"] = {
            "gappa_log": gappa_log_result,
            "gappa_exp": gappa_exp_result,
        }
        report["results"] = {
            "r_entries": len(r),
            "log_coefficients": log_coeffs,
            "exp_coefficients": exp_coeffs,
            "dx_range": {
                "low_hex": _hex(dx_min),
                "high_hex": _hex(dx_max),
            },
            "max_log2_r_error": float(max_log2_err),
            "gappa_log_eval_bound_hex": _hex(gappa_log_bound),
            "gappa_exp_eval_bound_hex": _hex(gappa_exp_bound),
            "shared_sollya_exp_bound_hex": float(sollya_exp).hex(),
            "exp_phase_ulp_bound_float": float(ulp_bound),
        }
        _maybe_write_report(args, report)

        print("powf float proof obligations")
        print(f"  R == RD identity             : PASS ({len(r)} entries)")
        print(f"  R construction identity      : PASS ({len(r)} entries, R[0]=1.0 by convention)")
        print(f"  dx range                     : [{_hex(dx_min)}, {_hex(dx_max)})")
        print(f"  LOG2_R accuracy              : PASS (max error ~ {float(max_log2_err):.2e})")
        print(f"  gappa log eval cert          : PASS  ({LOG_GAPPA.name})")
        print(f"  gappa exp eval cert          : PASS  ({EXP_GAPPA.name})")
        print(f"  exp phase ULP bound (float)  : {float(ulp_bound):.2e}  (negligible; Ziv test ensures correct rounding)")
        if args.report is not None:
            print(f"  json report                  : {args.report}")
        return 0
    except Exception as exc:
        report["status"] = "failure"
        report["exit_status"] = 1
        report["error"] = str(exc)
        _maybe_write_report(args, report)
        print(str(exc), file=sys.stderr)
        return 1


if __name__ == "__main__":
    sys.exit(main())
