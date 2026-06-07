#!/usr/bin/env bash
# Copyright (c) Ian Pike
# Copyright (c) CCMath contributors
#
# CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
# See LICENSE for more information.
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# Profile-specific runtime confirmation for asmlab. Static cost models err 9-36%
# versus real hardware. This runs registry-driven Google Benchmark targets.
#
# Usage:
#   scripts/bench.sh sqrt
#   scripts/bench.sh sqrt --profile positive_finite_general
#   scripts/bench.sh sqrt --profile positive_finite_general --mode latency
#   scripts/bench.sh diff sqrt --profile positive_finite_general
#   scripts/bench.sh powf --profile near_one   # fails clearly if not wired

set -euo pipefail

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
project_root="$(cd "${script_dir}/../../.." && pwd)"
profiles_json="${script_dir}/../registry/benchmark_profiles.json"
registry_json="${script_dir}/../registry/functions.json"
benches_dir="${project_root}/out/asmlab/benches"
build_dir="${project_root}/out/asmlab-bench"

action="run"
fn=""
profile="positive_finite_general"
bench_mode=""
variant=""
extra_args=()

while [[ $# -gt 0 ]]; do
    case "$1" in
        diff)
            action="diff"
            shift
            ;;
        --profile)
            profile="$2"
            shift 2
            ;;
        --mode)
            bench_mode="$2"
            shift 2
            ;;
        --variant)
            variant="$2"
            shift 2
            ;;
        --benchmark_filter=*)
            extra_args+=("$1")
            shift
            ;;
        --*)
            extra_args+=("$1")
            shift
            ;;
        *)
            if [[ -z "${fn}" ]]; then
                fn="$1"
            else
                extra_args+=("$1")
            fi
            shift
            ;;
    esac
done

fn="${fn:-sqrt}"

# powf_impl uses isolated asmlab impl benchmarks (original + candidate).
if [[ "${fn}" == "powf_impl" ]]; then
    cmd=(python3 "${script_dir}/bench_impl.py" powf_impl --profile "${profile}")
    if [[ -n "${variant}" ]]; then
        cmd+=(--variant "${variant}")
    fi
    echo ">> bench: ${cmd[*]}" >&2
    exec "${cmd[@]}"
fi

resolve_bench() {
    python3 - "${registry_json}" "${profiles_json}" "${fn}" "${profile}" <<'PY'
import json, sys
reg_path, prof_path, fn, profile = sys.argv[1:5]
with open(reg_path) as fh:
    reg = json.load(fh)["functions"]
with open(prof_path) as fh:
    prof = json.load(fh)
entry = reg.get(fn, {})
targets = prof.get("benchmark_targets", {})
# Map registry fn to bench target key
key = fn
if fn.endswith("_impl"):
    key = "pow_impl" if "powf" not in fn else "powf_impl"
elif fn.endswith("_rt"):
    key = fn.replace("_rt", "")
elif fn in ("sqrt_rt",):
    key = "sqrt"
tgt = targets.get(key) or targets.get(fn.split("_")[0], {})
if not tgt:
    # Legacy family map
    family = fn.split("_")[0]
    legacy = {"sqrt": ("CCM_BENCH_POWER", "ccm_benchmark_sqrt", True),
              "abs": ("CCM_BENCH_BASIC", "ccm_benchmark_abs", True)}
    if family in legacy:
        opt, name, wired = legacy[family]
        print(json.dumps({"cmake_opt": opt, "target": name, "wired": wired,
                          "filter": entry.get("benchmark_filter", "")}))
    else:
        print(json.dumps({"error": "no benchmark target for %s" % fn}))
    sys.exit(0)
fn_profiles = prof.get("function_profiles", {}).get(fn, ["positive_finite_general"])
if profile not in fn_profiles and fn_profiles:
    print(json.dumps({"warn": "profile %s not listed for %s" % (profile, fn)}))
print(json.dumps({
    "cmake_opt": tgt.get("cmake_opt", entry.get("bench", "CCM_BENCH_POWER")),
    "target": tgt.get("target", "ccm_benchmark_%s" % fn.split("_")[0]),
    "wired": tgt.get("wired", False),
    "filter": entry.get("benchmark_filter", ""),
    "profile": profile,
    "profiles": fn_profiles,
}))
PY
}

bench_info="$(resolve_bench)"
if echo "${bench_info}" | python3 -c "import json,sys; d=json.load(sys.stdin); sys.exit(0 if d.get('error') else 1)" 2>/dev/null; then
    echo "${bench_info}" | python3 -c "import json,sys; print(json.load(sys.stdin)['error'])" >&2
    echo "See tools/asmlab/README.md (Benchmarks) for wiring new benchmarks." >&2
    exit 1
fi

bench_opt="$(echo "${bench_info}" | python3 -c "import json,sys; print(json.load(sys.stdin)['cmake_opt'])")"
target="$(echo "${bench_info}" | python3 -c "import json,sys; print(json.load(sys.stdin)['target'])")"
wired="$(echo "${bench_info}" | python3 -c "import json,sys; print('yes' if json.load(sys.stdin).get('wired') else 'no')")"
filter="$(echo "${bench_info}" | python3 -c "import json,sys; print(json.load(sys.stdin).get('filter',''))")"

if [[ "${wired}" != "yes" ]]; then
    echo "benchmark not wired for registry function '${fn}' (target=${target})." >&2
    echo "Pow benchmarks are not yet implemented. Result is advisory only." >&2
    exit 2
fi

if [[ -n "${filter}" ]]; then
    has_filter=0
    for a in "${extra_args[@]:-}"; do
        [[ "${a}" == --benchmark_filter* ]] && has_filter=1
    done
    if [[ "${has_filter}" -eq 0 ]]; then
        extra_args+=("--benchmark_filter=${filter}")
    fi
fi

if [[ "${bench_mode}" == "latency" ]]; then
    extra_args+=("--benchmark_min_time=0.5")
elif [[ "${bench_mode}" == "throughput" ]]; then
    extra_args+=("--benchmark_repetitions=3")
fi

run_bench() {
    echo ">> bench: ${fn} profile=${profile} mode=${bench_mode:-default}" >&2
    echo ">> bench: configuring (${bench_opt}=ON)" >&2
    cmake -S "${project_root}" -B "${build_dir}" -G Ninja \
        -DCMAKE_BUILD_TYPE=Release \
        -DCCMATH_BUILD_BENCHMARKS=ON \
        -DCCMATH_BUILD_TESTS=OFF \
        -DCCMATH_BUILD_EXAMPLES=OFF \
        -D"${bench_opt}"=ON >/dev/null

    echo ">> bench: building ${target}" >&2
    if ! cmake --build "${build_dir}" --target "${target}" >/dev/null 2>&1; then
        echo "could not build ${target}; is it wired into benchmarks/CMakeLists.txt?" >&2
        exit 1
    fi

    local bin
    bin="$(find "${build_dir}" -name "${target}" -type f -perm -111 | head -1)"
    if [[ -z "${bin}" ]]; then
        echo "built but could not locate ${target} binary under ${build_dir}" >&2
        exit 1
    fi

    echo ">> bench: running ${bin}" >&2
    local output
    if [[ ${#extra_args[@]} -gt 0 ]]; then
        output="$("${bin}" "${extra_args[@]}" 2>&1)" || true
    else
        output="$("${bin}" 2>&1)" || true
    fi
    echo "${output}"

    mkdir -p "${benches_dir}"
    python3 - "${benches_dir}/${fn}-${profile}.json" "${fn}" "${profile}" "${bench_mode}" "${output}" <<'PY'
import json, sys, re, datetime
out_path, fn, profile, mode, raw = sys.argv[1:6]
ns_vals = [float(x) for x in re.findall(r"([\d.]+)\s*ns", raw)]
doc = {
    "function": fn,
    "profile": profile,
    "benchmark_mode": mode or "mixed",
    "timestamp": datetime.datetime.now(datetime.timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ"),
    "merge_grade": "advisory",
    "samples": len(ns_vals),
    "result_ns": min(ns_vals) if ns_vals else None,
    "raw_excerpt": raw[-2000:],
}
with open(out_path, "w") as fh:
    json.dump(doc, fh, indent=2)
    fh.write("\n")
print("wrote", out_path)
PY
}

if [[ "${action}" == "diff" ]]; then
    baseline="${benches_dir}/${fn}-${profile}-baseline.json"
    current="${benches_dir}/${fn}-${profile}.json"
    if [[ ! -f "${baseline}" ]]; then
        echo "no baseline at ${baseline}; run bench first and copy to -baseline.json" >&2
        exit 1
    fi
    run_bench >/dev/null
    python3 - "${baseline}" "${current}" <<'PY'
import json, sys
base, cur = json.load(open(sys.argv[1])), json.load(open(sys.argv[2]))
b, c = base.get("result_ns"), cur.get("result_ns")
if b and c:
    delta = c - b
    print("bench diff: %.2f ns -> %.2f ns (%+.2f)" % (b, c, delta))
    sys.exit(0 if delta <= 0 else 1)
print("bench diff: insufficient data")
sys.exit(1)
PY
    exit $?
fi

run_bench
