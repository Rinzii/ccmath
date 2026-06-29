#!/usr/bin/env python3
# Copyright (c) Ian Pike
# Copyright (c) CCMath contributors
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
"""Regression tests: powf_impl benchmark must stay wired."""

import json
import re
import sys
import unittest
from pathlib import Path

import _asmlab_common as C

ASMLAB_DIR = C.ASMLAB_DIR
REGISTRY = ASMLAB_DIR / "registry" / "benchmark_profiles.json"
BENCH_CPP = ASMLAB_DIR / "bench" / "powf_impl_bench.cpp"
BENCH_CMAKE = ASMLAB_DIR / "bench" / "CMakeLists.txt"


class PowfImplBenchWiringTest(unittest.TestCase):
    def test_powf_impl_wired_in_registry(self):
        data = json.loads(REGISTRY.read_text())
        tgt = data["benchmark_targets"]["powf_impl"]
        self.assertTrue(tgt.get("wired"), "powf_impl must be wired in benchmark_profiles.json")
        self.assertEqual(tgt.get("target"), "asmlab_bench_powf_impl")
        self.assertEqual(tgt.get("bench_kind"), "asmlab_impl")
        profiles = data["function_profiles"]["powf_impl"]
        self.assertIn("positive_finite_general", profiles)

    def test_bench_sources_exist(self):
        self.assertTrue(BENCH_CPP.exists(), "powf_impl_bench.cpp missing")
        self.assertTrue(BENCH_CMAKE.exists(), "tools/asmlab/bench/CMakeLists.txt missing")
        text = BENCH_CPP.read_text()
        self.assertIn("BM_asmlab_powf_impl_positive_finite_general", text)
        self.assertIn("ASMLAB_POWF_IMPL_CANDIDATE", text)

    def test_harness_templates_use_in_extension(self):
        templates = (
            ASMLAB_DIR / "harness" / "harness.cpp.in",
            ASMLAB_DIR / "harness" / "constexpr_probe.cpp.in",
            ASMLAB_DIR / "golden" / "harness.cpp.in",
        )
        for path in templates:
            self.assertTrue(path.is_file(), "%s missing" % path)
            text = path.read_text()
            broken = re.search(r"@ @\w+ @ @", text)
            self.assertIsNone(broken, "broken @@ token spacing in %s" % path)

        self.assertEqual(
            C.generated_path_from_template(templates[0], "/tmp/out").name,
            "harness.cpp",
        )
        self.assertEqual(
            C.generated_path_from_template(templates[1], "/tmp/out").name,
            "constexpr_probe.cpp",
        )

    def test_scenario_bench_status_labels(self):
        import bench_impl as bench_impl_mod
        self.assertEqual(bench_impl_mod.bench_status_from_result({"status": "ran"}), "ran")
        self.assertEqual(
            bench_impl_mod.bench_status_from_result({"status": "failed"}), "failed")
        self.assertEqual(
            bench_impl_mod.bench_status_from_result({"status": "unsupported"}), "unsupported")


if __name__ == "__main__":
    sys.path.insert(0, str(ASMLAB_DIR / "scripts"))
    unittest.main()
