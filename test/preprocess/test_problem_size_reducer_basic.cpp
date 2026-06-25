/*****************************************************************************/
// Copyright (c) 2020-2026 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <gtest/gtest.h>
#include <printemps.h>

namespace {
using namespace printemps;
/*****************************************************************************/
class TestProblemSizeReducerBasic : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestProblemSizeReducerBasic, run) {
    model::Model<int, double> model;

    auto& x = model.create_variables("x", 10, -10, 10);
    model.minimize(x.sum());
    model.create_constraint("g_0", 2 * x(0) == 4);
    model.create_constraint("g_1", 3 * x(1) <= 10);
    model.create_constraint("g_2", 8 * x(1) >= 20);
    model.create_constraint("g_3", x(1) + x(2) + 1 == 8);

    model.builder().update_derived_components();

    preprocess::ProblemSizeReducerBasic<int, double>  //
        problem_size_reducer_basic(&model);
    problem_size_reducer_basic.run(false);
    model.builder().update_derived_components();

    EXPECT_EQ(10, model.reference().number_of_fixed_variables());
    EXPECT_EQ(4, model.reference().number_of_disabled_constraints());
    EXPECT_TRUE(x(0).is_fixed());
    EXPECT_EQ(2, x(0).value());
    EXPECT_TRUE(x(1).is_fixed());
    EXPECT_EQ(3, x(1).value());
    EXPECT_TRUE(x(2).is_fixed());
    EXPECT_EQ(4, x(2).value());

    for (auto i = 3; i < 10; i++) {
        EXPECT_TRUE(x(i).is_fixed());
        EXPECT_EQ(-10, x(i).value());
    }
}

/*****************************************************************************/
TEST_F(TestProblemSizeReducerBasic, remove_insensitive_variables) {
    {
        model::Model<int, double> model;

        auto& x = model.create_variables("x", 10, 0, 1);
        model.minimize(x.sum());
        model.builder().update_derived_components();

        preprocess::ProblemSizeReducerBasic<int, double>
            problem_size_reducer_basic(&model);
        problem_size_reducer_basic.remove_insensitive_variables(false);

        for (auto i = 0; i < 10; i++) {
            EXPECT_TRUE(x(i).is_fixed());
            EXPECT_EQ(0, x(i).value());
        }
    }
    {
        model::Model<int, double> model;

        auto& x = model.create_variables("x", 10, 0, 1);
        model.maximize(x.sum());
        model.builder().update_derived_components();

        preprocess::ProblemSizeReducerBasic<int, double>
            problem_size_reducer_basic(&model);
        problem_size_reducer_basic.remove_insensitive_variables(false);

        for (auto i = 0; i < 10; i++) {
            EXPECT_TRUE(x(i).is_fixed());
            EXPECT_EQ(1, x(i).value());
        }
    }
    {
        model::Model<int, double> model;

        auto& x = model.create_variables("x", 10, 0, 1);
        model.minimize(-x.sum());
        model.builder().update_derived_components();

        preprocess::ProblemSizeReducerBasic<int, double>
            problem_size_reducer_basic(&model);
        problem_size_reducer_basic.remove_insensitive_variables(false);

        for (auto i = 0; i < 10; i++) {
            EXPECT_TRUE(x(i).is_fixed());
            EXPECT_EQ(1, x(i).value());
        }
    }
    {
        model::Model<int, double> model;

        auto& x = model.create_variables("x", 10, 0, 1);
        model.maximize(-x.sum());
        model.builder().update_derived_components();

        preprocess::ProblemSizeReducerBasic<int, double>
            problem_size_reducer_basic(&model);
        problem_size_reducer_basic.remove_insensitive_variables(false);

        for (auto i = 0; i < 10; i++) {
            EXPECT_TRUE(x(i).is_fixed());
            EXPECT_EQ(0, x(i).value());
        }
    }
}

/*****************************************************************************/
TEST_F(TestProblemSizeReducerBasic, remove_inactive_binary_variables) {
    {
        model::Model<int, double> model;

        auto& x = model.create_variables("x", 2, 0, 1);
        auto& f = model.create_constraints("f", 2);
        f(0)    = x(0) - x(1) <= 1;
        model.minimize(x.sum());
        model.builder().update_derived_components();

        preprocess::ProblemSizeReducerBasic<int, double>
            problem_size_reducer_basic(&model);
        problem_size_reducer_basic.remove_inactive_binary_variables(false);

        EXPECT_TRUE(x(0).is_fixed());
        EXPECT_EQ(0, x(0).value());
        EXPECT_FALSE(x(1).is_fixed());
    }

    {
        model::Model<int, double> model;

        auto& x = model.create_variables("x", 2, 0, 1);
        auto& f = model.create_constraints("f", 2);
        f(0)    = x(0) - x(1) >= 1;
        model.minimize(-x.sum());
        model.builder().update_derived_components();

        preprocess::ProblemSizeReducerBasic<int, double>
            problem_size_reducer_basic(&model);
        problem_size_reducer_basic.remove_inactive_binary_variables(false);

        EXPECT_TRUE(x(0).is_fixed());
        EXPECT_EQ(1, x(0).value());
        EXPECT_FALSE(x(1).is_fixed());
    }

    {
        model::Model<int, double> model;

        auto& x = model.create_variables("x", 2, 0, 1);
        auto& f = model.create_constraints("f", 2);
        f(0)    = x(0) - x(1) <= 1;
        model.maximize(x.sum());
        model.builder().update_derived_components();

        preprocess::ProblemSizeReducerBasic<int, double>
            problem_size_reducer_basic(&model);
        problem_size_reducer_basic.remove_inactive_binary_variables(false);

        EXPECT_TRUE(x(1).is_fixed());
        EXPECT_EQ(1, x(1).value());
        EXPECT_FALSE(x(0).is_fixed());
    }

    {
        model::Model<int, double> model;

        auto& x = model.create_variables("x", 2, 0, 1);
        auto& f = model.create_constraints("f", 2);
        f(0)    = x(0) - x(1) >= 1;
        model.maximize(-x.sum());
        model.builder().update_derived_components();

        preprocess::ProblemSizeReducerBasic<int, double>
            problem_size_reducer_basic(&model);
        problem_size_reducer_basic.remove_inactive_binary_variables(false);

        EXPECT_TRUE(x(1).is_fixed());
        EXPECT_EQ(0, x(1).value());
        EXPECT_FALSE(x(0).is_fixed());
    }
}

/*****************************************************************************/
TEST_F(TestProblemSizeReducerBasic,
       remove_redundant_constraints_with_tightening_variable_bounds) {
    {
        model::Model<int, double> model;

        auto& x = model.create_variable("x", 0, 10);
        auto& g = model.create_constraint("g", 3 * x + 1 == 7);

        preprocess::ProblemSizeReducerBasic<int, double>  //
            problem_size_reducer_basic(&model);
        problem_size_reducer_basic
            .remove_redundant_constraints_with_tightening_variable_bounds(
                false);

        EXPECT_TRUE(x.is_fixed());
        EXPECT_EQ(2, x.value());
        EXPECT_FALSE(g.is_enabled());
    }
    {
        model::Model<int, double> model;

        auto& x = model.create_variable("x", 0, 10);
        auto& g = model.create_constraint("g", 3 * x + 1 <= 7);

        preprocess::ProblemSizeReducerBasic<int, double>  //
            problem_size_reducer_basic(&model);
        problem_size_reducer_basic
            .remove_redundant_constraints_with_tightening_variable_bounds(
                false);

        EXPECT_FALSE(x.is_fixed());
        EXPECT_EQ(0, x.lower_bound());
        EXPECT_EQ(2, x.upper_bound());
        EXPECT_FALSE(g.is_enabled());
    }
    {
        model::Model<int, double> model;

        auto& x = model.create_variable("x", 0, 10);
        auto& g = model.create_constraint("g", 3 * x + 1 >= 7);

        preprocess::ProblemSizeReducerBasic<int, double>  //
            problem_size_reducer_basic(&model);
        problem_size_reducer_basic
            .remove_redundant_constraints_with_tightening_variable_bounds(
                false);

        EXPECT_FALSE(x.is_fixed());
        EXPECT_EQ(2, x.lower_bound());
        EXPECT_EQ(10, x.upper_bound());
        EXPECT_FALSE(g.is_enabled());
    }

    {
        model::Model<int, double> model;

        auto& x = model.create_variable("x", -10, 10);
        auto& g = model.create_constraint("g", -3 * x + 1 == 7);

        preprocess::ProblemSizeReducerBasic<int, double>  //
            problem_size_reducer_basic(&model);
        problem_size_reducer_basic
            .remove_redundant_constraints_with_tightening_variable_bounds(
                false);

        EXPECT_TRUE(x.is_fixed());
        EXPECT_EQ(-2, x.value());
        EXPECT_FALSE(g.is_enabled());
    }
    {
        model::Model<int, double> model;

        auto& x = model.create_variable("x", -10, 10);
        auto& g = model.create_constraint("g", -3 * x + 1 <= 7);

        preprocess::ProblemSizeReducerBasic<int, double>  //
            problem_size_reducer_basic(&model);
        problem_size_reducer_basic
            .remove_redundant_constraints_with_tightening_variable_bounds(
                false);

        EXPECT_FALSE(x.is_fixed());
        EXPECT_EQ(-2, x.lower_bound());
        EXPECT_EQ(10, x.upper_bound());
        EXPECT_FALSE(g.is_enabled());
    }
    {
        model::Model<int, double> model;

        auto& x = model.create_variable("x", -10, 10);
        auto& g = model.create_constraint("g", -3 * x + 1 >= 7);

        preprocess::ProblemSizeReducerBasic<int, double>  //
            problem_size_reducer_basic(&model);
        problem_size_reducer_basic
            .remove_redundant_constraints_with_tightening_variable_bounds(
                false);

        EXPECT_FALSE(x.is_fixed());
        EXPECT_EQ(-10, x.lower_bound());
        EXPECT_EQ(-2, x.upper_bound());
        EXPECT_FALSE(g.is_enabled());
    }

    {
        model::Model<int, double> model;

        auto& x = model.create_variable("x", 0, 10);
        auto& y = model.create_variable("y", 0, 1);
        auto& g = model.create_constraint("g", 3 * x + y == 7);
        y.fix_by(1);

        preprocess::ProblemSizeReducerBasic<int, double>  //
            problem_size_reducer_basic(&model);
        problem_size_reducer_basic
            .remove_redundant_constraints_with_tightening_variable_bounds(
                false);

        EXPECT_TRUE(x.is_fixed());
        EXPECT_EQ(2, x.value());
        EXPECT_FALSE(g.is_enabled());
    }
    {
        model::Model<int, double> model;

        auto& x = model.create_variable("x", 0, 10);
        auto& y = model.create_variable("y", 0, 1);
        auto& g = model.create_constraint("g", 3 * x + y <= 7);
        y.fix_by(1);

        preprocess::ProblemSizeReducerBasic<int, double>  //
            problem_size_reducer_basic(&model);
        problem_size_reducer_basic
            .remove_redundant_constraints_with_tightening_variable_bounds(
                false);

        EXPECT_FALSE(x.is_fixed());
        EXPECT_EQ(0, x.lower_bound());
        EXPECT_EQ(2, x.upper_bound());
        EXPECT_FALSE(g.is_enabled());
    }
    {
        model::Model<int, double> model;

        auto& x = model.create_variable("x", 0, 10);
        auto& y = model.create_variable("y", 0, 1);
        auto& g = model.create_constraint("g", 3 * x + y >= 7);
        y.fix_by(1);

        preprocess::ProblemSizeReducerBasic<int, double>  //
            problem_size_reducer_basic(&model);
        problem_size_reducer_basic
            .remove_redundant_constraints_with_tightening_variable_bounds(
                false);

        EXPECT_FALSE(x.is_fixed());
        EXPECT_EQ(2, x.lower_bound());
        EXPECT_EQ(10, x.upper_bound());
        EXPECT_FALSE(g.is_enabled());
    }
    {
        model::Model<int, double> model;

        auto& x = model.create_variable("x", -10, 10);
        auto& y = model.create_variable("y", 0, 1);
        auto& g = model.create_constraint("g", -3 * x + y == 7);
        y.fix_by(1);

        preprocess::ProblemSizeReducerBasic<int, double>  //
            problem_size_reducer_basic(&model);
        problem_size_reducer_basic
            .remove_redundant_constraints_with_tightening_variable_bounds(
                false);

        EXPECT_TRUE(x.is_fixed());
        EXPECT_EQ(-2, x.value());
        EXPECT_FALSE(g.is_enabled());
    }
    {
        model::Model<int, double> model;

        auto& x = model.create_variable("x", -10, 10);
        auto& y = model.create_variable("y", 0, 1);
        auto& g = model.create_constraint("g", -3 * x + y <= 7);
        y.fix_by(1);

        preprocess::ProblemSizeReducerBasic<int, double>  //
            problem_size_reducer_basic(&model);
        problem_size_reducer_basic
            .remove_redundant_constraints_with_tightening_variable_bounds(
                false);

        EXPECT_FALSE(x.is_fixed());
        EXPECT_EQ(-2, x.lower_bound());
        EXPECT_EQ(10, x.upper_bound());
        EXPECT_FALSE(g.is_enabled());
    }
    {
        model::Model<int, double> model;

        auto& x = model.create_variable("x", -10, 10);
        auto& y = model.create_variable("y", 0, 1);
        auto& g = model.create_constraint("g", -3 * x + y >= 7);
        y.fix_by(1);

        preprocess::ProblemSizeReducerBasic<int, double>  //
            problem_size_reducer_basic(&model);
        problem_size_reducer_basic
            .remove_redundant_constraints_with_tightening_variable_bounds(
                false);

        EXPECT_FALSE(x.is_fixed());
        EXPECT_EQ(-10, x.lower_bound());
        EXPECT_EQ(-2, x.upper_bound());
        EXPECT_FALSE(g.is_enabled());
    }

    {
        model::Model<int, double> model;

        auto& x = model.create_variable("x", 0, 10);
        auto& g = model.create_constraint("g", 3 * x + 1 == 7);
        x.fix_by(2);

        preprocess::ProblemSizeReducerBasic<int, double>  //
            problem_size_reducer_basic(&model);
        problem_size_reducer_basic
            .remove_redundant_constraints_with_tightening_variable_bounds(
                false);

        EXPECT_TRUE(x.is_fixed());
        EXPECT_FALSE(g.is_enabled());
    }
    {
        model::Model<int, double> model;

        auto& x = model.create_variable("x", 0, 10);
        auto& g = model.create_constraint("g", 3 * x + 1 <= 7);
        x.fix_by(1);

        preprocess::ProblemSizeReducerBasic<int, double>  //
            problem_size_reducer_basic(&model);
        problem_size_reducer_basic
            .remove_redundant_constraints_with_tightening_variable_bounds(
                false);

        EXPECT_TRUE(x.is_fixed());
        EXPECT_FALSE(g.is_enabled());
    }
    {
        model::Model<int, double> model;

        auto& x = model.create_variable("x", 0, 10);
        auto& g = model.create_constraint("g", 3 * x + 1 >= 7);
        x.fix_by(3);

        preprocess::ProblemSizeReducerBasic<int, double>  //
            problem_size_reducer_basic(&model);
        problem_size_reducer_basic
            .remove_redundant_constraints_with_tightening_variable_bounds(
                false);

        EXPECT_TRUE(x.is_fixed());
        EXPECT_FALSE(g.is_enabled());
    }

    {
        model::Model<int, double> model;

        auto& x = model.create_variable("x", -10, 10);
        auto& g = model.create_constraint("g", -3 * x + 1 == 7);
        x.fix_by(-2);

        preprocess::ProblemSizeReducerBasic<int, double>  //
            problem_size_reducer_basic(&model);
        problem_size_reducer_basic
            .remove_redundant_constraints_with_tightening_variable_bounds(
                false);

        EXPECT_TRUE(x.is_fixed());
        EXPECT_FALSE(g.is_enabled());
    }
    {
        model::Model<int, double> model;

        auto& x = model.create_variable("x", -10, 10);
        auto& g = model.create_constraint("g", -3 * x + 1 <= 7);
        x.fix_by(-2);

        preprocess::ProblemSizeReducerBasic<int, double>  //
            problem_size_reducer_basic(&model);
        problem_size_reducer_basic
            .remove_redundant_constraints_with_tightening_variable_bounds(
                false);

        EXPECT_TRUE(x.is_fixed());
        EXPECT_FALSE(g.is_enabled());
    }
    {
        model::Model<int, double> model;

        auto& x = model.create_variable("x", -10, 10);
        auto& g = model.create_constraint("g", -3 * x + 1 >= 7);
        x.fix_by(-2);

        preprocess::ProblemSizeReducerBasic<int, double>  //
            problem_size_reducer_basic(&model);
        problem_size_reducer_basic
            .remove_redundant_constraints_with_tightening_variable_bounds(
                false);

        EXPECT_TRUE(x.is_fixed());
        EXPECT_FALSE(g.is_enabled());
    }

    {
        model::Model<int, double> model;

        auto& x = model.create_variable("x", 0, 10);
        auto& y = model.create_variable("y", 0, 1);
        auto& g = model.create_constraint("g", 3 * x + y <= 7);

        preprocess::ProblemSizeReducerBasic<int, double>  //
            problem_size_reducer_basic(&model);
        problem_size_reducer_basic
            .remove_redundant_constraints_with_tightening_variable_bounds(
                false);

        EXPECT_FALSE(x.is_fixed());
        EXPECT_EQ(0, x.lower_bound());
        EXPECT_EQ(2, x.upper_bound());
        EXPECT_TRUE(g.is_enabled());
    }
    {
        model::Model<int, double> model;

        auto& x = model.create_variable("x", 0, 10);
        auto& y = model.create_variable("y", 0, 1);
        auto& g = model.create_constraint("g", 3 * x + y >= 7);

        preprocess::ProblemSizeReducerBasic<int, double>  //
            problem_size_reducer_basic(&model);
        problem_size_reducer_basic
            .remove_redundant_constraints_with_tightening_variable_bounds(
                false);

        EXPECT_FALSE(x.is_fixed());
        EXPECT_EQ(2, x.lower_bound());
        EXPECT_EQ(10, x.upper_bound());
        EXPECT_TRUE(g.is_enabled());
    }

    {
        model::Model<int, double> model;

        auto& x = model.create_variable("x", -10, 10);
        auto& y = model.create_variable("y", 0, 1);
        auto& g = model.create_constraint("g", -3 * x + y <= 7);

        preprocess::ProblemSizeReducerBasic<int, double>  //
            problem_size_reducer_basic(&model);
        problem_size_reducer_basic
            .remove_redundant_constraints_with_tightening_variable_bounds(
                false);

        EXPECT_FALSE(x.is_fixed());
        EXPECT_EQ(-2, x.lower_bound());
        EXPECT_EQ(10, x.upper_bound());
        EXPECT_TRUE(g.is_enabled());
    }
    {
        model::Model<int, double> model;

        auto& x = model.create_variable("x", -10, 10);
        auto& y = model.create_variable("y", 0, 1);
        auto& g = model.create_constraint("g", -3 * x + y >= 7);

        preprocess::ProblemSizeReducerBasic<int, double>  //
            problem_size_reducer_basic(&model);
        problem_size_reducer_basic
            .remove_redundant_constraints_with_tightening_variable_bounds(
                false);

        EXPECT_FALSE(x.is_fixed());
        EXPECT_EQ(-10, x.lower_bound());
        EXPECT_EQ(-2, x.upper_bound());
        EXPECT_TRUE(g.is_enabled());
    }
}

/*****************************************************************************/
TEST_F(TestProblemSizeReducerBasic, remove_implicit_fixed_variables) {
    model::Model<int, double> model;

    auto& x = model.create_variables("x", 10, -10, 10);
    x(0).set_bound(5, 5);

    preprocess::ProblemSizeReducerBasic<int, double>  //
        problem_size_reducer_basic(&model);
    problem_size_reducer_basic.remove_implicit_fixed_variables(false);

    EXPECT_EQ(5, x(0).value());
    EXPECT_TRUE(x(0).is_fixed());

    for (auto i = 1; i < 10; i++) {
        EXPECT_FALSE(x(i).is_fixed());
    }
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/