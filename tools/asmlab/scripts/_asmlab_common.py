#!/usr/bin/env python3
# Copyright (c) Ian Pike
# Copyright (c) CCMath contributors
#
# CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
# See LICENSE for more information.
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
"""Shared helpers for the asmlab scripts.

Standard library only, so it runs on the stock macOS Python 3.9 with no venv.
Everything here is read-only with respect to the ccmath library. Outputs go to
out/asmlab/ which is gitignored.
"""

import json
import os
import shutil
import subprocess
import sys
from pathlib import Path

# tools/asmlab/scripts/_asmlab_common.py -> repo root is three parents up.
PROJECT_ROOT = Path(__file__).resolve().parents[3]
ASMLAB_DIR = PROJECT_ROOT / "tools" / "asmlab"
REGISTRY_PATH = ASMLAB_DIR / "registry" / "functions.json"
ACCURACY_MANIFEST_PATH = ASMLAB_DIR / "registry" / "accuracy_manifest.json"
BENCHMARK_PROFILES_PATH = ASMLAB_DIR / "registry" / "benchmark_profiles.json"
PATH_CATEGORIES_PATH = ASMLAB_DIR / "registry" / "path_categories.json"
PATH_SCENARIOS_PATH = ASMLAB_DIR / "registry" / "path_scenarios.json"
CANDIDATE_EDGE_CASES_PATH = ASMLAB_DIR / "registry" / "candidate_edge_cases.json"
CANDIDATE_ACCURACY_STATUSES = frozenset({
    "pass", "fail", "not_run", "not_wired", "advisory", "production_only",
})
HARNESS_TEMPLATE = ASMLAB_DIR / "harness" / "harness_template.cpp"
CONSTEXPR_PROBE_TEMPLATE = ASMLAB_DIR / "harness" / "constexpr_probe_template.cpp"
REFDATA_DIR = ASMLAB_DIR / "refdata"
OUT_DIR = PROJECT_ROOT / "out" / "asmlab"
INCLUDE_DIR = PROJECT_ROOT / "include"

# Microarchitecture matrix. Each entry describes how to emit and how to score.
#   isa        : "x86" or "arm" (selects which analyzers apply and table source)
#   target     : clang --target triple, or None to use the native host target
#   emit_flags : list of codegen-selection flags (x86 uses -march, ARM uses -mcpu/-march)
#   mca_cpu    : -mcpu value for llvm-mca
#   mca_triple : -mtriple value for llvm-mca
#   uica       : True if uiCA models this core (recent Intel only)
#   uica_arch  : uiCA microarchitecture id (see uiCA docs). None when uica is False
#   note       : human description
ARCHES = {
    # --- Intel x86-64 ---
    "skylake": dict(isa="x86", target="x86_64-apple-macos", emit_flags=["-march=skylake"],
                    mca_cpu="skylake", mca_triple="x86_64-unknown-linux-gnu",
                    uica=True, uica_arch="SKL", note="Intel Skylake client"),
    "icelake-server": dict(isa="x86", target="x86_64-apple-macos", emit_flags=["-march=icelake-server"],
                           mca_cpu="icelake-server", mca_triple="x86_64-unknown-linux-gnu",
                           uica=True, uica_arch="ICL", note="Intel Ice Lake server"),
    "sapphirerapids": dict(isa="x86", target="x86_64-apple-macos", emit_flags=["-march=sapphirerapids"],
                           mca_cpu="sapphirerapids", mca_triple="x86_64-unknown-linux-gnu",
                           uica=True, uica_arch="ADL-P", note="Intel Golden Cove / SPR"),
    # --- AMD x86-64 (uiCA does not model Zen, llvm-mca + Agner only) ---
    "znver3": dict(isa="x86", target="x86_64-apple-macos", emit_flags=["-march=znver3"],
                   mca_cpu="znver3", mca_triple="x86_64-unknown-linux-gnu",
                   uica=False, uica_arch=None, note="AMD Zen 3"),
    "znver4": dict(isa="x86", target="x86_64-apple-macos", emit_flags=["-march=znver4"],
                   mca_cpu="znver4", mca_triple="x86_64-unknown-linux-gnu",
                   uica=False, uica_arch=None, note="AMD Zen 4"),
    "znver5": dict(isa="x86", target="x86_64-apple-macos", emit_flags=["-march=znver5"],
                   mca_cpu="znver5", mca_triple="x86_64-unknown-linux-gnu",
                   uica=False, uica_arch=None, note="AMD Zen 5"),
    # --- Portable x86-64 baselines (what a shipped library realistically targets) ---
    "x86-64-v2": dict(isa="x86", target="x86_64-apple-macos", emit_flags=["-march=x86-64-v2"],
                      mca_cpu="x86-64-v2", mca_triple="x86_64-unknown-linux-gnu",
                      uica=False, uica_arch=None, note="SSE4.2 baseline"),
    "x86-64-v3": dict(isa="x86", target="x86_64-apple-macos", emit_flags=["-march=x86-64-v3"],
                      mca_cpu="x86-64-v3", mca_triple="x86_64-unknown-linux-gnu",
                      uica=False, uica_arch=None, note="AVX2+FMA baseline"),
    # --- ARM64 / Apple Silicon (Agner Fog does not cover ARM, see agner-fog-reference.md) ---
    "apple-m1": dict(isa="arm", target=None, emit_flags=["-mcpu=apple-m1"],
                     mca_cpu="apple-m1", mca_triple="arm64-apple-macos",
                     uica=False, uica_arch=None, note="Apple Firestorm/Icestorm"),
    "armv8.2-a": dict(isa="arm", target="aarch64-linux-gnu", emit_flags=["-march=armv8.2-a"],
                      mca_cpu="cortex-a76", mca_triple="aarch64-unknown-linux-gnu",
                      uica=False, uica_arch=None, note="Generic ARMv8.2 baseline"),
}

DEFAULT_ARCHES = ["x86-64-v3", "znver4", "apple-m1"]

# Optimization-flag variants. Frame pointer omitted so the prologue does not pollute
# the analyzed region. No -Ofast / -ffast-math, ever: they change FP results.
FLAG_VARIANTS = {
    "O2": ["-O2", "-fomit-frame-pointer"],
    "O3": ["-O3", "-fomit-frame-pointer"],
}
DEFAULT_FLAGS = "O2"

CXX_STD = "c++17"


def fail(msg):
    print("asmlab: error: " + msg, file=sys.stderr)
    sys.exit(1)


def load_registry():
    if not REGISTRY_PATH.exists():
        fail("registry not found at %s" % REGISTRY_PATH)
    with REGISTRY_PATH.open() as fh:
        data = json.load(fh)
    return data["functions"]


def get_target(name):
    fns = load_registry()
    if name not in fns:
        fail("unknown function '%s'. Known: %s" % (name, ", ".join(sorted(fns))))
    return fns[name]


def resolve_arch(name):
    if name not in ARCHES:
        fail("unknown arch '%s'. Known: %s" % (name, ", ".join(sorted(ARCHES))))
    return ARCHES[name]


def arch_local_capable(arch):
    """True if this arch can be emitted on a macOS host without a Docker sysroot.

    The macOS SDK is multi-arch, so a *-apple-macos target (or the native host
    target) compiles to .s with no extra sysroot. Linux triples need the container.
    """
    tgt = arch["target"]
    return tgt is None or tgt.endswith("-apple-macos")


def parse_arch_list(arg):
    if not arg:
        return list(DEFAULT_ARCHES)
    return [a.strip() for a in arg.split(",") if a.strip()]


def llvm_bin(tool):
    """Locate an LLVM tool, preferring a Homebrew install over whatever is on PATH."""
    brew = Path("/opt/homebrew/opt/llvm/bin") / tool
    if brew.exists():
        return str(brew)
    found = shutil.which(tool)
    if found:
        return found
    return None


def cxx_compiler(compiler):
    """Map a logical compiler name to an executable. gcc routes through Docker."""
    if compiler == "clang":
        return shutil.which("clang++") or "/usr/bin/clang++"
    if compiler == "gcc":
        return shutil.which("g++-14") or shutil.which("g++") or "g++"
    fail("unknown compiler '%s'" % compiler)


def variant_dir(fn, arch, flags, compiler):
    return OUT_DIR / fn / ("%s-%s-%s" % (arch, compiler, flags))


def load_path_scenarios():
    return load_json_registry(PATH_SCENARIOS_PATH)


def get_scenario(fn, scenario_name):
    data = load_path_scenarios()
    by_fn = data.get("scenarios", {})
    if fn not in by_fn:
        fail("no scenarios for '%s'. Known: %s" % (fn, ", ".join(sorted(by_fn))))
    scenarios = by_fn[fn]
    if scenario_name not in scenarios:
        fail("unknown scenario '%s' for %s. Known: %s" % (
            scenario_name, fn, ", ".join(sorted(scenarios))))
    rec = dict(scenarios[scenario_name])
    rec["function"] = fn
    rec["scenario"] = scenario_name
    return rec


def list_scenarios(fn=None):
    data = load_path_scenarios()
    by_fn = data.get("scenarios", {})
    if fn:
        return sorted(by_fn.get(fn, {}).keys())
    return {k: sorted(v.keys()) for k, v in sorted(by_fn.items())}


def scenario_dir(fn, scenario, arch, flags, compiler, source_kind="original", variant=None):
    """Output directory for a path scenario slice."""
    base = OUT_DIR / fn / "scenarios" / scenario
    if variant:
        base = base / "variants" / variant
    else:
        base = base / source_kind
    return base / ("%s-%s-%s" % (arch, compiler, flags))


def run(cmd, **kwargs):
    """Run a command, returning CompletedProcess. Never raises on nonzero."""
    return subprocess.run(cmd, capture_output=True, text=True, **kwargs)


def symbol_for(fn):
    return "asmlab_" + fn


def load_json_registry(path):
    if not path.exists():
        fail("registry not found at %s" % path)
    with path.open() as fh:
        return json.load(fh)


def load_accuracy_manifest():
    return load_json_registry(ACCURACY_MANIFEST_PATH)


def load_candidate_edge_cases():
    if not CANDIDATE_EDGE_CASES_PATH.exists():
        return {"functions": {}, "candidate_accuracy_statuses": sorted(CANDIDATE_ACCURACY_STATUSES)}
    return load_json_registry(CANDIDATE_EDGE_CASES_PATH)


def production_gate_result(ulp_label, gate_mode="simple"):
    gate_path = OUT_DIR / "gates" / ("%s-%s.json" % (ulp_label, gate_mode or "simple"))
    if not gate_path.exists():
        return None
    doc = json.loads(gate_path.read_text())
    return doc.get("result")


def production_accuracy_field(fn, scenario_rec):
    """Production accuracy only. Never substitute for candidate validation."""
    labels = scenario_rec.get("accuracy_labels") or []
    ulp_label = labels[0] if labels else fn.replace("_impl", "")
    simple_result = production_gate_result(ulp_label, "simple")
    rigorous_result = production_gate_result(ulp_label, "rigorous")
    if simple_result == "pass":
        simple_status = "pass"
    elif simple_result == "fail":
        simple_status = "fail"
    else:
        simple_status = "not_run"
    if rigorous_result == "pass":
        rigorous_status = "pass"
    elif rigorous_result == "fail":
        rigorous_status = "fail"
    else:
        rigorous_status = "not_wired"
    return {
        "status": simple_status,
        "scope": "production",
        "simple": simple_status,
        "rigorous": rigorous_status,
        "ulp_label": ulp_label,
        "note": "Exercises unchanged include/ccmath/ paths only.",
    }


def candidate_accuracy_field(fn, variant=None, mode="simple"):
    """Load candidate accuracy from gate file or return not_run placeholder."""
    import variant_accuracy as va_mod
    return va_mod.candidate_accuracy_field(fn, variant=variant, mode=mode)


def load_benchmark_profiles():
    return load_json_registry(BENCHMARK_PROFILES_PATH)


def harness_mode(target):
    return target.get("harness_mode", "runtime_flatten")


def default_harness_mode(fn):
    if fn.endswith("_impl") or fn.endswith("_rt"):
        return "runtime_flatten"
    if fn.endswith("_gen"):
        return "constexpr_probe"
    return "runtime_no_flatten"
