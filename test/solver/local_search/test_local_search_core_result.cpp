/*****************************************************************************/
// Copyright (c) 2020-2026 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <gtest/gtest.h>
#include <printemps.h>

namespace {
using namespace printemps;
using namespace printemps::solver::local_search::core;

/*****************************************************************************/
class TestLocalSearchCoreResult : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown(void) {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestLocalSearchCoreResult, initialize) {
    LocalSearchCoreResult<int, double> result;

    EXPECT_EQ(result.total_update_status, 0);
    EXPECT_EQ(result.number_of_iterations, 0);
    EXPECT_DOUBLE_EQ(result.elapsed_time, 0.0);
    EXPECT_EQ(result.termination_status,
              LocalSearchCoreTerminationStatus::ITERATION_OVER);
}

/*****************************************************************************/
TEST_F(TestLocalSearchCoreResult, constructor) {
    LocalSearchCoreState<int, double> state;
    state.total_update_status = 1;
    state.iteration           = 10;
    state.elapsed_time        = 0.5;
    state.termination_status  = LocalSearchCoreTerminationStatus::LOCAL_OPTIMAL;

    option::Option                     option;
    LocalSearchCoreResult<int, double> result(state, option);

    EXPECT_EQ(result.total_update_status, 1);
    EXPECT_EQ(result.number_of_iterations, 10);
    EXPECT_DOUBLE_EQ(result.elapsed_time, 0.5);
    EXPECT_EQ(result.termination_status,
              LocalSearchCoreTerminationStatus::LOCAL_OPTIMAL);
}

}  // namespace
