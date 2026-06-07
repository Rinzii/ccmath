# CPU knowledge base

asmlab stores structured CPU and microarchitecture knowledge used when interpreting assembly, llvm-mca output, source maps, path scenarios, and benchmarks.

Location: `tools/asmlab/knowledge/`

Loader: `tools/asmlab/scripts/cpu_knowledge.py`

## What it is (and is not)

This is a **selected knowledge base**: curated facts, references, pattern semantics, and host calibration records. It is **not**:

- a complete uops.info or Agner dataset
- a substitute for local benchmarks on the target host
- proof of performance from static pattern matching

Store references and seed facts. Add numeric timings only when a source id backs them. Unknown values stay `null` with `confidence: unknown`.

## What it contains

| Area | File(s) | Purpose |
| --- | --- | --- |
| Schema | schema.json | Documented confidence, trust, and status enums |
| Source registry | sources.json | Trusted references (uops.info, Agner, vendor manuals, static tools, local calibration) |
| Microarchitectures | microarchitectures.json | asmlab arch labels mapped to vendor/core metadata and claim guardrails |
| Calibration | calibration/*.json | Host-specific lessons from golden and M1/x86 calibration runs |
| Pattern rules | pattern_rules/*.json | Reusable asm pattern semantics (stack-serial FMA, thin call slices) |
| Instruction facts | instruction_facts/x86, aarch64 | Seed facts with null timings unless sourced |

## Confidence model

Every fact should record:

- source id (from sources.json)
- confidence (high, medium, low, unknown, or documented calibration-specific values in schema.json)
- scope (isa, arch, function, scenario)

Rules:

- null timing values mean unknown, not zero.
- null timing with high or medium confidence is a validation error.
- Static model output is advisory until benchmark and accuracy evidence agree.
- Local calibration outranks folklore for host-specific scope warnings.
- Measured databases (uops.info, Agner) outrank llvm-mca for per-instruction x86 facts when available.

## Validation

```bash
python3 tools/asmlab/scripts/asmlab.py knowledge validate
```

Validates sources, microarchitectures, calibration, pattern rules, instruction facts, cross-references, and enum values.

## Report integration

Function and scenario reports attach an **advisory** `cpu_knowledge` block:

- microarchitecture entry when registered
- calibration status and scope warnings
- **heuristic** pattern rule matches from region.s and path_analysis (labeled advisory, not proof)
- kernel exposure heuristic for deciding when static spill/dependency notes apply

### Apple M1

M1 `powf_impl` reports show thin-slice and call-boundary warnings when the analyzed region is harness-like or ends in `bl` to the kernel. Golden calibration trust on M1 does **not** generalize to all M1 static claims.

### x86-64-v3

Preferred-static-target notes for C1 spill/dependency review appear only when the artifact exposes kernel coverage (scenario report with source map, or function report with sufficient internal attribution). They do not claim universal truth across hosts or compilers.

## Adding facts safely

1. Add or cite a source in sources.json.
2. Store the fact with source id and confidence.
3. Use null for unknown numeric timings.
4. Prefer local calibration JSON for host-specific lessons.
5. Add pattern rules instead of hardcoding detection in multiple scripts.
6. Run `knowledge validate` before committing knowledge changes.

## Why static facts do not prove performance

llvm-mca, OSACA, and uiCA model scheduling. They do not measure cache, OS effects, or turbo behavior. Heuristic pattern matches correlate with known asm shapes but are not measured proof.

The knowledge base separates:

- instruction semantics and vendor table references
- static model estimates
- local benchmark direction
- accuracy preservation

A static win is correlation until benchmark and accuracy gates agree.
