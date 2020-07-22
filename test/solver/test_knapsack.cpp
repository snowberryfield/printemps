/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <gtest/gtest.h>
#include <random>
#include <cppmh.h>

/*****************************************************************************/
namespace {
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

    /// Option default
    {
        /// initial value definition
        for (auto n = 0; n < number_of_items; n++) {
            x(n) = 0;
        }

        auto result = cppmh::solver::solve(&model);
        EXPECT_EQ(true, result.solution.is_feasible());
    }

    /// Option case 1
    {
        /// initial value definition
        for (auto n = 0; n < number_of_items; n++) {
            x(n) = 0;
        }

        /// solve
        cppmh::solver::Option option;

        option.iteration_max                           = 10;
        option.is_enabled_grouping_penalty_coefficient = true;
        option.is_enabled_initial_value_correction     = true;
        option.is_enabled_lagrange_dual                = true;
        option.is_enabled_local_search                 = true;
        option.is_enabled_parallel_evaluation          = true;
        option.is_enabled_parallel_neighborhood_update = true;
        option.target_objective_value                  = 1E100;
        option.verbose                                 = cppmh::solver::None;
        option.tabu_search.iteration_max               = 100;
        option.tabu_search.initial_tabu_tenure         = 10;
        option.tabu_search.tabu_mode =
            cppmh::solver::tabu_search::TabuMode::All;

        option.tabu_search.is_enabled_improvability_screening          = true;
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
    }

    /// Option case 2
    {
        /// initial value definition
        for (auto n = 0; n < number_of_items; n++) {
            x(n) = 0;
        }

        /// solve
        cppmh::solver::Option option;

        option.iteration_max                           = 10;
        option.is_enabled_grouping_penalty_coefficient = false;
        option.is_enabled_initial_value_correction     = false;
        option.is_enabled_lagrange_dual                = false;
        option.is_enabled_local_search                 = false;
        option.is_enabled_parallel_evaluation          = false;
        option.is_enabled_parallel_neighborhood_update = false;
        option.target_objective_value                  = 1E100;
        option.verbose                                 = cppmh::solver::None;
        option.tabu_search.iteration_max               = 100;
        option.tabu_search.initial_tabu_tenure         = 10;
        option.tabu_search.tabu_mode =
            cppmh::solver::tabu_search::TabuMode::Any;
        option.tabu_search.is_enabled_improvability_screening          = false;
        option.tabu_search.is_enabled_shuffle                          = false;
        option.tabu_search.is_enabled_move_curtail                     = false;
        option.tabu_search.is_enabled_automatic_break                  = false;
        option.tabu_search.is_enabled_automatic_tabu_tenure_adjustment = false;
        option.tabu_search.move_preserve_rate                          = 0.5;
        option.tabu_search.is_enabled_initial_modification             = false;
        option.tabu_search.ignore_tabu_if_augmented_incumbent          = false;
        option.tabu_search.ignore_tabu_if_feasible_incumbent           = false;

        auto result = cppmh::solver::solve(&model, option);
        EXPECT_EQ(true, result.solution.is_feasible());
    }
}
/*****************************************************************************/
}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/