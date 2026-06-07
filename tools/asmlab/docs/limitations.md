# Limitations

## Static vs measured

llvm-mca, microarch heuristics, and instruction tagging are static models. They can
disagree with hardware by large margins. perf.json is the only path toward measured
validation and is optional.

## CFG recovery

Blocks are split from linearized mapped instructions. Indirect branches, exception
edges, and unresolved jump targets are marked unknown. Block IDs may shift between
builds when codegen layout changes.

## Register pressure

Live ranges use cumulative register mention counting, not true liveness analysis.
Spill detection matches rsp-relative mov patterns only.

## Optimization remarks

YAML parsing is minimal (stdlib only). Remarks report compiler intent, not guaranteed
runtime impact. Missed vectorization does not always mean a slowdown.

## MCA per-block

Isolated block slices omit cross-block dependencies and may over- or under-estimate
pressure.

## Platform support

| Feature | Linux x86 | macOS x86 emit | macOS ARM |
| --- | --- | --- | --- |
| llvm-mca | yes | yes | partial model |
| opt remarks | yes | yes | yes |
| perf stat | yes | no | no |
| Instruments | no | stub | stub |

## Causality

The tooling labels evidence as correlation, likely_causal, or proven. Nothing in the
default pipeline claims proven causation. Instruction-count deltas alone are never
sufficient for regression root-cause.

## AST attribution

ast_schema.json defines future join keys only. Full Clang AST mapping is not implemented.

## Unsupported architectures

Deep MCA uses llvm scheduling models per ARCHES entry in _asmlab_common.py. Cores
without credible llvm-mca models should treat MCA output as advisory only.
