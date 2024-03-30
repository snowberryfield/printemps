/*****************************************************************************/
// Copyright (c) 2020-2024 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <gtest/gtest.h>
#include <printemps.h>

namespace {
using namespace printemps;
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
    solution::SparseSolution<int, double> solution;

    EXPECT_FLOAT_EQ(0.0, solution.objective);
    EXPECT_FLOAT_EQ(0.0, solution.total_violation);
    EXPECT_FALSE(solution.is_feasible);
    EXPECT_TRUE(solution.variables.empty());
}

/*****************************************************************************/
TEST_F(TestSparseSolution, initialize) {
    solution::SparseSolution<int, double> solution;
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

/*****************************************************************************/
TEST_F(TestSparseSolution, distance) {
    solution::SparseSolution<int, double> solution_0;
    solution::SparseSolution<int, double> solution_1;
    solution_0.variables["x"] = 1;
    solution_0.variables["y"] = 2;
    solution_0.variables["z"] = 3;
    solution_0.variables["w"] = 4;

    solution_1.variables["x"] = 1;
    solution_1.variables["y"] = 3;
    solution_1.variables["z"] = 5;
    solution_1.variables["w"] = 4;

    EXPECT_EQ(2, solution_0.distance(solution_1));
    EXPECT_EQ(2, solution_1.distance(solution_0));
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/