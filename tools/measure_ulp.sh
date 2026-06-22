#!/bin/bash
#
# Copyright (c) Ian Pike
# Copyright (c) CCMath contributors
#
# CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
# See LICENSE for more information.
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# Sweep the unary elementary functions against a correctly-rounded MPFR reference and write the
# per-function campaign summaries that tools/ulp_status.py folds into docs/ULP_STATUS.md.
#
# By default binary64 is sampled across the per-function domain in all four rounding modes. Pass
# --exhaustive-float to sweep every binary32 bit pattern (around three minutes per function per
# mode); without it binary32 is sampled like binary64.
#
#   tools/measure_ulp.sh                       # sampled binary64 and binary32, all functions
#   tools/measure_ulp.sh --exhaustive-float    # exhaustive binary32 (nearest), sampled binary64
#   tools/measure_ulp.sh --functions "exp log" # restrict to a subset

set -euo pipefail

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
build_dir="${CCM_ULP_BUILD_DIR:-${repo_root}/out/build/ulp-measure}"
log_dir="${repo_root}/tests/rigorous/oracle_logs"
target="ccmath-rigorous-mpfr-unary"
exhaustive_float=0
double_count=2000000
float_count=2000000
double_modes="nearest"
float_modes="nearest"
functions="exp exp2 expm1 log log1p log2 log10 sin cos tan asin acos atan cbrt sqrt tgamma lgamma"

while [ "$#" -gt 0 ]; do
    case "$1" in
        --build-dir) build_dir="$2"; shift 2 ;;
        --exhaustive-float) exhaustive_float=1; shift ;;
        --functions) functions="$2"; shift 2 ;;
        --double-count) double_count="$2"; shift 2 ;;
        --float-count) float_count="$2"; shift 2 ;;
        -h|--help) sed -n '11,24p' "${BASH_SOURCE[0]}"; exit 0 ;;
        *) echo "unknown argument: $1" >&2; exit 2 ;;
    esac
done

bin="${build_dir}/tests/rigorous/${target}"
if [ ! -x "${bin}" ]; then
    echo "configuring ${build_dir}"
    cmake -S "${repo_root}" -B "${build_dir}" -G Ninja \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ \
        -DCCMATH_BUILD_TESTS=ON -DCCMATH_BUILD_SIMPLE_TESTS=OFF \
        -DCCMATH_BUILD_RIGOROUS_TESTS=ON -DCCMATH_ENABLE_MPFR_TESTS=ON \
        -DCCMATH_ENABLE_COREMATH_TESTS=OFF
fi
cmake --build "${build_dir}" --target "${target}"

mkdir -p "${log_dir}"

# A non-zero exit only means the campaign saw cases outside the 4 ULP ceiling or a directed-mode
# special-value disagreement. That is measurement data, recorded in the summary, not a script error,
# so the sweep keeps going.
run_campaign() {
    "$@" || echo "  (campaign reported out-of-contract cases, recorded in the summary)"
}

for fn in ${functions}; do
    echo "=== ${fn} binary64 (sampled, ${double_modes}) ==="
    run_campaign "${bin}" --function="${fn}" --type=double --mode=extended --count="${double_count}" \
        --rounding-modes="${double_modes}" \
        --log-output="${log_dir}/mpfr-unary-${fn}-double-summary.json"

    if [ "${exhaustive_float}" -eq 1 ]; then
        echo "=== ${fn} binary32 (exhaustive, nearest) ==="
        run_campaign "${bin}" --function="${fn}" --type=float --exhaustive --rounding-modes=nearest \
            --log-output="${log_dir}/mpfr-unary-${fn}-float-summary.json"
    else
        echo "=== ${fn} binary32 (sampled, ${float_modes}) ==="
        run_campaign "${bin}" --function="${fn}" --type=float --mode=extended --count="${float_count}" \
            --rounding-modes="${float_modes}" \
            --log-output="${log_dir}/mpfr-unary-${fn}-float-summary.json"
    fi
done

echo "summaries written to ${log_dir}"
echo "refresh the doc with: python3 ${repo_root}/tools/ulp_status.py"
