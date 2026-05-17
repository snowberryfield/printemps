"""CLI entry point — wraps printemps::standalone::Standalone(argv).run()."""

from __future__ import annotations

import sys

from . import Standalone


def main() -> int:
    Standalone(sys.argv).run()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
