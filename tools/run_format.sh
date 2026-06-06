#!/bin/bash
# Check header formatting and apply clang-format only when needed.

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
bash "${SCRIPT_DIR}/ensure_format.sh"
