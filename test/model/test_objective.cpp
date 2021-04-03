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
class TestObjective : public ::testing::Test {
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
TEST_F(TestObjective, initialize) {
    auto objective =
        printemps::model::Objective<int, double>::create_instance();

    EXPECT_EQ(0, objective.evaluate());
    EXPECT_EQ(0, objective.evaluate({}));
    EXPECT_TRUE(objective.expression().sensitivities().empty());
    EXPECT_EQ(0, objective.expression().constant_value());
    EXPECT_EQ(0, objective.value());
    EXPECT_TRUE(objective.is_linear());
}

/*****************************************************************************/
TEST_F(TestObjective, create_instance_arg_function) {
    auto expression =
        printemps::model::Expression<int, double>::create_instance();
    auto variable = printemps::model::Variable<int, double>::create_instance();

    auto sensitivity = random_integer();
    auto constant    = random_integer();

    expression = sensitivity * variable + constant;

    auto f = [&expression](
                 const printemps::neighborhood::Move<int, double> &a_MOVE) {
        return expression.evaluate(a_MOVE);
    };

    auto objective =
        printemps::model::Objective<int, double>::create_instance(f);

    EXPECT_TRUE(objective.expression().sensitivities().empty());
    EXPECT_EQ(0, objective.expression().constant_value());
    EXPECT_EQ(0, objective.value());
    EXPECT_FALSE(objective.is_linear());
}

/*****************************************************************************/
TEST_F(TestObjective, create_instance_arg_expression) {
    auto expression =
        printemps::model::Expression<int, double>::create_instance();
    auto variable = printemps::model::Variable<int, double>::create_instance();

    auto sensitivity = random_integer();
    auto constant    = random_integer();

    expression = sensitivity * variable + constant;
    auto objective =
        printemps::model::Objective<int, double>::create_instance(expression);

    auto value = random_integer();
    variable   = value;

    EXPECT_EQ(sensitivity,
              objective.expression().sensitivities().at(&variable));
    EXPECT_EQ(constant, objective.expression().constant_value());
    EXPECT_EQ(0, objective.value());
    EXPECT_TRUE(objective.is_linear());
}

/*****************************************************************************/
TEST_F(TestObjective, setup_arg_function) {
    auto f =
        []([[maybe_unused]] const printemps::neighborhood::Move<int, double>
               &a_MOVE) { return 0; };

    auto objective =
        printemps::model::Objective<int, double>::create_instance();
    objective.setup(f);

    EXPECT_TRUE(objective.expression().sensitivities().empty());
    EXPECT_EQ(0, objective.expression().constant_value());
    EXPECT_EQ(0, objective.value());
    EXPECT_FALSE(objective.is_linear());
}

/*****************************************************************************/
TEST_F(TestObjective, setup_arg_expression) {
    auto expression =
        printemps::model::Expression<int, double>::create_instance();
    auto variable = printemps::model::Variable<int, double>::create_instance();

    auto sensitivity = random_integer();
    auto constant    = random_integer();

    expression = sensitivity * variable + constant;

    auto objective =
        printemps::model::Objective<int, double>::create_instance();
    objective.setup(expression);

    EXPECT_EQ(sensitivity,
              objective.expression().sensitivities().at(&variable));
    EXPECT_EQ(constant, objective.expression().constant_value());
    EXPECT_EQ(0, objective.value());
    EXPECT_TRUE(objective.is_linear());
}

/*****************************************************************************/
TEST_F(TestObjective, evaluate_function_arg_void) {
    auto expression =
        printemps::model::Expression<int, double>::create_instance();
    auto variable = printemps::model::Variable<int, double>::create_instance();

    auto sensitivity = random_integer();
    auto constant    = random_integer();

    expression = sensitivity * variable + constant;
    expression.setup_fixed_sensitivities();

    auto f = [&expression](
                 const printemps::neighborhood::Move<int, double> &a_MOVE) {
        return expression.evaluate(a_MOVE);
    };

    auto objective =
        printemps::model::Objective<int, double>::create_instance(f);

    auto value = random_integer();
    variable   = value;

    auto expected_value = sensitivity * value + constant;
    EXPECT_EQ(expected_value, objective.evaluate());
    objective.update();
    EXPECT_EQ(expected_value, objective.value());
}

/*****************************************************************************/
TEST_F(TestObjective, evaluate_expression_arg_void) {
    auto expression =
        printemps::model::Expression<int, double>::create_instance();
    auto variable = printemps::model::Variable<int, double>::create_instance();

    auto sensitivity = random_integer();
    auto constant    = random_integer();

    expression = sensitivity * variable + constant;

    auto objective =
        printemps::model::Objective<int, double>::create_instance(expression);

    auto value = random_integer();
    variable   = value;

    auto expected_value = sensitivity * value + constant;
    EXPECT_EQ(expected_value, objective.evaluate());
    objective.update();
    EXPECT_EQ(expected_value, objective.value());
}

/*****************************************************************************/
TEST_F(TestObjective, evaluate_function_arg_move) {
    auto expression =
        printemps::model::Expression<int, double>::create_instance();
    auto variable = printemps::model::Variable<int, double>::create_instance();

    auto sensitivity = random_integer();
    auto constant    = random_integer();

    expression = sensitivity * variable + constant;
    expression.setup_fixed_sensitivities();

    auto f = [&expression, &variable](
                 const printemps::neighborhood::Move<int, double> &a_MOVE) {
        return expression.evaluate(a_MOVE);
    };

    auto objective =
        printemps::model::Objective<int, double>::create_instance(f);

    {
        auto value = random_integer();
        variable   = value;

        auto expected_value = sensitivity * value + constant;
        EXPECT_EQ(expected_value, objective.evaluate());
        objective.update();
        EXPECT_EQ(expected_value, objective.value());

        /// expression.update() must be called after objective.update();
        expression.update();
    }
    {
        auto                                       value = random_integer();
        printemps::neighborhood::Move<int, double> move;
        move.alterations.emplace_back(&variable, value);

        auto expected_value = sensitivity * value + constant;
        EXPECT_EQ(expected_value, objective.evaluate(move));
        objective.update(move);
        EXPECT_EQ(expected_value, objective.value());
    }
}

/*****************************************************************************/
TEST_F(TestObjective, evaluate_expression_arg_move) {
    auto expression =
        printemps::model::Expression<int, double>::create_instance();
    auto variable = printemps::model::Variable<int, double>::create_instance();

    auto sensitivity = random_integer();
    auto constant    = random_integer();

    expression = sensitivity * variable + constant;

    auto objective =
        printemps::model::Objective<int, double>::create_instance(expression);

    {
        auto value = random_integer();
        variable   = value;
        objective.update();

        auto expected_value = sensitivity * value + constant;
        EXPECT_EQ(expected_value, objective.evaluate());
        objective.update();
        EXPECT_EQ(expected_value, objective.value());
    }
    {
        auto                                       value = random_integer();
        printemps::neighborhood::Move<int, double> move;
        move.alterations.emplace_back(&variable, value);

        auto expected_value = sensitivity * value + constant;
        EXPECT_EQ(expected_value, objective.evaluate(move));
        objective.update(move);
        EXPECT_EQ(expected_value, objective.value());
    }
}

/*****************************************************************************/
TEST_F(TestObjective, update_arg_void) {
    /// This method is tested in evaluate_function_arg_void() and
    /// tested in evaluate_expression_arg_void().
}

/*****************************************************************************/
TEST_F(TestObjective, update_arg_move) {
    /// This method is tested in evaluate_function_arg_move() and
    /// tested in evaluate_expression_arg_move().
}

/*****************************************************************************/
TEST_F(TestObjective, expression) {
    /// This method is tested in create_instance_arg_expression().
}

/*****************************************************************************/
TEST_F(TestObjective, value) {
    /// This method is tested in following tests:
    /// - evaluate_function_arg_void,
    /// - evaluate_expression_arg_void,
    /// - evaluate_function_arg_move and
    /// - evaluate_expression_arg_move.
}

/*****************************************************************************/
TEST_F(TestObjective, is_linear) {
    /// This method is tested in create_instance_arg_function() and
    /// create_instance_arg_expression().
}

/*****************************************************************************/
}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/