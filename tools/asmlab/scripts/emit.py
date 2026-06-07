#!/usr/bin/env python3
# Copyright (c) Ian Pike
# Copyright (c) CCMath contributors
#
# CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
# See LICENSE for more information.
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
"""Emit assembly for a registered ccmath function across a microarch matrix.

Generates a harness translation unit from harness_template.cpp + the registry,
compiles it to .s (and a stable disassembly) for each (arch, flags, compiler)
variant, and writes everything under out/asmlab/<fn>/<variant>/.

On the arm64 host, x86 targets are reached with clang --target=x86_64-apple-macos
and -S only (no link, so no x86 sysroot is needed). gcc and Linux-accurate codegen
go through docker/run.sh. This module only drives the local clang path.
"""

import argparse
import json
import sys

import _asmlab_common as C


def _volatile_loads(sig):
    """Reassign parameters via volatile seed so arguments are not manifest constants."""
    lines = []
    for i, ty in enumerate(sig["args"]):
        lines.append("\ta%d = (%s)(asmlab_opaque_seed + %du);" % (i, ty, i + 1))
    return "\n".join(lines)


def render_harness(fn, target):
    sig = target["signature"]
    params = ", ".join("%s a%d" % (ty, i) for i, ty in enumerate(sig["args"]))
    includes = "\n".join('#include "%s"' % h for h in target["includes"])
    mode = target.get("harness_mode") or C.default_harness_mode(fn)
    flatten = mode in ("runtime_flatten", "path_direct")
    flatten_attr = "__attribute__((flatten))" if flatten else ""
    flatten_comment = (
        "__attribute__((flatten)) recursively inlines the callee body into this symbol."
        if flatten else
        "No flatten attribute: observe wrapper shape and out-of-line calls."
    )
    tpl = C.HARNESS_TEMPLATE.read_text()
    return (tpl
            .replace("@@INCLUDES@@", includes)
            .replace("@@RET@@", sig["ret"])
            .replace("@@SYMBOL@@", C.symbol_for(fn))
            .replace("@@PARAMS@@", params)
            .replace("@@EXPR@@", target["expr"])
            .replace("@@FLATTEN_ATTR@@", flatten_attr)
            .replace("@@FLATTEN_COMMENT@@", flatten_comment)
            .replace("@@VOLATILE_LOADS@@", _volatile_loads(sig)))


def emit_variant(fn, target, arch_name, flags_name, compiler, source_map=False, deep_analyze=False):
    arch = C.resolve_arch(arch_name)
    if compiler == "clang" and not C.arch_local_capable(arch):
        print("  [%s] needs a Linux sysroot; run via docker/run.sh (skipped locally)" % arch_name,
              file=sys.stderr)
        return None
    flags = C.FLAG_VARIANTS[flags_name]
    outdir = C.variant_dir(fn, arch_name, flags_name, compiler)
    outdir.mkdir(parents=True, exist_ok=True)

    src = outdir / "harness.cpp"
    src.write_text(render_harness(fn, target))

    cxx = C.cxx_compiler(compiler)
    cmd = [cxx, "-std=" + C.CXX_STD, "-S", "-I", str(C.INCLUDE_DIR)]
    cmd += flags
    if arch["target"]:
        cmd += ["--target=" + arch["target"]]
    cmd += arch["emit_flags"]
    cmd += [str(src), "-o", str(outdir / "asm.s")]

    res = C.run(cmd)
    (outdir / "compile.cmd").write_text(" ".join(cmd) + "\n")
    if res.returncode != 0:
        (outdir / "compile.err").write_text(res.stderr)
        print("  [%s/%s/%s] EMIT FAILED (see compile.err)" % (arch_name, compiler, flags_name),
              file=sys.stderr)
        return None

    # Isolate the analyzed symbol's body, following a tail-call into the real
    # kernel when the wrapper is just a jmp/b to a locally-defined function.
    region, meta = isolate_region(outdir / "asm.s", C.symbol_for(fn))
    (outdir / "region.s").write_text(region)
    mode = target.get("harness_mode") or C.default_harness_mode(fn)
    emit_meta = {
        "function": fn,
        "requested_symbol": C.symbol_for(fn),
        "analyzed_symbol": meta["analyzed_symbol"],
        "follow_chain": meta["follow_chain"],
        "forwarder_followed": meta["forwarder_followed"],
        "harness_mode": mode,
        "arch": arch_name,
        "compiler": compiler,
        "flags": flags_name,
    }
    (outdir / "emit_meta.json").write_text(json.dumps(emit_meta, indent=2) + "\n")

    import classify as classify_mod
    pa = classify_mod.classify_variant(outdir, fn, target)

    if source_map or deep_analyze:
        import source_map as smod
        if not (outdir / "asm_verbose.s").exists():
            smod.emit_verbose_asm(outdir, src, arch, flags, compiler)
        smod.build_source_map(outdir, fn, target, pa)

    if deep_analyze:
        import analysis_pipeline as apipe
        apipe.run_deep_analysis(outdir, fn, target, arch_name, flags_name, compiler)

    followed = meta["analyzed_symbol"] if meta["forwarder_followed"] else None
    note = (" (followed -> %s)" % followed) if followed else ""
    print("  [%s/%s/%s] ok%s -> %s" % (arch_name, compiler, flags_name, note,
                                        outdir.relative_to(C.PROJECT_ROOT)))
    return outdir


# Control-transfer mnemonics whose target may hold the real work: tail jumps (jmp/b)
# and ordinary calls (call/callq/bl). A thin wrapper forwards through one of these.
_TRANSFER_MNEMONICS = ("jmp", "b", "b.", "bl", "call", "callq")
# Callees that are never the kernel: C++ exception/termination runtime helpers.
_HELPER_SUBSTRINGS = ("terminate", "cxa", "_Unwind", "stack_chk")
# A body with more than this many instructions is doing real work, not just forwarding.
_FORWARDER_MAX_INSNS = 8


def _strip_comment(s):
    """Drop trailing assembly comments. Hash-hash (Mach-O x86), semicolon (Mach-O ARM),
    and double-slash (ELF ARM) are comment markers. A bare hash is kept because aarch64
    immediates use hash-imm."""
    for marker in ("##", "//", ";"):
        idx = s.find(marker)
        if idx != -1:
            s = s[:idx]
    return s.rstrip()


def _is_local_label(name):
    """Assembler-internal labels (branch targets, debug anchors) rather than real
    functions: Mach-O uses L..., ELF uses .L..."""
    return name.startswith(".L") or (name.startswith("L") and not name.startswith("_"))


def _parse_symbol_bodies(asm_text):
    """Return {function_label: [body lines]} for every real function in the
    assembly. Internal L.../.L... labels are kept inside the body (they are branch
    targets), not treated as boundaries. Directives and comments are dropped."""
    bodies, current, cur_lines = {}, None, []
    for raw in asm_text.splitlines():
        code = _strip_comment(raw.strip())
        if not code:
            continue
        if code.endswith(":") and not code.startswith("."):
            name = code[:-1]
            if _is_local_label(name):
                if current is not None:
                    cur_lines.append(code)
                continue
            if current is not None:
                bodies[current] = cur_lines
            current = name
            cur_lines = []
            continue
        if current is None:
            continue
        if code.startswith(".cfi_endproc") or code.startswith(".section") or code.startswith(".text"):
            bodies[current] = cur_lines
            current = None
            cur_lines = []
            continue
        if code.startswith("."):
            continue
        cur_lines.append(code)
    if current is not None:
        bodies[current] = cur_lines
    return bodies


def _instr_count(body):
    return sum(1 for ln in body if not ln.endswith(":"))


def _single_local_callee(body, lookup):
    """If the body is a thin forwarder (a small wrapper that transfers to exactly one
    locally-defined symbol via jmp or call), return that symbol. Exception/termination
    helpers are ignored so a cleanup landing pad does not count as a second target."""
    if _instr_count(body) > _FORWARDER_MAX_INSNS:
        return None
    targets = []
    for ln in body:
        parts = ln.split()
        if len(parts) < 2 or parts[0].lower() not in _TRANSFER_MNEMONICS:
            continue
        tgt = parts[1]
        if any(h in tgt for h in _HELPER_SUBSTRINGS):
            continue
        if lookup(tgt):
            targets.append(lookup(tgt))
    uniq = set(targets)
    return targets[0] if len(uniq) == 1 else None


def isolate_region(asm_path, symbol):
    """Extract the analyzed kernel body. Resolves both Mach-O (_sym) and ELF (sym)
    naming and chases thin forwarders (tail jmp or plain call into a locally-defined
    symbol) so large out-of-line kernels are analyzed instead of a lone wrapper. Stops
    once it reaches a body that does real work, or at an external target such as libm
    pow. Returns (text, meta) where meta holds follow_chain and analyzed_symbol."""
    bodies = _parse_symbol_bodies(asm_path.read_text())

    def lookup(name):
        for cand in (name, "_" + name, name.lstrip("_")):
            if cand in bodies:
                return cand
        return None

    cur = lookup(symbol)
    if cur is None:
        return ("## isolation failed: symbol %s not found\n" % symbol,
                {"analyzed_symbol": symbol, "follow_chain": [symbol],
                 "forwarder_followed": False})
    follow_chain = [cur]
    for _ in range(6):
        nxt = _single_local_callee(bodies[cur], lookup)
        if nxt is None or nxt == cur:
            break
        cur = nxt
        follow_chain.append(cur)
    return ("\n".join(bodies[cur]) + "\n",
            {"analyzed_symbol": cur, "follow_chain": follow_chain,
             "forwarder_followed": len(follow_chain) > 1})


def main(argv=None):
    ap = argparse.ArgumentParser(description="Emit ccmath assembly for a registered function.")
    ap.add_argument("fn", help="registered function name (see registry/functions.json)")
    ap.add_argument("--arch", default="", help="comma list of arches (default: %s)" % ",".join(C.DEFAULT_ARCHES))
    ap.add_argument("--flags", default=C.DEFAULT_FLAGS, choices=list(C.FLAG_VARIANTS))
    ap.add_argument("--compiler", default="clang", choices=["clang", "gcc"])
    ap.add_argument("--source-map", action="store_true", help="emit debug asm and source map")
    ap.add_argument("--deep-analyze", action="store_true",
                    help="source map plus CFG, MCA, remarks, pressure, semantic passes")
    # Implied by --deep-analyze. Kept as explicit flag for emit-only workflows.
    args = ap.parse_args(argv)

    target = C.get_target(args.fn)
    arches = C.parse_arch_list(args.arch)
    print("asmlab emit: %s -> %s [%s, %s]" % (args.fn, ",".join(arches), args.compiler, args.flags))
    ok = 0
    if args.deep_analyze:
        args.source_map = True
    for a in arches:
        if emit_variant(args.fn, target, a, args.flags, args.compiler,
                        source_map=args.source_map, deep_analyze=args.deep_analyze):
            ok += 1
    print("emitted %d/%d variants under out/asmlab/%s/" % (ok, len(arches), args.fn))
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main())
