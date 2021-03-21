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
class TestExpression : public ::testing::Test {
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
TEST_F(TestExpression, initialize) {
    auto expression =
        printemps::model::Expression<int, double>::create_instance();

    /// Check the initial values of the base class members.
    EXPECT_EQ(0, expression.proxy_index());
    EXPECT_EQ(0, expression.flat_index());
    EXPECT_EQ(0, expression.multi_dimensional_index()[0]);
    EXPECT_EQ("", expression.name());

    /// Check the initial values of the derived class members.
    EXPECT_EQ(0, expression.constant_value());
    EXPECT_EQ(0, expression.value());
    EXPECT_EQ(true, expression.sensitivities().empty());
    EXPECT_EQ(true, expression.is_enabled());
}

/*****************************************************************************/
TEST_F(TestExpression, set_sensitivities) {
    auto expression =
        printemps::model::Expression<int, double>::create_instance();

    auto variable_0 =
        printemps::model::Variable<int, double>::create_instance();
    auto variable_1 =
        printemps::model::Variable<int, double>::create_instance();
    std::unordered_map<printemps::model::Variable<int, double>*, double>
        sensitivities;

    auto sensitivity_0 = random_integer();
    auto sensitivity_1 = random_integer();

    sensitivities[&variable_0] = sensitivity_0;
    sensitivities[&variable_1] = sensitivity_1;

    expression.set_sensitivities(sensitivities);

    EXPECT_EQ(2, static_cast<int>(expression.sensitivities().size()));
    EXPECT_EQ(sensitivity_0, expression.sensitivities().at(&variable_0));
    EXPECT_EQ(sensitivity_1, expression.sensitivities().at(&variable_1));
}

/*****************************************************************************/
TEST_F(TestExpression, sensitivities) {
    /// This method is tested in set_sensitivities().
}

/*****************************************************************************/
TEST_F(TestExpression, setup_fixed_sensitivities) {
    /// This method is tested in test_fixed_size_hash_map().
}

/*****************************************************************************/
TEST_F(TestExpression, constant_value) {
    auto expression =
        printemps::model::Expression<int, double>::create_instance();

    auto value = random_integer();
    expression = value;
    EXPECT_EQ(value, expression.constant_value());
}

/*****************************************************************************/
TEST_F(TestExpression, evaluate_arg_void) {
    auto expression =
        printemps::model::Expression<int, double>::create_instance();

    auto variable_0 =
        printemps::model::Variable<int, double>::create_instance();
    auto variable_1 =
        printemps::model::Variable<int, double>::create_instance();
    std::unordered_map<printemps::model::Variable<int, double>*, double>
        sensitivities;

    auto sensitivity_0 = random_integer();
    auto sensitivity_1 = random_integer();
    auto constant      = random_integer();

    expression =
        sensitivity_0 * variable_0 + sensitivity_1 * variable_1 + constant;

    auto v_value_0 = random_integer();
    auto v_value_1 = random_integer();

    variable_0 = v_value_0;
    variable_1 = v_value_1;

    auto expected_result =
        sensitivity_0 * v_value_0 + sensitivity_1 * v_value_1 + constant;

    EXPECT_EQ(expected_result, expression.evaluate());
}

/*****************************************************************************/
TEST_F(TestExpression, evaluate_arg_move) {
    auto expression =
        printemps::model::Expression<int, double>::create_instance();

    auto variable_0 =
        printemps::model::Variable<int, double>::create_instance();
    auto variable_1 =
        printemps::model::Variable<int, double>::create_instance();
    std::unordered_map<printemps::model::Variable<int, double>*, double>
        sensitivities;

    auto sensitivity_0 = random_integer();
    auto sensitivity_1 = random_integer();
    auto constant      = random_integer();

    expression =
        sensitivity_0 * variable_0 + sensitivity_1 * variable_1 + constant;
    expression.setup_fixed_sensitivities();

    auto v_value_0 = random_integer();
    auto v_value_1 = random_integer();

    variable_0 = v_value_0;
    variable_1 = v_value_1;

    expression.update();

    printemps::neighborhood::Move<int, double> move;
    v_value_0 = random_integer();
    v_value_1 = random_integer();

    move.alterations.emplace_back(&variable_0, v_value_0);
    move.alterations.emplace_back(&variable_1, v_value_1);

    auto expected_result =
        sensitivity_0 * v_value_0 + sensitivity_1 * v_value_1 + constant;

    EXPECT_EQ(expected_result, expression.evaluate(move));
}

/*****************************************************************************/
TEST_F(TestExpression, update_arg_void) {
    auto expression =
        printemps::model::Expression<int, double>::create_instance();

    auto variable_0 =
        printemps::model::Variable<int, double>::create_instance();
    auto variable_1 =
        printemps::model::Variable<int, double>::create_instance();
    std::unordered_map<printemps::model::Variable<int, double>*, double>
        sensitivities;

    auto sensitivity_0 = random_integer();
    auto sensitivity_1 = random_integer();
    auto constant      = random_integer();

    expression =
        sensitivity_0 * variable_0 + sensitivity_1 * variable_1 + constant;

    auto v_value_0 = random_integer();
    auto v_value_1 = random_integer();

    variable_0 = v_value_0;
    variable_1 = v_value_1;

    expression.update();
    auto expected_result =
        sensitivity_0 * v_value_0 + sensitivity_1 * v_value_1 + constant;

    EXPECT_EQ(expected_result, expression.value());
}

/*****************************************************************************/
TEST_F(TestExpression, update_arg_move) {
    auto expression =
        printemps::model::Expression<int, double>::create_instance();

    auto variable_0 =
        printemps::model::Variable<int, double>::create_instance();
    auto variable_1 =
        printemps::model::Variable<int, double>::create_instance();
    std::unordered_map<printemps::model::Variable<int, double>*, double>
        sensitivities;

    auto sensitivity_0 = random_integer();
    auto sensitivity_1 = random_integer();
    auto constant      = random_integer();

    expression =
        sensitivity_0 * variable_0 + sensitivity_1 * variable_1 + constant;
    expression.setup_fixed_sensitivities();

    auto v_value_0 = random_integer();
    auto v_value_1 = random_integer();

    variable_0 = v_value_0;
    variable_1 = v_value_1;

    expression.update();
    printemps::neighborhood::Move<int, double> move;
    v_value_0 = random_integer();
    v_value_1 = random_integer();

    move.alterations.emplace_back(&variable_0, v_value_0);
    move.alterations.emplace_back(&variable_1, v_value_1);

    expression.update(move);
    auto expected_result =
        sensitivity_0 * v_value_0 + sensitivity_1 * v_value_1 + constant;

    EXPECT_EQ(expected_result, expression.value());
}

/*****************************************************************************/
TEST_F(TestExpression, value) {
    /// tested in other cases
}

/*****************************************************************************/
TEST_F(TestExpression, self) {
    auto expression =
        printemps::model::Expression<int, double>::create_instance();

    auto variable_0 =
        printemps::model::Variable<int, double>::create_instance();
    auto variable_1 =
        printemps::model::Variable<int, double>::create_instance();
    std::unordered_map<printemps::model::Variable<int, double>*, double>
        sensitivities;

    auto sensitivity_0 = random_integer();
    auto sensitivity_1 = random_integer();
    auto constant      = random_integer();

    expression =
        sensitivity_0 * variable_0 + sensitivity_1 * variable_1 + constant;

    EXPECT_EQ(expression.value(), expression.self().value());
    EXPECT_EQ(expression.constant_value(), expression.self().constant_value());
    EXPECT_EQ(expression.sensitivities().size(),
              expression.self().sensitivities().size());
    EXPECT_EQ(expression.sensitivities().at(&variable_0),
              expression.self().sensitivities().at(&variable_0));
    EXPECT_EQ(expression.sensitivities().at(&variable_1),
              expression.self().sensitivities().at(&variable_1));
}

/*****************************************************************************/
TEST_F(TestExpression, is_enabled) {
    auto expression =
        printemps::model::Expression<int, double>::create_instance();
    expression.disable();
    EXPECT_EQ(false, expression.is_enabled());

    expression.enable();
    EXPECT_EQ(true, expression.is_enabled());

    expression.disable();
    EXPECT_EQ(false, expression.is_enabled());
}

/*****************************************************************************/
TEST_F(TestExpression, enable) {
    /// This method is tested in is_enabled().
}

/*****************************************************************************/
TEST_F(TestExpression, disable) {
    /// This method is tested in is_enabled().
}

/*****************************************************************************/
TEST_F(TestExpression, operator_plus) {
    auto expression =
        printemps::model::Expression<int, double>::create_instance();

    auto variable_0 =
        printemps::model::Variable<int, double>::create_instance();
    auto variable_1 =
        printemps::model::Variable<int, double>::create_instance();
    std::unordered_map<printemps::model::Variable<int, double>*, double>
        sensitivities;

    auto sensitivity_0 = random_integer();
    auto sensitivity_1 = random_integer();
    auto constant      = random_integer();

    expression =
        sensitivity_0 * variable_0 + sensitivity_1 * variable_1 + constant;
    expression += constant;

    EXPECT_EQ(expression.value(), (+expression).value());
    EXPECT_EQ(expression.constant_value(), (+expression).constant_value());
    EXPECT_EQ(expression.sensitivities().size(),
              (+expression).sensitivities().size());
    EXPECT_EQ(expression.sensitivities().at(&variable_0),
              (+expression).sensitivities().at(&variable_0));
    EXPECT_EQ(expression.sensitivities().at(&variable_1),
              (+expression).sensitivities().at(&variable_1));
}

/*****************************************************************************/
TEST_F(TestExpression, operator_minus) {
    auto expression =
        printemps::model::Expression<int, double>::create_instance();

    auto variable_0 =
        printemps::model::Variable<int, double>::create_instance();
    auto variable_1 =
        printemps::model::Variable<int, double>::create_instance();
    std::unordered_map<printemps::model::Variable<int, double>*, double>
        sensitivities;

    auto sensitivity_0 = random_integer();
    auto sensitivity_1 = random_integer();
    auto constant      = random_integer();

    expression =
        sensitivity_0 * variable_0 + sensitivity_1 * variable_1 + constant;

    EXPECT_EQ(-expression.value(), (-expression).value());
    EXPECT_EQ(-expression.constant_value(), (-expression).constant_value());
    EXPECT_EQ(expression.sensitivities().size(),
              (-expression).sensitivities().size());
    EXPECT_EQ(-expression.sensitivities().at(&variable_0),
              (-expression).sensitivities().at(&variable_0));
    EXPECT_EQ(-expression.sensitivities().at(&variable_1),
              (-expression).sensitivities().at(&variable_1));
}

/*****************************************************************************/
TEST_F(TestExpression, operator_equal_arg_t_value) {
    auto expression =
        printemps::model::Expression<int, double>::create_instance();

    auto value = random_integer();

    EXPECT_EQ(value, (expression = value).constant_value());
    EXPECT_EQ(value, expression.constant_value());
}

/*****************************************************************************/
TEST_F(TestExpression, operator_equal_arg_t_expression_like) {
    auto expression =
        printemps::model::Expression<int, double>::create_instance();

    printemps::model::Model<int, double> model;

    auto& variable_proxy   = model.create_variable("x");
    auto& expression_proxy = model.create_expression("y");
    expression_proxy       = variable_proxy;

    /// variable proxy
    EXPECT_EQ(
        1,
        (expression = variable_proxy).sensitivities().at(&variable_proxy[0]));
    EXPECT_EQ(1, expression.sensitivities().at(&variable_proxy[0]));

    /// variable
    EXPECT_EQ(1, (expression = variable_proxy[0])
                     .sensitivities()
                     .at(&variable_proxy[0]));
    EXPECT_EQ(1, expression.sensitivities().at(&variable_proxy[0]));

    /// expression proxy
    EXPECT_EQ(
        1,
        (expression = expression_proxy).sensitivities().at(&variable_proxy[0]));
    EXPECT_EQ(1, expression.sensitivities().at(&variable_proxy[0]));
}

/*****************************************************************************/
TEST_F(TestExpression, operator_plus_equal_arg_t_value) {
    auto expression =
        printemps::model::Expression<int, double>::create_instance();

    auto value_0 = random_integer();
    auto value_1 = random_integer();
    EXPECT_EQ(value_0, (expression += value_0).constant_value());
    EXPECT_EQ(value_0, expression.constant_value());
    EXPECT_EQ(value_0 + value_1, (expression += value_1).constant_value());
    EXPECT_EQ(value_0 + value_1, expression.constant_value());
}

/*****************************************************************************/
TEST_F(TestExpression, operator_plus_equal_arg_t_expression_like) {
    auto expression =
        printemps::model::Expression<int, double>::create_instance();

    printemps::model::Model<int, double> model;

    auto& variable_proxy   = model.create_variable("x");
    auto& expression_proxy = model.create_expression("y");
    expression_proxy       = variable_proxy;

    /// variable proxy
    EXPECT_EQ(
        1,
        (expression += variable_proxy).sensitivities().at(&variable_proxy[0]));
    EXPECT_EQ(1, expression.sensitivities().at(&variable_proxy[0]));

    /// variable
    EXPECT_EQ(2, (expression += variable_proxy[0])
                     .sensitivities()
                     .at(&variable_proxy[0]));
    EXPECT_EQ(2, expression.sensitivities().at(&variable_proxy[0]));

    /// expression proxy
    EXPECT_EQ(3, (expression += expression_proxy)
                     .sensitivities()
                     .at(&variable_proxy[0]));
    EXPECT_EQ(3, expression.sensitivities().at(&variable_proxy[0]));
}

/*****************************************************************************/
TEST_F(TestExpression, operator_plus_equal_arg_expression) {
    auto expression_0 =
        printemps::model::Expression<int, double>::create_instance();
    auto expression_1 =
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

    expression_0 = sensitivity_0_0 * variable_0 + sensitivity_1_0 * variable_1 +
                   constant_0;
    expression_1 = sensitivity_0_1 * variable_0 + sensitivity_1_1 * variable_1 +
                   constant_1;

    EXPECT_EQ(sensitivity_0_0 + sensitivity_0_1,
              (expression_0 += expression_1).sensitivities().at(&variable_0));
    EXPECT_EQ(sensitivity_0_0 + sensitivity_0_1,
              expression_0.sensitivities().at(&variable_0));
    EXPECT_EQ(sensitivity_1_0 + sensitivity_1_1,
              expression_0.sensitivities().at(&variable_1));
    EXPECT_EQ(constant_0 + constant_1, expression_0.constant_value());
}

/*****************************************************************************/
TEST_F(TestExpression, operator_minus_equal_arg_t_value) {
    auto expression =
        printemps::model::Expression<int, double>::create_instance();

    auto value_0 = random_integer();
    auto value_1 = random_integer();
    EXPECT_EQ(-value_0, (expression -= value_0).constant_value());
    EXPECT_EQ(-value_0, expression.constant_value());
    EXPECT_EQ(-value_0 - value_1, (expression -= value_1).constant_value());
    EXPECT_EQ(-value_0 - value_1, expression.constant_value());
}

/*****************************************************************************/
TEST_F(TestExpression, operator_minus_equal_arg_t_expression_like) {
    auto expression =
        printemps::model::Expression<int, double>::create_instance();

    printemps::model::Model<int, double> model;

    auto& variable_proxy   = model.create_variable("x");
    auto& expression_proxy = model.create_expression("y");
    expression_proxy       = variable_proxy;

    /// variable proxy
    EXPECT_EQ(
        -1,
        (expression -= variable_proxy).sensitivities().at(&variable_proxy[0]));
    EXPECT_EQ(-1, expression.sensitivities().at(&variable_proxy[0]));

    /// variable
    EXPECT_EQ(
        -2,
        (expression -= variable_proxy).sensitivities().at(&variable_proxy[0]));
    EXPECT_EQ(-2, expression.sensitivities().at(&variable_proxy[0]));

    /// expression proxy
    EXPECT_EQ(-3, (expression -= expression_proxy)
                      .sensitivities()
                      .at(&variable_proxy[0]));
    EXPECT_EQ(-3, expression.sensitivities().at(&variable_proxy[0]));
}

/*****************************************************************************/
TEST_F(TestExpression, operator_minus_equal_arg_expression) {
    auto expression_0 =
        printemps::model::Expression<int, double>::create_instance();
    auto expression_1 =
        printemps::model::Expression<int, double>::create_instance();
    auto expression_sum =
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

    expression_0 = sensitivity_0_0 * variable_0 + sensitivity_1_0 * variable_1 +
                   constant_0;
    expression_1 = sensitivity_0_1 * variable_0 + sensitivity_1_1 * variable_1 +
                   constant_1;

    EXPECT_EQ(sensitivity_0_0 - sensitivity_0_1,
              (expression_0 -= expression_1).sensitivities().at(&variable_0));

    EXPECT_EQ(sensitivity_0_0 - sensitivity_0_1,
              expression_0.sensitivities().at(&variable_0));
    EXPECT_EQ(sensitivity_1_0 - sensitivity_1_1,
              expression_0.sensitivities().at(&variable_1));

    EXPECT_EQ(constant_0 - constant_1, expression_0.constant_value());
}

/*****************************************************************************/
TEST_F(TestExpression, operator_product_equal_arg_t_value) {
    auto expression =
        printemps::model::Expression<int, double>::create_instance();
    auto variable = printemps::model::Variable<int, double>::create_instance();

    auto sensitivity = random_integer();
    auto constant    = random_integer();

    expression = sensitivity * variable + constant;

    auto value_0 = random_integer();
    auto value_1 = random_integer();

    EXPECT_EQ(constant * value_0, (expression *= value_0).constant_value());
    EXPECT_EQ(constant * value_0, expression.constant_value());
    EXPECT_EQ(sensitivity * value_0 * value_1,
              (expression *= value_1).sensitivities().at(&variable));
    EXPECT_EQ(sensitivity * value_0 * value_1,
              expression.sensitivities().at(&variable));
}

/*****************************************************************************/
TEST_F(TestExpression, operator_divide_equal_arg_t_value) {
    auto expression =
        printemps::model::Expression<int, double>::create_instance();
    auto variable = printemps::model::Variable<int, double>::create_instance();

    expression = 100 * variable + 200;

    EXPECT_EQ(100, (expression /= 2).constant_value());
    EXPECT_EQ(100, expression.constant_value());
    EXPECT_EQ(25, (expression /= 2).sensitivities().at(&variable));
    EXPECT_EQ(25, expression.sensitivities().at(&variable));
}

/*****************************************************************************/
}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/