#!/usr/bin/env bash
# Download a URL to a local file with curl retries and exponential backoff.
#
# Usage: tools/download_with_retry.sh <url> <output-path>
#
# Skips the download when output-path already exists and is non-empty.
# Environment:
#   DOWNLOAD_MAX_ATTEMPTS (default 8)
#   DOWNLOAD_INITIAL_DELAY_SEC (default 5)

set -euo pipefail

url="${1:?usage: download_with_retry.sh <url> <output-path>}"
output="${2:?usage: download_with_retry.sh <url> <output-path>}"
max_attempts="${DOWNLOAD_MAX_ATTEMPTS:-8}"
initial_delay="${DOWNLOAD_INITIAL_DELAY_SEC:-5}"

if [[ -f "${output}" && -s "${output}" ]]; then
    exit 0
fi

mkdir -p "$(dirname "${output}")"
tmp="${output}.part.$$"
trap 'rm -f "${tmp}"' EXIT

attempt=1
delay="${initial_delay}"
while true; do
    if curl --fail --location --silent --show-error \
        --connect-timeout 30 \
        --max-time 600 \
        --retry 3 \
        --retry-delay 2 \
        --retry-all-errors \
        -o "${tmp}" "${url}"; then
        mv "${tmp}" "${output}"
        exit 0
    fi

    rm -f "${tmp}"
    if (( attempt >= max_attempts )); then
        echo "error: failed to download ${url} after ${max_attempts} attempts" >&2
        exit 1
    fi

    echo "download attempt ${attempt}/${max_attempts} failed, retrying in ${delay}s..." >&2
    sleep "${delay}"
    delay=$(( delay * 2 ))
    if (( delay > 120 )); then delay=120; fi
    attempt=$(( attempt + 1 ))
done
