<img src="../../docs/resources/ccmath_logo_white.png" style="width: 35%;" alt="CCMath Logo">

# CCMath asmlab

Assembly analysis and performance validation for [CCMath](../../README.md).
The workflow inspects emitted assembly, confirms which path was measured, runs
static cost models, and only then looks at benches and accuracy.

All tooling lives under tools/asmlab/. It never edits include/ccmath/. Run
outputs land in out/asmlab/, which is gitignored.

## Bootstrap

```bash
bash tools/asmlab/scripts/bootstrap.sh
python3 tools/asmlab/scripts/asmlab.py list
```

## Everyday commands

Examples below use pow_impl or powf. Swap in any registered function name.

### Verify

Confirm the measured symbol is the CCMath kernel, not libm or a thin jmp.

```bash
python3 tools/asmlab/scripts/asmlab.py verify pow_impl --arch x86-64-v3
```

### Report and view

Emit, analyze, and optionally open the HTML report. Add --source-map for line attribution.

```bash
python3 tools/asmlab/scripts/asmlab.py report pow_impl --arch x86-64-v3 --source-map
python3 tools/asmlab/scripts/asmlab.py view pow_impl --arch x86-64-v3 --open
```

### Baseline and diff

Snapshot the current emit as a baseline, re-emit later, and diff metrics.

```bash
python3 tools/asmlab/scripts/asmlab.py baseline powf_impl --arch x86-64-v3 --source-map
python3 tools/asmlab/scripts/asmlab.py diff powf_impl --arch x86-64-v3 --source-map
```

### Deep analyze

Runs CFG recovery, per-block MCA, optimizer remarks, register pressure, and semantic
checks in one pass.

```bash
python3 tools/asmlab/scripts/asmlab.py deep-analyze powf_impl --arch x86-64-v3 --deep-analyze
```

### Accuracy gate

Run the production accuracy manifest for a function.

```bash
python3 tools/asmlab/scripts/asmlab.py gate powf --mode simple
```

### Bench profiles

Profile benchmarks where registry wiring exists.

```bash
bash tools/asmlab/scripts/bench.sh sqrt --profile positive_finite_general
bash tools/asmlab/scripts/bench.sh powf_impl --profile positive_finite_general
```

## When is a perf change worth merging?

A better llvm-mca score is a hint to keep digging, not a merge signal by itself.
Before you land a perf patch, check that verify actually hit the CCMath kernel (not
libm or a thin jmp), the static model looks better on the arches you ship, gate
still passes, and the bench profile you care about is faster.

llvm-mca and the other static tools are rough estimates. Mixed results between them
are normal.

## Constexpr vs runtime

Treat constexpr and runtime as two different checks. constexpr_check.py compiles
static_assert cases from the registry. Runtime emit uses harness templates with
volatile guards so arguments are not folded away.

A constexpr probe does not tell you what the runtime slice looks like, and the
reverse is true as well.

registry/functions.json defines these harness modes:

| Mode | Typical target |
| --- | --- |
| runtime_no_flatten | Public API, see dispatch |
| runtime_flatten | Impl kernels |
| constexpr_probe | gen paths |
| path_direct | Same as runtime_flatten for direct entries |

classify.py assigns path categories such as ccmath_internal_kernel, external_libm_call,
and hardware_instruction_lowering. The full vocabulary is in registry/path_categories.json.
Seeing pow or _pow in the slice usually means you measured libm, not pow_impl.

## Scenarios

Scenarios fix harness inputs and an intended path label per branch. They are a
static slice, not a dynamic trace.

```bash
python3 tools/asmlab/scripts/asmlab.py scenario list powf_impl
python3 tools/asmlab/scripts/asmlab.py scenario report powf_impl positive_finite_general \
  --arch x86-64-v3 --source-map
python3 tools/asmlab/scripts/asmlab.py scenario verify powf_impl positive_finite_general \
  --arch x86-64-v3
python3 tools/asmlab/scripts/asmlab.py scenario baseline powf_impl near_one \
  --arch x86-64-v3 --source-map
python3 tools/asmlab/scripts/asmlab.py scenario diff powf_impl near_one --arch x86-64-v3 --source-map
```

If a scenario report shows PATH MISMATCH, stop and fix the harness or inputs
before comparing metrics. Static numbers from different paths are not comparable.

Scenario outputs are under out/asmlab/<fn>/scenarios/<scenario>/.

## Candidate variants

Work on candidates under out/asmlab/variants/ only. Do not edit production headers
until review is finished.

```bash
python3 tools/asmlab/scripts/asmlab.py variant init powf_impl --name my_candidate
# edit out/asmlab/variants/powf_impl/my_candidate/candidate.hpp
python3 tools/asmlab/scripts/asmlab.py variant scenario report powf_impl my_candidate \
  positive_finite_general --arch x86-64-v3 --source-map
python3 tools/asmlab/scripts/asmlab.py variant scenario diff powf_impl my_candidate \
  positive_finite_general --arch x86-64-v3 --source-map
python3 tools/asmlab/scripts/bench_impl.py powf_impl --profile positive_finite_general \
  --variant my_candidate
python3 tools/asmlab/scripts/asmlab.py variant accuracy powf_impl my_candidate --mode simple
python3 tools/asmlab/scripts/asmlab.py variant compare powf_impl --all \
  --scenarios positive_finite_general,near_one --arch x86-64-v3
```

If the base kernel changed after variant init, scenario diff stops on the stale
tree. The variant patch command writes a review diff you apply by hand.

candidate_accuracy is separate from the production gate. A faster bench without a
passing candidate_accuracy run is not_enough_evidence on its own.

## Benchmarks

Profiles are in registry/benchmark_profiles.json. powf_impl benches the isolated
impl kernel (tools/asmlab/bench/powf_impl_bench.cpp), not the public ccm::powf
wrapper. Public pow bench wiring is still incomplete. Do not use sqrt bench
results as pow evidence.

bench.sh prints wiring hints when a profile is not configured. Add targets under
tools/asmlab/bench/ and registry entries to wire a new case.

## Source maps

Most emit commands accept --source-map. That writes source_map.json and, on diff,
asm_diff.json. Line mapping at O2/O3 is approximate. A line number of 0 means
attribution was uncertain.

## Golden calibration

Golden fixtures under tools/asmlab/golden/ (Horner vs Estrin) exercise the analysis
pipeline itself. They do not check CCMath numerical correctness.

```bash
python3 tools/asmlab/scripts/run_golden_analysis.py --quick
python3 tools/asmlab/scripts/test_golden_quick.py -v
```

## CPU knowledge

Selected microarchitecture facts with source tags and confidence live under
knowledge/. scripts/cpu_knowledge.py loads them and adds advisory notes to
reports. Those notes are hints, not proof.

```bash
python3 tools/asmlab/scripts/asmlab.py knowledge validate
```

See knowledge/README.md for file layout.

## Registry and scripts

| Path | Role |
| --- | --- |
| registry/functions.json | Targets, harness modes, constexpr cases |
| registry/path_scenarios.json | Scenario inputs and intended paths |
| registry/accuracy_manifest.json | Production gate coverage |
| registry/benchmark_profiles.json | Bench profiles and wiring |
| registry/variant_targets.json | Base files for variant init |
| registry/path_categories.json | Path classification vocabulary |
| scripts/asmlab.py | CLI entry point |
| harness/ | Harness and constexpr probe templates |

To add a function, copy a registry/functions.json entry, set harness_mode and
intended_path, and add accuracy_manifest coverage if you plan to gate it.

## Main artifacts

Each emit variant writes emit_meta.json, path_analysis.json, metrics.json, and
region.s. Passing --source-map also produces source_map.json and asm_verbose.s.
A deep-analyze pass adds cfg.json, mca.json, opt_remarks.json, reg_pressure.json,
semantic.json, and analysis_summary.json.

Markdown and JSON reports land in out/asmlab/reports/ with matching provenance
files. Baselines are stored as baselines/<fn>.json plus per-arch snapshot dirs.

## Limits

Static models can disagree with hardware by a wide margin. CFG recovery misses
indirect branches and exception edges. Register pressure is mention counting, not
true liveness. perf stat runs on Linux x86 only in the default pipeline.
ast_schema.json is a stub until full Clang AST mapping exists.

On Apple M1 you often get a thin call boundary instead of the full kernel. For
spill and dependency ranking, prefer x86-64-v3 when the slice does not expose the
body you care about.

Instruction counts from the default pipeline can suggest where to look, but they
do not prove causation on their own.

---

Part of [CCMath](https://github.com/Rinzii/ccmath). Apache-2.0 WITH LLVM-exception.
See [LICENSE](../../LICENSE).
