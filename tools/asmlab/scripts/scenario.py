#!/usr/bin/env python3
# Copyright (c) Ian Pike
# Copyright (c) CCMath contributors
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
"""Path scenario testing: isolate and verify implementation subpaths."""

import json
import shutil
import sys
from pathlib import Path

import _asmlab_common as C

# Reuse emit/classify/analyze/source_map from sibling modules.
import emit as emit_mod
import classify as classify_mod
import analyze as analyze_mod
import source_map as smod
import asm_diff as adiff_mod
import provenance as prov_mod


def _scenario_target(fn, scenario_rec):
    """Merge registry function entry with scenario overrides."""
    target = dict(C.get_target(fn))
    if scenario_rec.get("harness_mode"):
        target["harness_mode"] = scenario_rec["harness_mode"]
    target["intended_path"] = scenario_rec.get("intended_path", target.get("intended_path"))
    target["_scenario"] = scenario_rec.get("scenario")
    return target


def _format_input_literal(ty, value):
    if isinstance(value, str) and value.lower() == "nan":
        if ty == "float":
            return "asmlab_load_float(0x7fc00000u)"
        if ty == "double":
            return "asmlab_load_double(0x7ff8000000000000ull)"
        if ty == "long double":
            return "asmlab_load_double(0x7ff8000000000000ull)"
    if ty == "float":
        return "asmlab_load_float(%sf)" % value
    if ty == "double":
        return "asmlab_load_double(%s)" % value
    if ty == "long double":
        return "asmlab_load_double((long double)%s)" % value
    return "(%s)(%s)" % (ty, value)


def _scenario_prelude():
    return """
#include <cstring>

static inline float asmlab_load_float(float v) {
    volatile float storage = v;
    return storage;
}
static inline float asmlab_load_float(unsigned int bits) {
    volatile unsigned int b = bits;
    float out;
    std::memcpy(&out, (const void*)&b, sizeof(out));
    return out;
}
static inline double asmlab_load_double(double v) {
    volatile double storage = v;
    return storage;
}
static inline double asmlab_load_double(unsigned long long bits) {
    volatile unsigned long long b = bits;
    double out;
    std::memcpy(&out, (const void*)&b, sizeof(out));
    return out;
}
"""


def _scenario_loads(sig, inputs):
    """Load scenario inputs without manifest-constant folding of the call arguments."""
    lines = []
    for i, ty in enumerate(sig["args"]):
        key = "a%d" % i
        val = inputs.get(key)
        if val is None:
            lines.append("\ta%d = (%s)(asmlab_opaque_seed + %du);" % (i, ty, i + 1))
        else:
            lit = _format_input_literal(ty, val)
            lines.append(
                "\ta%d = (%s)((asmlab_opaque_seed - asmlab_opaque_seed) + %s);" % (i, ty, lit))
    return "\n".join(lines)


def render_scenario_harness(fn, target, scenario_rec):
    sig = target["signature"]
    params = ", ".join("%s a%d" % (ty, i) for i, ty in enumerate(sig["args"]))
    includes = "\n".join('#include "%s"' % h for h in target["includes"])
    mode = target.get("harness_mode") or C.default_harness_mode(fn)
    flatten = mode in ("runtime_flatten", "path_direct")
    flatten_attr = "__attribute__((flatten))" if flatten else ""
    flatten_comment = (
        "Scenario harness with fixed volatile inputs for path slice '%s'. "
        "Static assembly reflects selected inputs, not dynamic branch frequency."
        % scenario_rec.get("scenario", "")
    )
    tpl = C.HARNESS_TEMPLATE.read_text()
    body = (tpl
            .replace("@@INCLUDES@@", includes + _scenario_prelude())
            .replace("@@RET@@", sig["ret"])
            .replace("@@SYMBOL@@", C.symbol_for(fn))
            .replace("@@PARAMS@@", params)
            .replace("@@EXPR@@", target["expr"])
            .replace("@@FLATTEN_ATTR@@", flatten_attr)
            .replace("@@FLATTEN_COMMENT@@", flatten_comment)
            .replace("@@VOLATILE_LOADS@@", _scenario_loads(sig, scenario_rec.get("inputs", {}))))
    return body


def _path_match(scenario_rec, path_analysis):
    observed = path_analysis.get("path_category", "unknown")
    intended = scenario_rec.get("intended_path", "unknown")
    acceptable = scenario_rec.get("acceptable_paths") or [intended]
    match = observed in acceptable
    return {
        "intended_path": intended,
        "observed_path": observed,
        "acceptable_paths": acceptable,
        "path_match": match,
        "path_confidence": path_analysis.get("confidence", "unknown"),
        "path_mismatch": not match,
    }


FORCEABILITY_DISPLAY = {
    "forced_runtime_input": "actually forced",
    "opaque_runtime_input": "runtime-opaque but advisory",
    "constant_input_probe": "compile-time constant probe",
    "static_only_slice": "static-only",
    "input_profile_advisory": "input profile advisory",
    "unknown": "unknown",
}


def _forceability_note(scenario_rec):
    fb = scenario_rec.get("forceability", "unknown")
    display = FORCEABILITY_DISPLAY.get(fb, fb)
    notes = ["forceability category: %s (%s)" % (fb, display)]
    if fb == "constant_input_probe":
        notes.append(
            "WARNING: literal scenario inputs may be specialized or folded unless "
            "opaque runtime input handling prevents it.")
    elif fb == "input_profile_advisory":
        notes.append(
            "WARNING: scenario inputs are advisory; static assembly does not prove "
            "dynamic path selection.")
    return notes


def _gate_status(fn, scenario_rec):
    gate = scenario_rec.get("accuracy_gate")
    labels = scenario_rec.get("accuracy_labels") or []
    manifest = C.load_accuracy_manifest()
    supported = False
    for label in labels:
        if label in manifest:
            supported = True
            break
    if fn in manifest:
        supported = True
    if not gate:
        return "not_applicable"
    if not supported:
        return "advisory"
    return "supported"


def _bench_status(fn, scenario_rec, sdir=None):
    profile = scenario_rec.get("benchmark_profile")
    if not profile:
        return "not_configured"
    if sdir is not None:
        bp = Path(sdir) / "scenario_bench.json"
        if bp.exists():
            import bench_impl as bench_impl_mod
            return bench_impl_mod.bench_status_from_result(json.loads(bp.read_text()))
    profiles = C.load_benchmark_profiles()
    targets = profiles.get("benchmark_targets", {})
    t = targets.get(fn, {})
    if not t.get("wired"):
        return "unsupported"
    fn_profiles = profiles.get("function_profiles", {}).get(fn, [])
    if profile not in fn_profiles:
        return "not_configured"
    return "pending"


def emit_scenario(fn, scenario_name, arch_name, flags_name, compiler,
                  source_map=False, source_kind="original", variant=None):
    if variant:
        import variant as variant_mod
        return variant_mod.emit_variant(
            fn, variant, arch_name, flags_name, compiler,
            source_map=source_map, scenario_name=scenario_name)

    scenario_rec = C.get_scenario(fn, scenario_name)
    arch = C.resolve_arch(arch_name)
    if compiler == "clang" and not C.arch_local_capable(arch):
        print("  [%s] needs Docker (skipped locally)" % arch_name, file=sys.stderr)
        return None

    target = _scenario_target(fn, scenario_rec)
    outdir = C.scenario_dir(fn, scenario_name, arch_name, flags_name, compiler,
                            source_kind=source_kind, variant=variant)
    outdir.mkdir(parents=True, exist_ok=True)

    src = outdir / "harness.cpp"
    src.write_text(render_scenario_harness(fn, target, scenario_rec))

    cxx = C.cxx_compiler(compiler)
    flags = C.FLAG_VARIANTS[flags_name]
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
        print("  [%s] scenario emit FAILED" % arch_name, file=sys.stderr)
        return None

    region, meta = emit_mod.isolate_region(outdir / "asm.s", C.symbol_for(fn))
    (outdir / "region.s").write_text(region)
    emit_meta = {
        "function": fn,
        "scenario": scenario_name,
        "source_kind": "candidate" if variant else source_kind,
        "variant": variant,
        "requested_symbol": C.symbol_for(fn),
        "analyzed_symbol": meta["analyzed_symbol"],
        "follow_chain": meta["follow_chain"],
        "forwarder_followed": meta["forwarder_followed"],
        "harness_mode": target.get("harness_mode"),
        "scenario_inputs": scenario_rec.get("inputs"),
        "input_profile": scenario_rec.get("input_profile"),
        "forceability": scenario_rec.get("forceability"),
        "arch": arch_name,
        "compiler": compiler,
        "flags": flags_name,
    }
    (outdir / "emit_meta.json").write_text(json.dumps(emit_meta, indent=2) + "\n")

    pa = classify_mod.analyze_region(
        outdir / "region.s", emit_meta, dict(target, _fn_name=fn))
    (outdir / "path_analysis.json").write_text(json.dumps(pa, indent=2) + "\n")
    (outdir / "scenario_path_analysis.json").write_text(json.dumps(pa, indent=2) + "\n")

    if source_map:
        if not (outdir / "asm_verbose.s").exists():
            smod.emit_verbose_asm(outdir, src, arch, flags, compiler)
        smap = smod.build_source_map(outdir, fn, target, pa)
        shutil.copy2(outdir / "source_map.json", outdir / "scenario_source_map.json")
        if (outdir / "source_map.md").exists():
            shutil.copy2(outdir / "source_map.md", outdir / "scenario_source_map.md")

    metrics = analyze_mod.analyze_variant(outdir, arch_name)
    if metrics:
        shutil.copy2(outdir / "metrics.json", outdir / "scenario_metrics.json")

    (outdir / "scenario_meta.json").write_text(json.dumps({
        "function": fn,
        "scenario": scenario_name,
        "scenario_rec": {k: v for k, v in scenario_rec.items() if k != "inputs"},
        "inputs": scenario_rec.get("inputs"),
    }, indent=2) + "\n")

    return outdir


def write_scenario_report(fn, scenario_name, arch_name, flags, compiler,
                          source_kind="original", variant=None):
    sdir = C.scenario_dir(fn, scenario_name, arch_name, flags, compiler,
                          source_kind=source_kind, variant=variant)
    if not sdir.exists():
        return None, "scenario not emitted: %s" % sdir

    scenario_rec = C.get_scenario(fn, scenario_name)
    pa_path = sdir / "path_analysis.json"
    pa = json.loads(pa_path.read_text()) if pa_path.exists() else {}
    path_info = _path_match(scenario_rec, pa)

    metrics = {}
    mp = sdir / "scenario_metrics.json"
    if not mp.exists():
        mp = sdir / "metrics.json"
    if mp.exists():
        metrics = json.loads(mp.read_text())

    mca = metrics.get("llvm_mca", {})
    smap_avail = (sdir / "scenario_source_map.json").exists() or (sdir / "source_map.json").exists()

    bench_data = {}
    bp = sdir / "scenario_bench.json"
    if bp.exists():
        bench_data = json.loads(bp.read_text())

    manifest_info = {}
    staleness = {"staleness": "n/a"}
    if variant:
        import variant as variant_mod
        manifest_info = variant_mod.load_manifest(fn, variant)
        staleness = variant_mod.check_staleness(manifest_info)

    forceability = scenario_rec.get("forceability", "unknown")
    report = {
        "function": fn,
        "scenario": scenario_name,
        "scenario_kind": scenario_rec.get("scenario_kind", "mixed"),
        "source_kind": "candidate" if variant else source_kind,
        "variant": variant,
        "arch": arch_name,
        "compiler": compiler,
        "flags": flags,
        "input_profile": scenario_rec.get("input_profile"),
        "input_rationale": scenario_rec.get("input_rationale"),
        "forceability": forceability,
        "forceability_display": FORCEABILITY_DISPLAY.get(forceability, forceability),
        "forceability_notes": _forceability_note(scenario_rec),
        "static_path_note": (
            "Static path slice based on selected harness inputs; not a dynamic trace."
        ),
        "base_file": manifest_info.get("base_file") if variant else None,
        "base_git_commit": manifest_info.get("base_git_commit") if variant else None,
        "candidate_file": manifest_info.get("candidate_file") if variant else None,
        "staleness": staleness.get("staleness") if variant else "n/a",
        "staleness_warning": staleness.get("warning") if variant else None,
        "intended_path": path_info["intended_path"],
        "observed_path": path_info["observed_path"],
        "acceptable_paths": path_info["acceptable_paths"],
        "path_match": path_info["path_match"],
        "path_mismatch": path_info["path_mismatch"],
        "path_confidence": path_info["path_confidence"],
        "expected_external_calls": scenario_rec.get("expected_external_calls"),
        "observed_external_libm": pa.get("external_libm_calls", []),
        "accuracy_gate_status": _gate_status(fn, scenario_rec),
        "production_accuracy": C.production_accuracy_field(fn, scenario_rec),
        "candidate_accuracy": C.candidate_accuracy_field(fn, variant=variant),
        "benchmark_status": bench_data.get("status") or _bench_status(fn, scenario_rec, sdir=sdir),
        "benchmark_profile_name": scenario_rec.get("benchmark_profile"),
        "benchmark_result_ns": bench_data.get("result_ns"),
        "benchmark_target": bench_data.get("benchmark_target"),
        "benchmark_filter": bench_data.get("benchmark_filter"),
        "benchmark_reason": bench_data.get("reason"),
        "static_model_advisory": {
            "block_rthroughput": mca.get("block_rthroughput"),
            "ipc": mca.get("ipc"),
            "instructions": mca.get("instructions"),
        },
        "source_map": "available" if smap_avail else "unavailable",
        "valid_performance_comparison": path_info["path_match"],
        "path_analysis": pa,
    }

    import cpu_knowledge as cpu_kb
    report["cpu_knowledge"] = cpu_kb.annotate_scenario_report(report, pa, sdir)

    prov = prov_mod.collect(fn, flags, compiler, [arch_name])
    prov["scenario"] = scenario_name
    prov["source_kind"] = report["source_kind"]
    prov["variant"] = variant
    report["run"] = prov
    (sdir / "scenario_run.json").write_text(json.dumps(prov, indent=2) + "\n")
    (sdir / "scenario_report.json").write_text(json.dumps(report, indent=2) + "\n")

    md = _render_scenario_md(report)
    (sdir / "scenario_report.md").write_text(md + "\n")
    return sdir, report


def _render_scenario_md(report):
    lines = [
        "# Scenario report: %s / %s" % (report["function"], report["scenario"]),
        "",
        "- function: %s" % report["function"],
        "- scenario: %s" % report["scenario"],
        "- scenario kind: %s" % report.get("scenario_kind"),
        "- source kind: %s" % report.get("source_kind"),
        "- variant: %s" % (report.get("variant") or "none"),
        "- arch: %s" % report.get("arch"),
        "- input profile: %s" % report.get("input_profile"),
        "- forceability: %s (%s)" % (
            report.get("forceability"), report.get("forceability_display")),
        "",
    ]
    if report.get("source_kind") == "candidate":
        lines.extend([
            "- candidate file: %s" % report.get("candidate_file"),
            "- base source file: %s" % report.get("base_file"),
            "- base commit: %s" % report.get("base_git_commit"),
            "- staleness: %s" % report.get("staleness"),
            "",
        ])
        if report.get("staleness_warning"):
            lines.extend(["## STALENESS WARNING", "", report["staleness_warning"], ""])
    for note in report.get("forceability_notes", []):
        lines.append("- %s" % note)
    lines.extend([
        "",
        "## Path",
        "",
        "- intended path: %s" % report.get("intended_path"),
        "- observed path: %s" % report.get("observed_path"),
        "- path confidence: %s" % report.get("path_confidence"),
        "- path match: %s" % report.get("path_match"),
        "",
    ])
    if report.get("path_mismatch"):
        lines.extend([
            "## PATH MISMATCH",
            "",
            "  intended: %s" % report.get("intended_path"),
            "  observed: %s" % report.get("observed_path"),
            "  acceptable: %s" % ", ".join(report.get("acceptable_paths", [])),
            "",
            "This scenario is not a valid performance comparison until the path matches.",
            "",
        ])
    lines.extend([
        "## Static path note",
        "",
        report.get("static_path_note", ""),
        "",
        "## Gates",
        "",
        "- production accuracy simple: %s" % (
            (report.get("production_accuracy") or {}).get("simple", "-")),
        "- production accuracy rigorous: %s" % (
            (report.get("production_accuracy") or {}).get("rigorous", "-")),
        "- candidate accuracy simple: %s" % (
            (report.get("candidate_accuracy") or {}).get("simple",
                                                         (report.get("candidate_accuracy") or {}).get("status", "-"))),
        "- candidate accuracy rigorous: %s" % (
            (report.get("candidate_accuracy") or {}).get("rigorous", "-")),
        "- accuracy gate (legacy label): %s" % report.get("accuracy_gate_status"),
        "- benchmark profile: %s" % (report.get("benchmark_profile_name") or "-"),
        "- benchmark status: %s" % report.get("benchmark_status"),
        "- benchmark result: %s ns" % (
            report.get("benchmark_result_ns") if report.get("benchmark_result_ns") is not None
            else "-"),
        "- benchmark target: %s" % (report.get("benchmark_target") or "-"),
        "- source map: %s" % report.get("source_map"),
        "",
        "## Static model (advisory)",
        "",
    ])
    sm = report.get("static_model_advisory", {})
    lines.append("- throughput estimate: %s" % sm.get("block_rthroughput"))
    lines.append("- ipc: %s" % sm.get("ipc"))
    lines.append("- instructions: %s" % sm.get("instructions"))
    lines.append("")
    if report.get("cpu_knowledge"):
        import cpu_knowledge as cpu_kb
        lines.extend(cpu_kb.render_knowledge_md(report["cpu_knowledge"]))
    return "\n".join(lines)


def run_scenario_bench_for_emit(fn, scenario_name, arch, flags, compiler, variant=None):
    """Run isolated impl benchmark and attach scenario_bench.json next to scenario emit."""
    import bench_impl as bench_impl_mod
    scenario_rec = C.get_scenario(fn, scenario_name)
    profile = scenario_rec.get("benchmark_profile")
    if not profile:
        return {"status": "not_configured", "reason": "no benchmark_profile on scenario"}
    sdir = C.scenario_dir(fn, scenario_name, arch, flags, compiler, variant=variant)
    if fn == "powf_impl":
        result = bench_impl_mod.run_powf_impl_bench(profile, variant=variant)
    else:
        result = bench_impl_mod.run_scenario_bench(fn, scenario_name, scenario_rec, variant=variant)
        return result
    result["scenario"] = scenario_name
    result["arch"] = arch
    sdir.mkdir(parents=True, exist_ok=True)
    (sdir / "scenario_bench.json").write_text(json.dumps(result, indent=2) + "\n")
    bench_impl_mod.write_bench_artifact(result, scenario_name=scenario_name)
    return result


def run_scenario_pipeline(fn, scenario_name, arches, flags, compiler, source_map=False,
                          source_kind="original", variant=None, write_report=True,
                          run_bench=False):
    scenario_rec = C.get_scenario(fn, scenario_name)
    results = []
    for arch in arches:
        sdir = emit_scenario(fn, scenario_name, arch, flags, compiler,
                             source_map=source_map, source_kind=source_kind,
                             variant=variant)
        if not sdir:
            continue
        rep = None
        bench_res = None
        if run_bench and sdir:
            bench_res = run_scenario_bench_for_emit(
                fn, scenario_name, arch, flags, compiler, variant=variant)
        if write_report:
            _, rep = write_scenario_report(fn, scenario_name, arch, flags, compiler,
                                           source_kind=source_kind, variant=variant)
            if bench_res and rep:
                rep["benchmark_status"] = bench_res.get("status", rep.get("benchmark_status"))
                rep["benchmark_result_ns"] = bench_res.get("result_ns")
                rep["benchmark_target"] = bench_res.get("benchmark_target")
                (sdir / "scenario_report.json").write_text(json.dumps(rep, indent=2) + "\n")
        results.append((arch, sdir, rep))
        match = rep.get("path_match") if rep else None
        print("  [%s] scenario %s path=%s match=%s" % (
            arch, scenario_name,
            rep.get("observed_path") if rep else "?",
            match))
    return results


def save_scenario_baseline(fn, scenario_name, arch, flags, compiler):
    sdir = C.scenario_dir(fn, scenario_name, arch, flags, compiler, source_kind="original")
    base_root = C.OUT_DIR / "baselines" / fn / "scenarios" / scenario_name / "original"
    base_root.mkdir(parents=True, exist_ok=True)
    dest = base_root / ("%s-%s-%s" % (arch, compiler, flags))
    if sdir.exists():
        if dest.exists():
            shutil.rmtree(dest)
        shutil.copytree(sdir, dest)
    return dest


def _load_scenario_bench(fn, scenario_name, arch, flags, compiler, variant=None):
    sdir = C.scenario_dir(fn, scenario_name, arch, flags, compiler, variant=variant)
    bp = sdir / "scenario_bench.json"
    if bp.exists():
        return json.loads(bp.read_text())
    return {"status": "pending"}


def scenario_diff(fn, scenario_name, arch, flags, compiler, variant=None):
    cur = C.scenario_dir(fn, scenario_name, arch, flags, compiler, source_kind="original")
    if variant:
        cur_v = C.scenario_dir(fn, scenario_name, arch, flags, compiler,
                               variant=variant)
        base = cur
        cur = cur_v
        diff_kind = "original_vs_variant"
    else:
        base = C.OUT_DIR / "baselines" / fn / "scenarios" / scenario_name / "original"
        base = base / ("%s-%s-%s" % (arch, compiler, flags))
        diff_kind = "baseline_vs_current"

    if not cur.exists():
        return {"error": "current scenario not emitted"}
    if not base.exists():
        return {"error": "baseline missing; run scenario baseline first"}

    def load_json(path):
        return json.loads(path.read_text()) if path.exists() else None

    cur_rep = load_json(cur / "scenario_report.json")
    base_rep = load_json(base / "scenario_report.json")
    cur_pa = load_json(cur / "path_analysis.json")
    base_pa = load_json(base / "path_analysis.json")

    diff = {
        "function": fn,
        "scenario": scenario_name,
        "diff_kind": diff_kind,
        "variant": variant,
        "original": {
            "observed_path": (base_rep or {}).get("observed_path"),
            "path_match": (base_rep or {}).get("path_match"),
        },
        "candidate": {
            "observed_path": (cur_rep or {}).get("observed_path"),
            "path_match": (cur_rep or {}).get("path_match"),
        },
        "path_preserved": (
            (base_rep or {}).get("observed_path") == (cur_rep or {}).get("observed_path")
            and (cur_rep or {}).get("path_match")
        ),
        "reject_candidate": False,
        "decision": "review",
    }

    if variant and cur_rep and base_rep:
        orig_path = diff["original"].get("observed_path")
        cand_path = diff["candidate"].get("observed_path")
        if not diff["path_preserved"]:
            diff["reject_candidate"] = True
            diff["reason"] = "path changed from %s to %s" % (orig_path, cand_path)
            diff["decision"] = "reject: %s" % diff["reason"]
        elif not cur_rep.get("path_match"):
            diff["reject_candidate"] = True
            diff["reason"] = "candidate path mismatch (observed %s)" % cand_path
            diff["decision"] = "reject: %s" % diff["reason"]
        elif cur_rep.get("staleness") == "stale":
            diff["reject_candidate"] = True
            diff["reason"] = "variant is stale relative to base source"
            diff["decision"] = "reject: stale variant"
        else:
            diff["decision"] = "path preserved, compare static metrics separately"
            diff["reason"] = None

    cur_sm = load_json(cur / "scenario_source_map.json") or load_json(cur / "source_map.json")
    base_sm = load_json(base / "scenario_source_map.json") or load_json(base / "source_map.json")
    if cur_sm and base_sm:
        cur_m = (cur_rep or {}).get("static_model_advisory", {})
        base_m = (base_rep or {}).get("static_model_advisory", {})
        asm_diff = adiff_mod.diff_source_maps(base_sm, cur_sm, base_m, cur_m)
        asm_diff["scenario"] = scenario_name
        asm_diff["diff_kind"] = diff_kind
        (cur / "scenario_diff.json").write_text(json.dumps(asm_diff, indent=2) + "\n")
        adiff_mod.write_asm_diff_md(asm_diff, cur / "scenario_diff.md")
        diff["asm_diff_summary"] = {
            "instruction_delta": asm_diff.get("after_instruction_count", 0)
            - asm_diff.get("before_instruction_count", 0),
            "changed_source_lines": len(asm_diff.get("changed_source_lines", [])),
        }

    orig_bench = _load_scenario_bench(fn, scenario_name, arch, flags, compiler, variant=None)
    cand_bench = _load_scenario_bench(fn, scenario_name, arch, flags, compiler,
                                      variant=variant) if variant else {}
    diff["benchmark"] = {
        "original": orig_bench,
        "candidate": cand_bench if variant else None,
    }
    if variant:
        ob, cb = orig_bench.get("status"), cand_bench.get("status")
        diff["benchmark_both_ran"] = ob == "ran" and cb == "ran"
        if ob != "ran" or cb != "ran":
            diff["decision"] += " (benchmark incomplete)"
        elif orig_bench.get("result_ns") and cand_bench.get("result_ns"):
            delta = cand_bench["result_ns"] - orig_bench["result_ns"]
            diff["benchmark_ns_delta"] = round(delta, 2)

    diff["metrics_delta"] = {}
    if cur_rep and base_rep:
        c = cur_rep.get("static_model_advisory", {})
        b = base_rep.get("static_model_advisory", {})
        diff["metrics_delta"] = {
            "throughput_delta": (c.get("block_rthroughput") or 0) - (b.get("block_rthroughput") or 0),
            "ipc_delta": (c.get("ipc") or 0) - (b.get("ipc") or 0),
        }
        if diff["reject_candidate"] and diff["metrics_delta"].get("throughput_delta", 0) < 0:
            diff["decision"] += " (static win ignored due to path change)"

    (cur / "scenario_diff.json").write_text(json.dumps(diff, indent=2) + "\n")
    lines = [
        "# Scenario diff: %s / %s" % (fn, scenario_name),
        "",
        "- diff kind: %s" % diff_kind,
        "- original path: %s" % diff["original"].get("observed_path"),
        "- candidate path: %s" % diff["candidate"].get("observed_path"),
        "- path preserved: %s" % diff.get("path_preserved"),
        "- decision: %s" % diff.get("decision"),
        "",
    ]
    if diff.get("asm_diff_summary"):
        s = diff["asm_diff_summary"]
        lines.append("- instruction delta: %+d" % s.get("instruction_delta", 0))
        lines.append("- changed source lines: %d" % s.get("changed_source_lines", 0))
        lines.append("")
    (cur / "scenario_diff.md").write_text("\n".join(lines) + "\n")
    return diff


def generate_scenario_view(fn, scenario_name, arch, flags, compiler, variant=None):
    import view as view_mod
    sdir = C.scenario_dir(fn, scenario_name, arch, flags, compiler, variant=variant)
    if not (sdir / "source_map.json").exists() and not (sdir / "scenario_source_map.json").exists():
        return None, "scenario source map missing; use --source-map"

    sub = "scenarios/%s/%s/%s" % (fn, scenario_name, arch)
    if variant:
        sub += "/variants/%s" % variant
    out, err = view_mod.generate_html(
        fn, arch, flags, compiler, variant_dir=sdir, view_subpath=sub)
    if err:
        return None, err

    rep_path = sdir / "scenario_report.json"
    if rep_path.exists() and out:
        rep = json.loads(rep_path.read_text())
        html = out.read_text()
        banner = (
            "<p class='meta'>scenario: %s | intended: %s | observed: %s | "
            "match: %s | %s</p>" % (
                scenario_name, rep.get("intended_path", "-"),
                rep.get("observed_path", "-"), rep.get("path_match"),
                rep.get("static_path_note", "")))
        html = html.replace("<footer>", banner + "<footer>", 1)
        out.write_text(html)
    return out, None
