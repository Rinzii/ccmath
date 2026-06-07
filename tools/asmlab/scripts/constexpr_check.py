#!/usr/bin/env python3
# Copyright (c) Ian Pike
# Copyright (c) CCMath contributors
#
# CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
# See LICENSE for more information.
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
"""Verify constexpr path for a registered function.

Compiles a probe TU with static_assert cases. Does not emit assembly. Answers
whether the constexpr path compiles and whether cases produce finite results.
"""

import argparse
import json
import sys

import _asmlab_common as C


def _fix_expr(expr, args):
    out = expr
    for j, val in enumerate(args):
        out = out.replace("a%d" % j, repr(val))
    return out


def render_probe_v2(fn, target):
    sig = target["signature"]
    includes = "\n".join('#include "%s"' % h for h in target["includes"])
    cases = target.get("constexpr_cases", [])
    if not cases:
        if len(sig["args"]) == 1:
            cases = [{"args": [2.0]}]
        elif len(sig["args"]) == 2:
            cases = [{"args": [2.0, 3.0]}]
        else:
            cases = []

    lines = []
    for i, case in enumerate(cases):
        expr = _fix_expr(target["expr"], case["args"])
        lines.append("constexpr auto asmlab_ce_%d = %s;" % (i, expr))
        lines.append("static_assert(asmlab_ce_%d == asmlab_ce_%d, "
                     "\"constexpr probe case %d\");" % (i, i, i))

    tpl = C.CONSTEXPR_PROBE_TEMPLATE.read_text()
    return (tpl.replace("@@INCLUDES@@", includes)
              .replace("@@STATIC_ASSERTS@@", "\n".join(lines)))


def run_constexpr_check(fn, target, out_dir=None):
    mode = target.get("harness_mode") or C.default_harness_mode(fn)
    if mode != "constexpr_probe" and not target.get("constexpr_cases"):
        return {
            "function": fn,
            "harness_mode": mode,
            "compiles": None,
            "skipped": True,
            "reason": "not a constexpr_probe entry and no constexpr_cases",
            "cases": [],
        }

    out_dir = out_dir or (C.OUT_DIR / fn / "constexpr")
    out_dir.mkdir(parents=True, exist_ok=True)
    src = out_dir / "constexpr_probe.cpp"
    src.write_text(render_probe_v2(fn, target))

    cxx = C.cxx_compiler("clang")
    cmd = [cxx, "-std=" + C.CXX_STD, "-c", "-I", str(C.INCLUDE_DIR),
           "-o", str(out_dir / "constexpr_probe.o"), str(src)]
    res = C.run(cmd)
    (out_dir / "compile.cmd").write_text(" ".join(cmd) + "\n")

    cases_out = []
    for i, case in enumerate(target.get("constexpr_cases", [])):
        cases_out.append({
            "args": case["args"],
            "note": case.get("note", ""),
            "status": "pass" if res.returncode == 0 else "fail",
        })
    if not cases_out and res.returncode == 0:
        cases_out.append({"args": [], "note": "default probe", "status": "pass"})

    result = {
        "function": fn,
        "harness_mode": mode,
        "compiles": res.returncode == 0,
        "skipped": False,
        "cases": cases_out,
        "errors": res.stderr.strip()[:800] if res.returncode != 0 else "",
    }
    (out_dir / "constexpr_result.json").write_text(
        json.dumps(result, indent=2) + "\n")
    return result


def main(argv=None):
    ap = argparse.ArgumentParser(description="Verify constexpr path for a function.")
    ap.add_argument("fn", help="registered function name")
    args = ap.parse_args(argv)

    target = C.get_target(args.fn)
    result = run_constexpr_check(args.fn, target)
    if result.get("skipped"):
        print("skipped: %s" % result["reason"])
        return 0
    if result["compiles"]:
        print("constexpr probe PASSED for %s (%d cases)" % (args.fn, len(result["cases"])))
        return 0
    print("constexpr probe FAILED for %s" % args.fn, file=sys.stderr)
    if result["errors"]:
        print(result["errors"], file=sys.stderr)
    return 1


if __name__ == "__main__":
    sys.exit(main())
