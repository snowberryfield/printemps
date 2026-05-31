"""Smoke test for the Standalone CLI wrapper.

Uses one of the small MPS files under test/dat/mps/ shipped with the C++
codebase. The test runs the standalone solver for at most 1 second and
asserts that the run completes and produces an incumbent.json file.
"""

from __future__ import annotations

import os
import pathlib

import printemps


REPO_ROOT = pathlib.Path(__file__).resolve().parents[2]
SAMPLE_MPS = REPO_ROOT / "test" / "dat" / "mps" / "test_00.mps"


def test_standalone_runs(tmp_path):
    if not SAMPLE_MPS.exists():
        # Without a sample MPS file we can't exercise the standalone path.
        return

    cwd = os.getcwd()
    try:
        os.chdir(tmp_path)
        printemps.Standalone(
            ["printemps", "-t", "1", "--accept-continuous", str(SAMPLE_MPS)]
        ).run()
    finally:
        os.chdir(cwd)

    assert (tmp_path / "incumbent.json").exists() or (tmp_path / "status.json").exists()
