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
class TestProblemSizeReducerSpecial : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestProblemSizeReducerSpecial, remove_redundant_set_variables) {
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

        model.builder().update_derived_components();

        preprocess::ProblemSizeReducerSpecial<int, double>  //
            problem_size_reducer_special(&model);
        problem_size_reducer_special.remove_redundant_set_variables(false);

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

        model.builder().update_derived_components();

        preprocess::ProblemSizeReducerSpecial<int, double>  //
            problem_size_reducer_special(&model);
        problem_size_reducer_special.remove_redundant_set_variables(false);

        EXPECT_TRUE(x(4).is_fixed());
        EXPECT_TRUE(x(5).is_fixed());
        EXPECT_TRUE(x(6).is_fixed());
        EXPECT_TRUE(x(7).is_fixed());
    }
}

/*****************************************************************************/
TEST_F(TestProblemSizeReducerSpecial, remove_redundant_set_constraints) {
    model::Model<int, double> model;
    auto&                     x = model.create_variables("x", 10, 0, 1);

    model.create_constraint("g_0", x(0) + x(1) == 1);
    model.create_constraint("g_1", x.selection());

    model.builder().update_derived_components();

    preprocess::ProblemSizeReducerSpecial<int, double>  //
         problem_size_reducer_special(&model);
    auto result =
        problem_size_reducer_special.remove_redundant_set_constraints(false);
    EXPECT_EQ(1, result.number_of_fixed_variables);
    EXPECT_EQ(8, result.number_of_disabled_constraints);
    EXPECT_FALSE(x(0).is_fixed());
    EXPECT_FALSE(x(1).is_fixed());
    EXPECT_TRUE(x(2).is_fixed());
    EXPECT_TRUE(x(9).is_fixed());
}

/*****************************************************************************/
TEST_F(TestProblemSizeReducerSpecial, extract_implicit_equality_constraints) {
    model::Model<int, double> model;
    auto&                     x = model.create_variables("x", 10, -10, 10);
    model.minimize(x.sum());
    model.create_constraint("g_0", x(0) + x(1) <= 10);
    model.create_constraint("g_1", x(0) + x(1) >= 10);
    model.create_constraint("g_2", -2 * x(0) - x(1) <= -10);
    model.create_constraint("g_2", 2 * x(0) + x(1) <= 10);

    model.builder().update_derived_components();

    preprocess::ProblemSizeReducerSpecial<int, double>  //
         problem_size_reducer_special(&model);
    auto result =
        problem_size_reducer_special.extract_implicit_equality_constraints(
            false);
    EXPECT_EQ(2, result.number_of_disabled_constraints);
}

/*****************************************************************************/
TEST_F(TestProblemSizeReducerSpecial, remove_duplicated_constraints) {
    model::Model<int, double> model;
    auto&                     x = model.create_variables("x", 10, -10, 10);
    model.minimize(x.sum());
    model.create_constraint("g_0", 2 * x(0) + x(1) == 10);
    model.create_constraint("g_1", 2 * x(0) + x(1) == 10);
    model.create_constraint("g_2", 2 * x(0) + x(1) <= 10);
    model.create_constraint("g_3", 2 * x(0) + x(1) == 20);

    model.builder().update_derived_components();

    preprocess::ProblemSizeReducerSpecial<int, double>  //
         problem_size_reducer_special(&model);
    auto result =
        problem_size_reducer_special.remove_duplicated_constraints(false);
    EXPECT_EQ(1, result.number_of_disabled_constraints);
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/