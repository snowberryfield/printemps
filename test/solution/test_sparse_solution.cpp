/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <gtest/gtest.h>
#include <printemps.h>

namespace {
/*****************************************************************************/
class TestSparseSolution : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestSparseSolution, constructor) {
    printemps::solution::SparseSolution<int, double> solution;

    EXPECT_FLOAT_EQ(0.0, solution.objective);
    EXPECT_FLOAT_EQ(0.0, solution.total_violation);
    EXPECT_FALSE(solution.is_feasible);
    EXPECT_TRUE(solution.variables.empty());
}

/*****************************************************************************/
TEST_F(TestSparseSolution, initialize) {
    printemps::solution::SparseSolution<int, double> solution;
    solution.objective       = 100.0;
    solution.total_violation = 1000.0;
    solution.is_feasible     = true;
    solution.variables["x"]  = 1;

    solution.initialize();

    EXPECT_FLOAT_EQ(0.0, solution.objective);
    EXPECT_FLOAT_EQ(0.0, solution.total_violation);
    EXPECT_FALSE(solution.is_feasible);
    EXPECT_TRUE(solution.variables.empty());
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/