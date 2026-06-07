#!/usr/bin/env python3
# Copyright (c) Ian Pike
# Copyright (c) CCMath contributors
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
"""Golden calibration runner: Horner vs Estrin analysis validation."""

import argparse
import json
import re
import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path

import _asmlab_common as C
import golden_emit as gemit
import golden_findings as gfind

GOLDEN_OUT = C.OUT_DIR / "golden"
HISTORY_DIR = GOLDEN_OUT / "history"
BASELINE_FN = "golden_poly_horner"
CANDIDATE_FN = "golden_poly_estrin"
BENCH_BUILD = C.OUT_DIR / "golden-bench-build"


def _run_golden_bench(quick=False):
    """Build and run asmlab_bench_golden_poly (no ccmath dependency)."""
    BENCH_BUILD.mkdir(parents=True, exist_ok=True)
    min_time = "0.02s" if quick else "0.05s"
    cmd = [
        "cmake", "-S", str(C.PROJECT_ROOT), "-B", str(BENCH_BUILD), "-G", "Ninja",
        "-DCMAKE_BUILD_TYPE=Release",
        "-DCCMATH_BUILD_BENCHMARKS=ON",
        "-DCCMATH_BUILD_TESTS=OFF",
        "-DCCMATH_BUILD_EXAMPLES=OFF",
        "-DCCM_BENCH_POWER=ON",
    ]
    if C.run(cmd).returncode != 0:
        return {"status": "failed", "reason": "cmake configure failed"}

    if C.run(["cmake", "--build", str(BENCH_BUILD),
              "--target", "asmlab_bench_golden_poly"]).returncode != 0:
        return {"status": "failed", "reason": "build asmlab_bench_golden_poly failed"}

    binary = None
    for p in BENCH_BUILD.rglob("asmlab_bench_golden_poly"):
        if p.is_file() and (p.stat().st_mode & 0o111):
            binary = p
            break
    if not binary:
        return {"status": "failed", "reason": "golden bench binary not found"}

    run_cmd = [
        str(binary),
        "--benchmark_filter=BM_golden_poly",
        "--benchmark_min_time=%s" % min_time,
    ]
    res = C.run(run_cmd)
    text = res.stdout + res.stderr
    horner_ns = _parse_bench_ns(text, "horner")
    estrin_ns = _parse_bench_ns(text, "estrin")
    status = "ran" if horner_ns and estrin_ns and res.returncode == 0 else "failed"
    return {
        "status": status,
        "horner_ns": horner_ns,
        "estrin_ns": estrin_ns,
        "benchmark_direction_matches": (
            estrin_ns <= horner_ns * 1.05 if horner_ns and estrin_ns else None),
        "raw_excerpt": text[-2500:],
    }


def _parse_bench_ns(text, kind):
    # Google Benchmark reports Time then CPU columns. Use CPU (second ns value).
    pattern = r"BM_golden_poly_%s[^\n]*\n\s*[\d.]+\s*ns\s+([\d.]+)\s*ns" % kind
    m = re.search(pattern, text)
    if m:
        return float(m.group(1))
    vals = [float(x) for x in re.findall(r"([\d.]+)\s*ns", text)]
    return vals[0] if vals else None


def _write_history(report, arch, flags, compiler):
    HISTORY_DIR.mkdir(parents=True, exist_ok=True)
    ts = datetime.now(timezone.utc).strftime("%Y%m%dT%H%M%SZ")
    entry = {
        "timestamp": datetime.now(timezone.utc).isoformat(),
        "arch": arch,
        "flags": flags,
        "compiler": compiler,
        "passed": report.get("passed"),
        "summary": report.get("summary"),
    }
    hist_path = HISTORY_DIR / ("run_%s.json" % ts)
    hist_path.write_text(json.dumps(entry, indent=2) + "\n")
    (HISTORY_DIR / "latest.json").write_text(json.dumps(entry, indent=2) + "\n")

    entries = []
    index_path = HISTORY_DIR / "index.json"
    if index_path.exists():
        entries = json.loads(index_path.read_text())
    entries.append({"file": hist_path.name, "timestamp": entry["timestamp"],
                      "passed": entry["passed"]})
    index_path.write_text(json.dumps(entries, indent=2) + "\n")
    return hist_path


def _render_md(report):
    lines = [
        "# Golden analysis report",
        "",
        "- passed: %s" % report.get("passed"),
        "- arch: %s" % report.get("arch"),
        "- mode: %s" % report.get("mode"),
        "",
        "## Summary",
        "",
    ]
    for k, v in (report.get("summary") or {}).items():
        lines.append("- %s: %s" % (k, v))
    lines.extend(["", "## Validation checks", ""])
    for k, v in (report.get("validation", {}).get("checks") or {}).items():
        lines.append("- %s: %s" % (k, v))
    lines.extend(["", "## Ranked findings (Estrin vs Horner)", ""])
    for f in report.get("pair", {}).get("ranked_findings", []):
        lines.append("- [%d] %s (%s): %s" % (
            f.get("priority"), f.get("id"), f.get("confidence"), f.get("evidence")))
    lines.extend(["", "## Benchmark", ""])
    bench = report.get("benchmark", {})
    lines.append("- status: %s" % bench.get("status"))
    lines.append("- horner_ns: %s" % bench.get("horner_ns"))
    lines.append("- estrin_ns: %s" % bench.get("estrin_ns"))
    lines.append("- direction_matches: %s" % bench.get("benchmark_direction_matches"))
    lines.append("")
    lines.append("Golden analysis validates analysis quality, not numerical correctness.")
    return "\n".join(lines)


def run_golden(arch="x86-64-v3", flags="O2", compiler="clang", quick=False,
               skip_bench=False):
    mode = "quick" if quick else "full"
    print(">> golden analysis: %s / %s / %s (%s)" % (BASELINE_FN, CANDIDATE_FN, arch, mode))

    hdir = gemit.emit_golden(BASELINE_FN, arch, flags, compiler,
                             source_map=True, deep_analyze=True)
    edir = gemit.emit_golden(CANDIDATE_FN, arch, flags, compiler,
                             source_map=True, deep_analyze=True)
    if not hdir or not edir:
        report = {"passed": False, "error": "golden emit failed", "mode": mode}
        _write_outputs(report)
        return report

    pair = gfind.extract_pair_findings(hdir, edir)
    bench = {"status": "skipped"} if skip_bench else _run_golden_bench(quick=quick)
    validation = gfind.validate_pair(pair, bench if bench.get("status") == "ran" else None)

    report = {
        "passed": validation["passed"],
        "mode": mode,
        "arch": arch,
        "flags": flags,
        "compiler": compiler,
        "baseline_dir": str(hdir),
        "candidate_dir": str(edir),
        "pair": pair,
        "benchmark": bench,
        "validation": validation,
        "summary": {
            "mca_prediction_correct": validation["checks"].get("mca_prediction_correct"),
            "benchmark_direction_matches": validation["checks"].get(
                "benchmark_direction_matches"),
            "ranking_quality_ok": validation["checks"].get("ranking_quality_ok"),
            "matched_expected": validation["checks"].get("expected_findings_matched"),
            "missing_expected": validation["checks"].get("expected_findings_missing"),
        },
        "timestamp": datetime.now(timezone.utc).isoformat(),
    }

    diff_path = edir / "golden_analysis_diff.json"
    diff_path.write_text(json.dumps(pair.get("analysis_diff", {}), indent=2) + "\n")

    _write_history(report, arch, flags, compiler)
    _write_outputs(report)
    return report


def _write_outputs(report):
    GOLDEN_OUT.mkdir(parents=True, exist_ok=True)
    (GOLDEN_OUT / "golden_report.json").write_text(json.dumps(report, indent=2) + "\n")
    (GOLDEN_OUT / "golden_report.md").write_text(_render_md(report) + "\n")


def main(argv=None):
    ap = argparse.ArgumentParser(description="Run Horner vs Estrin golden analysis validation.")
    ap.add_argument("--arch", default="x86-64-v3")
    ap.add_argument("--flags", default=C.DEFAULT_FLAGS, choices=list(C.FLAG_VARIANTS))
    ap.add_argument("--compiler", default="clang", choices=["clang", "gcc"])
    ap.add_argument("--quick", action="store_true",
                    help="CI smoke: short benchmark, structural validation")
    ap.add_argument("--skip-bench", action="store_true")
    args = ap.parse_args(argv)

    report = run_golden(
        arch=args.arch, flags=args.flags, compiler=args.compiler,
        quick=args.quick, skip_bench=args.skip_bench)

    print("golden analysis: passed=%s" % report.get("passed"))
    if report.get("validation"):
        print("  checks: %s" % json.dumps(report["validation"].get("checks"), indent=2))
    return 0 if report.get("passed") else 1


if __name__ == "__main__":
    sys.exit(main())
