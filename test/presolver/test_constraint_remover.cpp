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
class TestConstraintRemover : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestConstraintRemover,
       remove_redundant_constraints_with_tightening_variable_bounds) {
    {
        printemps::model::Model<int, double> model;

        auto& x = model.create_variable("x", 0, 10);
        auto& g = model.create_constraint("g", 3 * x + 1 == 7);

        printemps::presolver::
            remove_redundant_constraints_with_tightening_variable_bounds(&model,
                                                                         false);
        EXPECT_TRUE(x.is_fixed());
        EXPECT_EQ(2, x.value());
        EXPECT_FALSE(g.is_enabled());
    }
    {
        printemps::model::Model<int, double> model;

        auto& x = model.create_variable("x", 0, 10);
        auto& g = model.create_constraint("g", 3 * x + 1 <= 7);

        printemps::presolver::
            remove_redundant_constraints_with_tightening_variable_bounds(&model,
                                                                         false);
        EXPECT_FALSE(x.is_fixed());
        EXPECT_EQ(0, x.lower_bound());
        EXPECT_EQ(2, x.upper_bound());
        EXPECT_FALSE(g.is_enabled());
    }
    {
        printemps::model::Model<int, double> model;

        auto& x = model.create_variable("x", 0, 10);
        auto& g = model.create_constraint("g", 3 * x + 1 >= 7);

        printemps::presolver::
            remove_redundant_constraints_with_tightening_variable_bounds(&model,
                                                                         false);
        EXPECT_FALSE(x.is_fixed());
        EXPECT_EQ(2, x.lower_bound());
        EXPECT_EQ(10, x.upper_bound());
        EXPECT_FALSE(g.is_enabled());
    }

    {
        printemps::model::Model<int, double> model;

        auto& x = model.create_variable("x", -10, 10);
        auto& g = model.create_constraint("g", -3 * x + 1 == 7);

        printemps::presolver::
            remove_redundant_constraints_with_tightening_variable_bounds(&model,
                                                                         false);
        EXPECT_TRUE(x.is_fixed());
        EXPECT_EQ(-2, x.value());
        EXPECT_FALSE(g.is_enabled());
    }
    {
        printemps::model::Model<int, double> model;

        auto& x = model.create_variable("x", -10, 10);
        auto& g = model.create_constraint("g", -3 * x + 1 <= 7);

        printemps::presolver::
            remove_redundant_constraints_with_tightening_variable_bounds(&model,
                                                                         false);
        EXPECT_FALSE(x.is_fixed());
        EXPECT_EQ(-2, x.lower_bound());
        EXPECT_EQ(10, x.upper_bound());
        EXPECT_FALSE(g.is_enabled());
    }
    {
        printemps::model::Model<int, double> model;

        auto& x = model.create_variable("x", -10, 10);
        auto& g = model.create_constraint("g", -3 * x + 1 >= 7);

        printemps::presolver::
            remove_redundant_constraints_with_tightening_variable_bounds(&model,
                                                                         false);
        EXPECT_FALSE(x.is_fixed());
        EXPECT_EQ(-10, x.lower_bound());
        EXPECT_EQ(-2, x.upper_bound());
        EXPECT_FALSE(g.is_enabled());
    }

    {
        printemps::model::Model<int, double> model;

        auto& x = model.create_variable("x", 0, 10);
        auto& y = model.create_variable("y", 0, 1);
        auto& g = model.create_constraint("g", 3 * x + y == 7);
        y.fix_by(1);

        printemps::presolver::
            remove_redundant_constraints_with_tightening_variable_bounds(&model,
                                                                         false);
        EXPECT_TRUE(x.is_fixed());
        EXPECT_EQ(2, x.value());
        EXPECT_FALSE(g.is_enabled());
    }
    {
        printemps::model::Model<int, double> model;

        auto& x = model.create_variable("x", 0, 10);
        auto& y = model.create_variable("y", 0, 1);
        auto& g = model.create_constraint("g", 3 * x + y <= 7);
        y.fix_by(1);

        printemps::presolver::
            remove_redundant_constraints_with_tightening_variable_bounds(&model,
                                                                         false);
        EXPECT_FALSE(x.is_fixed());
        EXPECT_EQ(0, x.lower_bound());
        EXPECT_EQ(2, x.upper_bound());
        EXPECT_FALSE(g.is_enabled());
    }
    {
        printemps::model::Model<int, double> model;

        auto& x = model.create_variable("x", 0, 10);
        auto& y = model.create_variable("y", 0, 1);
        auto& g = model.create_constraint("g", 3 * x + y >= 7);
        y.fix_by(1);

        printemps::presolver::
            remove_redundant_constraints_with_tightening_variable_bounds(&model,
                                                                         false);
        EXPECT_FALSE(x.is_fixed());
        EXPECT_EQ(2, x.lower_bound());
        EXPECT_EQ(10, x.upper_bound());
        EXPECT_FALSE(g.is_enabled());
    }
    {
        printemps::model::Model<int, double> model;

        auto& x = model.create_variable("x", -10, 10);
        auto& y = model.create_variable("y", 0, 1);
        auto& g = model.create_constraint("g", -3 * x + y == 7);
        y.fix_by(1);

        printemps::presolver::
            remove_redundant_constraints_with_tightening_variable_bounds(&model,
                                                                         false);
        EXPECT_TRUE(x.is_fixed());
        EXPECT_EQ(-2, x.value());
        EXPECT_FALSE(g.is_enabled());
    }
    {
        printemps::model::Model<int, double> model;

        auto& x = model.create_variable("x", -10, 10);
        auto& y = model.create_variable("y", 0, 1);
        auto& g = model.create_constraint("g", -3 * x + y <= 7);
        y.fix_by(1);

        printemps::presolver::
            remove_redundant_constraints_with_tightening_variable_bounds(&model,
                                                                         false);
        EXPECT_FALSE(x.is_fixed());
        EXPECT_EQ(-2, x.lower_bound());
        EXPECT_EQ(10, x.upper_bound());
        EXPECT_FALSE(g.is_enabled());
    }
    {
        printemps::model::Model<int, double> model;

        auto& x = model.create_variable("x", -10, 10);
        auto& y = model.create_variable("y", 0, 1);
        auto& g = model.create_constraint("g", -3 * x + y >= 7);
        y.fix_by(1);

        printemps::presolver::
            remove_redundant_constraints_with_tightening_variable_bounds(&model,
                                                                         false);
        EXPECT_FALSE(x.is_fixed());
        EXPECT_EQ(-10, x.lower_bound());
        EXPECT_EQ(-2, x.upper_bound());
        EXPECT_FALSE(g.is_enabled());
    }

    {
        printemps::model::Model<int, double> model;

        auto& x = model.create_variable("x", 0, 10);
        auto& g = model.create_constraint("g", 3 * x + 1 == 7);
        x.fix_by(2);

        printemps::presolver::
            remove_redundant_constraints_with_tightening_variable_bounds(&model,
                                                                         false);
        EXPECT_TRUE(x.is_fixed());
        EXPECT_FALSE(g.is_enabled());
    }
    {
        printemps::model::Model<int, double> model;

        auto& x = model.create_variable("x", 0, 10);
        auto& g = model.create_constraint("g", 3 * x + 1 <= 7);
        x.fix_by(1);

        printemps::presolver::
            remove_redundant_constraints_with_tightening_variable_bounds(&model,
                                                                         false);
        EXPECT_TRUE(x.is_fixed());
        EXPECT_FALSE(g.is_enabled());
    }
    {
        printemps::model::Model<int, double> model;

        auto& x = model.create_variable("x", 0, 10);
        auto& g = model.create_constraint("g", 3 * x + 1 >= 7);
        x.fix_by(3);

        printemps::presolver::
            remove_redundant_constraints_with_tightening_variable_bounds(&model,
                                                                         false);
        EXPECT_TRUE(x.is_fixed());
        EXPECT_FALSE(g.is_enabled());
    }

    {
        printemps::model::Model<int, double> model;

        auto& x = model.create_variable("x", -10, 10);
        auto& g = model.create_constraint("g", -3 * x + 1 == 7);
        x.fix_by(-2);

        printemps::presolver::
            remove_redundant_constraints_with_tightening_variable_bounds(&model,
                                                                         false);
        EXPECT_TRUE(x.is_fixed());
        EXPECT_FALSE(g.is_enabled());
    }
    {
        printemps::model::Model<int, double> model;

        auto& x = model.create_variable("x", -10, 10);
        auto& g = model.create_constraint("g", -3 * x + 1 <= 7);
        x.fix_by(-2);

        printemps::presolver::
            remove_redundant_constraints_with_tightening_variable_bounds(&model,
                                                                         false);
        EXPECT_TRUE(x.is_fixed());
        EXPECT_FALSE(g.is_enabled());
    }
    {
        printemps::model::Model<int, double> model;

        auto& x = model.create_variable("x", -10, 10);
        auto& g = model.create_constraint("g", -3 * x + 1 >= 7);
        x.fix_by(-2);

        printemps::presolver::
            remove_redundant_constraints_with_tightening_variable_bounds(&model,
                                                                         false);
        EXPECT_TRUE(x.is_fixed());
        EXPECT_FALSE(g.is_enabled());
    }

    {
        printemps::model::Model<int, double> model;

        auto& x = model.create_variable("x", 0, 10);
        auto& y = model.create_variable("y", 0, 1);
        auto& g = model.create_constraint("g", 3 * x + y <= 7);

        printemps::presolver::
            remove_redundant_constraints_with_tightening_variable_bounds(&model,
                                                                         false);
        EXPECT_FALSE(x.is_fixed());
        EXPECT_EQ(0, x.lower_bound());
        EXPECT_EQ(2, x.upper_bound());
        EXPECT_TRUE(g.is_enabled());
    }
    {
        printemps::model::Model<int, double> model;

        auto& x = model.create_variable("x", 0, 10);
        auto& y = model.create_variable("y", 0, 1);
        auto& g = model.create_constraint("g", 3 * x + y >= 7);

        printemps::presolver::
            remove_redundant_constraints_with_tightening_variable_bounds(&model,
                                                                         false);
        EXPECT_FALSE(x.is_fixed());
        EXPECT_EQ(2, x.lower_bound());
        EXPECT_EQ(10, x.upper_bound());
        EXPECT_TRUE(g.is_enabled());
    }

    {
        printemps::model::Model<int, double> model;

        auto& x = model.create_variable("x", -10, 10);
        auto& y = model.create_variable("y", 0, 1);
        auto& g = model.create_constraint("g", -3 * x + y <= 7);

        printemps::presolver::
            remove_redundant_constraints_with_tightening_variable_bounds(&model,
                                                                         false);
        EXPECT_FALSE(x.is_fixed());
        EXPECT_EQ(-2, x.lower_bound());
        EXPECT_EQ(10, x.upper_bound());
        EXPECT_TRUE(g.is_enabled());
    }
    {
        printemps::model::Model<int, double> model;

        auto& x = model.create_variable("x", -10, 10);
        auto& y = model.create_variable("y", 0, 1);
        auto& g = model.create_constraint("g", -3 * x + y >= 7);

        printemps::presolver::
            remove_redundant_constraints_with_tightening_variable_bounds(&model,
                                                                         false);
        EXPECT_FALSE(x.is_fixed());
        EXPECT_EQ(-10, x.lower_bound());
        EXPECT_EQ(-2, x.upper_bound());
        EXPECT_TRUE(g.is_enabled());
    }
}
}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/