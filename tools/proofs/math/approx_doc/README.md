Run these commands from the repository root.

```bash
sollya tools/proofs/math/approx_doc/sollya/sin_taylor_error.sollya > tools/proofs/math/approx_doc/logs/sin_taylor_error.log 2>&1
sollya tools/proofs/math/approx_doc/sollya/sin_small_error.sollya > tools/proofs/math/approx_doc/logs/sin_small_error.log 2>&1
sollya tools/proofs/math/approx_doc/sollya/sin_taylor_degree_error.sollya > tools/proofs/math/approx_doc/logs/sin_taylor_degree_error.log 2>&1
sollya tools/proofs/math/approx_doc/sollya/log2_core_table.sollya > tools/proofs/math/approx_doc/logs/log2_core_table.log 2>&1
sollya tools/proofs/math/approx_doc/sollya/log2_core_poly.sollya > tools/proofs/math/approx_doc/logs/log2_core_poly.log 2>&1
sollya tools/proofs/math/approx_doc/sollya/log2_core_points.sollya > tools/proofs/math/approx_doc/logs/log2_core_points.log 2>&1
```

Installed Sollya on this machine accepts:

```text
supnorm(p, f, I, absolute, 1b-80)
supnorm(p, f, I, relative, 1b-80)
```

The `log2_core_poly.sollya` relative `supnorm` call may still fail on Sollya 8.0 for the transformed kernel. The exact warning text is preserved in `tools/proofs/math/approx_doc/logs/log2_core_poly.log`.
