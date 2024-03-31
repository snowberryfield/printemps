/*****************************************************************************/
// Copyright (c) 2020-2024 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <printemps.h>

int main(void) {
    /*************************************************************************/
    /// Problem statement
    /*************************************************************************/
    /**
     * This program solves a knapsack problem which considers maximization of
     * total prices of items to be included in a "knapsack" with satisfying two
     * constraints, volume and weight capacities. The problem is formulated as a
     * binary linear optimization problem as follows:
     *
     * (P):  maximize  sum_{n=1}^{N} p_{n} x_{n}
     *          x
     *      subject to sum_{n=1}^{N} w_{n} x_{n} <= C_{w},
     *                 sum_{n=1}^{N} v_{n} x_{n} <= C_{v},
     *                 x_{n} \in {0, 1} (n=1,...,N),
     *
     * where N denotes the number of items. The symvol x_{n} (n=1,...,N) denotes
     * the binary decision variable which takes 1 if and only if the item n is
     * included in the knapsack. The symbols p_{n}, w_{n}, and v_{n} (n=1,...,N)
     * respectively denote the price, the weight, and the volume of the item n.
     * The symbols C_{w} and C_{v} respectively denote the weight and volume
     * capacities of the knapsack.
     */
    int number_of_items = 10000;
    int weight_capacity = 30000;
    int volume_capacity = 30000;

    std::vector<int> weights;
    std::vector<int> volumes;
    std::vector<int> prices;

    for (auto n = 0; n < number_of_items; n++) {
        prices.push_back(rand() % 100);
        weights.push_back(prices.back() + rand() % 10);
        volumes.push_back(prices.back() + rand() % 10);
    }

    /*************************************************************************/
    /// Model object definition
    /*************************************************************************/
    printemps::model::IPModel model;

    /*************************************************************************/
    /// Decision variable definitions
    /*************************************************************************/
    auto& x = model.create_variables("x", number_of_items, 0, 1);

    /*************************************************************************/
    /// Expression definitions
    /*************************************************************************/
    auto& total_weight =
        model.create_expression("total_weight", x.dot(weights));

    auto& total_volume =
        model.create_expression("total_volume", x.dot(volumes));

    auto& total_price =  //
        model.create_expression("total_price", x.dot(prices));

    /*************************************************************************/
    /// Constraint definitions
    /*************************************************************************/
    /// sum_{n=1}^{N} w_{n} x_{n} <= C_{w}
    model.create_constraint("total_weight", total_weight <= weight_capacity);

    /// sum_{n=1}^{N} v_{n} x_{n} <= C_{v}
    model.create_constraint("total_volume", total_volume <= volume_capacity);

    /*************************************************************************/
    /// Objective function definition
    /*************************************************************************/
    model.maximize(total_price);

    /*************************************************************************/
    /// Initial value definitions
    /*************************************************************************/
    for (auto n = 0; n < number_of_items; n++) {
        x(n) = 0;
    }

    /*************************************************************************/
    /// Run solver
    /*************************************************************************/
    printemps::option::Option option;
    option.output.verbose = printemps::option::verbose::Full;

    auto result = printemps::solver::solve(&model, option);

    /*************************************************************************/
    /// Print result
    /*************************************************************************/
    result.solution.print_variable_values();
    result.solution.print_expression_values();
    result.solution.print_constraint_values();
    result.solution.print_violation_values();

    return 0;
}
/*****************************************************************************/
// END
/*****************************************************************************/