#!/usr/bin/env python3
# Copyright (c) Ian Pike
# Copyright (c) CCMath contributors
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
"""Isolated candidate-source variants without editing production headers."""

import hashlib
import json
import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path

import _asmlab_common as C

VARIANT_TARGETS_PATH = C.ASMLAB_DIR / "registry" / "variant_targets.json"
VARIANTS_ROOT = C.OUT_DIR / "variants"


def load_variant_targets():
    return C.load_json_registry(VARIANT_TARGETS_PATH).get("targets", {})


def get_variant_target(fn):
    targets = load_variant_targets()
    if fn not in targets:
        C.fail("no variant target for '%s'. Known: %s" % (fn, ", ".join(sorted(targets))))
    return targets[fn]


def variant_workspace(fn, name):
    return VARIANTS_ROOT / fn / name


def _git_head():
    try:
        return subprocess.check_output(
            ["git", "rev-parse", "HEAD"], text=True, stderr=subprocess.DEVNULL
        ).strip()
    except (subprocess.CalledProcessError, FileNotFoundError):
        return "unknown"


def _content_hash(text):
    return hashlib.sha256(text.encode()).hexdigest()[:16]


def _rewrite_powf_impl(content, variant_name):
    """Keep ccm::gen::impl::internal::impl for data-table linkage. Add asmlab entry."""
    import re
    content = re.sub(
        r"\n\tconstexpr float powf_impl\(float base, float exp\) noexcept\s*\{"
        r"\s*return internal::impl::powf_impl\(base, exp\);\s*"
        r"(//[^\n]*\n\s*)?\}",
        "",
        content,
        count=1,
    )
    entry = (
        "\nnamespace asmlab { namespace variant { namespace powf_impl { "
        "namespace %s {\n"
        "float powf_impl(float base, float exp) noexcept {\n"
        "\treturn ::ccm::gen::impl::internal::impl::powf_impl(base, exp);\n"
        "}\n"
        "} } } }\n" % variant_name
    )
    if "} // namespace ccm::gen::impl" in content:
        content = content.replace(
            "} // namespace ccm::gen::impl",
            "} // namespace ccm::gen::impl" + entry,
        )
    else:
        content = content + entry
    return content


def _rewrite_sqrt_rt(content, variant_name):
    """Rewrite ccm::rt sqrt_rt into isolated variant namespace."""
    marker = "namespace ccm::rt::simd_impl"
    idx = content.find(marker)
    if idx == -1:
        C.fail("sqrt_rt candidate rewrite failed: missing ccm::rt::simd_impl")
    header = content[:idx]
    body = content[idx:]
    body = body.replace("namespace ccm::rt::simd_impl", "namespace ccm_rt_simd_impl")
    body = body.replace("} // namespace ccm::rt::simd_impl", "} // namespace ccm_rt_simd_impl")
    body = body.replace("namespace ccm::rt\n{", "namespace ccm_rt {")
    body = body.replace("} // namespace ccm::rt", "} // namespace ccm_rt")
    body = body.replace("simd_impl::", "ccm_rt_simd_impl::")
    open_ns = "namespace asmlab { namespace variant { namespace sqrt { namespace %s {\n" % (
        variant_name)
    close_ns = "\n} } } } // namespace asmlab variant sqrt\n"
    return header + open_ns + body + close_ns


def _candidate_header(fn, variant_name, base_file, base_commit):
    return """/*
 * asmlab candidate copy for experimentation.
 * Do not edit production source until this candidate passes validation.
 * Base source: %s
 * Base commit: %s
 * Function: %s
 * Variant: %s
 */

""" % (base_file, base_commit, fn, variant_name)


def create_candidate(fn, variant_name, force=False):
    vdir = variant_workspace(fn, variant_name)
    if vdir.exists() and not force:
        C.fail("variant '%s' already exists at %s (use --force)" % (variant_name, vdir))

    vdir.mkdir(parents=True, exist_ok=True)
    vcfg = get_variant_target(fn)
    base_rel = vcfg["base_file"]
    base_path = C.PROJECT_ROOT / base_rel
    if not base_path.exists():
        C.fail("base file not found: %s" % base_path)

    raw = base_path.read_text()
    base_commit = _git_head()
    base_hash = _content_hash(raw)

    rewrite = vcfg.get("namespace_rewrite", fn)
    if rewrite == "powf_impl":
        body = _rewrite_powf_impl(raw, variant_name)
    elif rewrite == "sqrt_rt":
        body = _rewrite_sqrt_rt(raw, variant_name)
    else:
        C.fail("unsupported namespace_rewrite: %s" % rewrite)

    candidate_path = vdir / "candidate.hpp"
    candidate_path.write_text(_candidate_header(fn, variant_name, base_rel, base_commit) + body)

    manifest = {
        "function": fn,
        "variant": variant_name,
        "base_file": base_rel,
        "base_git_commit": base_commit,
        "base_content_hash": base_hash,
        "candidate_file": str(candidate_path.relative_to(C.PROJECT_ROOT)),
        "created_at": datetime.now(timezone.utc).isoformat(),
        "source_kind": "candidate",
        "status": "experimental",
        "candidate_call": vcfg["candidate_call"].replace("{variant}", variant_name),
        "notes": "Edit candidate.hpp in this directory only.",
    }
    (vdir / "manifest.json").write_text(json.dumps(manifest, indent=2) + "\n")
    (vdir / "notes.md").write_text(
        "# Variant %s / %s\n\nEdit `candidate.hpp` here. Run scenario report to test.\n" % (
            fn, variant_name))
    return vdir, manifest


def load_manifest(fn, variant_name):
    mpath = variant_workspace(fn, variant_name) / "manifest.json"
    if not mpath.exists():
        C.fail("variant not initialized: run variant init %s --name %s" % (fn, variant_name))
    return json.loads(mpath.read_text())


def check_staleness(manifest):
    base_path = C.PROJECT_ROOT / manifest["base_file"]
    if not base_path.exists():
        return {"staleness": "unknown", "warning": "base file missing"}
    current_hash = _content_hash(base_path.read_text())
    stale = current_hash != manifest.get("base_content_hash")
    return {
        "staleness": "stale" if stale else "fresh",
        "base_content_hash": manifest.get("base_content_hash"),
        "current_content_hash": current_hash,
        "warning": "variant is stale relative to base source" if stale else None,
    }


def generate_patch(fn, variant_name, apply=False):
    manifest = load_manifest(fn, variant_name)
    stale = check_staleness(manifest)
    if stale["staleness"] == "stale":
        print("WARNING: %s" % stale["warning"], file=sys.stderr)

    import difflib
    base_path = C.PROJECT_ROOT / manifest["base_file"]
    cand_path = variant_workspace(fn, variant_name) / "candidate.hpp"
    base_lines = base_path.read_text().splitlines(keepends=True)
    # Compare against body only (skip candidate header comment block)
    cand_text = cand_path.read_text()
    cand_body = cand_text.split("*/\n", 1)[-1] if "*/\n" in cand_text else cand_text
    cand_lines = cand_body.splitlines(keepends=True)

    diff = difflib.unified_diff(
        base_lines, cand_lines,
        fromfile=manifest["base_file"],
        tofile=manifest["candidate_file"],
    )
    patch_text = "".join(diff)
    patch_path = variant_workspace(fn, variant_name) / "patch.diff"
    patch_path.write_text(patch_text if patch_text else (
        "# No diff (candidate body matches base after namespace rewrite only)\n"))
    print("Patch generated but not applied: %s" % patch_path.relative_to(C.PROJECT_ROOT))
    if apply:
        print("ERROR: --apply not implemented; apply patch manually after review", file=sys.stderr)
        return 1
    return 0


def render_variant_harness(fn, variant_name, scenario_rec=None):
    """Build harness.cpp that calls the candidate symbol, not production code."""
    import scenario as scenario_mod

    manifest = load_manifest(fn, variant_name)
    target = dict(C.get_target(fn))
    if scenario_rec:
        target = scenario_mod._scenario_target(fn, scenario_rec)

    vcfg = get_variant_target(fn)
    expr = manifest.get("candidate_call") or vcfg["candidate_call"].replace(
        "{variant}", variant_name)

    sig = target["signature"]
    params = ", ".join("%s a%d" % (ty, i) for i, ty in enumerate(sig["args"]))
    mode = target.get("harness_mode") or C.default_harness_mode(fn)
    flatten = mode in ("runtime_flatten", "path_direct")
    flatten_attr = "__attribute__((flatten))" if flatten else ""

    prelude = ""
    loads = ""
    if scenario_rec:
        prelude = scenario_mod._scenario_prelude()
        loads = scenario_mod._scenario_loads(sig, scenario_rec.get("inputs", {}))
    else:
        loads = "\n".join(
            "\ta%d = (%s)(asmlab_opaque_seed + %du);" % (i, ty, i + 1)
            for i, ty in enumerate(sig["args"]))

    return """/*
 * asmlab variant harness (candidate)
 * variant: %s
 * Do not edit include/ccmath for experiments.
 */
#include "candidate.hpp"
%s
#if defined(__GNUC__) || defined(__clang__)
#define ASMLAB_FLATTEN %s
#else
#define ASMLAB_FLATTEN
#endif

extern "C" volatile unsigned long long asmlab_opaque_seed;

extern "C" ASMLAB_FLATTEN %s %s(%s)
{
%s
	return %s;
}
""" % (
        variant_name,
        prelude,
        flatten_attr,
        sig["ret"],
        C.symbol_for(fn),
        params,
        loads,
        expr,
    )


def emit_variant(fn, variant_name, arch_name, flags_name, compiler, source_map=False,
                 scenario_name=None):
    """Emit assembly for a candidate variant (optionally under a scenario)."""
    import scenario as scenario_mod

    manifest = load_manifest(fn, variant_name)
    stale = check_staleness(manifest)
    if stale["staleness"] == "stale":
        print("WARNING: %s" % stale["warning"], file=sys.stderr)

    scenario_rec = None
    if scenario_name:
        scenario_rec = C.get_scenario(fn, scenario_name)

    arch = C.resolve_arch(arch_name)
    if compiler == "clang" and not C.arch_local_capable(arch):
        print("  [%s] needs Docker (skipped)" % arch_name, file=sys.stderr)
        return None

    if scenario_name:
        outdir = C.scenario_dir(fn, scenario_name, arch_name, flags_name, compiler,
                                variant=variant_name)
    else:
        outdir = VARIANTS_ROOT / fn / variant_name / "emit" / (
            "%s-%s-%s" % (arch_name, compiler, flags_name))
    outdir.mkdir(parents=True, exist_ok=True)

    harness = render_variant_harness(fn, variant_name, scenario_rec)
    src = outdir / "harness.cpp"
    src.write_text(harness)

    cxx = C.cxx_compiler(compiler)
    flags = C.FLAG_VARIANTS[flags_name]
    vdir = variant_workspace(fn, variant_name)
    cmd = [cxx, "-std=" + C.CXX_STD, "-S", "-I", str(C.INCLUDE_DIR), "-I", str(vdir)]
    cmd += flags
    if arch["target"]:
        cmd += ["--target=" + arch["target"]]
    cmd += arch["emit_flags"]
    cmd += [str(src), "-o", str(outdir / "asm.s")]

    res = C.run(cmd)
    (outdir / "compile.cmd").write_text(" ".join(cmd) + "\n")
    if res.returncode != 0:
        (outdir / "compile.err").write_text(res.stderr)
        print("  variant emit FAILED:\n%s" % res.stderr[:800], file=sys.stderr)
        return None

    region, meta = emit_mod.isolate_region(outdir / "asm.s", C.symbol_for(fn))
    (outdir / "region.s").write_text(region)

    target = scenario_mod._scenario_target(fn, scenario_rec) if scenario_rec else C.get_target(fn)
    emit_meta = {
        "function": fn,
        "variant": variant_name,
        "scenario": scenario_name,
        "source_kind": "candidate",
        "candidate_file": manifest["candidate_file"],
        "base_file": manifest["base_file"],
        "base_git_commit": manifest["base_git_commit"],
        "staleness": stale["staleness"],
        "requested_symbol": C.symbol_for(fn),
        "analyzed_symbol": meta["analyzed_symbol"],
        "follow_chain": meta["follow_chain"],
        "candidate_call": manifest.get("candidate_call"),
        "harness_mode": target.get("harness_mode"),
        "arch": arch_name,
        "compiler": compiler,
        "flags": flags_name,
    }
    (outdir / "emit_meta.json").write_text(json.dumps(emit_meta, indent=2) + "\n")

    import classify as classify_mod
    import analyze as analyze_mod
    import source_map as smod

    pa = classify_mod.analyze_region(outdir / "region.s", emit_meta, dict(target, _fn_name=fn))
    (outdir / "path_analysis.json").write_text(json.dumps(pa, indent=2) + "\n")

    if source_map:
        smod.emit_verbose_asm(outdir, src, arch, flags, compiler,
                              extra_include_dirs=[vdir])
        smod.build_source_map(outdir, fn, target, pa)
        if scenario_name and (outdir / "source_map.json").exists():
            import shutil
            shutil.copy2(outdir / "source_map.json", outdir / "scenario_source_map.json")
            if (outdir / "source_map.md").exists():
                shutil.copy2(outdir / "source_map.md", outdir / "scenario_source_map.md")

    metrics = analyze_mod.analyze_variant(outdir, arch_name)
    if scenario_name:
        import shutil
        (outdir / "scenario_path_analysis.json").write_text(json.dumps(pa, indent=2) + "\n")
        if metrics and (outdir / "metrics.json").exists():
            shutil.copy2(outdir / "metrics.json", outdir / "scenario_metrics.json")
        scenario_rec = C.get_scenario(fn, scenario_name)
        (outdir / "scenario_meta.json").write_text(json.dumps({
            "function": fn,
            "scenario": scenario_name,
            "variant": variant_name,
            "source_kind": "candidate",
            "scenario_rec": {k: v for k, v in scenario_rec.items() if k != "inputs"},
            "inputs": scenario_rec.get("inputs"),
        }, indent=2) + "\n")
    return outdir


def run_variant_scenario_pipeline(fn, variant_name, scenario_name, arches, flags, compiler,
                                  source_map=False):
    """Emit and report candidate variant under a path scenario."""
    import scenario as scenario_mod

    stale = check_staleness(load_manifest(fn, variant_name))
    if stale["staleness"] == "stale":
        print("WARNING: %s" % stale["warning"], file=sys.stderr)

    results = []
    for arch in arches:
        sdir = emit_variant(fn, variant_name, arch, flags, compiler,
                            source_map=source_map, scenario_name=scenario_name)
        if not sdir:
            continue
        _, rep = scenario_mod.write_scenario_report(
            fn, scenario_name, arch, flags, compiler, variant=variant_name)
        results.append((arch, sdir, rep))
        if rep:
            print("  [%s] candidate scenario %s path=%s match=%s staleness=%s" % (
                arch, scenario_name, rep.get("observed_path"),
                rep.get("path_match"), rep.get("staleness")))
    return results


# Late import to avoid circular dependency at module load for isolate_region
import emit as emit_mod
