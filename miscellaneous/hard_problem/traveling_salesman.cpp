/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/

#include <fstream>

#include <cppmh.h>

int main(void) {
    /*************************************************************************/
    /// Problem statement
    /*************************************************************************/
    int number_of_cities = 50;
    int x_max            = 100;
    int y_max            = 100;

    std::vector<int> x_positions(number_of_cities);
    std::vector<int> y_positions(number_of_cities);
    for (auto n = 0; n < number_of_cities; n++) {
        x_positions[n] = rand() % x_max;
        y_positions[n] = rand() % y_max;
    }

    std::vector<std::vector<double>> distances(number_of_cities);
    for (auto n = 0; n < number_of_cities; n++) {
        distances[n].resize(number_of_cities);
        for (auto m = 0; m < number_of_cities; m++) {
            distances[n][m] = sqrt(pow(x_positions[n] - x_positions[m], 2.0) +
                                   pow(y_positions[n] - y_positions[m], 2.0));
        }
    }
    /*************************************************************************/
    /// Model object definition
    /*************************************************************************/
    cppmh::model::IPModel model;

    /*************************************************************************/
    /// Decision variable definitions
    /*************************************************************************/
    auto& z =
        model.create_variables("z", {number_of_cities, number_of_cities}, 0, 1);
    auto& selection_row =
        model.create_constraints("selection_row", number_of_cities);
    auto& selection_column =
        model.create_constraints("selection_column", number_of_cities);

    /*************************************************************************/
    /// Expression definitions
    /*************************************************************************/
    auto& city_to_be_visited =
        model.create_expressions("city_to_be_visited", number_of_cities);

    for (auto n = 0; n < number_of_cities; n++) {
        city_to_be_visited(n) = z.dot(
            {cppmh::model::All, n}, cppmh::utility::sequence(number_of_cities));
    }

    /*************************************************************************/
    /// Constraint definitions
    /*************************************************************************/
    for (auto n = 0; n < number_of_cities; n++) {
        selection_column(n) = z.selection({n, cppmh::model::All});
    }

    for (auto n = 0; n < number_of_cities; n++) {
        selection_row(n) = z.selection({cppmh::model::All, n});
    }

    /*************************************************************************/
    /// Objective function definition
    /*************************************************************************/
    std::function<double(const cppmh::model::IPMove&)> total_distance =
        [&distances, &z, &city_to_be_visited,
         number_of_cities](const cppmh::model::IPMove& a_MOVE) {
            double total_distance = 0.0;

            for (auto n = 0; n < number_of_cities; n++) {
                auto current = city_to_be_visited(n).evaluate(a_MOVE);
                auto next    = city_to_be_visited((n + 1) % number_of_cities)
                                .evaluate(a_MOVE);
                total_distance += distances[current][next];
            }
            return total_distance;
        };
    model.minimize(total_distance);

    /*************************************************************************/
    /// Initial value definitions
    /*************************************************************************/
    for (auto n = 0; n < number_of_cities; n++) {
        z(n, n) = 1;
    }

    /*************************************************************************/
    /// Run solver
    /*************************************************************************/
    cppmh::solver::Option option;
    option.verbose               = cppmh::solver::Full;
    option.tabu_search.tabu_mode = cppmh::solver::tabu_search::Any;
    auto result                  = cppmh::solver::solve(&model, option);

    /*************************************************************************/
    /// Print result
    /*************************************************************************/
    result.print_variable_values();
    result.print_expression_values();
    result.print_constraint_values();

#if 1
    /**
     * The following code output the tour obtained by the optimization to a
     * file. The tour can be plotted by gnuplot with the command:
     *   >plot "path.txt" with lines
     */
    std::ofstream ofs("tour.txt");

    for (auto n = 0; n < number_of_cities; n++) {
        auto current = result.expressions("city_to_be_visited").values(n);
        auto next    = result.expressions("city_to_be_visited")
                        .values((n + 1) % number_of_cities);

        ofs << x_positions[current] << " " << y_positions[next] << std::endl;
    }
    ofs.close();
#endif

    return 0;
}
/*****************************************************************************/
// END
/*****************************************************************************/