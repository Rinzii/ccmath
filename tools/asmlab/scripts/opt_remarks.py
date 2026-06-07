#!/usr/bin/env python3
# Copyright (c) Ian Pike
# Copyright (c) CCMath contributors
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
"""Ingest LLVM optimization remarks from YAML records."""

import re
import subprocess
from pathlib import Path

import analysis_common as AC


def _parse_yaml_documents(text):
    """Minimal YAML document splitter for LLVM opt records (no PyYAML dep)."""
    docs = []
    current = {}
    in_args = False
    args_indent = 0

    def flush():
        if current:
            docs.append(dict(current))

    for line in text.splitlines():
        if line.strip() == "---":
            flush()
            current = {}
            in_args = False
            continue
        if not line.strip() or line.strip().startswith("#"):
            continue

        m = re.match(r"^---\s*!(Passed|Missed|Failure|Analysis)", line)
        if m:
            flush()
            current = {"kind": m.group(1)}
            in_args = False
            continue

        m = re.match(r"^(\s*)(\w+):\s*(.*)$", line)
        if m:
            indent, key, val = m.groups()
            val = val.strip().strip("'\"")
            if key == "Args":
                in_args = True
                args_indent = len(indent)
                current.setdefault("args", [])
                continue
            if in_args and len(indent) > args_indent:
                current.setdefault("args", []).append(line.strip())
                continue
            in_args = False
            if key == "DebugLoc":
                current["_debugloc_pending"] = True
                continue
            current[key.lower()] = val
            continue

        if "File:" in line and "_debugloc_pending" in current:
            fm = re.search(r"File:\s*'([^']+)'", line)
            if fm:
                current["file"] = fm.group(1)
            lm = re.search(r"Line:\s*(\d+)", line)
            if lm:
                current["line"] = int(lm.group(1))
            continue

        if "Message:" in line or "Reason:" in line:
            km = re.match(r"\s*(Message|Reason):\s*(.*)", line)
            if km:
                current[km.group(1).lower()] = km.group(2).strip()

    flush()
    return docs


def _classify_remark(doc):
    name = (doc.get("name") or "").lower()
    pass_name = (doc.get("pass") or "").lower()
    message = (doc.get("message") or doc.get("reason") or "").lower()
    tags = []
    if "vector" in name or "vector" in message or pass_name == "loop-vectorize":
        tags.append("vectorization")
    if "inline" in pass_name or "inline" in name:
        tags.append("inlining")
    if "unroll" in name or "unroll" in message:
        tags.append("unroll")
    if "licm" in pass_name:
        tags.append("licm")
    if "missed" in (doc.get("kind") or "").lower() or doc.get("kind") == "Missed":
        tags.append("missed_optimization")
    if "fma" in message or "contract" in message:
        tags.append("fma_contraction")
    return tags


def emit_opt_record(variant_dir, compiler, flags, harness_cpp, include_dirs):
    """Compile harness with optimization record flags."""
    variant_dir = Path(variant_dir)
    opt_path = variant_dir / "opt.yaml"
    obj = variant_dir / "harness_opt.o"
    cmd = [compiler] + list(flags) + [
        "-fsave-optimization-record",
        "-foptimization-record-file=%s" % opt_path,
        "-c", str(harness_cpp), "-o", str(obj),
    ]
    for inc in include_dirs:
        cmd.extend(["-I", inc])
    proc = subprocess.run(cmd, capture_output=True, text=True, check=False)
    return proc.returncode == 0, opt_path, proc.stderr


def build_opt_remarks(variant_dir, source_map=None):
    variant_dir = Path(variant_dir)
    opt_path = variant_dir / "opt.yaml"
    if not opt_path.exists():
        data = {
            "analysis_version": AC.ANALYSIS_VERSION,
            "available": False,
            "reason": "opt.yaml missing (re-run emit with deep analysis)",
            "remarks": [],
            "confidence": "none",
        }
        AC.write_artifact(variant_dir, "opt_remarks", data, ["# Optimization remarks", "", "opt.yaml not found."])
        return data

    raw = opt_path.read_text(errors="replace")
    docs = _parse_yaml_documents(raw)

    remarks = []
    for doc in docs:
        tags = _classify_remark(doc)
        f = doc.get("file", "")
        layer = AC.attribution_layer(f)
        remarks.append({
            "kind": doc.get("kind", "unknown"),
            "pass": doc.get("pass", ""),
            "name": doc.get("name", ""),
            "file": f,
            "line": doc.get("line", 0),
            "function": doc.get("function", ""),
            "message": doc.get("message") or doc.get("reason", ""),
            "tags": tags,
            "attribution_layer": layer,
        })

    by_tag = {}
    for r in remarks:
        for t in r["tags"]:
            by_tag.setdefault(t, []).append(r)

    result = {
        "analysis_version": AC.ANALYSIS_VERSION,
        "available": True,
        "remark_count": len(remarks),
        "remarks": remarks,
        "by_tag": {k: len(v) for k, v in by_tag.items()},
        "missed_vectorization": [r for r in remarks if "vectorization" in r["tags"] and "missed" in r["tags"]],
        "inline_decisions": [r for r in remarks if "inlining" in r["tags"]],
        "confidence": "high" if remarks else "low",
        "notes": [
            "Remarks are compiler-reported correlates, not proven runtime causation.",
            "Missed optimization does not always imply performance loss.",
        ],
    }

    md = [
        "# Optimization remarks",
        "",
        "- total remarks: %d" % len(remarks),
        "",
    ]
    for tag, count in sorted(result["by_tag"].items()):
        md.append("- %s: %d" % (tag, count))
    md.append("")
    md.append("## Missed vectorization (sample)")
    for r in result["missed_vectorization"][:5]:
        md.append("- %s:%d %s" % (r["file"], r["line"], r["name"]))
    md.append("")

    AC.write_artifact(variant_dir, "opt_remarks", result, md)
    return result
