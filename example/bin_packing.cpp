/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <printemps.h>

int main(void) {
    /*************************************************************************/
    /// Problem statement
    /*************************************************************************/
    /**
     * This program solves a bin-packing problem which considers minimization of
     * the number of bin to pack all given items. The problem is formulated as a
     * binary linear optimization problem as follows:
     *
     * (P):  maximize   y
     *         x, y
     *      subject to sum_{m=1}^{M} x_{n, m} = 1        (n=1,...,N),
     *                 sum_{n=1}^{N} v_{n} x_{n, m} <= C (m=1,...,M),
     *                 x_{n, m} \in {0, 1}               (n=1,...,N,
     * m=1,...,M}), y_{m} \in {0, 1}                  (m=1,...,M}),
     *
     * where N and M denote the number of items and bins, respectively. The
     * symbol x_{n, m} (n=1,...,N, m=1,...,M) and y_{m} (m=1,...,M) denotes a
     * binary variable of which value takes 1 if and only if the item n is
     * included in bin m. The symbol y_{m} (m=1,...,M) also detnotes a binary
     * variable of which value takes 1 if the bin m is used. The symbol v_{n}
     * (n=1,...,N) denotes the volume of the item n, and the symbol C denotes
     * the capacity of a bin.
     */
    int number_of_items = 100;
    int number_of_bins  = 30;
    int bin_capacity    = 100;

    std::vector<int> item_volumes;

    for (auto n = 0; n < number_of_items; n++) {
        item_volumes.push_back(rand() % 50);
    }

    /*************************************************************************/
    /// Model object definition
    /*************************************************************************/
    printemps::model::IPModel model;

    /*************************************************************************/
    /// Decision variable definitions
    /*************************************************************************/
    auto& x =
        model.create_variables("x", {number_of_items, number_of_bins}, 0, 1);
    auto& y = model.create_variables("y", number_of_bins, 0, 1);

    /*************************************************************************/
    /// Expression definitions
    /*************************************************************************/
    auto& total_volume  //
        = model.create_expressions("total_volume", number_of_bins);

    for (auto m = 0; m < number_of_bins; m++) {
        total_volume(m) =
            x.dot({printemps::model_component::Range::All, m}, item_volumes);
    }

    auto& number_of_used_bins =
        model.create_expression("number_of_used_bins", y.sum());

    /*************************************************************************/
    /// Constraint definitions
    /*************************************************************************/

    /// sum_{m=1}^{M} x_{n, m} = 1 (n=1,...,N),
    auto& constraint_selection =
        model.create_constraints("selection", number_of_items);
    for (auto n = 0; n < number_of_items; n++) {
        constraint_selection(n) =
            x.selection({n, printemps::model_component::Range::All});
    }

    /// sum_{n=1}^{N} v_{n} x_{n, m} <= C (m=1,...,M)
    auto& constraint_total_volume =
        model.create_constraints("total_volume", number_of_bins);
    for (auto m = 0; m < number_of_bins; m++) {
        constraint_total_volume(m) = (total_volume(m) <= bin_capacity * y(m));
    }

    /**
     * This constraints are artificially added to eliminate the symmetry of the
     * optimization problem and to achieve efficient search, and are designed to
     * give preference to the younger numbered bins.
     */
    auto& constraint_cut = model.create_constraints("cut", number_of_bins - 1);
    for (auto m = 0; m < number_of_bins - 1; m++) {
        constraint_cut(m) = (y(m + 1) <= y(m));
    }

    /*************************************************************************/
    /// Objective function definition
    /*************************************************************************/
    auto& objective = model.create_expression("objective");

    objective = number_of_used_bins;

    /**
     * One of difficulties of bin packing problems when metaheuristic algorithm
     * are applied, is that the objective function has little sensitivity
     * for neighborhood operations, because the objective function does not
     * depend on x, assignments of items to bins. For the purpose of introducing
     * the sensitivities of x to the objective function, the objective function
     * is modified to give preference to the younger numbered bins in
     * determination of assigned bins of items. The newly-added term is designed
     * to take a relatively smaller value than that of the original objective
     * function, so that optimal solutions will not be changed in the sense of
     * the original objective function.
     */
    for (int m = 0; m < number_of_bins; m++) {
        objective += 1E-5 * m * total_volume(m);
    }

    model.minimize(objective);

    /*************************************************************************/
    /// Initial value definitions
    /*************************************************************************/
    for (auto n = 0; n < number_of_items; n++) {
        x(n, rand() % number_of_bins) = 1;
    }

    for (auto m = 0; m < number_of_bins; m++) {
        y(m) = 1;
    }

    /*************************************************************************/
    /// Run solver
    /*************************************************************************/
    printemps::option::Option option;
    option.neighborhood.selection_mode =
        printemps::option::selection_mode::Defined;
    option.tabu_search.tabu_mode = printemps::option::tabu_mode::Any;
    option.output.verbose        = printemps::option::verbose::Full;

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