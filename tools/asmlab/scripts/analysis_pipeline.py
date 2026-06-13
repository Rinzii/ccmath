#!/usr/bin/env python3
# Copyright (c) Ian Pike
# Copyright (c) CCMath contributors
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
"""Deep analyze passes and pipeline (CFG, MCA, remarks, semantic)."""

import json
import platform
import re
import shutil
import subprocess
import tempfile
from pathlib import Path

import _asmlab_common as C
import analysis_common as AC
import analyze as legacy_analyze

def _load_source_map(variant_dir):
    path = Path(variant_dir) / "source_map.json"
    if not path.exists():
        return None
    return json.loads(path.read_text())


def emit_opt_record(variant_dir, fn, target, arch_name, flags_name, compiler,
                    extra_include_dirs=None):
    """Compile harness with optimization record when not already present."""
    variant_dir = Path(variant_dir)
    opt_path = variant_dir / "opt.yaml"
    if opt_path.exists():
        return True
    arch = C.resolve_arch(arch_name)
    flags = ["-std=" + C.CXX_STD] + list(C.FLAG_VARIANTS[flags_name])
    flags += ["-gline-tables-only"]
    if arch["target"]:
        flags.append("--target=" + arch["target"])
    flags += arch["emit_flags"]
    harness = variant_dir / "harness.cpp"
    cxx = C.cxx_compiler(compiler)
    includes = [str(C.INCLUDE_DIR)]
    for inc in extra_include_dirs or []:
        includes.append(str(inc))
    ok, _, err = emit_opt_record_compile(
        variant_dir, cxx, flags, harness, includes)
    if not ok:
        (variant_dir / "opt_record.err").write_text(err or "")
    return ok


def run_deep_analysis(variant_dir, fn, target, arch_name, flags_name, compiler,
                      baseline_dir=None, record_history=False, extra_include_dirs=None):
    """Run all deep analysis passes for one variant directory."""
    variant_dir = Path(variant_dir)
    source_map = _load_source_map(variant_dir)
    if not source_map:
        return None, "source_map.json missing; enable --source-map or --deep-analyze"

    emit_opt_record(variant_dir, fn, target, arch_name, flags_name, compiler,
                    extra_include_dirs=extra_include_dirs)

    cfg = build_cfg(variant_dir, source_map)
    mca = build_mca(variant_dir, cfg, arch_name, compiler)
    opt = build_opt_remarks(variant_dir, source_map)
    rp = build_reg_pressure(variant_dir, source_map, cfg)
    micro = build_microarch(variant_dir, source_map, mca, cfg)
    vec = build_vectorization(variant_dir, source_map, opt)

    baseline_semantic = None
    if baseline_dir:
        baseline_semantic = AC.load_json(Path(baseline_dir) / "semantic.json")

    semantic = build_semantic(
        variant_dir, source_map, opt, rp, vec, baseline_semantic)
    perf = build_perf(variant_dir, mca)

    bundle = {
        "function": fn,
        "variant": variant_dir.name,
        "arch": arch_name,
        "source_map": {"instruction_count": source_map.get("instruction_count")},
        "cfg": cfg,
        "mca": mca,
        "opt_remarks": opt,
        "reg_pressure": rp,
        "microarch": micro,
        "vectorization": vec,
        "semantic": semantic,
        "perf": perf,
    }

    summary = {
        "analysis_version": AC.ANALYSIS_VERSION,
        "function": fn,
        "variant": variant_dir.name,
        "instruction_count": source_map.get("instruction_count"),
        "block_count": cfg.get("block_count"),
        "mca_ipc": (mca or {}).get("function", {}).get("ipc"),
        "bottleneck": (mca or {}).get("bottleneck", {}).get("primary"),
        "spill_count": rp.get("spill_count"),
        "vectorized": vec.get("vectorized"),
        "semantic_classifications": [c["kind"] for c in semantic.get("classifications", [])],
    }
    AC.write_artifact(variant_dir, "analysis_summary", summary, [
        "# Analysis summary",
        "",
        "- instructions: %s" % summary["instruction_count"],
        "- blocks: %s" % summary["block_count"],
        "- mca IPC: %s" % summary["mca_ipc"],
        "- bottleneck: %s" % summary["bottleneck"],
        "- spills: %s" % summary["spill_count"],
        "- vectorized: %s" % summary["vectorized"],
        "",
    ])

    return bundle, None


DEEP_ARTIFACT_NAMES = (
    "cfg", "mca", "opt_remarks", "reg_pressure", "microarch",
    "vectorization", "semantic", "perf", "analysis_summary",
)


def save_baseline_deep(variant_dir, fn, arch, flags, compiler):
    """Copy deep analysis JSON artifacts into baselines tree."""
    variant_dir = Path(variant_dir)
    dest_dir = C.OUT_DIR / "baselines" / fn / ("%s-%s-%s" % (arch, compiler, flags))
    dest_dir.mkdir(parents=True, exist_ok=True)
    saved = 0
    for name in DEEP_ARTIFACT_NAMES:
        src = variant_dir / ("%s.json" % name)
        if src.exists():
            shutil.copy2(src, dest_dir / src.name)
            saved += 1
    return dest_dir if saved else None


def baseline_deep_dir(fn, arch, flags, compiler):
    return C.OUT_DIR / "baselines" / fn / ("%s-%s-%s" % (arch, compiler, flags))


def run_deep_for_fn(fn, arches, flags, compiler, baseline_fn_dir=None):
    target = C.load_registry()[fn]
    results = []
    for variant_dir in sorted((C.OUT_DIR / fn).iterdir()):
        if not variant_dir.is_dir():
            continue
        arch_name = variant_dir.name.split("-clang-")[0].split("-gcc-")[0]
        if arch_name not in arches:
            continue
        baseline_dir = None
        if baseline_fn_dir:
            bdir = baseline_fn_dir / variant_dir.name
            if bdir.exists():
                baseline_dir = bdir
        bundle, err = run_deep_analysis(
            variant_dir, fn, target, arch_name, flags, compiler,
            baseline_dir=baseline_dir)
        if err:
            print("  [%s] skip: %s" % (variant_dir.name, err))
            continue
        results.append(bundle)
        print("  [%s] deep analysis ok" % variant_dir.name)
    return results


# --- deep pass implementations ---

# --- from cfg_analysis ---

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

# --- from mca_deep ---

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

# --- from opt_remarks ---

def _parse_yaml_documents(text):
    """Minimal YAML document splitter for LLVM opt records (no PyYAML dep)."""
    docs = []
    current = {}
    in_args = False
    args_indent = 0

    def flush():
        if current:
            docs.append(dict(current))

    for line in text.splitlines():
        if line.strip() == "---":
            flush()
            current = {}
            in_args = False
            continue
        if not line.strip() or line.strip().startswith("#"):
            continue

        m = re.match(r"^---\s*!(Passed|Missed|Failure|Analysis)", line)
        if m:
            flush()
            current = {"kind": m.group(1)}
            in_args = False
            continue

        m = re.match(r"^(\s*)(\w+):\s*(.*)$", line)
        if m:
            indent, key, val = m.groups()
            val = val.strip().strip("'\"")
            if key == "Args":
                in_args = True
                args_indent = len(indent)
                current.setdefault("args", [])
                continue
            if in_args and len(indent) > args_indent:
                current.setdefault("args", []).append(line.strip())
                continue
            in_args = False
            if key == "DebugLoc":
                current["_debugloc_pending"] = True
                continue
            current[key.lower()] = val
            continue

        if "File:" in line and "_debugloc_pending" in current:
            fm = re.search(r"File:\s*'([^']+)'", line)
            if fm:
                current["file"] = fm.group(1)
            lm = re.search(r"Line:\s*(\d+)", line)
            if lm:
                current["line"] = int(lm.group(1))
            continue

        if "Message:" in line or "Reason:" in line:
            km = re.match(r"\s*(Message|Reason):\s*(.*)", line)
            if km:
                current[km.group(1).lower()] = km.group(2).strip()

    flush()
    return docs


def _classify_remark(doc):
    name = (doc.get("name") or "").lower()
    pass_name = (doc.get("pass") or "").lower()
    message = (doc.get("message") or doc.get("reason") or "").lower()
    tags = []
    if "vector" in name or "vector" in message or pass_name == "loop-vectorize":
        tags.append("vectorization")
    if "inline" in pass_name or "inline" in name:
        tags.append("inlining")
    if "unroll" in name or "unroll" in message:
        tags.append("unroll")
    if "licm" in pass_name:
        tags.append("licm")
    if "missed" in (doc.get("kind") or "").lower() or doc.get("kind") == "Missed":
        tags.append("missed_optimization")
    if "fma" in message or "contract" in message:
        tags.append("fma_contraction")
    return tags


def emit_opt_record_compile(variant_dir, compiler, flags, harness_cpp, include_dirs):
    """Compile harness with optimization record flags."""
    variant_dir = Path(variant_dir)
    opt_path = variant_dir / "opt.yaml"
    obj = variant_dir / "harness_opt.o"
    cmd = [compiler] + list(flags) + [
        "-fsave-optimization-record",
        "-foptimization-record-file=%s" % opt_path,
        "-c", str(harness_cpp), "-o", str(obj),
    ]
    for inc in include_dirs:
        cmd.extend(["-I", inc])
    proc = subprocess.run(cmd, capture_output=True, text=True, check=False)
    return proc.returncode == 0, opt_path, proc.stderr


def build_opt_remarks(variant_dir, source_map=None):
    variant_dir = Path(variant_dir)
    opt_path = variant_dir / "opt.yaml"
    if not opt_path.exists():
        data = {
            "analysis_version": AC.ANALYSIS_VERSION,
            "available": False,
            "reason": "opt.yaml missing (re-run emit with deep analysis)",
            "remarks": [],
            "confidence": "none",
        }
        AC.write_artifact(variant_dir, "opt_remarks", data, ["# Optimization remarks", "", "opt.yaml not found."])
        return data

    raw = opt_path.read_text(errors="replace")
    docs = _parse_yaml_documents(raw)

    remarks = []
    for doc in docs:
        tags = _classify_remark(doc)
        f = doc.get("file", "")
        layer = AC.attribution_layer(f)
        remarks.append({
            "kind": doc.get("kind", "unknown"),
            "pass": doc.get("pass", ""),
            "name": doc.get("name", ""),
            "file": f,
            "line": doc.get("line", 0),
            "function": doc.get("function", ""),
            "message": doc.get("message") or doc.get("reason", ""),
            "tags": tags,
            "attribution_layer": layer,
        })

    by_tag = {}
    for r in remarks:
        for t in r["tags"]:
            by_tag.setdefault(t, []).append(r)

    result = {
        "analysis_version": AC.ANALYSIS_VERSION,
        "available": True,
        "remark_count": len(remarks),
        "remarks": remarks,
        "by_tag": {k: len(v) for k, v in by_tag.items()},
        "missed_vectorization": [r for r in remarks if "vectorization" in r["tags"] and "missed" in r["tags"]],
        "inline_decisions": [r for r in remarks if "inlining" in r["tags"]],
        "confidence": "high" if remarks else "low",
        "notes": [
            "Remarks are compiler-reported correlates, not proven runtime causation.",
            "Missed optimization does not always imply performance loss.",
        ],
    }

    md = [
        "# Optimization remarks",
        "",
        "- total remarks: %d" % len(remarks),
        "",
    ]
    for tag, count in sorted(result["by_tag"].items()):
        md.append("- %s: %d" % (tag, count))
    md.append("")
    md.append("## Missed vectorization (sample)")
    for r in result["missed_vectorization"][:5]:
        md.append("- %s:%d %s" % (r["file"], r["line"], r["name"]))
    md.append("")

    AC.write_artifact(variant_dir, "opt_remarks", result, md)
    return result

# --- from reg_pressure ---

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

# --- from vectorization ---

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
            "Scalar fallback regions are grouped by source line.",
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

# --- from semantic_analysis ---

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

# --- from microarch_model ---

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

# --- from perf_profile ---

def _find_perf():
    return shutil.which("perf")


def _run_perf_stat(binary, events=None):
    events = events or [
        "cycles", "instructions", "branches", "branch-misses",
        "cache-references", "cache-misses",
    ]
    perf = _find_perf()
    if not perf:
        return None, "perf not found"
    if not Path(binary).exists():
        return None, "benchmark binary not found: %s" % binary
    cmd = [perf, "stat", "-x,", "-e", ",".join(events), str(binary)]
    proc = subprocess.run(cmd, capture_output=True, text=True, check=False)
    if proc.returncode != 0:
        return None, proc.stderr or proc.stdout
    return proc.stdout, None


def _parse_perf_csv(stdout):
    metrics = {}
    for line in stdout.splitlines():
        if not line.strip() or line.startswith("#"):
            continue
        parts = line.split(",")
        if len(parts) >= 3:
            try:
                val = float(parts[0])
                name = parts[2].strip()
                metrics[name] = val
            except ValueError:
                continue
    if "cycles" in metrics and "instructions" in metrics and metrics["cycles"]:
        metrics["ipc"] = metrics["instructions"] / metrics["cycles"]
    return metrics


def build_perf(variant_dir, mca=None, bench_binary=None):
    variant_dir = Path(variant_dir)
    system = platform.system()
    perf_available = _find_perf() is not None and system == "Linux"

    measured = {}
    error = None
    if perf_available and bench_binary:
        out, err = _run_perf_stat(bench_binary)
        if out:
            measured = _parse_perf_csv(out)
        else:
            error = err

    predicted_ipc = None
    if mca and mca.get("function"):
        predicted_ipc = mca["function"].get("ipc")

    comparison = {}
    if measured.get("ipc") and predicted_ipc:
        comparison = {
            "measured_ipc": measured["ipc"],
            "predicted_ipc": predicted_ipc,
            "delta": measured["ipc"] - predicted_ipc,
            "note": "Static MCA IPC vs perf stat IPC on harness binary",
        }

    result = {
        "analysis_version": AC.ANALYSIS_VERSION,
        "platform": system,
        "perf_available": perf_available,
        "instruments_available": False,
        "vtune_available": False,
        "measured": measured,
        "predicted": {"ipc": predicted_ipc} if predicted_ipc else {},
        "comparison": comparison,
        "error": error,
        "confidence": "high" if measured else "none",
        "notes": [
            "Linux perf requires a runnable benchmark binary and appropriate permissions.",
            "Apple Instruments and VTune are not wired in this revision.",
            "Compare measured IPC to llvm-mca when perf stat data is available.",
        ],
    }

    md = [
        "# Runtime performance",
        "",
        "- platform: %s" % system,
        "- perf available: %s" % perf_available,
        "",
    ]
    if measured:
        for k, v in measured.items():
            md.append("- %s: %s" % (k, v))
    else:
        md.append("- no measured counters (see notes)")
    if comparison:
        md.append("")
        md.append("## Predicted vs measured")
        md.append("- measured IPC: %.3f" % comparison["measured_ipc"])
        md.append("- predicted IPC: %.3f" % comparison["predicted_ipc"])
    md.append("")

    AC.write_artifact(variant_dir, "perf", result, md)
    return result

