/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <gtest/gtest.h>
#include <printemps.h>

namespace {
/*****************************************************************************/
class TestDenseSolution : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestDenseSolution, constructor) {
    printemps::solution::DenseSolution<int, double> solution;

    EXPECT_TRUE(solution.variable_value_proxies.empty());
    EXPECT_TRUE(solution.expression_value_proxies.empty());
    EXPECT_TRUE(solution.constraint_value_proxies.empty());
    EXPECT_TRUE(solution.violation_value_proxies.empty());

    EXPECT_FLOAT_EQ(0.0, solution.objective);
    EXPECT_FLOAT_EQ(0.0, solution.total_violation);
    EXPECT_FALSE(solution.is_feasible);
}

/*****************************************************************************/
TEST_F(TestDenseSolution, initialize) {
    printemps::solution::DenseSolution<int, double> solution;
    solution.variable_value_proxies.resize(10);
    solution.expression_value_proxies.resize(10);
    solution.constraint_value_proxies.resize(10);
    solution.violation_value_proxies.resize(10);

    solution.objective       = 100.0;
    solution.total_violation = 1000.0;
    solution.is_feasible     = true;

    solution.initialize();

    EXPECT_TRUE(solution.variable_value_proxies.empty());
    EXPECT_TRUE(solution.expression_value_proxies.empty());
    EXPECT_TRUE(solution.constraint_value_proxies.empty());
    EXPECT_TRUE(solution.violation_value_proxies.empty());

    EXPECT_FLOAT_EQ(0.0, solution.objective);
    EXPECT_FLOAT_EQ(0.0, solution.total_violation);
    EXPECT_FALSE(solution.is_feasible);
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/