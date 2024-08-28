# CCMath Runtime Headers Info

> [!CAUTION]
> This directory contains internal runtime headers for the CCMath library. These headers are not intended for public use
> and are subject to change at any time. They are used for internal implementation details and are not guaranteed to be
> stable or to have a stable API. Assume anything included here directly could change its signature at any moment.

This folder handles runtime specific optimizations and functions meant to speed up evaluation during runtime that is not
possible at compile time.

## Folders

- `generic`: Runtime specific generic implementations for the library that provide runtime specific optimizations to
  speed up execution times.
- `simd`: Internal implementation details for SIMD wrappers and SIMD specific functions for the library.
