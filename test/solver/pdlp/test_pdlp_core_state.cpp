/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <gtest/gtest.h>
#include <printemps.h>

namespace {
using namespace printemps;
using namespace printemps::solver::pdlp::core;

/*****************************************************************************/
class TestPDLPCoreState : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown(void) {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestPDLPCoreState, initialize) {
    PDLPCoreState state;

    EXPECT_EQ(state.inner_iteration, 0);
    EXPECT_EQ(state.outer_iteration, 0);
    EXPECT_EQ(state.total_iteration, 0);
    EXPECT_EQ(state.number_of_solution_update_attempt, 0);
    EXPECT_DOUBLE_EQ(state.elapsed_time, 0.0);
    EXPECT_DOUBLE_EQ(state.absolute_gap, 0.0);
    EXPECT_DOUBLE_EQ(state.relative_gap, 0.0);
    EXPECT_DOUBLE_EQ(state.step_size_current, 0.0);
    EXPECT_DOUBLE_EQ(state.step_size_previous, 0.0);
    EXPECT_DOUBLE_EQ(state.step_size_cumulative_sum, 0.0);
    EXPECT_DOUBLE_EQ(state.primal_weight, 0.0);
    EXPECT_FALSE(state.is_enabled_restart);
    EXPECT_EQ(state.restart_mode, RestartMode::Current);
    EXPECT_EQ(state.termination_status,
              PDLPCoreTerminationStatus::ITERATION_OVER);
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/