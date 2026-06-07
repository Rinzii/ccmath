# Artifact schemas

All deep-analysis JSON files include analysis_version where applicable.

## cfg.json

```json
{
  "analysis_version": 1,
  "block_count": 12,
  "edge_count": 14,
  "loop_count": 1,
  "blocks": [{
    "id": "B0",
    "instruction_count": 8,
    "start_ordinal": 1,
    "end_ordinal": 8,
    "source_regions": [{"file": "...", "line": 42}]
  }],
  "edges": [{"from": "B0", "to": "B1", "kind": "fallthrough"}],
  "dominators": {"B0": "B0", "B1": "B0"},
  "loops": [{"header": "B3", "back_edge_from": "B5", "kind": "natural_loop"}],
  "confidence": "medium"
}
```

## mca.json

```json
{
  "available": true,
  "function": {
    "ipc": 1.23,
    "block_rthroughput": 4.5,
    "port_pressure": {"SKLPort0": 1.2}
  },
  "blocks": [{"block_id": "B0", "ipc": 0.9, "instruction_count": 8}],
  "bottleneck": {
    "primary": "scheduler_pressure",
    "confidence": "correlation",
    "evidence": ["SKLPort5=1.10"]
  }
}
```

## opt_remarks.json

```json
{
  "remark_count": 120,
  "remarks": [{
    "kind": "Missed",
    "pass": "loop-vectorize",
    "file": "include/ccmath/...",
    "line": 100,
    "tags": ["vectorization", "missed_optimization"],
    "causal_level": "correlation"
  }],
  "by_tag": {"inlining": 40, "vectorization": 3}
}
```

## reg_pressure.json

```json
{
  "peak_gpr_mentions": 10,
  "peak_vec_mentions": 6,
  "spill_count": 2,
  "reload_count": 2,
  "spills": [{
    "ordinal": 45,
    "file": "...",
    "line": 200,
    "kind": "spill",
    "causal_level": "likely_causal"
  }],
  "block_pressure": [{"block_id": "B2", "spill_count": 2}]
}
```

## semantic.json

```json
{
  "classifications": [{
    "kind": "missed_fma_contraction",
    "confidence": "medium",
    "causal_level": "likely_causal",
    "source_hint": "polynomial evaluation"
  }],
  "regressions_vs_baseline": []
}
```

## analysis_diff.json

```json
{
  "mca": {"ipc_delta": -0.15, "bottleneck_from": "backend_pressure", "bottleneck_to": "dependency_chain"},
  "register_pressure": {"spill_delta": 3},
  "vectorization": {"vectorized_from": true, "vectorized_to": false},
  "new_semantic_classifications": []
}
```

## analysis_summary.json

Roll-up for reports and history snapshots.

## ast_schema.json

Architecture-only placeholder for future Clang AST entity IDs. Not populated by tooling yet.
