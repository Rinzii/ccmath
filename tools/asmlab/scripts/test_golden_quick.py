#!/usr/bin/env python3
# Copyright (c) Ian Pike
# Copyright (c) CCMath contributors
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
"""CI smoke: golden fixture wiring and quick analysis validation."""

import json
import sys
import unittest
from pathlib import Path

import _asmlab_common as C

ASMLAB = C.ASMLAB_DIR
SCRIPTS = ASMLAB / "scripts"


class GoldenWiringTest(unittest.TestCase):
    def test_fixture_files_exist(self):
        for name in ("poly_horner", "poly_estrin"):
            d = ASMLAB / "golden" / name
            self.assertTrue((d / "kernel.hpp").exists())
            self.assertTrue((d / "expected.json").exists())

    def test_golden_registry(self):
        reg = json.loads((ASMLAB / "registry" / "golden_targets.json").read_text())
        self.assertIn("golden_poly_horner", reg["targets"])
        self.assertIn("golden_poly_estrin", reg["targets"])

    def test_pair_expected(self):
        pair = json.loads((ASMLAB / "golden" / "pair_expected.json").read_text())
        self.assertIn("dependency_chain_reduced", pair["expected_top_findings"])

    def test_no_ccmath_in_golden_kernels(self):
        for path in (ASMLAB / "golden").rglob("*.hpp"):
            text = path.read_text()
            self.assertNotIn("ccmath/", text)
            self.assertNotIn("ccm::", text)


class GoldenQuickRunTest(unittest.TestCase):
    @unittest.skipUnless(
        Path("/usr/bin/clang").exists() or Path("/opt/homebrew/bin/clang").exists(),
        "clang required for golden quick run")
    def test_quick_runner(self):
        import subprocess
        r = subprocess.run(
            [sys.executable, str(SCRIPTS / "run_golden_analysis.py"), "--quick"],
            cwd=str(C.PROJECT_ROOT),
            capture_output=True,
            text=True,
            timeout=300,
        )
        self.assertEqual(r.returncode, 0, msg=r.stdout + r.stderr)
        report_path = C.OUT_DIR / "golden" / "golden_report.json"
        self.assertTrue(report_path.exists())
        rep = json.loads(report_path.read_text())
        self.assertTrue(rep.get("passed"), rep.get("validation"))


if __name__ == "__main__":
    sys.path.insert(0, str(SCRIPTS))
    unittest.main()
