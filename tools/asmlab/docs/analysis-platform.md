# Analysis platform

asmlab deep analysis layers compiler-aware, CFG-aware, and microarchitecture-aware
reasoning on top of the existing source-map and asm-diff workflow. Nothing in this
layer replaces emit, source_map, asm_diff, or the baseline metrics snapshot.

## Goals

Answer questions such as:

- What source construct likely caused this slowdown?
- Why did vectorization fail?
- Why did spills appear?
- Why did throughput regress despite similar instruction count?

Attributions are labeled by confidence and causal level. Correlation is not causation.

## Entry points

```bash
# Full deep pass on one function (implies --source-map)
python tools/asmlab/scripts/asmlab.py deep-analyze sqrt --arch x86-64-v3 --deep-analyze

# Baseline with deep artifacts frozen for diff
python tools/asmlab/scripts/asmlab.py baseline powf_impl --arch x86-64-v3 --source-map --deep-analyze

# Diff includes analysis_diff.json when --deep-analyze
python tools/asmlab/scripts/asmlab.py diff powf_impl --arch x86-64-v3 --source-map --deep-analyze

# HTML viewer shows CFG block tags, MCA bottleneck, semantic tags
python tools/asmlab/scripts/asmlab.py view sqrt --arch x86-64-v3 --source-map --deep-analyze --open
```

## Pipeline modules

| Module | Artifacts | Role |
| --- | --- | --- |
| cfg_analysis.py | cfg.json, cfg.md | Basic blocks, edges, dominators, loops |
| mca_deep.py | mca.json, mca.md | Per-function and per-block llvm-mca |
| opt_remarks.py | opt_remarks.json, opt_remarks.md | LLVM YAML optimization records |
| reg_pressure.py | reg_pressure.json, spill_report.md | Spill/reload heuristics |
| microarch_model.py | microarch.json, microarch.md | Port pressure and bottleneck class |
| vectorization.py | vectorization.json | SIMD width and scalar fallback |
| semantic_analysis.py | semantic.json | FMA, vectorization, spill semantics |
| perf_profile.py | perf.json, perf.md | Linux perf stat when available |
| history_track.py | out/asmlab/history/<fn>.json | Commit-aware snapshots |
| analysis_diff.py | analysis_diff.json | Baseline vs current deep compare |
| ast_schema.py | ast_schema.json | Forward-looking Clang AST hooks |

Orchestration lives in analysis_pipeline.py.

## Causal levels

- proven: reserved for measured perf agreement or formal proof
- likely_causal: strong compiler remark or spill tied to source line
- correlation: static model or pattern match only
- unknown: insufficient evidence

## See also

- artifact-schemas.md
- data-flow.md
- limitations.md
- examples/README.md
