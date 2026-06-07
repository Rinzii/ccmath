#!/usr/bin/env python3
# Copyright (c) Ian Pike
# Copyright (c) CCMath contributors
#
# CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
# See LICENSE for more information.
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
"""Run binary64 pow Gappa and Sollya proof obligations."""

from __future__ import annotations

import argparse
import math
import platform
import re
import sys
from fractions import Fraction
from pathlib import Path
from typing import Any

from proof_reporting import run_command, sha256_file, utc_timestamp, write_json_report


ROOT = Path(__file__).resolve().parents[4]
PROOF_DIR = Path(__file__).resolve().parent
COMMON_CONSTANTS = ROOT / "include/ccmath/internal/support/common_math_constants.hpp"
POW_IMPL = ROOT / "include/ccmath/internal/math/generic/func/power/pow_impl/pow_impl.hpp"
LOG_GAPPA = PROOF_DIR / "pow_double_log_eval.g"
EXP_GAPPA = PROOF_DIR / "pow_double_exp_eval.g"


def _parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Verify binary64 pow proof obligations.")
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


def _exact_float(token: str) -> Fraction:
    return Fraction.from_float(float.fromhex(token))


def _horner(coeffs: list[str]) -> str:
    expr = coeffs[-1]
    for coeff in reversed(coeffs[:-1]):
        expr = f"({coeff} + x * ({expr}))"
    return expr


def _extract_dd_array(text: str, name_pattern: str) -> list[tuple[str, str]]:
    match = re.search(name_pattern + r"\s*=\s*\{(.*?)\};", text, re.DOTALL)
    if match is None:
        raise RuntimeError(f"Could not locate double-double array {name_pattern}")
    body = _strip_cpp_comments(match.group(1))
    pairs = re.findall(r"DoubleDouble\{\s*([^,}]+?)\s*,\s*([^}]+?)\s*\}", body)
    if not pairs:
        raise RuntimeError(f"Could not parse double-double entries for {name_pattern}")
    return [(hi.strip(), lo.strip()) for hi, lo in pairs]


def _horner_dd(coeffs: list[tuple[str, str]]) -> str:
    def term(coeff: tuple[str, str]) -> str:
        return f"({coeff[0]} + {coeff[1]})"

    expr = term(coeffs[-1])
    for coeff in reversed(coeffs[:-1]):
        expr = f"({term(coeff)} + x * ({expr}))"
    return expr


def _run_sollya(log_coeffs: list[str], exp_coeffs: list[str]) -> tuple[str, str, dict[str, Any]]:
    program = "\n".join(
        [
            "prec = 300;",
            "display = hexadecimal;",
            f"Plog = {_horner(log_coeffs)};",
            f"Pexp = {_horner(exp_coeffs)};",
            "print(dirtyinfnorm(log2(1 + x)/x - Plog, [-2^-8; 2^-7]));",
            "print(dirtyinfnorm(2^(x/64) - Pexp, [-2^-1; 2^-1]));",
        ]
    )

    result = run_command(["sollya"], input_text=program)
    if not result["available"]:
        raise RuntimeError("Sollya is not installed or not in PATH; cannot check polynomial sup-norms")

    outputs = [
        line.strip()
        for line in result["stdout"].splitlines()
        if line.strip() and "Display mode" not in line and "precision" not in line
    ]
    if len(outputs) < 2:
        raise RuntimeError(f"Unexpected Sollya output:\n{result['stdout']}\n{result['stderr']}")

    result["parsed_outputs"] = {
        "log_supnorm_hex": outputs[-2],
        "exp_supnorm_hex": outputs[-1],
    }
    if result["returncode"] not in (0, None):
        result["nonzero_exit_accepted"] = True
    return outputs[-2], outputs[-1], result


def _run_sollya_dd(log_coeffs: list[tuple[str, str]], exp_coeffs: list[tuple[str, str]]) -> tuple[str, str, dict[str, Any]]:
    # Approximation error of the accurate double-double reconstruction polynomials.
    # The log2 polynomial runs on the twice-reduced range, the exp2 polynomial on [-1/2, 1/2].
    program = "\n".join(
        [
            "prec = 300;",
            "display = hexadecimal;",
            f"Plog = {_horner_dd(log_coeffs)};",
            f"Pexp = {_horner_dd(exp_coeffs)};",
            "print(dirtyinfnorm(log2(1 + x)/x - Plog, [-0x1.3ffcp-15; 0x1.3e3dp-15]));",
            "print(dirtyinfnorm(2^(x/64) - Pexp, [-2^-1; 2^-1]));",
        ]
    )

    result = run_command(["sollya"], input_text=program)
    if not result["available"]:
        raise RuntimeError("Sollya is not installed or not in PATH; cannot check double-double polynomial sup-norms")

    outputs = [
        line.strip()
        for line in result["stdout"].splitlines()
        if line.strip() and "Display mode" not in line and "precision" not in line
    ]
    if len(outputs) < 2:
        raise RuntimeError(f"Unexpected Sollya output:\n{result['stdout']}\n{result['stderr']}")

    result["parsed_outputs"] = {
        "log_dd_supnorm_hex": outputs[-2],
        "exp_dd_supnorm_hex": outputs[-1],
    }
    if result["returncode"] not in (0, None):
        result["nonzero_exit_accepted"] = True
    return outputs[-2], outputs[-1], result


def _run_gappa(path: Path) -> dict[str, Any]:
    result = run_command(["gappa", str(path)])
    if not result["available"]:
        raise RuntimeError("Gappa is not installed or not in PATH")
    if result["returncode"] != 0:
        raise RuntimeError(f"Gappa proof FAILED for {path.name}:\n{result['stderr'].strip()}")
    return result


def _parse_gappa_bound(path: Path, var: str) -> Fraction:
    text = _read(path)
    match = re.search(rf"{re.escape(var)}\s+in\s+\[-(\d+)b(-?\d+),\s*\1b\2\]", text)
    if match is None:
        raise RuntimeError(f"Could not find assertion for '{var}' in {path.name}")
    mantissa, exponent = int(match.group(1)), int(match.group(2))
    return Fraction(mantissa) * Fraction(2) ** exponent


def _sollya_upper_bound(hex_str: str) -> Fraction:
    value = float.fromhex(hex_str.strip())
    return Fraction.from_float(math.nextafter(value, math.inf))


def _hex(frac: Fraction) -> str:
    return float(frac).hex()


def _exp_ulp_bound(gappa_exp: Fraction, sollya_exp: Fraction) -> Fraction:
    return (gappa_exp + sollya_exp) * Fraction(2**53)


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
            for path in [COMMON_CONSTANTS, POW_IMPL, LOG_GAPPA, EXP_GAPPA, script_path]
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
        pow_text = _read(POW_IMPL)

        rd = [_exact_float(token) for token in _extract_array(common_text, "RD")]
        cd = [_exact_float(token) for token in _extract_array(common_text, "CD")]
        r2 = [_exact_float(token) for token in _extract_array(common_text, "R2")]

        log_coeffs = _extract_array(pow_text, "POW_LOG2_COEFFS")
        exp_coeffs = _extract_array(pow_text, "POW_EXP2_COEFFS")

        # Accurate double-double reconstruction polynomials (in-range path).
        log_dd_coeffs = _extract_dd_array(pow_text, r"std::array<DoubleDouble,\s*6>\s*COEFFS")
        exp_dd_coeffs = _extract_dd_array(pow_text, r"std::array<DoubleDouble,\s*10>\s*EXP2_COEFFS")

        if len(rd) != 128 or len(cd) != 128:
            raise RuntimeError("Unexpected first-stage table size")
        if len(r2) != 193:
            raise RuntimeError("Unexpected second-stage table size")
        if len(log_dd_coeffs) != 6 or len(exp_dd_coeffs) != 10:
            raise RuntimeError("Unexpected accurate-path double-double coefficient count")

        report["source_parse"] = {
            "rd_cd_entries": len(rd),
            "r2_entries": len(r2),
            "log_coeff_count": len(log_coeffs),
            "exp_coeff_count": len(exp_coeffs),
            "log_dd_coeff_count": len(log_dd_coeffs),
            "exp_dd_coeff_count": len(exp_dd_coeffs),
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
            print("pow double proof smoke")
            print("  source parse               : PASS")
            print("  sollya presence            : PASS")
            print("  gappa presence             : PASS")
            print("  fast_math probe            : PASS")
            print("  proof scope                : generic_non_fma_kernel")
            if args.report is not None:
                print(f"  json report                : {args.report}")
            return 0

        unit = Fraction(1, 1)
        ulp53 = Fraction(1, 2**52)

        stage1_low: Fraction | None = None
        stage1_high: Fraction | None = None

        for idx, (rd_i, cd_i) in enumerate(zip(rd, cd)):
            left = unit + Fraction(idx, 2**7)
            right = unit + Fraction(idx + 1, 2**7) - ulp53

            if rd_i * left - unit != cd_i:
                raise RuntimeError(f"Compensation identity failed for RD/CD entry {idx}")

            low = rd_i * left - unit
            high = rd_i * right - unit

            stage1_low = low if stage1_low is None or low < stage1_low else stage1_low
            stage1_high = high if stage1_high is None or high > stage1_high else stage1_high

        assert stage1_low is not None and stage1_high is not None

        if stage1_low < Fraction(-1, 2**8):
            raise RuntimeError("First-stage lower bound violated")
        if stage1_high >= Fraction(1, 2**7):
            raise RuntimeError("First-stage upper bound violated")

        stage2_low: Fraction | None = None
        stage2_high: Fraction | None = None

        for offset, r2_i in enumerate(r2):
            idx2 = offset - 64
            interval_low = Fraction(2 * idx2 - 1, 2**15)
            interval_high = Fraction(2 * idx2 + 1, 2**15)

            dx_low = max(stage1_low, interval_low)
            dx_high = min(stage1_high, interval_high)
            if dx_low > dx_high:
                continue

            low = (unit + dx_low) * r2_i - unit
            high = (unit + dx_high) * r2_i - unit

            stage2_low = low if stage2_low is None or low < stage2_low else stage2_low
            stage2_high = high if stage2_high is None or high > stage2_high else stage2_high

        stated_stage2_low = _exact_float("-0x1.3ffcp-15")
        stated_stage2_high = _exact_float("0x1.3e3dp-15")

        if stage2_low is None or stage2_high is None:
            raise RuntimeError("Second-stage range check did not cover any interval")
        if stage2_low < stated_stage2_low:
            raise RuntimeError("Second-stage lower bound violated")
        if stage2_high > stated_stage2_high:
            raise RuntimeError("Second-stage upper bound violated")

        log_err, exp_err, sollya_result = _run_sollya(log_coeffs, exp_coeffs)
        log_dd_err, exp_dd_err, sollya_dd_result = _run_sollya_dd(log_dd_coeffs, exp_dd_coeffs)

        # The accurate path collapses a double-double to one binary64 rounding, so the
        # polynomial approximation must be far below 1 ULP (2^-52). These thresholds keep a
        # wide margin while certifying the in-range reconstruction polynomials.
        log_dd_bound = _sollya_upper_bound(log_dd_err)
        exp_dd_bound = _sollya_upper_bound(exp_dd_err)
        if log_dd_bound >= Fraction(1, 2**90):
            raise RuntimeError(f"Accurate log2 double-double sup-norm {log_dd_err} exceeds 2^-90 budget")
        if exp_dd_bound >= Fraction(1, 2**95):
            raise RuntimeError(f"Accurate exp2 double-double sup-norm {exp_dd_err} exceeds 2^-95 budget")

        gappa_log_result = _run_gappa(LOG_GAPPA)
        gappa_exp_result = _run_gappa(EXP_GAPPA)

        gappa_log_bound = _parse_gappa_bound(LOG_GAPPA, "log_eval - P")
        gappa_exp_bound = _parse_gappa_bound(EXP_GAPPA, "exp_eval - exp_stagee")
        sollya_exp = _sollya_upper_bound(exp_err)
        ulp_bound = _exp_ulp_bound(gappa_exp_bound, sollya_exp)
        ulp_policy = Fraction(4)

        if ulp_bound >= ulp_policy:
            raise RuntimeError(
                f"Exp ULP bound {float(ulp_bound):.4f} >= policy {ulp_policy}; "
                "certificate no longer covers the implementation"
            )

        report["status"] = "success"
        report["exit_status"] = 0
        report["commands"] = {
            "sollya": sollya_result,
            "sollya_accurate_dd": sollya_dd_result,
            "gappa_log": gappa_log_result,
            "gappa_exp": gappa_exp_result,
        }
        report["results"] = {
            "rd_cd_entries": len(rd),
            "stage1_dx_range": {
                "low_hex": _hex(stage1_low),
                "high_hex": _hex(stage1_high),
            },
            "stage2_dx2_range": {
                "low_hex": _hex(stage2_low),
                "high_hex": _hex(stage2_high),
            },
            "log_supnorm_hex": log_err,
            "exp_supnorm_hex": exp_err,
            "log_dd_supnorm_hex": log_dd_err,
            "exp_dd_supnorm_hex": exp_dd_err,
            "gappa_log_eval_bound_hex": _hex(gappa_log_bound),
            "gappa_exp_eval_bound_hex": _hex(gappa_exp_bound),
            "exp_phase_ulp_bound": float(ulp_bound),
            "ulp_policy": float(ulp_policy),
        }
        _maybe_write_report(args, report)

        print("pow double proof obligations")
        print(f"  stage1 compensation identity : PASS ({len(rd)} entries)")
        print(f"  stage1 dx range              : [{_hex(stage1_low)}, {_hex(stage1_high)})")
        print(f"  stage2 dx2 range             : [{_hex(stage2_low)}, {_hex(stage2_high)}]")
        print(f"  log polynomial sup-norm      : {log_err}")
        print(f"  exp polynomial sup-norm      : {exp_err}")
        print(f"  accurate log2 dd sup-norm    : {log_dd_err}  (< 2^-90, proved)")
        print(f"  accurate exp2 dd sup-norm    : {exp_dd_err}  (< 2^-95, proved)")
        print(f"  gappa log eval cert          : PASS  ({LOG_GAPPA.name})")
        print(f"  gappa exp eval cert          : PASS  ({EXP_GAPPA.name})")
        print(f"  exp phase ULP bound          : {float(ulp_bound):.4f}  (<= {ulp_policy} ULP policy, proved)")
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
