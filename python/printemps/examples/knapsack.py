"""Python translation of example/knapsack.cpp.

Maximizes total price of items packed in a knapsack subject to weight and
volume capacity constraints.
"""

from __future__ import annotations

import random

import printemps


def main(
    number_of_items: int = 10000,
    weight_capacity: int = 30000,
    volume_capacity: int = 30000,
    seed: int = 0,
) -> None:
    rng = random.Random(seed)

    prices = []
    weights = []
    volumes = []
    for _ in range(number_of_items):
        p = rng.randint(0, 99)
        prices.append(p)
        weights.append(p + rng.randint(0, 9))
        volumes.append(p + rng.randint(0, 9))

    model = printemps.IPModel()

    x = model.create_variables("x", number_of_items, 0, 1)

    total_weight = model.create_expression("total_weight", x.dot(weights))
    total_volume = model.create_expression("total_volume", x.dot(volumes))
    total_price = model.create_expression("total_price", x.dot(prices))

    model.create_constraint("total_weight", total_weight <= weight_capacity)
    model.create_constraint("total_volume", total_volume <= volume_capacity)

    model.maximize(total_price)

    for n in range(number_of_items):
        x[n] = 0

    option = printemps.Option()
    option.output.verbose = printemps.Verbose.Full

    result = printemps.solve(model, option)

    result.solution.print_variable_values()
    result.solution.print_expression_values()
    result.solution.print_constraint_values()
    result.solution.print_violation_values()


if __name__ == "__main__":
    main()
