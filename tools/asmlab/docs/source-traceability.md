# Source-to-assembly traceability

asmlab can map emitted assembly instructions back to source lines using compiler
debug line tables. This helps answer which source code caused a given instruction
sequence, and what changed between optimization attempts.

## Generate a source map

```bash
python3 tools/asmlab/scripts/asmlab.py emit pow_impl --arch x86-64-v3 --source-map
python3 tools/asmlab/scripts/asmlab.py report pow_impl --arch x86-64-v3 --source-map
```

Headless artifacts per variant:

| File | Content |
| --- | --- |
| `asm_verbose.s` | Assembly with `.loc` directives and file:line comments |
| `source_map.json` | Machine-readable instruction-to-source map |
| `source_map.md` | Compact terminal-friendly map |

## Read source attribution

Each mapped instruction includes:

- `source_layer`: generated_harness, ccmath_internal, ccmath_public, system_header, unknown
- `source_attribution`: harness-only, ccmath-internal, ccmath-public, system, unknown
- `mapping_confidence`: high, medium, low, unknown

**high** means a direct line-table match from `.loc` and verbose asm comment.

**low** or line `0` often means optimizer motion or ambiguous debug info.

**primary_attribution** uses instruction-majority counts (`attribution_counts`), not
whether any single line reached ccmath.

Reports state **harness-only** when most mapped instructions trace to the generated harness.
Reports state **ccmath-internal** only when that attribution is the majority.

`inline_stack` and `inline_context` are metadata-derived follow chains from emit isolation.
They are not full DWARF inline stacks unless DWARF parsing is added later.

Line `0` mappings are rendered as uncertain, not precise source lines.

## Source-aware diff

```bash
python3 tools/asmlab/scripts/asmlab.py baseline pow_impl --arch x86-64-v3 --source-map
# edit source
python3 tools/asmlab/scripts/asmlab.py diff pow_impl --arch x86-64-v3 --source-map
```

Writes `asm_diff.json` and `asm_diff.md` with:

- added/removed instructions
- changed source lines with before/after instruction counts
- tag deltas (spills, FMA, external calls)
- likely effects (conservative, correlational)

## HTML viewer

```bash
python3 tools/asmlab/scripts/asmlab.py view pow_impl --arch x86-64-v3
python3 tools/asmlab/scripts/asmlab.py view pow_impl --arch x86-64-v3 --open
```

Output: `out/asmlab/views/<fn>/<arch>/index.html`

The viewer is optional. All correlation data remains in JSON and markdown for CI and
terminal use.

## Limitations

- Mapping is approximate under `-O2`/`-O3`. Inlining and register allocation move code.
- Template-heavy headers may show multiple inline stack levels inconsistently.
- Volatile harness loads add harness instructions that are not part of ccmath.
- Likely effects describe correlation, not proven causation.
