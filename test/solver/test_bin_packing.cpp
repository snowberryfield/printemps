/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <gtest/gtest.h>
#include <random>
#include <printemps.h>

namespace {
/*****************************************************************************/
class TestBinpacking : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestBinpacking, bin_packing) {
    /*************************************************************************/
    /// Problem statement
    /*************************************************************************/
    int number_of_items = 100;
    int number_of_bins  = 30;
    int bin_capacity    = 100;

    std::vector<int> item_volumes;

    for (auto n = 0; n < number_of_items; n++) {
        item_volumes.push_back(rand() % 50);
    }

    /*************************************************************************/
    /// Model object definition
    /*************************************************************************/
    printemps::model::IPModel model;

    /*************************************************************************/
    /// Decision variable definitions
    /*************************************************************************/
    auto& x =
        model.create_variables("x", {number_of_items, number_of_bins}, 0, 1);
    auto& y = model.create_variables("y", number_of_bins, 0, 1);

    /*************************************************************************/
    /// Expression definitions
    /*************************************************************************/
    auto& total_volume  //
        = model.create_expressions("total_volume", number_of_bins);

    for (auto m = 0; m < number_of_bins; m++) {
        total_volume(m) =
            x.dot({printemps::model_component::Range::All, m}, item_volumes);
    }

    auto& number_of_used_bins =
        model.create_expression("number_of_used_bins", y.sum());

    /*************************************************************************/
    /// Constraint definitions
    /*************************************************************************/
    auto& constraint_selection =
        model.create_constraints("selection", number_of_items);
    for (auto n = 0; n < number_of_items; n++) {
        constraint_selection(n) =
            x.selection({n, printemps::model_component::Range::All});
    }

    auto& constraint_cut = model.create_constraints("cut", number_of_bins - 1);
    for (auto m = 0; m < number_of_bins - 1; m++) {
        constraint_cut(m) = (y(m + 1) <= y(m));
    }

    auto& constraint_total_volume =
        model.create_constraints("total_volume", number_of_bins);
    for (auto m = 0; m < number_of_bins; m++) {
        constraint_total_volume(m) = (total_volume(m) <= bin_capacity * y(m));
    }

    /*************************************************************************/
    /// Objective function definition
    /*************************************************************************/
    auto& objective = model.create_expression("objective");

    objective = number_of_used_bins;
    for (auto m = 0; m < number_of_bins; m++) {
        objective += 1E-5 * m * total_volume(m);
    }

    model.minimize(objective);

    for (auto n = 0; n < number_of_items; n++) {
        for (auto m = 0; m < number_of_bins; m++) {
            x(n, m) = 0;
        }
    }

    for (auto n = 0; n < number_of_items; n++) {
        x(n, rand() % number_of_bins) = 1;
    }

    for (auto m = 0; m < number_of_bins; m++) {
        y(m) = 1;
    }

    /// solve
    printemps::option::Option option;

    option.iteration_max                           = 50;
    option.is_enabled_grouping_penalty_coefficient = true;
    option.is_enabled_initial_value_correction     = true;
    option.is_enabled_lagrange_dual                = false;
    option.is_enabled_local_search                 = true;
    option.is_enabled_parallel_evaluation          = true;
    option.is_enabled_parallel_neighborhood_update = true;
    option.is_enabled_binary_move                  = true;
    option.is_enabled_integer_move                 = true;
    option.is_enabled_aggregation_move             = true;
    option.is_enabled_precedence_move              = true;
    option.is_enabled_variable_bound_move          = true;
    option.is_enabled_chain_move                   = true;
    option.is_enabled_user_defined_move            = true;
    option.target_objective_value                  = -1E100;
    option.verbose                         = printemps::option::verbose::None;
    option.tabu_search.iteration_max       = 200;
    option.tabu_search.initial_tabu_tenure = 10;
    option.tabu_search.tabu_mode           = printemps::option::tabu_mode::All;
    option.tabu_search.is_enabled_shuffle  = true;
    option.tabu_search.is_enabled_move_curtail                     = true;
    option.tabu_search.move_preserve_rate                          = 0.5;
    option.tabu_search.is_enabled_automatic_break                  = true;
    option.tabu_search.is_enabled_automatic_tabu_tenure_adjustment = true;
    option.tabu_search.is_enabled_initial_modification             = true;
    option.tabu_search.ignore_tabu_if_global_incumbent             = true;

    auto result = printemps::solver::solve(&model, option);
    EXPECT_TRUE(result.solution.is_feasible());

    ASSERT_THROW(printemps::solver::solve(&model, option), std::logic_error);
}
}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/