#!/usr/bin/env bash
# Install clang-format 20 from Ubuntu packages for CI format checks.

set -euo pipefail

sudo apt-get update -qq
sudo apt-get install -yqq --no-install-recommends clang-format-20
sudo ln -sf "$(command -v clang-format-20)" /usr/local/bin/clang-format
clang-format --version
