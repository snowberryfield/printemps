/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/

#include <printemps.h>

/*****************************************************************************/
struct QuadraticAssignmentProblem {
    int                              N;
    std::vector<std::vector<double>> A;
    std::vector<std::vector<double>> B;
};

/*****************************************************************************/
int main(void) {
    /*************************************************************************/
    /// Problem statement
    /*************************************************************************/
    /**
     * This program solves a quadratic assignment problem as an example of
     * optimization that incorporates nonlinear functions and user-defined
     * neighborhoods. The problem is formulated as follows:
     *
     * (P):  minimize  sum_{n=1}^{N} sum_{m=1}^{N} A_{n, m} B_{p_n, p_m}
     *          p
     *      subject to p is permutation of {1,2,...,N}.
     *
     * where N denotes the size of problem, and A and B are real N \times N
     * square matrixes.
     */

    /**
     * This quadratic assignment problem instance is Nug12 from QAPLIB [1]. The
     * Nug12 problem has an optimal solution (12, 7, 9, 3, 4, 8, 11, 1, 5, 6,
     * 10, 2) by permutation with objective function value 578.
     * [1] QAPLIB - A Quadratic Assignment Problem Library :
     * http://anjos.mgi.polymtl.ca/qaplib/
     */
    QuadraticAssignmentProblem qap;
    qap.N = 12;
    qap.A = {{0, 1, 2, 3, 1, 2, 3, 4, 2, 3, 4, 5},
             {1, 0, 1, 2, 2, 1, 2, 3, 3, 2, 3, 4},
             {2, 1, 0, 1, 3, 2, 1, 2, 4, 3, 2, 3},
             {3, 2, 1, 0, 4, 3, 2, 1, 5, 4, 3, 2},
             {1, 2, 3, 4, 0, 1, 2, 3, 1, 2, 3, 4},
             {2, 1, 2, 3, 1, 0, 1, 2, 2, 1, 2, 3},
             {3, 2, 1, 2, 2, 1, 0, 1, 3, 2, 1, 2},
             {4, 3, 2, 1, 3, 2, 1, 0, 4, 3, 2, 1},
             {2, 3, 4, 5, 1, 2, 3, 4, 0, 1, 2, 3},
             {3, 2, 3, 4, 2, 1, 2, 3, 1, 0, 1, 2},
             {4, 3, 2, 3, 3, 2, 1, 2, 2, 1, 0, 1},
             {5, 4, 3, 2, 4, 3, 2, 1, 3, 2, 1, 0}};

    qap.B = {{0, 5, 2, 4, 1, 0, 0, 6, 2, 1, 1, 1},
             {5, 0, 3, 0, 2, 2, 2, 0, 4, 5, 0, 0},
             {2, 3, 0, 0, 0, 0, 0, 5, 5, 2, 2, 2},
             {4, 0, 0, 0, 5, 2, 2, 10, 0, 0, 5, 5},
             {1, 2, 0, 5, 0, 10, 0, 0, 0, 5, 1, 1},
             {0, 2, 0, 2, 10, 0, 5, 1, 1, 5, 4, 0},
             {0, 2, 0, 2, 0, 5, 0, 10, 5, 2, 3, 3},
             {6, 0, 5, 10, 0, 1, 10, 0, 0, 0, 5, 0},
             {2, 4, 5, 0, 0, 1, 5, 0, 0, 0, 10, 10},
             {1, 5, 2, 0, 5, 5, 2, 0, 0, 0, 5, 0},
             {1, 0, 2, 5, 1, 4, 3, 5, 10, 5, 0, 2},
             {1, 0, 2, 5, 1, 0, 3, 0, 10, 0, 2, 0}};

    /*************************************************************************/
    /// Model object definition
    /*************************************************************************/
    printemps::model::IPModel model;

    /*************************************************************************/
    /// Decision variable definitions
    /*************************************************************************/
    auto& p = model.create_variables("p", qap.N, 0, qap.N - 1);

    /*************************************************************************/
    /// Objective function definition
    /*************************************************************************/
    std::function<double(const printemps::neighborhood::IPMove&)> f =
        [&qap, &p](const printemps::neighborhood::IPMove& a_MOVE) {
            double f = 0.0;

            std::vector<int> p_values(qap.N);
            /**
             * The method evaluate(a_MOVE) returns the value of decision (or
             * expression) if the current solution employs a_MOVE for new
             * solution.
             */
            for (auto n = 0; n < qap.N; n++) {
                p_values[n] = p(n).evaluate(a_MOVE);
            }

            for (auto n = 0; n < qap.N; n++) {
                for (auto m = 0; m < qap.N; m++) {
                    f += qap.A[n][m] * qap.B[p_values[n]][p_values[m]];
                }
            }
            return f;
        };
    model.minimize(f);

    /*************************************************************************/
    /// Initial value definitions
    /*************************************************************************/
    for (auto n = 0; n < qap.N; n++) {
        p(n) = n;
    }

    /*************************************************************************/
    /// Neighborhood definitions
    /*************************************************************************/
    /**
     * An user-defined neighborhood is defined by a lambda function which
     * updates candidate moves from the current solution to the next solution.
     * The candidate moves are expressed by std::vector of objects of
     * "printemps:: neighborhood::IPMove". A "printemps:: neighborhood::IPMove"
     * object contains a member "alterations" typed by std::vector of objects of
     * "printemps::model::Alteration". A "printemps::model::Alteration" object
     * is an alias of std::tuple of which the first element is a pointer to a
     * decision variable and the second element is a new value after movement.
     */

    std::function<void(std::vector<printemps::neighborhood::IPMove>*)>
        move_updater =
            [&qap, &p](std::vector<printemps::neighborhood::IPMove>* a_moves) {
                /**
                 * This lambda function defines user-defined moves for "swap"
                 * neighborhood which are obtained by swaps between two or three
                 * elements in a permutation.
                 */
                a_moves->resize(qap.N * (qap.N - 1) / 2 +
                                qap.N * (qap.N - 1) * (qap.N - 2) / 3);
                auto count = 0;

                /// Swap moves between two components
                for (auto n = 0; n < qap.N; n++) {
                    for (auto m = n + 1; m < qap.N; m++) {
                        (*a_moves)[count].alterations.clear();
                        (*a_moves)[count].alterations.emplace_back(
                            &p(n), p(m).value());
                        (*a_moves)[count].alterations.emplace_back(
                            &p(m), p(n).value());
                        count++;
                    }
                }
                /// Swap moves between three components
                for (auto n = 0; n < qap.N; n++) {
                    for (auto m = n + 1; m < qap.N; m++) {
                        for (auto k = m + 1; k < qap.N; k++) {
                            (*a_moves)[count].alterations.clear();
                            (*a_moves)[count].alterations.emplace_back(
                                &p(n), p(m).value());
                            (*a_moves)[count].alterations.emplace_back(
                                &p(m), p(k).value());
                            (*a_moves)[count].alterations.emplace_back(
                                &p(k), p(n).value());
                            count++;

                            (*a_moves)[count].alterations.clear();
                            (*a_moves)[count].alterations.emplace_back(
                                &p(n), p(k).value());
                            (*a_moves)[count].alterations.emplace_back(
                                &p(m), p(n).value());
                            (*a_moves)[count].alterations.emplace_back(
                                &p(k), p(m).value());
                            count++;
                        }
                    }
                }
            };
    model.neighborhood().user_defined().set_move_updater(move_updater);

    /*************************************************************************/
    /// Run solver
    /*************************************************************************/
    printemps::solver::Option option;
    option.is_enabled_binary_move       = false;
    option.is_enabled_integer_move      = false;
    option.is_enabled_user_defined_move = true;
    option.verbose                      = printemps::solver::Full;
    option.improvability_screening_mode = printemps::solver::Off;
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