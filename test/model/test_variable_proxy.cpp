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
class TestVariableProxy : public ::testing::Test {
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
TEST_F(TestVariableProxy, scalar_create_instance) {
    printemps::model::Model<int, double> model;
    auto& variable_proxy = model.create_variable("x");

    /// Check the initial values of the base class members.
    EXPECT_EQ(0, variable_proxy.index());
    EXPECT_EQ(1, variable_proxy.shape()[0]);
    EXPECT_EQ(1, variable_proxy.strides()[0]);
    EXPECT_EQ(1, variable_proxy.number_of_dimensions());
    EXPECT_EQ(1, variable_proxy.number_of_elements());
}

/*****************************************************************************/
TEST_F(TestVariableProxy, scalar_value) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy = model.create_variable("x");
    auto  value          = random_integer();
    variable_proxy       = value;
    EXPECT_EQ(value, variable_proxy.value());
}

/*****************************************************************************/
TEST_F(TestVariableProxy, scalar_set_value) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy = model.create_variable("x");
    auto  value          = random_integer();
    variable_proxy.set_value(value);
    EXPECT_EQ(value, variable_proxy.value());
}

/*****************************************************************************/
TEST_F(TestVariableProxy, scalar_evaluate_arg_void) {
    printemps::model::Model<int, double> model;
    auto& variable_proxy = model.create_variable("x");

    auto value     = random_integer();
    variable_proxy = value;
    EXPECT_EQ(value, variable_proxy.evaluate());
}

/*****************************************************************************/
TEST_F(TestVariableProxy, scalar_evaluate_arg_move) {
    printemps::model::Model<int, double> model;
    auto& variable_proxy_0 = model.create_variable("x_0");
    auto& variable_proxy_1 = model.create_variable("x_1");

    auto value_0_before = random_integer();
    auto value_1_before = random_integer();
    auto value_0_after  = random_integer();
    auto value_1_after  = random_integer();

    variable_proxy_0 = value_0_before;
    variable_proxy_1 = value_1_before;

    printemps::neighborhood::Move<int, double> move_0;
    printemps::neighborhood::Move<int, double> move_1;
    move_0.alterations.emplace_back(&variable_proxy_0[0], value_0_after);
    move_1.alterations.emplace_back(&variable_proxy_1[0], value_1_after);

    EXPECT_EQ(value_0_after, variable_proxy_0.evaluate(move_0));
    EXPECT_EQ(value_0_before, variable_proxy_0.evaluate(move_1));
    EXPECT_EQ(value_1_after, variable_proxy_1.evaluate(move_1));
    EXPECT_EQ(value_1_before, variable_proxy_1.evaluate(move_0));
}

/*****************************************************************************/
TEST_F(TestVariableProxy, scalar_fix) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy = model.create_variable("x");
    variable_proxy.fix();
    EXPECT_TRUE(variable_proxy.is_fixed());
}

/*****************************************************************************/
TEST_F(TestVariableProxy, scalar_is_fixed) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy = model.create_variable("x");
    variable_proxy.unfix();
    EXPECT_FALSE(variable_proxy.is_fixed());
}

/*****************************************************************************/
TEST_F(TestVariableProxy, scalar_unfix) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy = model.create_variable("x");
    variable_proxy.fix();
    EXPECT_TRUE(variable_proxy.is_fixed());
    variable_proxy.unfix();
    EXPECT_FALSE(variable_proxy.is_fixed());
}

/*****************************************************************************/
TEST_F(TestVariableProxy, scalar_fix_by) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy = model.create_variable("x");
    auto  value          = random_integer();
    variable_proxy.fix_by(value);
    EXPECT_TRUE(variable_proxy.is_fixed());
    EXPECT_EQ(value, variable_proxy.value());
}

/*****************************************************************************/
TEST_F(TestVariableProxy, scalar_sense) {
    {
        printemps::model::Model<int, double> model;

        auto& variable_proxy = model.create_variable("x");
        EXPECT_EQ(printemps::model::VariableSense::Integer,
                  variable_proxy.sense());
    }
    {
        printemps::model::Model<int, double> model;

        auto& variable_proxy = model.create_variable("x", 0, 1);
        EXPECT_EQ(printemps::model::VariableSense::Binary,
                  variable_proxy.sense());
    }
    {
        printemps::model::Model<int, double> model;

        auto& variable_proxy = model.create_variable("x", 0, 10);
        EXPECT_EQ(printemps::model::VariableSense::Integer,
                  variable_proxy.sense());
    }
}

/*****************************************************************************/
TEST_F(TestVariableProxy, scaler_set_bound) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy = model.create_variable("x");

    auto lower_bound = random_integer();
    auto upper_bound = lower_bound + random_positive_integer();

    variable_proxy.set_bound(lower_bound, upper_bound);

    EXPECT_EQ(lower_bound, variable_proxy.lower_bound());
    EXPECT_EQ(upper_bound, variable_proxy.upper_bound());
    EXPECT_TRUE(variable_proxy.has_bounds());

    ASSERT_THROW(variable_proxy.set_bound(upper_bound, lower_bound),
                 std::logic_error);
}

/*****************************************************************************/
TEST_F(TestVariableProxy, scaler_lower_bound) {
    /// This method is tested in scaler_set_bound().
}

/*****************************************************************************/
TEST_F(TestVariableProxy, scaler_upper_bound) {
    /// This method is tested in scaler_set_bound().
}

/*****************************************************************************/
TEST_F(TestVariableProxy, scaler_has_bounds) {
    /// This method is tested in scaler_set_bound().
}

/*****************************************************************************/
TEST_F(TestVariableProxy, scalar_set_name) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy = model.create_variable("x");
    variable_proxy.set_name("_x");
    EXPECT_EQ("_x", variable_proxy.name());
}

/*****************************************************************************/
TEST_F(TestVariableProxy, scalar_name) {
    /// This method is tested in scalar_set_name().
}

/*****************************************************************************/
TEST_F(TestVariableProxy, scalar_flat_indexed_variables_arg_void) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy                       = model.create_variable("x");
    auto  value                                = random_integer();
    variable_proxy.flat_indexed_variables()[0] = value;
    EXPECT_EQ(value, variable_proxy.flat_indexed_variables()[0].value());
}

/*****************************************************************************/
TEST_F(TestVariableProxy, scalar_flat_indexed_variables_arg_int) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy                     = model.create_variable("x");
    auto  value                              = random_integer();
    variable_proxy.flat_indexed_variables(0) = value;
    EXPECT_EQ(value, variable_proxy.flat_indexed_variables(0).value());
}

/*****************************************************************************/
TEST_F(TestVariableProxy, scalar_export_values) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy = model.create_variable("x");
    auto  value          = random_integer();
    variable_proxy       = value;
    EXPECT_EQ(value, variable_proxy.export_values_and_names().value());
}

/*****************************************************************************/
TEST_F(TestVariableProxy, scalar_to_expression) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy = model.create_variable("x");
    EXPECT_EQ(1, variable_proxy.to_expression().sensitivities().at(
                     &(variable_proxy[0])));
}

/*****************************************************************************/
TEST_F(TestVariableProxy, scalar_sum_arg_void) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy = model.create_variable("x");
    auto  expression =
        printemps::model::Expression<int, double>::create_instance();
    expression = variable_proxy.sum();

    for (auto i = 0; i < variable_proxy.number_of_elements(); i++) {
        variable_proxy[i] = 1;
    }

    EXPECT_EQ(1, expression.sensitivities().at(&(variable_proxy[0])));
    EXPECT_EQ(1, expression.evaluate());
}

/*****************************************************************************/
TEST_F(TestVariableProxy, scalar_sum_arg_indices) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy = model.create_variable("x");
    auto  expression =
        printemps::model::Expression<int, double>::create_instance();
    expression = variable_proxy.sum({printemps::model::Range::All});

    for (auto i = 0; i < variable_proxy.number_of_elements(); i++) {
        variable_proxy[i] = 1;
    }

    EXPECT_EQ(1, expression.sensitivities().at(&(variable_proxy[0])));
    EXPECT_EQ(1, expression.evaluate());

    ASSERT_THROW(variable_proxy.sum({printemps::model::Range::All, 0}),
                 std::logic_error);
}

/*****************************************************************************/
TEST_F(TestVariableProxy, scalar_dot_arg_vector) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy = model.create_variable("x");

    std::vector<double> sensitivities;
    sensitivities.push_back(random_integer());

    auto expression =
        printemps::model::Expression<int, double>::create_instance();
    expression = variable_proxy.dot(sensitivities);

    for (auto i = 0; i < variable_proxy.number_of_elements(); i++) {
        variable_proxy[i] = 1;
    }

    EXPECT_EQ(sensitivities[0],
              expression.sensitivities().at(&(variable_proxy[0])));
    EXPECT_EQ(sensitivities[0], expression.evaluate());
}

/*****************************************************************************/
TEST_F(TestVariableProxy, scalar_dot_arg_indice_vector) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy = model.create_variable("x");

    std::vector<double> sensitivities;
    sensitivities.push_back(random_integer());

    auto expression =
        printemps::model::Expression<int, double>::create_instance();
    expression =
        variable_proxy.dot({printemps::model::Range::All}, sensitivities);

    for (auto i = 0; i < variable_proxy.number_of_elements(); i++) {
        variable_proxy[i] = 1;
    }

    EXPECT_EQ(sensitivities[0],
              expression.sensitivities().at(&(variable_proxy[0])));
    EXPECT_EQ(sensitivities[0], expression.evaluate());

    ASSERT_THROW(
        variable_proxy.dot({0, printemps::model::Range::All}, sensitivities),
        std::logic_error);
}

/*****************************************************************************/
TEST_F(TestVariableProxy, scalar_selection_arg_void) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy = model.create_variable("x");
    auto  constraint =
        printemps::model::Constraint<int, double>::create_instance();

    constraint = variable_proxy.selection();

    EXPECT_EQ(1,
              constraint.expression().sensitivities().at(&(variable_proxy[0])));
    EXPECT_EQ(printemps::model::ConstraintSense::Equal, constraint.sense());
}

/*****************************************************************************/
TEST_F(TestVariableProxy, scalar_selection_arg_indices) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy = model.create_variable("x");
    auto  constraint =
        printemps::model::Constraint<int, double>::create_instance();

    constraint = variable_proxy.selection();

    EXPECT_EQ(1,
              constraint.expression().sensitivities().at(&(variable_proxy[0])));
    EXPECT_EQ(printemps::model::ConstraintSense::Equal, constraint.sense());
    ASSERT_THROW(variable_proxy.selection({printemps::model::Range::All, 0}),
                 std::logic_error);
}

/*****************************************************************************/
TEST_F(TestVariableProxy, scalar_operator_plus) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy = model.create_variable("x");
    EXPECT_EQ(1, (+variable_proxy).sensitivities().at(&(variable_proxy[0])));
}

/*****************************************************************************/
TEST_F(TestVariableProxy, scalar_operator_minus) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy = model.create_variable("x");
    EXPECT_EQ(-1, (-variable_proxy).sensitivities().at(&(variable_proxy[0])));
}

/*****************************************************************************/
TEST_F(TestVariableProxy, scalar_operator_equal_arg_t_variable) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy = model.create_variable("x");
    auto  value          = random_integer();

    EXPECT_EQ(value, (variable_proxy = value).value());
    EXPECT_EQ(value, variable_proxy.value());
}

/*****************************************************************************/
TEST_F(TestVariableProxy, scalar_operator_square_bracket) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy = model.create_variable("x");
    auto  value          = random_integer();
    variable_proxy[0]    = value;
    EXPECT_EQ(value, variable_proxy[0].value());
}

/*****************************************************************************/
TEST_F(TestVariableProxy, scalar_operator_round_bracket) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy = model.create_variable("x");
    auto  value          = random_integer();
    variable_proxy(0)    = value;
    EXPECT_EQ(value, variable_proxy(0).value());
}

/*****************************************************************************/
TEST_F(TestVariableProxy, scalar_operator_round_bracket_with_indices) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy = model.create_variable("x");
    auto  value          = random_integer();
    variable_proxy({0})  = value;
    EXPECT_EQ(value, variable_proxy({0}).value());
}

/*****************************************************************************/
TEST_F(TestVariableProxy, one_dimensional_create_instance) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy = model.create_variables("x", 2);

    /// Check the initial values of the base class members.
    EXPECT_EQ(0, variable_proxy.index());
    EXPECT_EQ(2, variable_proxy.shape()[0]);
    EXPECT_EQ(1, variable_proxy.strides()[0]);
    EXPECT_EQ(1, variable_proxy.number_of_dimensions());
    EXPECT_EQ(2, variable_proxy.number_of_elements());
}

/*****************************************************************************/
TEST_F(TestVariableProxy, one_dimensional_value) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy = model.create_variables("x", 2);
    auto  value          = random_integer();
    ASSERT_THROW(variable_proxy = value, std::logic_error);
}

/*****************************************************************************/
TEST_F(TestVariableProxy, one_dimensional_set_value) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy = model.create_variables("x", 2);
    auto  value          = random_integer();
    ASSERT_THROW(variable_proxy.set_value(value), std::logic_error);
}

/*****************************************************************************/
TEST_F(TestVariableProxy, one_dimensional_evaluate_arg_void) {
    printemps::model::Model<int, double> model;
    auto& variable_proxy = model.create_variables("x", 2);
    ASSERT_THROW(variable_proxy.evaluate(), std::logic_error);
}

/*****************************************************************************/
TEST_F(TestVariableProxy, one_dimensional_evaluate_arg_move) {
    printemps::model::Model<int, double> model;
    auto& variable_proxy = model.create_variables("x", 2);
    ASSERT_THROW(variable_proxy.evaluate({}), std::logic_error);
}

/*****************************************************************************/
TEST_F(TestVariableProxy, one_dimensional_fix) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy = model.create_variables("x", 2);
    ASSERT_THROW(variable_proxy.fix(), std::logic_error);
}

/*****************************************************************************/
TEST_F(TestVariableProxy, one_dimensional_is_fixed) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy = model.create_variables("x", 2);
    ASSERT_THROW(variable_proxy.is_fixed(), std::logic_error);
}

/*****************************************************************************/
TEST_F(TestVariableProxy, one_dimensional_unfix) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy = model.create_variables("x", 2);
    ASSERT_THROW(variable_proxy.unfix(), std::logic_error);
}

/*****************************************************************************/
TEST_F(TestVariableProxy, one_dimensional_sense) {
    {
        printemps::model::Model<int, double> model;

        auto& variable_proxy = model.create_variables("x", 2, 0, 1);
        ASSERT_THROW(variable_proxy.sense(), std::logic_error);
    }
    {
        printemps::model::Model<int, double> model;

        auto& variable_proxy = model.create_variables("x", 2, 0, 10);
        ASSERT_THROW(variable_proxy.sense(), std::logic_error);
    }
}

/*****************************************************************************/
TEST_F(TestVariableProxy, one_dimensional_fix_by) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy = model.create_variables("x", 2);
    auto  value          = random_integer();
    ASSERT_THROW(variable_proxy.fix_by(value), std::logic_error);
}

/*****************************************************************************/
TEST_F(TestVariableProxy, one_dimensional_set_bound) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy = model.create_variables("x", 2);

    auto lower_bound = random_integer();
    auto upper_bound = lower_bound + random_positive_integer();

    variable_proxy.set_bound(lower_bound, upper_bound);

    EXPECT_EQ(lower_bound, variable_proxy[0].lower_bound());
    EXPECT_EQ(upper_bound, variable_proxy[0].upper_bound());
    EXPECT_TRUE(variable_proxy[0].has_bounds());
    EXPECT_EQ(lower_bound, variable_proxy[1].lower_bound());
    EXPECT_EQ(upper_bound, variable_proxy[1].upper_bound());
    EXPECT_TRUE(variable_proxy[0].has_bounds());

    ASSERT_THROW(variable_proxy.lower_bound(), std::logic_error);
    ASSERT_THROW(variable_proxy.upper_bound(), std::logic_error);
    ASSERT_THROW(variable_proxy.has_bounds(), std::logic_error);
    ASSERT_THROW(variable_proxy.set_bound(upper_bound, lower_bound),
                 std::logic_error);
}

/*****************************************************************************/
TEST_F(TestVariableProxy, one_dimensional_lower_bound) {
    /// This method is tested in scaler_set_bound.
}

/*****************************************************************************/
TEST_F(TestVariableProxy, one_dimensional_upper_bound) {
    /// This method is tested in scaler_set_bound.
}

/*****************************************************************************/
TEST_F(TestVariableProxy, one_dimensional_has_bounds) {
    /// This method is tested in one_dimensional_set_bound.
}

/*****************************************************************************/
TEST_F(TestVariableProxy, one_dimensional_set_name) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy = model.create_variables("x", 2);

    ASSERT_THROW(variable_proxy.set_name("_x"), std::logic_error);
    ASSERT_THROW(variable_proxy.name(), std::logic_error);
}

/*****************************************************************************/
TEST_F(TestVariableProxy, one_dimensional_name) {
    /// This method is tested in one_dimensional_set_name().
}

/*****************************************************************************/
TEST_F(TestVariableProxy, one_dimensional_flat_indexed_variables_arg_void) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy                       = model.create_variables("x", 2);
    auto  value_0                              = random_integer();
    auto  value_1                              = random_integer();
    variable_proxy.flat_indexed_variables()[0] = value_0;
    variable_proxy.flat_indexed_variables()[1] = value_1;
    EXPECT_EQ(value_0, variable_proxy.flat_indexed_variables()[0].value());
    EXPECT_EQ(value_1, variable_proxy.flat_indexed_variables()[1].value());
}

/*****************************************************************************/
TEST_F(TestVariableProxy, one_dimensional_flat_indexed_variables_arg_int) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy                     = model.create_variables("x", 2);
    auto  value_0                            = random_integer();
    auto  value_1                            = random_integer();
    variable_proxy.flat_indexed_variables(0) = value_0;
    variable_proxy.flat_indexed_variables(1) = value_1;
    EXPECT_EQ(value_0, variable_proxy.flat_indexed_variables(0).value());
    EXPECT_EQ(value_1, variable_proxy.flat_indexed_variables(1).value());
}

/*****************************************************************************/
TEST_F(TestVariableProxy, one_dimensional_export_values) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy = model.create_variables("x", 2);
    auto  value_0        = random_integer();
    auto  value_1        = random_integer();
    variable_proxy[0]    = value_0;
    variable_proxy[1]    = value_1;
    EXPECT_EQ(value_0, variable_proxy.export_values_and_names().values(0));
    EXPECT_EQ(value_1, variable_proxy.export_values_and_names().values(1));
}

/*****************************************************************************/
TEST_F(TestVariableProxy, one_dimensional_to_expression) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy = model.create_variables("x", 2);
    ASSERT_THROW(variable_proxy.to_expression(), std::logic_error);
}

/*****************************************************************************/
TEST_F(TestVariableProxy, one_dimensional_sum_arg_void) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy = model.create_variables("x", 2);
    auto  expression =
        printemps::model::Expression<int, double>::create_instance();

    expression = variable_proxy.sum();

    for (auto i = 0; i < variable_proxy.number_of_elements(); i++) {
        variable_proxy[i] = 1;
    }

    EXPECT_EQ(1, expression.sensitivities().at(&(variable_proxy[0])));
    EXPECT_EQ(1, expression.sensitivities().at(&(variable_proxy[1])));
    EXPECT_EQ(2, expression.evaluate());
}

/*****************************************************************************/
TEST_F(TestVariableProxy, one_dimensional_sum_arg_indices) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy = model.create_variables("x", 2);
    auto  expression =
        printemps::model::Expression<int, double>::create_instance();

    expression = variable_proxy.sum({printemps::model::Range::All});

    for (auto i = 0; i < variable_proxy.number_of_elements(); i++) {
        variable_proxy[i] = 1;
    }

    EXPECT_EQ(1, expression.sensitivities().at(&(variable_proxy[0])));
    EXPECT_EQ(1, expression.sensitivities().at(&(variable_proxy[1])));
    EXPECT_EQ(2, expression.evaluate());
    ASSERT_THROW(variable_proxy.sum({printemps::model::Range::All, 0}),
                 std::logic_error);
}

/*****************************************************************************/
TEST_F(TestVariableProxy, one_dimensional_dot_arg_vector) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy = model.create_variables("x", 2);

    std::vector<double> sensitivities;
    for (auto i = 0; i < variable_proxy.number_of_elements(); i++) {
        sensitivities.push_back(random_integer());
    }

    auto expression =
        printemps::model::Expression<int, double>::create_instance();
    expression = variable_proxy.dot(sensitivities);

    for (auto i = 0; i < variable_proxy.number_of_elements(); i++) {
        variable_proxy[i] = 1;
    }

    EXPECT_EQ(sensitivities[0],
              expression.sensitivities().at(&(variable_proxy[0])));
    EXPECT_EQ(sensitivities[1],
              expression.sensitivities().at(&(variable_proxy[1])));
    EXPECT_EQ(sensitivities[0] + sensitivities[1], expression.evaluate());
}

/*****************************************************************************/
TEST_F(TestVariableProxy, one_dimensional_dot_arg_indice_vector) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy = model.create_variables("x", 2);

    std::vector<double> sensitivities;
    for (auto i = 0; i < variable_proxy.number_of_elements(); i++) {
        sensitivities.push_back(random_integer());
    }

    auto expression =
        printemps::model::Expression<int, double>::create_instance();
    expression = variable_proxy.dot(sensitivities);

    for (auto i = 0; i < variable_proxy.number_of_elements(); i++) {
        variable_proxy[i] = 1;
    }

    EXPECT_EQ(sensitivities[0],
              expression.sensitivities().at(&(variable_proxy[0])));
    EXPECT_EQ(sensitivities[1],
              expression.sensitivities().at(&(variable_proxy[1])));
    EXPECT_EQ(sensitivities[0] + sensitivities[1], expression.evaluate());

    ASSERT_THROW(
        variable_proxy.dot({0, printemps::model::Range::All}, sensitivities),
        std::logic_error);
}

/*****************************************************************************/
TEST_F(TestVariableProxy, one_dimensional_selection_arg_void) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy = model.create_variables("x", 2);
    auto  constraint =
        printemps::model::Constraint<int, double>::create_instance();

    constraint = variable_proxy.selection();

    EXPECT_EQ(1,
              constraint.expression().sensitivities().at(&(variable_proxy[0])));
    EXPECT_EQ(1,
              constraint.expression().sensitivities().at(&(variable_proxy[1])));
    EXPECT_EQ(printemps::model::ConstraintSense::Equal, constraint.sense());
}

/*****************************************************************************/
TEST_F(TestVariableProxy, one_dimensional_selection_arg_indices) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy = model.create_variables("x", 2);
    auto  constraint =
        printemps::model::Constraint<int, double>::create_instance();

    constraint = variable_proxy.selection();

    EXPECT_EQ(1,
              constraint.expression().sensitivities().at(&(variable_proxy[0])));
    EXPECT_EQ(1,
              constraint.expression().sensitivities().at(&(variable_proxy[1])));
    EXPECT_EQ(printemps::model::ConstraintSense::Equal, constraint.sense());
    ASSERT_THROW(variable_proxy.selection({printemps::model::Range::All, 0}),
                 std::logic_error);
}

/*****************************************************************************/
TEST_F(TestVariableProxy, one_dimensional_operator_plus) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy = model.create_variables("x", 2);
    ASSERT_THROW(+variable_proxy, std::logic_error);
}

/*****************************************************************************/
TEST_F(TestVariableProxy, one_dimensional_operator_minus) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy = model.create_variables("x", 2);
    ASSERT_THROW(-variable_proxy, std::logic_error);
}

/*****************************************************************************/
TEST_F(TestVariableProxy, one_dimensional_operator_equal_arg_t_variable) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy = model.create_variables("x", 2);
    auto  value          = random_integer();
    ASSERT_THROW(variable_proxy = value, std::logic_error);
}

/*****************************************************************************/
TEST_F(TestVariableProxy, one_dimensional_operator_square_bracket) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy = model.create_variables("x", 2);
    auto  value_0        = random_integer();
    auto  value_1        = random_integer();
    variable_proxy[0]    = value_0;
    variable_proxy[1]    = value_1;
    EXPECT_EQ(value_0, variable_proxy[0].value());
    EXPECT_EQ(value_1, variable_proxy[1].value());
}

/*****************************************************************************/
TEST_F(TestVariableProxy, one_dimensional_operator_round_bracket) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy = model.create_variables("x", 2);
    auto  value_0        = random_integer();
    auto  value_1        = random_integer();
    variable_proxy(0)    = value_0;
    variable_proxy(1)    = value_1;
    EXPECT_EQ(value_0, variable_proxy(0).value());
    EXPECT_EQ(value_1, variable_proxy(1).value());
}

/*****************************************************************************/
TEST_F(TestVariableProxy, one_dimensional_operator_round_bracket_with_indices) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy = model.create_variables("x", 2);
    auto  value_0        = random_integer();
    auto  value_1        = random_integer();
    variable_proxy({0})  = value_0;
    variable_proxy({1})  = value_1;
    EXPECT_EQ(value_0, variable_proxy({0}).value());
    EXPECT_EQ(value_1, variable_proxy({1}).value());
}

/*****************************************************************************/
TEST_F(TestVariableProxy, two_dimensional_create_instance) {
    printemps::model::Model<int, double> model;
    auto& variable_proxy = model.create_variables("x", {2, 3});

    /// Check the initial values of the base class members.
    EXPECT_EQ(0, variable_proxy.index());
    EXPECT_EQ(2, variable_proxy.shape()[0]);
    EXPECT_EQ(3, variable_proxy.shape()[1]);
    EXPECT_EQ(3, variable_proxy.strides()[0]);
    EXPECT_EQ(1, variable_proxy.strides()[1]);
    EXPECT_EQ(2, variable_proxy.number_of_dimensions());
    EXPECT_EQ(2 * 3, variable_proxy.number_of_elements());
}

/*****************************************************************************/
TEST_F(TestVariableProxy, two_dimensional_value) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy = model.create_variables("x", {2, 3});
    auto  value          = random_integer();
    ASSERT_THROW(variable_proxy = value, std::logic_error);
}

/*****************************************************************************/
TEST_F(TestVariableProxy, two_dimensional_set_value) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy = model.create_variables("x", {2, 3});
    auto  value          = random_integer();
    ASSERT_THROW(variable_proxy.set_value(value), std::logic_error);
}

/*****************************************************************************/
TEST_F(TestVariableProxy, two_evaluate_arg_void) {
    printemps::model::Model<int, double> model;
    auto& variable_proxy = model.create_variables("x", {2, 3});
    ASSERT_THROW(variable_proxy.evaluate(), std::logic_error);
}

/*****************************************************************************/
TEST_F(TestVariableProxy, two_evaluate_arg_move) {
    printemps::model::Model<int, double> model;
    auto& variable_proxy = model.create_variables("x", {2, 3});
    ASSERT_THROW(variable_proxy.evaluate({}), std::logic_error);
}

/*****************************************************************************/
TEST_F(TestVariableProxy, two_dimensional_fix) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy = model.create_variables("x", {2, 3});
    ASSERT_THROW(variable_proxy.fix(), std::logic_error);
}

/*****************************************************************************/
TEST_F(TestVariableProxy, two_dimensional_is_fixed) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy = model.create_variables("x", {2, 3});
    ASSERT_THROW(variable_proxy.is_fixed(), std::logic_error);
}

/*****************************************************************************/
TEST_F(TestVariableProxy, two_dimensional_unfix) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy = model.create_variables("x", {2, 3});
    ASSERT_THROW(variable_proxy.unfix(), std::logic_error);
}

/*****************************************************************************/
TEST_F(TestVariableProxy, two_dimensional_fix_by) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy = model.create_variables("x", {2, 3});
    auto  value          = random_integer();
    ASSERT_THROW(variable_proxy.fix_by(value), std::logic_error);
}

/*****************************************************************************/
TEST_F(TestVariableProxy, two_dimensional_sense) {
    {
        printemps::model::Model<int, double> model;

        auto& variable_proxy = model.create_variables("x", {2, 3}, 0, 1);
        ASSERT_THROW(variable_proxy.sense(), std::logic_error);
    }
    {
        printemps::model::Model<int, double> model;

        auto& variable_proxy = model.create_variables("x", {2, 3}, 0, 10);
        ASSERT_THROW(variable_proxy.sense(), std::logic_error);
    }
}

/*****************************************************************************/
TEST_F(TestVariableProxy, two_dimensional_set_bound) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy = model.create_variables("x", {2, 3});

    auto lower_bound = random_integer();
    auto upper_bound = lower_bound + random_positive_integer();

    variable_proxy.set_bound(lower_bound, upper_bound);

    EXPECT_EQ(lower_bound, variable_proxy[0].lower_bound());
    EXPECT_EQ(upper_bound, variable_proxy[0].upper_bound());
    EXPECT_TRUE(variable_proxy[0].has_bounds());
    EXPECT_EQ(lower_bound, variable_proxy[2 * 3 - 1].lower_bound());
    EXPECT_EQ(upper_bound, variable_proxy[2 * 3 - 1].upper_bound());
    EXPECT_TRUE(variable_proxy[2 * 3 - 1].has_bounds());

    ASSERT_THROW(variable_proxy.lower_bound(), std::logic_error);
    ASSERT_THROW(variable_proxy.upper_bound(), std::logic_error);
    ASSERT_THROW(variable_proxy.has_bounds(), std::logic_error);
    ASSERT_THROW(variable_proxy.set_bound(upper_bound, lower_bound),
                 std::logic_error);
}

/*****************************************************************************/
TEST_F(TestVariableProxy, two_dimensional_lower_bound) {
    /// This method is tested in two_dimensional_set_bound.
}

/*****************************************************************************/
TEST_F(TestVariableProxy, two_dimensional_upper_bound) {
    /// This method is tested in two_dimensional_set_bound.
}

/*****************************************************************************/
TEST_F(TestVariableProxy, two_dimensional_has_bounds) {
    /// This method is tested in two_dimensional_set_bound.
}

/*****************************************************************************/
TEST_F(TestVariableProxy, two_dimensional_set_name) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy = model.create_variables("x", {2, 3});

    ASSERT_THROW(variable_proxy.set_name("_x"), std::logic_error);
    ASSERT_THROW(variable_proxy.name(), std::logic_error);
}

/*****************************************************************************/
TEST_F(TestVariableProxy, two_dimensional_name) {
    /// This method is tested in two_dimensional_set_name().
}

/*****************************************************************************/
TEST_F(TestVariableProxy, two_dimensional_flat_indexed_variables_arg_void) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy = model.create_variables("x", {2, 3});
    auto  value_0        = random_integer();
    auto  value_1        = random_integer();
    variable_proxy.flat_indexed_variables()[0]         = value_0;
    variable_proxy.flat_indexed_variables()[2 * 3 - 1] = value_1;
    EXPECT_EQ(value_0, variable_proxy.flat_indexed_variables()[0].value());
    EXPECT_EQ(value_1,
              variable_proxy.flat_indexed_variables()[2 * 3 - 1].value());
}

/*****************************************************************************/
TEST_F(TestVariableProxy, two_dimensional_flat_indexed_variables_arg_int) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy = model.create_variables("x", {2, 3});
    auto  value_0        = random_integer();
    auto  value_1        = random_integer();
    variable_proxy.flat_indexed_variables(0)         = value_0;
    variable_proxy.flat_indexed_variables(2 * 3 - 1) = value_1;
    EXPECT_EQ(value_0, variable_proxy.flat_indexed_variables(0).value());
    EXPECT_EQ(value_1,
              variable_proxy.flat_indexed_variables(2 * 3 - 1).value());
}

/*****************************************************************************/
TEST_F(TestVariableProxy, two_dimensional_export_values) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy      = model.create_variables("x", {2, 3});
    auto  value_0             = random_integer();
    auto  value_1             = random_integer();
    variable_proxy[0]         = value_0;
    variable_proxy[2 * 3 - 1] = value_1;
    EXPECT_EQ(value_0, variable_proxy.export_values_and_names().values(0, 0));
    EXPECT_EQ(value_1, variable_proxy.export_values_and_names().values(1, 2));
}

/*****************************************************************************/
TEST_F(TestVariableProxy, two_dimensional_to_expression) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy = model.create_variables("x", {2, 3});
    ASSERT_THROW(variable_proxy.to_expression(), std::logic_error);
}

/*****************************************************************************/
TEST_F(TestVariableProxy, two_dimensional_sum_arg_void) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy = model.create_variables("x", {2, 3});
    auto  expression =
        printemps::model::Expression<int, double>::create_instance();

    expression = variable_proxy.sum();

    for (auto i = 0; i < variable_proxy.number_of_elements(); i++) {
        variable_proxy[i] = 1;
    }

    EXPECT_EQ(1, expression.sensitivities().at(&(variable_proxy[0])));
    EXPECT_EQ(1, expression.sensitivities().at(&(variable_proxy[2 * 3 - 1])));
    EXPECT_EQ(2 * 3, expression.evaluate());
}

/*****************************************************************************/
TEST_F(TestVariableProxy, two_dimensional_sum_arg_indices) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy = model.create_variables("x", {2, 3});

    auto expression_0 =
        printemps::model::Expression<int, double>::create_instance();
    auto expression_1 =
        printemps::model::Expression<int, double>::create_instance();
    auto expression_01 =
        printemps::model::Expression<int, double>::create_instance();

    expression_0  = variable_proxy.sum({printemps::model::Range::All, 0});
    expression_1  = variable_proxy.sum({0, printemps::model::Range::All});
    expression_01 = variable_proxy.sum(
        {printemps::model::Range::All, printemps::model::Range::All});

    for (auto i = 0; i < variable_proxy.number_of_elements(); i++) {
        variable_proxy[i] = 1;
    }

    EXPECT_EQ(1, expression_0.sensitivities().at(&(variable_proxy[0])));
    EXPECT_EQ(1, expression_0.sensitivities().at(&(variable_proxy[3])));
    EXPECT_EQ(2, expression_0.evaluate());

    EXPECT_EQ(1, expression_1.sensitivities().at(&(variable_proxy[0])));
    EXPECT_EQ(1, expression_1.sensitivities().at(&(variable_proxy[2])));
    EXPECT_EQ(3, expression_1.evaluate());

    EXPECT_EQ(1, expression_01.sensitivities().at(&(variable_proxy[0])));
    EXPECT_EQ(1,
              expression_01.sensitivities().at(&(variable_proxy[2 * 3 - 1])));
    EXPECT_EQ(2 * 3, expression_01.evaluate());

    ASSERT_THROW(variable_proxy.sum({printemps::model::Range::All}),
                 std::logic_error);
    ASSERT_THROW(variable_proxy.sum({printemps::model::Range::All, 0, 0}),
                 std::logic_error);
}

/*****************************************************************************/
TEST_F(TestVariableProxy, two_dimensional_dot_arg_vector) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy = model.create_variables("x", {2, 3});
    auto  expression =
        printemps::model::Expression<int, double>::create_instance();

    std::vector<double> sensitivities;
    for (auto i = 0; i < variable_proxy.number_of_elements(); i++) {
        sensitivities.push_back(random_integer());
    }
    ASSERT_THROW(variable_proxy.dot(sensitivities), std::logic_error);
}

/*****************************************************************************/
TEST_F(TestVariableProxy, two_dimensional_dot_arg_indice_vector) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy = model.create_variables("x", {2, 3});

    std::vector<double> sensitivities_0;
    auto                sum_0 = 0;
    for (auto i = 0; i < 2; i++) {
        sensitivities_0.push_back(random_integer());
        sum_0 += sensitivities_0.back();
    }

    std::vector<double> sensitivities_1;
    auto                sum_1 = 0;
    for (auto i = 0; i < 3; i++) {
        sensitivities_1.push_back(random_integer());
        sum_1 += sensitivities_1.back();
    }

    std::vector<double> sensitivities_01;
    for (auto i = 0; i < variable_proxy.number_of_elements(); i++) {
        sensitivities_01.push_back(random_integer());
    }

    auto expression_0 =
        printemps::model::Expression<int, double>::create_instance();
    auto expression_1 =
        printemps::model::Expression<int, double>::create_instance();
    auto expression_01 =
        printemps::model::Expression<int, double>::create_instance();

    expression_0 =
        variable_proxy.dot({printemps::model::Range::All, 0}, sensitivities_0);
    expression_1 =
        variable_proxy.dot({0, printemps::model::Range::All}, sensitivities_1);

    for (auto i = 0; i < variable_proxy.number_of_elements(); i++) {
        variable_proxy[i] = 1;
    }

    EXPECT_EQ(sensitivities_0[0],
              expression_0.sensitivities().at(&(variable_proxy[0])));

    EXPECT_EQ(sensitivities_0[1],
              expression_0.sensitivities().at(&(variable_proxy[3])));

    EXPECT_EQ(sum_0, expression_0.evaluate());

    EXPECT_EQ(sensitivities_1[0],
              expression_1.sensitivities().at(&(variable_proxy[0])));

    EXPECT_EQ(sensitivities_1[2],
              expression_1.sensitivities().at(&(variable_proxy[2])));

    EXPECT_EQ(sum_1, expression_1.evaluate());

    ASSERT_THROW(
        variable_proxy.dot({printemps::model::Range::All}, sensitivities_0),
        std::logic_error);
    ASSERT_THROW(variable_proxy.dot({printemps::model::Range::All,
                                     printemps::model::Range::All},
                                    sensitivities_01),
                 std::logic_error);
    ASSERT_THROW(variable_proxy.dot({printemps::model::Range::All, 0, 0},
                                    sensitivities_0),
                 std::logic_error);
}

/*****************************************************************************/
TEST_F(TestVariableProxy, two_dimensional_selection_arg_void) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy = model.create_variables("x", {2, 3});
    auto  constraint =
        printemps::model::Constraint<int, double>::create_instance();

    constraint = variable_proxy.selection();

    EXPECT_EQ(1,
              constraint.expression().sensitivities().at(&(variable_proxy[0])));
    EXPECT_EQ(1, constraint.expression().sensitivities().at(
                     &(variable_proxy[2 * 3 - 1])));
    EXPECT_EQ(printemps::model::ConstraintSense::Equal, constraint.sense());
}

/*****************************************************************************/
TEST_F(TestVariableProxy, two_dimensional_selection_arg_indices) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy = model.create_variables("x", {2, 3});

    auto constraint_0 =
        printemps::model::Constraint<int, double>::create_instance();
    auto constraint_1 =
        printemps::model::Constraint<int, double>::create_instance();
    auto constraint_01 =
        printemps::model::Constraint<int, double>::create_instance();

    constraint_0  = variable_proxy.selection({printemps::model::Range::All, 0});
    constraint_1  = variable_proxy.selection({0, printemps::model::Range::All});
    constraint_01 = variable_proxy.selection(
        {printemps::model::Range::All, printemps::model::Range::All});

    EXPECT_EQ(
        1, constraint_0.expression().sensitivities().at(&(variable_proxy[0])));
    EXPECT_EQ(
        1, constraint_0.expression().sensitivities().at(&(variable_proxy[3])));
    EXPECT_EQ(printemps::model::ConstraintSense::Equal, constraint_0.sense());

    EXPECT_EQ(
        1, constraint_1.expression().sensitivities().at(&(variable_proxy[0])));
    EXPECT_EQ(
        1, constraint_1.expression().sensitivities().at(&(variable_proxy[2])));
    EXPECT_EQ(printemps::model::ConstraintSense::Equal, constraint_0.sense());

    EXPECT_EQ(
        1, constraint_01.expression().sensitivities().at(&(variable_proxy[0])));
    EXPECT_EQ(1, constraint_01.expression().sensitivities().at(
                     &(variable_proxy[2 * 3 - 1])));
    EXPECT_EQ(printemps::model::ConstraintSense::Equal, constraint_0.sense());

    ASSERT_THROW(variable_proxy.selection({printemps::model::Range::All}),
                 std::logic_error);
    ASSERT_THROW(variable_proxy.selection({printemps::model::Range::All, 0, 0}),
                 std::logic_error);
}

/*****************************************************************************/
TEST_F(TestVariableProxy, two_dimensional_operator_plus) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy = model.create_variables("x", {2, 3});
    ASSERT_THROW(+variable_proxy, std::logic_error);
}

/*****************************************************************************/
TEST_F(TestVariableProxy, two_dimensional_operator_minus) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy = model.create_variables("x", {2, 3});
    ASSERT_THROW(-variable_proxy, std::logic_error);
}

/*****************************************************************************/
TEST_F(TestVariableProxy, two_dimensional_operator_equal) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy = model.create_variables("x", {2, 3});
    auto  value          = random_integer();
    ASSERT_THROW(variable_proxy = value, std::logic_error);
}

/*****************************************************************************/
TEST_F(TestVariableProxy, two_dimensional_operator_square_bracket) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy      = model.create_variables("x", {2, 3});
    auto  value_0             = random_integer();
    auto  value_1             = random_integer();
    variable_proxy[0]         = value_0;
    variable_proxy[2 * 3 - 1] = value_1;
    EXPECT_EQ(value_0, variable_proxy[0].value());
    EXPECT_EQ(value_1, variable_proxy[2 * 3 - 1].value());
}

/*****************************************************************************/
TEST_F(TestVariableProxy, two_dimensional_operator_round_bracket) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy = model.create_variables("x", {2, 3});
    auto  value_0        = random_integer();
    auto  value_1        = random_integer();
    variable_proxy(0, 0) = value_0;
    variable_proxy(1, 2) = value_1;
    EXPECT_EQ(value_0, variable_proxy(0, 0).value());
    EXPECT_EQ(value_1, variable_proxy(1, 2).value());
}

/*****************************************************************************/
TEST_F(TestVariableProxy, two_dimensional_operator_round_bracket_with_indices) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy   = model.create_variables("x", {2, 3});
    auto  value_0          = random_integer();
    auto  value_1          = random_integer();
    variable_proxy({0, 0}) = value_0;
    variable_proxy({1, 2}) = value_1;
    EXPECT_EQ(value_0, variable_proxy({0, 0}).value());
    EXPECT_EQ(value_1, variable_proxy({1, 2}).value());
}

/*****************************************************************************/
TEST_F(TestVariableProxy, three_dimensional_create_instance) {
    printemps::model::Model<int, double> model;
    auto& variable_proxy = model.create_variables("x", {2, 3, 4});

    /// Check the initial values of the base class members.
    EXPECT_EQ(0, variable_proxy.index());
    EXPECT_EQ(2, variable_proxy.shape()[0]);
    EXPECT_EQ(3, variable_proxy.shape()[1]);
    EXPECT_EQ(4, variable_proxy.shape()[2]);
    EXPECT_EQ(12, variable_proxy.strides()[0]);
    EXPECT_EQ(4, variable_proxy.strides()[1]);
    EXPECT_EQ(1, variable_proxy.strides()[2]);
    EXPECT_EQ(3, variable_proxy.number_of_dimensions());
    EXPECT_EQ(2 * 3 * 4, variable_proxy.number_of_elements());
}

/*****************************************************************************/
TEST_F(TestVariableProxy, three_dimensional_operator_round_bracket) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy    = model.create_variables("x", {2, 3, 4});
    auto  value_0           = random_integer();
    auto  value_1           = random_integer();
    variable_proxy(0, 0, 0) = value_0;
    variable_proxy(1, 2, 3) = value_1;
    EXPECT_EQ(value_0, variable_proxy(0, 0, 0).value());
    EXPECT_EQ(value_1, variable_proxy(1, 2, 3).value());
}

/*****************************************************************************/
TEST_F(TestVariableProxy,
       three_dimensional_operator_round_bracket_with_indices) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy      = model.create_variables("x", {2, 3, 4});
    auto  value_0             = random_integer();
    auto  value_1             = random_integer();
    variable_proxy({0, 0, 0}) = value_0;
    variable_proxy({1, 2, 3}) = value_1;
    EXPECT_EQ(value_0, variable_proxy({0, 0, 0}).value());
    EXPECT_EQ(value_1, variable_proxy({1, 2, 3}).value());
}

/*****************************************************************************/
TEST_F(TestVariableProxy, four_dimensional_create_instance) {
    printemps::model::Model<int, double> model;
    auto& variable_proxy = model.create_variables("x", {2, 3, 4, 5});

    /// Check the initial values of the base class members.
    EXPECT_EQ(0, variable_proxy.index());
    EXPECT_EQ(2, variable_proxy.shape()[0]);
    EXPECT_EQ(3, variable_proxy.shape()[1]);
    EXPECT_EQ(4, variable_proxy.shape()[2]);
    EXPECT_EQ(5, variable_proxy.shape()[3]);
    EXPECT_EQ(60, variable_proxy.strides()[0]);
    EXPECT_EQ(20, variable_proxy.strides()[1]);
    EXPECT_EQ(5, variable_proxy.strides()[2]);
    EXPECT_EQ(1, variable_proxy.strides()[3]);
    EXPECT_EQ(4, variable_proxy.number_of_dimensions());
    EXPECT_EQ(2 * 3 * 4 * 5, variable_proxy.number_of_elements());
}

/*****************************************************************************/
TEST_F(TestVariableProxy, four_dimensional_operator_round_bracket) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy       = model.create_variables("x", {2, 3, 4, 5});
    auto  value_0              = random_integer();
    auto  value_1              = random_integer();
    variable_proxy(0, 0, 0, 0) = value_0;
    variable_proxy(1, 2, 3, 4) = value_1;
    EXPECT_EQ(value_0, variable_proxy(0, 0, 0, 0).value());
    EXPECT_EQ(value_1, variable_proxy(1, 2, 3, 4).value());
}

/*****************************************************************************/
TEST_F(TestVariableProxy,
       four_dimensional_operator_round_bracket_with_indices) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy         = model.create_variables("x", {2, 3, 4, 5});
    auto  value_0                = random_integer();
    auto  value_1                = random_integer();
    variable_proxy({0, 0, 0, 0}) = value_0;
    variable_proxy({1, 2, 3, 4}) = value_1;
    EXPECT_EQ(value_0, variable_proxy({0, 0, 0, 0}).value());
    EXPECT_EQ(value_1, variable_proxy({1, 2, 3, 4}).value());
}
/*****************************************************************************/
}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/