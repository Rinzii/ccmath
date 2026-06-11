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
BUILD_DIR="$REPO_ROOT/out/docs"
TEX="$DOCS_DIR/APPROXIMATING_FUNCTIONS.tex"
PDF="$DOCS_DIR/APPROXIMATING_FUNCTIONS.pdf"
BUILD_PDF="$BUILD_DIR/APPROXIMATING_FUNCTIONS.pdf"
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
# Figures load the generated data files, which are not checked in, so a
# fresh checkout generates them on the first build.
if [[ ! -f "$REPO_ROOT/tools/proofs/math/approx_doc/data/sin_taylor_error.dat" ]]; then
  echo "==> Sollya data files missing; regenerating."
  REGEN_DATA=1
fi

if [[ $REGEN_DATA -eq 1 ]]; then
  if ! command -v sollya &>/dev/null; then
    echo "error: sollya not found; install it or omit --regen-data" >&2
    exit 1
  fi
  echo "==> Regenerating Sollya data files..."
  cd "$REPO_ROOT"
  mkdir -p "$REPO_ROOT/tools/proofs/math/approx_doc/data" \
           "$REPO_ROOT/tools/proofs/math/approx_doc/logs"
  for script in \
      sin_taylor_degree_error \
      sin_taylor_error \
      sin_small_error \
      sin_minimax_compare \
      sin_small_fpminimax \
      exp_small_fpminimax \
      hard_case_scan \
      log2_core_poly \
      log2_core_points \
      log2_core_table; do
    echo "    sollya $SOLLYA_DIR/${script}.sollya"
    # Sollya 8.0 exits with code 3 after a normal run, so only treat
    # other codes as failures.
    status=0
    sollya "$SOLLYA_DIR/${script}.sollya" \
      > "$REPO_ROOT/tools/proofs/math/approx_doc/logs/${script}.log" 2>&1 \
      || status=$?
    if [[ $status -ne 0 && $status -ne 3 ]]; then
      echo "error: sollya failed (exit $status) for ${script}.sollya" >&2
      exit 1
    fi
  done
  # Sollya reports the same exit code for success and for script errors,
  # so confirm the data files actually appeared.
  for dat in \
      sin_taylor_degree_error \
      sin_taylor_error \
      sin_small_error \
      sin_minimax_error \
      hard_case_scan \
      log2_core_poly_error \
      log2_core_points \
      log2_core_table; do
    if [[ ! -s "$REPO_ROOT/tools/proofs/math/approx_doc/data/${dat}.dat" ]]; then
      echo "error: ${dat}.dat was not generated; check the logs directory" >&2
      exit 1
    fi
  done
  echo "    done."
fi

# ---- build PDF --------------------------------------------------------
mkdir -p "$BUILD_DIR"

echo "==> Building APPROXIMATING_FUNCTIONS.pdf..."

# Figures read generated data files by paths relative to the repo root,
# so the TeX run must start there.  Aux files land in out/docs; the PDF
# is copied next to the .tex source afterward.
cd "$REPO_ROOT"
export BIBINPUTS="$DOCS_DIR:"

if command -v latexmk &>/dev/null; then
  latexmk \
    -pdf \
    -bibtex \
    -interaction=nonstopmode \
    -outdir="$BUILD_DIR" \
    "$TEX"
else
  # Manual fallback: pdflatex -> bibtex -> pdflatex x2
  pdflatex -interaction=nonstopmode -output-directory="$BUILD_DIR" "$TEX"
  (cd "$BUILD_DIR" && bibtex "$(basename "${TEX%.tex}")")
  pdflatex -interaction=nonstopmode -output-directory="$BUILD_DIR" "$TEX"
  pdflatex -interaction=nonstopmode -output-directory="$BUILD_DIR" "$TEX"
fi

if [[ -f "$BUILD_PDF" ]]; then
  cp "$BUILD_PDF" "$PDF"
  echo "==> Output: docs/APPROXIMATING_FUNCTIONS.pdf"
else
  echo "error: PDF not produced; check out/docs for logs" >&2
  exit 1
fi

# ---- optional cleanup -------------------------------------------------
if [[ $DO_CLEAN -eq 1 ]]; then
  echo "==> Cleaning build artifacts..."
  rm -rf "$BUILD_DIR"
fi
