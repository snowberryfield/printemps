/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <gtest/gtest.h>
#include <printemps.h>

namespace {
using namespace printemps;
using namespace printemps::solver;

/*****************************************************************************/
class TestGlobalState : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown(void) {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestGlobalState, initialize) {
    GlobalState<int, double> state;

    EXPECT_EQ(state.model_ptr, nullptr);
    EXPECT_EQ(state.solver_ptr, nullptr);
    EXPECT_EQ(state.termination_status, TerminationStatus::UNKNOWN);
    EXPECT_FALSE(state.is_proven_infeasible());
}

/*****************************************************************************/
TEST_F(TestGlobalState, is_proven_infeasible) {
    GlobalState<int, double> state;

    state.termination_status = TerminationStatus::INFEASIBLE;
    EXPECT_TRUE(state.is_proven_infeasible());

    state.termination_status = TerminationStatus::OPTIMAL;
    EXPECT_FALSE(state.is_proven_infeasible());

    state.termination_status = TerminationStatus::FEASIBLE;
    EXPECT_FALSE(state.is_proven_infeasible());
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/