#!/usr/bin/env python3
# Copyright (c) Ian Pike
# Copyright (c) CCMath contributors
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
"""Candidate-specific accuracy and edge-case conformance for isolated variants."""

import json
import sys
from datetime import datetime, timezone
from pathlib import Path

import _asmlab_common as C
import variant as variant_mod

BUILD_DIR = C.OUT_DIR / "accuracy-build"
GATES_DIR = C.OUT_DIR / "gates" / "candidates"
INPUTS_PATH = C.ASMLAB_DIR / "registry" / "candidate_edge_case_inputs.json"

SUPPORTED_FUNCTIONS = {"powf_impl"}


def _load_inputs_registry():
    if not INPUTS_PATH.exists():
        return {}
    return C.load_json_registry(INPUTS_PATH)


def _gate_path(fn, variant, mode):
    return GATES_DIR / ("%s-%s-%s.json" % (fn, variant, mode))


def _cmake_configure(variant_name, variant_dir):
    BUILD_DIR.mkdir(parents=True, exist_ok=True)
    cmd = [
        "cmake", "-S", str(C.PROJECT_ROOT), "-B", str(BUILD_DIR), "-G", "Ninja",
        "-DCMAKE_BUILD_TYPE=Release",
        "-DCCMATH_BUILD_BENCHMARKS=ON",
        "-DCCMATH_BUILD_TESTS=OFF",
        "-DCCMATH_BUILD_EXAMPLES=OFF",
        "-DCCM_BENCH_POWER=ON",
        "-DASMLAB_POWF_IMPL_VARIANT_NAME=%s" % variant_name,
        "-DASMLAB_POWF_IMPL_VARIANT_DIR=%s" % variant_dir,
    ]
    res = C.run(cmd)
    return res.returncode == 0, res.stderr


def _cmake_build(target):
    res = C.run(["cmake", "--build", str(BUILD_DIR), "--target", target])
    return res.returncode == 0, res.stderr


def _find_binary(name):
    for p in BUILD_DIR.rglob(name):
        if p.is_file() and (p.stat().st_mode & 0o111):
            return p
    return None


def _run_case(binary, case):
    cmd = [str(binary), "--check", case.get("check", "ulp"),
           "--base", str(case["base"]), "--exp", str(case["exp"])]
    if case.get("check", "ulp") == "ulp":
        cmd += ["--max-ulp", str(case.get("max_ulp", 4))]
    res = C.run(cmd)
    return {
        "check": case.get("check", "ulp"),
        "base": case["base"],
        "exp": case["exp"],
        "status": "pass" if res.returncode == 0 else "fail",
        "stdout": (res.stdout or "").strip()[-200:],
        "stderr": (res.stderr or "").strip()[-400:],
    }


def _production_accuracy_summary(fn, mode):
    edge_doc = C.load_candidate_edge_cases()
    ulp_label = edge_doc.get("functions", {}).get(fn, {}).get("ulp_label", "powf")
    simple = C.production_gate_result(ulp_label, "simple")
    rigorous = C.production_gate_result(ulp_label, "rigorous") if mode == "rigorous" else None
    return {
        "simple": simple or "not_run",
        "rigorous": rigorous or ("not_wired" if mode == "rigorous" else "not_run"),
        "scope": "production",
        "note": "Unchanged include/ccmath/ paths only.",
    }


def _group_status(group_results, wired):
    if not wired:
        return "not_wired"
    if not group_results:
        return "not_wired"
    if any(r.get("status") == "fail" for r in group_results):
        return "fail"
    return "pass"


def _aggregate_candidate_status(group_statuses, mode):
    if mode == "rigorous":
        return "not_wired"
    wired = {k: v for k, v in group_statuses.items() if v != "not_wired"}
    if not wired:
        return "not_wired"
    if any(v == "fail" for v in wired.values()):
        return "fail"
    return "pass"


def run_variant_accuracy(fn, variant, mode="simple", write_gate=True):
    if fn not in SUPPORTED_FUNCTIONS:
        C.fail("variant accuracy not implemented for '%s'" % fn)

    manifest = variant_mod.load_manifest(fn, variant)
    stale = variant_mod.check_staleness(manifest)
    if stale.get("staleness") == "stale":
        return {
            "function": fn,
            "variant": variant,
            "mode": mode,
            "status": "fail",
            "reason": stale.get("warning"),
            "staleness": "stale",
        }

    vdir = variant_mod.variant_workspace(fn, variant)
    if not (vdir / "candidate.hpp").exists():
        C.fail("variant not initialized: %s" % vdir)

    if mode == "rigorous":
        report = {
            "function": fn,
            "variant": variant,
            "mode": mode,
            "timestamp": datetime.now(timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ"),
            "staleness": stale.get("staleness"),
            "status": "not_wired",
            "production_accuracy": _production_accuracy_summary(fn, mode),
            "candidate_accuracy": {
                "simple": "not_run",
                "rigorous": "not_wired",
                "scope": "candidate",
                "note": (
                    "Candidate-specific MPFR rigorous campaign not wired. "
                    "Use simple mode for std::powf oracle probes."
                ),
                "groups": {
                    k: "not_wired"
                    for k in C.load_candidate_edge_cases()
                    .get("functions", {})
                    .get(fn, {})
                    .get("edge_case_groups", {})
                },
            },
            "group_results": {},
            "constexpr_status": "not_wired",
        }
        if write_gate:
            _write_gate_report(report)
        return report

    ok, err = _cmake_configure(variant, str(vdir))
    if not ok:
        return {
            "function": fn,
            "variant": variant,
            "mode": mode,
            "status": "fail",
            "reason": "cmake configure failed",
            "stderr": err[-2000:],
        }

    bin_name = "asmlab_candidate_accuracy_powf_impl_%s" % variant
    if not _cmake_build(bin_name):
        return {
            "function": fn,
            "variant": variant,
            "mode": mode,
            "status": "fail",
            "reason": "build failed for %s" % bin_name,
        }

    binary = _find_binary(bin_name)
    if not binary:
        return {
            "function": fn,
            "variant": variant,
            "mode": mode,
            "status": "fail",
            "reason": "binary not found: %s" % bin_name,
        }

    inputs_doc = _load_inputs_registry().get(fn, {})
    edge_doc = C.load_candidate_edge_cases().get("functions", {}).get(fn, {})
    all_groups = edge_doc.get("edge_case_groups", {})

    group_results = {}
    group_statuses = {}
    for group_name in sorted(all_groups.keys()):
        spec = inputs_doc.get(group_name, {})
        wired = spec.get("wired", False)
        if not wired or mode not in spec.get("modes", ["simple"]):
            group_statuses[group_name] = "not_wired"
            continue
        results = []
        for case in spec.get("cases", []):
            results.append(_run_case(binary, case))
        group_results[group_name] = results
        group_statuses[group_name] = _group_status(results, wired=True)

    cand_simple = _aggregate_candidate_status(group_statuses, mode)
    report = {
        "function": fn,
        "variant": variant,
        "mode": mode,
        "timestamp": datetime.now(timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ"),
        "staleness": stale.get("staleness"),
        "status": cand_simple,
        "production_accuracy": _production_accuracy_summary(fn, mode),
        "candidate_accuracy": {
            "simple": cand_simple if mode == "simple" else "not_run",
            "rigorous": "not_wired",
            "scope": "candidate",
            "oracle": "std::powf",
            "max_ulp": 4,
            "note": "Isolated variant TU. Not the production powf gate.",
            "groups": group_statuses,
        },
        "group_results": group_results,
        "constexpr_status": "advisory",
        "binary": str(binary),
    }
    if write_gate:
        _write_gate_report(report)
    return report


def _write_gate_report(report):
    GATES_DIR.mkdir(parents=True, exist_ok=True)
    path = _gate_path(report["function"], report["variant"], report["mode"])
    path.write_text(json.dumps(report, indent=2) + "\n")
    return path


def load_candidate_accuracy_report(fn, variant, mode="simple"):
    path = _gate_path(fn, variant, mode)
    if not path.exists():
        return None
    return json.loads(path.read_text())


def candidate_accuracy_field(fn, variant=None, mode="simple"):
    if not variant:
        return {
            "status": "not_applicable",
            "scope": "candidate",
            "note": "Original implementation report, no candidate under test.",
        }
    report = load_candidate_accuracy_report(fn, variant, mode)
    if not report:
        edge_doc = C.load_candidate_edge_cases()
        groups = edge_doc.get("functions", {}).get(fn, {}).get("edge_case_groups", {})
        return {
            "status": "not_run",
            "scope": "candidate",
            "variant": variant,
            "simple": "not_run",
            "rigorous": "not_wired",
            "groups": {k: "not_wired" for k in groups},
            "note": "Run: asmlab.py variant accuracy %s %s --mode simple" % (fn, variant),
        }
    cand = report.get("candidate_accuracy", {})
    return {
        "status": cand.get("simple") if mode == "simple" else cand.get("rigorous", "not_wired"),
        "scope": "candidate",
        "variant": variant,
        "simple": cand.get("simple", "not_run"),
        "rigorous": cand.get("rigorous", "not_wired"),
        "groups": cand.get("groups", {}),
        "production_accuracy": report.get("production_accuracy"),
        "note": cand.get("note"),
        "gate_file": str(_gate_path(fn, variant, mode)),
    }


def render_accuracy_md(report):
    lines = [
        "# Candidate accuracy: %s / %s" % (report["function"], report["variant"]),
        "",
        "- mode: %s" % report.get("mode"),
        "- status: %s" % report.get("status"),
        "- staleness: %s" % report.get("staleness"),
        "",
        "## Production accuracy",
        "",
    ]
    prod = report.get("production_accuracy", {})
    lines.append("- simple: %s" % prod.get("simple"))
    lines.append("- rigorous: %s" % prod.get("rigorous"))
    lines.extend([
        "",
        "## Candidate accuracy",
        "",
        "- simple: %s" % report.get("candidate_accuracy", {}).get("simple"),
        "- rigorous: %s" % report.get("candidate_accuracy", {}).get("rigorous"),
        "",
        "## Edge-case groups",
        "",
        "| Group | Status | Cases |",
        "| --- | --- | --- |",
    ])
    groups = report.get("candidate_accuracy", {}).get("groups", {})
    results = report.get("group_results", {})
    for name in sorted(groups.keys()):
        n = len(results.get(name, []))
        lines.append("| %s | %s | %d |" % (name, groups[name], n))
    return "\n".join(lines)


def main(argv=None):
    import argparse
    ap = argparse.ArgumentParser(description="Candidate-specific accuracy gate")
    ap.add_argument("fn")
    ap.add_argument("variant")
    ap.add_argument("--mode", default="simple", choices=["simple", "rigorous"])
    args = ap.parse_args(argv)
    report = run_variant_accuracy(args.fn, args.variant, mode=args.mode)
    md_path = variant_mod.variant_workspace(args.fn, args.variant) / (
        "accuracy_%s.md" % args.mode)
    md_path.write_text(render_accuracy_md(report) + "\n")
    print(json.dumps({
        "status": report.get("status"),
        "production_accuracy": report.get("production_accuracy"),
        "candidate_accuracy": report.get("candidate_accuracy"),
        "gate_file": str(_gate_path(args.fn, args.variant, args.mode)),
    }, indent=2))
    return 0 if report.get("status") in ("pass", "not_wired") else 1


if __name__ == "__main__":
    sys.exit(main())
