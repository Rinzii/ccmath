#!/usr/bin/env python3
# Copyright (c) Ian Pike
# Copyright (c) CCMath contributors
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
"""Shared helpers for deep analysis passes."""

import json
from pathlib import Path

import _asmlab_common as C

CAUSAL_PROVEN = "proven"
CAUSAL_LIKELY = "likely_causal"
CAUSAL_CORRELATION = "correlation"
CAUSAL_UNKNOWN = "unknown"

ANALYSIS_VERSION = 1


def load_json(path, default=None):
    if not path or not Path(path).exists():
        return default
    return json.loads(Path(path).read_text())


def write_artifact(variant_dir, name, data, md_lines=None):
    """Write name.json and optional name.md under variant_dir."""
    variant_dir = Path(variant_dir)
    jpath = variant_dir / ("%s.json" % name)
    jpath.write_text(json.dumps(data, indent=2) + "\n")
    if md_lines is not None:
        (variant_dir / ("%s.md" % name)).write_text("\n".join(md_lines) + "\n")
    return jpath


def attribution_layer(path):
    if not path:
        return "unknown"
    p = path.replace("\\", "/")
    if "out/asmlab" in p and "harness" in p:
        return "harness"
    if "include/ccmath/internal/" in p:
        return "ccmath_internal"
    if "include/ccmath/" in p:
        return "ccmath_public"
    return "unknown"


def insn_mnemonic(text):
    parts = text.strip().split(None, 1)
    return parts[0].lower().rstrip(",") if parts else ""


def is_branch_mnem(mnem):
    m = mnem.lower()
    if m.startswith("j") and m not in ("jmp",):
        return True
    return m in ("jmp", "je", "jne", "jz", "jnz", "jl", "jg", "ja", "jb",
                 "call", "callq", "ret", "retq", "b", "b.eq", "b.ne", "cbz", "cbnz")


def branch_targets(text):
    """Return list of label/symbol targets from a branch instruction."""
    parts = text.split()
    if len(parts) < 2:
        return []
    tgt = parts[-1].rstrip(",")
    if tgt.startswith(".") or tgt.startswith("#"):
        return []
    return [tgt]
