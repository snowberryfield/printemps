/*****************************************************************************/
// Copyright (c) 2020-2026 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <gtest/gtest.h>
#include <printemps.h>

namespace {
using namespace printemps;
using namespace printemps::solver::pdlp::core;
using namespace printemps::solver::pdlp::controller;

/*****************************************************************************/
class TestPDLPControllerResult : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown(void) {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestPDLPControllerResult, initialize) {
    PDLPControllerResult result;

    EXPECT_EQ(result.core.number_of_iterations, 0);
    EXPECT_DOUBLE_EQ(result.core.elapsed_time, 0.0);
    EXPECT_EQ(result.core.termination_status,
              PDLPCoreTerminationStatus::ITERATION_OVER);
}

/*****************************************************************************/
TEST_F(TestPDLPControllerResult, constructor) {
    PDLPCoreResult core_result;
    core_result.number_of_iterations = 45;
    core_result.elapsed_time         = 2.1;
    core_result.termination_status   = PDLPCoreTerminationStatus::OPTIMAL;

    PDLPControllerResult result(core_result);

    EXPECT_EQ(result.core.number_of_iterations, 45);
    EXPECT_DOUBLE_EQ(result.core.elapsed_time, 2.1);
    EXPECT_EQ(result.core.termination_status,
              PDLPCoreTerminationStatus::OPTIMAL);
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/