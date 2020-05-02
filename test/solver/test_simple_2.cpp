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
class TestSimple2 : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestSimple2, simple_2) {
    /**
     * This program solves the following simple binary integer optimization
     * problem:
     * (P):  minimize  f
     *          x,y
     *      subject to x_1 + x_2 + x_2              >= 2,
     *                                   y_1 + y_2   = 1,
     *                 x_1             + y_1         = 1,
     *                 x_1, x_2, x_3, y_1, y_2 \in {0, 1},
     *
     *        where    f_1 = 2 x_1 + 7 x_2 + 9 x_3,
     *                 f_2 = 5 y_1 + 6 y_2.
     */
    cppmh::model::IPModel model;

    auto& x = model.create_variables("x", 3, 0, 1);
    auto& y = model.create_variables("y", 2, 0, 1);
    auto& g = model.create_constraints("g", 3);
    auto& f = model.create_expressions("f", 2);

    std::vector<int> c = {2, 5, 7};
    std::vector<int> d = {5, 6};

    f(0) = x.dot(c);
    f(1) = y.dot(d);
    g(0) = x.sum() >= 2;
    g(1) = y.selection();
    g(2) = x(0) + y(0) == 1;

    model.minimize(f.sum());

    /// Option default
    {
        /// initial solution
        for (auto i = 0; i < 3; i++) {
            x(i) = 0;
        }

        for (auto i = 0; i < 2; i++) {
            y(i) = 0;
        }

        /// solve
        auto result = cppmh::solver::solve(&model);
        EXPECT_EQ(true, result.is_feasible());
        EXPECT_EQ(1, result.variables("x").values(0));
        EXPECT_EQ(1, result.variables("x").values(1));
        EXPECT_EQ(0, result.variables("x").values(2));
        EXPECT_EQ(0, result.variables("y").values(0));
        EXPECT_EQ(1, result.variables("y").values(1));
        EXPECT_EQ(13, result.objective());
    }

    /// Option case 1
    {
        /// initial solution
        for (auto i = 0; i < 3; i++) {
            x(i) = 0;
        }

        for (auto i = 0; i < 2; i++) {
            y(i) = 0;
        }

        /// solve
        cppmh::solver::Option option;

        option.iteration_max                           = 10;
        option.is_enabled_grouping_penalty_coefficient = true;
        option.is_enabled_initial_value_correction     = true;
        option.is_enabled_local_search                 = true;
        option.is_enabled_parallel_evaluation          = true;
        option.is_enabled_parallel_neighborhood_update = true;
        option.target_objective_value                  = -1E100;
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
        option.tabu_search.ignore_tabu_if_augmented_incumbent          = true;
        option.tabu_search.ignore_tabu_if_feasible_incumbent           = true;

        auto result = cppmh::solver::solve(&model, option);
        EXPECT_EQ(true, result.is_feasible());
        EXPECT_EQ(1, result.variables("x").values(0));
        EXPECT_EQ(1, result.variables("x").values(1));
        EXPECT_EQ(0, result.variables("x").values(2));
        EXPECT_EQ(0, result.variables("y").values(0));
        EXPECT_EQ(1, result.variables("y").values(1));
        EXPECT_EQ(13, result.objective());
    }

    /// Option case 2
    {
        /// initial solution
        for (auto i = 0; i < 3; i++) {
            x(i) = 0;
        }

        for (auto i = 0; i < 2; i++) {
            y(i) = 0;
        }
        y(0) = 1;

        /// solve
        cppmh::solver::Option option;

        option.iteration_max                           = 10;
        option.is_enabled_local_search                 = false;
        option.is_enabled_grouping_penalty_coefficient = false;
        option.is_enabled_initial_value_correction     = false;
        option.is_enabled_parallel_evaluation          = false;
        option.is_enabled_parallel_neighborhood_update = false;
        option.target_objective_value                  = -1E100;
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
        option.tabu_search.ignore_tabu_if_augmented_incumbent          = false;
        option.tabu_search.ignore_tabu_if_feasible_incumbent           = false;

        auto result = cppmh::solver::solve(&model, option);
        EXPECT_EQ(true, result.is_feasible());
        EXPECT_EQ(1, result.variables("x").values(0));
        EXPECT_EQ(1, result.variables("x").values(1));
        EXPECT_EQ(0, result.variables("x").values(2));
        EXPECT_EQ(0, result.variables("y").values(0));
        EXPECT_EQ(1, result.variables("y").values(1));
        EXPECT_EQ(13, result.objective());
    }
}
/*****************************************************************************/
}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/