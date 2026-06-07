#!/usr/bin/env python3
# Copyright (c) Ian Pike
# Copyright (c) CCMath contributors
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
"""Vectorization width and missed-vectorize remarks from asm and opt records."""

import re

import analysis_common as AC

XMM = re.compile(r"\bxmm\d+\b", re.I)
YMM = re.compile(r"\bymm\d+\b", re.I)
ZMM = re.compile(r"\bzmm\d+\b", re.I)
MASK = re.compile(r"\b(k[0-7]|%k[0-7])\b", re.I)
SCALAR_FP = re.compile(r"\b(movsd|addsd|mulsd|subsd|divsd|sqrtsd)\b", re.I)


def build_vectorization(variant_dir, source_map, opt_remarks=None):
    instructions = source_map.get("instructions", [])
    vec_insns = []
    scalar_insns = []
    width = {"xmm": 0, "ymm": 0, "zmm": 0}
    masked = 0

    for ent in instructions:
        asm = ent.get("assembly", "")
        mnem = AC.insn_mnemonic(asm)
        if mnem.endswith(":"):
            continue
        loc = ent.get("source", {})
        rec = {
            "ordinal": ent.get("ordinal"),
            "assembly": asm,
            "file": loc.get("file", ""),
            "line": loc.get("line", 0),
        }
        if ZMM.search(asm):
            width["zmm"] += 1
            vec_insns.append(rec)
        elif YMM.search(asm):
            width["ymm"] += 1
            vec_insns.append(rec)
        elif XMM.search(asm):
            width["xmm"] += 1
            vec_insns.append(rec)
        elif SCALAR_FP.search(asm):
            scalar_insns.append(rec)
        if MASK.search(asm):
            masked += 1

    vectorized = width["xmm"] + width["ymm"] + width["zmm"] > 0
    dominant_width = max(width, key=width.get) if vectorized else "scalar"
    lane_util = "unknown"
    if vectorized and scalar_insns:
        ratio = len(vec_insns) / max(1, len(vec_insns) + len(scalar_insns))
        lane_util = "partial" if ratio < 0.8 else "mostly_vector"

    missed = []
    if opt_remarks:
        missed = opt_remarks.get("missed_vectorization", [])

    failures = []
    for m in missed:
        failures.append({
            "file": m.get("file", ""),
            "line": m.get("line", 0),
            "message": m.get("message") or m.get("name", ""),
        })

    scalar_fallback_regions = []
    if scalar_insns and vectorized:
        by_file = {}
        for s in scalar_insns:
            key = (s["file"], s["line"])
            by_file.setdefault(key, []).append(s)
        for (f, ln), items in sorted(by_file.items())[:10]:
            scalar_fallback_regions.append({
                "file": f,
                "line": ln,
                "scalar_insn_count": len(items),
            })

    result = {
        "analysis_version": AC.ANALYSIS_VERSION,
        "vectorized": vectorized,
        "dominant_width": dominant_width,
        "width_counts": width,
        "vector_insn_count": len(vec_insns),
        "scalar_fp_count": len(scalar_insns),
        "masked_ops": masked,
        "lane_utilization": lane_util,
        "missed_vectorization": failures,
        "scalar_fallback_regions": scalar_fallback_regions,
        "confidence": "high" if vectorized else ("medium" if missed else "low"),
        "notes": [
            "Width detection is register-class based, not loop-centric.",
            "Scalar fallback regions are grouped by source line correlation.",
        ],
    }

    md = [
        "# Vectorization analysis",
        "",
        "- vectorized: %s" % vectorized,
        "- dominant width: %s" % dominant_width,
        "- vector insns: %d" % len(vec_insns),
        "- scalar fp insns: %d" % len(scalar_insns),
        "- masked ops: %d" % masked,
        "",
    ]
    if failures:
        md.append("## Missed vectorization")
        for f in failures[:5]:
            md.append("- %s:%d %s" % (f["file"], f["line"], f["message"][:80]))
        md.append("")

    AC.write_artifact(variant_dir, "vectorization", result, md)
    return result
