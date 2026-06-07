#!/usr/bin/env bash
# Copyright (c) Ian Pike
# Copyright (c) CCMath contributors
#
# CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
# See LICENSE for more information.
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# Accuracy gate for asmlab. A performance change is only allowed to land if accuracy
# is unchanged. This script reuses the existing test suites with function-specific
# coverage from registry/accuracy_manifest.json.
#
#   simple   (default) native build + manifest-driven ctest
#   rigorous            Linux/MPFR campaign via .AI/linux-test
#   all                 simple then rigorous
#
# Usage:
#   scripts/accuracy_gate.sh                    # simple suite, all functions
#   scripts/accuracy_gate.sh simple powf        # manifest-driven powf simple gate
#   scripts/accuracy_gate.sh rigorous powf      # rigorous powf campaign
#   scripts/accuracy_gate.sh all pow_impl

set -euo pipefail

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
project_root="$(cd "${script_dir}/../../.." && pwd)"
mode="${1:-simple}"
fn="${2:-}"
build_dir="${project_root}/out/asmlab-gate"
manifest="${script_dir}/../registry/accuracy_manifest.json"
gates_dir="${project_root}/out/asmlab/gates"

lookup_manifest() {
    local key="$1"
    python3 - "${manifest}" "${key}" <<'PY'
import json, sys
path, key = sys.argv[1], sys.argv[2]
with open(path) as fh:
    data = json.load(fh)
entry = data.get(key)
if not entry:
    # Fall back to registry ulp_label
    reg_path = path.replace("accuracy_manifest.json", "functions.json")
    with open(reg_path) as fh:
        reg = json.load(fh)["functions"]
    if key in reg:
        label = reg[key].get("ulp_label") or key
        entry = data.get(label)
if not entry:
    print("", end="")
    sys.exit(0)
print(json.dumps(entry))
PY
}

resolve_gate_key() {
    local fn="$1"
    if [[ -z "${fn}" ]]; then
        echo ""
        return
    fi
    python3 - "${manifest}" "${fn}" <<'PY'
import json, sys
path, fn = sys.argv[1], sys.argv[2]
with open(path.replace("accuracy_manifest.json", "functions.json")) as fh:
    reg = json.load(fh)["functions"]
if fn in reg:
    print(reg[fn].get("ulp_label") or fn)
else:
    print(fn)
PY
}

write_gate_report() {
    local fn="$1" mode="$2" result="$3"
    mkdir -p "${gates_dir}"
    local out="${gates_dir}/${fn:-all}-${mode}.json"
    python3 - "${out}" "${fn}" "${mode}" "${result}" <<'PY'
import json, sys, datetime
out, fn, mode, result = sys.argv[1:5]
doc = {
    "function": fn or "all",
    "mode": mode,
    "timestamp": datetime.datetime.now(datetime.timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ"),
    "result": result,
}
with open(out, "w") as fh:
    json.dump(doc, fh, indent=2)
    fh.write("\n")
print(out)
PY
}

run_simple() {
    echo ">> accuracy gate: simple suite (native, manifest-driven)" >&2
    cmake -S "${project_root}" -B "${build_dir}" -G Ninja \
        -DCMAKE_BUILD_TYPE=Release \
        -DCCMATH_BUILD_TESTS=ON \
        -DCCMATH_BUILD_SIMPLE_TESTS=ON \
        -DCCMATH_BUILD_RIGOROUS_TESTS=OFF \
        -DCCMATH_BUILD_BENCHMARKS=OFF \
        -DCCMATH_BUILD_EXAMPLES=OFF >/dev/null
    cmake --build "${build_dir}" >/dev/null

    local gate_key
    gate_key="$(resolve_gate_key "${fn}")"
    local entry
    entry="$(lookup_manifest "${gate_key}")"

    if [[ -z "${fn}" ]]; then
        ctest --test-dir "${build_dir}" -L simple --output-on-failure
        write_gate_report "all" "simple" "pass" >/dev/null
        return
    fi

    if [[ -z "${entry}" ]]; then
        echo "no accuracy manifest for '${fn}' (key=${gate_key}); falling back to -R ${fn}" >&2
        ctest --test-dir "${build_dir}" -L simple -R "${fn}" --output-on-failure
        write_gate_report "${fn}" "simple" "pass" >/dev/null
        return
    fi

    echo ">> powf rigorous accuracy style report (simple):" >&2
    python3 - "${entry}" <<'PY' >&2
import json, sys
entry = json.loads(sys.argv[1])
labels = entry.get("simple", [])
print("  labels run: %s" % ", ".join(labels))
print("  MPFR oracle: %s" % ("yes" if entry.get("mpfr_oracle") else "no"))
print("  boundary corpus: %s" % ("yes" if entry.get("boundary_corpus") else "no"))
print("  worst-case replay: %s" % ("yes" if entry.get("worst_case_replay") else "no"))
PY

    local label
    for label in $(python3 - "${entry}" <<'PY'
import json, sys
for item in json.loads(sys.argv[1]).get("simple", []):
    print(item.split(":")[0])
PY
); do
        echo ">> running ctest -R ${label}" >&2
        ctest --test-dir "${build_dir}" -L simple -R "${label}" --output-on-failure
    done

    write_gate_report "${fn}" "simple" "pass" >/dev/null
    echo "  result: pass" >&2
}

run_rigorous() {
    echo ">> accuracy gate: rigorous MPFR campaign (Docker)" >&2
    local linux_script="${project_root}/.AI/linux-test/linux_rigorous_test.sh"
    if [[ ! -x "${linux_script}" && ! -f "${linux_script}" ]]; then
        echo "rigorous gate needs ${linux_script} (Docker/Colima). See .AI/linux-test/README.md" >&2
        return 2
    fi
    local gate_key
    gate_key="$(resolve_gate_key "${fn}")"
    local entry
    entry="$(lookup_manifest "${gate_key}")"

    if [[ -n "${entry}" ]]; then
        python3 - "${entry}" "${fn}" <<'PY' >&2
import json, sys
entry = json.loads(sys.argv[1])
fn = sys.argv[2]
labels = entry.get("rigorous", [])
print("%s rigorous accuracy:" % fn)
print("  labels run: %s" % ", ".join(labels))
print("  MPFR oracle: %s" % ("yes" if entry.get("mpfr_oracle") else "no"))
print("  boundary corpus: %s" % ("yes" if entry.get("boundary_corpus") else "no"))
print("  worst-case replay: %s" % ("yes" if entry.get("worst_case_replay") else "no"))
PY
    fi

    if [[ -n "${fn}" ]]; then
        bash "${linux_script}" rigorous "${fn}"
    else
        bash "${linux_script}" rigorous
    fi
    write_gate_report "${fn:-all}" "rigorous" "pass" >/dev/null
    echo "  result: pass" >&2
}

case "${mode}" in
    simple) run_simple ;;
    rigorous) run_rigorous ;;
    all) run_simple && run_rigorous ;;
    *)
        echo "usage: accuracy_gate.sh [simple|rigorous|all] [registry-function]" >&2
        exit 1
        ;;
esac

echo ">> accuracy gate PASSED (${mode}${fn:+ for ${fn}})" >&2
