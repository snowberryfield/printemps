/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <gtest/gtest.h>
#include <random>
#include <cppmh.h>

namespace {
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
    cppmh::model::IPModel model;

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
    cppmh::solver::Option option;

    option.iteration_max                           = 50;
    option.is_enabled_grouping_penalty_coefficient = true;
    option.is_enabled_initial_value_correction     = true;
    option.is_enabled_lagrange_dual                = true;
    option.is_enabled_local_search                 = true;
    option.is_enabled_parallel_evaluation          = true;
    option.is_enabled_parallel_neighborhood_update = true;
    option.is_enabled_improvability_screening      = true;
    option.is_enabled_binary_move                  = true;
    option.is_enabled_integer_move                 = true;
    option.is_enabled_aggregation_move             = true;
    option.is_enabled_precedence_move              = true;
    option.is_enabled_variable_bound_move          = true;
    option.is_enabled_exclusive_move               = true;
    option.is_enabled_chain_move                   = true;
    option.is_enabled_user_defined_move            = true;
    option.target_objective_value                  = 1E100;
    option.verbose                                 = cppmh::solver::None;
    option.tabu_search.iteration_max               = 100;
    option.tabu_search.initial_tabu_tenure         = 10;
    option.tabu_search.tabu_mode = cppmh::solver::tabu_search::TabuMode::All;

    option.tabu_search.is_enabled_shuffle                          = true;
    option.tabu_search.is_enabled_move_curtail                     = true;
    option.tabu_search.is_enabled_automatic_break                  = true;
    option.tabu_search.is_enabled_automatic_tabu_tenure_adjustment = true;
    option.tabu_search.move_preserve_rate                          = 0.5;
    option.tabu_search.is_enabled_initial_modification             = true;
    option.tabu_search.ignore_tabu_if_augmented_incumbent          = true;
    option.tabu_search.ignore_tabu_if_feasible_incumbent           = true;

    auto result = cppmh::solver::solve(&model, option);
    EXPECT_EQ(true, result.solution.is_feasible());

    ASSERT_THROW(cppmh::solver::solve(&model, option), std::logic_error);
}
/*****************************************************************************/
}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/