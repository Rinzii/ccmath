#!/usr/bin/env python3
"""Convert every document in docs/ to PDF and write the results to build/docs/.

The build/ directory is listed in .gitignore, so nothing written here is
checked in.  The output tree mirrors the docs/ source tree:

    docs/proofs/POW_PROOF.tex -> build/docs/proofs/POW_PROOF.pdf
    docs/STATUS.md            -> build/docs/STATUS.pdf

LaTeX files (.tex) are compiled with pdflatex, run twice so that internal
cross-references and the table of contents resolve correctly.

Markdown files (.md) are converted with pandoc.

All other file types are skipped.

Requirements: pdflatex and pandoc must be in PATH.

Usage (from the repository root):
    python3 scripts/generate_docs_pdf.py
"""

from __future__ import annotations

import shutil
import subprocess
import sys
import tempfile
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
SRC  = ROOT / "docs"
OUT  = ROOT / "build" / "docs"


def _check_tools() -> None:
    missing = [t for t in ("pdflatex", "pandoc") if shutil.which(t) is None]
    if missing:
        print(f"error: required tools not found in PATH: {', '.join(missing)}", file=sys.stderr)
        sys.exit(1)


def _rel(path: Path) -> str:
    return str(path.relative_to(ROOT))


def _compile_latex(src: Path, dst: Path) -> None:
    """Compile a .tex file with two pdflatex passes so cross-references resolve."""
    dst.parent.mkdir(parents=True, exist_ok=True)
    with tempfile.TemporaryDirectory() as tmp:
        tmp_path = Path(tmp)
        shutil.copy(src, tmp_path / src.name)

        for pass_num in (1, 2):
            result = subprocess.run(
                ["pdflatex", "-interaction=nonstopmode", src.name],
                cwd=tmp_path,
                capture_output=True,
                text=True,
            )
            if result.returncode != 0:
                print(f"  FAIL  {_rel(src)} (pass {pass_num})", file=sys.stderr)
                # Show only lines starting with ! for brevity
                errors = [l for l in result.stdout.splitlines() if l.startswith("!")]
                print("\n".join(errors[:10]), file=sys.stderr)
                return

        shutil.copy(tmp_path / src.with_suffix(".pdf").name, dst)
    print(f"  OK    {_rel(dst)}")


def _convert_markdown(src: Path, dst: Path) -> None:
    """Convert a Markdown file to PDF via pandoc."""
    dst.parent.mkdir(parents=True, exist_ok=True)
    result = subprocess.run(
        [
            "pandoc",
            str(src),
            "--output", str(dst),
            "--pdf-engine=pdflatex",
            "--resource-path", str(src.parent),
            "--variable", "geometry:margin=1in",
            "--variable", "fontsize=11pt",
            "--variable", "linestretch=1.15",
        ],
        capture_output=True,
        text=True,
    )
    if result.returncode != 0:
        print(f"  FAIL  {_rel(src)}", file=sys.stderr)
        print(result.stderr.strip(), file=sys.stderr)
    else:
        print(f"  OK    {_rel(dst)}")


def main() -> None:
    _check_tools()
    OUT.mkdir(parents=True, exist_ok=True)

    sources = sorted(SRC.rglob("*"))
    processed = 0

    for src in sources:
        if not src.is_file():
            continue

        relative = src.relative_to(SRC)
        dst = OUT / relative.with_suffix(".pdf")

        if src.suffix == ".tex":
            _compile_latex(src, dst)
            processed += 1
        elif src.suffix == ".md":
            _convert_markdown(src, dst)
            processed += 1

    if processed == 0:
        print("no .tex or .md files found in docs/")
    else:
        print(f"\n{processed} file(s) written to {_rel(OUT)}/")


if __name__ == "__main__":
    main()
