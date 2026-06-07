# CCMath asmlab: CPU knowledge base

Selected microarchitecture facts for asmlab report notes. Source-attributed,
not a full uops.info or Agner scrape.

See [../README.md](../README.md) for the full asmlab guide.

## Layout

```text
knowledge/
  schema.json
  sources.json
  microarchitectures.json
  calibration/
  pattern_rules/
  instruction_facts/x86/
  instruction_facts/aarch64/
```

Null timings mean unknown. Validation fails if a fact claims high confidence
without a source.

## Validate

```bash
python3 tools/asmlab/scripts/asmlab.py knowledge validate
```

## Loader

scripts/cpu_knowledge.py attaches advisory cpu_knowledge blocks to function and
scenario reports via annotate_function_report and annotate_scenario_report.
