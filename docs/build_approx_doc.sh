#!/usr/bin/env bash
# Build docs/APPROXIMATING_FUNCTIONS.pdf from its LaTeX source.
#
# Run from the repository root:
#   bash docs/build_approx_doc.sh
#
# Options:
#   --regen-data   Re-run Sollya scripts to refresh data files before building.
#   --clean        Remove build artifacts after a successful build.
#   --help         Print this message and exit.

set -euo pipefail

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
DOCS_DIR="$REPO_ROOT/docs"
TEX="$DOCS_DIR/APPROXIMATING_FUNCTIONS.tex"
BUILD_DIR="$REPO_ROOT/out"
SOLLYA_DIR="$REPO_ROOT/tools/proofs/math/approx_doc/sollya"

REGEN_DATA=0
DO_CLEAN=0

for arg in "$@"; do
  case "$arg" in
    --regen-data) REGEN_DATA=1 ;;
    --clean)      DO_CLEAN=1 ;;
    --help)
      sed -n '2,9p' "$0" | sed 's/^# \{0,1\}//'
      exit 0
      ;;
    *)
      echo "Unknown option: $arg" >&2
      exit 1
      ;;
  esac
done

# ---- optional: regenerate Sollya data files ---------------------------
if [[ $REGEN_DATA -eq 1 ]]; then
  if ! command -v sollya &>/dev/null; then
    echo "error: sollya not found; install it or omit --regen-data" >&2
    exit 1
  fi
  echo "==> Regenerating Sollya data files..."
  cd "$REPO_ROOT"
  for script in \
      sin_taylor_degree_error \
      sin_taylor_error \
      sin_small_error \
      log2_core_poly \
      log2_core_points \
      log2_core_table; do
    echo "    sollya $SOLLYA_DIR/${script}.sollya"
    sollya "$SOLLYA_DIR/${script}.sollya" \
      > "$REPO_ROOT/tools/proofs/math/approx_doc/logs/${script}.log" 2>&1
  done
  echo "    done."
fi

# ---- build PDF --------------------------------------------------------
mkdir -p "$BUILD_DIR"

echo "==> Building APPROXIMATING_FUNCTIONS.pdf..."

if command -v latexmk &>/dev/null; then
  latexmk \
    -pdf \
    -bibtex \
    -interaction=nonstopmode \
    -outdir="$BUILD_DIR" \
    "$TEX"
else
  # Manual fallback: pdflatex -> bibtex -> pdflatex x2
  cd "$BUILD_DIR"
  pdflatex -interaction=nonstopmode "$TEX"
  bibtex "$(basename "${TEX%.tex}")"
  pdflatex -interaction=nonstopmode "$TEX"
  pdflatex -interaction=nonstopmode "$TEX"
fi

PDF="$BUILD_DIR/APPROXIMATING_FUNCTIONS.pdf"
if [[ -f "$PDF" ]]; then
  cp "$PDF" "$DOCS_DIR/APPROXIMATING_FUNCTIONS.pdf"
  echo "==> Output: docs/APPROXIMATING_FUNCTIONS.pdf"
else
  echo "error: PDF not produced; check $BUILD_DIR for logs" >&2
  exit 1
fi

# ---- optional cleanup -------------------------------------------------
if [[ $DO_CLEAN -eq 1 ]]; then
  echo "==> Cleaning build artifacts..."
  rm -rf "$BUILD_DIR"
fi
