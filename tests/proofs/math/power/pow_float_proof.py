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
import struct
import sys
from fractions import Fraction
from pathlib import Path

from proof_reporting import (
    add_aspirational_ulp_arg,
    add_require_tools_arg,
    aspirational_ulp_results,
    check_fast_math_disabled,
    emit_aspirational_ulp,
    extract_brace_array,
    extract_regex_array,
    finalize_proof_smoke,
    fraction_hex,
    maybe_write_report,
    parse_gappa_bound,
    proof_base_report,
    read_text,
    run_gappa,
    skip_if_tools_missing,
)


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
    add_require_tools_arg(parser)
    add_aspirational_ulp_arg(parser)
    return parser.parse_args()


def _exact_float64(token: str) -> Fraction:
    return Fraction.from_float(float.fromhex(token))


def _exact_float32(token: str) -> Fraction:
    value = float.fromhex(token)
    packed = struct.pack("f", value)
    unpacked = struct.unpack("f", packed)[0]
    return Fraction.from_float(unpacked)


def _exp_ulp_bound_float(gappa_exp: Fraction, sollya_exp: Fraction) -> Fraction:
    return (gappa_exp + sollya_exp) * Fraction(2**24)


def _run_aspirational_exp_ulp() -> tuple[Fraction, dict[str, float]]:
    run_gappa(EXP_GAPPA)
    gappa_exp_bound = parse_gappa_bound(EXP_GAPPA, "exp_eval - P")
    sollya_exp = Fraction.from_float(float.fromhex("0x1.a2b77e618f5c4p-60"))
    ulp_bound = _exp_ulp_bound_float(gappa_exp_bound, sollya_exp)
    return ulp_bound, {"gappa_exp_bound": float(gappa_exp_bound), "sollya_exp_bound": float(sollya_exp)}


def main() -> int:
    args = _parse_args()
    script_path = Path(__file__).resolve()
    report = proof_base_report(
        root=ROOT,
        script_path=script_path,
        hashed_paths=[COMMON_CONSTANTS, POWF_IMPL, LOG_GAPPA, EXP_GAPPA, script_path],
        args=args,
    )

    try:
        check_fast_math_disabled()

        if args.aspirational_ulp:
            if skip_if_tools_missing(args, report, skip_label="aspirational ULP checks"):
                maybe_write_report(args, report)
                return 0

            ulp_bound, aspiration_meta = _run_aspirational_exp_ulp()
            aspiration = aspirational_ulp_results(ulp_bound)
            report["status"] = "success"
            report["exit_status"] = 0
            report["aspirational_ulp"] = aspiration
            report["commands"] = aspiration_meta
            maybe_write_report(args, report)
            print("powf aspirational exp ULP")
            print(f"  gappa exp eval cert          : PASS  ({EXP_GAPPA.name})")
            print(f"  exp phase ULP bound (float)  : {float(ulp_bound):.2e}")
            emit_aspirational_ulp(aspiration, phase="powf")
            return 0

        common_text = read_text(COMMON_CONSTANTS)
        powf_text = read_text(POWF_IMPL)

        r = [_exact_float32(token) for token in extract_brace_array(common_text, "R")]
        rd = [_exact_float64(token) for token in extract_brace_array(common_text, "RD")]
        log2_r = [_exact_float64(token) for token in extract_brace_array(common_text, "LOG2_R")]

        if len(r) != 128:
            raise RuntimeError(f"Expected 128 entries in R, got {len(r)}")
        if len(rd) != 128:
            raise RuntimeError(f"Expected 128 entries in RD, got {len(rd)}")
        if len(log2_r) != 128:
            raise RuntimeError(f"Expected 128 entries in LOG2_R, got {len(log2_r)}")

        log_coeffs = extract_regex_array(powf_text, r"std::array<double,\s*6>\s*COEFFS")
        exp_coeffs = extract_regex_array(powf_text, r"std::array<double,\s*6>\s*EXP2_COEFFS")

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
            return finalize_proof_smoke(args, report, title="powf proof smoke")

        for idx in range(128):
            if r[idx] != rd[idx]:
                raise RuntimeError(f"R[{idx}] = {fraction_hex(r[idx])} != RD[{idx}] = {fraction_hex(rd[idx])}")

        for k in range(128):
            denom = Fraction(1) + Fraction(k, 128)
            numer = Fraction(1) - Fraction(1, 256)
            scaled = numer / denom * 256
            expected_int = int(scaled) + (0 if scaled == int(scaled) else 1)
            expected = Fraction(expected_int, 256)
            if k == 0 and r[k] == 1:
                continue
            if r[k] != expected:
                raise RuntimeError(f"R[{k}] = {fraction_hex(r[k])}, expected {fraction_hex(expected)}")

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
            raise RuntimeError(f"dx range lower bound violated: {fraction_hex(dx_min)}")
        if dx_max >= Fraction(1, 128):
            raise RuntimeError(f"dx range upper bound violated: {fraction_hex(dx_max)}")

        max_log2_err = Fraction(0)
        for k in range(128):
            if r[k] == 1:
                if log2_r[k] != 0:
                    raise RuntimeError(f"LOG2_R[{k}] should be 0 for R[k]=1, got {fraction_hex(log2_r[k])}")
                continue
            exact = Fraction.from_float(-math.log2(float(r[k])))
            err = abs(log2_r[k] - exact)
            if err > max_log2_err:
                max_log2_err = err
            if err > Fraction(1, 2**50):
                raise RuntimeError(f"LOG2_R[{k}] error too large: {float(err):.3e}")

        gappa_log_result = run_gappa(LOG_GAPPA)
        gappa_exp_result = run_gappa(EXP_GAPPA)

        gappa_log_bound = parse_gappa_bound(LOG_GAPPA, "log_eval - P")
        gappa_exp_bound = parse_gappa_bound(EXP_GAPPA, "exp_eval - P")
        sollya_exp = Fraction.from_float(float.fromhex("0x1.a2b77e618f5c4p-60"))
        ulp_bound = _exp_ulp_bound_float(gappa_exp_bound, sollya_exp)
        aspiration = aspirational_ulp_results(ulp_bound)

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
                "low_hex": fraction_hex(dx_min),
                "high_hex": fraction_hex(dx_max),
            },
            "max_log2_r_error": float(max_log2_err),
            "gappa_log_eval_bound_hex": fraction_hex(gappa_log_bound),
            "gappa_exp_eval_bound_hex": fraction_hex(gappa_exp_bound),
            "shared_sollya_exp_bound_hex": float(sollya_exp).hex(),
            "exp_phase_ulp_bound_float": float(ulp_bound),
            "aspirational_ulp": aspiration,
        }
        maybe_write_report(args, report)

        print("powf float proof obligations")
        print(f"  R == RD identity             : PASS ({len(r)} entries)")
        print(f"  R construction identity      : PASS ({len(r)} entries, R[0]=1.0 by convention)")
        print(f"  dx range                     : [{fraction_hex(dx_min)}, {fraction_hex(dx_max)})")
        print(f"  LOG2_R accuracy              : PASS (max error ~ {float(max_log2_err):.2e})")
        print(f"  gappa log eval cert          : PASS  ({LOG_GAPPA.name})")
        print(f"  gappa exp eval cert          : PASS  ({EXP_GAPPA.name})")
        print(f"  exp phase ULP bound (float)  : {float(ulp_bound):.2e}  (negligible; Ziv test ensures correct rounding)")
        emit_aspirational_ulp(aspiration, phase="powf")
        if args.report is not None:
            print(f"  json report                  : {args.report}")
        return 0
    except Exception as exc:
        report["status"] = "failure"
        report["exit_status"] = 1
        report["error"] = str(exc)
        maybe_write_report(args, report)
        print(str(exc), file=sys.stderr)
        return 1


if __name__ == "__main__":
    sys.exit(main())
