"""Python bindings for printemps, an integer linear programming solver."""

from __future__ import annotations

import sys

from . import _printemps
from ._printemps import (
    Constraint,
    ConstraintProxy,
    DoubleValueProxy,
    Expression,
    ExpressionProxy,
    GeneralOption,
    IntValueProxy,
    IPModel,
    NamedSolution,
    NeighborhoodOption,
    Option,
    OutputOption,
    Range,
    Result,
    SelectionMode,
    Standalone,
    Status,
    TabuMode,
    TabuSearchOption,
    Variable,
    VariableProxy,
    Verbose,
    solve,
)

# Convenience alias mirroring printemps::model_component::Range::All.
ALL = Range.All

__all__ = [
    "ALL",
    "Constraint",
    "ConstraintProxy",
    "DoubleValueProxy",
    "Expression",
    "ExpressionProxy",
    "GeneralOption",
    "IntValueProxy",
    "IPModel",
    "NamedSolution",
    "NeighborhoodOption",
    "Option",
    "OutputOption",
    "Range",
    "Result",
    "SelectionMode",
    "Standalone",
    "Status",
    "TabuMode",
    "TabuSearchOption",
    "Variable",
    "VariableProxy",
    "Verbose",
    "solve",
]


def _main_for_console_script() -> int:
    """Entry point installed as the `printemps` console script."""
    Standalone(sys.argv).run()
    return 0
