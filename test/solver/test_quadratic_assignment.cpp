/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <gtest/gtest.h>
#include <random>
#include <printemps.h>

namespace {
/*****************************************************************************/
class TestQuadracitAssignment : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
struct QuadraticAssignmentProblem {
    int                              N;
    std::vector<std::vector<double>> A;
    std::vector<std::vector<double>> B;
};

/*****************************************************************************/
TEST_F(TestQuadracitAssignment, quadratic_assignment) {
    /*************************************************************************/
    /// Problem statement
    /*************************************************************************/
    QuadraticAssignmentProblem qap;
    /// This quadratic assignment problem instance is Nug12 from QAPLIB.
    /// http://anjos.mgi.polymtl.ca/qaplib/
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
    /// Neighborhood definition
    /*************************************************************************/
    std::function<void(std::vector<printemps::neighborhood::IPMove>*)>
        move_updater =
            [&qap, &p](std::vector<printemps::neighborhood::IPMove>* a_moves) {
                a_moves->resize(qap.N * (qap.N - 1) / 2 +
                                qap.N * (qap.N - 1) * (qap.N - 2) / 3);
                auto count = 0;
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

    /// initial solution
    for (auto n = 0; n < qap.N; n++) {
        p(n) = n;
    }
    /// solve
    printemps::option::Option option;

    option.iteration_max                           = 50;
    option.is_enabled_grouping_penalty_coefficient = true;
    option.is_enabled_initial_value_correction     = true;
    option.is_enabled_lagrange_dual                = true;
    option.is_enabled_local_search                 = true;
    option.is_enabled_parallel_evaluation          = true;
    option.is_enabled_parallel_neighborhood_update = true;
    option.is_enabled_binary_move                  = false;
    option.is_enabled_integer_move                 = false;
    option.is_enabled_aggregation_move             = false;
    option.is_enabled_precedence_move              = false;
    option.is_enabled_variable_bound_move          = false;
    option.is_enabled_chain_move                   = true;
    option.is_enabled_user_defined_move            = true;
    option.target_objective_value                  = -1E100;
    option.verbose                         = printemps::option::verbose::None;
    option.tabu_search.iteration_max       = 100;
    option.tabu_search.initial_tabu_tenure = 10;
    option.tabu_search.tabu_mode           = printemps::option::tabu_mode::All;

    option.tabu_search.is_enabled_shuffle                          = true;
    option.tabu_search.is_enabled_move_curtail                     = true;
    option.tabu_search.is_enabled_automatic_break                  = true;
    option.tabu_search.is_enabled_automatic_tabu_tenure_adjustment = true;
    option.tabu_search.move_preserve_rate                          = 0.5;
    option.tabu_search.is_enabled_initial_modification             = true;
    option.tabu_search.ignore_tabu_if_global_incumbent             = true;

    auto result = printemps::solver::solve(&model, option);
    EXPECT_TRUE(result.solution.is_feasible());

    ASSERT_THROW(printemps::solver::solve(&model, option), std::logic_error);
}
/*****************************************************************************/
}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/