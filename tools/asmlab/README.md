<img src="../../docs/resources/ccmath_logo_white.png" style="width: 35%;" alt="CCMath Logo">

# CCMath asmlab

Assembly analysis and performance validation for [CCMath](../../README.md).
Script-driven workflow for inspecting the assembly CCMath emits, checking which
path was measured, scoring it with static cost models, and only then looking at
benches and accuracy.

Lives under tools/asmlab/. Never edits include/ccmath/. Outputs go to
out/asmlab/ (gitignored).

## Bootstrap

```bash
bash tools/asmlab/scripts/bootstrap.sh
python3 tools/asmlab/scripts/asmlab.py list
```

## Everyday commands

Path check on a registered kernel:

```bash
python3 tools/asmlab/scripts/asmlab.py verify pow_impl --arch x86-64-v3
```

Report with source map and optional HTML viewer:

```bash
python3 tools/asmlab/scripts/asmlab.py report pow_impl --arch x86-64-v3 --source-map
python3 tools/asmlab/scripts/asmlab.py view pow_impl --arch x86-64-v3 --open
```

Baseline, re-emit, diff:

```bash
python3 tools/asmlab/scripts/asmlab.py baseline powf_impl --arch x86-64-v3 --source-map
python3 tools/asmlab/scripts/asmlab.py diff powf_impl --arch x86-64-v3 --source-map
```

Deep pass (CFG, per-block MCA, opt remarks, register pressure, semantics):

```bash
python3 tools/asmlab/scripts/asmlab.py deep-analyze powf_impl --arch x86-64-v3 --deep-analyze
```

Production accuracy gate:

```bash
python3 tools/asmlab/scripts/asmlab.py gate powf --mode simple
```

Profile benchmark (where wired):

```bash
bash tools/asmlab/scripts/bench.sh sqrt --profile positive_finite_general
bash tools/asmlab/scripts/bench.sh powf_impl --profile positive_finite_general
```

## When is a perf change worth merging?

All of these, not just a static model win:

1. Path verified. The measured symbol is the CCMath kernel, not libm or a thin jmp.
2. Static model improved on the arches you care about (llvm-mca is always run).
3. Accuracy unchanged. gate passes for the function manifest.
4. Benchmark improved on the relevant input profile.

Static analysis alone means worth testing, not accepted. llvm-mca and friends are
estimates. Disagreement between tools is normal.

## Constexpr vs runtime

Separate checks. constexpr_check.py compiles static_assert cases from the registry.
Runtime emit uses the harness templates with volatile guards so arguments are not
folded away.

Do not infer runtime codegen from a constexpr probe or the other way around.

Harness modes in registry/functions.json:

| Mode | Typical target |
| --- | --- |
| runtime_no_flatten | Public API, see dispatch |
| runtime_flatten | Impl kernels |
| constexpr_probe | gen paths |
| path_direct | Same as runtime_flatten for direct entries |

Path categories (classify.py) include ccmath_internal_kernel, external_libm_call,
hardware_instruction_lowering, unknown, and others in registry/path_categories.json.
If you see pow or _pow in the slice, you measured libm, not pow_impl.

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

PATH MISMATCH in a scenario report means stop. Comparing static metrics across
different paths is invalid.

Artifacts live under out/asmlab/<fn>/scenarios/<scenario>/.

## Candidate variants

Edit candidates only under out/asmlab/variants/, never production headers until
review is done.

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

Stale variants (base file changed after init) are rejected in scenario diff.
variant patch writes a review diff. It does not apply by default.

candidate_accuracy is isolated from production gate. A bench win without
candidate_accuracy pass is not_enough_evidence.

## Benchmarks

Profiles are in registry/benchmark_profiles.json. powf_impl benches the isolated
impl kernel (tools/asmlab/bench/powf_impl_bench.cpp), not the public ccm::powf
wrapper. Public pow bench wiring is still incomplete. Do not use sqrt bench
results as pow evidence.

bench.sh prints wiring hints when a profile is not configured. Add targets under
tools/asmlab/bench/ and registry entries to wire a new case.

## Source maps

Pass --source-map on emit, report, baseline, diff, or deep-analyze. Produces
source_map.json and optional asm_diff.json on diff. Mapping at O2/O3 is
approximate. line 0 means uncertain attribution.

## Golden calibration

Validates asmlab analysis on tool-owned fixtures (Horner vs Estrin under
tools/asmlab/golden/), not CCMath numerical correctness.

```bash
python3 tools/asmlab/scripts/run_golden_analysis.py --quick
python3 tools/asmlab/scripts/test_golden_quick.py -v
```

## CPU knowledge

Selected microarchitecture facts with source and confidence live in knowledge/.
Loader is scripts/cpu_knowledge.py. Reports get advisory annotations, not proof.

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

Per emit variant: emit_meta.json, path_analysis.json, metrics.json, region.s.
With --source-map: source_map.json, asm_verbose.s.
With --deep-analyze: cfg.json, mca.json, opt_remarks.json, reg_pressure.json,
semantic.json, analysis_summary.json.
Reports: out/asmlab/reports/<fn>.md, .json, .provenance.json.
Baselines: baselines/<fn>.json plus per-arch snapshot dirs.

## Limits

- Static models can be wrong by large margins versus hardware.
- CFG recovery misses indirect branches and exception edges.
- Register pressure uses mention counting, not true liveness.
- perf stat only on Linux x86 in the default pipeline.
- ast_schema.json is a placeholder. Full Clang AST mapping is not implemented.
- Apple M1 slices are often thin call boundaries. Prefer x86-64-v3 for spill and
  dependency ranking when the kernel is not fully exposed.

Nothing in the default pipeline claims proven causation from instruction counts
alone.

---

Part of [CCMath](https://github.com/Rinzii/ccmath). Apache-2.0 WITH LLVM-exception.
See [LICENSE](../../LICENSE).
