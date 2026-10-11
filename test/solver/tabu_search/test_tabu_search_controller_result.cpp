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
class TestTabuSearchControllerResult : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown(void) {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestTabuSearchControllerResult, initialize) {
    TabuSearchControllerResult<int, double> result;

    EXPECT_EQ(result.update_status,
              solution::IncumbentHolderConstant::STATUS_NOT_UPDATED);
    EXPECT_EQ(result.state.iteration, 0);
}

/*****************************************************************************/
TEST_F(TestTabuSearchControllerResult, constructor) {
    TabuSearchControllerState<int, double> state;
    state.total_update_status = 3;
    state.iteration           = 15;

    TabuSearchControllerResult<int, double> result(state);

    EXPECT_EQ(result.update_status, 3);
    EXPECT_EQ(result.state.iteration, 15);
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/