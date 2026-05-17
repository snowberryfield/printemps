#!/usr/bin/env python3
"""Copy the printemps C++ header tree into python/_vendor/printemps/.

This is the release-time step that lets the source distribution be
self-contained: after running this script, `python -m build --sdist`
produces a tarball that bundles the printemps headers, and the resulting
sdist can be `pip install`-ed without the surrounding repository.

Usage:
    python tools/vendor.py            # populate _vendor/printemps/
    python tools/vendor.py --clean    # remove _vendor/ first
"""

from __future__ import annotations

import argparse
import shutil
import sys
from pathlib import Path


PYTHON_DIR = Path(__file__).resolve().parents[1]  # repo/python/
SRC = PYTHON_DIR.parent / "printemps"  # repo/printemps/
DST = PYTHON_DIR / "_vendor" / "printemps"  # repo/python/_vendor/printemps/


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--clean", action="store_true", help="Remove _vendor/ before copying."
    )
    args = parser.parse_args()

    if not (SRC / "printemps.h").exists():
        print(f"ERROR: {SRC / 'printemps.h'} not found.", file=sys.stderr)
        print(
            "This script must run inside a checkout of the printemps repo "
            "with the C++ headers available at ../printemps/.",
            file=sys.stderr,
        )
        return 1

    if DST.exists():
        if not args.clean:
            print(f"Removing existing {DST}")
        shutil.rmtree(DST)

    DST.parent.mkdir(parents=True, exist_ok=True)
    shutil.copytree(SRC, DST)
    n_files = sum(1 for _ in DST.rglob("*") if _.is_file())
    print(f"Vendored {n_files} files: {SRC} -> {DST}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
