"""Sudoku smoke test on a relatively easy puzzle."""

from __future__ import annotations

import printemps
from printemps.examples.sudoku import build_model


# An easier instance than example/sudoku.cpp's puzzle, to fit a short test run.
EASY = [
    [5, 3, 0, 0, 7, 0, 0, 0, 0],
    [6, 0, 0, 1, 9, 5, 0, 0, 0],
    [0, 9, 8, 0, 0, 0, 0, 6, 0],
    [8, 0, 0, 0, 6, 0, 0, 0, 3],
    [4, 0, 0, 8, 0, 3, 0, 0, 1],
    [7, 0, 0, 0, 2, 0, 0, 0, 6],
    [0, 6, 0, 0, 0, 0, 2, 8, 0],
    [0, 0, 0, 4, 1, 9, 0, 0, 5],
    [0, 0, 0, 0, 8, 0, 0, 7, 9],
]


def _validate(grid):
    N = len(grid)
    rows_ok = all(set(row) == set(range(1, N + 1)) for row in grid)
    cols_ok = all(
        set(grid[r][c] for r in range(N)) == set(range(1, N + 1)) for c in range(N)
    )
    blocks_ok = all(
        set(grid[3 * bi + dr][3 * bj + dc] for dr in range(3) for dc in range(3))
        == set(range(1, N + 1))
        for bi in range(3)
        for bj in range(3)
    )
    return rows_ok and cols_ok and blocks_ok


def test_sudoku_easy():
    model, N = build_model(EASY)

    option = printemps.Option()
    option.tabu_search.initial_tabu_tenure = 5
    option.general.iteration_max = 1000

    result = printemps.solve(model, option)

    assert result.solution.is_feasible()

    var_x = result.solution.variables("x")
    answer = [[0] * N for _ in range(N)]
    for n in range(N):
        for m in range(N):
            for k in range(N):
                answer[n][m] += (k + 1) * var_x(n, m, k)

    assert _validate(answer)
