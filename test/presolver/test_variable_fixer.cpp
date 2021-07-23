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
        printemps::model::Model<int, double> model;

        auto& x = model.create_variables("x", 10, 0, 1);
        model.minimize(x.sum());
        model.setup_variable_sensitivity();

        printemps::presolver::remove_independent_variables(&model, false);

        for (auto i = 0; i < 10; i++) {
            EXPECT_TRUE(x(i).is_fixed());
            EXPECT_EQ(0, x(i).value());
        }
    }
    {
        printemps::model::Model<int, double> model;

        auto& x = model.create_variables("x", 10, 0, 1);
        model.maximize(x.sum());
        model.setup_variable_sensitivity();

        printemps::presolver::remove_independent_variables(&model, false);
        for (auto i = 0; i < 10; i++) {
            EXPECT_TRUE(x(i).is_fixed());
            EXPECT_EQ(1, x(i).value());
        }
    }
    {
        printemps::model::Model<int, double> model;

        auto& x = model.create_variables("x", 10, 0, 1);
        model.minimize(-x.sum());
        model.setup_variable_sensitivity();

        printemps::presolver::remove_independent_variables(&model, false);
        for (auto i = 0; i < 10; i++) {
            EXPECT_TRUE(x(i).is_fixed());
            EXPECT_EQ(1, x(i).value());
        }
    }
    {
        printemps::model::Model<int, double> model;

        auto& x = model.create_variables("x", 10, 0, 1);
        model.maximize(-x.sum());
        model.setup_variable_sensitivity();

        printemps::presolver::remove_independent_variables(&model, false);
        for (auto i = 0; i < 10; i++) {
            EXPECT_TRUE(x(i).is_fixed());
            EXPECT_EQ(0, x(i).value());
        }
    }
}

/*****************************************************************************/
TEST_F(TestVariableFixer, fix_implicit_fixed_variables) {
    printemps::model::Model<int, double> model;

    auto& x = model.create_variables("x", 10, -10, 10);
    x(0).set_bound(5, 5);
    printemps::presolver::fix_implicit_fixed_variables(&model, false);

    EXPECT_EQ(5, x(0).value());
    EXPECT_TRUE(x(0).is_fixed());

    for (auto i = 1; i < 10; i++) {
        EXPECT_FALSE(x(i).is_fixed());
    }
}

/*****************************************************************************/
TEST_F(TestVariableFixer, fix_redundant_set_variables) {
    {
        printemps::model::Model<int, double> model;

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

        model.setup_is_linear();
        model.categorize_variables();
        model.categorize_constraints();
        model.setup_variable_related_constraints();
        model.setup_variable_sensitivity();

        printemps::presolver::fix_redundant_set_variables(&model, false);

        EXPECT_TRUE(x(3).is_fixed());
        EXPECT_TRUE(x(4).is_fixed());
        EXPECT_TRUE(x(8).is_fixed());
    }

    {
        printemps::model::Model<int, double> model;

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

        model.setup_is_linear();
        model.categorize_variables();
        model.categorize_constraints();
        model.setup_variable_related_constraints();
        model.setup_variable_sensitivity();
        printemps::presolver::fix_redundant_set_variables(&model, false);

        EXPECT_TRUE(x(4).is_fixed());
        EXPECT_TRUE(x(5).is_fixed());
        EXPECT_TRUE(x(6).is_fixed());
        EXPECT_TRUE(x(7).is_fixed());
    }
}
}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/