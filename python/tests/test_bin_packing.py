"""Smoke test for a small bin-packing instance."""

from __future__ import annotations

import random

import printemps


def test_bin_packing_small():
    rng = random.Random(0)
    number_of_items = 10
    number_of_bins = 5
    bin_capacity = 100

    item_volumes = [rng.randint(0, 49) for _ in range(number_of_items)]

    model = printemps.IPModel()

    x = model.create_variables("x", [number_of_items, number_of_bins], 0, 1)
    y = model.create_variables("y", number_of_bins, 0, 1)

    total_volume = model.create_expressions("total_volume", number_of_bins)
    for m in range(number_of_bins):
        total_volume[m] = x.dot([printemps.ALL, m], item_volumes)

    number_of_used_bins = model.create_expression("number_of_used_bins", y.sum())

    constraint_selection = model.create_constraints("selection", number_of_items)
    for n in range(number_of_items):
        constraint_selection[n] = x.selection([n, printemps.ALL])

    constraint_total_volume = model.create_constraints("total_volume", number_of_bins)
    for m in range(number_of_bins):
        constraint_total_volume[m] = total_volume(m) <= bin_capacity * y(m)

    objective = model.create_expression("objective")
    objective += number_of_used_bins

    model.minimize(objective)

    for n in range(number_of_items):
        x[n, rng.randrange(number_of_bins)] = 1
    for m in range(number_of_bins):
        y[m] = 1

    option = printemps.Option()
    option.neighborhood.selection_mode = printemps.SelectionMode.Defined
    option.tabu_search.tabu_mode = printemps.TabuMode.Any

    result = printemps.solve(model, option)

    assert result.solution.is_feasible()
