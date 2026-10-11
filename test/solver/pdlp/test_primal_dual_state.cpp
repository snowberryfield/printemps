/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <gtest/gtest.h>
#include <printemps.h>

namespace {
using namespace printemps;
using namespace printemps::solver::pdlp::core;

/*****************************************************************************/
class TestPrimalDualState : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown(void) {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestPrimalDualState, initialize) {
    PrimalDualState state;

    EXPECT_DOUBLE_EQ(state.objective, 0.0);
    EXPECT_DOUBLE_EQ(state.absolute_violation_norm, 0.0);
    EXPECT_DOUBLE_EQ(state.relative_violation_norm, 0.0);
    EXPECT_DOUBLE_EQ(state.objective_coefficients_norm, 0.0);

    EXPECT_DOUBLE_EQ(state.objective_lower_bound,
                     std::numeric_limits<double>::max());
    EXPECT_DOUBLE_EQ(state.objective_upper_bound,
                     std::numeric_limits<double>::lowest());

    EXPECT_EQ(state.solution.size(), 0U);
    EXPECT_EQ(state.lhs.size(), 0U);
    EXPECT_EQ(state.direction.size(), 0U);
}

/*****************************************************************************/
TEST_F(TestPrimalDualState, constructor) {
    PrimalDualState state(5);

    EXPECT_EQ(state.solution.size(), 5U);
    EXPECT_EQ(state.lhs.size(), 5U);
    EXPECT_EQ(state.direction.size(), 5U);
    EXPECT_EQ(state.solution_trial.size(), 5U);
    EXPECT_EQ(state.move.size(), 5U);
    EXPECT_EQ(state.lagrangian_coefficients.size(), 5U);
    EXPECT_EQ(state.reduced_cost_coefficients.size(), 5U);
    EXPECT_EQ(state.solution_baseline.size(), 5U);
    EXPECT_EQ(state.solution_average.size(), 5U);
    EXPECT_EQ(state.buffer.size(), 5U);
}

/*****************************************************************************/
TEST_F(TestPrimalDualState, setup) {
    /// This method is tested in constructor.
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/