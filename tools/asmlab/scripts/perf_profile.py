#!/usr/bin/env python3
# Copyright (c) Ian Pike
# Copyright (c) CCMath contributors
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
"""Runtime perf counter integration (Linux perf, stubs elsewhere)."""

import platform
import shutil
import subprocess
from pathlib import Path

import analysis_common as AC


def _find_perf():
    return shutil.which("perf")


def _run_perf_stat(binary, events=None):
    events = events or [
        "cycles", "instructions", "branches", "branch-misses",
        "cache-references", "cache-misses",
    ]
    perf = _find_perf()
    if not perf:
        return None, "perf not found"
    if not Path(binary).exists():
        return None, "benchmark binary not found: %s" % binary
    cmd = [perf, "stat", "-x,", "-e", ",".join(events), str(binary)]
    proc = subprocess.run(cmd, capture_output=True, text=True, check=False)
    if proc.returncode != 0:
        return None, proc.stderr or proc.stdout
    return proc.stdout, None


def _parse_perf_csv(stdout):
    metrics = {}
    for line in stdout.splitlines():
        if not line.strip() or line.startswith("#"):
            continue
        parts = line.split(",")
        if len(parts) >= 3:
            try:
                val = float(parts[0])
                name = parts[2].strip()
                metrics[name] = val
            except ValueError:
                continue
    if "cycles" in metrics and "instructions" in metrics and metrics["cycles"]:
        metrics["ipc"] = metrics["instructions"] / metrics["cycles"]
    return metrics


def build_perf(variant_dir, mca=None, bench_binary=None):
    variant_dir = Path(variant_dir)
    system = platform.system()
    perf_available = _find_perf() is not None and system == "Linux"

    measured = {}
    error = None
    if perf_available and bench_binary:
        out, err = _run_perf_stat(bench_binary)
        if out:
            measured = _parse_perf_csv(out)
        else:
            error = err

    predicted_ipc = None
    if mca and mca.get("function"):
        predicted_ipc = mca["function"].get("ipc")

    comparison = {}
    if measured.get("ipc") and predicted_ipc:
        comparison = {
            "measured_ipc": measured["ipc"],
            "predicted_ipc": predicted_ipc,
            "delta": measured["ipc"] - predicted_ipc,
            "confidence": AC.CAUSAL_CORRELATION,
            "note": "Static MCA IPC vs perf stat IPC on harness binary",
        }

    result = {
        "analysis_version": AC.ANALYSIS_VERSION,
        "platform": system,
        "perf_available": perf_available,
        "instruments_available": False,
        "vtune_available": False,
        "measured": measured,
        "predicted": {"ipc": predicted_ipc} if predicted_ipc else {},
        "comparison": comparison,
        "error": error,
        "confidence": "high" if measured else "none",
        "notes": [
            "Linux perf requires a runnable benchmark binary and appropriate permissions.",
            "Apple Instruments and VTune are not wired in this revision.",
            "Measured counters validate static predictions only at correlation level.",
        ],
    }

    md = [
        "# Runtime performance",
        "",
        "- platform: %s" % system,
        "- perf available: %s" % perf_available,
        "",
    ]
    if measured:
        for k, v in measured.items():
            md.append("- %s: %s" % (k, v))
    else:
        md.append("- no measured counters (see notes)")
    if comparison:
        md.append("")
        md.append("## Predicted vs measured")
        md.append("- measured IPC: %.3f" % comparison["measured_ipc"])
        md.append("- predicted IPC: %.3f" % comparison["predicted_ipc"])
    md.append("")

    AC.write_artifact(variant_dir, "perf", result, md)
    return result
