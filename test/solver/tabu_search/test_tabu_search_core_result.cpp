/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <gtest/gtest.h>
#include <printemps.h>

namespace {
using namespace printemps;
using namespace printemps::solver::tabu_search::core;

/*****************************************************************************/
class TestTabuSearchCoreResult : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown(void) {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestTabuSearchCoreResult, initialize) {
    TabuSearchCoreResult<int, double> result;

    EXPECT_EQ(result.total_update_status, 0);
    EXPECT_EQ(result.number_of_iterations, 0);
    EXPECT_DOUBLE_EQ(result.elapsed_time, 0.0);
    EXPECT_EQ(result.number_of_updated_moves, 0);
    EXPECT_EQ(result.number_of_evaluated_moves, 0);
    EXPECT_DOUBLE_EQ(result.elapsed_time_for_move_evaluation, 0.0);
    EXPECT_DOUBLE_EQ(result.elapsed_time_for_move_update, 0.0);
    EXPECT_EQ(result.termination_status,
              TabuSearchCoreTerminationStatus::ITERATION_OVER);
    EXPECT_EQ(result.tabu_tenure, 0);
    EXPECT_FALSE(result.is_few_permissible_neighborhood);
    EXPECT_FALSE(result.is_found_new_feasible_solution);
}

/*****************************************************************************/
TEST_F(TestTabuSearchCoreResult, constructor) {
    TabuSearchCoreState<int, double> state;
    state.total_update_status = 1;
    state.iteration           = 50;
    state.elapsed_time        = 2.5;
    state.number_of_moves     = 100;
    state.termination_status  = TabuSearchCoreTerminationStatus::OPTIMAL;
    state.tabu_tenure         = 7;

    option::Option                    option;
    TabuSearchCoreResult<int, double> result(state, option);

    EXPECT_EQ(result.total_update_status, 1);
    EXPECT_EQ(result.number_of_iterations, 50);
    EXPECT_DOUBLE_EQ(result.elapsed_time, 2.5);
    EXPECT_EQ(result.termination_status,
              TabuSearchCoreTerminationStatus::OPTIMAL);
    EXPECT_EQ(result.tabu_tenure, 7);
}

/*****************************************************************************/
TEST_F(TestTabuSearchCoreResult, setup) {
    /// This method is tested in constructor.
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/