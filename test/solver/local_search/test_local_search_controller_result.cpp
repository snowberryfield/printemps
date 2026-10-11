/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <gtest/gtest.h>
#include <printemps.h>

namespace {
using namespace printemps;
using namespace printemps::solver::local_search::core;
using namespace printemps::solver::local_search::controller;

/*****************************************************************************/
class TestLocalSearchControllerResult : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown(void) {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestLocalSearchControllerResult, initialize) {
    LocalSearchControllerResult<int, double> result;

    EXPECT_EQ(result.core.total_update_status, 0);
    EXPECT_EQ(result.core.number_of_iterations, 0);
    EXPECT_DOUBLE_EQ(result.core.elapsed_time, 0.0);
    EXPECT_EQ(result.core.termination_status,
              LocalSearchCoreTerminationStatus::ITERATION_OVER);
}

/*****************************************************************************/
TEST_F(TestLocalSearchControllerResult, constructor) {
    LocalSearchCoreResult<int, double> core_result;
    core_result.total_update_status  = 2;
    core_result.number_of_iterations = 20;
    core_result.elapsed_time         = 1.2;
    core_result.termination_status =
        LocalSearchCoreTerminationStatus::LOCAL_OPTIMAL;

    LocalSearchControllerResult<int, double> result(core_result);

    EXPECT_EQ(result.core.total_update_status, 2);
    EXPECT_EQ(result.core.number_of_iterations, 20);
    EXPECT_DOUBLE_EQ(result.core.elapsed_time, 1.2);
    EXPECT_EQ(result.core.termination_status,
              LocalSearchCoreTerminationStatus::LOCAL_OPTIMAL);
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/