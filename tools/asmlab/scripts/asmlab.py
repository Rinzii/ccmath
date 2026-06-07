"""CCMath asmlab: emit, score, and gate assembly changes for registered math functions.

Subcommands:
  list, emit, check, analyze, verify, report, baseline, diff, gate, bench
  scenario, variant, golden, view
"""

import argparse
import json
import subprocess
import sys

import _asmlab_common as C
import emit as emit_mod
import analyze as analyze_mod
import classify as classify_mod
import constexpr_check as constexpr_mod
import run_meta as prov_mod
import asm_diff as asm_diff_mod
import view as view_mod
import analysis_pipeline as apipe_mod
import analysis_diff as adiff_mod
import scenario as scenario_mod
import variant as variant_mod


def emit_and_analyze(fn, arches, flags, compiler, analyze=True, source_map=False,
                     deep_analyze=False):
    target = C.get_target(fn)
    if deep_analyze:
        source_map = True
    results = {}
    for a in arches:
        vdir = emit_mod.emit_variant(fn, target, a, flags, compiler,
                                     source_map=source_map, deep_analyze=deep_analyze)
        if not vdir:
            continue
        if analyze:
            m = analyze_mod.analyze_variant(vdir, a)
            if m:
                results[a] = m
        else:
            pa_path = vdir / "path_analysis.json"
            if pa_path.exists():
                results[a] = {"path_analysis": json.loads(pa_path.read_text()),
                              "arch": a}
    return results


def emit_only(fn, arches, flags, compiler, source_map=False, deep_analyze=False):
    target = C.get_target(fn)
    if deep_analyze:
        source_map = True
    results = {}
    for a in arches:
        vdir = emit_mod.emit_variant(fn, target, a, flags, compiler,
                                     source_map=source_map, deep_analyze=deep_analyze)
        if vdir:
            pa_path = vdir / "path_analysis.json"
            entry = {"arch": a, "variant": vdir.name}
            if pa_path.exists():
                entry["path_analysis"] = json.loads(pa_path.read_text())
            meta_path = vdir / "emit_meta.json"
            if meta_path.exists():
                entry["emit_meta"] = json.loads(meta_path.read_text())
            results[a] = entry
    return results


def metrics_table(results):
    head = "| arch | isa | static est. (cyc) | uOps | insns | IPC | path | conf |"
    sep = "| --- | --- | --- | --- | --- | --- | --- | --- |"
    rows = [head, sep]
    for arch in sorted(results):
        m = results[arch]
        mca = m.get("llvm_mca", {})
        pa = m.get("path_analysis") or {}
        if "error" in mca:
            rows.append("| %s | %s | error: %s | | | | %s | %s |" % (
                arch, m.get("isa", "-"), mca["error"][:30],
                pa.get("path_category", "-"), pa.get("confidence", "-")))
            continue
        est = mca.get("static_model_estimate_cycles", mca.get("block_rthroughput", "-"))
        hot_ipc = mca.get("ipc", "-")
        rows.append("| %s | %s | %s | %s | %s | %s | %s | %s |" % (
            arch, m.get("isa", "-"), est, mca.get("total_uops", "-"),
            mca.get("instructions", "-"), hot_ipc,
            pa.get("path_category", "-"), pa.get("confidence", "-")))
    return "\n".join(rows)


def _constexpr_section(fn, target):
    mode = target.get("harness_mode") or C.default_harness_mode(fn)
    if mode == "constexpr_probe" or target.get("constexpr_cases"):
        result = constexpr_mod.run_constexpr_check(fn, target)
        lines = []
        if result.get("skipped"):
            lines.append("- constexpr probe: skipped (%s)" % result.get("reason", ""))
        elif result["compiles"]:
            lines.append("- constexpr probe: **pass** (%d cases)" % len(result["cases"]))
            for c in result["cases"]:
                lines.append("  - args %s: %s" % (c.get("args", []), c.get("status", "")))
        else:
            lines.append("- constexpr probe: **fail**")
            if result.get("errors"):
                lines.append("  - %s" % result["errors"][:200])
        return lines, result
    lines = [
        "- constexpr probe: not run (harness_mode=%s)" % mode,
        "- runtime harness does not prove constexpr path behavior",
    ]
    return lines, {"skipped": True, "harness_mode": mode}


def _runtime_section(fn, results, flags, compiler, primary):
    lines = []
    if not primary or primary not in results:
        lines.append("- no runtime variants emitted")
        return lines
    m = results[primary]
    pa = m.get("path_analysis") or {}
    vdir = C.variant_dir(fn, primary, flags, compiler)
    meta_path = vdir / "emit_meta.json"
    if meta_path.exists():
        meta = json.loads(meta_path.read_text())
        lines.append("- requested symbol: `%s`" % meta.get("requested_symbol", ""))
        lines.append("- analyzed symbol: `%s`" % meta.get("analyzed_symbol", ""))
        lines.append("- forwarder followed: %s" % meta.get("forwarder_followed", False))
        if meta.get("follow_chain"):
            lines.append("- follow chain: %s" % " -> ".join(meta["follow_chain"]))
        lines.append("- harness mode: %s" % meta.get("harness_mode", ""))
    if pa.get("external_libm_call_present"):
        lines.append("- **WARNING: external libm call present. Internal ccmath kernel was NOT measured.**")
    if pa.get("possible_constant_fold"):
        lines.append("- **WARNING: possible constant fold. Runtime path may not be selected.**")
    if pa.get("assembly_empty_or_trivial"):
        lines.append("- **WARNING: assembly empty or trivial.**")
    lines.append("- instruction count: %s" % pa.get("instruction_count", "-"))
    lines.append("- branch count: %s" % pa.get("branch_count", "-"))
    lines.append("- call count: %s (local: %s, external: %s)" % (
        pa.get("call_count", "-"), len(pa.get("local_calls", [])),
        len(pa.get("external_calls", []))))
    lines.append("- FP ops: %s, FMA: %s, div/sqrt: %s" % (
        pa.get("fp_op_count", "-"), pa.get("fma_count", "-"), pa.get("div_sqrt_count", "-")))
    lines.append("- vector instructions: %s" % pa.get("vector_present", False))
    lines.append("- likely thin forwarder: %s" % pa.get("likely_thin_forwarder", False))
    lines.append("- likely fully inlined: %s" % pa.get("likely_fully_inlined", False))
    lines.append("- kernel measured: %s" % pa.get("kernel_measured", False))
    region = vdir / "region.s"
    if region.exists():
        body = region.read_text().strip().splitlines()
        shown = "\n".join(body[:40])
        more = "" if len(body) <= 40 else "\n; ... %d more lines" % (len(body) - 40)
        lines.append("")
        lines.append("```asm")
        lines.append(shown + more)
        lines.append("```")
    return lines


def _path_classification_section(results, primary):
    lines = []
    if not primary or primary not in results:
        lines.append("- no classification available")
        return lines
    pa = results[primary].get("path_analysis") or {}
    lines.append("- path category: **%s**" % pa.get("path_category", "unknown"))
    lines.append("- confidence: %s" % pa.get("confidence", "low"))
    for ev in pa.get("evidence", []):
        lines.append("  - %s" % ev)
    return lines


def _static_model_section(results):
    lines = [
        "## Static model estimate",
        "",
        "These numbers are **static model estimates**, not measured wall-clock performance.",
        "",
        metrics_table(results),
        "",
    ]
    primary = next((a for a in C.DEFAULT_ARCHES if a in results), None)
    if primary:
        m = results[primary]
        disagree = m.get("model_disagreement")
        if disagree and disagree.get("disagrees"):
            lines.append("Model disagreement (llvm-mca vs uiCA): relative diff %.1f%%" % (
                disagree["relative_diff"] * 100))
            lines.append("")
    return lines


def build_report(fn, results, flags, compiler, constexpr_result, prov):
    target = C.get_target(fn)
    primary = next((a for a in C.DEFAULT_ARCHES if a in results), None) or (
        sorted(results)[0] if results else None)

    import cpu_knowledge as cpu_kb
    cpu_by_arch = {}
    for arch in results:
        vdir = C.variant_dir(fn, arch, flags, compiler)
        cpu_by_arch[arch] = cpu_kb.annotate_function_report(
            fn, arch, results[arch], vdir)

    report = {
        "function": fn,
        "run": prov,
        "expr": target["expr"],
        "includes": target["includes"],
        "intended_path": target.get("intended_path", ""),
        "harness_mode": target.get("harness_mode") or C.default_harness_mode(fn),
        "constexpr_behavior": constexpr_result,
        "variants": results,
        "cpu_knowledge": cpu_by_arch,
    }

    md = []
    md.append("# asmlab report: %s" % fn)
    md.append("")
    md.append("- generated: %s (commit %s%s, %s, -%s)" % (
        prov["timestamp"], prov["git_commit"],
        ", dirty" if prov["git_dirty"] else "", compiler, flags))
    md.append("- expr: %s" % target["expr"])
    md.append("")
    md.append("## Compile-time behavior")
    md.append("")
    ce_lines, _ = _constexpr_section(fn, target)
    md.extend(ce_lines)
    md.append("")
    md.append("## Runtime codegen")
    md.append("")
    md.extend(_runtime_section(fn, results, flags, compiler, primary))
    md.append("")
    md.append("## Path classification")
    md.append("")
    md.extend(_path_classification_section(results, primary))
    md.append("")
    md.extend(_source_map_section(fn, flags, compiler, primary))
    md.append("")
    md.extend(_static_model_section(results))
    if primary and cpu_by_arch.get(primary):
        md.append("")
        import cpu_knowledge as cpu_kb
        ann = {
            "microarchitecture": cpu_kb.get_microarch(primary),
            "calibration": cpu_kb._calibration_summary(cpu_kb.get_calibration(primary)),
            "matched_patterns": cpu_by_arch[primary].get("matched_patterns", []),
            "warnings": cpu_by_arch[primary].get("warnings", []),
            "kernel_exposed_heuristic": cpu_by_arch[primary].get(
                "kernel_exposed_heuristic"),
        }
        md.extend(cpu_kb.render_knowledge_md(ann))
        if len(results) > 1:
            md.append("Other arches: %s" % ", ".join(
                a for a in sorted(results) if a != primary))
            md.append("")

    return md, report


def _source_map_section(fn, flags, compiler, primary):
    lines = ["## Source map", ""]
    if not primary:
        lines.append("- no variant selected")
        return lines
    vdir = C.variant_dir(fn, primary, flags, compiler)
    sm_path = vdir / "source_map.json"
    if not sm_path.exists():
        lines.append("- not generated (re-run with --source-map)")
        return lines
    sm = json.loads(sm_path.read_text())
    lines.append("- primary attribution: **%s**" % sm.get("primary_attribution", "unknown"))
    lines.append("- mapped instructions: %d" % sm.get("instruction_count", 0))
    lines.append("- primary symbol: `%s`" % sm.get("primary_symbol", ""))
    lines.append("- artifacts: `source_map.json`, `source_map.md`")
    ccmath = [l for l in sm.get("source_lines", [])
              if l.get("attribution") == "ccmath-internal"][:5]
    if ccmath:
        lines.append("- sample ccmath lines:")
        for l in ccmath:
            lines.append("  - %s:%d (%d insns)" % (l["file"], l["line"], l["instruction_count"]))
    return lines


def write_report(fn, results, flags, compiler, constexpr_result=None):
    target = C.get_target(fn)
    arches = list(results.keys())
    prov = prov_mod.collect(fn, flags, compiler, arches)
    if constexpr_result is None:
        _, constexpr_result = _constexpr_section(fn, target)

    md_lines, report = build_report(fn, results, flags, compiler, constexpr_result, prov)
    reports = C.OUT_DIR / "reports"
    reports.mkdir(parents=True, exist_ok=True)
    md_path = reports / ("%s.md" % fn)
    json_path = reports / ("%s.json" % fn)
    md_path.write_text("\n".join(md_lines) + "\n")
    json_path.write_text(json.dumps(report, indent=2) + "\n")
    prov_mod.write_run_json(reports / ("%s.run.json" % fn), prov)
    return md_path


def snapshot(results, prov=None, fn=None):
    snap = {"metrics": {}, "path_classification": {}}
    if prov:
        snap["run"] = prov
    for arch, m in results.items():
        mca = m.get("llvm_mca", {})
        if "error" in mca:
            continue
        snap["metrics"][arch] = {
            "static_model_estimate_cycles": mca.get("static_model_estimate_cycles",
                                                    mca.get("block_rthroughput")),
            "rthru": mca.get("block_rthroughput"),
            "uops": mca.get("total_uops"),
            "insns": mca.get("instructions"),
        }
        pa = m.get("path_analysis") or {}
        if pa:
            snap["path_classification"][arch] = {
                "path_category": pa.get("path_category"),
                "confidence": pa.get("confidence"),
            }
    return snap


def cmd_knowledge_validate(_args):
    import cpu_knowledge as cpu_kb
    errors = cpu_kb.validate_all()
    if errors:
        print("knowledge validation FAILED (%d errors):" % len(errors), file=sys.stderr)
        for e in errors:
            print("  %s" % e, file=sys.stderr)
        return 1
    print("knowledge validation OK")
    return 0


def cmd_list(_args):
    fns = C.load_registry()
    print("Registered functions:")
    for name in sorted(fns):
        t = fns[name]
        mode = t.get("harness_mode", C.default_harness_mode(name))
        print("  %-12s %s  [%s]" % (name, t["expr"], mode))
    print("\nArches (default: %s):" % ", ".join(C.DEFAULT_ARCHES))
    for name in sorted(C.ARCHES):
        a = C.ARCHES[name]
        local = "local" if C.arch_local_capable(a) else "docker"
        print("  %-16s %-5s %-6s %s" % (name, a["isa"], local, a["note"]))
    return 0


def cmd_check(args):
    return cmd_report(args)


def cmd_report(args):
    flags, compiler = args.flags, args.compiler
    arches = C.parse_arch_list(args.arch)
    rc = 0
    for fn in args.fn:
        print("== %s ==" % fn)
        target = C.get_target(fn)
        _, constexpr_result = _constexpr_section(fn, target)
        results = emit_and_analyze(fn, arches, flags, compiler,
                                  source_map=getattr(args, "source_map", False),
                                  deep_analyze=getattr(args, "deep_analyze", False))
        if not results:
            print("  no results for %s" % fn)
            rc = 1
            continue
        print(metrics_table(results))
        out = write_report(fn, results, flags, compiler, constexpr_result)
        print("  report -> %s" % out.relative_to(C.PROJECT_ROOT))
    return rc


def cmd_verify(args):
    flags, compiler = args.flags, args.compiler
    arches = C.parse_arch_list(args.arch)
    target = C.get_target(args.fn)
    print("== verify %s ==" % args.fn)
    _, constexpr_result = _constexpr_section(args.fn, target)
    if constexpr_result.get("skipped"):
        print("  constexpr: skipped")
    elif constexpr_result.get("compiles"):
        print("  constexpr: pass")
    else:
        print("  constexpr: FAIL")
    results = emit_only(args.fn, arches, flags, compiler,
                        source_map=getattr(args, "source_map", False),
                        deep_analyze=getattr(args, "deep_analyze", False))
    if not results:
        return 1
    for arch in sorted(results):
        pa = results[arch].get("path_analysis", {})
        print("  [%s] %s (%s)" % (arch, pa.get("path_category", "?"),
                                   pa.get("confidence", "?")))
    return 0 if constexpr_result.get("compiles", True) else 1


def cmd_baseline(args):
    arches = C.parse_arch_list(args.arch)
    results = emit_and_analyze(args.fn, arches, args.flags, args.compiler,
                              source_map=getattr(args, "source_map", False),
                              deep_analyze=getattr(args, "deep_analyze", False))
    prov = prov_mod.collect(args.fn, args.flags, args.compiler, arches)
    snap = snapshot(results, prov=prov, fn=args.fn)
    snap["commit"] = prov["git_commit"]
    base_dir = C.OUT_DIR / "baselines"
    base_dir.mkdir(parents=True, exist_ok=True)
    path = base_dir / ("%s.json" % args.fn)
    path.write_text(json.dumps(snap, indent=2) + "\n")
    if getattr(args, "source_map", False):
        for a in arches:
            vdir = C.variant_dir(args.fn, a, args.flags, args.compiler)
            saved = asm_diff_mod.save_baseline_source_map(
                vdir, args.fn, a, args.flags, args.compiler)
            if saved:
                print("  source map baseline -> %s" % saved.relative_to(C.PROJECT_ROOT))
            if getattr(args, "deep_analyze", False):
                bdeep = apipe_mod.save_baseline_deep(
                    vdir, args.fn, a, args.flags, args.compiler)
                if bdeep:
                    print("  deep analysis baseline -> %s" % bdeep.relative_to(C.PROJECT_ROOT))
    print("baseline for %s saved (%d arches) -> %s" % (
        args.fn, len(snap["metrics"]), path.relative_to(C.PROJECT_ROOT)))
    return 0


def cmd_diff(args):
    base_path = C.OUT_DIR / "baselines" / ("%s.json" % args.fn)
    if not base_path.exists():
        C.fail("no baseline for %s; run: asmlab.py baseline %s" % (args.fn, args.fn))
    base = json.loads(base_path.read_text())
    base_metrics = base.get("metrics", base)
    arches = C.parse_arch_list(args.arch)
    results = emit_and_analyze(args.fn, arches, args.flags, args.compiler,
                              source_map=getattr(args, "source_map", False),
                              deep_analyze=getattr(args, "deep_analyze", False))
    cur = snapshot(results)
    print("diff %s vs baseline (negative delta = faster static model estimate):" % args.fn)
    print("| arch | est. base->now (delta) | uOps | insns | path |")
    print("| --- | --- | --- | --- | --- |")
    worse = better = 0
    for arch in sorted(set(base_metrics) | set(cur["metrics"])):
        b, c = base_metrics.get(arch), cur["metrics"].get(arch)
        if not b or not c:
            print("| %s | (missing) | | | |" % arch)
            continue
        b_est = b.get("static_model_estimate_cycles", b.get("rthru"))
        c_est = c.get("static_model_estimate_cycles", c.get("rthru"))
        d = round(c_est - b_est, 2)
        if d < 0:
            better += 1
        elif d > 0:
            worse += 1
        mark = "" if d == 0 else (" faster" if d < 0 else " SLOWER")
        path_cat = cur.get("path_classification", {}).get(arch, {}).get("path_category", "-")
        print("| %s | %s -> %s (%+0.2f)%s | %s -> %s | %s -> %s | %s |" % (
            arch, b_est, c_est, d, mark, b["uops"], c["uops"],
            b["insns"], c["insns"], path_cat))
    print("\nsummary: %d arch(es) faster, %d slower (static model estimates only)" % (
        better, worse))
    if getattr(args, "source_map", False):
        for a in sorted(set(base_metrics) | set(cur["metrics"])):
            b_m = base_metrics.get(a, {})
            c_m = cur["metrics"].get(a, {})
            diff = asm_diff_mod.run_diff(
                args.fn, a, args.flags, args.compiler, b_m, c_m)
            if diff.get("error"):
                print("  [%s] asm diff: %s" % (a, diff["error"]))
            else:
                print("  [%s] asm diff: %+d insns, %d changed source lines -> %s" % (
                    a,
                    diff.get("after_instruction_count", 0) - diff.get("before_instruction_count", 0),
                    len(diff.get("changed_source_lines", [])),
                    (C.variant_dir(args.fn, a, args.flags, args.compiler) / "asm_diff.md").relative_to(
                        C.PROJECT_ROOT)))
            if getattr(args, "deep_analyze", False):
                vdir = C.variant_dir(args.fn, a, args.flags, args.compiler)
                bdir = apipe_mod.baseline_deep_dir(args.fn, a, args.flags, args.compiler)
                if bdir.exists():
                    adiff = adiff_mod.compare_variants(vdir, bdir)
                    print("  [%s] deep diff: IPC delta=%s spill delta=%s" % (
                        a, adiff.get("mca", {}).get("ipc_delta"),
                        adiff.get("register_pressure", {}).get("spill_delta")))
    return 1 if worse and not better else 0


def cmd_deep_analyze(args):
    arches = C.parse_arch_list(args.arch)
    target = C.get_target(args.fn)
    print("== deep-analyze %s ==" % args.fn)
    for a in arches:
        vdir = C.variant_dir(args.fn, a, args.flags, args.compiler)
        if not (vdir / "source_map.json").exists():
            emit_mod.emit_variant(args.fn, target, a, args.flags, args.compiler,
                                  source_map=True, deep_analyze=True)
        else:
            bundle, err = apipe_mod.run_deep_analysis(
                vdir, args.fn, target, a, args.flags, args.compiler)
            if err:
                print("  [%s] %s" % (a, err))
                continue
        analyze_mod.analyze_variant(vdir, a)
        print("  [%s] deep analysis artifacts in %s" % (
            a, vdir.relative_to(C.PROJECT_ROOT)))
    return 0


def cmd_view(args):
    arches = C.parse_arch_list(args.arch)
    rc = 0
    for a in arches:
        vdir = C.variant_dir(args.fn, a, args.flags, args.compiler)
        if not (vdir / "source_map.json").exists():
            target = C.get_target(args.fn)
            emit_mod.emit_variant(args.fn, target, a, args.flags, args.compiler,
                                  source_map=True)
        out, err = view_mod.generate_html(args.fn, a, args.flags, args.compiler)
        if err:
            print("  [%s] %s" % (a, err), file=sys.stderr)
            rc = 1
            continue
        print("  view -> %s" % out.relative_to(C.PROJECT_ROOT))
        if args.open:
            import webbrowser
            webbrowser.open(out.as_uri())
    return rc


def cmd_gate(args):
    script = C.ASMLAB_DIR / "scripts" / "accuracy_gate.sh"
    cmd = ["bash", str(script), args.mode, args.fn]
    print(">> gate: %s" % " ".join(cmd))
    return subprocess.call(cmd)


def cmd_bench(args):
    profiles = C.load_benchmark_profiles()
    tgt = profiles.get("benchmark_targets", {}).get(args.fn, {})
    if tgt.get("bench_kind") == "asmlab_impl":
        import bench_impl as bench_impl_mod
        result = bench_impl_mod.run_powf_impl_bench(
            args.profile or "positive_finite_general",
            variant=getattr(args, "variant", "") or None)
        bench_impl_mod.write_bench_artifact(result)
        print(json.dumps(result, indent=2))
        return 0 if result.get("status") == "ran" else 1
    script = C.ASMLAB_DIR / "scripts" / "bench.sh"
    cmd = ["bash", str(script), args.fn]
    if args.profile:
        cmd += ["--profile", args.profile]
    if args.mode:
        cmd += ["--mode", args.mode]
    if getattr(args, "variant", ""):
        cmd += ["--variant", args.variant]
    cmd += args.bench_args
    print(">> bench: %s" % " ".join(cmd))
    return subprocess.call(cmd)


def _scenario_arches(args):
    return C.parse_arch_list(args.arch)


def cmd_scenario_list(args):
    names = C.list_scenarios(args.fn)
    if not names:
        print("no scenarios for %s" % args.fn)
        return 1
    print("scenarios for %s:" % args.fn)
    for n in names:
        rec = C.get_scenario(args.fn, n)
        print("  %s: %s (%s)" % (
            n, rec.get("intended_path", "?"), rec.get("scenario_kind", "?")))
    return 0


def cmd_scenario_verify(args):
    arches = _scenario_arches(args)
    results = scenario_mod.run_scenario_pipeline(
        args.fn, args.scenario, arches, args.flags, args.compiler,
        source_map=getattr(args, "source_map", False))
    if not results:
        return 1
    rc = 0
    for arch, sdir, rep in results:
        if rep and rep.get("path_mismatch"):
            print("  [%s] PATH MISMATCH: intended=%s observed=%s" % (
                arch, rep.get("intended_path"), rep.get("observed_path")))
            rc = 1
    return rc


def cmd_scenario_report(args):
    arches = _scenario_arches(args)
    scenario_mod.run_scenario_pipeline(
        args.fn, args.scenario, arches, args.flags, args.compiler,
        source_map=getattr(args, "source_map", False))
    for arch in arches:
        sdir, rep = scenario_mod.write_scenario_report(
            args.fn, args.scenario, arch, args.flags, args.compiler)
        if sdir:
            print("  [%s] -> %s" % (arch, (sdir / "scenario_report.md").relative_to(C.PROJECT_ROOT)))
            if rep.get("path_mismatch"):
                print("  [%s] PATH MISMATCH" % arch)
    return 0


def cmd_scenario_baseline(args):
    arches = _scenario_arches(args)
    scenario_mod.run_scenario_pipeline(
        args.fn, args.scenario, arches, args.flags, args.compiler,
        source_map=getattr(args, "source_map", False))
    for arch in arches:
        dest = scenario_mod.save_scenario_baseline(
            args.fn, args.scenario, arch, args.flags, args.compiler)
        print("  baseline -> %s" % dest.relative_to(C.PROJECT_ROOT))
    return 0


def cmd_scenario_diff(args):
    arches = _scenario_arches(args)
    scenario_mod.run_scenario_pipeline(
        args.fn, args.scenario, arches, args.flags, args.compiler,
        source_map=getattr(args, "source_map", False))
    rc = 0
    for arch in arches:
        diff = scenario_mod.scenario_diff(
            args.fn, args.scenario, arch, args.flags, args.compiler,
            variant=getattr(args, "variant", None))
        if diff.get("error"):
            print("  [%s] %s" % (arch, diff["error"]))
            rc = 1
            continue
        print("  [%s] %s" % (arch, diff.get("decision")))
        if diff.get("reject_candidate"):
            rc = 1
    return rc


def cmd_scenario_validate(args):
    rc = cmd_scenario_report(args)
    if rc != 0:
        return rc
    gate = _gate_status_scenario(args)
    if gate != 0:
        rc = gate
    return rc


def _gate_status_scenario(args):
    rec = C.get_scenario(args.fn, args.scenario)
    mode = rec.get("accuracy_gate", "simple")
    gate_st = scenario_mod._gate_status(args.fn, rec)
    if gate_st in ("advisory", "not_applicable"):
        print("accuracy gate: %s (scenario gate not merge-grade)" % gate_st)
        return 0
    return cmd_gate(argparse.Namespace(fn=args.fn, mode=mode, flags=args.flags,
                                      compiler=args.compiler, arch=args.arch))


def cmd_variant_init(args):
    vdir, manifest = variant_mod.create_candidate(
        args.fn, args.variant_name, force=getattr(args, "force", False))
    print("Created candidate workspace: %s" % vdir.relative_to(C.PROJECT_ROOT))
    print("  candidate: %s" % manifest["candidate_file"])
    print("  base: %s @ %s" % (manifest["base_file"], manifest["base_git_commit"][:12]))
    return 0


def cmd_variant_patch(args):
    return variant_mod.generate_patch(
        args.fn, args.variant_name, apply=getattr(args, "apply", False))


def cmd_variant_scenario_report(args):
    arches = _scenario_arches(args)
    scenario_mod.run_scenario_pipeline(
        args.fn, args.scenario, arches, args.flags, args.compiler,
        source_map=getattr(args, "source_map", False), source_kind="original",
        run_bench=True)
    scenario_mod.run_scenario_pipeline(
        args.fn, args.scenario, arches, args.flags, args.compiler,
        source_map=getattr(args, "source_map", False), variant=args.variant_name,
        run_bench=True)
    return 0


def cmd_variant_accuracy(args):
    import variant_accuracy as va_mod
    mode = getattr(args, "mode", "simple")
    report = va_mod.run_variant_accuracy(
        args.fn, args.variant_name, mode=mode)
    print("candidate accuracy: %s" % report.get("status"))
    print("  production simple: %s" % report.get("production_accuracy", {}).get("simple"))
    print("  candidate simple: %s" % report.get("candidate_accuracy", {}).get("simple"))
    print("  gate -> %s" % va_mod._gate_path(args.fn, args.variant_name, mode))
    if report.get("status") == "fail":
        return 1
    return 0


def cmd_variant_scenario_validate(args):
    import variant_accuracy as va_mod
    acc = va_mod.run_variant_accuracy(
        args.fn, args.variant_name, mode="simple")
    if acc.get("status") == "fail":
        print("  candidate accuracy FAILED")
        return 1
    rc = cmd_variant_scenario_report(args)
    if rc != 0:
        return rc
    arches = _scenario_arches(args)
    for arch in arches:
        diff = scenario_mod.scenario_diff(
            args.fn, args.scenario, arch, args.flags, args.compiler,
            variant=args.variant_name)
        if diff.get("reject_candidate"):
            print("  [%s] REJECT: %s" % (arch, diff.get("reason") or diff.get("decision")))
            rc = 1
        orig_bench = scenario_mod._load_scenario_bench(
            args.fn, args.scenario, arch, args.flags, args.compiler, variant=None)
        cand_bench = scenario_mod._load_scenario_bench(
            args.fn, args.scenario, arch, args.flags, args.compiler,
            variant=args.variant_name)
        print("  [%s] benchmark original: %s" % (arch, orig_bench.get("status")))
        print("  [%s] benchmark candidate: %s" % (arch, cand_bench.get("status")))
        if orig_bench.get("status") != "ran" or cand_bench.get("status") != "ran":
            print("  [%s] benchmark incomplete (not merge-grade)" % arch)
            rc = 1
    return rc


def cmd_variant_compare(args):
    import candidate_compare as cc_mod
    arches = C.parse_arch_list(args.arch) if args.arch else [C.DEFAULT_ARCHES[0]]
    if len(arches) != 1:
        print("variant compare supports one --arch at a time", file=sys.stderr)
        return 1
    arch = arches[0]
    variants = []
    if getattr(args, "all_variants", False):
        variants = cc_mod.list_workspace_variants(args.fn)
    elif getattr(args, "variants", None):
        variants = list(args.variants)
    if not variants:
        print("variant compare: specify --variants NAME ... or --all", file=sys.stderr)
        return 1
    scenarios = list(args.scenarios) if getattr(args, "scenarios", None) else []
    rc = 0
    out_dir, report = cc_mod.run_compare(
        args.fn, variants, scenarios, arch, args.flags, args.compiler,
        source_map=getattr(args, "source_map", False),
        run_bench=not getattr(args, "skip_bench", False),
        force=getattr(args, "force_emit", False))
    print("")
    print("compare report -> %s" % out_dir.relative_to(C.PROJECT_ROOT))
    print("recommendation: %s" % report.get("recommendation"))
    for ent in report.get("ranking", []):
        print("  #%d %s: %s" % (ent["rank"], ent["variant"], ent["decision"]))
        if ent["decision"].startswith("reject"):
            rc = 1
    return rc


def cmd_variant_scenario_diff(args):
    arches = _scenario_arches(args)
    scenario_mod.run_scenario_pipeline(
        args.fn, args.scenario, arches, args.flags, args.compiler,
        source_map=getattr(args, "source_map", False), source_kind="original",
        run_bench=True)
    scenario_mod.run_scenario_pipeline(
        args.fn, args.scenario, arches, args.flags, args.compiler,
        source_map=getattr(args, "source_map", False), variant=args.variant_name,
        run_bench=True)
    rc = 0
    for arch in arches:
        diff = scenario_mod.scenario_diff(
            args.fn, args.scenario, arch, args.flags, args.compiler,
            variant=args.variant_name)
        if diff.get("error"):
            print("  [%s] %s" % (arch, diff["error"]))
            rc = 1
            continue
        print("  [%s] %s" % (arch, diff.get("decision")))
        if diff.get("reject_candidate"):
            rc = 1
    return rc


def cmd_validate(args):
    if not getattr(args, "source_map", False):
        args.source_map = False
    rc = cmd_report(args)
    gate_args = argparse.Namespace(fn=args.fn, mode="simple")
    if cmd_gate(gate_args) != 0:
        return 1
    bench_args = argparse.Namespace(fn=args.fn, profile="positive_finite_general",
                                    mode="latency", bench_args=[])
    bench_rc = cmd_bench(bench_args)
    if bench_rc != 0:
        print("bench advisory: failed or not wired (see bench output)", file=sys.stderr)
    return rc


def main(argv=None):
    ap = argparse.ArgumentParser(prog="asmlab.py", description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = ap.add_subparsers(dest="cmd", required=True)

    def add_common(p, many=False, source_map=False, deep=False):
        p.add_argument("fn", nargs="+" if many else None, help="registered function name")
        p.add_argument("--arch", default="", help="comma list (default: %s)" % ",".join(C.DEFAULT_ARCHES))
        p.add_argument("--flags", default=C.DEFAULT_FLAGS, choices=list(C.FLAG_VARIANTS))
        p.add_argument("--compiler", default="clang", choices=["clang", "gcc"])
        if source_map:
            p.add_argument("--source-map", action="store_true",
                           help="emit debug asm and source/assembly correlation")
        if deep:
            p.add_argument("--deep-analyze", action="store_true",
                           help="CFG, MCA, opt remarks, register pressure, semantic passes")

    def deep_flags(a):
        extra = []
        if getattr(a, "source_map", False):
            extra.append("--source-map")
        if getattr(a, "deep_analyze", False):
            extra.append("--deep-analyze")
        return extra

    sub.add_parser("list").set_defaults(func=cmd_list)
    p_check = sub.add_parser("check", help="emit, analyze, and write report (preferred)")
    add_common(p_check, many=True, source_map=True, deep=True)
    p_check.set_defaults(func=cmd_check)
    p_emit = sub.add_parser("emit"); add_common(p_emit, source_map=True, deep=True)
    p_emit.set_defaults(func=lambda a: emit_mod.main(
        [a.fn, "--arch", a.arch, "--flags", a.flags, "--compiler", a.compiler] + deep_flags(a)))
    p_an = sub.add_parser("analyze"); add_common(p_an)
    p_an.set_defaults(func=lambda a: analyze_mod.main([a.fn, "--arch", a.arch]))
    p_verify = sub.add_parser("verify"); add_common(p_verify, source_map=True, deep=True)
    p_verify.set_defaults(func=cmd_verify)
    p_rep = sub.add_parser("report"); add_common(p_rep, many=True, source_map=True, deep=True)
    p_rep.set_defaults(func=cmd_report)
    p_base = sub.add_parser("baseline"); add_common(p_base, source_map=True, deep=True)
    p_base.set_defaults(func=cmd_baseline)
    p_diff = sub.add_parser("diff"); add_common(p_diff, source_map=True, deep=True)
    p_diff.set_defaults(func=cmd_diff)
    p_deep = sub.add_parser("deep-analyze"); add_common(p_deep, source_map=True, deep=True)
    p_deep.set_defaults(func=cmd_deep_analyze)
    p_view = sub.add_parser("view"); add_common(p_view, source_map=True, deep=True)
    p_view.add_argument("--open", action="store_true", help="open HTML in browser")
    p_view.set_defaults(func=cmd_view)
    p_gate = sub.add_parser("gate"); add_common(p_gate)
    p_gate.add_argument("--mode", default="simple", choices=["simple", "rigorous", "all"])
    p_gate.set_defaults(func=cmd_gate)
    p_bench = sub.add_parser("bench"); add_common(p_bench)
    p_bench.add_argument("--profile", default="", help="benchmark profile name")
    p_bench.add_argument("--mode", default="", help="latency, throughput, or mixed")
    p_bench.add_argument("--variant", default="", help="candidate variant name (impl benches)")
    p_bench.add_argument("bench_args", nargs="*", help="extra Google Benchmark flags")
    p_bench.set_defaults(func=cmd_bench)
    p_val = sub.add_parser("validate"); add_common(p_val, source_map=True, deep=True)
    p_val.set_defaults(func=cmd_validate)

    p_know = sub.add_parser("knowledge", help="CPU knowledge base")
    know_sub = p_know.add_subparsers(dest="knowledge_cmd", required=True)
    p_kv = know_sub.add_parser("validate", help="validate knowledge JSON files")
    p_kv.set_defaults(func=cmd_knowledge_validate)

    p_golden = sub.add_parser("golden", help="Horner vs Estrin calibration validation")
    p_golden.add_argument("--arch", default="x86-64-v3")
    p_golden.add_argument("--flags", default=C.DEFAULT_FLAGS, choices=list(C.FLAG_VARIANTS))
    p_golden.add_argument("--compiler", default="clang", choices=["clang", "gcc"])
    p_golden.add_argument("--quick", action="store_true")
    p_golden.add_argument("--skip-bench", action="store_true")
    p_golden.set_defaults(func=lambda a: __import__("run_golden_analysis").main([
        "--arch", a.arch, "--flags", a.flags, "--compiler", a.compiler]
        + (["--quick"] if a.quick else [])
        + (["--skip-bench"] if a.skip_bench else [])))

    p_sc = sub.add_parser("scenario", help="path scenario testing")
    sc_sub = p_sc.add_subparsers(dest="scenario_cmd", required=True)

    p_slist = sc_sub.add_parser("list")
    p_slist.add_argument("fn")
    p_slist.set_defaults(func=cmd_scenario_list)

    def add_scenario_cmd(name, handler):
        p = sc_sub.add_parser(name)
        p.add_argument("fn")
        p.add_argument("scenario")
        p.add_argument("--arch", default="")
        p.add_argument("--flags", default=C.DEFAULT_FLAGS, choices=list(C.FLAG_VARIANTS))
        p.add_argument("--compiler", default="clang", choices=["clang", "gcc"])
        p.add_argument("--source-map", action="store_true")
        p.add_argument("--variant", default="", help="candidate variant name for diff")
        p.set_defaults(func=handler)

    add_scenario_cmd("verify", cmd_scenario_verify)
    add_scenario_cmd("report", cmd_scenario_report)
    add_scenario_cmd("baseline", cmd_scenario_baseline)
    add_scenario_cmd("diff", cmd_scenario_diff)
    add_scenario_cmd("validate", cmd_scenario_validate)

    p_var = sub.add_parser("variant", help="isolated candidate-source variants")
    var_sub = p_var.add_subparsers(dest="variant_cmd", required=True)

    p_vinit = var_sub.add_parser("init", help="create editable candidate copy from base impl")
    p_vinit.add_argument("fn")
    p_vinit.add_argument("--name", required=True, dest="variant_name")
    p_vinit.add_argument("--force", action="store_true", help="overwrite existing variant")
    p_vinit.set_defaults(func=cmd_variant_init)

    p_vpatch = var_sub.add_parser("patch", help="generate patch.diff (does not apply by default)")
    p_vpatch.add_argument("fn")
    p_vpatch.add_argument("variant_name")
    p_vpatch.add_argument("--apply", action="store_true", help="explicit apply (not implemented)")
    p_vpatch.set_defaults(func=cmd_variant_patch)

    p_vsd = var_sub.add_parser("scenario")
    vsc_sub = p_vsd.add_subparsers(dest="variant_scenario_cmd", required=True)

    def add_variant_scenario_cmd(name, handler):
        p = vsc_sub.add_parser(name)
        p.add_argument("fn")
        p.add_argument("variant_name")
        p.add_argument("scenario")
        p.add_argument("--arch", default="")
        p.add_argument("--flags", default=C.DEFAULT_FLAGS, choices=list(C.FLAG_VARIANTS))
        p.add_argument("--compiler", default="clang", choices=["clang", "gcc"])
        p.add_argument("--source-map", action="store_true")
        p.set_defaults(func=handler)

    add_variant_scenario_cmd("report", cmd_variant_scenario_report)
    add_variant_scenario_cmd("diff", cmd_variant_scenario_diff)
    add_variant_scenario_cmd("validate", cmd_variant_scenario_validate)

    p_vacc = var_sub.add_parser("accuracy", help="candidate-specific accuracy and edge cases")
    p_vacc.add_argument("fn")
    p_vacc.add_argument("variant_name")
    p_vacc.add_argument("--mode", default="simple", choices=["simple", "rigorous"])
    p_vacc.set_defaults(func=cmd_variant_accuracy)

    p_vcmp = var_sub.add_parser("compare", help="compare multiple candidates across scenarios")
    p_vcmp.add_argument("fn")
    p_vcmp.add_argument("--variants", nargs="*", default=None,
                        help="candidate names (default: none unless --all)")
    p_vcmp.add_argument("--all", dest="all_variants", action="store_true",
                        help="compare all workspace variants with manifest.json")
    p_vcmp.add_argument("--scenarios", nargs="*", default=None,
                        help="scenario names (default: all registered for function)")
    p_vcmp.add_argument("--arch", default="x86-64-v3")
    p_vcmp.add_argument("--flags", default=C.DEFAULT_FLAGS, choices=list(C.FLAG_VARIANTS))
    p_vcmp.add_argument("--compiler", default="clang", choices=["clang", "gcc"])
    p_vcmp.add_argument("--source-map", action="store_true")
    p_vcmp.add_argument("--skip-bench", action="store_true",
                        help="do not run benchmarks for missing scenario_bench.json")
    p_vcmp.add_argument("--force-emit", action="store_true",
                        help="re-emit all scenario artifacts")
    p_vcmp.set_defaults(func=cmd_variant_compare)

    args = ap.parse_args(argv)
    return args.func(args)


if __name__ == "__main__":
    sys.exit(main())
