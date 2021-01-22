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
    printemps::utility::IntegerUniformRandom m_random_integer;
    printemps::utility::IntegerUniformRandom m_random_positive_integer;

    virtual void SetUp(void) {
        m_random_integer.setup(-1000, 1000, 0);
        m_random_positive_integer.setup(1, 1000, 0);
    }
    virtual void TearDown() {
        /// nothing to do
    }
    int random_integer(void) {
        return m_random_integer.generate_random();
    }

    int random_positive_integer(void) {
        return m_random_positive_integer.generate_random();
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
    model.setup_variable_related_constraints();
    model.setup_is_linear();

    printemps::model::presolve(&model, false);

    model.categorize_variables();
    model.categorize_constraints();

    EXPECT_EQ(10, model.number_of_fixed_variables());
    EXPECT_EQ(4, model.number_of_disabled_constraints());
    EXPECT_EQ(true, x(0).is_fixed());
    EXPECT_EQ(2, x(0).value());
    EXPECT_EQ(true, x(1).is_fixed());
    EXPECT_EQ(3, x(1).value());
    EXPECT_EQ(true, x(2).is_fixed());
    EXPECT_EQ(4, x(2).value());

    for (auto i = 3; i < 10; i++) {
        EXPECT_EQ(true, x(i).is_fixed());
        EXPECT_EQ(-10, x(i).value());
    }
}

/*****************************************************************************/
TEST_F(TestPresolver, remove_independent_variables) {
    {
        printemps::model::Model<int, double> model;

        auto& x = model.create_variables("x", 10, 0, 1);
        model.minimize(x.sum());
        model.setup_variable_related_constraints();
        model.setup_is_linear();
        printemps::model::remove_independent_variables(&model, false);
        for (auto i = 0; i < 10; i++) {
            EXPECT_EQ(true, x(i).is_fixed());
            EXPECT_EQ(0, x(i).value());
        }
        model.categorize_variables();
        model.categorize_constraints();

        EXPECT_EQ(10, model.number_of_fixed_variables());
    }
    {
        printemps::model::Model<int, double> model;

        auto& x = model.create_variables("x", 10, 0, 1);
        model.maximize(x.sum());
        model.setup_variable_related_constraints();
        model.setup_is_linear();
        printemps::model::remove_independent_variables(&model, false);
        for (auto i = 0; i < 10; i++) {
            EXPECT_EQ(true, x(i).is_fixed());
            EXPECT_EQ(1, x(i).value());
        }
        model.categorize_variables();
        model.categorize_constraints();

        EXPECT_EQ(10, model.number_of_fixed_variables());
    }
    {
        printemps::model::Model<int, double> model;

        auto& x = model.create_variables("x", 10, 0, 1);
        model.minimize(-x.sum());
        model.setup_variable_related_constraints();
        model.setup_is_linear();
        printemps::model::remove_independent_variables(&model, false);
        for (auto i = 0; i < 10; i++) {
            EXPECT_EQ(true, x(i).is_fixed());
            EXPECT_EQ(1, x(i).value());
        }
        model.categorize_variables();
        model.categorize_constraints();

        EXPECT_EQ(10, model.number_of_fixed_variables());
    }
    {
        printemps::model::Model<int, double> model;

        auto& x = model.create_variables("x", 10, 0, 1);
        model.maximize(-x.sum());
        model.setup_variable_related_constraints();
        model.setup_is_linear();
        printemps::model::remove_independent_variables(&model, false);
        for (auto i = 0; i < 10; i++) {
            EXPECT_EQ(true, x(i).is_fixed());
            EXPECT_EQ(0, x(i).value());
        }
        model.categorize_variables();
        model.categorize_constraints();

        EXPECT_EQ(10, model.number_of_fixed_variables());
    }
}

/*****************************************************************************/
TEST_F(TestPresolver,
       remove_redundant_constraints_with_tightening_variable_bounds) {
    {
        printemps::model::Model<int, double> model;

        auto& x = model.create_variable("x", 0, 10);
        auto& g = model.create_constraint("g", 3 * x + 1 == 7);

        printemps::model::
            remove_redundant_constraints_with_tightening_variable_bounds(&model,
                                                                         false);
        EXPECT_EQ(true, x.is_fixed());
        EXPECT_EQ(2, x.value());
        EXPECT_EQ(false, g.is_enabled());
    }
    {
        printemps::model::Model<int, double> model;

        auto& x = model.create_variable("x", 0, 10);
        auto& g = model.create_constraint("g", 3 * x + 1 <= 7);

        printemps::model::
            remove_redundant_constraints_with_tightening_variable_bounds(&model,
                                                                         false);
        EXPECT_EQ(false, x.is_fixed());
        EXPECT_EQ(0, x.lower_bound());
        EXPECT_EQ(2, x.upper_bound());
        EXPECT_EQ(false, g.is_enabled());
    }
    {
        printemps::model::Model<int, double> model;

        auto& x = model.create_variable("x", 0, 10);
        auto& g = model.create_constraint("g", 3 * x + 1 >= 7);

        printemps::model::
            remove_redundant_constraints_with_tightening_variable_bounds(&model,
                                                                         false);
        EXPECT_EQ(false, x.is_fixed());
        EXPECT_EQ(2, x.lower_bound());
        EXPECT_EQ(10, x.upper_bound());
        EXPECT_EQ(false, g.is_enabled());
    }

    {
        printemps::model::Model<int, double> model;

        auto& x = model.create_variable("x", -10, 10);
        auto& g = model.create_constraint("g", -3 * x + 1 == 7);

        printemps::model::
            remove_redundant_constraints_with_tightening_variable_bounds(&model,
                                                                         false);
        EXPECT_EQ(true, x.is_fixed());
        EXPECT_EQ(-2, x.value());
        EXPECT_EQ(false, g.is_enabled());
    }
    {
        printemps::model::Model<int, double> model;

        auto& x = model.create_variable("x", -10, 10);
        auto& g = model.create_constraint("g", -3 * x + 1 <= 7);

        printemps::model::
            remove_redundant_constraints_with_tightening_variable_bounds(&model,
                                                                         false);
        EXPECT_EQ(false, x.is_fixed());
        EXPECT_EQ(-2, x.lower_bound());
        EXPECT_EQ(10, x.upper_bound());
        EXPECT_EQ(false, g.is_enabled());
    }
    {
        printemps::model::Model<int, double> model;

        auto& x = model.create_variable("x", -10, 10);
        auto& g = model.create_constraint("g", -3 * x + 1 >= 7);

        printemps::model::
            remove_redundant_constraints_with_tightening_variable_bounds(&model,
                                                                         false);
        EXPECT_EQ(false, x.is_fixed());
        EXPECT_EQ(-10, x.lower_bound());
        EXPECT_EQ(-2, x.upper_bound());
        EXPECT_EQ(false, g.is_enabled());
    }

    {
        printemps::model::Model<int, double> model;

        auto& x = model.create_variable("x", 0, 10);
        auto& y = model.create_variable("y", 0, 1);
        auto& g = model.create_constraint("g", 3 * x + y == 7);
        y.fix_by(1);

        printemps::model::
            remove_redundant_constraints_with_tightening_variable_bounds(&model,
                                                                         false);
        EXPECT_EQ(true, x.is_fixed());
        EXPECT_EQ(2, x.value());
        EXPECT_EQ(false, g.is_enabled());
    }
    {
        printemps::model::Model<int, double> model;

        auto& x = model.create_variable("x", 0, 10);
        auto& y = model.create_variable("y", 0, 1);
        auto& g = model.create_constraint("g", 3 * x + y <= 7);
        y.fix_by(1);

        printemps::model::
            remove_redundant_constraints_with_tightening_variable_bounds(&model,
                                                                         false);
        EXPECT_EQ(false, x.is_fixed());
        EXPECT_EQ(0, x.lower_bound());
        EXPECT_EQ(2, x.upper_bound());
        EXPECT_EQ(false, g.is_enabled());
    }
    {
        printemps::model::Model<int, double> model;

        auto& x = model.create_variable("x", 0, 10);
        auto& y = model.create_variable("y", 0, 1);
        auto& g = model.create_constraint("g", 3 * x + y >= 7);
        y.fix_by(1);

        printemps::model::
            remove_redundant_constraints_with_tightening_variable_bounds(&model,
                                                                         false);
        EXPECT_EQ(false, x.is_fixed());
        EXPECT_EQ(2, x.lower_bound());
        EXPECT_EQ(10, x.upper_bound());
        EXPECT_EQ(false, g.is_enabled());
    }
    {
        printemps::model::Model<int, double> model;

        auto& x = model.create_variable("x", -10, 10);
        auto& y = model.create_variable("y", 0, 1);
        auto& g = model.create_constraint("g", -3 * x + y == 7);
        y.fix_by(1);

        printemps::model::
            remove_redundant_constraints_with_tightening_variable_bounds(&model,
                                                                         false);
        EXPECT_EQ(true, x.is_fixed());
        EXPECT_EQ(-2, x.value());
        EXPECT_EQ(false, g.is_enabled());
    }
    {
        printemps::model::Model<int, double> model;

        auto& x = model.create_variable("x", -10, 10);
        auto& y = model.create_variable("y", 0, 1);
        auto& g = model.create_constraint("g", -3 * x + y <= 7);
        y.fix_by(1);

        printemps::model::
            remove_redundant_constraints_with_tightening_variable_bounds(&model,
                                                                         false);
        EXPECT_EQ(false, x.is_fixed());
        EXPECT_EQ(-2, x.lower_bound());
        EXPECT_EQ(10, x.upper_bound());
        EXPECT_EQ(false, g.is_enabled());
    }
    {
        printemps::model::Model<int, double> model;

        auto& x = model.create_variable("x", -10, 10);
        auto& y = model.create_variable("y", 0, 1);
        auto& g = model.create_constraint("g", -3 * x + y >= 7);
        y.fix_by(1);

        printemps::model::
            remove_redundant_constraints_with_tightening_variable_bounds(&model,
                                                                         false);
        EXPECT_EQ(false, x.is_fixed());
        EXPECT_EQ(-10, x.lower_bound());
        EXPECT_EQ(-2, x.upper_bound());
        EXPECT_EQ(false, g.is_enabled());
    }

    {
        printemps::model::Model<int, double> model;

        auto& x = model.create_variable("x", 0, 10);
        auto& g = model.create_constraint("g", 3 * x + 1 == 7);
        x.fix_by(2);

        printemps::model::
            remove_redundant_constraints_with_tightening_variable_bounds(&model,
                                                                         false);
        EXPECT_EQ(true, x.is_fixed());
        EXPECT_EQ(false, g.is_enabled());
    }
    {
        printemps::model::Model<int, double> model;

        auto& x = model.create_variable("x", 0, 10);
        auto& g = model.create_constraint("g", 3 * x + 1 <= 7);
        x.fix_by(1);

        printemps::model::
            remove_redundant_constraints_with_tightening_variable_bounds(&model,
                                                                         false);
        EXPECT_EQ(true, x.is_fixed());
        EXPECT_EQ(false, g.is_enabled());
    }
    {
        printemps::model::Model<int, double> model;

        auto& x = model.create_variable("x", 0, 10);
        auto& g = model.create_constraint("g", 3 * x + 1 >= 7);
        x.fix_by(3);

        printemps::model::
            remove_redundant_constraints_with_tightening_variable_bounds(&model,
                                                                         false);
        EXPECT_EQ(true, x.is_fixed());
        EXPECT_EQ(false, g.is_enabled());
    }

    {
        printemps::model::Model<int, double> model;

        auto& x = model.create_variable("x", -10, 10);
        auto& g = model.create_constraint("g", -3 * x + 1 == 7);
        x.fix_by(-2);

        printemps::model::
            remove_redundant_constraints_with_tightening_variable_bounds(&model,
                                                                         false);
        EXPECT_EQ(true, x.is_fixed());
        EXPECT_EQ(false, g.is_enabled());
    }
    {
        printemps::model::Model<int, double> model;

        auto& x = model.create_variable("x", -10, 10);
        auto& g = model.create_constraint("g", -3 * x + 1 <= 7);
        x.fix_by(-2);

        printemps::model::
            remove_redundant_constraints_with_tightening_variable_bounds(&model,
                                                                         false);
        EXPECT_EQ(true, x.is_fixed());
        EXPECT_EQ(false, g.is_enabled());
    }
    {
        printemps::model::Model<int, double> model;

        auto& x = model.create_variable("x", -10, 10);
        auto& g = model.create_constraint("g", -3 * x + 1 >= 7);
        x.fix_by(-2);

        printemps::model::
            remove_redundant_constraints_with_tightening_variable_bounds(&model,
                                                                         false);
        EXPECT_EQ(true, x.is_fixed());
        EXPECT_EQ(false, g.is_enabled());
    }

    {
        printemps::model::Model<int, double> model;

        auto& x = model.create_variable("x", 0, 10);
        auto& y = model.create_variable("y", 0, 1);
        auto& g = model.create_constraint("g", 3 * x + y <= 7);

        printemps::model::
            remove_redundant_constraints_with_tightening_variable_bounds(&model,
                                                                         false);
        EXPECT_EQ(false, x.is_fixed());
        EXPECT_EQ(0, x.lower_bound());
        EXPECT_EQ(2, x.upper_bound());
        EXPECT_EQ(true, g.is_enabled());
    }
    {
        printemps::model::Model<int, double> model;

        auto& x = model.create_variable("x", 0, 10);
        auto& y = model.create_variable("y", 0, 1);
        auto& g = model.create_constraint("g", 3 * x + y >= 7);

        printemps::model::
            remove_redundant_constraints_with_tightening_variable_bounds(&model,
                                                                         false);
        EXPECT_EQ(false, x.is_fixed());
        EXPECT_EQ(2, x.lower_bound());
        EXPECT_EQ(10, x.upper_bound());
        EXPECT_EQ(true, g.is_enabled());
    }

    {
        printemps::model::Model<int, double> model;

        auto& x = model.create_variable("x", -10, 10);
        auto& y = model.create_variable("y", 0, 1);
        auto& g = model.create_constraint("g", -3 * x + y <= 7);

        printemps::model::
            remove_redundant_constraints_with_tightening_variable_bounds(&model,
                                                                         false);
        EXPECT_EQ(false, x.is_fixed());
        EXPECT_EQ(-2, x.lower_bound());
        EXPECT_EQ(10, x.upper_bound());
        EXPECT_EQ(true, g.is_enabled());
    }
    {
        printemps::model::Model<int, double> model;

        auto& x = model.create_variable("x", -10, 10);
        auto& y = model.create_variable("y", 0, 1);
        auto& g = model.create_constraint("g", -3 * x + y >= 7);

        printemps::model::
            remove_redundant_constraints_with_tightening_variable_bounds(&model,
                                                                         false);
        EXPECT_EQ(false, x.is_fixed());
        EXPECT_EQ(-10, x.lower_bound());
        EXPECT_EQ(-2, x.upper_bound());
        EXPECT_EQ(true, g.is_enabled());
    }
}

/*****************************************************************************/
TEST_F(TestPresolver, fix_implicit_fixed_variables) {
    printemps::model::Model<int, double> model;

    auto& x = model.create_variables("x", 10, -10, 10);
    x(0).set_bound(5, 5);
    printemps::model::fix_implicit_fixed_variables(&model, false);
    EXPECT_EQ(5, x(0).value());
    EXPECT_EQ(true, x(0).is_fixed());

    for (auto i = 1; i < 10; i++) {
        EXPECT_EQ(false, x(i).is_fixed());
    }
    model.categorize_variables();
    model.categorize_constraints();

    EXPECT_EQ(1, model.number_of_fixed_variables());
}

/*****************************************************************************/
TEST_F(TestPresolver, fix_redundant_variables) {
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

        model.setup_unique_name();
        model.setup_variable_related_constraints();
        model.setup_is_linear();
        model.setup_variable_sensitivity();

        model.categorize_variables();
        model.categorize_constraints();
        printemps::model::fix_redundant_variables(&model, false);
        model.categorize_variables();
        model.categorize_constraints();

        EXPECT_EQ(true, x(3).is_fixed());
        EXPECT_EQ(true, x(4).is_fixed());
        EXPECT_EQ(true, x(8).is_fixed());
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

        model.setup_unique_name();
        model.setup_variable_related_constraints();
        model.setup_is_linear();
        model.setup_variable_sensitivity();

        model.categorize_variables();
        model.categorize_constraints();
        printemps::model::fix_redundant_variables(&model, false);
        model.categorize_variables();
        model.categorize_constraints();

        EXPECT_EQ(true, x(4).is_fixed());
        EXPECT_EQ(true, x(5).is_fixed());
        EXPECT_EQ(true, x(6).is_fixed());
        EXPECT_EQ(true, x(7).is_fixed());
    }
}
/*****************************************************************************/
}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/