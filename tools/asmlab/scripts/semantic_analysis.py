#!/usr/bin/env python3
# Copyright (c) Ian Pike
# Copyright (c) CCMath contributors
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
"""Compiler remark and asm pattern tags for deep analyze output."""

import re

import analysis_common as AC

FMA = re.compile(r"\b(vfmadd|vfma|fmadd)\b", re.I)
SCALAR_MULADD = re.compile(r"\b(mulsd|mulss).*\badd(sd|ss)\b", re.I)
REDUNDANT_LOAD = re.compile(r"\bmov[a-z]*\b.*\(%", re.I)
BRANCHLESS = re.compile(r"\b(cmov|blendv|vpblend|vblend)\b", re.I)


def _tag_instruction(ent):
    asm = ent.get("assembly", "")
    mnem = AC.insn_mnemonic(asm)
    tags = []
    if FMA.search(asm):
        tags.append("fma_present")
    if SCALAR_MULADD.search(asm):
        tags.append("mul_add_pair")
    if BRANCHLESS.search(asm):
        tags.append("branchless_select")
    if "call" in mnem:
        tags.append("call_site")
    loc = ent.get("source", {})
    return {
        "ordinal": ent.get("ordinal"),
        "tags": tags,
        "file": loc.get("file", ""),
        "line": loc.get("line", 0),
        "assembly": asm,
    }


def build_semantic(variant_dir, source_map, opt_remarks=None, reg_pressure=None,
                   vectorization=None, baseline_semantic=None):
    instructions = source_map.get("instructions", [])
    tagged = [_tag_instruction(e) for e in instructions if not AC.insn_mnemonic(e.get("assembly", "")).endswith(":")]

    classifications = []

    fma_count = sum(1 for t in tagged if "fma_present" in t["tags"])
    muladd_count = sum(1 for t in tagged if "mul_add_pair" in t["tags"])
    if muladd_count > fma_count:
        classifications.append({
            "kind": "missed_fma_contraction",
            "evidence": "mul+add pairs without vfmadd",
            "count": muladd_count - fma_count,
            "source_hint": "polynomial evaluation or dot-like accumulation",
        })

    if opt_remarks:
        for r in opt_remarks.get("missed_vectorization", [])[:3]:
            classifications.append({
                "kind": "missed_vectorization",
                "file": r.get("file", ""),
                "line": r.get("line", 0),
                "message": r.get("message") or r.get("name", ""),
            })
        for r in opt_remarks.get("inline_decisions", []):
            if r.get("kind") == "Missed":
                classifications.append({
                    "kind": "missed_inlining",
                    "file": r.get("file", ""),
                    "line": r.get("line", 0),
                    "message": r.get("message", ""),
                })

    if reg_pressure and reg_pressure.get("spill_count", 0) > 0:
        classifications.append({
            "kind": "register_spill",
            "spill_count": reg_pressure["spill_count"],
            "reload_count": reg_pressure["reload_count"],
            "source_hint": "live range growth in hot kernel",
        })

    if vectorization and vectorization.get("scalar_fallback_regions"):
        for reg in vectorization["scalar_fallback_regions"][:3]:
            classifications.append({
                "kind": "scalar_fallback",
                "file": reg["file"],
                "line": reg["line"],
            })

    regressions = []
    if baseline_semantic:
        base_kinds = {c["kind"] for c in baseline_semantic.get("classifications", [])}
        for c in classifications:
            if c["kind"] not in base_kinds and c["kind"] in (
                    "missed_fma_contraction", "register_spill", "missed_vectorization", "scalar_fallback"):
                regressions.append({**c, "delta": "new_in_current"})

    result = {
        "analysis_version": AC.ANALYSIS_VERSION,
        "classifications": classifications,
        "regressions_vs_baseline": regressions,
        "tagged_instruction_count": len(tagged),
        "notes": [
            "Tags combine asm patterns, remarks, and spill heuristics.",
            "Confirm with bench before treating a tag as a perf root cause.",
        ],
    }

    md = [
        "# Asm pattern tags",
        "",
        "## Tags",
        "",
    ]
    for c in classifications:
        md.append("- %s" % c["kind"])
        if c.get("file"):
            md.append("  - %s:%d" % (c["file"], c["line"]))
    if regressions:
        md.append("")
        md.append("## New vs baseline")
        for r in regressions:
            md.append("- %s" % r["kind"])
    md.append("")

    AC.write_artifact(variant_dir, "semantic", result, md)
    return result
