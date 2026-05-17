"""Python translation of example/simple_2.cpp.

Solves:
    minimize  f_1 + f_2
    subject to x_1 + x_2 + x_3 >= 2
               y_1 + y_2 = 1
               x_1 + y_1 = 1
               x_i, y_j in {0, 1},
    where f_1 = 2 x_1 + 7 x_2 + 9 x_3 + 1, f_2 = 5 y_1 + 6 y_2.
"""

from __future__ import annotations

import printemps


def main() -> None:
    model = printemps.IPModel()

    x = model.create_variables("x", 3, 0, 1)
    y = model.create_variables("y", 2, 0, 1)
    g = model.create_constraints("g", 3)
    f = model.create_expressions("f", 2)

    c = [2, 7, 9]
    d = [5, 6]

    f[0] = x.dot(c) + 1
    f[1] = y.dot(d)
    g[0] = x.sum() >= 2
    g[1] = y.selection()
    g[2] = x(0) + y(0) == 1

    model.minimize(f.sum())

    result = printemps.solve(model)

    result.solution.print_variable_values()
    result.solution.print_expression_values()
    result.solution.print_constraint_values()
    result.solution.print_violation_values()


if __name__ == "__main__":
    main()
