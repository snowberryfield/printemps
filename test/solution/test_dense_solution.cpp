/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <gtest/gtest.h>
#include <printemps.h>

namespace {
using namespace printemps;
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
    solution::DenseSolution<int, double> solution;

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
    solution::DenseSolution<int, double> solution;
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

/*****************************************************************************/
TEST_F(TestDenseSolution, distance) {
    solution::DenseSolution<int, double> solution_0;
    solution_0.variable_value_proxies.push_back(
        multi_array::ValueProxy<int>(-1, {2, 2}));

    solution::DenseSolution<int, double> solution_1;
    solution_1.variable_value_proxies.push_back(
        multi_array::ValueProxy<int>(-1, {2, 2}));

    solution_0.variable_value_proxies.front().values(0, 0) = 1;
    solution_0.variable_value_proxies.front().values(0, 1) = 2;
    solution_0.variable_value_proxies.front().values(1, 0) = 3;
    solution_0.variable_value_proxies.front().values(1, 1) = 4;

    solution_1.variable_value_proxies.front().values(0, 0) = 1;
    solution_1.variable_value_proxies.front().values(0, 1) = 3;
    solution_1.variable_value_proxies.front().values(1, 0) = 5;
    solution_1.variable_value_proxies.front().values(1, 1) = 4;

    EXPECT_EQ(2, solution_0.distance(solution_1));
    EXPECT_EQ(2, solution_1.distance(solution_0));
}

/*****************************************************************************/
TEST_F(TestDenseSolution, to_sparse) {
    solution::DenseSolution<int, double> solution;
    solution.variable_value_proxies.push_back(
        multi_array::ValueProxy<int>(-1, {2, 2}));

    solution.variable_value_proxies.front().names(0, 0) = "x_0_0";
    solution.variable_value_proxies.front().names(0, 1) = "x_0_1";
    solution.variable_value_proxies.front().names(1, 0) = "x_1_0";
    solution.variable_value_proxies.front().names(1, 1) = "x_1_1";

    solution.variable_value_proxies.front().values(0, 0) = 1;
    solution.variable_value_proxies.front().values(0, 1) = 0;
    solution.variable_value_proxies.front().values(1, 0) = 3;
    solution.variable_value_proxies.front().values(1, 1) = 4;

    solution.objective                  = 3.14;
    solution.total_violation            = 15;
    solution.global_augmented_objective = 1E5;
    solution.is_feasible                = true;

    auto sparse_solution = solution.to_sparse();

    EXPECT_TRUE(sparse_solution.variables.find("x_0_0") !=
                sparse_solution.variables.end());
    EXPECT_FALSE(sparse_solution.variables.find("x_0_1") !=
                 sparse_solution.variables.end());
    EXPECT_TRUE(sparse_solution.variables.find("x_1_0") !=
                sparse_solution.variables.end());
    EXPECT_TRUE(sparse_solution.variables.find("x_1_1") !=
                sparse_solution.variables.end());

    EXPECT_EQ(1, sparse_solution.variables.at("x_0_0"));
    EXPECT_EQ(3, sparse_solution.variables.at("x_1_0"));
    EXPECT_EQ(4, sparse_solution.variables.at("x_1_1"));
    EXPECT_EQ(3, static_cast<int>(sparse_solution.variables.size()));

    EXPECT_FLOAT_EQ(3.14, sparse_solution.objective);
    EXPECT_EQ(15, sparse_solution.total_violation);
    EXPECT_FLOAT_EQ(1E5, sparse_solution.global_augmented_objective);
    EXPECT_TRUE(sparse_solution.is_feasible);
}
}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/