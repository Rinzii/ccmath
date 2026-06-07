# Candidate variants

asmlab supports isolated candidate-source testing without editing `include/ccmath/`.
Candidates live under `out/asmlab/variants/` (gitignored via `/out/`). They are
**temporary experiment workspaces**, not committed production sources.

## Workflow

```bash
# 1. Create candidate from registered base implementation
python3 tools/asmlab/scripts/asmlab.py variant init powf_impl --name no_extra_temp

# 2. Edit the candidate copy only
$EDITOR out/asmlab/variants/powf_impl/no_extra_temp/candidate.hpp

# 3. Scenario report (original + candidate)
python3 tools/asmlab/scripts/asmlab.py variant scenario report powf_impl no_extra_temp \
  positive_finite_general --arch x86-64-v3 --source-map

# 4. Diff original vs candidate on the same scenario
python3 tools/asmlab/scripts/asmlab.py variant scenario diff powf_impl no_extra_temp \
  positive_finite_general --arch x86-64-v3 --source-map

# 5. Benchmark original vs candidate impl kernels
python3 tools/asmlab/scripts/bench_impl.py powf_impl --profile positive_finite_general
python3 tools/asmlab/scripts/bench_impl.py powf_impl --profile positive_finite_general \
  --variant no_extra_temp

# 6. Candidate-specific accuracy (isolated variant, not production gate)
python3 tools/asmlab/scripts/asmlab.py variant accuracy powf_impl no_extra_temp --mode simple
python3 tools/asmlab/scripts/asmlab.py variant accuracy powf_impl no_extra_temp --mode rigorous

# 7. Generate review patch (not applied by default)
python3 tools/asmlab/scripts/asmlab.py variant patch powf_impl no_extra_temp
```

`variant scenario report|diff|validate` runs benchmarks when the scenario defines
`benchmark_profile`. `variant scenario validate` also runs candidate simple accuracy
before scenario gates. Reports show `benchmark status: ran` or an explicit failure reason.

## Workspace layout

```text
out/asmlab/variants/<function>/<variant>/
  manifest.json
  candidate.hpp
  notes.md
  patch.diff          (after variant patch)
```

`manifest.json` records base file, base git commit, base content hash, candidate path,
and the harness call expression.

## Compilation strategy

The variant harness includes `candidate.hpp` from the variant workspace and calls a
symbol in `asmlab::variant::<function>::<variant>::...`. Production headers are unchanged.
Dependencies still resolve through `-I include/`.

Registered targets: `registry/variant_targets.json`.

## Staleness

If the base file under `include/ccmath/` changes after variant creation, reports warn:

```text
variant is stale relative to base source
```

Scenario diff rejects stale candidates. Refresh with `variant init --force` after review.

## Reports

Candidate scenario reports label:

- `source kind: candidate`
- `candidate file`, `base source file`, `base commit`, `staleness`
- original vs candidate observed paths
- `reject_candidate: true` when the candidate changes the intended path

## Production vs candidate validation

Candidate validation is **separate** from production validation. Scenario and
compare reports never collapse these into one field.

| Field | Scope |
| --- | --- |
| `production_accuracy` | Manifest gate on unchanged `include/ccmath/` code |
| `candidate_accuracy` | Isolated variant under `out/asmlab/variants/` |

A production `powf` gate pass does **not** prove an isolated `powf_impl` candidate is
correct. Benchmark wins without `candidate_accuracy: pass` are `not_enough_evidence`, not
merge-grade.

Candidate accuracy status values:

```text
pass | fail | not_run | not_wired | advisory | production_only
```

`powf_impl` edge-case groups: `tools/asmlab/registry/candidate_edge_cases.json`.
Input corpora: `tools/asmlab/registry/candidate_edge_case_inputs.json`.

Simple-mode candidate accuracy (std::powf oracle, 4 ULP) is wired for:
positive_finite_general, near_one, integer_exponent, negative_base_integer_exponent,
negative_base_noninteger_exponent, zero_signed_zero, nan_inf, subnormal,
overflow_boundary, underflow_boundary, known_worst_ulp.

Still `not_wired`: rounding_sensitive, ziv_retry_path, rigorous MPFR campaign.

Gate artifacts: `out/asmlab/gates/candidates/<fn>-<variant>-<mode>.json`

Benchmark gates use explicit status labels (`ran`, `advisory`, `not_wired`,
`not_applicable`). Unsupported gates must not look like passes.

## Multi-candidate compare

Compare original vs many candidates across scenarios in one run:

```bash
python3 tools/asmlab/scripts/asmlab.py variant compare powf_impl \
  --variants late_ex_sign estrin_poly \
  --scenarios positive_finite_general near_one integer_exponent \
  --arch x86-64-v3 --source-map
```

See `multi-candidate-analysis.md` for ranking policy and report layout.

## Patch generation

`variant patch` writes `patch.diff` comparing the candidate body to the base file.
Default behavior: **patch generated but not applied**. Use `--apply` only after explicit
review (apply is not automated).
