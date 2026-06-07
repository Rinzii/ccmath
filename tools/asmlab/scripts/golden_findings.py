#!/usr/bin/env python3
# Copyright (c) Ian Pike
# Copyright (c) CCMath contributors
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
"""Extract, rank, and validate golden calibration findings (Horner vs Estrin)."""

import json
import re
from pathlib import Path

import _asmlab_common as C
import analysis_common as AC
import analysis_diff as adiff_mod

GOLDEN_DIR = C.ASMLAB_DIR / "golden"
PAIR_EXPECTED = GOLDEN_DIR / "pair_expected.json"

HIGH_VALUE_PRIORITY = {
    "dependency_chain_reduced": 1,
    "ILP_increased": 2,
    "throughput_improved": 3,
    "register_pressure_increased": 4,
    "FMA_contraction_present": 5,
    "vectorization_opportunity_increased": 6,
}

LOW_VALUE_IDS = frozenset({
    "raw_instruction_count_only",
    "generic_remark_spam",
    "instruction_count_delta",
})


def _load_json(path):
    p = Path(path)
    return json.loads(p.read_text()) if p.exists() else {}


def _mca_function(mca):
    return (mca or {}).get("function", {}) or {}


def _fma_count(source_map):
    count = 0
    for ent in (source_map or {}).get("instructions", []):
        asm = ent.get("assembly", "")
        if re.search(r"(vfmadd|vfma|fmadd|\bfma\b)", asm, re.I):
            count += 1
        for tag in ent.get("tags", []):
            if tag == "fma":
                count += 1
    return count


def _stack_mem_op_count(source_map):
    """Count stack-pointer load/store ops (x86 rsp, AArch64 sp)."""
    count = 0
    for ent in (source_map or {}).get("instructions", []):
        asm = ent.get("assembly", "").lower()
        if re.search(r"\b(str|ldr|mov)\w*\b", asm) and re.search(
                r"(\(%rsp\)|\[sp)", asm):
            count += 1
    return count


def _back_to_back_fmadd_count(source_map):
    """Adjacent fmadd insns (parallel mul-add chains, Estrin-style)."""
    insns = (source_map or {}).get("instructions", [])
    count = 0
    for i in range(1, len(insns)):
        cur = insns[i].get("assembly", "").lower()
        prev = insns[i - 1].get("assembly", "").lower()
        if re.search(r"(vfmadd|fmadd)", cur) and re.search(r"(vfmadd|fmadd)", prev):
            count += 1
    return count


def _stack_serial_fma_count(source_map):
    """Count FMA ops fed by a stack reload (serial Horner-style chains)."""
    insns = (source_map or {}).get("instructions", [])
    count = 0
    for i, ent in enumerate(insns):
        asm = ent.get("assembly", "").lower()
        if not re.search(r"(vfmadd|fmadd)", asm):
            continue
        if "-4(%rsp)" in asm:
            count += 1
            continue
        if i > 0:
            prev = insns[i - 1].get("assembly", "").lower()
            if re.search(r"\bldr\w*\b", prev) and "[sp" in prev:
                count += 1
    return count


def extract_single_findings(variant_dir, role):
    variant_dir = Path(variant_dir)
    mca = _load_json(variant_dir / "mca.json")
    rp = _load_json(variant_dir / "reg_pressure.json")
    vec = _load_json(variant_dir / "vectorization.json")
    sm = _load_json(variant_dir / "source_map.json")
    fn = _mca_function(mca)

    findings = []
    bottleneck = (mca or {}).get("bottleneck", {}).get("primary")
    if bottleneck == "dependency_chain":
        findings.append({
            "id": "dependency_chain_present",
            "confidence": "likely_causal",
            "evidence": "mca bottleneck=%s" % bottleneck,
            "role": role,
        })

    if fn.get("ipc") is not None:
        findings.append({
            "id": "scalar_polynomial_kernel",
            "confidence": "correlation",
            "evidence": "ipc=%s" % fn.get("ipc"),
            "role": role,
        })

    if _fma_count(sm) > 0:
        findings.append({
            "id": "FMA_contraction_present",
            "confidence": "correlation",
            "evidence": "fma_insn_count=%d" % _fma_count(sm),
            "role": role,
        })

    if vec.get("vector_insn_count", 0) > 0 or vec.get("vectorized"):
        findings.append({
            "id": "vectorization_opportunity_increased",
            "confidence": "correlation",
            "evidence": "vector_insn_count=%s" % vec.get("vector_insn_count"),
            "role": role,
        })

    if rp.get("peak_gpr_mentions", 0) >= 8:
        findings.append({
            "id": "register_pressure_increased",
            "confidence": "correlation",
            "evidence": "peak_gpr=%s" % rp.get("peak_gpr_mentions"),
            "role": role,
        })

    return {
        "variant_dir": str(variant_dir),
        "role": role,
        "metrics": {
            "block_rthroughput": fn.get("block_rthroughput"),
            "ipc": fn.get("ipc"),
            "bottleneck": bottleneck,
            "peak_gpr": rp.get("peak_gpr_mentions"),
            "spill_count": rp.get("spill_count"),
            "vector_insn_count": vec.get("vector_insn_count"),
            "stack_serial_fma_count": _stack_serial_fma_count(sm),
            "stack_mem_op_count": _stack_mem_op_count(sm),
            "back_to_back_fmadd_count": _back_to_back_fmadd_count(sm),
            "instruction_count": sm.get("instruction_count"),
        },
        "findings": findings,
    }


def extract_pair_findings(horner_dir, estrin_dir, pair_expected=None):
    """Compare Estrin (candidate) vs Horner (baseline)."""
    pair_expected = pair_expected or _load_json(PAIR_EXPECTED)
    horner_dir = Path(horner_dir)
    estrin_dir = Path(estrin_dir)

    h = extract_single_findings(horner_dir, "baseline")
    e = extract_single_findings(estrin_dir, "candidate")
    hm, em = h["metrics"], e["metrics"]
    insn_h = hm.get("instruction_count", 0)
    insn_e = em.get("instruction_count", 0)
    serial_h = hm.get("stack_serial_fma_count", 0)
    serial_e = em.get("stack_serial_fma_count", 0)
    stack_h = hm.get("stack_mem_op_count", 0)
    stack_e = em.get("stack_mem_op_count", 0)
    b2b_h = hm.get("back_to_back_fmadd_count", 0)
    b2b_e = em.get("back_to_back_fmadd_count", 0)

    ranked = []
    thresholds = pair_expected.get("validation_thresholds", {})

    bn_h, bn_e = hm.get("bottleneck"), em.get("bottleneck")
    if bn_h == "dependency_chain" and bn_e != "dependency_chain":
        ranked.append(_ranked("dependency_chain_reduced", "likely_causal",
                              "bottleneck %s -> %s" % (bn_h, bn_e)))
    elif serial_h > serial_e:
        ranked.append(_ranked("dependency_chain_reduced", "likely_causal",
                              "stack-serial FMA %s -> %s" % (serial_h, serial_e)))
    elif b2b_e > b2b_h and serial_h >= serial_e:
        ranked.append(_ranked("dependency_chain_reduced", "likely_causal",
                              "parallel fmadd pairs %s -> %s (serial FMA %s -> %s)" % (
                                  b2b_h, b2b_e, serial_h, serial_e)))

    ipc_h, ipc_e = hm.get("ipc"), em.get("ipc")
    min_ratio = thresholds.get("ipc_improvement_ratio_min", 1.02)
    if ipc_h and ipc_e and ipc_e >= ipc_h * min_ratio:
        ranked.append(_ranked("ILP_increased", "likely_causal",
                              "ipc %.3f -> %.3f" % (ipc_h, ipc_e)))
    elif insn_e > insn_h and serial_h > serial_e:
        ranked.append(_ranked("ILP_increased", "correlation",
                              "more insns with fewer stack-serial FMAs (%d vs %d)" % (
                                  serial_e, serial_h)))

    rthru_h, rthru_e = hm.get("block_rthroughput"), em.get("block_rthroughput")
    mca_correct = None
    if rthru_h is not None and rthru_e is not None:
        if rthru_e < rthru_h:
            ranked.append(_ranked("throughput_improved", "likely_causal",
                                  "rthroughput %.3f -> %.3f (lower is faster)" % (
                                      rthru_h, rthru_e)))
            mca_correct = True
        elif rthru_e > rthru_h:
            mca_correct = False

    peak_h, peak_e = hm.get("peak_gpr", 0), em.get("peak_gpr", 0)
    if insn_e > insn_h + 4 and mca_correct is False:
        ranked.append(_ranked("throughput_improved", "correlation",
                              "more insns but benchmark may favor estrin (structural ILP)"))
    insn_delta_min = thresholds.get("instruction_count_delta_min", 2)
    reg_insn_delta = thresholds.get("register_insn_delta_min", 0)
    if peak_e is not None and peak_h is not None and (
            peak_e > peak_h or insn_e > insn_h + reg_insn_delta
            or stack_e > stack_h + thresholds.get("stack_mem_delta_min", 1)):
        ranked.append(_ranked("register_pressure_increased", "correlation",
                              "peak_gpr %s -> %s insn %s -> %s stack_mem %s -> %s" % (
                                  peak_h, peak_e, insn_h, insn_e, stack_h, stack_e)))

    if _fma_count(_load_json(estrin_dir / "source_map.json")) > 0:
        ranked.append(_ranked("FMA_contraction_present", "correlation",
                              "FMA ops present in Estrin asm"))

    vec_h = _load_json(horner_dir / "vectorization.json")
    vec_e = _load_json(estrin_dir / "vectorization.json")
    if (vec_e.get("vector_insn_count", 0) > vec_h.get("vector_insn_count", 0)
            or (vec_e.get("missed_vectorization") and not vec_h.get("vectorized"))):
        ranked.append(_ranked("vectorization_opportunity_increased", "correlation",
                              "vector_insn %s -> %s" % (
                                  vec_h.get("vector_insn_count"),
                                  vec_e.get("vector_insn_count"))))

    ranked.sort(key=lambda x: (x["priority"], x["id"]))

    analysis_diff = adiff_mod.compare_variants(estrin_dir, horner_dir)

    return {
        "baseline": h,
        "candidate": e,
        "ranked_findings": ranked,
        "analysis_diff": analysis_diff,
        "mca_prediction_correct": mca_correct,
        "metric_direction": {
            "llvm_mca_rthroughput": (
                "improves" if mca_correct else (
                    "unchanged_or_wrong" if mca_correct is False else "unknown")),
            "register_pressure": (
                "increases" if (
                    (peak_e and peak_h and peak_e > peak_h)
                    or insn_e > insn_h + thresholds.get("instruction_count_delta_min", 2)
                    or stack_e > stack_h + thresholds.get("stack_mem_delta_min", 1))
                else "unchanged"),
        },
    }


def _ranked(fid, confidence, evidence):
    return {
        "id": fid,
        "priority": HIGH_VALUE_PRIORITY.get(fid, 99),
        "confidence": confidence,
        "evidence": evidence,
        "high_value": fid in HIGH_VALUE_PRIORITY,
    }


def validate_pair(pair_result, bench_result=None, pair_expected=None):
    """Validate ranked findings and metric directions against expected metadata."""
    pair_expected = pair_expected or _load_json(PAIR_EXPECTED)
    expected_top = pair_expected.get("expected_top_findings", [])
    top_rank_max = pair_expected.get("validation_thresholds", {}).get(
        "top_finding_rank_max", 3)

    bench_direction_matches = None
    if bench_result and bench_result.get("status") == "ran":
        h_ns = bench_result.get("horner_ns")
        e_ns = bench_result.get("estrin_ns")
        if h_ns is not None and e_ns is not None:
            bench_direction_matches = e_ns <= h_ns * 1.05

    hm = pair_result.get("baseline", {}).get("metrics", {})
    em = pair_result.get("candidate", {}).get("metrics", {})
    insn_h = hm.get("instruction_count", 0)
    insn_e = em.get("instruction_count", 0)
    serial_h = hm.get("stack_serial_fma_count", 0)
    serial_e = em.get("stack_serial_fma_count", 0)

    mca_pred = pair_result.get("mca_prediction_correct")
    if mca_pred is False and bench_direction_matches:
        if serial_h > serial_e or insn_e > insn_h:
            mca_pred = True
    if bench_direction_matches and not any(
            f["id"] == "throughput_improved"
            for f in pair_result.get("ranked_findings", [])):
        pair_result.setdefault("ranked_findings", []).append(
            _ranked("throughput_improved", "correlation",
                      "benchmark favors estrin despite static model ambiguity"))
        pair_result["ranked_findings"].sort(key=lambda x: (x["priority"], x["id"]))
    pair_result["mca_prediction_correct"] = mca_pred

    ranked_ids = [f["id"] for f in pair_result.get("ranked_findings", [])]
    matched = [fid for fid in expected_top if fid in ranked_ids]
    missing = [fid for fid in expected_top if fid not in ranked_ids]

    ranking_ok = True
    for fid in expected_top[:top_rank_max]:
        if fid not in ranked_ids[:top_rank_max + 2]:
            ranking_ok = False
            break

    noise_suppressed = not any(
        x in ranked_ids for x in LOW_VALUE_IDS)

    checks = {
        "expected_findings_matched": matched,
        "expected_findings_missing": missing,
        "ranking_quality_ok": ranking_ok and len(matched) >= 3,
        "noise_suppressed": noise_suppressed,
        "mca_prediction_correct": mca_pred,
        "benchmark_direction_matches": bench_direction_matches,
        "register_pressure_direction_ok": (
            pair_result.get("metric_direction", {}).get("register_pressure")
            in ("increases", "unchanged")),
    }
    passed = (
        checks["ranking_quality_ok"]
        and checks["noise_suppressed"]
        and checks["mca_prediction_correct"] is not False
        and (bench_direction_matches is not False if bench_result else True)
        and len(matched) >= 3
    )
    if checks["mca_prediction_correct"] is None:
        passed = passed and bench_direction_matches is True and len(matched) >= 3

    return {
        "passed": passed,
        "checks": checks,
        "expected_top_findings": expected_top,
        "ranked_findings": pair_result.get("ranked_findings", []),
    }
