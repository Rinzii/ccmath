#!/usr/bin/env python3
# Copyright (c) Ian Pike
# Copyright (c) CCMath contributors
#
# CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
# See LICENSE for more information.
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
"""Regenerate the measured tables in docs/ULP_STATUS.md.

Reads the function registry (tools/ulp_status_registry.json) and the per-function MPFR campaign
summaries written under tests/rigorous/oracle_logs/ by the mpfr_unary measurement target, then
rewrites the block between the BEGIN GENERATED and END GENERATED markers in the doc. Functions
with no completed summary render as measurement pending, so the doc never claims a number that was
not measured. Run after a sweep (tools/measure_ulp.sh). Running it twice is idempotent.
"""
import argparse
import json
import os
import sys

REPO_ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
DEFAULT_REGISTRY = os.path.join(REPO_ROOT, "tools", "ulp_status_registry.json")
DEFAULT_LOGS = os.path.join(REPO_ROOT, "tests", "rigorous", "oracle_logs")
DEFAULT_DOC = os.path.join(REPO_ROOT, "docs", "ULP_STATUS.md")

BEGIN_MARKER = "<!-- BEGIN GENERATED: tools/ulp_status.py -->"
END_MARKER = "<!-- END GENERATED -->"

CLASS_LABEL = {
    "exact": "Exact",
    "correctly_rounded": "Correctly rounded",
    "bounded": "Bounded",
    "not_rounding": "Not a rounding function",
    "not_implemented": "Not implemented",
}


def load_summary(logs_dir, glob_pattern, fn_id, fp_type):
    name = glob_pattern.format(id=fn_id, type=fp_type)
    path = os.path.join(logs_dir, name)
    if not os.path.isfile(path):
        return None
    with open(path, encoding="utf-8") as handle:
        return json.load(handle)


def format_ulp(value):
    # Trig functions near a pole or zero produce enormous ULP distances that are dominated by
    # argument reduction rather than ordinary rounding. Keep small numbers exact and render the
    # outliers compactly so the tables stay readable.
    if value >= 1000000:
        return "{:.1e}".format(value)
    return str(value)


def format_avg(value):
    if value == 0:
        return "0"
    return "{:.3g}".format(value)


# The three type variants. summary_type is the token in the summary filename; long double has no
# summary because the harness measures binary32 and binary64 only, so it always renders pending.
TYPES = [
    {"label": "binary32", "summary_type": "float", "suffix": "f", "cpp": "float"},
    {"label": "binary64", "summary_type": "double", "suffix": "", "cpp": "double"},
    {"label": "long double", "summary_type": None, "suffix": "l", "cpp": "long double"},
]


def kernel_for_type(symbol, cpp):
    # The internal-impl kernels are named with a _float or _double suffix and have no dedicated long
    # double kernel, so long double uses the double one. The gen kernels are T templates, so the same
    # symbol instantiates at each type.
    if not symbol:
        return ""
    if symbol.endswith("_float"):
        base = symbol[: -len("_float")]
        return symbol if cpp == "float" else base + "_double"
    return "{}<{}>".format(symbol, cpp)


def measured_cells(fn, summary_type, logs_dir, glob_pattern):
    fn_id = fn.get("id")
    if summary_type is None or not fn_id:
        return ("pending", "pending")
    summary = load_summary(logs_dir, glob_pattern, fn_id, summary_type)
    if summary is None:
        return ("pending", "pending")
    return (format_ulp(summary.get("max_observed_ulp", 0)), format_avg(summary.get("average_ulp", 0.0)))


def build_row(fn, logs_dir, glob_pattern):
    name = fn["name"]
    klass = fn["class"]
    label = CLASS_LABEL.get(klass, klass)
    note = fn.get("note", "")

    if klass == "exact":
        return [name, label, "0", "0", note]
    if klass == "not_rounding":
        return [name, label, "n/a", "n/a", note]
    if klass == "not_implemented":
        return [name, label, "not impl", "not impl", note]

    fn_id = fn.get("id")
    summary = load_summary(logs_dir, glob_pattern, fn_id, "float") if fn_id else None
    if summary is None:
        # fma is correctly rounded by its own campaign even without a unary summary.
        if klass == "correctly_rounded" and fn_id is None:
            return [name, label, "0", "0", note]
        return [name, label, "pending", "pending", note]

    return [name, label, format_ulp(summary.get("max_observed_ulp", 0)), format_avg(summary.get("average_ulp", 0.0)),
            note]


def render_by_type_family(family, logs_dir, glob_pattern):
    # Accuracy-bearing families break each function into its float, double, and long double variant
    # rows (expf, exp, expl), each with its own measured numbers and the exact kernel that runs.
    lines = [f"### {family['name']}", ""]
    lines.append("| Variant | Class | Max ULP | Avg ULP | Tested kernel | Notes |")
    lines.append("|---------|-------|---------|---------|---------------|-------|")
    for fn in family["functions"]:
        name = fn["name"]
        label = CLASS_LABEL.get(fn["class"], fn["class"])
        symbol = fn.get("symbol", "")
        note = fn.get("note", "")
        no_suffix = fn.get("no_suffix", False)
        for index, variant in enumerate(TYPES):
            display = name if no_suffix else name + variant["suffix"]
            cell_name = "{} ({})".format(display, variant["label"])
            max_ulp, avg_ulp = measured_cells(fn, variant["summary_type"], logs_dir, glob_pattern)
            kernel = kernel_for_type(symbol, variant["cpp"])
            row = [cell_name, label, max_ulp, avg_ulp, kernel, note if index == 0 else ""]
            lines.append("| " + " | ".join(cell if cell else "" for cell in row) + " |")
    lines.append("")
    return lines


def render_family(family, logs_dir, glob_pattern):
    if family.get("by_type"):
        return render_by_type_family(family, logs_dir, glob_pattern)
    lines = [f"### {family['name']}", ""]
    lines.append("| Function | Class | Max ULP | Avg ULP | Notes |")
    lines.append("|----------|-------|---------|---------|-------|")
    for fn in family["functions"]:
        row = build_row(fn, logs_dir, glob_pattern)
        lines.append("| " + " | ".join(cell if cell else "" for cell in row) + " |")
    lines.append("")
    return lines


def first_available_summary(registry, logs_dir, glob_pattern):
    for family in registry["families"]:
        for fn in family["functions"]:
            fn_id = fn.get("id")
            if not fn_id:
                continue
            summary = load_summary(logs_dir, glob_pattern, fn_id, "float")
            if summary is not None:
                return summary
    return None


def render_provenance(registry, logs_dir, glob_pattern):
    summary = first_available_summary(registry, logs_dir, glob_pattern)
    if summary is None:
        return ["> No campaign summaries found yet. Run `tools/measure_ulp.sh` then `tools/ulp_status.py`.", ""]
    return [
        "> ccmath generic kernels (--impl=gen), round-to-nearest. MPFR correctly-rounded reference. The "
        "accuracy families break each function into its float, double, and long double variant (expf, exp, "
        "expl). binary32 is measured by an exhaustive sweep over all bit patterns, so its max is a proven "
        "worst case. binary64 and long double are pending. The Tested kernel column names the exact symbol "
        "that runs. Measured on {} with {}. Last refreshed {}.".format(
            summary.get("platform", "?"), summary.get("compiler", "?"), summary.get("timestamp", "?")),
        "",
    ]


def generate_block(registry, logs_dir, glob_pattern):
    lines = []
    lines.extend(render_provenance(registry, logs_dir, glob_pattern))
    for family in registry["families"]:
        lines.extend(render_family(family, logs_dir, glob_pattern))
    return "\n".join(lines).rstrip() + "\n"


def main():
    parser = argparse.ArgumentParser(description="Regenerate the measured tables in docs/ULP_STATUS.md.")
    parser.add_argument("--registry", default=DEFAULT_REGISTRY)
    parser.add_argument("--logs", default=DEFAULT_LOGS)
    parser.add_argument("--doc", default=DEFAULT_DOC)
    parser.add_argument("--check", action="store_true", help="Fail if the doc would change instead of writing it.")
    args = parser.parse_args()

    with open(args.registry, encoding="utf-8") as handle:
        registry = json.load(handle)
    glob_pattern = registry.get("summary_glob", "mpfr-unary-{id}-{type}-summary.json")

    block = generate_block(registry, args.logs, glob_pattern)

    with open(args.doc, encoding="utf-8") as handle:
        doc = handle.read()

    if BEGIN_MARKER not in doc or END_MARKER not in doc:
        sys.stderr.write("error: doc is missing the generated markers\n")
        return 2

    head, rest = doc.split(BEGIN_MARKER, 1)
    _, tail = rest.split(END_MARKER, 1)
    new_doc = head + BEGIN_MARKER + "\n\n" + block + "\n" + END_MARKER + tail

    if args.check:
        if new_doc != doc:
            sys.stderr.write("error: docs/ULP_STATUS.md is stale, run tools/ulp_status.py\n")
            return 1
        return 0

    if new_doc != doc:
        with open(args.doc, "w", encoding="utf-8") as handle:
            handle.write(new_doc)
        sys.stderr.write("updated {}\n".format(args.doc))
    return 0


if __name__ == "__main__":
    sys.exit(main())
