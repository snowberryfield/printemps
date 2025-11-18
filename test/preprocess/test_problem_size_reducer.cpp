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
class TestProblemSizeReducer : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
class TestVariableFixer : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestVariableFixer, remove_independent_variables) {
    {
        model::Model<int, double> model;

        auto& x = model.create_variables("x", 10, 0, 1);
        model.minimize(x.sum());
        model.builder().setup_structure();

        preprocess::ProblemSizeReducer<int, double> problem_size_reducer(
            &model);
        problem_size_reducer.remove_independent_variables(false);

        for (auto i = 0; i < 10; i++) {
            EXPECT_TRUE(x(i).is_fixed());
            EXPECT_EQ(0, x(i).value());
        }
    }
    {
        model::Model<int, double> model;

        auto& x = model.create_variables("x", 10, 0, 1);
        model.maximize(x.sum());
        model.builder().setup_structure();

        preprocess::ProblemSizeReducer<int, double> problem_size_reducer(
            &model);
        problem_size_reducer.remove_independent_variables(false);

        for (auto i = 0; i < 10; i++) {
            EXPECT_TRUE(x(i).is_fixed());
            EXPECT_EQ(1, x(i).value());
        }
    }
    {
        model::Model<int, double> model;

        auto& x = model.create_variables("x", 10, 0, 1);
        model.minimize(-x.sum());
        model.builder().setup_structure();

        preprocess::ProblemSizeReducer<int, double> problem_size_reducer(
            &model);
        problem_size_reducer.remove_independent_variables(false);

        for (auto i = 0; i < 10; i++) {
            EXPECT_TRUE(x(i).is_fixed());
            EXPECT_EQ(1, x(i).value());
        }
    }
    {
        model::Model<int, double> model;

        auto& x = model.create_variables("x", 10, 0, 1);
        model.maximize(-x.sum());
        model.builder().setup_structure();

        preprocess::ProblemSizeReducer<int, double> problem_size_reducer(
            &model);
        problem_size_reducer.remove_independent_variables(false);

        for (auto i = 0; i < 10; i++) {
            EXPECT_TRUE(x(i).is_fixed());
            EXPECT_EQ(0, x(i).value());
        }
    }
}

/*****************************************************************************/
TEST_F(TestVariableFixer, remove_implicit_fixed_variables) {
    model::Model<int, double> model;

    auto& x = model.create_variables("x", 10, -10, 10);
    x(0).set_bound(5, 5);

    preprocess::ProblemSizeReducer<int, double>  //
        problem_size_reducer(&model);
    problem_size_reducer.remove_implicit_fixed_variables(false);

    EXPECT_EQ(5, x(0).value());
    EXPECT_TRUE(x(0).is_fixed());

    for (auto i = 1; i < 10; i++) {
        EXPECT_FALSE(x(i).is_fixed());
    }
}

/*****************************************************************************/
TEST_F(TestVariableFixer, remove_redundant_set_variables) {
    {
        model::Model<int, double> model;

        auto& x = model.create_variables("x", 9, 0, 1);
        auto& g = model.create_constraints("g", 9);

        g(0) = x(0) + x(1) + x(2) <= 1;
        g(1) = x(0) + x(1) + x(2) == 1;
        g(2) = x(0) + x(1) + x(2) >= 1;
        g(3) = x(3) + x(4) + x(5) <= 1;
        g(4) = x(3) + x(4) + x(5) == 1;
        g(5) = x(3) + x(4) + x(5) >= 1;
        g(6) = x(6) + x(7) + x(8) <= 1;
        g(7) = x(6) + x(7) + x(8) == 1;
        g(8) = x(6) + x(7) + x(8) >= 1;

        model.minimize(x(0) + x(1) + x(2)        //
                       + x(3) - x(4) - 2 * x(5)  //
                       + x(6) + x(7) + 2 * x(8));

        model.builder().setup_structure();

        preprocess::ProblemSizeReducer<int, double>  //
            problem_size_reducer(&model);
        problem_size_reducer.remove_redundant_set_variables(false);

        EXPECT_TRUE(x(3).is_fixed());
        EXPECT_TRUE(x(4).is_fixed());
        EXPECT_TRUE(x(8).is_fixed());
    }

    {
        model::Model<int, double> model;

        auto& x = model.create_variables("x", 9, 0, 1);
        auto& g = model.create_constraints("g", 9);

        g(0) = x(0) + x(1) + x(2) <= 1;
        g(1) = x(0) + x(1) + x(2) == 1;
        g(2) = x(0) + x(1) + x(2) >= 1;
        g(3) = x(3) + x(4) + x(5) <= 1;
        g(4) = x(3) + x(4) + x(5) == 1;
        g(5) = x(3) + x(4) + x(5) >= 1;
        g(6) = x(6) + x(7) + x(8) <= 1;
        g(7) = x(6) + x(7) + x(8) == 1;
        g(8) = x(6) + x(7) + x(8) >= 1;

        model.maximize(x(0) + x(1) + x(2)        //
                       + x(3) - x(4) - 2 * x(5)  //
                       + x(6) + x(7) + 2 * x(8));

        model.builder().setup_structure();

        preprocess::ProblemSizeReducer<int, double>  //
            problem_size_reducer(&model);
        problem_size_reducer.remove_redundant_set_variables(false);

        EXPECT_TRUE(x(4).is_fixed());
        EXPECT_TRUE(x(5).is_fixed());
        EXPECT_TRUE(x(6).is_fixed());
        EXPECT_TRUE(x(7).is_fixed());
    }
}

/*****************************************************************************/
TEST_F(TestProblemSizeReducer,
       remove_redundant_constraints_with_tightening_variable_bounds) {
    {
        model::Model<int, double> model;

        auto& x = model.create_variable("x", 0, 10);
        auto& g = model.create_constraint("g", 3 * x + 1 == 7);

        preprocess::ProblemSizeReducer<int, double>  //
            problem_size_reducer(&model);
        problem_size_reducer
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

        preprocess::ProblemSizeReducer<int, double>  //
            problem_size_reducer(&model);
        problem_size_reducer
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

        preprocess::ProblemSizeReducer<int, double>  //
            problem_size_reducer(&model);
        problem_size_reducer
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

        preprocess::ProblemSizeReducer<int, double>  //
            problem_size_reducer(&model);
        problem_size_reducer
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

        preprocess::ProblemSizeReducer<int, double>  //
            problem_size_reducer(&model);
        problem_size_reducer
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

        preprocess::ProblemSizeReducer<int, double>  //
            problem_size_reducer(&model);
        problem_size_reducer
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

        preprocess::ProblemSizeReducer<int, double>  //
            problem_size_reducer(&model);
        problem_size_reducer
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

        preprocess::ProblemSizeReducer<int, double>  //
            problem_size_reducer(&model);
        problem_size_reducer
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

        preprocess::ProblemSizeReducer<int, double>  //
            problem_size_reducer(&model);
        problem_size_reducer
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

        preprocess::ProblemSizeReducer<int, double>  //
            problem_size_reducer(&model);
        problem_size_reducer
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

        preprocess::ProblemSizeReducer<int, double>  //
            problem_size_reducer(&model);
        problem_size_reducer
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

        preprocess::ProblemSizeReducer<int, double>  //
            problem_size_reducer(&model);
        problem_size_reducer
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

        preprocess::ProblemSizeReducer<int, double>  //
            problem_size_reducer(&model);
        problem_size_reducer
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

        preprocess::ProblemSizeReducer<int, double>  //
            problem_size_reducer(&model);
        problem_size_reducer
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

        preprocess::ProblemSizeReducer<int, double>  //
            problem_size_reducer(&model);
        problem_size_reducer
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

        preprocess::ProblemSizeReducer<int, double>  //
            problem_size_reducer(&model);
        problem_size_reducer
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

        preprocess::ProblemSizeReducer<int, double>  //
            problem_size_reducer(&model);
        problem_size_reducer
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

        preprocess::ProblemSizeReducer<int, double>  //
            problem_size_reducer(&model);
        problem_size_reducer
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

        preprocess::ProblemSizeReducer<int, double>  //
            problem_size_reducer(&model);
        problem_size_reducer
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

        preprocess::ProblemSizeReducer<int, double>  //
            problem_size_reducer(&model);
        problem_size_reducer
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

        preprocess::ProblemSizeReducer<int, double>  //
            problem_size_reducer(&model);
        problem_size_reducer
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

        preprocess::ProblemSizeReducer<int, double>  //
            problem_size_reducer(&model);
        problem_size_reducer
            .remove_redundant_constraints_with_tightening_variable_bounds(
                false);

        EXPECT_FALSE(x.is_fixed());
        EXPECT_EQ(-10, x.lower_bound());
        EXPECT_EQ(-2, x.upper_bound());
        EXPECT_TRUE(g.is_enabled());
    }
}

/*****************************************************************************/
TEST_F(TestProblemSizeReducer, remove_duplicated_constraints) {
    model::Model<int, double> model;
    auto&                     x = model.create_variables("x", 10, -10, 10);
    model.minimize(x.sum());
    model.create_constraint("g_0", 2 * x(0) + x(1) == 10);
    model.create_constraint("g_1", 2 * x(0) + x(1) == 10);
    model.create_constraint("g_2", 2 * x(0) + x(1) <= 10);
    model.create_constraint("g_3", 2 * x(0) + x(1) == 20);

    model.builder().setup_structure();

    preprocess::ProblemSizeReducer<int, double>  //
         problem_size_reducer(&model);
    auto number_of_newly_disabled_constraints =
        problem_size_reducer.remove_duplicated_constraints(false);
    EXPECT_EQ(1, number_of_newly_disabled_constraints);
}

/*****************************************************************************/
TEST_F(TestProblemSizeReducer, remove_redundant_set_constraints) {
    model::Model<int, double> model;
    auto&                     x = model.create_variables("x", 10, 0, 1);

    model.create_constraint("g_0", x(0) + x(1) == 1);
    model.create_constraint("g_1", x.selection());

    model.builder().setup_structure();

    preprocess::ProblemSizeReducer<int, double>  //
         problem_size_reducer(&model);
    auto result = problem_size_reducer.remove_redundant_set_constraints(false);
    EXPECT_EQ(1, result.first);
    EXPECT_EQ(8, result.second);
    EXPECT_FALSE(x(0).is_fixed());
    EXPECT_FALSE(x(1).is_fixed());
    EXPECT_TRUE(x(2).is_fixed());
    EXPECT_TRUE(x(9).is_fixed());
}

/*****************************************************************************/
TEST_F(TestProblemSizeReducer, extract_implicit_equality_constraints) {
    model::Model<int, double> model;
    auto&                     x = model.create_variables("x", 10, -10, 10);
    model.minimize(x.sum());
    model.create_constraint("g_0", x(0) + x(1) <= 10);
    model.create_constraint("g_1", x(0) + x(1) >= 10);
    model.create_constraint("g_2", -2 * x(0) - x(1) <= -10);
    model.create_constraint("g_2", 2 * x(0) + x(1) <= 10);

    model.builder().setup_structure();

    preprocess::ProblemSizeReducer<int, double>  //
         problem_size_reducer(&model);
    auto number_of_newly_disabled_constraints =
        problem_size_reducer.extract_implicit_equality_constraints(false);
    EXPECT_EQ(2, number_of_newly_disabled_constraints);
}

/*****************************************************************************/
TEST_F(TestProblemSizeReducer, reduce_problem_size) {
    model::Model<int, double> model;

    auto& x = model.create_variables("x", 10, -10, 10);
    model.minimize(x.sum());
    model.create_constraint("g_0", 2 * x(0) == 4);
    model.create_constraint("g_1", 3 * x(1) <= 10);
    model.create_constraint("g_2", 8 * x(1) >= 20);
    model.create_constraint("g_3", x(1) + x(2) + 1 == 8);

    model.builder().setup_structure();

    preprocess::ProblemSizeReducer<int, double>  //
        problem_size_reducer(&model);
    problem_size_reducer.reduce_problem_size(false);
    model.builder().setup_structure();

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
}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/