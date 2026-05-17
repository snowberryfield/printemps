"""Verify simple_1.py finds the known optimum.

The optimum of the problem in example/simple_1.cpp is (x_1, x_2) = (7, 70)
with objective 707. (The C++ source's comment claims (7, 10) with 707,
but 7 + 10*10 = 107, and 66*7 + 14*10 = 602 < 1430 violates the first
constraint — that's a typo in the original comment.)
"""

from __future__ import annotations

import printemps


def test_simple_1_optimum():
    model = printemps.IPModel()
    x = model.create_variables("x", 2, -100, 100)
    g = model.create_constraints("g", 2)
    g[0] = 66 * x(0) + 14 * x(1) >= 1430
    g[1] = -82 * x(0) + 28 * x(1) >= 1306
    model.minimize(x(0) + 10 * x(1))

    result = printemps.solve(model)

    assert result.solution.is_feasible()
    assert int(result.solution.objective()) == 707
    assert result.solution.variables("x")(0) == 7
    assert result.solution.variables("x")(1) == 70
