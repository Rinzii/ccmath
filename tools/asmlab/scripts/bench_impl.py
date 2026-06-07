#!/usr/bin/env python3
# Copyright (c) Ian Pike
# Copyright (c) CCMath contributors
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
"""Isolated impl-kernel benchmarks for asmlab (original and candidate variants)."""

import json
import re
import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path

import _asmlab_common as C

BUILD_DIR = C.OUT_DIR / "bench-build"
BENCHES_DIR = C.OUT_DIR / "benches"

PROFILE_FILTERS = {
    "positive_finite_general": "BM_asmlab_powf_impl_positive_finite_general",
    "near_one": "BM_asmlab_powf_impl_near_one",
    "integer_exponent": "BM_asmlab_powf_impl_integer_exponent",
}


def resolve_target(fn):
    profiles = C.load_benchmark_profiles()
    tgt = profiles.get("benchmark_targets", {}).get(fn, {})
    if not tgt:
        return None
    return tgt


def _cmake_configure(variant_name=None, variant_dir=None):
    BUILD_DIR.mkdir(parents=True, exist_ok=True)
    gen_args, _ = C.cmake_generator_args()
    cmd = [
        "cmake", "-S", str(C.PROJECT_ROOT), "-B", str(BUILD_DIR),
        *gen_args,
        "-DCCMATH_BUILD_BENCHMARKS=ON",
        "-DCCMATH_BUILD_TESTS=OFF",
        "-DCCMATH_BUILD_EXAMPLES=OFF",
        "-DCCM_BENCH_POWER=ON",
    ]
    if variant_name and variant_dir:
        cmd += [
            "-DASMLAB_POWF_IMPL_VARIANT_NAME=%s" % variant_name,
            "-DASMLAB_POWF_IMPL_VARIANT_DIR=%s" % variant_dir,
        ]
    res = C.run(cmd)
    return res.returncode == 0, res.stderr


def _cmake_build(target):
    _, build_config = C.cmake_generator_args()
    res = C.run(C.cmake_build_command(BUILD_DIR, target, build_config))
    return res.returncode == 0, res.stderr


def _find_binary(name):
    return C.find_built_program(BUILD_DIR, name)


def _parse_ns(output):
    vals = [float(x) for x in re.findall(r"([\d.]+)\s*ns", output)]
    return min(vals) if vals else None


def run_powf_impl_bench(profile, variant=None, mode="", extra_args=None):
    """Run isolated powf_impl benchmark. Returns result dict with explicit status."""
    fn = "powf_impl"
    tgt = resolve_target(fn)
    if not tgt or not tgt.get("wired"):
        return {
            "function": fn,
            "profile": profile,
            "status": "unsupported",
            "reason": "powf_impl benchmark target not wired in registry",
        }
    if profile not in PROFILE_FILTERS:
        return {
            "function": fn,
            "profile": profile,
            "status": "not_configured",
            "reason": "no benchmark filter for profile %s" % profile,
        }

    variant_dir = None
    if variant:
        import variant as variant_mod
        vdir = variant_mod.variant_workspace(fn, variant)
        if not (vdir / "candidate.hpp").exists():
            return {
                "function": fn,
                "profile": profile,
                "variant": variant,
                "source_kind": "candidate",
                "status": "failed",
                "reason": "variant not initialized: %s" % vdir,
            }
        stale = variant_mod.check_staleness(variant_mod.load_manifest(fn, variant))
        if stale.get("staleness") == "stale":
            return {
                "function": fn,
                "profile": profile,
                "variant": variant,
                "source_kind": "candidate",
                "status": "failed",
                "reason": stale.get("warning"),
            }
        variant_dir = str(vdir)

    ok, err = _cmake_configure(variant, variant_dir)
    if not ok:
        return {
            "function": fn,
            "profile": profile,
            "variant": variant,
            "source_kind": "candidate" if variant else "original",
            "status": "failed",
            "reason": "cmake configure failed",
            "stderr": err[-2000:],
        }

    bin_name = "asmlab_bench_powf_impl"
    if variant:
        bin_name = "asmlab_bench_powf_impl_%s" % variant
    if not _cmake_build(bin_name):
        return {
            "function": fn,
            "profile": profile,
            "variant": variant,
            "source_kind": "candidate" if variant else "original",
            "status": "failed",
            "reason": "build failed for %s" % bin_name,
        }

    binary = _find_binary(bin_name)
    if not binary:
        return {
            "function": fn,
            "profile": profile,
            "variant": variant,
            "status": "failed",
            "reason": "binary not found: %s" % bin_name,
        }

    bench_filter = PROFILE_FILTERS[profile]
    run_cmd = [str(binary), "--benchmark_filter=%s" % bench_filter,
               "--benchmark_min_time=0.05s"]
    if extra_args:
        run_cmd += extra_args
    res = C.run(run_cmd)
    result_ns = _parse_ns(res.stdout + res.stderr)
    status = "ran" if result_ns is not None and res.returncode == 0 else "failed"

    out = {
        "function": fn,
        "profile": profile,
        "variant": variant,
        "source_kind": "candidate" if variant else "original",
        "status": status,
        "benchmark_target": bin_name,
        "benchmark_filter": bench_filter,
        "benchmark_mode": mode or "latency",
        "result_ns": result_ns,
        "timestamp": datetime.now(timezone.utc).isoformat(),
        "merge_grade": "advisory",
        "raw_excerpt": (res.stdout + res.stderr)[-3000:],
    }
    if status == "failed":
        out["reason"] = "benchmark exited %d or no ns parsed" % res.returncode
    return out


def write_bench_artifact(result, scenario_name=None):
    BENCHES_DIR.mkdir(parents=True, exist_ok=True)
    parts = [result["function"], result.get("profile", "default")]
    if result.get("variant"):
        parts += ["variant", result["variant"]]
    if scenario_name:
        parts = [result["function"], "scenarios", scenario_name]
        if result.get("variant"):
            parts += ["variants", result["variant"]]
        parts += [result.get("profile", "default")]
    path = BENCHES_DIR / ("-".join(parts) + ".json")
    path.write_text(json.dumps(result, indent=2) + "\n")
    return path


def run_scenario_bench(fn, scenario_name, scenario_rec, variant=None):
    """Run profile benchmark for a scenario and write scenario_bench.json under scenario dir."""
    profile = scenario_rec.get("benchmark_profile")
    if not profile:
        result = {
            "function": fn,
            "scenario": scenario_name,
            "variant": variant,
            "status": "not_configured",
            "reason": "scenario has no benchmark_profile",
        }
        return result

    if fn == "powf_impl":
        result = run_powf_impl_bench(profile, variant=variant)
    else:
        tgt = resolve_target(fn)
        if not tgt or not tgt.get("wired"):
            result = {
                "function": fn,
                "scenario": scenario_name,
                "profile": profile,
                "status": "unsupported",
                "reason": "no wired impl benchmark for %s" % fn,
            }
        else:
            result = {
                "function": fn,
                "scenario": scenario_name,
                "profile": profile,
                "status": "unsupported",
                "reason": "bench_impl.py has no runner for %s yet" % fn,
            }

    result["scenario"] = scenario_name
    arch_placeholder = "host"
    sdir = C.scenario_dir(
        fn, scenario_name, arch_placeholder, C.DEFAULT_FLAGS, "clang",
        variant=variant)
    # Scenario bench is arch-agnostic runtime. Store beside first matching scenario emit if exists.
    candidates = list((C.OUT_DIR / fn / "scenarios" / scenario_name).rglob("scenario_report.json"))
    if variant:
        candidates = [p for p in candidates if("/variants/%s/" % variant) in str(p)]
    else:
        candidates = [p for p in candidates if "/variants/" not in str(p)]
    if candidates:
        sdir = candidates[0].parent
    sdir.mkdir(parents=True, exist_ok=True)
    (sdir / "scenario_bench.json").write_text(json.dumps(result, indent=2) + "\n")
    write_bench_artifact(result, scenario_name=scenario_name)
    return result


def bench_status_from_result(result):
    """Map bench result dict to scenario report label."""
    st = result.get("status", "unsupported")
    if st == "ran":
        return "ran"
    if st == "not_configured":
        return "not_configured"
    if st == "unsupported":
        return "unsupported"
    if st == "failed":
        return "failed"
    return "unsupported"


def main(argv=None):
    import argparse
    ap = argparse.ArgumentParser(description="Run asmlab impl-kernel benchmarks.")
    ap.add_argument("fn", nargs="?", default="powf_impl")
    ap.add_argument("--profile", default="positive_finite_general")
    ap.add_argument("--variant", default="")
    ap.add_argument("--scenario", default="")
    args = ap.parse_args(argv)

    if args.fn != "powf_impl":
        print("only powf_impl impl bench is implemented", file=sys.stderr)
        return 1

    if args.scenario:
        rec = C.get_scenario(args.fn, args.scenario)
        result = run_scenario_bench(args.fn, args.scenario, rec,
                                    variant=args.variant or None)
    else:
        result = run_powf_impl_bench(args.profile, variant=args.variant or None)
        write_bench_artifact(result)

    print(json.dumps(result, indent=2))
    path_info = ""
    return 0 if result.get("status") == "ran" else 1


if __name__ == "__main__":
    sys.exit(main())
