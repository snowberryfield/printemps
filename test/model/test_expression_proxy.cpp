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
class TestExpressionProxy : public ::testing::Test {
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
TEST_F(TestExpressionProxy, scalar_create_instance) {
    printemps::model::Model<int, double> model;
    auto& expression_proxy = model.create_expression("e");

    /// Check the initial values of the base class members.
    EXPECT_EQ(0, expression_proxy.index());
    EXPECT_EQ(1, expression_proxy.shape()[0]);
    EXPECT_EQ(1, expression_proxy.strides()[0]);
    EXPECT_EQ(1, expression_proxy.number_of_dimensions());
    EXPECT_EQ(1, expression_proxy.number_of_elements());
}

/*****************************************************************************/
TEST_F(TestExpressionProxy, scalar_sensitivities) {
    printemps::model::Model<int, double> model;
    auto& expression_proxy = model.create_expression("e");
    auto  variable = printemps::model::Variable<int, double>::create_instance();

    auto sensitivity = random_integer();
    expression_proxy = sensitivity * variable;

    EXPECT_EQ(sensitivity, expression_proxy.sensitivities().at(&variable));
}

/*****************************************************************************/
TEST_F(TestExpressionProxy, scalar_constant_value) {
    printemps::model::Model<int, double> model;
    auto& expression_proxy = model.create_expression("e");

    auto constant    = random_integer();
    expression_proxy = constant;

    EXPECT_EQ(constant, expression_proxy.constant_value());
}

/*****************************************************************************/
TEST_F(TestExpressionProxy, scalar_evaluate_arg_void) {
    printemps::model::Model<int, double> model;
    auto& expression_proxy = model.create_expression("e");

    auto variable_0 =
        printemps::model::Variable<int, double>::create_instance();
    auto variable_1 =
        printemps::model::Variable<int, double>::create_instance();

    auto sensitivity_0 = random_integer();
    auto sensitivity_1 = random_integer();
    auto constant      = random_integer();

    expression_proxy =
        sensitivity_0 * variable_0 + sensitivity_1 * variable_1 + constant;

    auto value_0 = random_integer();
    auto value_1 = random_integer();

    variable_0 = value_0;
    variable_1 = value_1;

    auto expected_result =
        sensitivity_0 * value_0 + sensitivity_1 * value_1 + constant;

    EXPECT_EQ(expected_result, expression_proxy.evaluate());
}

/*****************************************************************************/
TEST_F(TestExpressionProxy, scalar_evaluate_arg_move) {
    printemps::model::Model<int, double> model;
    auto& expression_proxy = model.create_expression("e");

    auto variable_0 =
        printemps::model::Variable<int, double>::create_instance();
    auto variable_1 =
        printemps::model::Variable<int, double>::create_instance();

    auto sensitivity_0 = random_integer();
    auto sensitivity_1 = random_integer();
    auto constant      = random_integer();

    expression_proxy =
        sensitivity_0 * variable_0 + sensitivity_1 * variable_1 + constant;

    for (auto&& expression : expression_proxy.flat_indexed_expressions()) {
        expression.setup_fixed_sensitivities();
    }

    auto value_0 = random_integer();
    auto value_1 = random_integer();

    variable_0 = value_0;
    variable_1 = value_1;

    expression_proxy.update();

    printemps::neighborhood::Move<int, double> move;
    value_0 = random_integer();
    value_1 = random_integer();

    move.alterations.emplace_back(&variable_0, value_0);
    move.alterations.emplace_back(&variable_1, value_1);

    auto expected_result =
        sensitivity_0 * value_0 + sensitivity_1 * value_1 + constant;

    EXPECT_EQ(expected_result, expression_proxy.evaluate(move));
}

/*****************************************************************************/
TEST_F(TestExpressionProxy, scalar_update_arg_void) {
    printemps::model::Model<int, double> model;
    auto& expression_proxy = model.create_expression("e");

    auto variable_0 =
        printemps::model::Variable<int, double>::create_instance();
    auto variable_1 =
        printemps::model::Variable<int, double>::create_instance();

    auto sensitivity_0 = random_integer();
    auto sensitivity_1 = random_integer();
    auto constant      = random_integer();

    expression_proxy =
        sensitivity_0 * variable_0 + sensitivity_1 * variable_1 + constant;

    auto value_0 = random_integer();
    auto value_1 = random_integer();

    variable_0 = value_0;
    variable_1 = value_1;

    expression_proxy.update();
    auto expected_result =
        sensitivity_0 * value_0 + sensitivity_1 * value_1 + constant;

    EXPECT_EQ(expected_result, expression_proxy.value());
}

/*****************************************************************************/
TEST_F(TestExpressionProxy, scalar_update_arg_move) {
    printemps::model::Model<int, double> model;
    auto& expression_proxy = model.create_expression("e");

    auto variable_0 =
        printemps::model::Variable<int, double>::create_instance();
    auto variable_1 =
        printemps::model::Variable<int, double>::create_instance();

    auto sensitivity_0 = random_integer();
    auto sensitivity_1 = random_integer();
    auto constant      = random_integer();

    expression_proxy =
        sensitivity_0 * variable_0 + sensitivity_1 * variable_1 + constant;

    for (auto&& expression : expression_proxy.flat_indexed_expressions()) {
        expression.setup_fixed_sensitivities();
    }

    auto value_0 = random_integer();
    auto value_1 = random_integer();

    variable_0 = value_0;
    variable_1 = value_1;

    expression_proxy.update();

    printemps::neighborhood::Move<int, double> move;
    value_0 = random_integer();
    value_1 = random_integer();

    move.alterations.emplace_back(&variable_0, value_0);
    move.alterations.emplace_back(&variable_1, value_1);

    expression_proxy.update(move);
    auto expected_result =
        sensitivity_0 * value_0 + sensitivity_1 * value_1 + constant;

    EXPECT_EQ(expected_result, expression_proxy.value());
}

/*****************************************************************************/
TEST_F(TestExpressionProxy, scalar_value) {
    /// This method is tested in other cases.
}

/*****************************************************************************/
TEST_F(TestExpressionProxy, scalar_set_name) {
    printemps::model::Model<int, double> model;

    auto& expression_proxy = model.create_expression("e");
    expression_proxy.set_name("_e");
    EXPECT_EQ("_e", expression_proxy.name());
}

/*****************************************************************************/
TEST_F(TestExpressionProxy, scalar_name) {
    /// This method is tested in scalar_set_name().
}

/*****************************************************************************/
TEST_F(TestExpressionProxy, scalar_flat_indexed_expressions_arg_void) {
    printemps::model::Model<int, double> model;
    auto& expression_proxy = model.create_expression("e");

    auto variable_0 =
        printemps::model::Variable<int, double>::create_instance();
    auto variable_1 =
        printemps::model::Variable<int, double>::create_instance();

    auto sensitivity_0 = random_integer();
    auto sensitivity_1 = random_integer();
    auto constant      = random_integer();

    expression_proxy[0] =
        sensitivity_0 * variable_0 + sensitivity_1 * variable_1 + constant;

    for (auto&& expression : expression_proxy.flat_indexed_expressions()) {
        expression *= 2;
    }

    EXPECT_EQ(2 * sensitivity_0,
              expression_proxy.flat_indexed_expressions()[0].sensitivities().at(
                  &variable_0));

    EXPECT_EQ(2 * sensitivity_1,
              expression_proxy.flat_indexed_expressions()[0].sensitivities().at(
                  &variable_1));

    EXPECT_EQ(2 * constant,
              expression_proxy.flat_indexed_expressions()[0].constant_value());
}

/*****************************************************************************/
TEST_F(TestExpressionProxy, scalar_flat_indexed_expressions_arg_int) {
    printemps::model::Model<int, double> model;
    auto& expression_proxy = model.create_expression("e");

    auto variable_0 =
        printemps::model::Variable<int, double>::create_instance();
    auto variable_1 =
        printemps::model::Variable<int, double>::create_instance();

    auto sensitivity_0 = random_integer();
    auto sensitivity_1 = random_integer();
    auto constant      = random_integer();

    expression_proxy[0] =
        sensitivity_0 * variable_0 + sensitivity_1 * variable_1 + constant;

    expression_proxy.flat_indexed_expressions(0) *= 2;

    EXPECT_EQ(2 * sensitivity_0,
              expression_proxy.flat_indexed_expressions(0).sensitivities().at(
                  &variable_0));

    EXPECT_EQ(2 * sensitivity_1,
              expression_proxy.flat_indexed_expressions(0).sensitivities().at(
                  &variable_1));

    EXPECT_EQ(2 * constant,
              expression_proxy.flat_indexed_expressions(0).constant_value());
}

/*****************************************************************************/
TEST_F(TestExpressionProxy, scalar_export_values_and_names) {
    printemps::model::Model<int, double> model;
    auto& expression_proxy = model.create_expression("e");

    auto variable_0 =
        printemps::model::Variable<int, double>::create_instance();
    auto variable_1 =
        printemps::model::Variable<int, double>::create_instance();

    auto sensitivity_0 = random_integer();
    auto sensitivity_1 = random_integer();
    auto constant      = random_integer();

    expression_proxy[0] =
        sensitivity_0 * variable_0 + sensitivity_1 * variable_1 + constant;

    auto value_0 = random_integer();
    auto value_1 = random_integer();

    variable_0 = value_0;
    variable_1 = value_1;

    expression_proxy.update();
    auto expected_value =
        sensitivity_0 * value_0 + sensitivity_1 * value_1 + constant;

    EXPECT_EQ(expected_value,
              expression_proxy.export_values_and_names().value());
}

/*****************************************************************************/
TEST_F(TestExpressionProxy, scalar_to_expression) {
    printemps::model::Model<int, double> model;
    auto& expression_proxy = model.create_expression("e");

    auto variable_0 =
        printemps::model::Variable<int, double>::create_instance();
    auto variable_1 =
        printemps::model::Variable<int, double>::create_instance();

    auto sensitivity_0 = random_integer();
    auto sensitivity_1 = random_integer();
    auto constant      = random_integer();

    expression_proxy =
        sensitivity_0 * variable_0 + sensitivity_1 * variable_1 + constant;

    auto expression = expression_proxy.to_expression();

    EXPECT_EQ(sensitivity_0, expression.sensitivities().at(&variable_0));
    EXPECT_EQ(sensitivity_1, expression.sensitivities().at(&variable_1));
    EXPECT_EQ(constant, expression.constant_value());
}

/*****************************************************************************/
TEST_F(TestExpressionProxy, scalar_sum_arg_void) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy   = model.create_variable("x");
    auto& expression_proxy = model.create_expression("e");
    expression_proxy       = variable_proxy;

    auto expression =
        printemps::model::Expression<int, double>::create_instance();
    expression = expression_proxy.sum();

    for (auto i = 0; i < variable_proxy.number_of_elements(); i++) {
        variable_proxy[i] = 1;
    }

    EXPECT_EQ(1, expression.sensitivities().at(&(variable_proxy[0])));
    EXPECT_EQ(1, expression.evaluate());
}

/*****************************************************************************/
TEST_F(TestExpressionProxy, scalar_sum_arg_indices) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy   = model.create_variable("x");
    auto& expression_proxy = model.create_expression("e");
    expression_proxy       = variable_proxy;

    auto expression =
        printemps::model::Expression<int, double>::create_instance();
    expression = expression_proxy.sum({printemps::model::Range::All});

    for (auto i = 0; i < variable_proxy.number_of_elements(); i++) {
        variable_proxy[i] = 1;
    }

    EXPECT_EQ(1, expression.sensitivities().at(&(variable_proxy[0])));
    EXPECT_EQ(1, expression.evaluate());

    ASSERT_THROW(variable_proxy.sum({printemps::model::Range::All, 0}),
                 std::logic_error);
}

/*****************************************************************************/
TEST_F(TestExpressionProxy, scalar_dot_arg_vector) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy   = model.create_variable("x");
    auto& expression_proxy = model.create_expression("e");
    expression_proxy       = variable_proxy;

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
TEST_F(TestExpressionProxy, scalar_dot_arg_indice_vector) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy   = model.create_variable("x");
    auto& expression_proxy = model.create_expression("e");
    expression_proxy       = variable_proxy;

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
        expression_proxy.dot({0, printemps::model::Range::All}, sensitivities),
        std::logic_error);
}

/*****************************************************************************/
TEST_F(TestExpressionProxy, scalar_is_enabled) {
    printemps::model::Model<int, double> model;
    auto& expression_proxy = model.create_expression("e");
    expression_proxy.disable();
    EXPECT_EQ(false, expression_proxy.is_enabled());

    expression_proxy.enable();
    EXPECT_EQ(true, expression_proxy.is_enabled());

    expression_proxy.disable();
    EXPECT_EQ(false, expression_proxy.is_enabled());
}

/*****************************************************************************/
TEST_F(TestExpressionProxy, scalar_enable) {
    /// This method is tested in scalar_is_enabled().
}

/*****************************************************************************/
TEST_F(TestExpressionProxy, scalar_disable) {
    /// This method is tested in scalar_is_enabled().
}

/*****************************************************************************/
TEST_F(TestExpressionProxy, scalar_operator_plus) {
    printemps::model::Model<int, double> model;
    auto& expression_proxy = model.create_expression("e");

    auto variable_0 =
        printemps::model::Variable<int, double>::create_instance();
    auto variable_1 =
        printemps::model::Variable<int, double>::create_instance();

    auto sensitivity_0 = random_integer();
    auto sensitivity_1 = random_integer();
    auto constant      = random_integer();

    expression_proxy =
        sensitivity_0 * variable_0 + sensitivity_1 * variable_1 + constant;

    EXPECT_EQ(sensitivity_0,
              (+expression_proxy).sensitivities().at(&variable_0));
    EXPECT_EQ(sensitivity_1,
              (+expression_proxy).sensitivities().at(&variable_1));
    EXPECT_EQ(constant, (+expression_proxy).constant_value());
}

/*****************************************************************************/
TEST_F(TestExpressionProxy, scalar_operator_minus) {
    printemps::model::Model<int, double> model;
    auto& expression_proxy = model.create_expression("e");

    auto variable_0 =
        printemps::model::Variable<int, double>::create_instance();
    auto variable_1 =
        printemps::model::Variable<int, double>::create_instance();

    auto sensitivity_0 = random_integer();
    auto sensitivity_1 = random_integer();
    auto constant      = random_integer();

    expression_proxy =
        sensitivity_0 * variable_0 + sensitivity_1 * variable_1 + constant;

    EXPECT_EQ(-sensitivity_0,
              (-expression_proxy).sensitivities().at(&variable_0));
    EXPECT_EQ(-sensitivity_1,
              (-expression_proxy).sensitivities().at(&variable_1));
    EXPECT_EQ(-constant, (-expression_proxy).constant_value());
}

/*****************************************************************************/
TEST_F(TestExpressionProxy, scalar_operator_equal_arg_t_value) {
    printemps::model::Model<int, double> model;
    auto& expression_proxy = model.create_expression("e");
    auto  value            = random_integer();

    EXPECT_EQ(value, (expression_proxy = value).constant_value());
    EXPECT_EQ(value, expression_proxy.constant_value());
}

/*****************************************************************************/
TEST_F(TestExpressionProxy, scalar_operator_equal_arg_t_expression_like) {
    printemps::model::Model<int, double> model;
    auto& expression_proxy       = model.create_expression("e");
    auto& variable_proxy         = model.create_variable("x");
    auto& expression_proxy_other = model.create_expression("y");

    expression_proxy_other = variable_proxy;

    /// variable proxy
    EXPECT_EQ(1, (expression_proxy = variable_proxy)
                     .sensitivities()
                     .at(&variable_proxy[0]));
    EXPECT_EQ(1, expression_proxy.sensitivities().at(&variable_proxy[0]));

    /// variable
    EXPECT_EQ(1, (expression_proxy = variable_proxy[0])
                     .sensitivities()
                     .at(&variable_proxy[0]));
    EXPECT_EQ(1, expression_proxy.sensitivities().at(&variable_proxy[0]));

    /// expression proxy
    EXPECT_EQ(1, (expression_proxy = expression_proxy_other)
                     .sensitivities()
                     .at(&variable_proxy[0]));
    EXPECT_EQ(1, expression_proxy.sensitivities().at(&variable_proxy[0]));

    /// If the size of variable_proxy or expression_proxy_other > 1, an error
    /// will be thrown at the to_expression()
}

/*****************************************************************************/
TEST_F(TestExpressionProxy, scalar_operator_equal_expression) {
    printemps::model::Model<int, double> model;
    auto& expression_proxy = model.create_expression("e");

    auto expression =
        printemps::model::Expression<int, double>::create_instance();

    auto variable_0 =
        printemps::model::Variable<int, double>::create_instance();
    auto variable_1 =
        printemps::model::Variable<int, double>::create_instance();

    auto sensitivity_0 = random_integer();
    auto sensitivity_1 = random_integer();
    auto constant      = random_integer();

    expression =
        sensitivity_0 * variable_0 + sensitivity_1 * variable_1 + constant;

    EXPECT_EQ(sensitivity_0,
              (expression_proxy += expression).sensitivities().at(&variable_0));
    EXPECT_EQ(sensitivity_1, expression_proxy.sensitivities().at(&variable_1));
    EXPECT_EQ(constant, expression_proxy.constant_value());
}

/*****************************************************************************/
TEST_F(TestExpressionProxy, scalar_operator_plus_equal_arg_t_value) {
    printemps::model::Model<int, double> model;
    auto& expression_proxy = model.create_expression("e");
    auto  value_0          = random_integer();
    auto  value_1          = random_integer();

    EXPECT_EQ(value_0, (expression_proxy += value_0).constant_value());
    EXPECT_EQ(value_0, expression_proxy.constant_value());
    EXPECT_EQ(value_0 + value_1,
              (expression_proxy += value_1).constant_value());
    EXPECT_EQ(value_0 + value_1, expression_proxy.constant_value());
}

/*****************************************************************************/
TEST_F(TestExpressionProxy, scalar_operator_plus_equal_arg_t_expression_like) {
    printemps::model::Model<int, double> model;
    auto& expression_proxy       = model.create_expression("e");
    auto& variable_proxy         = model.create_variable("x");
    auto& expression_proxy_other = model.create_expression("y");

    expression_proxy_other = variable_proxy;

    /// variable proxy
    EXPECT_EQ(1, (expression_proxy += variable_proxy)
                     .sensitivities()
                     .at(&variable_proxy[0]));
    EXPECT_EQ(1, expression_proxy.sensitivities().at(&variable_proxy[0]));

    /// variable
    EXPECT_EQ(2, (expression_proxy += variable_proxy[0])
                     .sensitivities()
                     .at(&variable_proxy[0]));
    EXPECT_EQ(2, expression_proxy.sensitivities().at(&variable_proxy[0]));

    /// expression proxy
    EXPECT_EQ(3, (expression_proxy += expression_proxy_other)
                     .sensitivities()
                     .at(&variable_proxy[0]));
    EXPECT_EQ(3, expression_proxy.sensitivities().at(&variable_proxy[0]));

    /// If the size of variable_proxy or expression_proxy_other > 1, an error
    /// will be thrown at the to_expression()
}

/*****************************************************************************/
TEST_F(TestExpressionProxy, operator_plus_equal_arg_expression) {
    printemps::model::Model<int, double> model;
    auto& expression_proxy = model.create_expression("e");
    auto  expression =
        printemps::model::Expression<int, double>::create_instance();

    auto variable_0 =
        printemps::model::Variable<int, double>::create_instance();
    auto variable_1 =
        printemps::model::Variable<int, double>::create_instance();

    auto sensitivity_0_0 = random_integer();
    auto sensitivity_0_1 = random_integer();
    auto sensitivity_1_0 = random_integer();
    auto sensitivity_1_1 = random_integer();

    auto constant_0 = random_integer();
    auto constant_1 = random_integer();

    expression_proxy = sensitivity_0_0 * variable_0 +
                       sensitivity_1_0 * variable_1 + constant_0;
    expression = sensitivity_0_1 * variable_0 + sensitivity_1_1 * variable_1 +
                 constant_1;

    EXPECT_EQ(sensitivity_0_0 + sensitivity_0_1,
              (expression_proxy += expression).sensitivities().at(&variable_0));

    EXPECT_EQ(sensitivity_0_0 + sensitivity_0_1,
              expression_proxy.sensitivities().at(&variable_0));
    EXPECT_EQ(sensitivity_1_0 + sensitivity_1_1,
              expression_proxy.sensitivities().at(&variable_1));

    EXPECT_EQ(constant_0 + constant_1, expression_proxy.constant_value());
}

/*****************************************************************************/
TEST_F(TestExpressionProxy, scalar_operator_minus_equal_arg_t_value) {
    printemps::model::Model<int, double> model;
    auto& expression_proxy = model.create_expression("e");
    auto  value_0          = random_integer();
    auto  value_1          = random_integer();

    EXPECT_EQ(-value_0, (expression_proxy -= value_0).constant_value());
    EXPECT_EQ(-value_0, expression_proxy.constant_value());
    EXPECT_EQ(-value_0 - value_1,
              (expression_proxy -= value_1).constant_value());
    EXPECT_EQ(-value_0 - value_1, expression_proxy.constant_value());
}

/*****************************************************************************/
TEST_F(TestExpressionProxy, scalar_operator_minus_equal_arg_t_expression_like) {
    printemps::model::Model<int, double> model;
    auto& expression_proxy       = model.create_expression("e");
    auto& variable_proxy         = model.create_variable("x");
    auto& expression_proxy_other = model.create_expression("y");

    expression_proxy_other = variable_proxy;

    /// variable proxy
    EXPECT_EQ(-1, (expression_proxy -= variable_proxy)
                      .sensitivities()
                      .at(&variable_proxy[0]));
    EXPECT_EQ(-1, expression_proxy.sensitivities().at(&variable_proxy[0]));

    /// variable
    EXPECT_EQ(-2, (expression_proxy -= variable_proxy[0])
                      .sensitivities()
                      .at(&variable_proxy[0]));
    EXPECT_EQ(-2, expression_proxy.sensitivities().at(&variable_proxy[0]));

    /// expression proxy
    EXPECT_EQ(-3, (expression_proxy -= expression_proxy_other)
                      .sensitivities()
                      .at(&variable_proxy[0]));
    EXPECT_EQ(-3, expression_proxy.sensitivities().at(&variable_proxy[0]));

    /// If the size of variable_proxy or expression_proxy_other > 1, an error
    /// will be thrown at the to_expression()
}

/*****************************************************************************/
TEST_F(TestExpressionProxy, scalar_operator_minus_equal_arg_expression) {
    printemps::model::Model<int, double> model;
    auto& expression_proxy = model.create_expression("e");
    auto  expression =
        printemps::model::Expression<int, double>::create_instance();

    auto variable_0 =
        printemps::model::Variable<int, double>::create_instance();
    auto variable_1 =
        printemps::model::Variable<int, double>::create_instance();

    auto sensitivity_0_0 = random_integer();
    auto sensitivity_0_1 = random_integer();
    auto sensitivity_1_0 = random_integer();
    auto sensitivity_1_1 = random_integer();

    auto constant_0 = random_integer();
    auto constant_1 = random_integer();

    expression_proxy = sensitivity_0_0 * variable_0 +
                       sensitivity_1_0 * variable_1 + constant_0;
    expression = sensitivity_0_1 * variable_0 + sensitivity_1_1 * variable_1 +
                 constant_1;

    EXPECT_EQ(sensitivity_0_0 - sensitivity_0_1,
              (expression_proxy -= expression).sensitivities().at(&variable_0));

    EXPECT_EQ(sensitivity_0_0 - sensitivity_0_1,
              expression_proxy.sensitivities().at(&variable_0));
    EXPECT_EQ(sensitivity_1_0 - sensitivity_1_1,
              expression_proxy.sensitivities().at(&variable_1));

    EXPECT_EQ(constant_0 - constant_1, expression_proxy.constant_value());
}

/*****************************************************************************/
TEST_F(TestExpressionProxy, scalar_operator_product_equal_arg_t_value) {
    printemps::model::Model<int, double> model;
    auto& expression_proxy = model.create_expression("e");
    auto  variable = printemps::model::Variable<int, double>::create_instance();

    auto sensitivity = random_integer();
    auto constant    = random_integer();

    expression_proxy = sensitivity * variable + constant;

    auto value_0 = random_integer();
    auto value_1 = random_integer();

    EXPECT_EQ(constant * value_0,
              (expression_proxy *= value_0).constant_value());
    EXPECT_EQ(constant * value_0, expression_proxy.constant_value());
    EXPECT_EQ(sensitivity * value_0 * value_1,
              (expression_proxy *= value_1).sensitivities().at(&variable));
    EXPECT_EQ(sensitivity * value_0 * value_1,
              expression_proxy.sensitivities().at(&variable));
}

/*****************************************************************************/
TEST_F(TestExpressionProxy, scalar_operator_divide_equal_arg_t_value) {
    printemps::model::Model<int, double> model;
    auto& expression_proxy = model.create_expression("e");
    auto  variable = printemps::model::Variable<int, double>::create_instance();

    expression_proxy = 100 * variable + 200;

    EXPECT_EQ(100, (expression_proxy /= 2).constant_value());
    EXPECT_EQ(100, expression_proxy.constant_value());
    EXPECT_EQ(25, (expression_proxy /= 2).sensitivities().at(&variable));
    EXPECT_EQ(25, expression_proxy.sensitivities().at(&variable));
}

/*****************************************************************************/
TEST_F(TestExpressionProxy, one_dimensional_create_instance) {
    printemps::model::Model<int, double> model;

    auto& expression_proxy = model.create_expressions("e", 2);

    /// Check the initial values of the base class members.
    EXPECT_EQ(0, expression_proxy.index());
    EXPECT_EQ(2, expression_proxy.shape()[0]);
    EXPECT_EQ(1, expression_proxy.strides()[0]);
    EXPECT_EQ(1, expression_proxy.number_of_dimensions());
    EXPECT_EQ(2, expression_proxy.number_of_elements());
}

/*****************************************************************************/
TEST_F(TestExpressionProxy, one_dimensional_sensitivities) {
    printemps::model::Model<int, double> model;
    auto& expression_proxy = model.create_expressions("e", 2);
    ASSERT_THROW(auto sensitivities = expression_proxy.sensitivities(),
                 std::logic_error);
}

/*****************************************************************************/
TEST_F(TestExpressionProxy, one_dimensional_constant_value) {
    printemps::model::Model<int, double> model;
    auto& expression_proxy = model.create_expressions("e", 2);
    ASSERT_THROW(
        [[maybe_unused]] auto constant = expression_proxy.constant_value(),
        std::logic_error);
}

/*****************************************************************************/
TEST_F(TestExpressionProxy, one_dimensional_evaluate_arg_void) {
    printemps::model::Model<int, double> model;
    auto& expression_proxy = model.create_expressions("e", 2);
    ASSERT_THROW(expression_proxy.evaluate(), std::logic_error);
}

/*****************************************************************************/
TEST_F(TestExpressionProxy, one_dimensional_evaluate_arg_move) {
    printemps::model::Model<int, double> model;
    auto& expression_proxy = model.create_expressions("e", 2);
    ASSERT_THROW(expression_proxy.evaluate({}), std::logic_error);
}

/*****************************************************************************/
TEST_F(TestExpressionProxy, one_dimensional_update_arg_void) {
    printemps::model::Model<int, double> model;
    auto& expression_proxy = model.create_expressions("e", 2);
    ASSERT_THROW(expression_proxy.update(), std::logic_error);
}

/*****************************************************************************/
TEST_F(TestExpressionProxy, one_dimensional_update_arg_move) {
    printemps::model::Model<int, double> model;
    auto& expression_proxy = model.create_expressions("e", 2);
    ASSERT_THROW(expression_proxy.update({}), std::logic_error);
}

/*****************************************************************************/
TEST_F(TestExpressionProxy, one_dimensional_value) {
    printemps::model::Model<int, double> model;
    auto& expression_proxy = model.create_expressions("e", 2);
    ASSERT_THROW([[maybe_unused]] auto value = expression_proxy.value(),
                 std::logic_error);
}

/*****************************************************************************/
TEST_F(TestExpressionProxy, one_dimensional_set_name) {
    printemps::model::Model<int, double> model;

    auto& expression_proxy = model.create_expressions("e", 2);

    ASSERT_THROW(expression_proxy.set_name("_e"), std::logic_error);
    ASSERT_THROW(expression_proxy.name(), std::logic_error);
}

/*****************************************************************************/
TEST_F(TestExpressionProxy, one_dimensional_name) {
    /// This method is tested in two_dimensional_set_name().
}

/*****************************************************************************/
TEST_F(TestExpressionProxy, one_dimensional_flat_indexed_expressions_arg_void) {
    printemps::model::Model<int, double> model;
    auto& expression_proxy = model.create_expressions("e", 2);

    auto variable_0 =
        printemps::model::Variable<int, double>::create_instance();
    auto variable_1 =
        printemps::model::Variable<int, double>::create_instance();

    auto sensitivity_0 = random_integer();
    auto sensitivity_1 = random_integer();
    auto constant      = random_integer();

    expression_proxy[0] =
        sensitivity_0 * variable_0 + sensitivity_1 * variable_1 + constant;
    expression_proxy[1] = expression_proxy[0] * 2;

    for (auto&& expression : expression_proxy.flat_indexed_expressions()) {
        expression *= 2;
    }

    EXPECT_EQ(2 * sensitivity_0,
              expression_proxy.flat_indexed_expressions()[0].sensitivities().at(
                  &variable_0));

    EXPECT_EQ(2 * sensitivity_1,
              expression_proxy.flat_indexed_expressions()[0].sensitivities().at(
                  &variable_1));

    EXPECT_EQ(2 * constant,
              expression_proxy.flat_indexed_expressions()[0].constant_value());

    EXPECT_EQ(4 * sensitivity_0,
              expression_proxy.flat_indexed_expressions()[1].sensitivities().at(
                  &variable_0));

    EXPECT_EQ(4 * sensitivity_1,
              expression_proxy.flat_indexed_expressions()[1].sensitivities().at(
                  &variable_1));

    EXPECT_EQ(4 * constant,
              expression_proxy.flat_indexed_expressions()[1].constant_value());
}

/*****************************************************************************/
TEST_F(TestExpressionProxy, one_dimensional_flat_indexed_expressions_arg_int) {
    printemps::model::Model<int, double> model;
    auto& expression_proxy = model.create_expressions("e", 2);

    auto variable_0 =
        printemps::model::Variable<int, double>::create_instance();
    auto variable_1 =
        printemps::model::Variable<int, double>::create_instance();

    auto sensitivity_0 = random_integer();
    auto sensitivity_1 = random_integer();
    auto constant      = random_integer();

    expression_proxy[0] =
        sensitivity_0 * variable_0 + sensitivity_1 * variable_1 + constant;
    expression_proxy[1] = expression_proxy[0] * 2;

    expression_proxy.flat_indexed_expressions(0) *= 2;
    expression_proxy.flat_indexed_expressions(1) *= 2;

    EXPECT_EQ(2 * sensitivity_0,
              expression_proxy.flat_indexed_expressions(0).sensitivities().at(
                  &variable_0));

    EXPECT_EQ(2 * sensitivity_1,
              expression_proxy.flat_indexed_expressions(0).sensitivities().at(
                  &variable_1));

    EXPECT_EQ(2 * constant,
              expression_proxy.flat_indexed_expressions(0).constant_value());

    EXPECT_EQ(4 * sensitivity_0,
              expression_proxy.flat_indexed_expressions(1).sensitivities().at(
                  &variable_0));

    EXPECT_EQ(4 * sensitivity_1,
              expression_proxy.flat_indexed_expressions(1).sensitivities().at(
                  &variable_1));

    EXPECT_EQ(4 * constant,
              expression_proxy.flat_indexed_expressions(1).constant_value());
}

/*****************************************************************************/
TEST_F(TestExpressionProxy, one_dimensional_export_values_and_names) {
    printemps::model::Model<int, double> model;
    auto& expression_proxy = model.create_expressions("e", 2);

    auto variable_0 =
        printemps::model::Variable<int, double>::create_instance();
    auto variable_1 =
        printemps::model::Variable<int, double>::create_instance();

    auto sensitivity_0 = random_integer();
    auto sensitivity_1 = random_integer();
    auto constant      = random_integer();

    expression_proxy[0] =
        sensitivity_0 * variable_0 + sensitivity_1 * variable_1 + constant;
    expression_proxy[1] = expression_proxy[0] * 2;

    auto value_0 = random_integer();
    auto value_1 = random_integer();

    variable_0 = value_0;
    variable_1 = value_1;

    for (auto&& expression : expression_proxy.flat_indexed_expressions()) {
        expression.update();
    }
    auto expected_value_0 =
        sensitivity_0 * value_0 + sensitivity_1 * value_1 + constant;
    auto expected_value_1 = expected_value_0 * 2;

    EXPECT_EQ(expected_value_0,
              expression_proxy.export_values_and_names().values(0));
    EXPECT_EQ(expected_value_1,
              expression_proxy.export_values_and_names().values(1));
}

/*****************************************************************************/
TEST_F(TestExpressionProxy, one_dimensional_to_expression) {
    printemps::model::Model<int, double> model;
    auto& expression_proxy = model.create_expressions("e", 2);
    ASSERT_THROW(auto expression = expression_proxy.to_expression(),
                 std::logic_error);
}

/*****************************************************************************/
TEST_F(TestExpressionProxy, one_dimensional_sum_arg_void) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy   = model.create_variables("x", 2);
    auto& expression_proxy = model.create_expressions("e", 2);

    for (auto i = 0; i < variable_proxy.number_of_elements(); i++) {
        expression_proxy[i] = variable_proxy[i];
    }

    auto expression =
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
TEST_F(TestExpressionProxy, one_dimensional_sum_arg_indices) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy   = model.create_variables("x", 2);
    auto& expression_proxy = model.create_expressions("e", 2);

    for (auto i = 0; i < variable_proxy.number_of_elements(); i++) {
        expression_proxy[i] = variable_proxy[i];
    }

    auto expression =
        printemps::model::Expression<int, double>::create_instance();
    expression = variable_proxy.sum({printemps::model::Range::All});

    for (auto i = 0; i < variable_proxy.number_of_elements(); i++) {
        variable_proxy[i] = 1;
    }

    EXPECT_EQ(1, expression.sensitivities().at(&(variable_proxy[0])));
    EXPECT_EQ(1, expression.sensitivities().at(&(variable_proxy[1])));
    EXPECT_EQ(2, expression.evaluate());
    ASSERT_THROW(expression_proxy.sum({printemps::model::Range::All, 0}),
                 std::logic_error);
}

/*****************************************************************************/
TEST_F(TestExpressionProxy, one_dimensional_dot_arg_vector) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy   = model.create_variables("x", 2);
    auto& expression_proxy = model.create_expressions("e", 2);

    for (auto i = 0; i < variable_proxy.number_of_elements(); i++) {
        expression_proxy[i] = variable_proxy[i];
    }

    std::vector<double> sensitivities;
    for (auto i = 0; i < variable_proxy.number_of_elements(); i++) {
        sensitivities.push_back(random_integer());
    }

    auto expression =
        printemps::model::Expression<int, double>::create_instance();
    expression = expression_proxy.dot(sensitivities);

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
TEST_F(TestExpressionProxy, one_dimensional_dot_arg_indice_vector) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy   = model.create_variables("x", 2);
    auto& expression_proxy = model.create_expressions("e", 2);

    for (auto i = 0; i < variable_proxy.number_of_elements(); i++) {
        expression_proxy[i] = variable_proxy[i];
    }

    std::vector<double> sensitivities;
    for (auto i = 0; i < variable_proxy.number_of_elements(); i++) {
        sensitivities.push_back(random_integer());
    }

    auto expression =
        printemps::model::Expression<int, double>::create_instance();
    expression = expression_proxy.dot(sensitivities);

    for (auto i = 0; i < variable_proxy.number_of_elements(); i++) {
        variable_proxy[i] = 1;
    }

    EXPECT_EQ(sensitivities[0],
              expression.sensitivities().at(&(variable_proxy[0])));
    EXPECT_EQ(sensitivities[1],
              expression.sensitivities().at(&(variable_proxy[1])));
    EXPECT_EQ(sensitivities[0] + sensitivities[1], expression.evaluate());

    ASSERT_THROW(
        expression_proxy.dot({0, printemps::model::Range::All}, sensitivities),
        std::logic_error);
}

/*****************************************************************************/
TEST_F(TestExpressionProxy, one_dimensional_is_enabled) {
    printemps::model::Model<int, double> model;
    auto& expression_proxy = model.create_expressions("e", 2);
    expression_proxy.disable();
    ASSERT_THROW(expression_proxy.is_enabled(), std::logic_error);
    EXPECT_EQ(false, expression_proxy[0].is_enabled());
    EXPECT_EQ(false, expression_proxy[1].is_enabled());

    expression_proxy.enable();
    ASSERT_THROW(expression_proxy.is_enabled(), std::logic_error);
    EXPECT_EQ(true, expression_proxy[0].is_enabled());
    EXPECT_EQ(true, expression_proxy[1].is_enabled());

    expression_proxy.disable();
    ASSERT_THROW(expression_proxy.is_enabled(), std::logic_error);
    EXPECT_EQ(false, expression_proxy[0].is_enabled());
    EXPECT_EQ(false, expression_proxy[1].is_enabled());
}

/*****************************************************************************/
TEST_F(TestExpressionProxy, one_dimensional_enable) {
    /// This method is tested in one_dimensional_is_enabled().
}

/*****************************************************************************/
TEST_F(TestExpressionProxy, one_dimensional_disable) {
    /// This method is tested in one_dimensional_is_enabled().
}

/*****************************************************************************/
TEST_F(TestExpressionProxy, one_dimensional_operator_plus) {
    printemps::model::Model<int, double> model;
    auto& expression_proxy = model.create_expressions("e", 2);
    ASSERT_THROW(auto expression = +expression_proxy, std::logic_error);
}

/*****************************************************************************/
TEST_F(TestExpressionProxy, one_dimensional_operator_minus) {
    printemps::model::Model<int, double> model;
    auto& expression_proxy = model.create_expressions("e", 2);
    ASSERT_THROW(auto expression = -expression_proxy, std::logic_error);
}

/*****************************************************************************/
TEST_F(TestExpressionProxy, one_dimensional_operator_equal_arg_t_value) {
    printemps::model::Model<int, double> model;
    auto& expression_proxy = model.create_expressions("e", 2);
    auto  value            = random_integer();
    ASSERT_THROW(expression_proxy = value, std::logic_error);
}

/*****************************************************************************/
TEST_F(TestExpressionProxy,
       one_dimensional_operator_equal_arg_t_expression_like) {
    printemps::model::Model<int, double> model;
    auto& expression_proxy       = model.create_expressions("e", 2);
    auto& variable_proxy         = model.create_variable("x");
    auto& expression_proxy_other = model.create_expression("y");
    expression_proxy_other       = variable_proxy;

    /// variable proxy
    ASSERT_THROW(expression_proxy = variable_proxy, std::logic_error);

    /// variable
    ASSERT_THROW(expression_proxy = variable_proxy[0], std::logic_error);

    /// expression proxy
    ASSERT_THROW(expression_proxy = expression_proxy_other, std::logic_error);
}

/*****************************************************************************/
TEST_F(TestExpressionProxy, one_dimensional_operator_equal_expression) {
    printemps::model::Model<int, double> model;
    auto& expression_proxy = model.create_expressions("e", 2);

    auto expression =
        printemps::model::Expression<int, double>::create_instance();
    ASSERT_THROW(expression_proxy = expression, std::logic_error);
}

/*****************************************************************************/
TEST_F(TestExpressionProxy, one_dimensional_operator_plus_equal_arg_t_value) {
    printemps::model::Model<int, double> model;
    auto& expression_proxy = model.create_expressions("e", 2);
    auto  value            = random_integer();
    ASSERT_THROW(expression_proxy += value, std::logic_error);
}

/*****************************************************************************/
TEST_F(TestExpressionProxy,
       one_dimensional_operator_plus_equal_arg_t_expression_like) {
    printemps::model::Model<int, double> model;
    auto& expression_proxy       = model.create_expressions("e", 2);
    auto& variable_proxy         = model.create_variable("x");
    auto& expression_proxy_other = model.create_expression("y");
    expression_proxy_other       = variable_proxy;

    /// variable proxy
    ASSERT_THROW(expression_proxy += variable_proxy, std::logic_error);

    /// variable
    ASSERT_THROW(expression_proxy += variable_proxy[0], std::logic_error);

    /// expression proxy
    ASSERT_THROW(expression_proxy += expression_proxy_other, std::logic_error);
}

/*****************************************************************************/
TEST_F(TestExpressionProxy,
       one_dimensional_operator_plus_equal_arg_expression) {
    printemps::model::Model<int, double> model;
    auto& expression_proxy = model.create_expressions("e", 2);

    auto expression =
        printemps::model::Expression<int, double>::create_instance();
    ASSERT_THROW(expression_proxy += expression, std::logic_error);
}

/*****************************************************************************/
TEST_F(TestExpressionProxy, one_dimensional_operator_minus_equal_arg_t_value) {
    printemps::model::Model<int, double> model;
    auto& expression_proxy = model.create_expressions("e", 2);
    auto  value            = random_integer();
    ASSERT_THROW(expression_proxy -= value, std::logic_error);
}

/*****************************************************************************/
TEST_F(TestExpressionProxy,
       one_dimensional_operator_minus_equal_arg_t_expression_like) {
    printemps::model::Model<int, double> model;
    auto& expression_proxy       = model.create_expressions("e", 2);
    auto& variable_proxy         = model.create_variable("x");
    auto& expression_proxy_other = model.create_expression("y");
    expression_proxy_other       = variable_proxy;

    /// variable proxy
    ASSERT_THROW(expression_proxy -= variable_proxy, std::logic_error);

    /// variable
    ASSERT_THROW(expression_proxy -= variable_proxy[0], std::logic_error);

    /// expression proxy
    ASSERT_THROW(expression_proxy -= expression_proxy_other, std::logic_error);
}

/*****************************************************************************/
TEST_F(TestExpressionProxy,
       one_dimensional_operator_minus_equal_arg_expression) {
    printemps::model::Model<int, double> model;
    auto& expression_proxy = model.create_expressions("e", 2);

    auto expression =
        printemps::model::Expression<int, double>::create_instance();
    ASSERT_THROW(expression_proxy -= expression, std::logic_error);
}

/*****************************************************************************/
TEST_F(TestExpressionProxy,
       one_dimensional_operator_product_equal_arg_t_value) {
    printemps::model::Model<int, double> model;
    auto& expression_proxy = model.create_expressions("e", 2);
    auto  value            = random_integer();
    ASSERT_THROW(expression_proxy *= value, std::logic_error);
}

/*****************************************************************************/
TEST_F(TestExpressionProxy, one_dimensional_operator_divide_equal_arg_t_value) {
    printemps::model::Model<int, double> model;
    auto& expression_proxy = model.create_expressions("e", 2);
    auto  value            = random_integer();
    ASSERT_THROW(expression_proxy /= value, std::logic_error);
}

/*****************************************************************************/
TEST_F(TestExpressionProxy, one_dimensional_operator_square_bracket) {
    printemps::model::Model<int, double> model;

    auto& expression_proxy = model.create_expressions("e", 2);
    auto  value_0          = random_integer();
    auto  value_1          = random_integer();
    expression_proxy[0]    = value_0;
    expression_proxy[1]    = value_1;
    EXPECT_EQ(value_0, expression_proxy[0].constant_value());
    EXPECT_EQ(value_1, expression_proxy[1].constant_value());
}

/*****************************************************************************/
TEST_F(TestExpressionProxy, one_dimensional_operator_round_bracket) {
    printemps::model::Model<int, double> model;

    auto& expression_proxy = model.create_expressions("e", 2);
    auto  value_0          = random_integer();
    auto  value_1          = random_integer();
    expression_proxy(0)    = value_0;
    expression_proxy(1)    = value_1;
    EXPECT_EQ(value_0, expression_proxy(0).constant_value());
    EXPECT_EQ(value_1, expression_proxy(1).constant_value());
}

/*****************************************************************************/
TEST_F(TestExpressionProxy,
       one_dimensional_operator_round_bracket_with_indices) {
    printemps::model::Model<int, double> model;

    auto& expression_proxy = model.create_expressions("e", 2);
    auto  value_0          = random_integer();
    auto  value_1          = random_integer();
    expression_proxy({0})  = value_0;
    expression_proxy({1})  = value_1;
    EXPECT_EQ(value_0, expression_proxy({0}).constant_value());
    EXPECT_EQ(value_1, expression_proxy({1}).constant_value());
}

/*****************************************************************************/
TEST_F(TestExpressionProxy, two_dimensional_create_instance) {
    printemps::model::Model<int, double> model;

    auto& expression_proxy = model.create_expressions("e", {2, 3});

    /// Check the initial values of the base class members.
    EXPECT_EQ(0, expression_proxy.index());
    EXPECT_EQ(2, expression_proxy.shape()[0]);
    EXPECT_EQ(3, expression_proxy.shape()[1]);
    EXPECT_EQ(3, expression_proxy.strides()[0]);
    EXPECT_EQ(1, expression_proxy.strides()[1]);
    EXPECT_EQ(2, expression_proxy.number_of_dimensions());
    EXPECT_EQ(2 * 3, expression_proxy.number_of_elements());
}

/*****************************************************************************/
TEST_F(TestExpressionProxy, two_dimensional_sensitivities) {
    printemps::model::Model<int, double> model;
    auto& expression_proxy = model.create_expressions("e", {2, 3});
    ASSERT_THROW(auto sensitivities = expression_proxy.sensitivities(),
                 std::logic_error);
}

/*****************************************************************************/
TEST_F(TestExpressionProxy, two_dimensional_constant_value) {
    printemps::model::Model<int, double> model;
    auto& expression_proxy = model.create_expressions("e", {2, 3});
    ASSERT_THROW(
        [[maybe_unused]] auto constant = expression_proxy.constant_value(),
        std::logic_error);
}

/*****************************************************************************/
TEST_F(TestExpressionProxy, two_dimensional_evaluate_arg_void) {
    printemps::model::Model<int, double> model;
    auto& expression_proxy = model.create_expressions("e", {2, 3});
    ASSERT_THROW(expression_proxy.evaluate(), std::logic_error);
}

/*****************************************************************************/
TEST_F(TestExpressionProxy, two_dimensional_evaluate_arg_move) {
    printemps::model::Model<int, double> model;
    auto& expression_proxy = model.create_expressions("e", {2, 3});
    ASSERT_THROW(expression_proxy.evaluate({}), std::logic_error);
}

/*****************************************************************************/
TEST_F(TestExpressionProxy, two_dimensional_update_arg_void) {
    printemps::model::Model<int, double> model;
    auto& expression_proxy = model.create_expressions("e", {2, 3});
    ASSERT_THROW(expression_proxy.update(), std::logic_error);
}

/*****************************************************************************/
TEST_F(TestExpressionProxy, two_dimensional_update_arg_move) {
    printemps::model::Model<int, double> model;
    auto& expression_proxy = model.create_expressions("e", {2, 3});
    ASSERT_THROW(expression_proxy.update({}), std::logic_error);
}

/*****************************************************************************/
TEST_F(TestExpressionProxy, two_dimensional_value) {
    printemps::model::Model<int, double> model;
    auto& expression_proxy = model.create_expressions("e", {2, 3});
    ASSERT_THROW([[maybe_unused]] auto value = expression_proxy.value(),
                 std::logic_error);
}

/*****************************************************************************/
TEST_F(TestExpressionProxy, two_dimensional_set_name) {
    printemps::model::Model<int, double> model;

    auto& expression_proxy = model.create_expressions("e", {2, 3});

    ASSERT_THROW(expression_proxy.set_name("_e"), std::logic_error);
    ASSERT_THROW(expression_proxy.name(), std::logic_error);
}

/*****************************************************************************/
TEST_F(TestExpressionProxy, two_dimensional_name) {
    /// This method is tested in two_dimensional_set_name().
}

/*****************************************************************************/
TEST_F(TestExpressionProxy, two_dimensional_flat_indexed_expressions_arg_void) {
    printemps::model::Model<int, double> model;
    auto& expression_proxy = model.create_expressions("e", {2, 3});

    auto variable_0 =
        printemps::model::Variable<int, double>::create_instance();
    auto variable_1 =
        printemps::model::Variable<int, double>::create_instance();

    auto sensitivity_0 = random_integer();
    auto sensitivity_1 = random_integer();
    auto constant      = random_integer();

    expression_proxy[0] =
        sensitivity_0 * variable_0 + sensitivity_1 * variable_1 + constant;
    expression_proxy[2 * 3 - 1] = expression_proxy[0] * 2;

    for (auto&& expression : expression_proxy.flat_indexed_expressions()) {
        expression *= 2;
    }

    EXPECT_EQ(2 * sensitivity_0,
              expression_proxy.flat_indexed_expressions()[0].sensitivities().at(
                  &variable_0));

    EXPECT_EQ(2 * sensitivity_1,
              expression_proxy.flat_indexed_expressions()[0].sensitivities().at(
                  &variable_1));

    EXPECT_EQ(2 * constant,
              expression_proxy.flat_indexed_expressions()[0].constant_value());

    EXPECT_EQ(4 * sensitivity_0,
              expression_proxy.flat_indexed_expressions()[2 * 3 - 1]
                  .sensitivities()
                  .at(&variable_0));

    EXPECT_EQ(4 * sensitivity_1,
              expression_proxy.flat_indexed_expressions()[2 * 3 - 1]
                  .sensitivities()
                  .at(&variable_1));

    EXPECT_EQ(4 * constant,
              expression_proxy.flat_indexed_expressions()[2 * 3 - 1]
                  .constant_value());
}

/*****************************************************************************/
TEST_F(TestExpressionProxy, two_dimensional_flat_indexed_expressions_arg_int) {
    printemps::model::Model<int, double> model;
    auto& expression_proxy = model.create_expressions("e", {2, 3});

    auto variable_0 =
        printemps::model::Variable<int, double>::create_instance();
    auto variable_1 =
        printemps::model::Variable<int, double>::create_instance();

    auto sensitivity_0 = random_integer();
    auto sensitivity_1 = random_integer();
    auto constant      = random_integer();

    expression_proxy[0] =
        sensitivity_0 * variable_0 + sensitivity_1 * variable_1 + constant;
    expression_proxy[2 * 3 - 1] = expression_proxy[0] * 2;

    expression_proxy.flat_indexed_expressions(0) *= 2;
    expression_proxy.flat_indexed_expressions(2 * 3 - 1) *= 2;

    EXPECT_EQ(2 * sensitivity_0,
              expression_proxy.flat_indexed_expressions(0).sensitivities().at(
                  &variable_0));

    EXPECT_EQ(2 * sensitivity_1,
              expression_proxy.flat_indexed_expressions(0).sensitivities().at(
                  &variable_1));

    EXPECT_EQ(2 * constant,
              expression_proxy.flat_indexed_expressions(0).constant_value());

    EXPECT_EQ(
        4 * sensitivity_0,
        expression_proxy.flat_indexed_expressions(2 * 3 - 1).sensitivities().at(
            &variable_0));

    EXPECT_EQ(
        4 * sensitivity_1,
        expression_proxy.flat_indexed_expressions(2 * 3 - 1).sensitivities().at(
            &variable_1));

    EXPECT_EQ(
        4 * constant,
        expression_proxy.flat_indexed_expressions(2 * 3 - 1).constant_value());
}

/*****************************************************************************/
TEST_F(TestExpressionProxy, two_dimensional_export_values_and_names) {
    printemps::model::Model<int, double> model;
    auto& expression_proxy = model.create_expressions("e", {2, 3});

    auto variable_0 =
        printemps::model::Variable<int, double>::create_instance();
    auto variable_1 =
        printemps::model::Variable<int, double>::create_instance();

    auto sensitivity_0 = random_integer();
    auto sensitivity_1 = random_integer();
    auto constant      = random_integer();

    expression_proxy[0] =
        sensitivity_0 * variable_0 + sensitivity_1 * variable_1 + constant;
    expression_proxy[2 * 3 - 1] = expression_proxy[0] * 2;

    auto value_0 = random_integer();
    auto value_1 = random_integer();

    variable_0 = value_0;
    variable_1 = value_1;

    for (auto&& expression : expression_proxy.flat_indexed_expressions()) {
        expression.update();
    }
    auto expected_value_0 =
        sensitivity_0 * value_0 + sensitivity_1 * value_1 + constant;
    auto expected_value_1 = expected_value_0 * 2;

    EXPECT_EQ(expected_value_0,
              expression_proxy.export_values_and_names().values(0, 0));
    EXPECT_EQ(expected_value_1,
              expression_proxy.export_values_and_names().values(1, 2));
}

/*****************************************************************************/
TEST_F(TestExpressionProxy, two_dimensional_to_expression) {
    printemps::model::Model<int, double> model;
    auto& expression_proxy = model.create_expressions("e", {2, 3});
    ASSERT_THROW(auto expression = expression_proxy.to_expression(),
                 std::logic_error);
}

/*****************************************************************************/
TEST_F(TestExpressionProxy, two_dimensional_sum_arg_void) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy   = model.create_variables("x", {2, 3});
    auto& expression_proxy = model.create_expressions("e", {2, 3});

    for (auto i = 0; i < variable_proxy.number_of_elements(); i++) {
        expression_proxy[i] = variable_proxy[i];
    }

    auto expression =
        printemps::model::Expression<int, double>::create_instance();
    expression = expression_proxy.sum();

    for (auto i = 0; i < variable_proxy.number_of_elements(); i++) {
        variable_proxy[i] = 1;
    }

    EXPECT_EQ(1, expression.sensitivities().at(&(variable_proxy[0])));
    EXPECT_EQ(1, expression.sensitivities().at(&(variable_proxy[2 * 3 - 1])));
    EXPECT_EQ(2 * 3, expression.evaluate());
}

/*****************************************************************************/
TEST_F(TestExpressionProxy, two_dimensional_sum_arg_indices) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy   = model.create_variables("x", {2, 3});
    auto& expression_proxy = model.create_expressions("e", {2, 3});

    for (auto i = 0; i < variable_proxy.number_of_elements(); i++) {
        expression_proxy[i] = variable_proxy[i];
    }

    auto expression_0 =
        printemps::model::Expression<int, double>::create_instance();
    auto expression_1 =
        printemps::model::Expression<int, double>::create_instance();
    auto expression_01 =
        printemps::model::Expression<int, double>::create_instance();

    expression_0  = expression_proxy.sum({printemps::model::Range::All, 0});
    expression_1  = expression_proxy.sum({0, printemps::model::Range::All});
    expression_01 = expression_proxy.sum(
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

    ASSERT_THROW(expression_proxy.sum({printemps::model::Range::All}),
                 std::logic_error);
    ASSERT_THROW(expression_proxy.sum({printemps::model::Range::All, 0, 0}),
                 std::logic_error);
}

/*****************************************************************************/
TEST_F(TestExpressionProxy, two_dimensional_dot_arg_vector) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy   = model.create_variables("x", {2, 3});
    auto& expression_proxy = model.create_expressions("e", {2, 3});

    for (auto i = 0; i < variable_proxy.number_of_elements(); i++) {
        expression_proxy[i] = variable_proxy[i];
    }

    std::vector<double> sensitivities;
    for (auto i = 0; i < variable_proxy.number_of_elements(); i++) {
        sensitivities.push_back(random_integer());
    }
    ASSERT_THROW(expression_proxy.dot(sensitivities), std::logic_error);
}

/*****************************************************************************/
TEST_F(TestExpressionProxy, two_dimensional_dot_arg_indice_vector) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy   = model.create_variables("x", {2, 3});
    auto& expression_proxy = model.create_expressions("e", {2, 3});

    for (auto i = 0; i < variable_proxy.number_of_elements(); i++) {
        expression_proxy[i] = variable_proxy[i];
    }

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

    expression_0 = expression_proxy.dot({printemps::model::Range::All, 0},
                                        sensitivities_0);
    expression_1 = expression_proxy.dot({0, printemps::model::Range::All},
                                        sensitivities_1);

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
        expression_proxy.dot({printemps::model::Range::All}, sensitivities_0),
        std::logic_error);
    ASSERT_THROW(expression_proxy.dot({printemps::model::Range::All,
                                       printemps::model::Range::All},
                                      sensitivities_01),
                 std::logic_error);
    ASSERT_THROW(expression_proxy.dot({printemps::model::Range::All, 0, 0},
                                      sensitivities_0),
                 std::logic_error);
}

/*****************************************************************************/
TEST_F(TestExpressionProxy, two_dimensional_is_enabled) {
    printemps::model::Model<int, double> model;
    auto& expression_proxy = model.create_expressions("e", {2, 3});
    expression_proxy.disable();
    ASSERT_THROW(expression_proxy.is_enabled(), std::logic_error);
    EXPECT_EQ(false, expression_proxy[0].is_enabled());
    EXPECT_EQ(false, expression_proxy[2 * 3 - 1].is_enabled());

    expression_proxy.enable();
    ASSERT_THROW(expression_proxy.is_enabled(), std::logic_error);
    EXPECT_EQ(true, expression_proxy[0].is_enabled());
    EXPECT_EQ(true, expression_proxy[2 * 3 - 1].is_enabled());

    expression_proxy.disable();
    ASSERT_THROW(expression_proxy.is_enabled(), std::logic_error);
    EXPECT_EQ(false, expression_proxy[0].is_enabled());
    EXPECT_EQ(false, expression_proxy[2 * 3 - 1].is_enabled());
}

/*****************************************************************************/
TEST_F(TestExpressionProxy, two_dimensional_enable) {
    /// This method is tested in two_dimensional_is_enabled().
}

/*****************************************************************************/
TEST_F(TestExpressionProxy, two_dimensional_disable) {
    /// This method is tested in two_dimensional_is_enabled().
}

/*****************************************************************************/
TEST_F(TestExpressionProxy, two_dimensional_operator_plus) {
    printemps::model::Model<int, double> model;
    auto& expression_proxy = model.create_expressions("e", {2, 3});
    ASSERT_THROW(auto expression = +expression_proxy, std::logic_error);
}

/*****************************************************************************/
TEST_F(TestExpressionProxy, two_dimensional_operator_minus) {
    printemps::model::Model<int, double> model;
    auto& expression_proxy = model.create_expressions("e", {2, 3});
    ASSERT_THROW(auto expression = -expression_proxy, std::logic_error);
}

/*****************************************************************************/
TEST_F(TestExpressionProxy, two_dimensional_operator_equal_arg_t_value) {
    printemps::model::Model<int, double> model;
    auto& expression_proxy = model.create_expressions("e", {2, 3});
    auto  value            = random_integer();
    ASSERT_THROW(expression_proxy = value, std::logic_error);
}

/*****************************************************************************/
TEST_F(TestExpressionProxy,
       two_dimensional_operator_equal_arg_t_expression_like) {
    printemps::model::Model<int, double> model;
    auto& expression_proxy       = model.create_expressions("e", {2, 3});
    auto& variable_proxy         = model.create_variable("x");
    auto& expression_proxy_other = model.create_expression("y");
    expression_proxy_other       = variable_proxy;

    /// variable proxy
    ASSERT_THROW(expression_proxy = variable_proxy, std::logic_error);

    /// variable
    ASSERT_THROW(expression_proxy = variable_proxy[0], std::logic_error);

    /// expression proxy
    ASSERT_THROW(expression_proxy = expression_proxy_other, std::logic_error);
}

/*****************************************************************************/
TEST_F(TestExpressionProxy, two_dimensional_operator_equal_expression) {
    printemps::model::Model<int, double> model;
    auto& expression_proxy = model.create_expressions("e", {2, 3});

    auto expression =
        printemps::model::Expression<int, double>::create_instance();
    ASSERT_THROW(expression_proxy = expression, std::logic_error);
}

/*****************************************************************************/
TEST_F(TestExpressionProxy, two_dimensional_operator_plus_equal_arg_t_value) {
    printemps::model::Model<int, double> model;
    auto& expression_proxy = model.create_expressions("e", {2, 3});
    auto  value            = random_integer();
    ASSERT_THROW(expression_proxy += value, std::logic_error);
}

/*****************************************************************************/
TEST_F(TestExpressionProxy,
       two_dimensional_operator_plus_equal_arg_t_expression_like) {
    printemps::model::Model<int, double> model;
    auto& expression_proxy       = model.create_expressions("e", {2, 3});
    auto& variable_proxy         = model.create_variable("x");
    auto& expression_proxy_other = model.create_expression("y");
    expression_proxy_other       = variable_proxy;

    /// variable proxy
    ASSERT_THROW(expression_proxy += variable_proxy, std::logic_error);

    /// variable
    ASSERT_THROW(expression_proxy += variable_proxy[0], std::logic_error);

    /// expression proxy
    ASSERT_THROW(expression_proxy += expression_proxy_other, std::logic_error);
}

/*****************************************************************************/
TEST_F(TestExpressionProxy,
       two_dimensional_operator_plus_equal_arg_expression) {
    printemps::model::Model<int, double> model;
    auto& expression_proxy = model.create_expressions("e", {2, 3});

    auto expression =
        printemps::model::Expression<int, double>::create_instance();
    ASSERT_THROW(expression_proxy += expression, std::logic_error);
}

/*****************************************************************************/
TEST_F(TestExpressionProxy, two_dimensional_operator_minus_equal_arg_t_value) {
    printemps::model::Model<int, double> model;
    auto& expression_proxy = model.create_expressions("e", {2, 3});
    auto  value            = random_integer();
    ASSERT_THROW(expression_proxy -= value, std::logic_error);
}

/*****************************************************************************/
TEST_F(TestExpressionProxy,
       two_dimensional_operator_minus_equal_arg_t_expression_like) {
    printemps::model::Model<int, double> model;
    auto& expression_proxy       = model.create_expressions("e", {2, 3});
    auto& variable_proxy         = model.create_variable("x");
    auto& expression_proxy_other = model.create_expression("y");
    expression_proxy_other       = variable_proxy;

    /// variable proxy
    ASSERT_THROW(expression_proxy -= variable_proxy, std::logic_error);

    /// variable
    ASSERT_THROW(expression_proxy -= variable_proxy[0], std::logic_error);

    /// expression proxy
    ASSERT_THROW(expression_proxy -= expression_proxy_other, std::logic_error);
}

/*****************************************************************************/
TEST_F(TestExpressionProxy,
       two_dimensional_operator_minus_equal_arg_expression) {
    printemps::model::Model<int, double> model;
    auto& expression_proxy = model.create_expressions("e", {2, 3});

    auto expression =
        printemps::model::Expression<int, double>::create_instance();
    ASSERT_THROW(expression_proxy -= expression, std::logic_error);
}

/*****************************************************************************/
TEST_F(TestExpressionProxy,
       two_dimensional_operator_product_equal_arg_t_value) {
    printemps::model::Model<int, double> model;
    auto& expression_proxy = model.create_expressions("e", {2, 3});
    auto  value            = random_integer();
    ASSERT_THROW(expression_proxy *= value, std::logic_error);
}

/*****************************************************************************/
TEST_F(TestExpressionProxy, two_dimensional_operator_divide_equal_arg_t_value) {
    printemps::model::Model<int, double> model;
    auto& expression_proxy = model.create_expressions("e", {2, 3});
    auto  value            = random_integer();
    ASSERT_THROW(expression_proxy /= value, std::logic_error);
}

/*****************************************************************************/
TEST_F(TestExpressionProxy, two_dimensional_operator_square_bracket) {
    printemps::model::Model<int, double> model;

    auto& expression_proxy      = model.create_expressions("x", {2, 3});
    auto  value_0               = random_integer();
    auto  value_1               = random_integer();
    expression_proxy[0]         = value_0;
    expression_proxy[2 * 3 - 1] = value_1;
    EXPECT_EQ(value_0, expression_proxy[0].constant_value());
    EXPECT_EQ(value_1, expression_proxy[2 * 3 - 1].constant_value());
}

/*****************************************************************************/
TEST_F(TestExpressionProxy, two_dimensional_operator_round_bracket) {
    printemps::model::Model<int, double> model;

    auto& expression_proxy = model.create_expressions("x", {2, 3});
    auto  value_0          = random_integer();
    auto  value_1          = random_integer();
    expression_proxy(0, 0) = value_0;
    expression_proxy(1, 2) = value_1;
    EXPECT_EQ(value_0, expression_proxy(0, 0).constant_value());
    EXPECT_EQ(value_1, expression_proxy(1, 2).constant_value());
}

/*****************************************************************************/
TEST_F(TestExpressionProxy,
       two_dimensional_operator_round_bracket_with_indices) {
    printemps::model::Model<int, double> model;

    auto& expression_proxy   = model.create_expressions("x", {2, 3});
    auto  value_0            = random_integer();
    auto  value_1            = random_integer();
    expression_proxy({0, 0}) = value_0;
    expression_proxy({1, 2}) = value_1;
    EXPECT_EQ(value_0, expression_proxy({0, 0}).constant_value());
    EXPECT_EQ(value_1, expression_proxy({1, 2}).constant_value());
}

/*****************************************************************************/
TEST_F(TestExpressionProxy, three_dimensional_create_instance) {
    printemps::model::Model<int, double> model;

    auto& expression_proxy = model.create_expressions("e", {2, 3, 4});

    /// Check the initial values of the base class members.
    EXPECT_EQ(0, expression_proxy.index());
    EXPECT_EQ(2, expression_proxy.shape()[0]);
    EXPECT_EQ(3, expression_proxy.shape()[1]);
    EXPECT_EQ(4, expression_proxy.shape()[2]);
    EXPECT_EQ(12, expression_proxy.strides()[0]);
    EXPECT_EQ(4, expression_proxy.strides()[1]);
    EXPECT_EQ(1, expression_proxy.strides()[2]);
    EXPECT_EQ(3, expression_proxy.number_of_dimensions());
    EXPECT_EQ(2 * 3 * 4, expression_proxy.number_of_elements());
}

/*****************************************************************************/
TEST_F(TestExpressionProxy, three_dimensional_operator_round_bracket) {
    printemps::model::Model<int, double> model;

    auto& expression_proxy    = model.create_expressions("e", {2, 3, 4});
    auto  value_0             = random_integer();
    auto  value_1             = random_integer();
    expression_proxy(0, 0, 0) = value_0;
    expression_proxy(1, 2, 3) = value_1;
    EXPECT_EQ(value_0, expression_proxy(0, 0, 0).constant_value());
    EXPECT_EQ(value_1, expression_proxy(1, 2, 3).constant_value());
}

/*****************************************************************************/
TEST_F(TestExpressionProxy,
       three_dimensional_operator_round_bracket_with_indices) {
    printemps::model::Model<int, double> model;

    auto& expression_proxy      = model.create_expressions("e", {2, 3, 4});
    auto  value_0               = random_integer();
    auto  value_1               = random_integer();
    expression_proxy({0, 0, 0}) = value_0;
    expression_proxy({1, 2, 3}) = value_1;
    EXPECT_EQ(value_0, expression_proxy({0, 0, 0}).constant_value());
    EXPECT_EQ(value_1, expression_proxy({1, 2, 3}).constant_value());
}

/*****************************************************************************/
TEST_F(TestExpressionProxy, four_dimensional_create_instance) {
    printemps::model::Model<int, double> model;

    auto& expression_proxy = model.create_expressions("e", {2, 3, 4, 5});

    /// Check the initial values of the base class members.
    EXPECT_EQ(0, expression_proxy.index());
    EXPECT_EQ(2, expression_proxy.shape()[0]);
    EXPECT_EQ(3, expression_proxy.shape()[1]);
    EXPECT_EQ(4, expression_proxy.shape()[2]);
    EXPECT_EQ(5, expression_proxy.shape()[3]);
    EXPECT_EQ(60, expression_proxy.strides()[0]);
    EXPECT_EQ(20, expression_proxy.strides()[1]);
    EXPECT_EQ(5, expression_proxy.strides()[2]);
    EXPECT_EQ(1, expression_proxy.strides()[3]);
    EXPECT_EQ(4, expression_proxy.number_of_dimensions());
    EXPECT_EQ(2 * 3 * 4 * 5, expression_proxy.number_of_elements());
}

/*****************************************************************************/
TEST_F(TestExpressionProxy, four_dimensional_operator_round_bracket) {
    printemps::model::Model<int, double> model;

    auto& expression_proxy       = model.create_expressions("e", {2, 3, 4, 5});
    auto  value_0                = random_integer();
    auto  value_1                = random_integer();
    expression_proxy(0, 0, 0, 0) = value_0;
    expression_proxy(1, 2, 3, 4) = value_1;
    EXPECT_EQ(value_0, expression_proxy(0, 0, 0, 0).constant_value());
    EXPECT_EQ(value_1, expression_proxy(1, 2, 3, 4).constant_value());
}

/*****************************************************************************/
TEST_F(TestExpressionProxy,
       four_dimensional_operator_round_bracket_with_indices) {
    printemps::model::Model<int, double> model;

    auto& expression_proxy = model.create_expressions("e", {2, 3, 4, 5});
    auto  value_0          = random_integer();
    auto  value_1          = random_integer();
    expression_proxy({0, 0, 0, 0}) = value_0;
    expression_proxy({1, 2, 3, 4}) = value_1;
    EXPECT_EQ(value_0, expression_proxy({0, 0, 0, 0}).constant_value());
    EXPECT_EQ(value_1, expression_proxy({1, 2, 3, 4}).constant_value());
}

/*****************************************************************************/
}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/