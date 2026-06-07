# Example reports

Generate examples locally:

```bash
cd /path/to/ccmath

# Small kernel (hardware lowering)
python tools/asmlab/scripts/asmlab.py deep-analyze sqrt --arch x86-64-v3 --deep-analyze
python tools/asmlab/scripts/asmlab.py view sqrt --arch x86-64-v3 --deep-analyze

# Larger kernel (internal path)
python tools/asmlab/scripts/asmlab.py deep-analyze powf_impl --arch x86-64-v3 --deep-analyze

# Baseline + synthetic regression diff
python tools/asmlab/scripts/asmlab.py baseline powf_impl --arch x86-64-v3 --deep-analyze
# change codegen or flags, then:
python tools/asmlab/scripts/asmlab.py diff powf_impl --arch x86-64-v3 --deep-analyze
```

## Output locations

| Artifact | Path |
| --- | --- |
| CFG | out/asmlab/<fn>/<variant>/cfg.md |
| MCA | out/asmlab/<fn>/<variant>/mca.md |
| Remarks | out/asmlab/<fn>/<variant>/opt_remarks.md |
| Spills | out/asmlab/<fn>/<variant>/spill_report.md |
| Semantic | out/asmlab/<fn>/<variant>/semantic.json |
| Deep diff | out/asmlab/<fn>/<variant>/analysis_diff.md |
| History | out/asmlab/history/<fn>.json |
| Viewer | out/asmlab/views/<fn>/<arch>/index.html |

## Validation strategy

1. Smoke: deep-analyze completes without error for sqrt and powf_impl on x86-64-v3.
2. Artifact presence: cfg.json, mca.json, opt_remarks.json, analysis_summary.json exist.
3. Sanity: cfg block_count > 0 for multi-block kernels.
4. MCA: mca.json function.ipc is numeric when llvm-mca is on PATH.
5. Remarks: opt_remarks.json remark_count > 0 for O2/O3 builds.
6. Diff: analysis_diff.json written after baseline --deep-analyze then diff --deep-analyze.
7. Viewer: index.html loads and shows analysis panel when deep artifacts exist.
8. Non-regression: source_map.json instruction_count unchanged vs --source-map only run.

Run smoke after toolchain changes:

```bash
python tools/asmlab/scripts/asmlab.py deep-analyze sqrt --arch x86-64-v3 --deep-analyze
test -f out/asmlab/sqrt/x86-64-v3-clang-O2/cfg.json
test -f out/asmlab/sqrt/x86-64-v3-clang-O2/mca.json
```
