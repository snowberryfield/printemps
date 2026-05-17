"""Python translation of example/simple_1.cpp.

Solves:
    minimize    x_1 + 10 x_2
    subject to  66 x_1 + 14 x_2 >= 1430
               -82 x_1 + 28 x_2 >= 1306
                x_1, x_2 integer.
Optimal solution: (x_1, x_2) = (7, 10), objective = 707.
"""

from __future__ import annotations

import printemps


def main() -> None:
    model = printemps.IPModel()

    x = model.create_variables("x", 2, -100, 100)
    g = model.create_constraints("g", 2)

    g[0] = 66 * x(0) + 14 * x(1) >= 1430
    g[1] = -82 * x(0) + 28 * x(1) >= 1306
    model.minimize(x(0) + 10 * x(1))

    result = printemps.solve(model)

    print(f"objective = {result.solution.objective()}")
    print(f"x(0) = {result.solution.variables('x')(0)}")
    print(f"x(1) = {result.solution.variables('x')(1)}")


if __name__ == "__main__":
    main()
