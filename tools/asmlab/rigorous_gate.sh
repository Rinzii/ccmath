#!/usr/bin/env bash
# Copyright (c) Ian Pike
# Copyright (c) CCMath contributors
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# Run Linux rigorous oracle tests in Docker. accuracy_gate.sh calls this for
# rigorous and all modes. Override with CCMATH_RIGOROUS_GATE if needed.
#
# Usage: tools/asmlab/rigorous_gate.sh [all|quick|...] [ctest-filter]

set -euo pipefail

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
project_root="$(cd "${script_dir}/../.." && pwd)"

if [[ -n "${CCMATH_RIGOROUS_GATE:-}" ]]; then
    rigorous_script="${CCMATH_RIGOROUS_GATE}"
else
    rigorous_script=""
    while IFS= read -r -d '' candidate; do
        rigorous_script="${candidate}"
        break
    done < <(find "${project_root}" -name linux_rigorous_test.sh -type f -print0 2>/dev/null)
fi

if [[ -z "${rigorous_script}" || ! -f "${rigorous_script}" ]]; then
    echo "rigorous gate: linux_rigorous_test.sh not found in this checkout" >&2
    echo "See CONTRIBUTING.md for rigorous validation setup." >&2
    exit 2
fi

exec bash "${rigorous_script}" "$@"
