# Constexpr vs runtime verification

asmlab treats compile-time behavior and runtime codegen as **separate** obligations.
Do not infer one from the other.

## Compile-time behavior

`constexpr_check.py` compiles a probe TU with `static_assert` cases from the registry
`constexpr_cases` field. It answers:

- Can the function compile in a constant-expression context?
- Do probe cases produce consistent constexpr results?

This does not emit assembly. It does not prove runtime dispatch choices.

## Runtime codegen

The runtime harness uses:

- `extern "C"` linkage for a stable symbol
- Volatile-guarded arguments to reduce constant folding
- Optional `flatten` for kernel inspection

The runtime section of each report lists:

- Requested vs analyzed symbol
- Whether a forwarder was followed
- Instruction, branch, and call counts
- Warnings for libm calls, trivial assembly, or possible constant fold

## Report sections

Reports always separate:

```text
Compile-time behavior
Runtime codegen
Path classification
```

A constexpr-capable function may execute at runtime. Constant-looking assembly does
not prove the constexpr branch was used. It may be folding, builtin lowering, or libm.

## When to use each harness mode

- **Public API** (`ccm::pow`): `runtime_no_flatten` to see dispatch.
- **Kernels** (`pow_impl`, `powf_rt`): `runtime_flatten` to inspect the body.
- **Gen paths** (`pow_gen`): `constexpr_probe` for compile-time verification only.
