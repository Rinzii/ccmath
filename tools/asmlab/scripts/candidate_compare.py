#!/usr/bin/env python3
# Copyright (c) Ian Pike
# Copyright (c) CCMath contributors
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
"""Multi-candidate comparison across scenarios."""

import json
import re
from datetime import datetime, timezone
from pathlib import Path

import _asmlab_common as C
import asm_diff as adiff_mod
import scenario as scenario_mod
import variant as variant_mod

import cpu_knowledge as cpu_kb

DECISION_PRIORITY = {
    "reject_stale": 0,
    "reject_path_mismatch": 1,
    "reject_accuracy": 2,
    "reject_special_values": 3,
    "reject_benchmark": 4,
    "reject_constexpr": 5,
    "incomplete": 6,
    "static_only": 7,
    "ready_for_review": 8,
}

POSITIVE_DECISIONS = {"ready_for_review"}


def list_workspace_variants(fn):
    root = C.OUT_DIR / "variants" / fn
    if not root.exists():
        return []
    return sorted(
        d.name for d in root.iterdir()
        if d.is_dir() and (d / "manifest.json").exists()
    )


def list_function_scenarios(fn):
    data = C.load_json_registry(C.PATH_SCENARIOS_PATH)
    return sorted(data.get("scenarios", {}).get(fn, {}).keys())


def compare_out_dir(fn, arch):
    return C.OUT_DIR / fn / "candidate_compare" / arch


def _load_json(path):
    if not path.exists():
        return None
    return json.loads(path.read_text())


def _scenario_emit_dir(fn, scenario, arch, flags, compiler, variant=None):
    return C.scenario_dir(fn, scenario, arch, flags, compiler, variant=variant)


def _artifacts_ready(sdir, need_source_map=False):
    if not sdir or not sdir.exists():
        return False
    if not (sdir / "scenario_report.json").exists():
        return False
    if need_source_map and not (
            (sdir / "scenario_source_map.json").exists()
            or (sdir / "source_map.json").exists()):
        return False
    return True


def ensure_scenario_artifacts(fn, scenario, arch, flags, compiler, variant=None,
                             source_map=False, run_bench=True, force=False):
    sdir = _scenario_emit_dir(fn, scenario, arch, flags, compiler, variant=variant)
    if force or not _artifacts_ready(sdir, need_source_map=source_map):
        emitted = scenario_mod.emit_scenario(
            fn, scenario, arch, flags, compiler,
            source_map=source_map, source_kind="original", variant=variant)
        if emitted is None:
            raise RuntimeError("emit failed for %s" % (variant or "original"))
        if run_bench:
            scenario_mod.run_scenario_bench_for_emit(
                fn, scenario, arch, flags, compiler, variant=variant)
        scenario_mod.write_scenario_report(
            fn, scenario, arch, flags, compiler,
            source_kind="original", variant=variant)
    if not _artifacts_ready(sdir, need_source_map=False):
        raise RuntimeError("artifacts incomplete for %s" % (variant or "original"))
    return sdir


def _load_source_map(sdir):
    for name in ("scenario_source_map.json", "source_map.json"):
        p = sdir / name
        if p.exists():
            return _load_json(p)
    return None


def _source_map_confidence(sm, pa, arch, fn, sdir=None):
    if not sm:
        return {"level": "unknown", "advisory": True, "reason": "source map missing"}
    counts = sm.get("attribution_counts") or {}
    internal = counts.get("ccmath-internal", 0)
    harness = counts.get("harness-only", 0)
    primary = sm.get("primary_attribution", "")
    insn = sm.get("instruction_count") or (pa or {}).get("instruction_count") or 0
    reasons = []
    advisory = False
    if primary == "harness-only" or harness > internal:
        reasons.append("primary attribution is harness-only")
        advisory = True
    if internal < cpu_kb.KERNEL_EXPOSED_MIN_INTERNAL:
        reasons.append("low internal attribution count (%d)" % internal)
        advisory = True
    if fn == "powf_impl" and arch == "apple-m1" and not cpu_kb.kernel_exposed_in_source_map(
            sdir, pa):
        reasons.append("M1 powf_impl thin slice")
        advisory = True
    level = "high"
    if advisory:
        level = "low" if insn <= 40 else "medium"
    if not reasons:
        reasons.append("kernel exposed with internal attribution")
    return {
        "level": level,
        "advisory": advisory,
        "reason": "; ".join(reasons),
        "internal_attribution": internal,
        "harness_attribution": harness,
        "instruction_count": insn,
        "primary_attribution": primary,
    }


def _stack_traffic_count(sm):
    if not sm:
        return 0
    n = 0
    for ent in sm.get("instructions", []):
        asm = ent.get("assembly", "").lower()
        tags = set(ent.get("tags", []))
        if "store" in tags or "load" in tags:
            if re.search(r"\[sp|\(%rsp\)|%rsp", asm):
                n += 1
        if "spill" in tags or "reload" in tags:
            n += 1
    return n


def _tag_counts(sm):
    if not sm:
        return {}
    return adiff_mod._summarize_tags(sm)


def _distinct_regs(sm, isa):
    if not sm:
        return {"fp": 0, "gpr": 0}
    fp = set()
    gpr = set()
    for ent in sm.get("instructions", []):
        asm = ent.get("assembly", "")
        if isa == "x86":
            for m in re.findall(r"%xmm\d+", asm, re.I):
                fp.add(m.lower())
            for m in re.findall(r"%r[a-z0-9]+|%e[a-z0-9]+", asm, re.I):
                gpr.add(m.lower())
        else:
            for m in re.findall(r"\b[wx]\d+\b", asm, re.I):
                if m.lower().startswith(("w", "x")):
                    gpr.add(m.lower())
            for m in re.findall(r"\b[sdqv]\d+\b", asm, re.I):
                fp.add(m.lower())
    return {"fp": len(fp), "gpr": len(gpr)}


def _dependency_chain_patterns(region_text, isa_hint, variant_dir, pa):
    patterns = cpu_kb.detect_patterns(pa, region_text or "", isa_hint=isa_hint,
                                      variant_dir=variant_dir)
    dep_ids = {
        "x86_stack_serial_fma",
        "aarch64_stack_serial_fma",
        "back_to_back_fma_dependency",
    }
    return [p for p in patterns if p.get("pattern_rule_id") in dep_ids]


def _region_text(sdir):
    for name in ("region.s", "asm.s"):
        p = sdir / name
        if p.exists():
            return p.read_text().lower()
    return ""


def _path_safety(orig_rep, cand_rep, orig_pa, cand_pa, staleness):
    signals = []
    if staleness.get("staleness") == "stale":
        return "fail", ["candidate stale"], True
    if not cand_rep.get("path_match"):
        return "fail", ["candidate path mismatch"], True
    if orig_rep.get("observed_path") != cand_rep.get("observed_path"):
        return "fail", ["path changed %s -> %s" % (
            orig_rep.get("observed_path"), cand_rep.get("observed_path"))], True
    if cand_pa.get("external_libm_call_present"):
        return "fail", ["unexpected libm call"], True
    if cand_pa.get("likely_thin_forwarder"):
        signals.append("thin forwarder")
    if orig_pa.get("hardware_instruction_lowering") != cand_pa.get("hardware_instruction_lowering"):
        signals.append("hardware lowering changed")
    if cand_rep.get("path_confidence") == "low":
        signals.append("low path confidence")
    kernel_exposed = cpu_kb.kernel_exposed_in_source_map(
        cand_rep.get("_sdir"), cand_pa)
    if kernel_exposed:
        signals.append("kernel exposed")
    else:
        signals.append("kernel not fully exposed")
    if signals and any(s in signals for s in ("thin forwarder", "low path confidence")):
        return "warn", signals, False
    return "pass", signals, False


def _spill_reload_heuristic(orig_sm, cand_sm, sm_conf):
    b = _tag_counts(orig_sm)
    a = _tag_counts(cand_sm)
    spill_delta = a.get("spill", 0) - b.get("spill", 0)
    reload_delta = a.get("reload", 0) - b.get("reload", 0)
    stack_delta = _stack_traffic_count(cand_sm) - _stack_traffic_count(orig_sm)
    confidence = "high"
    if sm_conf.get("advisory"):
        confidence = "advisory"
    if not orig_sm or not cand_sm:
        confidence = "unknown"
    return {
        "spill_delta": spill_delta,
        "reload_delta": reload_delta,
        "stack_traffic_delta": stack_delta,
        "confidence": confidence,
        "source_map_confidence": sm_conf.get("level"),
    }


def _register_pressure_proxy(orig_pa, cand_pa, orig_sm, cand_sm, spill_h, isa):
    insn_d = (cand_pa.get("instruction_count") or 0) - (orig_pa.get("instruction_count") or 0)
    orig_regs = _distinct_regs(orig_sm, isa)
    cand_regs = _distinct_regs(cand_sm, isa)
    fp_d = cand_regs["fp"] - orig_regs["fp"]
    gpr_d = cand_regs["gpr"] - orig_regs["gpr"]
    call_d = (cand_pa.get("call_count") or 0) - (orig_pa.get("call_count") or 0)
    score = "neutral"
    if spill_h.get("confidence") == "unknown":
        return "unknown", {
            "instruction_delta": insn_d,
            "fp_reg_delta": fp_d,
            "gpr_reg_delta": gpr_d,
            "call_delta": call_d,
        }
    improved = (spill_h.get("spill_delta", 0) < 0
                or spill_h.get("stack_traffic_delta", 0) < 0
                or (insn_d < -3 and call_d <= 0))
    worse = (spill_h.get("spill_delta", 0) > 0
             or spill_h.get("stack_traffic_delta", 0) > 2
             or call_d > 0)
    if improved and not worse:
        score = "improved"
    elif worse and not improved:
        score = "worse"
    return score, {
        "instruction_delta": insn_d,
        "fp_reg_delta": fp_d,
        "gpr_reg_delta": gpr_d,
        "call_delta": call_d,
    }


def _dependency_chain_delta(orig_region, cand_region, isa, orig_sdir, cand_sdir, orig_pa, cand_pa):
    if isa == "x86":
        isa_hint = "x86"
    elif isa == "arm":
        isa_hint = "arm"
    else:
        isa_hint = None
    o_pat = _dependency_chain_patterns(orig_region, isa_hint, orig_sdir, orig_pa)
    c_pat = _dependency_chain_patterns(cand_region, isa_hint, cand_sdir, cand_pa)
    o_n, c_n = len(o_pat), len(c_pat)
    if o_n == 0 and c_n == 0:
        return "unknown", {"original_patterns": 0, "candidate_patterns": 0}
    if c_n < o_n:
        return "improved", {"original_patterns": o_n, "candidate_patterns": c_n}
    if c_n > o_n:
        return "worse", {"original_patterns": o_n, "candidate_patterns": c_n}
    return "neutral", {"original_patterns": o_n, "candidate_patterns": c_n}


def _call_boundary_score(orig_pa, cand_pa, orig_rep, cand_rep):
    o_calls = orig_pa.get("call_count") or 0
    c_calls = cand_pa.get("call_count") or 0
    o_ext = len(orig_pa.get("external_calls") or [])
    c_ext = len(cand_pa.get("external_calls") or [])
    o_loc = len(orig_pa.get("local_calls") or [])
    c_loc = len(cand_pa.get("local_calls") or [])
    if c_ext > o_ext and orig_rep.get("expected_external_calls") is False:
        return "fail", {
            "call_delta": c_calls - o_calls,
            "external_call_delta": c_ext - o_ext,
            "local_call_delta": c_loc - o_loc,
        }
    if c_calls < o_calls or (o_loc > c_loc and c_ext <= o_ext):
        return "improved", {
            "call_delta": c_calls - o_calls,
            "external_call_delta": c_ext - o_ext,
            "local_call_delta": c_loc - o_loc,
        }
    if c_calls > o_calls:
        return "worse", {
            "call_delta": c_calls - o_calls,
            "external_call_delta": c_ext - o_ext,
            "local_call_delta": c_loc - o_loc,
        }
    return "neutral", {
        "call_delta": c_calls - o_calls,
        "external_call_delta": c_ext - o_ext,
        "local_call_delta": c_loc - o_loc,
    }


def _benchmark_heuristic(orig_bench, cand_bench):
    ob_st = orig_bench.get("status", "not_run")
    cb_st = cand_bench.get("status", "not_run")
    if ob_st != "ran" or cb_st != "ran":
        st = "not_run"
        if ob_st == "not_wired" or cb_st == "not_wired":
            st = "not_wired"
        return {
            "benchmark_status": st,
            "benchmark_delta_ns": None,
            "benchmark_delta_percent": None,
        }
    o_ns = orig_bench.get("result_ns")
    c_ns = cand_bench.get("result_ns")
    if o_ns is None or c_ns is None:
        return {
            "benchmark_status": "advisory",
            "benchmark_delta_ns": None,
            "benchmark_delta_percent": None,
        }
    delta = round(c_ns - o_ns, 2)
    pct = round(100.0 * delta / o_ns, 2) if o_ns else None
    status = "neutral"
    if delta < -0.5:
        status = "pass"
    elif delta > 0.5:
        status = "fail"
    return {
        "benchmark_status": status,
        "benchmark_delta_ns": delta,
        "benchmark_delta_percent": pct,
        "original_ns": o_ns,
        "candidate_ns": c_ns,
    }


def _accuracy_heuristic(fn, scenario_rec, cand_rep, variant):
    import variant_accuracy as va_mod
    # Gate files are authoritative. Cached scenario_report candidate_accuracy may be stale.
    production = C.production_accuracy_field(fn, scenario_rec)
    candidate = va_mod.candidate_accuracy_field(fn, variant=variant, mode="simple")
    gate_mode = scenario_rec.get("accuracy_gate")
    special = "not_applicable"
    if scenario_rec.get("input_profile") == "nan_inf_signed_zero":
        groups = candidate.get("groups") or {}
        nan_status = groups.get("nan_inf", "not_wired")
        if nan_status == "pass":
            special = "pass"
        elif nan_status == "fail":
            special = "fail"
        else:
            special = "not_wired"
        if gate_mode == "rigorous" and production.get("simple") == "pass":
            special = "advisory" if special == "not_wired" else special
    return {
        "production_accuracy": production,
        "candidate_accuracy": candidate,
        "candidate_accuracy_simple": candidate.get("simple", candidate.get("status")),
        "candidate_accuracy_groups": candidate.get("groups", {}),
        "accuracy_status": candidate.get("status", "not_run"),
        "accuracy_note": candidate.get("note"),
        "special_value_status": special,
        "scenario_gate_status": cand_rep.get("accuracy_gate_status"),
        "production_gate_result": production.get("simple", production.get("status")),
    }


def _static_delta(orig_rep, cand_rep):
    o = orig_rep.get("static_model_advisory", {})
    c = cand_rep.get("static_model_advisory", {})
    return {
        "throughput_delta": (c.get("block_rthroughput") or 0) - (o.get("block_rthroughput") or 0),
        "ipc_delta": round((c.get("ipc") or 0) - (o.get("ipc") or 0), 3),
        "instruction_delta": (c.get("instructions") or 0) - (o.get("instructions") or 0),
    }


def _decide_scenario(fn, arch, heuristics, m1_spill_advisory):
    if heuristics.get("reject_candidate"):
        return "reject_path_mismatch"
    if heuristics.get("staleness") == "stale":
        return "reject_stale"
    ps = heuristics.get("path_safety")
    if ps == "fail":
        return "reject_path_mismatch"
    cb = heuristics.get("call_boundary")
    if cb == "fail":
        return "reject_path_mismatch"
    cand_acc = heuristics.get("candidate_accuracy_simple", "not_run")
    if cand_acc == "fail":
        return "reject_accuracy"
    sv = heuristics.get("special_value_status")
    if sv == "fail":
        return "reject_special_values"
    bench = heuristics.get("benchmark_status")
    if bench == "fail":
        return "reject_benchmark"
    missing_cand_acc = cand_acc in ("not_run", "not_wired", "production_only", "advisory")
    cand_groups = heuristics.get("candidate_accuracy_groups") or {}
    if fn == "powf_impl":
        worst = cand_groups.get("known_worst_ulp", "not_wired")
        if worst in ("not_run", "not_wired", "fail"):
            missing_cand_acc = True
    if bench == "pass" and missing_cand_acc:
        return "incomplete"
    if bench in ("not_run", "not_wired", "advisory", None):
        if heuristics.get("static_improved") and not heuristics.get("benchmark_win"):
            return "static_only" if not m1_spill_advisory else "incomplete"
        return "incomplete"
    if missing_cand_acc:
        return "incomplete"
    if bench == "pass" and cand_acc == "pass" and ps in ("pass", "warn"):
        return "ready_for_review"
    if heuristics.get("static_improved"):
        return "static_only"
    return "incomplete"


def compare_variant_scenario(fn, variant, scenario, arch, flags, compiler,
                             source_map=False, run_bench=True, force=False):
    scenario_rec = C.get_scenario(fn, scenario)
    isa = C.ARCHES.get(arch, {}).get("isa", "x86")

    orig_dir = ensure_scenario_artifacts(
        fn, scenario, arch, flags, compiler, variant=None,
        source_map=source_map, run_bench=run_bench, force=force)
    cand_dir = ensure_scenario_artifacts(
        fn, scenario, arch, flags, compiler, variant=variant,
        source_map=source_map, run_bench=run_bench, force=force)

    orig_rep = _load_json(orig_dir / "scenario_report.json") or {}
    cand_rep = _load_json(cand_dir / "scenario_report.json") or {}
    orig_rep["_sdir"] = orig_dir
    cand_rep["_sdir"] = cand_dir

    orig_pa = orig_rep.get("path_analysis") or _load_json(orig_dir / "path_analysis.json") or {}
    cand_pa = cand_rep.get("path_analysis") or _load_json(cand_dir / "path_analysis.json") or {}

    manifest = variant_mod.load_manifest(fn, variant)
    staleness = variant_mod.check_staleness(manifest)

    orig_sm = _load_source_map(orig_dir)
    cand_sm = _load_source_map(cand_dir)
    if orig_sm:
        orig_sm["_variant_dir"] = orig_dir
    if cand_sm:
        cand_sm["_variant_dir"] = cand_dir

    sm_conf = _source_map_confidence(cand_sm, cand_pa, arch, fn, sdir=cand_dir)
    kernel_exposed = cpu_kb.kernel_exposed_in_source_map(cand_dir, cand_pa)
    m1_spill_advisory = (
        fn == "powf_impl" and arch == "apple-m1" and not kernel_exposed)

    path_safety, path_signals, reject = _path_safety(
        orig_rep, cand_rep, orig_pa, cand_pa, staleness)

    spill_h = _spill_reload_heuristic(orig_sm, cand_sm, sm_conf)
    if m1_spill_advisory:
        spill_h["confidence"] = "advisory"
        spill_h["m1_thin_slice"] = True

    reg_score, reg_detail = _register_pressure_proxy(
        orig_pa, cand_pa, orig_sm, cand_sm, spill_h, isa)

    dep_delta, dep_detail = _dependency_chain_delta(
        _region_text(orig_dir), _region_text(cand_dir), isa,
        orig_dir, cand_dir, orig_pa, cand_pa)
    if m1_spill_advisory:
        dep_delta = "unknown"

    call_score, call_detail = _call_boundary_score(orig_pa, cand_pa, orig_rep, cand_rep)

    orig_bench = scenario_mod._load_scenario_bench(
        fn, scenario, arch, flags, compiler, variant=None)
    cand_bench = scenario_mod._load_scenario_bench(
        fn, scenario, arch, flags, compiler, variant=variant)
    bench_h = _benchmark_heuristic(orig_bench, cand_bench)

    acc_h = _accuracy_heuristic(fn, scenario_rec, cand_rep, variant)
    static_d = _static_delta(orig_rep, cand_rep)

    static_improved = (
        static_d.get("throughput_delta", 0) < 0
        or spill_h.get("spill_delta", 0) < 0
        or reg_score == "improved")
    benchmark_win = bench_h.get("benchmark_status") == "pass"

    cpu_ann = (cand_rep.get("cpu_notes") or cand_rep.get("cpu_knowledge")
               or cpu_kb.annotate_scenario_report(
                   cand_rep, cand_pa, cand_dir))

    heuristics_for_decide = {
        "reject_candidate": reject,
        "staleness": staleness.get("staleness"),
        "path_safety": path_safety,
        "call_boundary": call_score,
        "candidate_accuracy_simple": acc_h.get("candidate_accuracy_simple"),
        "candidate_accuracy_groups": acc_h.get("candidate_accuracy_groups"),
        "production_accuracy_status": acc_h["production_accuracy"].get("simple",
                                                                        acc_h["production_accuracy"].get("status")),
        "special_value_status": acc_h["special_value_status"],
        "benchmark_status": bench_h.get("benchmark_status"),
        "static_improved": static_improved,
        "benchmark_win": benchmark_win,
    }
    decision = _decide_scenario(fn, arch, heuristics_for_decide, m1_spill_advisory)

    result = {
        "variant": variant,
        "scenario": scenario,
        "source_kind": "candidate",
        "staleness": staleness.get("staleness"),
        "path_classification": cand_rep.get("observed_path"),
        "path_mismatch": not cand_rep.get("path_match", True),
        "reject_candidate": reject,
        "source_map": cand_rep.get("source_map"),
        "source_map_confidence": sm_conf,
        "kernel_exposed": kernel_exposed,
        "instruction_count": cand_pa.get("instruction_count"),
        "instruction_count_delta": reg_detail.get("instruction_delta"),
        "static_model_estimate": cand_rep.get("static_model_advisory", {}).get("block_rthroughput"),
        "static_delta": static_d,
        "ipc_estimate": cand_rep.get("static_model_advisory", {}).get("ipc"),
        "path_safety": path_safety,
        "path_signals": path_signals,
        "spill_heuristic": spill_h,
        "register_pressure_proxy": reg_score,
        "register_pressure_detail": reg_detail,
        "dependency_chain_delta": dep_delta,
        "dependency_chain_detail": dep_detail,
        "call_boundary": call_score,
        "call_boundary_detail": call_detail,
        "benchmark": bench_h,
        "accuracy": acc_h,
        "constexpr_status": "not_applicable",
        "decision": decision,
        "note_warnings": [
            w.get("message") for w in (cpu_ann.get("warnings") or [])
        ],
        "m1_spill_ranking_suppressed": m1_spill_advisory,
    }
    return result


def aggregate_variant_ranking(scenario_results):
    """Worst decision across scenarios defines variant standing."""
    by_variant = {}
    for row in scenario_results:
        v = row["variant"]
        by_variant.setdefault(v, []).append(row)
    ranking = []
    for variant, rows in sorted(by_variant.items()):
        decisions = [r.get("decision", "incomplete") for r in rows]
        worst = min(decisions, key=lambda d: DECISION_PRIORITY.get(d, 99))
        wins = []
        risks = []
        for r in rows:
            if r.get("benchmark", {}).get("benchmark_status") == "pass":
                wins.append("%s bench" % r["scenario"])
            if r.get("spill_heuristic", {}).get("spill_delta", 0) < 0:
                wins.append("%s spill" % r["scenario"])
            if r["decision"].startswith("reject"):
                risks.append("%s: %s" % (r["scenario"], r["decision"]))
            if r.get("m1_spill_ranking_suppressed"):
                risks.append("%s: M1 thin slice" % r["scenario"])
        best_scenario = None
        best_bench = None
        for r in rows:
            bd = r.get("benchmark", {}).get("benchmark_delta_ns")
            if bd is not None and (best_bench is None or bd < best_bench):
                best_bench = bd
                best_scenario = r["scenario"]
        non_reject = [r for r in rows if not r.get("decision", "").startswith("reject")]
        ranking.append({
            "variant": variant,
            "decision": worst,
            "priority": DECISION_PRIORITY.get(worst, 99),
            "scenarios_tested": len(rows),
            "best_scenario": best_scenario,
            "major_wins": wins[:5],
            "major_risks": risks[:5],
            "static_only": bool(non_reject) and all(
                r["decision"] == "static_only" for r in non_reject),
        })
    ranking.sort(key=lambda x: (-x["priority"], x["variant"]))
    for i, ent in enumerate(ranking, 1):
        ent["rank"] = i
    return ranking


def run_compare(fn, variants, scenarios, arch, flags, compiler,
                source_map=False, run_bench=True, force=False):
    if not variants:
        variants = list_workspace_variants(fn)
    if not scenarios:
        scenarios = list_function_scenarios(fn)

    scenario_results = []
    warnings = []

    print("Comparing %s on %s: %d variants x %d scenarios" % (
        fn, arch, len(variants), len(scenarios)))

    import variant_accuracy as va_mod
    for variant in variants:
        acc = va_mod.run_variant_accuracy(fn, variant, mode="simple")
        print("  accuracy %s: %s" % (variant, acc.get("status")))

    for scenario in scenarios:
        print("  scenario: %s" % scenario)
        scenario_rows = []
        for variant in variants:
            try:
                row = compare_variant_scenario(
                    fn, variant, scenario, arch, flags, compiler,
                    source_map=source_map, run_bench=run_bench, force=force)
                scenario_rows.append(row)
                scenario_results.append(row)
                print("    %s -> %s" % (variant, row["decision"]))
            except Exception as exc:
                decision = "reject_path_mismatch"
                if "emit failed" in str(exc):
                    decision = "reject_path_mismatch"
                err = {
                    "variant": variant,
                    "scenario": scenario,
                    "decision": decision,
                    "error": str(exc),
                    "reject_candidate": True,
                }
                scenario_results.append(err)
                warnings.append("%s/%s: %s" % (variant, scenario, exc))
                print("    %s -> error: %s" % (variant, exc))
        out_dir = compare_out_dir(fn, arch) / "scenarios"
        out_dir.mkdir(parents=True, exist_ok=True)
        (out_dir / ("%s.json" % scenario)).write_text(
            json.dumps({"scenario": scenario, "results": scenario_rows}, indent=2) + "\n")

    ranking = aggregate_variant_ranking(scenario_results)

    rejections = [
        {"variant": r["variant"], "scenario": r["scenario"], "decision": r["decision"],
         "reason": r.get("path_signals") or r.get("note_warnings")}
        for r in scenario_results
        if r.get("decision", "").startswith("reject")
    ]
    static_only = [
        r["variant"] for r in ranking if r.get("static_only")
    ]
    sm_notes = []
    for r in scenario_results:
        conf = (r.get("source_map_confidence") or {})
        if conf.get("advisory"):
            sm_notes.append("%s/%s: %s" % (
                r.get("variant"), r.get("scenario"), conf.get("reason")))

    report = {
        "function": fn,
        "arch": arch,
        "compiler": compiler,
        "flags": flags,
        "generated_at": datetime.now(timezone.utc).isoformat(),
        "scenarios": scenarios,
        "variants": variants,
        "ranking": ranking,
        "scenario_results": scenario_results,
        "decisions": {r["variant"]: r["decision"] for r in ranking},
        "rejections": rejections,
        "static_only_variants": static_only,
        "source_map_notes": sm_notes,
        "warnings": warnings,
        "recommendation": _recommendation(ranking, scenario_results, arch, fn),
    }

    out_root = compare_out_dir(fn, arch)
    out_root.mkdir(parents=True, exist_ok=True)
    (out_root / "compare.json").write_text(json.dumps(report, indent=2) + "\n")
    (out_root / "compare.md").write_text(render_compare_md(report) + "\n")
    return out_root, report


def _recommendation(ranking, scenario_results, arch, fn):
    if not ranking:
        return "no candidates found; run variant init first"
    top = ranking[0]
    if top["decision"].startswith("reject"):
        return "reject all listed candidates; fix path or accuracy before review"
    if top["decision"] == "incomplete":
        return "run missing benchmarks and rigorous accuracy gates before merge review"
    if top["decision"] == "static_only":
        return "static metrics improved, run bench and candidate accuracy before merge"
    if any(r.get("m1_spill_ranking_suppressed") for r in scenario_results):
        return "M1 comparisons are scope-limited; prefer x86-64-v3 for spill ranking"
    if top["decision"] == "ready_for_review":
        return "run rigorous gate then review the ranking table"
    return "review ranking table and run missing evidence"


def render_compare_md(report):
    lines = [
        "# Candidate comparison: %s" % report["function"],
        "",
        "- arch: %s" % report["arch"],
        "- compiler: %s / %s" % (report["compiler"], report["flags"]),
        "- generated: %s" % report.get("generated_at"),
        "- variants: %s" % ", ".join(report.get("variants", [])),
        "- scenarios: %s" % ", ".join(report.get("scenarios", [])),
        "",
        "## Summary ranking",
        "",
        "| Rank | Candidate | Decision | Best scenario | Major wins | Major risks |",
        "| --- | --- | --- | --- | --- | --- |",
    ]
    for ent in report.get("ranking", []):
        wins = "; ".join(ent.get("major_wins", [])[:3]) or "-"
        risks = "; ".join(ent.get("major_risks", [])[:3]) or "-"
        lines.append("| %d | %s | %s | %s | %s | %s |" % (
            ent.get("rank"), ent["variant"], ent["decision"],
            ent.get("best_scenario") or "-", wins, risks))

    lines.extend([
        "",
        "## Scenario matrix",
        "",
        "| Scenario | Candidate | Path | Spills d | Calls d | Static d | Bench d | Prod acc | Cand acc | Decision |",
        "| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |",
    ])
    for r in report.get("scenario_results", []):
        if "error" in r:
            lines.append("| %s | %s | error | - | - | - | - | - | - | incomplete |" % (
                r.get("scenario"), r.get("variant")))
            continue
        sh = r.get("spill_heuristic", {})
        spill_s = "%+d" % sh.get("spill_delta", 0)
        if sh.get("confidence") == "advisory":
            spill_s += " (adv)"
        call_d = r.get("call_boundary_detail", {}).get("call_delta", 0)
        static_d = r.get("static_delta", {}).get("throughput_delta", 0)
        bench_d = r.get("benchmark", {}).get("benchmark_delta_ns")
        bench_s = ("%+.2f ns" % bench_d) if bench_d is not None else r.get("benchmark", {}).get(
            "benchmark_status", "-")
        prod_acc = (r.get("accuracy", {}).get("production_accuracy") or {}).get("status", "-")
        cand_acc = (r.get("accuracy", {}).get("candidate_accuracy") or {}).get("status", "-")
        lines.append("| %s | %s | %s | %s | %+d | %+g | %s | %s | %s | %s |" % (
            r["scenario"], r["variant"], r.get("path_classification", "-"),
            spill_s, call_d, static_d, bench_s, prod_acc, cand_acc, r.get("decision")))

    lines.extend(["", "## Rejections", ""])
    rej = report.get("rejections", [])
    if not rej:
        lines.append("- none")
    else:
        for r in rej:
            lines.append("- **%s** / %s: %s" % (r["variant"], r["scenario"], r["decision"]))

    lines.extend(["", "## Static-only warnings", ""])
    so = report.get("static_only_variants", [])
    if not so:
        lines.append("- none flagged as static-only across all scenarios")
    else:
        for v in so:
            lines.append("- %s: static metrics improved without benchmark proof" % v)

    lines.extend(["", "## Source-map notes", ""])
    sm = report.get("source_map_notes", [])
    if not sm:
        lines.append("- no low-confidence mapping warnings")
    else:
        for n in sm:
            lines.append("- %s" % n)

    lines.extend(["", "## CPU knowledge warnings", ""])
    cpu_warn = set()
    for r in report.get("scenario_results", []):
        for w in r.get("note_warnings") or r.get("cpu_knowledge_warnings") or []:
            cpu_warn.add(w)
    if not cpu_warn:
        lines.append("- none")
    else:
        for w in sorted(cpu_warn):
            lines.append("- %s" % w)

    lines.extend([
        "",
        "## Recommendation",
        "",
        report.get("recommendation", ""),
        "",
    ])
    return "\n".join(lines)
