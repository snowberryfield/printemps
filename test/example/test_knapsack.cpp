/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <gtest/gtest.h>
#include <random>
#include <printemps.h>

namespace {
using namespace printemps;
/*****************************************************************************/
class TestKnapsack : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestKnapsack, knapsack) {
    /*************************************************************************/
    /// Problem statement
    /*************************************************************************/
    int number_of_items = 10000;

    std::vector<int> weights;
    std::vector<int> volumes;
    std::vector<int> prices;

    for (auto n = 0; n < number_of_items; n++) {
        prices.push_back(rand() % 100);
        weights.push_back(prices.back() + rand() % 10);
        volumes.push_back(prices.back() + rand() % 10);
    }

    /*************************************************************************/
    /// Model object definition
    /*************************************************************************/
    model::IPModel model;

    /*************************************************************************/
    /// Decision variable definitions
    /*************************************************************************/
    auto& x = model.create_variables("x", number_of_items, 0, 1);

    /*************************************************************************/
    /// Expression definitions
    /*************************************************************************/
    auto& total_weight =
        model.create_expression("total_weight", x.dot(weights));

    auto& total_volume =
        model.create_expression("total_volume", x.dot(volumes));

    auto& total_price =  //
        model.create_expression("total_price", x.dot(prices));

    /*************************************************************************/
    /// Constraint definitions
    /*************************************************************************/
    model.create_constraint("total_weight", total_weight <= 30000);
    model.create_constraint("total_volume", total_volume <= 30000);

    /*************************************************************************/
    /// Objective function definition
    /*************************************************************************/
    model.maximize(total_price);

    /// initial value definition
    for (auto n = 0; n < number_of_items; n++) {
        x(n) = 0;
    }

    /// solve
    option::Option option;

    auto result = solver::solve(&model, option);
    EXPECT_TRUE(result.solution.is_feasible());

    ASSERT_THROW(solver::solve(&model, option), std::runtime_error);
}
}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/