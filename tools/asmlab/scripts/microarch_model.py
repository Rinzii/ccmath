#!/usr/bin/env python3
# Copyright (c) Ian Pike
# Copyright (c) CCMath contributors
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
"""Microarchitecture modeling from llvm-mca and instruction metadata."""

import analysis_common as AC

# Simplified port/latency hints for common x86 SIMD ops (approximation only).
_UOP_HINTS = {
    "vfmadd": {"ports": ["FMA"], "latency": 4, "throughput": 0.5},
    "vfma": {"ports": ["FMA"], "latency": 4, "throughput": 0.5},
    "vmul": {"ports": ["FMA", "MUL"], "latency": 3, "throughput": 0.5},
    "vadd": {"ports": ["VEC"], "latency": 1, "throughput": 0.5},
    "vmov": {"ports": ["LOAD", "STORE"], "latency": 1, "throughput": 0.5},
    "mov": {"ports": ["LOAD", "STORE"], "latency": 1, "throughput": 0.5},
    "call": {"ports": ["BRANCH"], "latency": 1, "throughput": 1.0},
    "jmp": {"ports": ["BRANCH"], "latency": 0, "throughput": 1.0},
}


def _classify_insn(asm):
    mnem = AC.insn_mnemonic(asm)
    for prefix, hint in _UOP_HINTS.items():
        if mnem.startswith(prefix):
            return {"mnemonic": mnem, **hint}
    return {"mnemonic": mnem, "ports": ["GENERIC"], "latency": 1, "throughput": 1.0}


def build_microarch(variant_dir, source_map, mca, cfg=None):
    instructions = source_map.get("instructions", [])
    classified = []
    port_pressure = {}
    fusion_candidates = 0
    dep_chain_score = 0

    prev_mnem = None
    for ent in instructions:
        asm = ent.get("assembly", "")
        if AC.insn_mnemonic(asm).endswith(":"):
            continue
        info = _classify_insn(asm)
        for p in info["ports"]:
            port_pressure[p] = port_pressure.get(p, 0) + 1
        if prev_mnem == "cmp" and info["mnemonic"].startswith("j"):
            fusion_candidates += 1
        if info["latency"] >= 3:
            dep_chain_score += 1
        classified.append({
            "ordinal": ent.get("ordinal"),
            "assembly": asm,
            "ports": info["ports"],
            "latency_hint": info["latency"],
            "source": ent.get("source", {}),
        })
        prev_mnem = info["mnemonic"]

    bottleneck = (mca or {}).get("bottleneck", {})
    primary = bottleneck.get("primary", "unknown")

    frontend = port_pressure.get("BRANCH", 0) + port_pressure.get("LOAD", 0) * 0.1
    backend = port_pressure.get("FMA", 0) + port_pressure.get("VEC", 0) + port_pressure.get("MUL", 0)
    scheduler = dep_chain_score

    if primary == "unknown":
        if backend > frontend * 2:
            primary = "backend_pressure"
        elif frontend > backend * 2:
            primary = "frontend_pressure"
        elif scheduler > len(instructions) * 0.3:
            primary = "dependency_bottleneck"

    result = {
        "analysis_version": AC.ANALYSIS_VERSION,
        "instruction_count": len(classified),
        "port_pressure": port_pressure,
        "fusion_candidates": fusion_candidates,
        "dependency_chain_ops": dep_chain_score,
        "bottleneck": {
            "primary": primary,
            "frontend_score": round(frontend, 2),
            "backend_score": round(backend, 2),
            "scheduler_score": scheduler,
        },
        "mca_correlation": bottleneck,
        "confidence": "low_to_medium",
        "notes": [
            "Port and latency tables are simplified heuristics, not uops.info dumps.",
            "Fusion detection is limited to cmp+j macro-op patterns.",
            "Use llvm-mca for scheduler-grade modeling when available.",
        ],
    }

    md = [
        "# Microarchitecture model",
        "",
        "- primary bottleneck: %s" % primary,
        "- fusion candidates: %d" % fusion_candidates,
        "- dependency-chain ops: %d" % dep_chain_score,
        "",
        "## Port pressure (heuristic)",
        "",
    ]
    for p, c in sorted(port_pressure.items(), key=lambda x: -x[1]):
        md.append("- %s: %d" % (p, c))
    md.append("")

    AC.write_artifact(variant_dir, "microarch", result, md)
    return result
