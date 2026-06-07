#!/usr/bin/env python3
# Copyright (c) Ian Pike
# Copyright (c) CCMath contributors
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
"""Deep analyze passes built on source maps and emit output."""

import json
from pathlib import Path

import _asmlab_common as C
import analysis_common as AC
import cfg_analysis
import mca_deep
import microarch_model
import opt_remarks
import perf_profile
import reg_pressure
import semantic_analysis
import vectorization


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
    ok, _, err = opt_remarks.emit_opt_record(
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

    cfg = cfg_analysis.build_cfg(variant_dir, source_map)
    mca = mca_deep.build_mca(variant_dir, cfg, arch_name, compiler)
    opt = opt_remarks.build_opt_remarks(variant_dir, source_map)
    rp = reg_pressure.build_reg_pressure(variant_dir, source_map, cfg)
    micro = microarch_model.build_microarch(variant_dir, source_map, mca, cfg)
    vec = vectorization.build_vectorization(variant_dir, source_map, opt)

    baseline_semantic = None
    if baseline_dir:
        baseline_semantic = AC.load_json(Path(baseline_dir) / "semantic.json")

    semantic = semantic_analysis.build_semantic(
        variant_dir, source_map, opt, rp, vec, baseline_semantic)
    perf = perf_profile.build_perf(variant_dir, mca)

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
    import shutil
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
