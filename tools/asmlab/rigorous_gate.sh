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
internal_script="${project_root}/.AI/linux-test/linux_rigorous_test.sh"

if [[ ! -f "${internal_script}" ]]; then
    echo "rigorous gate: linux test script not found in this checkout" >&2
    echo "See CONTRIBUTING.md for rigorous validation setup." >&2
    exit 2
fi

exec bash "${internal_script}" "$@"
