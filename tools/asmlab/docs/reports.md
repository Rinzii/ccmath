# Reports and provenance

## Outputs per function

| File | Content |
| --- | --- |
| `out/asmlab/reports/<fn>.md` | Human-readable report |
| `out/asmlab/reports/<fn>.json` | Machine-readable full report |
| `out/asmlab/reports/<fn>.provenance.json` | Reproducibility metadata |
| `out/asmlab/<fn>/<variant>/emit_meta.json` | Symbol chain and harness mode |
| `out/asmlab/<fn>/<variant>/path_analysis.json` | Classification and metrics |
| `out/asmlab/<fn>/<variant>/metrics.json` | Static model estimates |
| `out/asmlab/<fn>/<variant>/source_map.json` | Instruction-to-source map |
| `out/asmlab/<fn>/<variant>/asm_diff.json` | Source-aware before/after diff |
| `out/asmlab/views/<fn>/<arch>/index.html` | Optional HTML viewer |

## Provenance fields

Each report records git commit, dirty status, compiler and version, flags, arch list,
host OS and CPU, tool versions (llvm-mca, uiCA, OSACA), and timestamp.

## Static model labeling

llvm-mca `block_rthroughput` is labeled **static model estimate**. It is not measured
cycles per call. uiCA and OSACA results, when present, are also static model estimates.
Disagreement between tools is reported, not treated as failure.

## Validation tiers

Reports tag what actually ran:

| Tier | Meaning |
| --- | --- |
| `codegen-supported` | Assembly emitted for variant |
| `static-analysis-supported` | llvm-mca (or optional tools) ran |
| `locally-benchmarked` | bench.sh ran on host |
| `docker-benchmarked` | bench ran in container |
| `hardware-validated` | Hardware counters (not yet supported) |

Default reports reach `codegen-supported` and `static-analysis-supported` only.

## Decision model

An optimization candidate is evaluated on seven points:

1. Path verified
2. Static model improved
3. Real benchmark improved (profile-specific)
4. Accuracy unchanged (manifest-driven gate on production code)
4b. Candidate accuracy unchanged when reviewing isolated variants (separate field)
5. Conformance unchanged
6. Compile-time behavior unchanged
7. Cross-target impact known or marked unknown

If only static analysis improves, mark the candidate **worth testing**, not accepted.

## Candidate vs production accuracy

Scenario and compare reports expose separate JSON fields:

- `production_accuracy` — exercises unchanged `include/ccmath/` paths
- `candidate_accuracy` — isolated variant under `out/asmlab/variants/`

Never treat a production gate pass as candidate validation. Run candidate accuracy:

```bash
python3 tools/asmlab/scripts/asmlab.py variant accuracy powf_impl <variant> --mode simple
```

Gate artifacts: `out/asmlab/gates/candidates/<fn>-<variant>-<mode>.json`

See `registry/candidate_edge_cases.json` and `registry/candidate_edge_case_inputs.json`
for `powf_impl` edge-case groups and wired corpora.

## Baselines

`baseline` snapshots include provenance, static model estimates per arch, and path
classification. `diff` compares only static model metrics and reports path category
per arch. Compare baselines only when provenance contexts match.
