# asmlab: assembly analysis and performance validation

asmlab is an external, script-driven workflow for inspecting the assembly ccmath
generates, classifying which code path was measured, scoring regions with static cost
models, and gating changes behind accuracy and benchmark evidence. It lives entirely
under `tools/asmlab/` and never edits `include/ccmath/`.

**Current maturity: Level 4–7.** The tool emits assembly, isolates regions, classifies
paths, runs static models, manages baselines, runs function-specific accuracy gates, and
supports profile-aware benchmarks where wired. It does not claim hardware-validated or
merge-grade performance evidence without benchmark and accuracy confirmation.

## What it does

1. Emits assembly for registered functions across a microarchitecture matrix.
2. Isolates the meaningful region and follows thin local forwarders.
3. Classifies the measured path (kernel, wrapper, libm, hardware insn, unknown).
4. Separates compile-time probes from runtime codegen in reports.
5. Scores each variant with llvm-mca (always) and optional uiCA/OSACA as **static model
   estimates**, not measured performance.
6. Runs function-specific accuracy gates from `registry/accuracy_manifest.json`.
7. Runs profile-specific benchmarks where Google Benchmark targets exist.

## Quickstart

```bash
# Provision tooling (idempotent).
bash tools/asmlab/scripts/bootstrap.sh

# List registered functions and harness modes.
python3 tools/asmlab/scripts/asmlab.py list

# Path verification (constexpr + emit + classify).
python3 tools/asmlab/scripts/asmlab.py verify pow_impl --arch x86-64-v3

# Source-to-assembly map (headless JSON + markdown).
python3 tools/asmlab/scripts/asmlab.py report pow_impl --arch x86-64-v3 --source-map

# HTML viewer (optional, local review).
python3 tools/asmlab/scripts/asmlab.py view pow_impl --arch x86-64-v3

# Deep analysis: CFG, llvm-mca per block, opt remarks, register pressure, semantics.
python3 tools/asmlab/scripts/asmlab.py deep-analyze powf_impl --arch x86-64-v3 --deep-analyze
python3 tools/asmlab/scripts/asmlab.py baseline powf_impl --arch x86-64-v3 --deep-analyze
python3 tools/asmlab/scripts/asmlab.py diff powf_impl --arch x86-64-v3 --deep-analyze

# Full report (markdown + JSON + provenance).
python3 tools/asmlab/scripts/asmlab.py report pow_impl

# Baseline and diff (static model estimates).
python3 tools/asmlab/scripts/asmlab.py baseline pow_impl
python3 tools/asmlab/scripts/asmlab.py diff pow_impl

# Function-specific accuracy gate.
python3 tools/asmlab/scripts/asmlab.py gate powf --mode simple

# Profile benchmark (sqrt public API; powf_impl isolated impl kernel).
bash tools/asmlab/scripts/bench.sh sqrt --profile positive_finite_general
bash tools/asmlab/scripts/bench.sh powf_impl --profile positive_finite_general
bash tools/asmlab/scripts/bench.sh powf_impl --profile positive_finite_general --variant test_candidate

# Isolated candidate variant (edit out/asmlab/variants/ only, not include/ccmath/).
python3 tools/asmlab/scripts/asmlab.py variant init sqrt --name test_candidate
python3 tools/asmlab/scripts/asmlab.py variant scenario report sqrt test_candidate \
  positive_finite_general --arch x86-64-v3 --source-map
```

See `docs/candidate-variants.md` for the full candidate workflow.

```bash
# Golden analysis calibration (Horner vs Estrin, tool-owned fixtures only)
python3 tools/asmlab/scripts/run_golden_analysis.py --quick
```

See `docs/golden-analysis.md`.

## CPU knowledge base

Structured microarchitecture knowledge (sources, calibration, pattern rules, instruction facts) lives under `knowledge/`. Reports and scenario reports attach cpu_knowledge annotations via `cpu_knowledge.py`.

See `docs/cpu-knowledge.md`.

## The decision loop

A runtime optimization is worth testing only when:

1. **Path verified.** The report shows the intended ccmath path was measured, not libm.
2. **Static model improved.** `diff` shows lower static model estimate on target arches.
3. **Accuracy unchanged.** `gate` passes with manifest-listed coverage.
4. **Benchmark improved.** `bench.sh` shows lower ns/op for the relevant profile.

Static wins alone mean "worth testing," not "accepted." See `docs/reports.md`.

## Layout

| Path | Purpose |
| --- | --- |
| `registry/functions.json` | Analysis targets, harness modes, intended paths |
| `registry/accuracy_manifest.json` | Function-specific gate coverage |
| `registry/benchmark_profiles.json` | Input profiles and benchmark wiring |
| `registry/path_categories.json` | Path classification vocabulary |
| `registry/variant_targets.json` | Base impl files for candidate variant init |
| `out/asmlab/variants/` | Gitignored candidate workspaces (manifest, candidate.hpp) |
| `harness/` | Runtime and constexpr probe templates |
| `scripts/asmlab.py` | Orchestrator |
| `scripts/classify.py` | Path classification |
| `scripts/constexpr_check.py` | Compile-time path probe |
| `scripts/provenance.py` | Reproducibility metadata |
| `scripts/analysis_pipeline.py` | Deep analysis orchestrator |
| `scripts/cfg_analysis.py` | CFG and loop recovery |
| `scripts/mca_deep.py` | Per-block llvm-mca |
| `scripts/opt_remarks.py` | LLVM optimization records |
| `docs/` | External documentation |

Outputs go to `out/asmlab/` (gitignored): variants, reports, baselines, gates, benches.

## Adding a function

Copy an entry in `registry/functions.json`. Set `harness_mode`, `intended_path`, and
add a matching `accuracy_manifest.json` entry. Public entries use `runtime_no_flatten`
to observe dispatch. Kernel entries use `runtime_flatten` to inline the body.

See `docs/path-classification.md` and `docs/constexpr-runtime.md`.

## Further reading

- [Path classification](docs/path-classification.md)
- [Constexpr vs runtime](docs/constexpr-runtime.md)
- [Reports and provenance](docs/reports.md)
- [Benchmarking](docs/benchmarking.md)
- [Source traceability](docs/source-traceability.md)
- [Analysis platform](docs/analysis-platform.md)
- [Artifact schemas](docs/artifact-schemas.md)
- [Limitations](docs/limitations.md)
- [Path scenarios](docs/path-scenarios.md)
