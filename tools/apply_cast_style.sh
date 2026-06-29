#!/bin/bash
# Auto-fix C-style and concrete casts under include/ with clang-tidy.
# Casts written as T(value) with a template parameter type are left alone.

set -euo pipefail

CLANG_TIDY="${CLANG_TIDY:-clang-tidy}"
checks='-*,google-readability-casting,cppcoreguidelines-pro-type-cstyle-cast'
compile_args=(-std=c++17 -Iinclude -xc++ -Wno-pragma-once-outside-header)

if ! find include -name '*.hpp' -print -quit | grep -q .; then
    echo "No header files found under include/."
    exit 0
fi

while IFS= read -r -d '' file; do
    # Skip headers that do not parse on their own.
    if "${CLANG_TIDY}" "${file}" --quiet --checks="${checks}" -- "${compile_args[@]}" 2>&1 \
        | grep -q 'clang-diagnostic-error'; then
        continue
    fi
    "${CLANG_TIDY}" "${file}" --quiet --fix --checks="${checks}" -- "${compile_args[@]}" >/dev/null 2>&1 || true
done < <(find include -name '*.hpp' -print0)

echo "Cast-style auto-fixes applied (concrete casts only)."
