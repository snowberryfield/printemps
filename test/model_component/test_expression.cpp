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
        printemps::model_component::Expression<int, double>::create_instance();

    /// Check the initial values of the base class members.
    EXPECT_EQ(0, expression.proxy_index());
    EXPECT_EQ(0, expression.flat_index());
    EXPECT_EQ(0, expression.multi_dimensional_index()[0]);
    EXPECT_EQ("", expression.name());

    /// Check the initial values of the derived class members.
    EXPECT_EQ(0, expression.constant_value());
    EXPECT_EQ(0, expression.value());

    EXPECT_TRUE(expression.is_enabled());
    EXPECT_TRUE(expression.sensitivities().empty());

    EXPECT_EQ(static_cast<std::uint64_t>(0),
              expression.plus_one_coefficient_mask());
    EXPECT_EQ(static_cast<std::uint64_t>(0),
              expression.minus_one_coefficient_mask());
    EXPECT_FALSE(expression.has_effective_plus_one_coefficient_mask());
    EXPECT_FALSE(expression.has_effective_minus_one_coefficient_mask());
}

/*****************************************************************************/
TEST_F(TestExpression, set_sensitivities) {
    auto expression =
        printemps::model_component::Expression<int, double>::create_instance();

    auto variable_0 =
        printemps::model_component::Variable<int, double>::create_instance();
    auto variable_1 =
        printemps::model_component::Variable<int, double>::create_instance();
    std::unordered_map<printemps::model_component::Variable<int, double>*,
                       double>
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
TEST_F(TestExpression, setup_mask) {
    {
        auto expression =
            printemps::model_component::Expression<int,
                                                   double>::create_instance();
        auto variable_0 =
            printemps::model_component::Variable<int,
                                                 double>::create_instance();
        auto variable_1 =
            printemps::model_component::Variable<int,
                                                 double>::create_instance();
        expression = variable_0 + variable_1;
        expression.setup_mask();

        EXPECT_TRUE(expression.has_effective_plus_one_coefficient_mask());
        EXPECT_FALSE(expression.has_effective_minus_one_coefficient_mask());

        EXPECT_FALSE(reinterpret_cast<std::uint64_t>(&variable_0) &
                     expression.minus_one_coefficient_mask());
        EXPECT_FALSE(reinterpret_cast<std::uint64_t>(&variable_1) &
                     expression.minus_one_coefficient_mask());
    }

    {
        auto expression =
            printemps::model_component::Expression<int,
                                                   double>::create_instance();
        auto variable_0 =
            printemps::model_component::Variable<int,
                                                 double>::create_instance();
        auto variable_1 =
            printemps::model_component::Variable<int,
                                                 double>::create_instance();
        expression = -variable_0 - variable_1;
        expression.setup_mask();

        EXPECT_FALSE(expression.has_effective_plus_one_coefficient_mask());
        EXPECT_TRUE(expression.has_effective_minus_one_coefficient_mask());

        EXPECT_FALSE(reinterpret_cast<std::uint64_t>(&variable_0) &
                     expression.plus_one_coefficient_mask());
        EXPECT_FALSE(reinterpret_cast<std::uint64_t>(&variable_1) &
                     expression.plus_one_coefficient_mask());
    }

    {
        auto expression =
            printemps::model_component::Expression<int,
                                                   double>::create_instance();
        auto variable_0 =
            printemps::model_component::Variable<int,
                                                 double>::create_instance();
        auto variable_1 =
            printemps::model_component::Variable<int,
                                                 double>::create_instance();
        expression = variable_0 - variable_1;
        expression.setup_mask();

        EXPECT_EQ((reinterpret_cast<std::uint64_t>(&variable_0) &
                   expression.plus_one_coefficient_mask()) > 0,
                  expression.has_effective_plus_one_coefficient_mask());

        EXPECT_EQ((reinterpret_cast<std::uint64_t>(&variable_1) &
                   expression.minus_one_coefficient_mask()) > 0,
                  expression.has_effective_minus_one_coefficient_mask());

        EXPECT_FALSE(reinterpret_cast<std::uint64_t>(&variable_1) &
                     expression.plus_one_coefficient_mask());
        EXPECT_FALSE(reinterpret_cast<std::uint64_t>(&variable_0) &
                     expression.minus_one_coefficient_mask());
    }

    {
        auto expression =
            printemps::model_component::Expression<int,
                                                   double>::create_instance();
        auto variable_0 =
            printemps::model_component::Variable<int,
                                                 double>::create_instance();
        auto variable_1 =
            printemps::model_component::Variable<int,
                                                 double>::create_instance();
        expression = 2 * variable_0 - 2 * variable_1;
        expression.setup_mask();

        EXPECT_FALSE(expression.has_effective_plus_one_coefficient_mask());
        EXPECT_FALSE(expression.has_effective_minus_one_coefficient_mask());

        EXPECT_FALSE(reinterpret_cast<std::uint64_t>(&variable_0) &
                     expression.plus_one_coefficient_mask());
        EXPECT_FALSE(reinterpret_cast<std::uint64_t>(&variable_1) &
                     expression.plus_one_coefficient_mask());
        EXPECT_FALSE(reinterpret_cast<std::uint64_t>(&variable_0) &
                     expression.minus_one_coefficient_mask());
        EXPECT_FALSE(reinterpret_cast<std::uint64_t>(&variable_1) &
                     expression.minus_one_coefficient_mask());
    }
}

/*****************************************************************************/
TEST_F(TestExpression, constant_value) {
    auto expression =
        printemps::model_component::Expression<int, double>::create_instance();

    auto value = random_integer();
    expression = value;
    EXPECT_EQ(value, expression.constant_value());
}

/*****************************************************************************/
TEST_F(TestExpression, evaluate_arg_void) {
    auto expression =
        printemps::model_component::Expression<int, double>::create_instance();

    auto variable_0 =
        printemps::model_component::Variable<int, double>::create_instance();
    auto variable_1 =
        printemps::model_component::Variable<int, double>::create_instance();
    std::unordered_map<printemps::model_component::Variable<int, double>*,
                       double>
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
        printemps::model_component::Expression<int, double>::create_instance();

    auto variable_0 =
        printemps::model_component::Variable<int, double>::create_instance();
    auto variable_1 =
        printemps::model_component::Variable<int, double>::create_instance();
    std::unordered_map<printemps::model_component::Variable<int, double>*,
                       double>
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
TEST_F(TestExpression, evaluate_with_mask) {
    auto expression =
        printemps::model_component::Expression<int, double>::create_instance();

    auto variable_0 =
        printemps::model_component::Variable<int, double>::create_instance();
    auto variable_1 =
        printemps::model_component::Variable<int, double>::create_instance();
    auto variable_2 =
        printemps::model_component::Variable<int, double>::create_instance();

    expression = variable_0 - variable_1 + 2 * variable_2 + 3;

    expression.setup_fixed_sensitivities();
    expression.setup_mask();

    variable_0 = 0;
    variable_1 = 0;
    variable_2 = 0;

    expression.update();

    EXPECT_EQ(4, expression.evaluate_with_mask(&variable_0, 1));
    EXPECT_EQ(2, expression.evaluate_with_mask(&variable_1, 1));
    EXPECT_EQ(5, expression.evaluate_with_mask(&variable_2, 1));
}

/*****************************************************************************/
TEST_F(TestExpression, update_arg_void) {
    auto expression =
        printemps::model_component::Expression<int, double>::create_instance();

    auto variable_0 =
        printemps::model_component::Variable<int, double>::create_instance();
    auto variable_1 =
        printemps::model_component::Variable<int, double>::create_instance();
    std::unordered_map<printemps::model_component::Variable<int, double>*,
                       double>
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
        printemps::model_component::Expression<int, double>::create_instance();

    auto variable_0 =
        printemps::model_component::Variable<int, double>::create_instance();
    auto variable_1 =
        printemps::model_component::Variable<int, double>::create_instance();
    std::unordered_map<printemps::model_component::Variable<int, double>*,
                       double>
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
        printemps::model_component::Expression<int, double>::create_instance();

    auto variable_0 =
        printemps::model_component::Variable<int, double>::create_instance();
    auto variable_1 =
        printemps::model_component::Variable<int, double>::create_instance();
    std::unordered_map<printemps::model_component::Variable<int, double>*,
                       double>
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
        printemps::model_component::Expression<int, double>::create_instance();
    expression.disable();
    EXPECT_FALSE(expression.is_enabled());

    expression.enable();
    EXPECT_TRUE(expression.is_enabled());

    expression.disable();
    EXPECT_FALSE(expression.is_enabled());
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
TEST_F(TestExpression, erase) {
    auto expression =
        printemps::model_component::Expression<int, double>::create_instance();

    auto variable_0 =
        printemps::model_component::Variable<int, double>::create_instance();
    auto variable_1 =
        printemps::model_component::Variable<int, double>::create_instance();

    expression = variable_0 + 2 * variable_1 + 4;
    expression.erase(&variable_0);

    EXPECT_EQ(2, expression.sensitivities().at(&variable_1));
    EXPECT_TRUE(expression.sensitivities().find(&variable_0) ==
                expression.sensitivities().end());
}

/*****************************************************************************/
TEST_F(TestExpression, substitute) {
    auto expression_0 =
        printemps::model_component::Expression<int, double>::create_instance();
    auto expression_1 =
        printemps::model_component::Expression<int, double>::create_instance();

    auto variable_0 =
        printemps::model_component::Variable<int, double>::create_instance();
    auto variable_1 =
        printemps::model_component::Variable<int, double>::create_instance();

    expression_0 = variable_0 + 2 * variable_1 + 4;
    expression_1 = 3 * variable_0 + 7;
    expression_0.substitute(&variable_1, expression_1);

    EXPECT_EQ(7, expression_0.sensitivities().at(&variable_0));
    EXPECT_EQ(18, expression_0.constant_value());
    EXPECT_TRUE(expression_0.sensitivities().find(&variable_1) ==
                expression_0.sensitivities().end());
}

/*****************************************************************************/
TEST_F(TestExpression, lower_bound) {
    auto expression =
        printemps::model_component::Expression<int, double>::create_instance();

    auto variable_0 =
        printemps::model_component::Variable<int, double>::create_instance();
    auto variable_1 =
        printemps::model_component::Variable<int, double>::create_instance();
    auto variable_2 =
        printemps::model_component::Variable<int, double>::create_instance();

    variable_0.set_bound(-10, 20);
    variable_1.set_bound(-100, 200);
    variable_2.fix_by(30);
    expression = variable_0 - 2 * variable_1 + variable_2 + 4;

    EXPECT_EQ(-376, expression.lower_bound());
    EXPECT_EQ(254, expression.upper_bound());
    EXPECT_EQ(30, expression.fixed_term_value());
}

/*****************************************************************************/
TEST_F(TestExpression, upper_bound) {
    /// This method is tested in lower_bound().
}

/*****************************************************************************/
TEST_F(TestExpression, fixed_term_value) {
    /// This method is tested in lower_bound().
}

/*****************************************************************************/
TEST_F(TestExpression, mutable_variable_sensitivities) {
    auto expression =
        printemps::model_component::Expression<int, double>::create_instance();

    auto variable_0 =
        printemps::model_component::Variable<int, double>::create_instance();
    auto variable_1 =
        printemps::model_component::Variable<int, double>::create_instance();
    auto variable_2 =
        printemps::model_component::Variable<int, double>::create_instance();
    auto variable_3 =
        printemps::model_component::Variable<int, double>::create_instance();
    auto variable_4 =
        printemps::model_component::Variable<int, double>::create_instance();
    auto variable_5 =
        printemps::model_component::Variable<int, double>::create_instance();

    expression = variable_0 - variable_1 - variable_2 + variable_3 +
                 variable_4 + variable_5;
    variable_3.fix_by(1);
    variable_4.fix_by(1);
    variable_5.fix_by(1);

    auto mutable_variable_sensitivities =
        expression.mutable_variable_sensitivities();

    auto positive_mutable_variable_sensitivities =
        expression.positive_mutable_variable_sensitivities();

    auto negative_mutable_variable_sensitivities =
        expression.negative_mutable_variable_sensitivities();

    EXPECT_EQ(3, static_cast<int>(mutable_variable_sensitivities.size()));
    EXPECT_EQ(1,
              static_cast<int>(positive_mutable_variable_sensitivities.size()));
    EXPECT_TRUE(positive_mutable_variable_sensitivities.find(&variable_0) !=
                positive_mutable_variable_sensitivities.end());

    EXPECT_EQ(2,
              static_cast<int>(negative_mutable_variable_sensitivities.size()));
    EXPECT_TRUE(negative_mutable_variable_sensitivities.find(&variable_1) !=
                negative_mutable_variable_sensitivities.end());
    EXPECT_TRUE(negative_mutable_variable_sensitivities.find(&variable_2) !=
                negative_mutable_variable_sensitivities.end());
}

/*****************************************************************************/
TEST_F(TestExpression, positive_mutable_variable_sensitivities) {
    /// This method is tested in mutable_variable_sensitivities().
}

/*****************************************************************************/
TEST_F(TestExpression, negative_mutable_variable_sensitivities) {
    /// This method is tested in mutable_variable_sensitivities().
}

/*****************************************************************************/
TEST_F(TestExpression, plus_one_coefficient_mask) {
    /// This method is tested in setup_mask().
}

/*****************************************************************************/
TEST_F(TestExpression, minus_one_coefficient_mask) {
    /// This method is tested in setup_mask().
}

/*****************************************************************************/
TEST_F(TestExpression, has_effective_plus_one_coefficient_mask) {
    /// This method is tested in setup_mask().
}

/*****************************************************************************/
TEST_F(TestExpression, has_effective_minus_one_coefficient_mask) {
    /// This method is tested in setup_mask().
}

/*****************************************************************************/
TEST_F(TestExpression, operator_plus) {
    auto expression =
        printemps::model_component::Expression<int, double>::create_instance();

    auto variable_0 =
        printemps::model_component::Variable<int, double>::create_instance();
    auto variable_1 =
        printemps::model_component::Variable<int, double>::create_instance();

    auto sensitivity_0 = random_integer();
    auto sensitivity_1 = random_integer();
    auto constant      = random_integer();

    expression =
        sensitivity_0 * variable_0 + sensitivity_1 * variable_1 + constant;

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
        printemps::model_component::Expression<int, double>::create_instance();

    auto variable_0 =
        printemps::model_component::Variable<int, double>::create_instance();
    auto variable_1 =
        printemps::model_component::Variable<int, double>::create_instance();

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
        printemps::model_component::Expression<int, double>::create_instance();

    auto value = random_integer();

    EXPECT_EQ(value, (expression = value).constant_value());
    EXPECT_EQ(value, expression.constant_value());
}

/*****************************************************************************/
TEST_F(TestExpression, operator_equal_arg_t_expression_like) {
    auto expression =
        printemps::model_component::Expression<int, double>::create_instance();

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
        printemps::model_component::Expression<int, double>::create_instance();

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
        printemps::model_component::Expression<int, double>::create_instance();

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
        printemps::model_component::Expression<int, double>::create_instance();
    auto expression_1 =
        printemps::model_component::Expression<int, double>::create_instance();

    auto variable_0 =
        printemps::model_component::Variable<int, double>::create_instance();
    auto variable_1 =
        printemps::model_component::Variable<int, double>::create_instance();

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
        printemps::model_component::Expression<int, double>::create_instance();

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
        printemps::model_component::Expression<int, double>::create_instance();

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
        printemps::model_component::Expression<int, double>::create_instance();
    auto expression_1 =
        printemps::model_component::Expression<int, double>::create_instance();
    auto expression_sum =
        printemps::model_component::Expression<int, double>::create_instance();

    auto variable_0 =
        printemps::model_component::Variable<int, double>::create_instance();
    auto variable_1 =
        printemps::model_component::Variable<int, double>::create_instance();

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
        printemps::model_component::Expression<int, double>::create_instance();
    auto variable =
        printemps::model_component::Variable<int, double>::create_instance();

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
        printemps::model_component::Expression<int, double>::create_instance();
    auto variable =
        printemps::model_component::Variable<int, double>::create_instance();

    expression = 100 * variable + 200;

    EXPECT_EQ(100, (expression /= 2).constant_value());
    EXPECT_EQ(100, expression.constant_value());
    EXPECT_EQ(25, (expression /= 2).sensitivities().at(&variable));
    EXPECT_EQ(25, expression.sensitivities().at(&variable));
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/