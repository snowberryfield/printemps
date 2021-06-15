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
class TestPresolver : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestPresolver, presolve) {
    printemps::model::Model<int, double> model;

    auto& x = model.create_variables("x", 10, -10, 10);
    model.minimize(x.sum());
    model.create_constraint("g_0", 2 * x(0) == 4);
    model.create_constraint("g_1", 3 * x(1) <= 10);
    model.create_constraint("g_2", 8 * x(1) >= 20);
    model.create_constraint("g_3", x(1) + x(2) + 1 == 8);

    model.setup_is_linear();
    model.categorize_variables();
    model.categorize_constraints();
    model.setup_variable_related_constraints();
    model.setup_variable_sensitivity();

    printemps::presolver::presolve(&model, true, false);
    model.categorize_variables();
    model.categorize_constraints();

    EXPECT_EQ(10, model.number_of_fixed_variables());
    EXPECT_EQ(4, model.number_of_disabled_constraints());
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