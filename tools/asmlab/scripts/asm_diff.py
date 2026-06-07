#!/usr/bin/env python3
# Copyright (c) Ian Pike
# Copyright (c) CCMath contributors
#
# CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
# See LICENSE for more information.
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
"""Source-aware assembly diff between baseline and current source maps."""

import json
from pathlib import Path

import _asmlab_common as C


def _line_key(entry):
    return (entry.get("file", ""), entry.get("line", 0))


def _insn_set(sm):
    return set(e.get("assembly", "") for e in sm.get("instructions", []))


def _summarize_tags(sm):
    counts = {}
    for e in sm.get("instructions", []):
        for t in e.get("tags", []):
            counts[t] = counts.get(t, 0) + 1
    return counts


def _line_tag_counts(sm, file, line):
    counts = {}
    for e in sm.get("instructions", []):
        loc = e.get("source", {})
        if loc.get("file") == file and loc.get("line") == line:
            for t in e.get("tags", []):
                counts[t] = counts.get(t, 0) + 1
    return counts


def _likely_effect(before_tags, after_tags, before_cnt, after_cnt):
    effects = []
    if before_tags.get("spill", 0) > after_tags.get("spill", 0):
        effects.append("fewer stack spills")
    if after_tags.get("spill", 0) > before_tags.get("spill", 0):
        effects.append("more stack spills")
    if before_tags.get("fma", 0) < after_tags.get("fma", 0):
        effects.append("more FMA instructions (possible contraction)")
    if before_tags.get("external_call", 0) < after_tags.get("external_call", 0):
        effects.append("new external call (possible libm fallback)")
    if before_tags.get("external_call", 0) > after_tags.get("external_call", 0):
        effects.append("removed external call")
    if before_cnt > after_cnt:
        effects.append("fewer instructions at this source line")
    if after_cnt > before_cnt:
        effects.append("more instructions at this source line")
    return effects


def diff_source_maps(before_sm, after_sm, metrics_before=None, metrics_after=None):
    """Compare two source_map.json payloads. Returns diff dict."""
    b_insns = _insn_set(before_sm)
    a_insns = _insn_set(after_sm)
    added = sorted(a_insns - b_insns)
    removed = sorted(b_insns - a_insns)

    b_lines = {_line_key(e): e for e in before_sm.get("source_lines", [])}
    a_lines = {_line_key(e): e for e in after_sm.get("source_lines", [])}
    all_keys = set(b_lines) | set(a_lines)

    changed_lines = []
    for key in sorted(all_keys):
        b = b_lines.get(key)
        a = a_lines.get(key)
        if not b or not a:
            changed_lines.append({
                "file": key[0],
                "line": key[1],
                "status": "added" if not b else "removed",
                "before_insn_count": b.get("instruction_count", 0) if b else 0,
                "after_insn_count": a.get("instruction_count", 0) if a else 0,
                "snippet": (a or b or {}).get("snippet", ""),
                "likely_effects": ["source line appeared in map" if not b else
                                   "source line no longer in map"],
            })
            continue
        if (b.get("instruction_count") != a.get("instruction_count")
                or set(b.get("tags", [])) != set(a.get("tags", []))):
            b_tag_cnt = _line_tag_counts(before_sm, key[0], key[1])
            a_tag_cnt = _line_tag_counts(after_sm, key[0], key[1])
            if not b_tag_cnt and b.get("tags"):
                b_tag_cnt = {t: b.get("instruction_count", 1) for t in b.get("tags", [])}
            if not a_tag_cnt and a.get("tags"):
                a_tag_cnt = {t: a.get("instruction_count", 1) for t in a.get("tags", [])}
            effects = _likely_effect(
                b_tag_cnt, a_tag_cnt,
                b.get("instruction_count", 0),
                a.get("instruction_count", 0))
            changed_lines.append({
                "file": key[0],
                "line": key[1],
                "status": "changed",
                "before_insn_count": b.get("instruction_count", 0),
                "after_insn_count": a.get("instruction_count", 0),
                "before_tags": b.get("tags", []),
                "after_tags": a.get("tags", []),
                "snippet": a.get("snippet", b.get("snippet", "")),
                "likely_effects": effects or ["instruction mix changed"],
            })

    diff = {
        "function": after_sm.get("function", before_sm.get("function")),
        "variant": after_sm.get("variant"),
        "before_instruction_count": before_sm.get("instruction_count", 0),
        "after_instruction_count": after_sm.get("instruction_count", 0),
        "before_path_category": before_sm.get("path_category"),
        "after_path_category": after_sm.get("path_category"),
        "before_attribution": before_sm.get("primary_attribution"),
        "after_attribution": after_sm.get("primary_attribution"),
        "added_instructions": added[:100],
        "removed_instructions": removed[:100],
        "added_instruction_count": len(added),
        "removed_instruction_count": len(removed),
        "changed_source_lines": changed_lines,
        "tag_delta": {},
    }

    b_tags = _summarize_tags(before_sm)
    a_tags = _summarize_tags(after_sm)
    for t in set(b_tags) | set(a_tags):
        diff["tag_delta"][t] = a_tags.get(t, 0) - b_tags.get(t, 0)

    if metrics_before and metrics_after:
        diff["before_static_estimate"] = metrics_before.get("static_model_estimate_cycles",
                                                            metrics_before.get("rthru"))
        diff["after_static_estimate"] = metrics_after.get("static_model_estimate_cycles",
                                                           metrics_after.get("rthru"))
        b_est = diff["before_static_estimate"] or 0
        a_est = diff["after_static_estimate"] or 0
        diff["static_estimate_delta"] = round(a_est - b_est, 2)

    return diff


def write_asm_diff_md(diff, path):
    lines = [
        "# Assembly diff: %s" % diff.get("function", ""),
        "",
        "- before instructions: %d" % diff.get("before_instruction_count", 0),
        "- after instructions: %d" % diff.get("after_instruction_count", 0),
        "- added: %d, removed: %d" % (
            diff.get("added_instruction_count", 0),
            diff.get("removed_instruction_count", 0)),
        "- path: %s -> %s" % (
            diff.get("before_path_category", "-"),
            diff.get("after_path_category", "-")),
    ]
    if "static_estimate_delta" in diff:
        lines.append("- static model estimate delta: %+0.2f" % diff["static_estimate_delta"])
    lines.append("")
    lines.append("## Changed source lines")
    lines.append("")
    for ch in diff.get("changed_source_lines", [])[:80]:
        lines.append("%s:%d (%s)" % (ch["file"], ch["line"], ch["status"]))
        lines.append("  before: %d insns  after: %d insns" % (
            ch.get("before_insn_count", 0), ch.get("after_insn_count", 0)))
        if ch.get("snippet"):
            lines.append("  %s" % ch["snippet"][:100])
        for eff in ch.get("likely_effects", []):
            lines.append("  likely: %s" % eff)
        lines.append("")
    path.write_text("\n".join(lines) + "\n")


def baseline_source_map_path(fn, arch, flags, compiler):
    return C.OUT_DIR / "baselines" / fn / ("%s-%s-%s" % (arch, compiler, flags)) / "source_map.json"


def save_baseline_source_map(variant_dir, fn, arch, flags, compiler):
    src = variant_dir / "source_map.json"
    if not src.exists():
        return None
    dest_dir = C.OUT_DIR / "baselines" / fn / ("%s-%s-%s" % (arch, compiler, flags))
    dest_dir.mkdir(parents=True, exist_ok=True)
    data = json.loads(src.read_text())
    dest = dest_dir / "source_map.json"
    dest.write_text(json.dumps(data, indent=2) + "\n")
    return dest


def run_diff(fn, arch, flags, compiler, metrics_before=None, metrics_after=None):
    variant_dir = C.variant_dir(fn, arch, flags, compiler)
    cur_path = variant_dir / "source_map.json"
    base_path = baseline_source_map_path(fn, arch, flags, compiler)
    if not cur_path.exists():
        return {"error": "current source_map.json missing; use --source-map"}
    if not base_path.exists():
        return {"error": "baseline source map missing; run baseline --source-map first"}

    before_sm = json.loads(base_path.read_text())
    after_sm = json.loads(cur_path.read_text())
    diff = diff_source_maps(before_sm, after_sm, metrics_before, metrics_after)

    out_dir = variant_dir
    out_dir.mkdir(parents=True, exist_ok=True)
    (out_dir / "asm_diff.json").write_text(json.dumps(diff, indent=2) + "\n")
    write_asm_diff_md(diff, out_dir / "asm_diff.md")
    return diff
