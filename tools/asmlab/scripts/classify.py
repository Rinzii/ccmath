#!/usr/bin/env python3
# Copyright (c) Ian Pike
# Copyright (c) CCMath contributors
#
# CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
# See LICENSE for more information.
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
"""Classify emitted assembly regions by semantic path type.

Parses region.s and optional emit_meta.json to produce instruction metrics,
structural signals, and a conservative path category with confidence level.
"""

import json
import re
import sys
from pathlib import Path

import _asmlab_common as C

_PATH_CATEGORIES_PATH = C.ASMLAB_DIR / "registry" / "path_categories.json"

_BRANCH_PREFIXES = (
    "jmp", "je", "jne", "jz", "jnz", "jl", "jle", "jg", "jge", "ja", "jb",
    "jae", "jbe", "jo", "jno", "js", "jns", "jp", "jnp", "jcxz", "jecxz",
    "b.", "b.eq", "b.ne", "b.lt", "b.le", "b.gt", "b.ge", "b.hi", "b.lo",
    "cbz", "cbnz", "tbz", "tbnz",
)
_CALL_PREFIXES = ("call", "callq", "bl", "blr")
_FP_OPS = ("mul", "fma", "fmul", "vfm", "fadd", "fsub", "fdiv", "div", "sqrt")
_LOAD_STORE = ("mov", "movq", "movsd", "movss", "vmov", "ldr", "str", "ldp", "stp", "lea")


def _load_path_config():
    with _PATH_CATEGORIES_PATH.open() as fh:
        return json.load(fh)


def _strip_comment(s):
    for marker in ("##", "//", ";"):
        idx = s.find(marker)
        if idx != -1:
            s = s[:idx]
    return s.rstrip()


def _parse_instructions(region_text):
    """Return list of (mnemonic_lower, full_line, operands)."""
    insns = []
    for raw in region_text.splitlines():
        code = _strip_comment(raw.strip())
        if not code or code.endswith(":") or code.startswith("."):
            continue
        parts = code.replace(",", " ").split()
        if not parts:
            continue
        mnem = parts[0].lower()
        ops = " ".join(parts[1:])
        insns.append((mnem, code, ops))
    return insns


def _is_ccmath_mangled(target):
    t = target.lower()
    return "ccm" in t or "3ccm" in t or "asmlab_" in t


def _is_external_call_target(target, libm_symbols):
    t = target.lower().lstrip("_").split("@")[0].split("$")[0]
    if _is_ccmath_mangled(target):
        return False
    for sym in libm_symbols:
        base = sym.lstrip("_").lower()
        if t == base or t.endswith(base):
            return True
    if "@plt" in target.lower() or "plt" in target.lower():
        return True
    return False


def _is_local_call_target_ext(target, local_symbols):
    if _is_ccmath_mangled(target):
        return True
    return _is_local_call_target(target, local_symbols)


def _is_local_call_target(target, local_symbols):
    for cand in (target, "_" + target, target.lstrip("_")):
        if cand in local_symbols:
            return True
    return False


def analyze_region(region_path, emit_meta=None, registry_target=None):
    """Analyze a region.s file and return path_analysis dict."""
    cfg = _load_path_config()
    libm_symbols = cfg["libm_symbols"]
    hw_sqrt = set(m.lower() for m in cfg["hardware_sqrt_mnemonics"])

    text = region_path.read_text() if region_path.exists() else ""
    insns = _parse_instructions(text)

    branch_count = 0
    call_count = 0
    local_calls = []
    external_calls = []
    load_store_count = 0
    fp_op_count = 0
    fma_count = 0
    div_sqrt_count = 0
    vector_present = False
    table_load_hints = 0
    hw_sqrt_present = False

    local_symbols = set()
    if emit_meta:
        local_symbols.update(emit_meta.get("follow_chain", []))
        local_symbols.add(emit_meta.get("requested_symbol", ""))
        local_symbols.add(emit_meta.get("analyzed_symbol", ""))

    for mnem, _line, ops in insns:
        if mnem.startswith("j") or mnem in _BRANCH_PREFIXES or mnem.startswith("b."):
            branch_count += 1
        if mnem in _CALL_PREFIXES:
            call_count += 1
            parts = ops.split()
            if parts:
                tgt = parts[0]
                if _is_external_call_target(tgt, libm_symbols):
                    external_calls.append(tgt)
                elif _is_local_call_target_ext(tgt, local_symbols):
                    local_calls.append(tgt)
                else:
                    external_calls.append(tgt)
        if any(mnem.startswith(p) or p in mnem for p in _LOAD_STORE):
            load_store_count += 1
        if any(p in mnem for p in _FP_OPS):
            fp_op_count += 1
        if "fma" in mnem or "vfmadd" in mnem or "vfnmadd" in mnem:
            fma_count += 1
        if "div" in mnem or "sqrt" in mnem:
            div_sqrt_count += 1
        if mnem in hw_sqrt:
            hw_sqrt_present = True
        if any(r in ops for r in ("xmm", "ymm", "zmm", "v0", "v1", "v2", "v3")):
            vector_present = True
        if mnem == "lea" and ("rip" in ops or "pc" in ops):
            table_load_hints += 1

    insn_count = len(insns)
    external_libm = [c for c in external_calls
                     if _is_external_call_target(c, libm_symbols)]
    likely_thin_forwarder = insn_count <= 8 and call_count + branch_count <= 2
    internal_only_calls = call_count == 0 or (
        call_count == len(local_calls) and not external_calls
    )
    likely_fully_inlined = internal_only_calls
    assembly_empty_or_trivial = insn_count <= 2
    possible_constant_fold = (
        insn_count <= 6 and fp_op_count == 0 and call_count == 0
        and registry_target and "pow" in registry_target.get("expr", "").lower()
    )

    path_hints = (registry_target or {}).get("path_hints", {})
    intended = (registry_target or {}).get("intended_path", "")
    harness_mode = (registry_target or {}).get("harness_mode", "runtime_flatten")
    fn_name = (registry_target or {}).get("_fn_name", "")

    evidence = []
    category = "unknown"
    confidence = "low"

    if external_libm:
        category = "external_libm_call"
        confidence = "high"
        evidence.append("external libm call(s): %s" % ", ".join(external_libm))
    elif hw_sqrt_present and insn_count <= 12:
        category = "hardware_instruction_lowering"
        confidence = "high"
        evidence.append("hardware sqrt instruction in short region")
    elif harness_mode == "constexpr_probe":
        category = "ccmath_constexpr_path"
        confidence = "medium"
        evidence.append("constexpr_probe harness mode")
    elif fn_name.endswith("_impl") or fn_name.endswith("_rt"):
        has_internal = local_calls or any(_is_ccmath_mangled(c) for c in external_calls)
        if likely_fully_inlined or (insn_count > 12 and not external_calls):
            category = "ccmath_internal_kernel"
            confidence = "high" if insn_count > 20 else "medium"
            evidence.append("direct impl/rt entry with inlined body")
        elif has_internal and not external_libm:
            category = "ccmath_internal_kernel"
            confidence = "medium"
            evidence.append("impl/rt entry with internal ccmath callee")
        elif likely_thin_forwarder and local_calls:
            category = "ccmath_internal_kernel"
            confidence = "medium"
            evidence.append("thin forwarder to local kernel")
    elif fn_name in ("sqrt", "sqrtf") and hw_sqrt_present:
        category = "hardware_instruction_lowering"
        confidence = "high"
        evidence.append("public sqrt lowers to hardware instruction")
    elif harness_mode == "runtime_no_flatten" and local_calls:
        category = "ccmath_public_wrapper"
        confidence = "medium"
        evidence.append("no-flatten harness shows wrapper with local call")
    elif not fn_name.endswith("_gen") and insn_count > 15 and not external_calls:
        category = "ccmath_runtime_path"
        confidence = "medium"
        evidence.append("substantial runtime body without external calls")
    elif fn_name.endswith("_gen"):
        category = "ccmath_constexpr_path"
        confidence = "low"
        evidence.append("gen entry (constexpr-capable)")

    ld_fmt = path_hints.get("long_double_format")
    if ld_fmt == "ld80" and "powl" in fn_name:
        if category == "ccmath_internal_kernel":
            category = "long_double_ld80_kernel_path"
            evidence.append("long_double_format=ld80 hint")
    elif ld_fmt == "ld64" and "powl" in fn_name:
        category = "long_double_ld64_alias_path"
        evidence.append("long_double_format=ld64 hint")

    if intended and intended != category and category != "unknown":
        if intended == category:
            evidence.append("matches registry intended_path")
        else:
            evidence.append("registry intended_path=%s differs from detected %s"
                            % (intended, category))
            if confidence == "high":
                confidence = "medium"

    if assembly_empty_or_trivial:
        evidence.append("assembly empty or trivial (possible fold or wrong path)")
    if possible_constant_fold:
        evidence.append("possible constant fold: no FP ops in non-trivial function")

    kernel_measured = (
        not external_libm
        and category in (
            "ccmath_internal_kernel", "ccmath_runtime_path",
            "hardware_instruction_lowering", "finite_normal_hot_path",
            "bounded_integer_path", "long_double_ld80_kernel_path",
        )
    )

    return {
        "instruction_count": insn_count,
        "branch_count": branch_count,
        "call_count": call_count,
        "local_calls": local_calls,
        "external_calls": external_calls,
        "external_libm_calls": external_libm,
        "load_store_count": load_store_count,
        "fp_op_count": fp_op_count,
        "fma_count": fma_count,
        "div_sqrt_count": div_sqrt_count,
        "vector_present": vector_present,
        "table_load_hints": table_load_hints,
        "likely_thin_forwarder": likely_thin_forwarder,
        "likely_fully_inlined": likely_fully_inlined,
        "external_libm_call_present": bool(external_libm),
        "hardware_instruction_lowering": hw_sqrt_present,
        "assembly_empty_or_trivial": assembly_empty_or_trivial,
        "possible_constant_fold": possible_constant_fold,
        "kernel_measured": kernel_measured,
        "path_category": category,
        "confidence": confidence,
        "evidence": evidence,
    }


def classify_variant(variant_dir, fn, target):
    """Classify a variant directory. Writes path_analysis.json."""
    region = variant_dir / "region.s"
    meta_path = variant_dir / "emit_meta.json"
    emit_meta = None
    if meta_path.exists():
        emit_meta = json.loads(meta_path.read_text())
    reg = dict(target)
    reg["_fn_name"] = fn
    analysis = analyze_region(region, emit_meta, reg)
    (variant_dir / "path_analysis.json").write_text(
        json.dumps(analysis, indent=2) + "\n")
    return analysis


def main(argv=None):
    ap = __import__("argparse").ArgumentParser(
        description="Classify emitted assembly path for a registered function.")
    ap.add_argument("fn", help="registered function name")
    ap.add_argument("--arch", default="", help="comma list of arches")
    args = ap.parse_args(argv)

    target = C.get_target(args.fn)
    fn_dir = C.OUT_DIR / args.fn
    if not fn_dir.exists():
        C.fail("no emitted variants for '%s'. Run emit first." % args.fn)

    want = set(C.parse_arch_list(args.arch)) if args.arch else None
    count = 0
    for variant_dir in sorted(p for p in fn_dir.iterdir() if p.is_dir()):
        arch_name = variant_dir.name.split("-clang-")[0].split("-gcc-")[0]
        if arch_name not in C.ARCHES:
            continue
        if want and arch_name not in want:
            continue
        a = classify_variant(variant_dir, args.fn, target)
        print("  %s: %s (%s)" % (variant_dir.name, a["path_category"], a["confidence"]))
        count += 1
    return 0 if count else 1


if __name__ == "__main__":
    sys.exit(main())
