#!/usr/bin/env python3
# Copyright (c) Ian Pike
# Copyright (c) CCMath contributors
#
# CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
# See LICENSE for more information.
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
"""Score emitted ccmath assembly with static cost models.

Runs llvm-mca (always, for x86 and ARM), and optionally uiCA (recent Intel only)
and OSACA (x86/ARM bounds) when those tools are installed. Normalizes every run
into a single metrics.json per variant:

  {instructions, total_uops, block_rthroughput, dispatch_width, ipc, code_size,
   port_pressure{resource: usage}, uica{...}, osaca{...}}

block_rthroughput is a static model estimate (llvm-mca reciprocal throughput).
It is not measured wall-clock performance. Static models err 9-36% versus hardware,
so this filters candidates. bench.sh confirms the real win.
"""

import argparse
import json
import re
import sys

import _asmlab_common as C


def run_llvm_mca(region_path, arch):
    mca = C.llvm_bin("llvm-mca")
    if not mca:
        return {"error": "llvm-mca not found; run scripts/bootstrap.sh"}
    cmd = [mca, "--json", "-mtriple=" + arch["mca_triple"], "-mcpu=" + arch["mca_cpu"],
           str(region_path)]
    res = C.run(cmd)
    if res.returncode != 0 or not res.stdout.strip():
        return {"error": (res.stderr or "llvm-mca produced no output").strip()[:400]}
    data = json.loads(res.stdout)
    region = data["CodeRegions"][0]
    sv = region["SummaryView"]
    iters = sv.get("Iterations", 1) or 1
    resources = data["TargetInfo"]["Resources"]
    pressure = {}
    for e in region.get("ResourcePressureView", {}).get("ResourcePressureInfo", []):
        usage = e.get("ResourceUsage", 0.0)
        if usage <= 1e-9:
            continue
        # Some scheduling models (notably Apple) suffix sub-unit indices with control
        # bytes, e.g. "CyUnitFloatDiv.\x00". Strip non-printables and merge sub-units
        # of the same logical port so pressure reads correctly and tables stay valid.
        name = "".join(c for c in resources[e["ResourceIndex"]] if c.isprintable()).rstrip(".")
        pressure[name] = round(pressure.get(name, 0.0) + usage / iters, 3)
    rthru = round(sv.get("BlockRThroughput", 0.0), 2)
    return {
        "static_model_estimate_cycles": rthru,
        "block_rthroughput": rthru,
        "instructions": sv.get("Instructions", 0) // iters,
        "total_uops": sv.get("TotaluOps", 0) // iters,
        "dispatch_width": sv.get("DispatchWidth"),
        "ipc": round(sv.get("IPC", 0.0), 3),
        "port_pressure": dict(sorted(pressure.items(), key=lambda kv: -kv[1])),
        "label": "static_model_estimate",
    }


def run_uica(region_path, arch):
    """uiCA models only recent Intel cores. Optional, skipped if not installed."""
    if not arch.get("uica"):
        return {"skipped": "uiCA does not model this microarchitecture"}
    uica = C.llvm_bin("uiCA.py") or __import__("shutil").which("uiCA.py")
    if not uica:
        return {"skipped": "uiCA not installed; see scripts/bootstrap.sh"}
    cmd = [sys.executable, uica, str(region_path), "-arch", arch["uica_arch"]]
    res = C.run(cmd)
    if res.returncode != 0:
        return {"error": res.stderr.strip()[:300]}
    m = re.search(r"([\d.]+)", res.stdout)
    cpi = float(m.group(1)) if m else None
    return {
        "static_model_estimate_cycles": cpi,
        "cycles_per_iter": cpi,
        "label": "static_model_estimate",
        "raw": res.stdout.strip()[:200],
    }


def run_osaca(region_path, arch):
    """OSACA gives throughput/critical-path bounds for x86 and ARM. Optional."""
    osaca = __import__("shutil").which("osaca")
    if not osaca:
        return {"skipped": "OSACA not installed; see scripts/bootstrap.sh"}
    cmd = [osaca, "--arch", arch["mca_cpu"], str(region_path)]
    res = C.run(cmd)
    if res.returncode != 0:
        return {"error": res.stderr.strip()[:300]}
    raw = res.stdout.strip()
    tp = None
    cp = None
    m_tp = re.search(r"Throughput:\s*([\d.]+)", raw)
    m_cp = re.search(r"Critical path:\s*([\d.]+)", raw)
    if m_tp:
        tp = float(m_tp.group(1))
    if m_cp:
        cp = float(m_cp.group(1))
    return {
        "static_model_estimate_cycles": tp,
        "throughput": tp,
        "critical_path": cp,
        "label": "static_model_estimate",
        "raw": raw[-400:],
    }


def code_size(variant_dir):
    region = variant_dir / "region.s"
    if not region.exists():
        return None
    insns = 0
    for ln in region.read_text().splitlines():
        s = ln.strip()
        if not s or s.startswith((".", "#", ";", "//")) or s.endswith(":"):
            continue
        insns += 1
    return insns


def _model_disagreement(mca, uica, threshold=0.10):
    """True when llvm-mca and uiCA estimates differ by more than threshold."""
    if "error" in mca or not uica.get("cycles_per_iter"):
        return None
    mca_val = mca.get("static_model_estimate_cycles") or mca.get("block_rthroughput")
    uica_val = uica.get("static_model_estimate_cycles") or uica.get("cycles_per_iter")
    if not mca_val or not uica_val:
        return None
    denom = max(abs(mca_val), abs(uica_val), 1e-9)
    rel = abs(mca_val - uica_val) / denom
    return {
        "relative_diff": round(rel, 3),
        "llvm_mca": mca_val,
        "uica": uica_val,
        "disagrees": rel > threshold,
    }


def analyze_variant(variant_dir, arch_name):
    arch = C.resolve_arch(arch_name)
    region = variant_dir / "region.s"
    if not region.exists():
        return None
    mca = run_llvm_mca(region, arch)
    uica = run_uica(region, arch)
    osaca = run_osaca(region, arch)
    path_analysis = None
    pa_path = variant_dir / "path_analysis.json"
    if pa_path.exists():
        path_analysis = json.loads(pa_path.read_text())
    metrics = {
        "variant": variant_dir.name,
        "arch": arch_name,
        "isa": arch["isa"],
        "asm_insn_count": code_size(variant_dir),
        "llvm_mca": mca,
        "uica": uica,
        "osaca": osaca,
        "model_disagreement": _model_disagreement(mca, uica),
        "path_analysis": path_analysis,
        "validation_tiers": ["codegen-supported", "static-analysis-supported"],
    }
    (variant_dir / "metrics.json").write_text(json.dumps(metrics, indent=2) + "\n")
    return metrics


def fmt_summary(m):
    mca = m["llvm_mca"]
    if "error" in mca:
        return "%-26s ERROR: %s" % (m["variant"], mca["error"][:60])
    top = next(iter(mca["port_pressure"].items()), ("-", 0))
    return "%-26s rthru=%-6s uops=%-4s insns=%-4s ipc=%-6s hot=%s(%.2f)" % (
        m["variant"], mca["block_rthroughput"], mca["total_uops"], mca["instructions"],
        mca["ipc"], top[0], top[1])


def main(argv=None):
    ap = argparse.ArgumentParser(description="Score emitted ccmath assembly with static cost models.")
    ap.add_argument("fn", help="registered function name")
    ap.add_argument("--arch", default="", help="comma list of arches (default: emitted ones)")
    args = ap.parse_args(argv)

    fn_dir = C.OUT_DIR / args.fn
    if not fn_dir.exists():
        C.fail("no emitted variants for '%s'. Run emit.py first." % args.fn)

    want = set(C.parse_arch_list(args.arch)) if args.arch else None
    print("asmlab analyze: %s" % args.fn)
    rows = []
    for variant_dir in sorted(p for p in fn_dir.iterdir() if p.is_dir()):
        arch_name = variant_dir.name.split("-clang-")[0].split("-gcc-")[0]
        if arch_name not in C.ARCHES:
            continue
        if want and arch_name not in want:
            continue
        m = analyze_variant(variant_dir, arch_name)
        if m:
            rows.append(m)
            print("  " + fmt_summary(m))
    print("wrote metrics.json for %d variants" % len(rows))
    return 0 if rows else 1


if __name__ == "__main__":
    sys.exit(main())
