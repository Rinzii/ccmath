# Proof smoke tooling

tests/proofs holds proof freshness runners that read production headers and optional Gappa or Sollya obligations. These are smoke checks, not a substitute for docs/proofs notes or full oracle campaigns.

Reproducible proof generation and approximation scripts live under tools/proofs/. Human-readable proof notes live under docs/proofs/.

## CMake wiring

tests/rigorous/CMakeLists.txt registers ccmath-rigorous-pow-proof-smoke when Python3, gappa, and sollya are on PATH. It runs:

```text
tests/proofs/math/power/proof_freshness_check.py --smoke-only
```

That script delegates to pow_double_proof.py and pow_float_proof.py.

## Layout (pow today)

| Path | Role |
| --- | --- |
| math/power/proof_freshness_check.py | Combined entry for CMake smoke |
| math/power/pow_double_proof.py, pow_float_proof.py | Parse impl headers, run smoke or full Gappa |
| math/power/pow_*_eval.g | Gappa obligation files |
| math/power/proof_reporting.py | Shared JSON report helpers |
| math/power/pow_accuracy_probe.cpp | Standalone ULP probe (not wired to CTest yet) |

Smoke mode checks tool presence and that proof inputs still match include/ccmath constants. Full mode runs Gappa and Sollya (--full).

## Run locally

```bash
python3 tests/proofs/math/power/proof_freshness_check.py --smoke-only
python3 tests/proofs/math/power/proof_freshness_check.py --full --report /tmp/pow-proof.json
```

Or through CMake after configure:

```bash
cmake --build out/<preset> --target ccmath-rigorous-pow-proof-smoke
```

## Add proof smoke for another function

1. Add tests/proofs/math/<family>/ with a proof script that reads the real implementation headers.
2. Add a combined freshness script or extend an existing one.
3. Register a custom target in tests/rigorous/CMakeLists.txt with the same Python3, gappa, and sollya gate if full runs are required.

Keep coefficient and evaluation-order changes in sync with tools/proofs runners and docs/proofs notes in the same change.

Rigorous suite overview: ../rigorous/README.md
