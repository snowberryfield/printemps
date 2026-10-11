/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <gtest/gtest.h>
#include <printemps.h>

namespace {
using namespace printemps;
using namespace printemps::solver::lagrange_dual::core;

/*****************************************************************************/
class TestLagrangeDualCoreState : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown(void) {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestLagrangeDualCoreState, initialize) {
    LagrangeDualCoreState<int, double> state;

    EXPECT_EQ(state.update_status, 0);
    EXPECT_EQ(state.total_update_status, 0);
    EXPECT_EQ(state.iteration, 0);
    EXPECT_DOUBLE_EQ(state.elapsed_time, 0.0);
    EXPECT_EQ(state.termination_status,
              LagrangeDualCoreTerminationStatus::ITERATION_OVER);

    EXPECT_DOUBLE_EQ(state.lagrangian, 0.0);
    EXPECT_DOUBLE_EQ(state.lagrangian_incumbent, 0.0);
    EXPECT_DOUBLE_EQ(state.step_size, 0.0);
    EXPECT_TRUE(state.dual.empty());
    EXPECT_TRUE(state.dual_incumbent.empty());
    EXPECT_EQ(state.queue.size(), 0);
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/