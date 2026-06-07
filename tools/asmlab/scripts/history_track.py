#!/usr/bin/env python3
# Copyright (c) Ian Pike
# Copyright (c) CCMath contributors
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
"""Historical snapshots for deep analyze runs (optional)."""

import json
import subprocess
from datetime import datetime, timezone
from pathlib import Path

import _asmlab_common as C


def _git_head():
    try:
        return subprocess.check_output(
            ["git", "rev-parse", "HEAD"], text=True, stderr=subprocess.DEVNULL
        ).strip()[:12]
    except (subprocess.CalledProcessError, FileNotFoundError):
        return "unknown"


def _snapshot_summary(analysis_bundle):
    return {
        "instruction_count": analysis_bundle.get("source_map", {}).get("instruction_count"),
        "block_count": analysis_bundle.get("cfg", {}).get("block_count"),
        "spill_count": analysis_bundle.get("reg_pressure", {}).get("spill_count"),
        "vectorized": analysis_bundle.get("vectorization", {}).get("vectorized"),
        "mca_ipc": (analysis_bundle.get("mca", {}) or {}).get("function", {}).get("ipc"),
        "bottleneck": (analysis_bundle.get("mca", {}) or {}).get("bottleneck", {}).get("primary"),
        "semantic_kinds": [c["kind"] for c in analysis_bundle.get("semantic", {}).get("classifications", [])],
    }


def append_history(fn, variant_dir, analysis_bundle, history_root=None):
    history_root = Path(history_root or C.OUT_DIR / "history")
    history_root.mkdir(parents=True, exist_ok=True)
    hist_path = history_root / ("%s.json" % fn)

    entries = []
    if hist_path.exists():
        entries = json.loads(hist_path.read_text())

    entry = {
        "timestamp": datetime.now(timezone.utc).isoformat(),
        "commit": _git_head(),
        "variant_dir": str(variant_dir),
        "arch": Path(variant_dir).name,
        "summary": _snapshot_summary(analysis_bundle),
    }
    entries.append(entry)

    hist_path.write_text(json.dumps(entries, indent=2) + "\n")
    return entries
