#!/usr/bin/env bash
# Validate powl ld80 kernel approximations with sollya before ULP tests.
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../../../.." && pwd)"
SCRIPT="${ROOT}/tools/proofs/math/power/validate_powl_ld80_kernel.sollya"

if ! command -v sollya >/dev/null 2>&1; then
  echo "error: sollya not found on PATH" >&2
  exit 1
fi

echo "Running ${SCRIPT}..."
sollya "${SCRIPT}" || test $? -eq 3
