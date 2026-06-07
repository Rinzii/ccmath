# Benchmarking

Benchmarks require input profiles. A bare `powf: 5.1 ns/op` without profile context
is not sufficient for merge decisions.

## Profiles

Defined in `registry/benchmark_profiles.json`:

| Profile | Use |
| --- | --- |
| `positive_finite_general` | Default hot path |
| `near_one` | Values near 1.0 |
| `subnormal` | Subnormal inputs |
| `integer_exponent` | Integer exponent fast path |
| `nan_inf_signed_zero` | Special values |

Not every function uses every profile. See `function_profiles` in the registry.

## Usage

```bash
bash tools/asmlab/scripts/bench.sh sqrt --profile positive_finite_general
bash tools/asmlab/scripts/bench.sh sqrt --profile positive_finite_general --mode latency
bash tools/asmlab/scripts/bench.sh diff sqrt --profile positive_finite_general
```

## Benchmark modes

| Mode | Intent |
| --- | --- |
| `latency` | Dependent-chain style (min_time emphasis) |
| `throughput` | Independent calls (repetitions) |
| `mixed` | Default Google Benchmark behavior |

## Merge grade

Results are tagged `advisory` until pow benchmarks are wired. sqrt family benchmarks
run against `ccm_benchmark_sqrt` today.

## powf_impl isolated impl benchmarks

`powf_impl` uses `tools/asmlab/bench/powf_impl_bench.cpp`, built as `asmlab_bench_powf_impl`.
This measures the internal kernel through an isolated TU (not the public `ccm::powf` wrapper).

```bash
# Original impl kernel
python3 tools/asmlab/scripts/bench_impl.py powf_impl --profile positive_finite_general
bash tools/asmlab/scripts/bench.sh powf_impl --profile positive_finite_general

# Candidate variant (after variant init)
python3 tools/asmlab/scripts/bench_impl.py powf_impl --profile positive_finite_general \
  --variant test_candidate
bash tools/asmlab/scripts/bench.sh powf_impl --profile positive_finite_general --variant test_candidate

# Scenario-integrated (original + candidate + bench)
python3 tools/asmlab/scripts/asmlab.py variant scenario report powf_impl test_candidate \
  positive_finite_general --arch x86-64-v3
```

Benchmark status in scenario reports:

| Status | Meaning |
| --- | --- |
| `ran` | Google Benchmark executed and ns/op parsed |
| `failed` | Build or run failed |
| `unsupported` | Registry says not wired |
| `not_configured` | Scenario has no benchmark_profile |
| `pending` | Wired but bench not run yet |

## Known gap: public pow/powf wrappers

`ccm_benchmark_pow` for public `ccm::pow` / `ccm::powf` is not implemented. Running
`bench.sh powf` still fails with exit code 2. Use `powf_impl` impl benches for kernel work.

Do not treat a sqrt benchmark result as evidence for pow kernel changes.

## Output

Results write to `out/asmlab/benches/<fn>-<profile>.json` with function, profile, mode,
sample count, result ns, and merge_grade.
