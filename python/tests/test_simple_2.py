"""Verify simple_2.py reaches a feasible solution."""

from __future__ import annotations

import printemps


def test_simple_2_feasible():
    model = printemps.IPModel()
    x = model.create_variables("x", 3, 0, 1)
    y = model.create_variables("y", 2, 0, 1)
    g = model.create_constraints("g", 3)
    f = model.create_expressions("f", 2)

    f[0] = x.dot([2, 7, 9]) + 1
    f[1] = y.dot([5, 6])
    g[0] = x.sum() >= 2
    g[1] = y.selection()
    g[2] = x(0) + y(0) == 1

    model.minimize(f.sum())

    result = printemps.solve(model)

    assert result.solution.is_feasible()
    obj = result.solution.objective()
    assert obj > 0
