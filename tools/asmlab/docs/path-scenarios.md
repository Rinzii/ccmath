# Path scenarios

Path scenarios let you test a specific implementation route for a function, not only
the whole public API or one generic kernel emission.

## Why scenarios exist

Functions like `powf_impl` route through many branches: finite-normal kernels,
special cases, integer exponents, libm fallbacks, and platform-specific paths. A
single generic emit cannot answer which path you measured.

Each scenario fixes harness inputs and declares an intended path. Reports compare
intended vs observed classification and flag mismatches.

## Limitations

Scenario assembly is a **static path slice** from selected inputs. It is not a
dynamic trace and does not prove branch frequency.

## List scenarios

```bash
python3 tools/asmlab/scripts/asmlab.py scenario list sqrt
python3 tools/asmlab/scripts/asmlab.py scenario list powf_impl
```

## Run a scenario

```bash
python3 tools/asmlab/scripts/asmlab.py scenario report powf_impl positive_finite_general --arch x86-64-v3 --source-map
python3 tools/asmlab/scripts/asmlab.py scenario verify sqrt positive_finite_general --arch x86-64-v3
```

## Baseline and diff

```bash
python3 tools/asmlab/scripts/asmlab.py scenario baseline powf_impl near_one --arch x86-64-v3 --source-map
# edit code
python3 tools/asmlab/scripts/asmlab.py scenario diff powf_impl near_one --arch x86-64-v3 --source-map
```

## Variant comparison

Compare original vs a named candidate on the same scenario:

```bash
python3 tools/asmlab/scripts/asmlab.py variant scenario diff powf_impl my_candidate positive_finite_general --arch x86-64-v3 --source-map
```

If the candidate changes the observed path, the report rejects it even when static
metrics improve.

## Path mismatch

When observed path is not in the scenario acceptable set:

```text
PATH MISMATCH:
  intended: ccmath_internal_kernel
  observed: external_libm_call
```

Do not treat this scenario as a valid performance comparison.

## Artifacts

Under `out/asmlab/<fn>/scenarios/<scenario>/original/<arch-compiler-flags>/`:

- `scenario_report.json` / `scenario_report.md`
- `scenario_path_analysis.json`
- `scenario_metrics.json`
- `scenario_source_map.json` (with `--source-map`)
- `scenario_diff.json` (after baseline or variant diff)

## Viewer

Scenario HTML pages are written under:

`out/asmlab/views/scenarios/<fn>/<scenario>/<arch>/index.html`
