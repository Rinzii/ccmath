#!/usr/bin/env python3
# Copyright (c) Ian Pike
# Copyright (c) CCMath contributors
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
"""Extended llvm-mca analysis with per-block and bottleneck classification."""

import json
import tempfile
from pathlib import Path

import _asmlab_common as C
import analysis_common as AC
import analyze as legacy_analyze


def _run_mca_on_text(asm_text, arch):
    with tempfile.NamedTemporaryFile(mode="w", suffix=".s", delete=False) as f:
        f.write(asm_text)
        tmp = f.name
    try:
        return legacy_analyze.run_llvm_mca(Path(tmp), arch), None
    finally:
        Path(tmp).unlink(missing_ok=True)


def _parse_mca_summary(mca_json):
    """Normalize llvm-mca JSON dict into summary fields."""
    if not mca_json or "error" in mca_json:
        return {"error": (mca_json or {}).get("error", "mca failed")}
    pressure = mca_json.get("port_pressure", {})
    top = sorted(pressure.items(), key=lambda kv: -kv[1])[:3]
    hints = ["%s=%.2f" % (k, v) for k, v in top]
    return {
        "block_rthroughput": mca_json.get("block_rthroughput"),
        "total_cycles": mca_json.get("static_model_estimate_cycles"),
        "ipc": mca_json.get("ipc"),
        "dispatch_width": mca_json.get("dispatch_width"),
        "instructions": mca_json.get("instructions"),
        "total_uops": mca_json.get("total_uops"),
        "port_pressure": pressure,
        "pressure_hints": hints,
    }


def _block_asm(block):
    lines = []
    for ent in block.get("instructions", []):
        asm = ent.get("assembly", "").strip()
        if asm and not asm.endswith(":"):
            lines.append("\t" + asm)
    return "\n".join(lines)


def build_mca(variant_dir, cfg, arch_name, compiler):
    variant_dir = Path(variant_dir)
    region = variant_dir / "region.s"
    if not region.exists():
        return None

    arch = C.resolve_arch(arch_name)
    llvm_mca = C.llvm_bin("llvm-mca")
    if not llvm_mca:
        data = {
            "analysis_version": AC.ANALYSIS_VERSION,
            "available": False,
            "reason": "llvm-mca not found on PATH",
            "confidence": "none",
        }
        AC.write_artifact(variant_dir, "mca", data, ["# MCA", "", "llvm-mca not found."])
        return data

    fn_mca, fn_err = _run_mca_on_text(region.read_text(), arch)
    function_level = _parse_mca_summary(fn_mca)

    blocks_mca = []
    for block in cfg.get("blocks", []):
        body = _block_asm(block)
        if not body.strip():
            continue
        wrapped = "# LLVM-MCA-BEGIN\n" + body + "\n# LLVM-MCA-END\n"
        bout, berr = _run_mca_on_text(wrapped, arch)
        bs = _parse_mca_summary(bout)
        bs["block_id"] = block["id"]
        bs["instruction_count"] = block.get("instruction_count", 0)
        bs["source_regions"] = block.get("source_regions", [])
        if berr:
            bs["error"] = berr[:500]
        blocks_mca.append(bs)

    bottleneck = _classify_bottleneck(function_level, blocks_mca)

    result = {
        "analysis_version": AC.ANALYSIS_VERSION,
        "available": True,
        "llvm_mca": llvm_mca,
        "mcpu": arch["mca_cpu"],
        "triple": arch["mca_triple"],
        "function": function_level,
        "blocks": blocks_mca,
        "bottleneck": bottleneck,
        "confidence": "medium",
        "notes": [
            "llvm-mca models static scheduling, not measured runtime.",
            "Per-block regions are isolated slices and may miss cross-block dependencies.",
            "Bottleneck classification is heuristic from pressure hints.",
        ],
    }

    md = [
        "# MCA analysis",
        "",
        "- llvm-mca: %s" % llvm_mca,
        "- mcpu: %s" % arch["mca_cpu"],
        "- bottleneck: %s" % bottleneck.get("primary", "unknown"),
        "",
        "## Function summary",
        "",
    ]
    for k in ("total_cycles", "ipc", "dispatch_width"):
        if k in function_level:
            md.append("- %s: %s" % (k, function_level[k]))
    md.append("")
    md.append("## Per-block")
    for b in blocks_mca:
        md.append("- %s: %d insns, cycles=%s" % (
            b["block_id"], b["instruction_count"], b.get("total_cycles")))
    md.append("")
    md.append("## Limitations")
    for n in result["notes"]:
        md.append("- %s" % n)

    AC.write_artifact(variant_dir, "mca", result, md)
    return result


def _classify_bottleneck(function_level, blocks_mca):
    hints = function_level.get("pressure_hints", []) or []
    text = " ".join(hints).lower()
    primary = "unknown"
    if "retire" in text:
        primary = "retire_pressure"
    elif "scheduler" in text or "dispatch" in text:
        primary = "scheduler_pressure"
    elif "resource" in text:
        primary = "resource_pressure"
    elif function_level.get("ipc") and function_level["ipc"] < 0.5:
        primary = "dependency_chain"

    return {
        "primary": primary,
        "evidence": hints[:10],
    }
