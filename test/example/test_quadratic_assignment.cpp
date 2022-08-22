/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <gtest/gtest.h>
#include <printemps.h>

namespace {
using namespace printemps;
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
    model::IPModel model;

    /*************************************************************************/
    /// Decision variable definitions
    /*************************************************************************/
    auto& p = model.create_variables("p", qap.N, 0, qap.N - 1);

    /*************************************************************************/
    /// Objective function definition
    /*************************************************************************/
    std::function<double(const neighborhood::IPMove&)> f =
        [&qap, &p](const neighborhood::IPMove& a_MOVE) {
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
    std::function<void(std::vector<neighborhood::IPMove>*)> move_updater =
        [&qap, &p](std::vector<neighborhood::IPMove>* a_moves_ptr) {
            a_moves_ptr->resize(qap.N * (qap.N - 1) / 2 +
                                qap.N * (qap.N - 1) * (qap.N - 2) / 3);
            auto count = 0;
            for (auto n = 0; n < qap.N; n++) {
                for (auto m = n + 1; m < qap.N; m++) {
                    (*a_moves_ptr)[count].alterations.clear();
                    (*a_moves_ptr)[count].alterations.emplace_back(
                        &p(n), p(m).value());
                    (*a_moves_ptr)[count].alterations.emplace_back(
                        &p(m), p(n).value());
                    count++;
                }
            }
            for (auto n = 0; n < qap.N; n++) {
                for (auto m = n + 1; m < qap.N; m++) {
                    for (auto k = m + 1; k < qap.N; k++) {
                        (*a_moves_ptr)[count].alterations.clear();
                        (*a_moves_ptr)[count].alterations.emplace_back(
                            &p(n), p(m).value());
                        (*a_moves_ptr)[count].alterations.emplace_back(
                            &p(m), p(k).value());
                        (*a_moves_ptr)[count].alterations.emplace_back(
                            &p(k), p(n).value());
                        count++;

                        (*a_moves_ptr)[count].alterations.clear();
                        (*a_moves_ptr)[count].alterations.emplace_back(
                            &p(n), p(k).value());
                        (*a_moves_ptr)[count].alterations.emplace_back(
                            &p(m), p(n).value());
                        (*a_moves_ptr)[count].alterations.emplace_back(
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
    option::Option option;
    option.neighborhood.is_enabled_binary_move       = false;
    option.neighborhood.is_enabled_integer_move      = false;
    option.neighborhood.is_enabled_user_defined_move = true;
    option.neighborhood.improvability_screening_mode =
        option::improvability_screening_mode::Off;

    auto result = solver::solve(&model, option);
    EXPECT_TRUE(result.solution.is_feasible());

    ASSERT_THROW(solver::solve(&model, option), std::logic_error);
}
}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/