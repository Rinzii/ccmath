# Runtime implementation headers

> [!CAUTION]
> Not public API. Signatures and paths can change in any commit.

Runtime-only kernels (`*_rt`), SIMD paths, and other code that must not run at
compile time in the current dispatch shape.

## Folders

- `generic`: portable runtime implementations
- `simd`: SIMD-specific kernels and wrappers
