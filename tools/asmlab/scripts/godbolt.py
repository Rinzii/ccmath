#!/usr/bin/env python3
# Copyright (c) Ian Pike
# Copyright (c) CCMath contributors
#
# CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
# See LICENSE for more information.
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
"""Compiler Explorer (godbolt.org) integration for asmlab.

Zero-install path: compile a snippet on godbolt, get the assembly plus llvm-mca
output, and mint a shareable short link. Useful for quick exploration of an
instruction sequence and for sharing a finding, without installing LLVM locally.

Scope note: godbolt has no ccmath headers, so this works on self-contained
snippets and isolated kernels (for example a region.s body re-expressed as a tiny
function), not on full ccmath functions. For whole ccmath functions across the
arch matrix, use emit.py / analyze.py locally (they have the include tree).

Subcommands:
  compilers [--lang c++]                 list compiler ids (find one for --id)
  compile  <file|-> [--id ..] [--args ..] [--mca-cpu ..]   compile + asm + llvm-mca
  link     <file|-> [--id ..] [--args ..]                  shareable godbolt link
"""

import argparse
import json
import sys
import urllib.error
import urllib.request

API = "https://godbolt.org"
DEFAULT_ID = "clang_trunk"            # override with --id, see compilers subcommand
DEFAULT_ARGS = "-O2 -std=c++17 -fomit-frame-pointer"
MCA_TOOL = "llvm-mcatrunk"            # CE tool id for llvm-mca (trunk)


def _post(path, payload):
    req = urllib.request.Request(
        API + path, data=json.dumps(payload).encode(),
        headers={"Content-Type": "application/json", "Accept": "application/json"},
        method="POST")
    with urllib.request.urlopen(req, timeout=30) as r:
        return json.loads(r.read().decode())


def _get(path):
    req = urllib.request.Request(API + path, headers={"Accept": "application/json"})
    with urllib.request.urlopen(req, timeout=30) as r:
        return json.loads(r.read().decode())


def read_source(arg):
    if arg == "-":
        return sys.stdin.read()
    with open(arg) as fh:
        return fh.read()


def cmd_compilers(args):
    try:
        data = _get("/api/compilers/%s?fields=id,name,semver" % args.lang)
    except urllib.error.URLError as e:
        print("godbolt unreachable: %s" % e, file=sys.stderr)
        return 2
    for c in data:
        print("%-22s %s" % (c.get("id", "?"), c.get("name", "")))
    print("\nPass one of the ids above via --id.", file=sys.stderr)
    return 0


def _compile_payload(source, args, with_mca, mca_cpu):
    tools = []
    if with_mca:
        mca_args = "-mcpu=%s" % mca_cpu if mca_cpu else ""
        tools.append({"id": MCA_TOOL, "args": mca_args})
    return {
        "source": source,
        "options": {
            "userArguments": args,
            "filters": {"binary": False, "labels": True, "directives": True,
                        "commentOnly": True, "intel": False, "execute": False},
            "tools": tools,
            "libraries": [],
        },
    }


def cmd_compile(args):
    source = read_source(args.file)
    payload = _compile_payload(source, args.args, True, args.mca_cpu)
    try:
        data = _post("/api/compiler/%s/compile" % args.id, payload)
    except urllib.error.URLError as e:
        print("godbolt unreachable: %s (try `compilers` to verify --id)" % e, file=sys.stderr)
        return 2
    if data.get("code", 0) != 0:
        print("compile failed:", file=sys.stderr)
        for line in data.get("stderr", []):
            print("  " + line.get("text", ""), file=sys.stderr)
        return 1
    print("; --- assembly (%s %s) ---" % (args.id, args.args))
    for line in data.get("asm", []):
        print(line.get("text", ""))
    for tool in data.get("tools", []):
        if tool.get("id") == MCA_TOOL:
            print("\n; --- llvm-mca ---")
            for line in tool.get("stdout", []):
                print(line.get("text", ""))
    return 0


def cmd_link(args):
    source = read_source(args.file)
    session = {
        "sessions": [{
            "id": 1, "language": "c++", "source": source,
            "compilers": [{"id": args.id, "options": args.args}],
        }],
    }
    try:
        data = _post("/api/shortener", {"sessions": session["sessions"]})
    except urllib.error.URLError as e:
        print("godbolt unreachable: %s" % e, file=sys.stderr)
        return 2
    print(data.get("url", "(no url returned)"))
    return 0


def main(argv=None):
    ap = argparse.ArgumentParser(prog="godbolt.py", description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = ap.add_subparsers(dest="cmd", required=True)

    p_c = sub.add_parser("compilers")
    p_c.add_argument("--lang", default="c++")
    p_c.set_defaults(func=cmd_compilers)

    p_co = sub.add_parser("compile")
    p_co.add_argument("file", help="source file, or - for stdin")
    p_co.add_argument("--id", default=DEFAULT_ID)
    p_co.add_argument("--args", default=DEFAULT_ARGS)
    p_co.add_argument("--mca-cpu", default="", help="llvm-mca -mcpu (e.g. skylake, znver4)")
    p_co.set_defaults(func=cmd_compile)

    p_l = sub.add_parser("link")
    p_l.add_argument("file", help="source file, or - for stdin")
    p_l.add_argument("--id", default=DEFAULT_ID)
    p_l.add_argument("--args", default=DEFAULT_ARGS)
    p_l.set_defaults(func=cmd_link)

    args = ap.parse_args(argv)
    return args.func(args)


if __name__ == "__main__":
    sys.exit(main())
