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
    model::IPModel model;

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
        total_volume(m) = x.dot({model_component::Range::All, m}, item_volumes);
    }

    auto& number_of_used_bins =
        model.create_expression("number_of_used_bins", y.sum());

    /*************************************************************************/
    /// Constraint definitions
    /*************************************************************************/
    auto& constraint_selection =
        model.create_constraints("selection", number_of_items);
    for (auto n = 0; n < number_of_items; n++) {
        constraint_selection(n) = x.selection({n, model_component::Range::All});
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
    option::Option option;
    option.neighborhood.selection_mode = option::selection_mode::Defined;
    option.tabu_search.tabu_mode       = option::tabu_mode::Any;

    auto result = solver::solve(&model, option);
    EXPECT_TRUE(result.solution.is_feasible());

    ASSERT_THROW(solver::solve(&model, option), std::runtime_error);
}
}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/