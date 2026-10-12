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
using namespace printemps::solver::lagrange_dual::controller;

/*****************************************************************************/
class TestLagrangeDualControllerResult : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown(void) {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestLagrangeDualControllerResult, initialize) {
    LagrangeDualControllerResult<int, double> result;

    EXPECT_EQ(result.core.total_update_status, 0);
    EXPECT_EQ(result.core.number_of_iterations, 0);
    EXPECT_DOUBLE_EQ(result.core.elapsed_time, 0.0);
    EXPECT_EQ(result.core.termination_status,
              LagrangeDualCoreTerminationStatus::ITERATION_OVER);
}

/*****************************************************************************/
TEST_F(TestLagrangeDualControllerResult, constructor) {
    LagrangeDualCoreResult<int, double> core_result;
    core_result.total_update_status  = 2;
    core_result.number_of_iterations = 30;
    core_result.elapsed_time         = 1.5;
    core_result.termination_status =
        LagrangeDualCoreTerminationStatus::CONVERGE;

    LagrangeDualControllerResult<int, double> result(core_result);

    EXPECT_EQ(result.core.total_update_status, 2);
    EXPECT_EQ(result.core.number_of_iterations, 30);
    EXPECT_DOUBLE_EQ(result.core.elapsed_time, 1.5);
    EXPECT_EQ(result.core.termination_status,
              LagrangeDualCoreTerminationStatus::CONVERGE);
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/