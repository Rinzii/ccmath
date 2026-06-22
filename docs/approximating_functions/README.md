Build the PDF:

```bash
./build.sh
```

Regenerate Sollya data files and build:

```bash
./build.sh --regen-data
```

Run individual Sollya scripts:

```bash
sollya sollya/sin_taylor_error.sollya > logs/sin_taylor_error.log 2>&1
sollya sollya/sin_small_error.sollya > logs/sin_small_error.log 2>&1
sollya sollya/sin_taylor_degree_error.sollya > logs/sin_taylor_degree_error.log 2>&1
sollya sollya/sin_minimax_compare.sollya > logs/sin_minimax_compare.log 2>&1
sollya sollya/sin_small_fpminimax.sollya > logs/sin_small_fpminimax.log 2>&1
sollya sollya/exp_small_fpminimax.sollya > logs/exp_small_fpminimax.log 2>&1
sollya sollya/hard_case_scan.sollya > logs/hard_case_scan.log 2>&1
sollya sollya/log2_core_table.sollya > logs/log2_core_table.log 2>&1
sollya sollya/log2_core_poly.sollya > logs/log2_core_poly.log 2>&1
sollya sollya/log2_core_points.sollya > logs/log2_core_points.log 2>&1
```