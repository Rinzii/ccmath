#!/usr/bin/env python3
# Copyright (c) Ian Pike
# Copyright (c) CCMath contributors
#
# CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
# See LICENSE for more information.
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
"""Record git, compiler, and tool versions for an asmlab run."""

import datetime
from datetime import timezone
import json
import os
import platform
import re
import shutil
import sys

import _asmlab_common as C


def _git_commit():
    res = C.run(["git", "-C", str(C.PROJECT_ROOT), "rev-parse", "HEAD"])
    short = C.run(["git", "-C", str(C.PROJECT_ROOT), "rev-parse", "--short", "HEAD"])
    return {
        "full": res.stdout.strip() if res.returncode == 0 else "unknown",
        "short": short.stdout.strip() if short.returncode == 0 else "unknown",
    }


def _git_dirty():
    res = C.run(["git", "-C", str(C.PROJECT_ROOT), "diff", "--quiet"])
    return res.returncode != 0


def _compiler_version(compiler):
    cxx = C.cxx_compiler(compiler)
    res = C.run([cxx, "--version"])
    if res.returncode != 0:
        return {"executable": cxx, "version": "unknown"}
    first = res.stdout.strip().splitlines()[0] if res.stdout else "unknown"
    return {"executable": cxx, "version": first}


def _host_cpu():
    sysname = platform.system()
    if sysname == "Darwin":
        res = C.run(["sysctl", "-n", "machdep.cpu.brand_string"])
        if res.returncode == 0:
            return res.stdout.strip()
    if sysname == "Linux":
        try:
            with open("/proc/cpuinfo") as fh:
                for line in fh:
                    if line.startswith("model name"):
                        return line.split(":", 1)[1].strip()
        except OSError:
            pass
    return platform.processor() or "unknown"


def _tool_version(name, finder):
    path = finder()
    if not path:
        return None
    if name == "llvm-mca":
        res = C.run([path, "--version"])
        ver = res.stdout.strip().splitlines()[0] if res.stdout else "unknown"
        return {"path": path, "version": ver}
    if name == "osaca":
        res = C.run([path, "--version"])
        ver = res.stdout.strip().splitlines()[0] if res.stdout else "unknown"
        return {"path": path, "version": ver}
    return {"path": path, "version": "installed"}


def collect(fn, flags, compiler, arches, extra=None):
    """Build a provenance dict for a report or gate run."""
    git = _git_commit()
    prov = {
        "timestamp": datetime.datetime.now(timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ"),
        "function": fn,
        "registry_entry": fn,
        "git_commit": git["short"],
        "git_commit_full": git["full"],
        "git_dirty": _git_dirty(),
        "compiler": _compiler_version(compiler),
        "cxx_standard": C.CXX_STD,
        "optimization_flags": C.FLAG_VARIANTS.get(flags, []),
        "flags_variant": flags,
        "arches": list(arches),
        "host_os": platform.system(),
        "host_cpu": _host_cpu(),
        "host_machine": platform.machine(),
        "tools": {
            "llvm_mca": _tool_version("llvm-mca", lambda: C.llvm_bin("llvm-mca")),
            "uica": _tool_version("uica", lambda: C.llvm_bin("uiCA.py")
                                  or shutil.which("uiCA.py")),
            "osaca": _tool_version("osaca", lambda: shutil.which("osaca")),
        },
        "docker_used": os.environ.get("CCMATH_ASMLAB_DOCKER", "0") == "1",
        "godbolt_link_generated": False,
    }
    if extra:
        prov.update(extra)
    return prov


def write_provenance(path, prov):
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(prov, indent=2) + "\n")
    return path
