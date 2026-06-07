#!/usr/bin/env python3
# Copyright (c) Ian Pike
# Copyright (c) CCMath contributors
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
"""CFG recovery from mapped assembly instructions."""

import analysis_common as AC


def _recover_blocks(instructions):
    """Split linear instruction list into basic blocks."""
    blocks = []
    current = {"id": "B0", "label": "entry", "instructions": [], "start_ordinal": None}
    label_to_block = {"entry": "B0"}
    block_id = 0

    def flush():
        nonlocal block_id
        if current["instructions"] or current["start_ordinal"] is not None:
            blocks.append(current)

    for ent in instructions:
        ordinal = ent.get("ordinal", 0)
        asm = ent.get("assembly", "")
        mnem = AC.insn_mnemonic(asm)

        if current["start_ordinal"] is None:
            current["start_ordinal"] = ordinal

        # Split before label targets that appear as internal symbols in verbose asm
        if mnem.endswith(":"):
            continue

        current["instructions"].append(ent)

        is_term = mnem in ("jmp", "ret", "retq", "b", "call", "callq")
        is_cond = AC.is_branch_mnem(mnem) and mnem not in ("call", "callq", "jmp")

        if is_term or is_cond:
            flush()
            block_id += 1
            next_label = "B%d" % block_id
            fallthrough = not (mnem in ("jmp", "ret", "retq") or mnem.startswith("b"))
            current = {
                "id": next_label,
                "label": next_label,
                "instructions": [],
                "start_ordinal": None,
            }
            if fallthrough:
                label_to_block["fallthrough_%d" % block_id] = next_label

    flush()

    if not blocks:
        blocks = [{"id": "B0", "label": "entry", "instructions": instructions,
                   "start_ordinal": instructions[0].get("ordinal", 1) if instructions else None}]

    for i, b in enumerate(blocks):
        b["id"] = "B%d" % i
        b["label"] = b["id"]
        b["instruction_count"] = len(b["instructions"])
        b["end_ordinal"] = (b["instructions"][-1].get("ordinal") if b["instructions"] else None)
    return blocks


def _build_edges(blocks):
    edges = []
    for i, b in enumerate(blocks):
        if not b["instructions"]:
            continue
        last = b["instructions"][-1]
        asm = last.get("assembly", "")
        mnem = AC.insn_mnemonic(asm)
        src = b["id"]

        if mnem in ("ret", "retq"):
            edges.append({"from": src, "to": "exit", "kind": "return"})
            continue
        if mnem in ("call", "callq"):
            edges.append({"from": src, "to": "call", "kind": "call",
                          "target": AC.branch_targets(asm)})
            if i + 1 < len(blocks):
                edges.append({"from": src, "to": blocks[i + 1]["id"], "kind": "fallthrough"})
            continue
        if mnem == "jmp":
            edges.append({"from": src, "to": "unknown", "kind": "unconditional",
                          "target": AC.branch_targets(asm)})
            continue
        if AC.is_branch_mnem(mnem):
            edges.append({"from": src, "to": "unknown", "kind": "conditional",
                          "target": AC.branch_targets(asm)})
            if i + 1 < len(blocks):
                edges.append({"from": src, "to": blocks[i + 1]["id"], "kind": "fallthrough"})
            continue
        if i + 1 < len(blocks):
            edges.append({"from": src, "to": blocks[i + 1]["id"], "kind": "fallthrough"})
    return edges


def _compute_idom(block_ids, edges, entry="B0"):
    """Simple iterative immediate-dominator computation."""
    preds = {b: set() for b in block_ids}
    for e in edges:
        if e["to"] in preds and e["from"] in preds:
            preds[e["to"]].add(e["from"])

    idom = {entry: entry}
    others = [b for b in block_ids if b != entry]
    for b in others:
        idom[b] = None

    changed = True
    while changed:
        changed = False
        for b in others:
            ps = [p for p in preds[b] if p in idom]
            if not ps:
                continue
            new_idom = ps[0]
            for p in ps[1:]:
                new_idom = _intersect(idom, new_idom, p, entry)
            if idom[b] != new_idom:
                idom[b] = new_idom
                changed = True
    return idom


def _intersect(idom, b1, b2, entry):
    finger1, finger2 = b1, b2
    while finger1 != finger2:
        while finger1 and finger1 != entry and idom.get(finger1) and (
                list(block_order(idom)).index(finger1) if False else True):
            if finger1 == b2:
                break
            finger1 = idom.get(finger1, entry)
        while finger2 and finger2 != entry:
            if finger2 == finger1:
                break
            finger2 = idom.get(finger2, entry)
        if finger1 == finger2:
            return finger1
        if finger1 == entry or finger2 == entry:
            return entry
    return finger1 or entry


def block_order(idom):
    return idom.keys()


def _find_loops(block_ids, edges, idom):
    """Back-edge loop detection: edge to dominator."""
    loops = []
    for e in edges:
        tgt, src = e.get("to"), e.get("from")
        if tgt in idom and idom.get(src) == tgt:
            loops.append({"header": tgt, "back_edge_from": src, "kind": "natural_loop"})
    return loops


def _dominance_fix_intersect(idom, b1, b2, entry):
    """Meet operator for dominators (simplified for small CFGs)."""
    ancestors1 = set()
    f = b1
    while f:
        ancestors1.add(f)
        if f == entry:
            break
        f = idom.get(f)
    f = b2
    while f:
        if f in ancestors1:
            return f
        if f == entry:
            return entry
        f = idom.get(f)
    return entry


def build_cfg(variant_dir, source_map):
    instructions = source_map.get("instructions", [])
    blocks = _recover_blocks(instructions)
    block_ids = [b["id"] for b in blocks]
    edges = _build_edges(blocks)

    idom = {}
    if block_ids:
        entry = block_ids[0]
        idom = {entry: entry}
        for b in block_ids[1:]:
            idom[b] = None
        preds = {b: set() for b in block_ids}
        for e in edges:
            if e["to"] in preds:
                preds[e["to"]].add(e["from"])
        changed = True
        while changed:
            changed = False
            for b in block_ids[1:]:
                ps = list(preds[b])
                if not ps:
                    continue
                new = ps[0]
                for p in ps[1:]:
                    new = _dominance_fix_intersect(idom, new, p, entry)
                if idom[b] != new:
                    idom[b] = new
                    changed = True

    loops = _find_loops(block_ids, edges, idom)

    for b in blocks:
        sources = set()
        for ent in b["instructions"]:
            loc = ent.get("source", {})
            f, ln = loc.get("file", ""), loc.get("line", 0)
            if f:
                sources.add((f, ln))
        b["source_regions"] = [{"file": f, "line": ln} for f, ln in sorted(sources)]

    result = {
        "analysis_version": AC.ANALYSIS_VERSION,
        "block_count": len(blocks),
        "edge_count": len(edges),
        "loop_count": len(loops),
        "blocks": blocks,
        "edges": edges,
        "dominators": idom,
        "loops": loops,
        "confidence": "medium",
        "notes": [
            "CFG recovered from linearized mapped instructions.",
            "Indirect branches and computed targets are unresolved.",
            "Edge targets marked unknown when label resolution failed.",
        ],
    }

    md = [
        "# CFG analysis",
        "",
        "- blocks: %d" % len(blocks),
        "- edges: %d" % len(edges),
        "- loops: %d" % len(loops),
        "- confidence: medium (heuristic block recovery)",
        "",
        "## Blocks",
        "",
    ]
    for b in blocks:
        md.append("### %s (%d insns, ordinals %s-%s)" % (
            b["id"], b["instruction_count"], b.get("start_ordinal"), b.get("end_ordinal")))
        for reg in b.get("source_regions", [])[:3]:
            md.append("- %s:%d" % (reg["file"], reg["line"]))
        md.append("")

    AC.write_artifact(variant_dir, "cfg", result, md)
    return result
