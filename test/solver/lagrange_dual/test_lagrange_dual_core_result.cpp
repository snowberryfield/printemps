/*****************************************************************************/
// Copyright (c) 2020-2026 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <gtest/gtest.h>
#include <printemps.h>

namespace {
using namespace printemps;
using namespace printemps::solver::lagrange_dual::core;

/*****************************************************************************/
class TestLagrangeDualCoreResult : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown(void) {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestLagrangeDualCoreResult, initialize) {
    LagrangeDualCoreResult<int, double> result;

    EXPECT_EQ(result.total_update_status, 0);
    EXPECT_EQ(result.number_of_iterations, 0);
    EXPECT_DOUBLE_EQ(result.elapsed_time, 0.0);
    EXPECT_EQ(result.termination_status,
              LagrangeDualCoreTerminationStatus::ITERATION_OVER);
    EXPECT_TRUE(result.dual.empty());
}

/*****************************************************************************/
TEST_F(TestLagrangeDualCoreResult, constructor) {
    LagrangeDualCoreState<int, double> state;
    state.total_update_status = 1;
    state.iteration           = 25;
    state.elapsed_time        = 0.8;
    state.termination_status  = LagrangeDualCoreTerminationStatus::CONVERGE;
    state.lagrangian          = -10.5;

    option::Option                      option;
    LagrangeDualCoreResult<int, double> result(state, option);

    EXPECT_EQ(result.total_update_status, 1);
    EXPECT_EQ(result.number_of_iterations, 25);
    EXPECT_DOUBLE_EQ(result.elapsed_time, 0.8);
    EXPECT_EQ(result.termination_status,
              LagrangeDualCoreTerminationStatus::CONVERGE);
    EXPECT_DOUBLE_EQ(result.lagrangian, -10.5);
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/