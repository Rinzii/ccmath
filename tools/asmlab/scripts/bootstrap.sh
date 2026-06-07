#!/usr/bin/env bash
# Copyright (c) Ian Pike
# Copyright (c) CCMath contributors
#
# CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
# See LICENSE for more information.
#
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# Provision the asmlab toolchain and cache reference tables. Idempotent; safe to
# re-run. Nothing here needs sudo. What it sets up:
#
#   llvm-mca / llvm-objdump   required   Homebrew LLVM (preferred) or PATH
#   uiCA                      optional   recent-Intel throughput model (git clone)
#   OSACA                     optional   x86/ARM throughput+critical-path (pip --user)
#   Agner Fog tables          reference  instruction_tables.pdf -> refdata/
#   uops.info pointer         reference  refdata/uops-info.url
#
# For the cross-arch / gcc / Linux-accurate path use docker/run.sh instead; this
# script targets the local host loop.

set -uo pipefail

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
asmlab_dir="$(cd "${script_dir}/.." && pwd)"
refdata="${asmlab_dir}/refdata"
mkdir -p "${refdata}"

ok() { printf '  [ok]   %s\n' "$1"; }
warn() { printf '  [warn] %s\n' "$1"; }
info() { printf '== %s\n' "$1"; }

have() { command -v "$1" >/dev/null 2>&1; }

info "LLVM analysis tools (llvm-mca, llvm-objdump)"
mca=""
for cand in /opt/homebrew/opt/llvm/bin/llvm-mca "$(command -v llvm-mca 2>/dev/null)"; do
    if [[ -n "${cand}" && -x "${cand}" ]]; then mca="${cand}"; break; fi
done
if [[ -n "${mca}" ]]; then
    ok "llvm-mca: ${mca} ($("${mca}" --version 2>/dev/null | sed -n '1p'))"
else
    if have brew; then
        warn "llvm-mca missing; installing Homebrew LLVM (this can take a while)..."
        brew install llvm && ok "installed; llvm-mca at $(brew --prefix llvm)/bin/llvm-mca"
    else
        warn "llvm-mca missing and Homebrew not found. Install LLVM, e.g.:"
        warn "  macOS:  brew install llvm"
        warn "  Debian: apt-get install llvm   (provides llvm-mca)"
    fi
fi

info "uiCA (optional; recent-Intel only)"
uica_dir="${refdata}/uiCA"
if [[ -d "${uica_dir}/.git" ]]; then
    ok "uiCA already cloned at ${uica_dir}"
elif have git; then
    if git clone --depth 1 https://github.com/andreas-abel/uiCA.git "${uica_dir}" >/dev/null 2>&1; then
        ok "cloned uiCA -> ${uica_dir} (run its ./setup.sh once to build XED)"
    else
        warn "could not clone uiCA (network?); skipping"
    fi
else
    warn "git not found; skipping uiCA"
fi

info "OSACA (optional; x86 + ARM bounds)"
if have osaca; then
    ok "osaca: $(command -v osaca)"
elif have pip3; then
    if pip3 install --user --quiet osaca >/dev/null 2>&1; then
        ok "installed osaca via pip --user (ensure ~/.local/bin is on PATH)"
    else
        warn "pip install osaca failed; skipping (optional)"
    fi
else
    warn "pip3 not found; skipping OSACA"
fi

info "Reference tables"
agner_pdf="${refdata}/instruction_tables.pdf"
if [[ -s "${agner_pdf}" ]]; then
    ok "Agner Fog tables cached: ${agner_pdf}"
elif have curl; then
    if curl -fsSL https://www.agner.org/optimize/instruction_tables.pdf -o "${agner_pdf}"; then
        ok "downloaded Agner Fog tables -> ${agner_pdf}"
    else
        warn "could not download Agner Fog tables; fetch manually from agner.org/optimize"
    fi
else
    warn "curl not found; fetch instruction_tables.pdf from agner.org/optimize into refdata/"
fi
printf 'https://uops.info/\nhttps://uops.info/xml.html (machine-readable instruction data)\n' \
    > "${refdata}/uops-info.url"
ok "uops.info pointer written: ${refdata}/uops-info.url"

info "Done. See .AI/asm-optimization/toolchain.md for usage."
