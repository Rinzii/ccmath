#!/usr/bin/env python3
# Copyright (c) Ian Pike
# Copyright (c) CCMath contributors
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
"""Emit and deep-analyze asmlab golden calibration fixtures."""

import json
import sys
from pathlib import Path

import _asmlab_common as C

GOLDEN_REGISTRY = C.ASMLAB_DIR / "registry" / "golden_targets.json"
GOLDEN_HARNESS = C.ASMLAB_DIR / "golden" / "harness_template.cpp"
GOLDEN_OUT = C.OUT_DIR / "golden"


def load_golden_targets():
    data = json.loads(GOLDEN_REGISTRY.read_text())
    return data.get("targets", {})


def get_golden_target(name):
    targets = load_golden_targets()
    if name not in targets:
        C.fail("unknown golden target '%s'. Known: %s" % (name, ", ".join(sorted(targets))))
    return dict(targets[name])


def golden_variant_dir(fn, arch, flags, compiler):
    return GOLDEN_OUT / fn / ("%s-%s-%s" % (arch, compiler, flags))


def fixture_include_dir(target):
    return C.ASMLAB_DIR / target["fixture_dir"]


def render_golden_harness(fn, target):
    sig = target["signature"]
    params = ", ".join("%s a%d" % (ty, i) for i, ty in enumerate(sig["args"]))
    flatten = target.get("harness_mode") == "runtime_flatten"
    flatten_attr = "__attribute__((flatten))" if flatten else ""
    loads = "\n".join(
        "\ta%d = (%s)(asmlab_opaque_seed + %du);" % (i, ty, i + 1)
        for i, ty in enumerate(sig["args"]))
    tpl = GOLDEN_HARNESS.read_text()
    return (tpl
            .replace("@@KERNEL_HEADER@@", target["kernel_header"])
            .replace("@@RET@@", sig["ret"])
            .replace("@@SYMBOL@@", C.symbol_for(fn))
            .replace("@@PARAMS@@", params)
            .replace("@@EXPR@@", target["namespace_call"])
            .replace("@@FLATTEN_ATTR@@", flatten_attr)
            .replace("@@VOLATILE_LOADS@@", loads))


def emit_golden(fn, arch_name, flags_name, compiler, source_map=True, deep_analyze=True):
    import emit as emit_mod
    import classify as classify_mod
    import source_map as smod
    import analysis_pipeline as apipe

    target = get_golden_target(fn)
    arch = C.resolve_arch(arch_name)
    if compiler == "clang" and not C.arch_local_capable(arch):
        print("  [%s] needs Docker (skipped)" % arch_name, file=sys.stderr)
        return None

    outdir = golden_variant_dir(fn, arch_name, flags_name, compiler)
    outdir.mkdir(parents=True, exist_ok=True)
    inc = fixture_include_dir(target)

    src = outdir / "harness.cpp"
    src.write_text(render_golden_harness(fn, target))

    cxx = C.cxx_compiler(compiler)
    flags = C.FLAG_VARIANTS[flags_name]
    cmd = [cxx, "-std=" + C.CXX_STD, "-S", "-I", str(inc)]
    cmd += flags
    if arch["target"]:
        cmd += ["--target=" + arch["target"]]
    cmd += arch["emit_flags"]
    cmd += [str(src), "-o", str(outdir / "asm.s")]

    res = C.run(cmd)
    (outdir / "compile.cmd").write_text(" ".join(cmd) + "\n")
    if res.returncode != 0:
        (outdir / "compile.err").write_text(res.stderr)
        print("  golden emit FAILED: %s" % fn, file=sys.stderr)
        return None

    region, meta = emit_mod.isolate_region(outdir / "asm.s", C.symbol_for(fn))
    (outdir / "region.s").write_text(region)
    emit_meta = {
        "function": fn,
        "golden_fixture": True,
        "fixture_dir": target["fixture_dir"],
        "requested_symbol": C.symbol_for(fn),
        "analyzed_symbol": meta["analyzed_symbol"],
        "follow_chain": meta["follow_chain"],
        "forwarder_followed": meta["forwarder_followed"],
        "harness_mode": target.get("harness_mode"),
        "arch": arch_name,
        "compiler": compiler,
        "flags": flags_name,
    }
    (outdir / "emit_meta.json").write_text(json.dumps(emit_meta, indent=2) + "\n")

    pa = classify_mod.analyze_region(
        outdir / "region.s", emit_meta, dict(target, _fn_name=fn))
    (outdir / "path_analysis.json").write_text(json.dumps(pa, indent=2) + "\n")

    if source_map or deep_analyze:
        smod.emit_verbose_asm(outdir, src, arch, flags, compiler, extra_include_dirs=[inc])
        smod.build_source_map(outdir, fn, target, pa)

    if deep_analyze:
        apipe.run_deep_analysis(
            outdir, fn, target, arch_name, flags_name, compiler,
            record_history=False, extra_include_dirs=[inc])

    return outdir
