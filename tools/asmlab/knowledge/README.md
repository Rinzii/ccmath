# asmlab CPU knowledge base

Structured microarchitecture knowledge for interpreting assembly, static models, source maps, scenarios, and benchmarks.

This is a **selected knowledge base**: source-attributed facts, pattern semantics, and calibration lessons. It is **not** a complete vendor timing database or a scraped copy of uops.info, Agner, or Arm tables.

## Principles

- Store evidence, source, scope, and confidence.
- Do not store unsourced folklore as fact.
- Use null and confidence unknown when timings are not established.
- Prefer measured local calibration over static model output.
- Prefer measured instruction databases (uops.info, Agner) for x86 facts when available.
- Heuristic pattern matches in reports are advisory, not proof.

## Layout

```text
knowledge/
  README.md
  schema.json               Documented enums for validation
  sources.json              Source registry (URLs, trust, data kinds)
  microarchitectures.json   asmlab arch to microarch metadata
  calibration/              Local calibration records per host/arch
  pattern_rules/            Reusable asm pattern semantics
  instruction_facts/          Seed instruction facts (null timings unless sourced)
    x86/
    aarch64/
```

## Validation

```bash
python3 tools/asmlab/scripts/asmlab.py knowledge validate
```

Checks JSON validity, source and pattern cross-references, enum values (see schema.json), null timing rules, and required calibration fields.

## Consumption

Python loader: tools/asmlab/scripts/cpu_knowledge.py

Reports call annotate_function_report and annotate_scenario_report to attach microarchitecture entries, calibration warnings, and heuristic pattern matches.

See tools/asmlab/docs/cpu-knowledge.md for human documentation.
