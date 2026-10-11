/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <gtest/gtest.h>
#include <printemps.h>

namespace {
using namespace printemps;
using namespace printemps::solver::cdcl::core;
using namespace printemps::solver::cdcl::controller;

/*****************************************************************************/
class TestCDCLControllerResult : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown(void) {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestCDCLControllerResult, initialize) {
    CDCLControllerResult<int, double> result;

    EXPECT_EQ(result.core.termination_status,
              CDCLCoreTerminationStatus::NOT_SOLVED);
    EXPECT_TRUE(result.core.solution.empty());
}

/*****************************************************************************/
TEST_F(TestCDCLControllerResult, constructor) {
    CDCLCoreResult core_result;
    core_result.termination_status = CDCLCoreTerminationStatus::FEASIBLE;
    core_result.solution           = {1, 0};

    CDCLControllerResult<int, double> result(core_result);

    EXPECT_EQ(result.core.termination_status,
              CDCLCoreTerminationStatus::FEASIBLE);
    EXPECT_EQ(result.core.solution.size(), 2U);
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/