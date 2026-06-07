# Golden analysis calibration

Golden analysis validates **asmlab analysis quality**, not numerical correctness of the polynomial kernels.

## Why Horner vs Estrin

This pair resembles real libm approximation kernels and exercises meaningful optimization tradeoffs:

| Tradeoff | Horner (baseline) | Estrin (candidate) |
| --- | --- | --- |
| Dependency depth | Deep serial mul-add chain | Shorter chains via x² reuse |
| ILP | Limited | More independent ops |
| Register pressure | Lower peak temporaries | More live temporaries |
| Vectorization | Harder to widen | More SIMD-friendly structure |
| FMA | Natural contraction sites | Multiple mul-add pairs |

## Placement (internal only)

All fixture code lives under `tools/asmlab/golden/`. It is **not** part of CCMath:

- Not under `include/ccmath/`
- Not in public benchmarks implying supported API
- Not installed or exported

Purpose:

```text
Validate the analysis platform, NOT expand CCMath.
```

## Layout

```text
tools/asmlab/golden/
  poly_horner/kernel.hpp + expected.json
  poly_estrin/kernel.hpp + expected.json
  pair_expected.json
  harness_template.cpp

tools/asmlab/bench/golden_poly_bench.cpp
tools/asmlab/scripts/run_golden_analysis.py
out/asmlab/golden/
  golden_report.json
  golden_report.md
  history/
```

## Run

```bash
# Full calibration (emit, deep analysis, benchmark, diff, validation)
python3 tools/asmlab/scripts/run_golden_analysis.py

# CI smoke
python3 tools/asmlab/scripts/run_golden_analysis.py --quick

# Wiring tests only
python3 tools/asmlab/scripts/test_golden_quick.py -v
```

## What is validated

| System | Validation |
| --- | --- |
| Source mapping | Golden harness maps to fixture headers |
| CFG recovery | cfg.json block structure present |
| llvm-mca | Directional throughput prediction (lower RThroughput = faster) |
| Semantic classification | Comparative findings from MCA + asm patterns |
| Vectorization | vector_insn_count and remark correlation |
| Register pressure | peak_gpr directional increase for Estrin |
| Spill detection | Directional only (spills are arch-dependent) |
| Diff analysis | analysis_diff.json Horner vs Estrin |
| Benchmark | BM_golden_poly_horner vs BM_golden_poly_estrin |
| Historical tracking | out/asmlab/golden/history/ snapshots |
| Causal ranking | Top findings prioritize dependency/ILP/throughput |

## Safe conclusions

- Directional: Estrin is predicted/measured faster on typical hosts
- Directional: Estrin shows higher register pressure heuristics
- Ranking: dependency-chain reduction ranks near the top

## Heuristic limitations

- llvm-mca cycle estimates are static models, not hardware truth
- Register pressure uses textual live-set heuristics, not SSA liveness
- Vectorization detection is correlational without auto-vectorization guarantees
- Benchmark results are advisory (merge_grade: advisory)
- Spills may not appear on all architectures

Golden analysis validates **directional correctness and ranking quality**, not exact assembly or cycle counts.
