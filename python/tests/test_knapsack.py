"""Smoke test for a small knapsack instance."""

from __future__ import annotations

import random

import printemps


def test_knapsack_small():
    rng = random.Random(0)
    N = 50
    prices = [rng.randint(0, 99) for _ in range(N)]
    weights = [p + rng.randint(0, 9) for p in prices]
    volumes = [p + rng.randint(0, 9) for p in prices]

    model = printemps.IPModel()
    x = model.create_variables("x", N, 0, 1)

    total_weight = model.create_expression("total_weight", x.dot(weights))
    total_volume = model.create_expression("total_volume", x.dot(volumes))
    total_price = model.create_expression("total_price", x.dot(prices))

    model.create_constraint("total_weight", total_weight <= 200)
    model.create_constraint("total_volume", total_volume <= 200)

    model.maximize(total_price)
    for n in range(N):
        x[n] = 0

    result = printemps.solve(model)

    assert result.solution.is_feasible()
    assert result.solution.objective() > 0
