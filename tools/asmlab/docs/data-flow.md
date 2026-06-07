# Data flow

## Existing path (unchanged)

```
functions.json
    -> emit.py (harness.cpp, asm.s, region.s, emit_meta.json)
    -> classify.py (path_analysis.json)
    -> analyze.py (metrics.json via llvm-mca --json)
```

## Source traceability path (unchanged)

```
emit + --source-map
    -> source_map.py (asm_verbose.s, source_map.json/md)
    -> asm_diff.py (asm_diff.json/md on diff)
    -> view.py (static HTML)
```

## Deep analysis path (new layer)

```
emit + --deep-analyze  (implies --source-map)
    -> source_map.py
    -> analysis_pipeline.py
         -> opt_remarks.py (opt.yaml via -fsave-optimization-record)
         -> cfg_analysis.py
         -> mca_deep.py (per-function + per-block llvm-mca)
         -> reg_pressure.py
         -> microarch_model.py
         -> vectorization.py
         -> semantic_analysis.py
         -> perf_profile.py (optional Linux perf)
         -> history_track.py
         -> ast_schema.py (stub)
    -> analysis_summary.json
```

## Baseline and diff

```
baseline --deep-analyze
    -> baselines/<fn>.json (metrics snapshot, unchanged)
    -> baselines/<fn>/<arch-compiler-flags>/source_map.json
    -> baselines/<fn>/<arch-compiler-flags>/*.json (deep artifacts)

diff --deep-analyze
    -> re-emit + analyze
    -> asm_diff.py
    -> analysis_diff.py (current variant vs baseline deep dir)
```

## Viewer

view.py embeds source_map, metrics, cfg, mca, semantic, analysis_diff as one JSON
blob in static HTML. No server required.

## Reproducibility

- compile.cmd records exact clang invocation
- provenance.json records tool versions when report runs
- history/<fn>.json appends commit hash and summary per deep-analyze
