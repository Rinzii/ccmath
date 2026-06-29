#!/usr/bin/env python3
# Copyright (c) Ian Pike
# Copyright (c) CCMath contributors
#
# CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
# See LICENSE for more information.
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
"""Source-to-assembly traceability for asmlab.

Parses clang/gcc assembly with debug line tables (-gline-tables-only -fverbose-asm)
and maps instructions to source locations. Writes source_map.json and source_map.md.
"""

import contextlib
import json
import re
import sys
from pathlib import Path

import _asmlab_common as C

DEBUG_FLAGS = ["-gline-tables-only", "-fverbose-asm"]

_BRANCH = re.compile(r"^(j|b\.|bne|beq|bl|cbz|tbz)", re.I)
_CALL = re.compile(r"^(call|callq|bl)\b", re.I)
_LOAD = re.compile(r"^(mov|lea|ldr|ldp|vmov|movsd|movss|movapd)", re.I)
_STORE = re.compile(r"^(mov.*\[|str|stp)", re.I)
_SPILL = re.compile(r"\[rsp", re.I)
_FMA = re.compile(r"(fma|vfmadd|vfnmadd)", re.I)
_FP = re.compile(r"(mul|add|sub|div|sqrt|vfm|fmul|fadd|fsub|fdiv|fsqrt)", re.I)
_VEC = re.compile(r"(xmm|ymm|zmm|v[0-9])", re.I)
_SQRT_DIV = re.compile(r"(sqrt|div|sdiv|fdiv|vdiv)", re.I)
_TABLE = re.compile(r"(rip\)|pc\))", re.I)
_INT = re.compile(
    r"^(add|sub|mul|imul|and|or|xor|shl|shr|sar|inc|dec|incq|decq|addq|subq|cmp|test)\b", re.I)
_CONST = re.compile(r"^(mov|movabs|movq|movl|movw|movb).*\$|#(-?[0-9]+|0x)", re.I)


def emit_verbose_asm(outdir, src, arch, flags, compiler, extra_include_dirs=None):
    """Compile harness to asm_verbose.s with debug line tables."""
    cxx = C.cxx_compiler(compiler)
    cmd = [cxx, "-std=" + C.CXX_STD, "-S", "-I", str(C.INCLUDE_DIR)]
    for inc in extra_include_dirs or []:
        cmd += ["-I", str(inc)]
    cmd += flags + DEBUG_FLAGS
    if arch["target"]:
        cmd += ["--target=" + arch["target"]]
    cmd += arch["emit_flags"]
    cmd += [str(src), "-o", str(outdir / "asm_verbose.s")]
    res = C.run(cmd)
    (outdir / "compile_verbose.cmd").write_text(" ".join(cmd) + "\n")
    if res.returncode != 0:
        (outdir / "compile_verbose.err").write_text(res.stderr)
        return False
    return True


def _parse_file_table(lines):
    """Return {file_id: {dir, name, path}} from .file directives."""
    files = {0: {"dir": "", "name": "", "path": ""}}
    for ln in lines:
        code = ln.strip().split("##")[0].strip()
        m = re.match(r'\.file\s+(\d+)\s+"([^"]*)"\s+"([^"]*)"', code)
        if m:
            fid, directory, fname = m.groups()
            if directory and fname and not directory.endswith(fname):
                if fname.startswith("/") or directory.endswith("/"):
                    path = directory.rstrip("/") + "/" + fname.lstrip("/")
                else:
                    path = directory + "/" + fname
            elif fname:
                path = fname
            else:
                path = directory
            path = path.replace("//", "/")
            files[int(fid)] = {"dir": directory, "name": fname, "path": path}
    return files


def _parse_loc_comment(ln):
    """Parse ## path:line:col from clang verbose asm comment."""
    idx = ln.find("##")
    if idx == -1:
        return None
    comment = ln[idx + 2:].strip()
    m = re.match(r"(.+?):(\d+):(\d+)", comment)
    if not m:
        m2 = re.match(r"(.+?):(\d+)", comment)
        if m2:
            return {"file": m2.group(1).strip(), "line": int(m2.group(2)), "column": 0}
        return None
    return {"file": m.group(1).strip(), "line": int(m.group(2)), "column": int(m.group(3))}


def _parse_loc_directive(ln, files):
    m = re.match(r"\.loc\s+(\d+)\s+(\d+)\s+(\d+)", ln.strip())
    if not m:
        return None
    fid, line, col = int(m.group(1)), int(m.group(2)), int(m.group(3))
    f = files.get(fid, {})
    path = f.get("path", "")
    return {"file": path, "line": line, "column": col, "file_id": fid}


def _attribution_layer(path, fn):
    if not path:
        return "unknown", "unknown"
    p = path.replace("\\", "/")
    if "out/asmlab/variants" in p and "candidate.hpp" in p:
        return "candidate_source", "candidate"
    if "out/asmlab" in p and "harness.cpp" in p:
        return "generated_harness", "harness-only"
    if "include/ccmath/internal/" in p:
        return "ccmath_internal", "ccmath-internal"
    if "include/ccmath/" in p:
        return "ccmath_public", "ccmath-public"
    if "include/ccmath" in p or p.startswith("ccmath/"):
        return "ccmath_internal", "ccmath-internal"
    if "/usr/" in p or "sdk" in p.lower():
        return "system_header", "system"
    return "unknown", "unknown"


def _mapping_confidence(loc):
    if not loc or not loc.get("file"):
        return "unknown"
    if loc.get("line", 0) == 0:
        return "low"
    if loc.get("from_loc") and loc.get("from_comment"):
        return "high"
    if loc.get("from_loc"):
        return "high"
    if loc.get("from_comment"):
        return "medium"
    return "low"


def _is_fp_mnemonic(mnem, full_line):
    m = mnem.lower()
    if _FMA.search(full_line):
        return True
    if m in ("fadd", "fsub", "fmul", "fdiv", "fsqrt", "fsin", "fcos"):
        return True
    if m.endswith(("sd", "ss", "pd", "ps")) and any(
            x in m for x in ("mul", "add", "sub", "div", "sqrt")):
        return True
    if re.search(r"\bv(fadd|fsub|fmul|fdiv|sqrt|div)\b", full_line, re.I):
        return True
    return False


def _classify_tags(mnem, full_line):
    tags = []
    m = mnem.lower()
    if _CALL.match(m):
        tags.append("call")
        if re.search(r"ccm|asmlab", full_line, re.I):
            tags.append("local_call")
        elif "plt" in full_line.lower() or "@" in full_line:
            tags.append("external_call")
        else:
            tags.append("external_call")
    if _BRANCH.match(m) or m.startswith("j"):
        tags.append("branch")
    if _LOAD.match(m):
        tags.append("load")
    if _STORE.search(full_line) or (m.startswith("mov") and "[" in full_line):
        tags.append("store")
    if _SPILL.search(full_line):
        if "mov" in m and "[" in full_line:
            tags.append("spill")
        if "mov" in m and "xmm" in full_line and "[" in full_line:
            tags.append("reload")
    if _FMA.search(full_line):
        tags.append("fma")
    if _is_fp_mnemonic(m, full_line):
        tags.append("fp_op")
    if _VEC.search(full_line):
        tags.append("vector_op")
    if _SQRT_DIV.search(full_line) and _is_fp_mnemonic(m, full_line):
        tags.append("sqrt_div")
    if (m == "lea" or m == "leaq") and re.search(r"\(%rip\)", full_line, re.I):
        if "GOT" not in full_line.upper() and "PLT" not in full_line.upper():
            tags.append("table_load")
    if _CONST.search(full_line):
        tags.append("constant_materialization")
    if _INT.match(m):
        tags.append("int_op")
    if m in ("pushq", "push", "subq", "sub") and "rsp" in full_line:
        tags.append("prologue")
    if m in ("popq", "pop", "retq", "ret") or "epilogue" in full_line:
        tags.append("epilogue")
    if not tags:
        tags.append("unknown")
    return tags


def _snippet_cache():
    return {}


def _read_snippet(path, line, cache):
    if not path or line <= 0:
        return ""
    key = (path, line)
    if key in cache:
        return cache[key]
    candidates = []
    if path.startswith("include/"):
        candidates.append(C.PROJECT_ROOT / path)
    candidates.append(C.PROJECT_ROOT / path)
    candidates.append(Path(path))
    for p in candidates:
        if p.exists():
            with contextlib.suppress(OSError):
                lines = p.read_text().splitlines()
                if 1 <= line <= len(lines):
                    cache[key] = lines[line - 1].strip()
                    return cache[key]
    cache[key] = ""
    return ""


def _symbol_lookup(name, bodies):
    for cand in (name, "_" + name, name.lstrip("_")):
        if cand in bodies:
            return cand
    return None


def _parse_verbose_functions(asm_text):
    """Parse verbose asm into {symbol: [(raw_line, insn_or_none, loc_meta)]}."""
    lines = asm_text.splitlines()
    files = _parse_file_table(lines)
    bodies = {}
    current = None
    cur_entries = []
    current_loc = None

    def flush():
        nonlocal current, cur_entries
        if current is not None:
            bodies[current] = cur_entries
        cur_entries = []

    for raw in lines:
        stripped = raw.strip()
        if not stripped or stripped.startswith("#"):
            continue
        code = stripped.split("##")[0].split(";")[0].strip()
        if not code:
            continue
        if code.endswith(":") and not code.startswith("."):
            name = code[:-1]
            if name.startswith(".L") or (name.startswith("L") and not name.startswith("_")
                                        and not name.startswith("LCPI")):
                if current is not None:
                    cur_entries.append((raw, None, None))
                continue
            flush()
            current = name
            continue
        if current is None:
            continue
        if code.startswith(".cfi_endproc") or code.startswith(".section"):
            flush()
            current = None
            continue
        loc_comment = _parse_loc_comment(raw)
        if code.startswith(".loc"):
            loc = _parse_loc_directive(code, files)
            if loc:
                loc["from_loc"] = True
                if loc_comment:
                    loc["file"] = loc_comment["file"] or loc["file"]
                    loc["from_comment"] = True
                current_loc = loc
            cur_entries.append((raw, None, dict(current_loc) if current_loc else None))
            continue
        if code.startswith("."):
            cur_entries.append((raw, None, dict(current_loc) if current_loc else None))
            continue
        if code.endswith(":"):
            continue
        parts = code.split(None, 1)
        mnem = parts[0].lower().rstrip(",")
        loc = dict(current_loc) if current_loc else {}
        if loc_comment and not loc.get("file"):
            loc.update(loc_comment)
            loc["from_comment"] = True
        cur_entries.append((raw, {"mnem": mnem, "text": code}, loc if loc else None))
    flush()
    return bodies


def _kernel_callee_from_region(region_text):
    for ln in region_text.splitlines():
        parts = ln.strip().split()
        if len(parts) >= 2 and parts[0].lower() in ("jmp", "call", "callq", "b"):
            tgt = parts[1]
            if "ccm" in tgt.lower() or "pow" in tgt.lower() or "sqrt" in tgt.lower():
                return tgt
    return None


def _resolve_primary_symbol(emit_meta, region_text, bodies, fn):
    analyzed = emit_meta.get("analyzed_symbol", "")
    sym = _symbol_lookup(analyzed, bodies) or analyzed
    callee = _kernel_callee_from_region(region_text)
    if callee:
        csym = _symbol_lookup(callee, bodies)
        if csym and (fn.endswith("_impl") or fn.endswith("_rt")):
            return csym, sym, "kernel_symbol"
    if sym in bodies:
        return sym, sym, "analyzed_symbol"
    for s in bodies:
        if "pow_impl" in s or "powf_impl" in s or "sqrt" in s.lower():
            if fn in ("pow_impl", "powf_impl", "sqrt", "sqrtf", "sqrt_rt"):
                return s, sym, "heuristic_kernel"
    return sym, sym, "analyzed_symbol"


def _entries_to_instructions(entries, ordinal_start=1):
    insns = []
    ordinal = ordinal_start
    for raw, insn, loc in entries:
        if insn is None:
            continue
        insns.append({
            "ordinal": ordinal,
            "assembly": insn["text"],
            "mnemonic": insn["mnem"],
            "raw": raw.strip(),
            "source": loc or {},
            "tags": _classify_tags(insn["mnem"], insn["text"]),
        })
        ordinal += 1
    return insns


def build_source_map(variant_dir, fn, target, path_analysis=None):
    """Build source map from asm_verbose.s and write artifacts."""
    verbose_path = variant_dir / "asm_verbose.s"
    if not verbose_path.exists():
        return {"error": "asm_verbose.s missing; re-emit with --source-map"}

    emit_meta = {}
    meta_path = variant_dir / "emit_meta.json"
    if meta_path.exists():
        emit_meta = json.loads(meta_path.read_text())

    region_text = ""
    region_path = variant_dir / "region.s"
    if region_path.exists():
        region_text = region_path.read_text()

    bodies = _parse_verbose_functions(verbose_path.read_text())
    primary, harness_sym, resolve_mode = _resolve_primary_symbol(
        emit_meta, region_text, bodies, fn)

    cache = {}
    mapped = []
    if primary in bodies:
        mapped = _entries_to_instructions(bodies[primary])

    harness_insns = []
    if harness_sym != primary and harness_sym in bodies:
        harness_insns = _entries_to_instructions(bodies[harness_sym])

    follow_chain = emit_meta.get("follow_chain", [])
    inline_stack_base = ["generated harness: %s" % harness_sym]
    for sym in follow_chain:
        if sym != harness_sym:
            inline_stack_base.append("inlined: %s" % sym)

    for ent in mapped + harness_insns:
        loc = ent["source"]
        layer, attr = _attribution_layer(loc.get("file", ""), fn)
        ent["source_layer"] = layer
        ent["source_attribution"] = attr
        ent["mapping_confidence"] = _mapping_confidence(loc)
        line = loc.get("line", 0)
        ent["source_snippet"] = _read_snippet(loc.get("file", ""), line, cache)
        if path_analysis:
            ent["path_category"] = path_analysis.get("path_category", "unknown")
        if attr == "ccmath-internal" and inline_stack_base:
            ent["inline_stack"] = list(inline_stack_base)
        elif attr == "harness-only":
            ent["inline_stack"] = [inline_stack_base[0]] if inline_stack_base else []

    # Summarize by source line
    by_line = {}
    for ent in mapped:
        loc = ent["source"]
        key = (loc.get("file", ""), loc.get("line", 0))
        if key not in by_line:
            by_line[key] = {"file": key[0], "line": key[1], "insn_count": 0,
                            "tags": set(), "snippets": ent["source_snippet"],
                            "attribution": ent["source_attribution"],
                            "confidence": ent["mapping_confidence"]}
        by_line[key]["insn_count"] += 1
        by_line[key]["tags"].update(ent["tags"])

    line_summary = []
    for key in sorted(by_line, key=lambda k: (k[0], k[1])):
        e = by_line[key]
        line_summary.append({
            "file": e["file"],
            "line": e["line"],
            "instruction_count": e["insn_count"],
            "tags": sorted(e["tags"]),
            "snippet": e["snippets"],
            "attribution": e["attribution"],
            "confidence": e["confidence"],
        })

    attr_insn_counts = {}
    for ent in mapped:
        a = ent.get("source_attribution", "unknown")
        attr_insn_counts[a] = attr_insn_counts.get(a, 0) + 1
    if attr_insn_counts:
        primary_attr = max(attr_insn_counts, key=attr_insn_counts.get)
    else:
        primary_attr = "unknown"

    result = {
        "function": fn,
        "variant": variant_dir.name,
        "primary_symbol": primary,
        "harness_symbol": harness_sym,
        "resolve_mode": resolve_mode,
        "follow_chain": follow_chain,
        "inline_context": {
            "outer_call_site": harness_sym,
            "kernel_symbol": primary,
            "follow_chain": follow_chain,
        },
        "primary_attribution": primary_attr,
        "attribution_counts": attr_insn_counts,
        "instruction_count": len(mapped),
        "harness_instruction_count": len(harness_insns),
        "path_category": (path_analysis or {}).get("path_category"),
        "path_confidence": (path_analysis or {}).get("confidence"),
        "instructions": mapped,
        "harness_instructions": harness_insns if harness_insns else None,
        "source_lines": line_summary,
        "mapping_notes": [
            "Mappings come from compiler line tables (-gline-tables-only -fverbose-asm).",
            "Optimized code may have ambiguous or line-zero attributions.",
            "inline_stack and inline_context are metadata-derived follow chains, not full DWARF inline stacks.",
        ],
    }

    variant_dir.joinpath("source_map.json").write_text(
        json.dumps(result, indent=2) + "\n")
    write_source_map_md(result, variant_dir / "source_map.md")
    return result


def write_source_map_md(result, path):
    lines = [
        "# Source map: %s (%s)" % (result["function"], result["variant"]),
        "",
        "- primary symbol: %s" % result.get("primary_symbol", ""),
        "- source attribution: %s" % result.get("primary_attribution", "unknown"),
        "- attribution counts: %s" % result.get("attribution_counts", {}),
        "- path category: %s" % result.get("path_category", "-"),
        "- instructions mapped: %d" % result.get("instruction_count", 0),
        "",
    ]
    if result.get("error"):
        lines.append("Error: %s" % result["error"])
        path.write_text("\n".join(lines) + "\n")
        return

    lines.append("## Instruction to source")
    lines.append("")
    for ent in result.get("instructions", [])[:200]:
        loc = ent.get("source", {})
        f = loc.get("file", "?")
        ln = loc.get("line", 0)
        conf = ent.get("mapping_confidence", "unknown")
        lines.append("[%d] %s" % (ent["ordinal"], ent["assembly"]))
        lines.append("     confidence: %s | tags: %s" % (conf, ", ".join(ent.get("tags", []))))
        if ln == 0:
            lines.append("     -> %s:line unknown (compiler line 0)" % f)
        else:
            lines.append("     -> %s:%d" % (f, ln))
        snip = ent.get("source_snippet", "")
        if snip:
            lines.append("        %s" % snip[:120])
        lines.append("")
    if len(result.get("instructions", [])) > 200:
        lines.append("... %d more (see source_map.json)" % (
            len(result["instructions"]) - 200))
    path.write_text("\n".join(lines) + "\n")


def main(argv=None):
    ap = __import__("argparse").ArgumentParser(description="Build source map for a variant.")
    ap.add_argument("fn")
    ap.add_argument("--arch", default="")
    args = ap.parse_args(argv)
    target = C.get_target(args.fn)
    fn_dir = C.OUT_DIR / args.fn
    want = set(C.parse_arch_list(args.arch)) if args.arch else None
    for variant_dir in sorted(p for p in fn_dir.iterdir() if p.is_dir()):
        arch_name = variant_dir.name.split("-clang-")[0].split("-gcc-")[0]
        if want and arch_name not in want:
            continue
        pa = None
        pa_path = variant_dir / "path_analysis.json"
        if pa_path.exists():
            pa = json.loads(pa_path.read_text())
        r = build_source_map(variant_dir, args.fn, target, pa)
        print("  %s: %d insns, attribution=%s" % (
            variant_dir.name, r.get("instruction_count", 0),
            r.get("primary_attribution", "?")))
    return 0


if __name__ == "__main__":
    sys.exit(main())
