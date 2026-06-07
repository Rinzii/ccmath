#!/usr/bin/env python3
# Copyright (c) Ian Pike
# Copyright (c) CCMath contributors
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
"""Diff deep analyze artifacts between baseline and current runs."""

import json
from pathlib import Path

import analysis_common as AC


def _delta(a, b, key):
    va = (a or {}).get(key)
    vb = (b or {}).get(key)
    if va is None or vb is None:
        return None
    return vb - va if isinstance(va, (int, float)) else None


def compare_variants(current_dir, baseline_dir):
    current_dir = Path(current_dir)
    baseline_dir = Path(baseline_dir)

    def load(name):
        return AC.load_json(current_dir / ("%s.json" % name)), AC.load_json(baseline_dir / ("%s.json" % name))

    cur_mca, base_mca = load("mca")
    cur_cfg, base_cfg = load("cfg")
    cur_rp, base_rp = load("reg_pressure")
    cur_vec, base_vec = load("vectorization")
    cur_sem, base_sem = load("semantic")

    mca_diff = {
        "ipc_delta": _delta((base_mca or {}).get("function", {}), (cur_mca or {}).get("function", {}), "ipc"),
        "throughput_delta": _delta((base_mca or {}).get("function", {}), (cur_mca or {}).get("function", {}), "block_rthroughput"),
        "bottleneck_from": (base_mca or {}).get("bottleneck", {}).get("primary"),
        "bottleneck_to": (cur_mca or {}).get("bottleneck", {}).get("primary"),
    }

    cfg_diff = {
        "block_count_delta": _delta(base_cfg, cur_cfg, "block_count"),
        "loop_count_delta": _delta(base_cfg, cur_cfg, "loop_count"),
    }

    spill_diff = {
        "spill_delta": _delta(base_rp, cur_rp, "spill_count"),
        "reload_delta": _delta(base_rp, cur_rp, "reload_count"),
    }

    vec_diff = {
        "vectorized_from": (base_vec or {}).get("vectorized"),
        "vectorized_to": (cur_vec or {}).get("vectorized"),
        "width_from": (base_vec or {}).get("dominant_width"),
        "width_to": (cur_vec or {}).get("dominant_width"),
    }

    new_semantic = []
    base_kinds = {c["kind"] for c in (base_sem or {}).get("classifications", [])}
    for c in (cur_sem or {}).get("classifications", []):
        if c["kind"] not in base_kinds:
            new_semantic.append(c)

    block_mca = []
    cur_blocks = {(b.get("block_id")): b for b in (cur_mca or {}).get("blocks", [])}
    base_blocks = {(b.get("block_id")): b for b in (base_mca or {}).get("blocks", [])}
    for bid in sorted(set(cur_blocks) | set(base_blocks)):
        cb, bb = cur_blocks.get(bid), base_blocks.get(bid)
        block_mca.append({
            "block_id": bid,
            "ipc_delta": _delta(bb, cb, "ipc"),
            "instruction_delta": _delta(bb, cb, "instructions"),
        })

    result = {
        "analysis_version": AC.ANALYSIS_VERSION,
        "current": str(current_dir),
        "baseline": str(baseline_dir),
        "mca": mca_diff,
        "cfg": cfg_diff,
        "register_pressure": spill_diff,
        "vectorization": vec_diff,
        "new_semantic_classifications": new_semantic,
        "per_block_mca": block_mca,
        "confidence": "medium",
        "notes": [
            "Block IDs may shift when CFG recovery changes between builds.",
        ],
    }

    md = [
        "# Deep analysis diff",
        "",
        "## MCA",
        "- IPC delta: %s" % mca_diff.get("ipc_delta"),
        "- bottleneck: %s -> %s" % (mca_diff.get("bottleneck_from"), mca_diff.get("bottleneck_to")),
        "",
        "## Register pressure",
        "- spill delta: %s" % spill_diff.get("spill_delta"),
        "",
        "## Vectorization",
        "- %s -> %s (%s -> %s)" % (
            vec_diff.get("vectorized_from"), vec_diff.get("vectorized_to"),
            vec_diff.get("width_from"), vec_diff.get("width_to")),
        "",
    ]
    if new_semantic:
        md.append("## New semantic classifications")
        for c in new_semantic:
            md.append("- %s" % c["kind"])
        md.append("")

    out = current_dir / "analysis_diff.json"
    out.write_text(json.dumps(result, indent=2) + "\n")
    (current_dir / "analysis_diff.md").write_text("\n".join(md) + "\n")
    return result
