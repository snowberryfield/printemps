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

/*****************************************************************************/
class TestLocalSearchCoreState : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown(void) {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestLocalSearchCoreState, initialize) {
    LocalSearchCoreState<int, double> state;

    EXPECT_EQ(state.update_status, 0);
    EXPECT_EQ(state.total_update_status, 0);
    EXPECT_EQ(state.iteration, 0);
    EXPECT_DOUBLE_EQ(state.elapsed_time, 0.0);
    EXPECT_EQ(state.termination_status,
              LocalSearchCoreTerminationStatus::ITERATION_OVER);

    EXPECT_EQ(state.number_of_moves, 0);
    EXPECT_EQ(state.number_of_performed_moves, 0);
    EXPECT_FALSE(state.is_found_improving_solution);
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/