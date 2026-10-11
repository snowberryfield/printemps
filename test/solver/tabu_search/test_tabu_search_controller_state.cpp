/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <gtest/gtest.h>
#include <printemps.h>

namespace {
using namespace printemps;
using namespace printemps::solver::tabu_search::controller;

/*****************************************************************************/
class TestTabuSearchControllerState : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown(void) {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestTabuSearchControllerState, initialize) {
    TabuSearchControllerState<int, double> state;

    EXPECT_DOUBLE_EQ(state.total_elapsed_time, 0.0);
    EXPECT_DOUBLE_EQ(state.tabu_search_elapsed_time, 0.0);
    EXPECT_EQ(state.iteration, 0);
    EXPECT_EQ(state.relaxation_count, 0);
    EXPECT_EQ(state.restart_count, 0);
    EXPECT_EQ(state.total_number_of_inner_iterations, 0);
    EXPECT_EQ(state.total_number_of_evaluated_moves, 0);
    EXPECT_DOUBLE_EQ(state.averaged_inner_iteration_speed, 0.0);
    EXPECT_DOUBLE_EQ(state.averaged_move_evaluation_speed, 0.0);
    EXPECT_DOUBLE_EQ(state.current_primal_intensity, 0.0);
    EXPECT_DOUBLE_EQ(state.current_dual_intensity, 0.0);
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/