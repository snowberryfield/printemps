"""Python translation of example/sudoku.cpp.

Solves a Sudoku puzzle as a 0/1 integer programming feasibility problem.
"""

from __future__ import annotations

import printemps


# "World's hardest Sudoku" by Arto Inkala (matches example/sudoku.cpp).
INITIAL = [
    [0, 0, 5, 3, 0, 0, 0, 0, 0],
    [8, 0, 0, 0, 0, 0, 0, 2, 0],
    [0, 7, 0, 0, 1, 0, 5, 0, 0],
    [4, 0, 0, 0, 0, 5, 3, 0, 0],
    [0, 1, 0, 0, 7, 0, 0, 0, 6],
    [0, 0, 3, 2, 0, 0, 0, 8, 0],
    [0, 6, 0, 5, 0, 0, 0, 0, 9],
    [0, 0, 4, 0, 0, 0, 0, 3, 0],
    [0, 0, 0, 0, 0, 9, 7, 0, 0],
]


def build_model(initial=INITIAL):
    N = len(initial)
    model = printemps.IPModel()

    x = model.create_variables("x", [N, N, N], 0, 1)

    expression_block = model.create_expressions("expression_block", [N // 3, N // 3, N])
    for i in range(N // 3):
        for j in range(N // 3):
            for k in range(N):
                for n in range(i * N // 3, (i + 1) * N // 3):
                    for m in range(j * N // 3, (j + 1) * N // 3):
                        expression_block[i, j, k] += x(n, m, k)
    expression_block.disable()

    constraint_number = model.create_constraints("constraint_number", [N, N])
    for n in range(N):
        for m in range(N):
            constraint_number[n, m] = x.selection([n, m, printemps.ALL])

    constraint_row = model.create_constraints("constraint_row", [N, N])
    for m in range(N):
        for k in range(N):
            constraint_row[m, k] = x.selection([printemps.ALL, m, k])

    constraint_column = model.create_constraints("constraint_column", [N, N])
    for n in range(N):
        for k in range(N):
            constraint_column[n, k] = x.selection([n, printemps.ALL, k])

    constraint_block = model.create_constraints("constraint_block", [N // 3, N // 3, N])
    for i in range(N // 3):
        for j in range(N // 3):
            for k in range(N):
                constraint_block[i, j, k] = expression_block(i, j, k) == 1

    for n in range(N):
        for m in range(N):
            if initial[n][m] > 0:
                x(n, m, initial[n][m] - 1).fix_by(1)

    return model, N


def main() -> None:
    model, N = build_model()

    option = printemps.Option()
    option.output.verbose = printemps.Verbose.Full
    option.tabu_search.initial_tabu_tenure = 5
    option.general.iteration_max = 1000

    result = printemps.solve(model, option)

    answer = [[0] * N for _ in range(N)]
    var_x = result.solution.variables("x")
    for n in range(N):
        for m in range(N):
            for k in range(N):
                answer[n][m] += (k + 1) * var_x(n, m, k)

    for n in range(N):
        print(" ".join(str(v) for v in answer[n]))


if __name__ == "__main__":
    main()
