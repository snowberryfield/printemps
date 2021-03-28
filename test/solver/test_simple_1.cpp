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
class TestSimple1 : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestSimple1, simple_1) {
    /**
     * This program solves the following simple liner integer optimization
     * problem [1]:
     * (P):  minimize       x_1 + 10 x_2
     *          x
     *      subject to   66 x_1 + 14 x_2 >= 1430,
     *                  -82 x_1 + 28 x_2 >= 1306,
     *                      x_1 and x_2 are integer.
     *
     * The problem (P) has an optimal solution (x_1, x_2) = (7, 10) with
     * objective function value 707.
     *
     * [1] R.Fletcher: Practical Methods of Optimization, Second Edition,
     * John Wiley & Sons (2000).
     */
    printemps::model::IPModel model;

    auto& x = model.create_variables("x", 2, -100, 100);
    auto& g = model.create_constraints("g", 2);

    g(0) = 66 * x(0) + 14 * x(1) >= 1430;
    g(1) = -82 * x(0) + 28 * x(1) >= 1306;
    model.minimize(x(0) + 10 * x(1));

    /// initial solution(consider presolving)
    x(0) = 18;
    x(1) = 50;

    /// solve
    printemps::solver::Option option;

    option.iteration_max                           = 50;
    option.is_enabled_grouping_penalty_coefficient = true;
    option.is_enabled_initial_value_correction     = true;
    option.is_enabled_lagrange_dual                = true;
    option.is_enabled_local_search                 = true;
    option.is_enabled_parallel_evaluation          = true;
    option.is_enabled_parallel_neighborhood_update = true;
    option.is_enabled_binary_move                  = true;
    option.is_enabled_integer_move                 = true;
    option.is_enabled_aggregation_move             = true;
    option.is_enabled_precedence_move              = true;
    option.is_enabled_variable_bound_move          = true;
    option.is_enabled_chain_move                   = true;
    option.is_enabled_user_defined_move            = true;
    option.target_objective_value                  = -1E100;
    option.verbose                                 = printemps::solver::None;
    option.tabu_search.iteration_max               = 100;
    option.tabu_search.initial_tabu_tenure         = 10;
    option.tabu_search.tabu_mode =
        printemps::solver::tabu_search::TabuMode::All;

    option.tabu_search.is_enabled_shuffle                          = true;
    option.tabu_search.is_enabled_move_curtail                     = true;
    option.tabu_search.is_enabled_automatic_break                  = true;
    option.tabu_search.is_enabled_automatic_tabu_tenure_adjustment = true;
    option.tabu_search.move_preserve_rate                          = 0.5;
    option.tabu_search.is_enabled_initial_modification             = true;
    option.tabu_search.ignore_tabu_if_global_incumbent             = true;

    auto result = printemps::solver::solve(&model, option);
    EXPECT_TRUE(result.solution.is_feasible());
    EXPECT_EQ(7, result.solution.variables("x").values(0));
    EXPECT_EQ(70, result.solution.variables("x").values(1));

    ASSERT_THROW(printemps::solver::solve(&model, option), std::logic_error);
}
/*****************************************************************************/
}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/