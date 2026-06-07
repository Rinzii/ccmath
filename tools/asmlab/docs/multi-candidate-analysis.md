# Multi-candidate analysis

Compare multiple isolated candidate variants against the original implementation across path scenarios in one run.

## Candidate accuracy (run before merge review)

```bash
python3 tools/asmlab/scripts/asmlab.py variant accuracy powf_impl late_ex_sign --mode simple
```

`variant compare` runs simple candidate accuracy for each variant automatically.
Gate files live under `out/asmlab/gates/candidates/`. Production `powf-simple` pass
does not validate isolated candidates.

## Compare command

```bash
python3 tools/asmlab/scripts/asmlab.py variant compare powf_impl \
  --variants late_ex_sign cold_exception_split estrin_poly \
  --scenarios positive_finite_general near_one integer_exponent nan_inf_signed_zero \
  --arch x86-64-v3 \
  --source-map
```

Compare every workspace candidate:

```bash
python3 tools/asmlab/scripts/asmlab.py variant compare powf_impl --all \
  --scenarios positive_finite_general near_one \
  --arch x86-64-v3 --source-map
```

Flags:

- `--source-map` — enable source-map-aware spill/reload heuristics
- `--skip-bench` — reuse existing `scenario_bench.json` only
- `--force-emit` — re-emit scenario artifacts before comparing

## Output

```text
out/asmlab/<function>/candidate_compare/<arch>/
  compare.json
  compare.md
  scenarios/
    positive_finite_general.json
    near_one.json
    ...
```

Terminal output prints a ranking table and a conservative recommendation.

## Comparison dimensions

Per candidate and scenario the report collects:

- path safety (pass / warn / fail)
- spill, reload, and stack-traffic deltas from per-instruction source-map tags
- register-pressure proxy (instruction count, distinct regs, stack traffic)
- dependency-chain heuristic delta (architecture-specific FMA patterns)
- call-boundary score (local vs external calls)
- benchmark delta (when `scenario_bench.json` exists)
- `production_accuracy` and `candidate_accuracy` (separate fields, never merged)
- conservative decision label

## Ranking policy

Decisions are ranked conservatively:

1. Reject stale candidates
2. Reject path mismatches
3. Reject `candidate_accuracy: fail` or special-value failures (when evidenced)
4. Reject benchmark regressions
5. `candidate_accuracy: not_run` or `not_wired` → not merge-grade
6. Benchmark win without `candidate_accuracy: pass` → `not_enough_evidence`
7. Prefer measured benchmark wins over static wins when candidate accuracy passes
8. Treat spill or call reductions as secondary when path and benchmark are acceptable
9. Mark static-only improvements as `advisory_only`
10. Mark missing benchmarks as `not_enough_evidence` (not merge-grade)

`production_accuracy: pass` alone is `production_only` scope. It does not validate the
candidate. Static heuristics never override benchmark or candidate accuracy results.

## Interpreting the scenario matrix

| Column | Meaning |
| --- | --- |
| Path | Observed path classification for the candidate |
| Spills d | Spill-tag delta vs original (`adv` = low-confidence source map) |
| Calls d | Call-count delta from path analysis |
| Static d | llvm-mca throughput estimate delta (advisory) |
| Bench d | Candidate minus original CPU ns |
| Prod acc | `production_accuracy.status` on unchanged library code |
| Cand acc | `candidate_accuracy.status` for the isolated variant |
| Decision | Per-scenario conservative label |

## Apple M1 scope

M1 `powf_impl` artifacts are often thin call-boundary slices. The compare report:

- surfaces CPU knowledge thin-slice warnings
- suppresses spill and dependency ranking on M1 when the kernel is not exposed
- recommends x86-64-v3 scenario artifacts for spill and dependency claims

## Workflow

1. `variant init` — create candidate workspace under `out/asmlab/variants/`
2. Edit `candidate.hpp` only
3. `variant compare` — rank candidates across scenarios
4. For promising candidates: run `variant scenario validate` and rigorous accuracy gates manually
5. Do not merge from static metrics alone

See also `candidate-variants.md` and `cpu-knowledge.md`.
