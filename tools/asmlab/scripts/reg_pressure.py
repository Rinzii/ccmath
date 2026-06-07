#!/usr/bin/env python3
# Copyright (c) Ian Pike
# Copyright (c) CCMath contributors
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
"""Register pressure and spill/reload attribution heuristics."""

import re

import analysis_common as AC

GPR = re.compile(
    r"\b(r[a-z]x|r\d+d|r\d+w|r\d+b|eax|ebx|ecx|edx|esi|edi|ebp|esp|r8|r9|r10|r11|r12|r13|r14|r15)\b",
    re.I,
)
VEC = re.compile(r"\b(xmm\d+|ymm\d+|zmm\d+)\b", re.I)
SPILL_STORE = re.compile(r"\bmov[a-z]*\b.*\b\d*\(%rsp\)", re.I)
SPILL_LOAD = re.compile(r"\bmov[a-z]*\b.*\(%rsp\).*%,", re.I)


def _regs_in(text):
    g = set(m.group(0).lower() for m in GPR.finditer(text))
    v = set(m.group(0).lower() for m in VEC.finditer(text))
    return g, v


def build_reg_pressure(variant_dir, source_map, cfg=None):
    instructions = source_map.get("instructions", [])
    spills = []
    reloads = []
    live_gpr = set()
    live_vec = set()
    peak_gpr = 0
    peak_vec = 0
    pressure_regions = []

    for ent in instructions:
        asm = ent.get("assembly", "")
        mnem = AC.insn_mnemonic(asm)
        if mnem.endswith(":"):
            continue

        g, v = _regs_in(asm)
        live_gpr |= g
        live_vec |= v
        peak_gpr = max(peak_gpr, len(live_gpr))
        peak_vec = max(peak_vec, len(live_vec))

        is_spill = bool(SPILL_STORE.search(asm)) and "rsp" in asm.lower()
        is_reload = bool(SPILL_LOAD.search(asm)) and "rsp" in asm.lower()

        loc = ent.get("source", {})
        rec = {
            "ordinal": ent.get("ordinal"),
            "assembly": asm,
            "file": loc.get("file", ""),
            "line": loc.get("line", 0),
            "attribution_layer": AC.attribution_layer(loc.get("file", "")),
        }
        if is_spill:
            rec["kind"] = "spill"
            spills.append(rec)
        if is_reload:
            rec["kind"] = "reload"
            reloads.append(rec)

    if peak_gpr >= 12 or peak_vec >= 8:
        pressure_regions.append({
            "kind": "high_register_usage",
            "peak_gpr": peak_gpr,
            "peak_vec": peak_vec,
            "note": "Approximate live-set size from textual register mentions.",
        })

    block_pressure = []
    if cfg:
        for block in cfg.get("blocks", []):
            block_spills = [s for s in spills if block.get("start_ordinal") <= s["ordinal"] <= (block.get("end_ordinal") or 10**9)]
            block_pressure.append({
                "block_id": block["id"],
                "spill_count": len([x for x in block_spills if x["kind"] == "spill"]),
                "reload_count": len([x for x in block_spills if x["kind"] == "reload"]),
                "source_regions": block.get("source_regions", []),
            })

    result = {
        "analysis_version": AC.ANALYSIS_VERSION,
        "peak_gpr_mentions": peak_gpr,
        "peak_vec_mentions": peak_vec,
        "spill_count": len(spills),
        "reload_count": len(reloads),
        "spills": spills,
        "reloads": reloads,
        "pressure_regions": pressure_regions,
        "block_pressure": block_pressure,
        "notes": [
            "Live ranges are approximated by cumulative register mentions, not true liveness.",
            "Stack slot indices are not tracked, only rsp-relative spill/reload patterns.",
        ],
    }

    md = [
        "# Register pressure",
        "",
        "- peak GPR mentions: %d" % peak_gpr,
        "- peak vector mentions: %d" % peak_vec,
        "- spills: %d" % len(spills),
        "- reloads: %d" % len(reloads),
        "",
        "## Spill samples",
        "",
    ]
    for s in spills[:8]:
        md.append("- ord %s %s:%d %s" % (s["ordinal"], s["file"], s["line"], s["assembly"][:60]))
    md.append("")

    AC.write_artifact(variant_dir, "reg_pressure", result, md)
    return result
