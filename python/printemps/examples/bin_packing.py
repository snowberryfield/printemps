"""Python translation of example/bin_packing.cpp.

Minimizes the number of bins used to pack all items, subject to per-bin
capacity. A small symmetry-breaking term is added to the objective to make
the search direction-sensitive.
"""

from __future__ import annotations

import random

import printemps


def main(
    number_of_items: int = 100,
    number_of_bins: int = 30,
    bin_capacity: int = 100,
    seed: int = 0,
) -> None:
    rng = random.Random(seed)

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

    constraint_cut = model.create_constraints("cut", number_of_bins - 1)
    for m in range(number_of_bins - 1):
        constraint_cut[m] = y(m + 1) <= y(m)

    objective = model.create_expression("objective")
    objective += number_of_used_bins
    for m in range(number_of_bins):
        objective += 1.0e-5 * m * total_volume(m)

    model.minimize(objective)

    for n in range(number_of_items):
        x[n, rng.randrange(number_of_bins)] = 1

    for m in range(number_of_bins):
        y[m] = 1

    option = printemps.Option()
    option.neighborhood.selection_mode = printemps.SelectionMode.Defined
    option.tabu_search.tabu_mode = printemps.TabuMode.Any
    option.output.verbose = printemps.Verbose.Full

    result = printemps.solve(model, option)

    result.solution.print_variable_values()
    result.solution.print_expression_values()
    result.solution.print_constraint_values()
    result.solution.print_violation_values()


if __name__ == "__main__":
    main()
