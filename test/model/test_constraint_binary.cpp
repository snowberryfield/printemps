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
class TestConstraintBinary : public ::testing::Test {
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
TEST_F(TestConstraintBinary, function_lower) {
    printemps::model::Model<int, double> model;

    auto variable = printemps::model::Variable<int, double>::create_instance();
    auto expression =
        printemps::model::Expression<int, double>::create_instance();
    auto target = random_integer();

    expression = variable;
    std::function<double(const printemps::model::Move<int, double>&)> f =
        [&expression](const printemps::model::Move<int, double>& a_MOVE) {
            return expression.evaluate(a_MOVE);
        };

    /// Function <= Integer
    {
        auto constraint_result = f <= target;

        EXPECT_EQ(false, constraint_result.is_linear());
        EXPECT_EQ(printemps::model::ConstraintSense::Lower,
                  constraint_result.sense());

        auto value = random_integer();
        variable   = value;
        constraint_result.update();
        EXPECT_EQ(value - target, constraint_result.constraint_value());
        EXPECT_EQ(std::max(value - target, 0),
                  constraint_result.violation_value());
    }

    ///  Integer <= Function
    {
        auto constraint_result = target <= f;

        EXPECT_EQ(false, constraint_result.is_linear());
        EXPECT_EQ(printemps::model::ConstraintSense::Lower,
                  constraint_result.sense());

        auto value = random_integer();
        variable   = value;
        constraint_result.update();
        EXPECT_EQ(target - value, constraint_result.constraint_value());
        EXPECT_EQ(std::max(target - value, 0),
                  constraint_result.violation_value());
    }
}

/*****************************************************************************/
TEST_F(TestConstraintBinary, function_equal) {
    printemps::model::Model<int, double> model;

    auto variable = printemps::model::Variable<int, double>::create_instance();
    auto expression =
        printemps::model::Expression<int, double>::create_instance();
    auto target = random_integer();

    expression = variable;
    std::function<double(const printemps::model::Move<int, double>&)> f =
        [&expression](const printemps::model::Move<int, double>& a_MOVE) {
            return expression.evaluate(a_MOVE);
        };

    /// Function == Integer
    {
        auto constraint_result = f == target;

        EXPECT_EQ(false, constraint_result.is_linear());
        EXPECT_EQ(printemps::model::ConstraintSense::Equal,
                  constraint_result.sense());

        auto value = random_integer();
        variable   = value;
        constraint_result.update();
        EXPECT_EQ(value - target, constraint_result.constraint_value());
        EXPECT_EQ(abs(value - target), constraint_result.violation_value());
    }

    ///  Integer == Function
    {
        auto constraint_result = target == f;

        EXPECT_EQ(false, constraint_result.is_linear());
        EXPECT_EQ(printemps::model::ConstraintSense::Equal,
                  constraint_result.sense());

        auto value = random_integer();
        variable   = value;
        constraint_result.update();
        EXPECT_EQ(target - value, constraint_result.constraint_value());
        EXPECT_EQ(abs(target - value), constraint_result.violation_value());
    }
}

/*****************************************************************************/
TEST_F(TestConstraintBinary, function_upper) {
    printemps::model::Model<int, double> model;

    auto variable = printemps::model::Variable<int, double>::create_instance();
    auto expression =
        printemps::model::Expression<int, double>::create_instance();
    auto target = random_integer();

    expression = variable;
    std::function<double(const printemps::model::Move<int, double>&)> f =
        [&expression](const printemps::model::Move<int, double>& a_MOVE) {
            return expression.evaluate(a_MOVE);
        };

    /// Function >= Integer
    {
        auto constraint_result = f >= target;

        EXPECT_EQ(false, constraint_result.is_linear());
        EXPECT_EQ(printemps::model::ConstraintSense::Upper,
                  constraint_result.sense());

        auto value = random_integer();
        variable   = value;
        constraint_result.update();
        EXPECT_EQ(value - target, constraint_result.constraint_value());
        EXPECT_EQ(std::max(target - value, 0),
                  constraint_result.violation_value());
    }

    ///  Integer >= Function
    {
        auto constraint_result = target >= f;

        EXPECT_EQ(false, constraint_result.is_linear());
        EXPECT_EQ(printemps::model::ConstraintSense::Upper,
                  constraint_result.sense());

        auto value = random_integer();
        variable   = value;
        constraint_result.update();
        EXPECT_EQ(target - value, constraint_result.constraint_value());
        EXPECT_EQ(std::max(value - target, 0),
                  constraint_result.violation_value());
    }
}

/*****************************************************************************/
TEST_F(TestConstraintBinary, variable_lower) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy   = model.create_variable("x");
    auto& expression_proxy = model.create_expression("e");
    auto  variable = printemps::model::Variable<int, double>::create_instance();
    auto  expression =
        printemps::model::Expression<int, double>::create_instance();
    auto constant = random_integer();

    expression_proxy = variable_proxy;
    expression       = variable;

    /// Variable <= Variable
    {
        auto constraint_result = variable <= variable;

        EXPECT_EQ(0,
                  constraint_result.expression().sensitivities().at(&variable));
        EXPECT_EQ(0, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Lower,
                  constraint_result.sense());
    }

    /// Variable <= Integer
    {
        auto constraint_result = variable <= constant;

        EXPECT_EQ(1,
                  constraint_result.expression().sensitivities().at(&variable));
        EXPECT_EQ(-constant, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Lower,
                  constraint_result.sense());
    }

    ///  Integer <= Variable
    {
        auto constraint_result = constant <= variable;

        EXPECT_EQ(-1,
                  constraint_result.expression().sensitivities().at(&variable));
        EXPECT_EQ(constant, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Lower,
                  constraint_result.sense());
    }

    /// Variable <= VariableProxy
    {
        auto constraint_result = variable <= variable_proxy;

        EXPECT_EQ(1,
                  constraint_result.expression().sensitivities().at(&variable));
        EXPECT_EQ(-1, constraint_result.expression().sensitivities().at(
                          &variable_proxy[0]));
        EXPECT_EQ(0, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Lower,
                  constraint_result.sense());
    }

    /// VariableProxy <= Variable
    {
        auto constraint_result = variable_proxy <= variable;

        EXPECT_EQ(1, constraint_result.expression().sensitivities().at(
                         &variable_proxy[0]));
        EXPECT_EQ(-1,
                  constraint_result.expression().sensitivities().at(&variable));
        EXPECT_EQ(0, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Lower,
                  constraint_result.sense());
    }

    /// Variable <= Expression
    {
        auto constraint_result = variable <= expression;

        EXPECT_EQ(0,
                  constraint_result.expression().sensitivities().at(&variable));
        EXPECT_EQ(0, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Lower,
                  constraint_result.sense());
    }

    /// Expression <= Variable
    {
        auto constraint_result = expression <= variable;

        EXPECT_EQ(0,
                  constraint_result.expression().sensitivities().at(&variable));
        EXPECT_EQ(0, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Lower,
                  constraint_result.sense());
    }

    /// Variable <= ExpressionProxy
    {
        auto constraint_result = variable <= expression_proxy;

        EXPECT_EQ(1,
                  constraint_result.expression().sensitivities().at(&variable));
        EXPECT_EQ(-1, constraint_result.expression().sensitivities().at(
                          &variable_proxy[0]));
        EXPECT_EQ(0, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Lower,
                  constraint_result.sense());
    }

    /// ExpressionProxy <= Variable
    {
        auto constraint_result = expression_proxy <= variable;

        EXPECT_EQ(1, constraint_result.expression().sensitivities().at(
                         &variable_proxy[0]));
        EXPECT_EQ(-1,
                  constraint_result.expression().sensitivities().at(&variable));
        EXPECT_EQ(0, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Lower,
                  constraint_result.sense());
    }
}

/*****************************************************************************/
TEST_F(TestConstraintBinary, variable_equal) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy   = model.create_variable("x");
    auto& expression_proxy = model.create_expression("e");
    auto  variable = printemps::model::Variable<int, double>::create_instance();
    auto  expression =
        printemps::model::Expression<int, double>::create_instance();
    auto constant = random_integer();

    expression_proxy = variable_proxy;
    expression       = variable;

    /// Variable <= Variable
    {
        auto constraint_result = variable == variable;

        EXPECT_EQ(0,
                  constraint_result.expression().sensitivities().at(&variable));
        EXPECT_EQ(0, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Equal,
                  constraint_result.sense());
    }

    /// Variable == Integer
    {
        auto constraint_result = variable == constant;

        EXPECT_EQ(1,
                  constraint_result.expression().sensitivities().at(&variable));
        EXPECT_EQ(-constant, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Equal,
                  constraint_result.sense());
    }

    ///  Integer == Variable
    {
        auto constraint_result = constant == variable;

        EXPECT_EQ(-1,
                  constraint_result.expression().sensitivities().at(&variable));
        EXPECT_EQ(constant, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Equal,
                  constraint_result.sense());
    }

    /// Variable == VariableProxy
    {
        auto constraint_result = variable == variable_proxy;

        EXPECT_EQ(1,
                  constraint_result.expression().sensitivities().at(&variable));
        EXPECT_EQ(-1, constraint_result.expression().sensitivities().at(
                          &variable_proxy[0]));
        EXPECT_EQ(0, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Equal,
                  constraint_result.sense());
    }

    /// VariableProxy == Variable
    {
        auto constraint_result = variable_proxy == variable;

        EXPECT_EQ(1, constraint_result.expression().sensitivities().at(
                         &variable_proxy[0]));
        EXPECT_EQ(-1,
                  constraint_result.expression().sensitivities().at(&variable));
        EXPECT_EQ(0, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Equal,
                  constraint_result.sense());
    }

    /// Variable == Expression
    {
        auto constraint_result = variable == expression;

        EXPECT_EQ(0,
                  constraint_result.expression().sensitivities().at(&variable));
        EXPECT_EQ(0, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Equal,
                  constraint_result.sense());
    }

    /// Expression == Variable
    {
        auto constraint_result = expression == variable;

        EXPECT_EQ(0,
                  constraint_result.expression().sensitivities().at(&variable));
        EXPECT_EQ(0, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Equal,
                  constraint_result.sense());
    }

    /// Variable == ExpressionProxy
    {
        auto constraint_result = variable == expression_proxy;

        EXPECT_EQ(1,
                  constraint_result.expression().sensitivities().at(&variable));
        EXPECT_EQ(-1, constraint_result.expression().sensitivities().at(
                          &variable_proxy[0]));
        EXPECT_EQ(0, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Equal,
                  constraint_result.sense());
    }

    /// ExpressionProxy == Variable
    {
        auto constraint_result = expression_proxy == variable;

        EXPECT_EQ(1, constraint_result.expression().sensitivities().at(
                         &variable_proxy[0]));
        EXPECT_EQ(-1,
                  constraint_result.expression().sensitivities().at(&variable));
        EXPECT_EQ(0, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Equal,
                  constraint_result.sense());
    }
}

/*****************************************************************************/
TEST_F(TestConstraintBinary, variable_upper) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy   = model.create_variable("x");
    auto& expression_proxy = model.create_expression("e");
    auto  variable = printemps::model::Variable<int, double>::create_instance();
    auto  expression =
        printemps::model::Expression<int, double>::create_instance();
    auto constant = random_integer();

    expression_proxy = variable_proxy;
    expression       = variable;

    /// Variable >= Variable
    {
        auto constraint_result = variable >= variable;

        EXPECT_EQ(0,
                  constraint_result.expression().sensitivities().at(&variable));
        EXPECT_EQ(0, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Upper,
                  constraint_result.sense());
    }

    /// Variable >= Integer
    {
        auto constraint_result = variable >= constant;

        EXPECT_EQ(1,
                  constraint_result.expression().sensitivities().at(&variable));
        EXPECT_EQ(-constant, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Upper,
                  constraint_result.sense());
    }

    ///  Integer >= Variable
    {
        auto constraint_result = constant >= variable;

        EXPECT_EQ(-1,
                  constraint_result.expression().sensitivities().at(&variable));
        EXPECT_EQ(constant, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Upper,
                  constraint_result.sense());
    }

    /// Variable >= VariableProxy
    {
        auto constraint_result = variable >= variable_proxy;

        EXPECT_EQ(1,
                  constraint_result.expression().sensitivities().at(&variable));
        EXPECT_EQ(-1, constraint_result.expression().sensitivities().at(
                          &variable_proxy[0]));
        EXPECT_EQ(0, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Upper,
                  constraint_result.sense());
    }

    /// VariableProxy >= Variable
    {
        auto constraint_result = variable_proxy >= variable;

        EXPECT_EQ(1, constraint_result.expression().sensitivities().at(
                         &variable_proxy[0]));
        EXPECT_EQ(-1,
                  constraint_result.expression().sensitivities().at(&variable));
        EXPECT_EQ(0, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Upper,
                  constraint_result.sense());
    }

    /// Variable >= Expression
    {
        auto constraint_result = variable >= expression;

        EXPECT_EQ(0,
                  constraint_result.expression().sensitivities().at(&variable));
        EXPECT_EQ(0, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Upper,
                  constraint_result.sense());
    }

    /// Expression >= Variable
    {
        auto constraint_result = expression >= variable;

        EXPECT_EQ(0,
                  constraint_result.expression().sensitivities().at(&variable));
        EXPECT_EQ(0, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Upper,
                  constraint_result.sense());
    }

    /// Variable >= ExpressionProxy
    {
        auto constraint_result = variable >= expression_proxy;

        EXPECT_EQ(1,
                  constraint_result.expression().sensitivities().at(&variable));
        EXPECT_EQ(-1, constraint_result.expression().sensitivities().at(
                          &variable_proxy[0]));
        EXPECT_EQ(0, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Upper,
                  constraint_result.sense());
    }

    /// ExpressionProxy >= Variable
    {
        auto constraint_result = expression_proxy >= variable;

        EXPECT_EQ(1, constraint_result.expression().sensitivities().at(
                         &variable_proxy[0]));
        EXPECT_EQ(-1,
                  constraint_result.expression().sensitivities().at(&variable));
        EXPECT_EQ(0, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Upper,
                  constraint_result.sense());
    }
}

/*****************************************************************************/
TEST_F(TestConstraintBinary, variable_proxy_lower) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy   = model.create_variable("x");
    auto& expression_proxy = model.create_expression("e");
    auto  variable = printemps::model::Variable<int, double>::create_instance();
    auto  expression =
        printemps::model::Expression<int, double>::create_instance();
    auto constant = random_integer();

    expression_proxy = variable_proxy;
    expression       = variable;

    /// VariableProxy <= VariableProxy
    {
        auto constraint_result = variable_proxy <= variable_proxy;

        EXPECT_EQ(0, constraint_result.expression().sensitivities().at(
                         &variable_proxy[0]));
        EXPECT_EQ(0, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Lower,
                  constraint_result.sense());
    }

    /// VariableProxy <= Integer
    {
        auto constraint_result = variable_proxy <= constant;

        EXPECT_EQ(1, constraint_result.expression().sensitivities().at(
                         &variable_proxy[0]));
        EXPECT_EQ(-constant, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Lower,
                  constraint_result.sense());
    }

    ///  Integer <= VariableProxy
    {
        auto constraint_result = constant <= variable_proxy;

        EXPECT_EQ(-1, constraint_result.expression().sensitivities().at(
                          &variable_proxy[0]));
        EXPECT_EQ(constant, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Lower,
                  constraint_result.sense());
    }

    /// VariableProxy <= Variable
    {
        auto constraint_result = variable_proxy <= variable;

        EXPECT_EQ(1, constraint_result.expression().sensitivities().at(
                         &variable_proxy[0]));
        EXPECT_EQ(-1,
                  constraint_result.expression().sensitivities().at(&variable));
        EXPECT_EQ(0, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Lower,
                  constraint_result.sense());
    }

    /// Variable <= VariableProxy
    {
        auto constraint_result = variable <= variable_proxy;

        EXPECT_EQ(1,
                  constraint_result.expression().sensitivities().at(&variable));
        EXPECT_EQ(-1, constraint_result.expression().sensitivities().at(
                          &variable_proxy[0]));
        EXPECT_EQ(0, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Lower,
                  constraint_result.sense());
    }

    /// VariableProxy <= Expression
    {
        auto constraint_result = variable_proxy <= expression;

        EXPECT_EQ(1, constraint_result.expression().sensitivities().at(
                         &variable_proxy[0]));
        EXPECT_EQ(-1,
                  constraint_result.expression().sensitivities().at(&variable));
        EXPECT_EQ(0, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Lower,
                  constraint_result.sense());
    }

    /// Expression <= VariableProxy
    {
        auto constraint_result = expression <= variable_proxy;

        EXPECT_EQ(1,
                  constraint_result.expression().sensitivities().at(&variable));
        EXPECT_EQ(-1, constraint_result.expression().sensitivities().at(
                          &variable_proxy[0]));
        EXPECT_EQ(0, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Lower,
                  constraint_result.sense());
    }

    /// VariableProxy <= ExpressionProxy
    {
        auto constraint_result = variable_proxy <= expression_proxy;

        EXPECT_EQ(0, constraint_result.expression().sensitivities().at(
                         &variable_proxy[0]));
        EXPECT_EQ(0, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Lower,
                  constraint_result.sense());
    }

    /// ExpressionProxy <= VariableProxy
    {
        auto constraint_result = expression_proxy <= variable_proxy;

        EXPECT_EQ(0, constraint_result.expression().sensitivities().at(
                         &variable_proxy[0]));
        EXPECT_EQ(0, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Lower,
                  constraint_result.sense());
    }
}

/*****************************************************************************/
TEST_F(TestConstraintBinary, variable_proxy_equal) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy   = model.create_variable("x");
    auto& expression_proxy = model.create_expression("e");
    auto  variable = printemps::model::Variable<int, double>::create_instance();
    auto  expression =
        printemps::model::Expression<int, double>::create_instance();
    auto constant = random_integer();

    expression_proxy = variable_proxy;
    expression       = variable;

    /// VariableProxy == VariableProxy
    {
        auto constraint_result = variable_proxy == variable_proxy;

        EXPECT_EQ(0, constraint_result.expression().sensitivities().at(
                         &variable_proxy[0]));
        EXPECT_EQ(0, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Equal,
                  constraint_result.sense());
    }

    /// VariableProxy == Integer
    {
        auto constraint_result = variable_proxy == constant;

        EXPECT_EQ(1, constraint_result.expression().sensitivities().at(
                         &variable_proxy[0]));
        EXPECT_EQ(-constant, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Equal,
                  constraint_result.sense());
    }

    ///  Integer == VariableProxy
    {
        auto constraint_result = constant == variable_proxy;

        EXPECT_EQ(-1, constraint_result.expression().sensitivities().at(
                          &variable_proxy[0]));
        EXPECT_EQ(constant, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Equal,
                  constraint_result.sense());
    }

    /// VariableProxy <= Variable
    {
        auto constraint_result = variable_proxy == variable;

        EXPECT_EQ(1, constraint_result.expression().sensitivities().at(
                         &variable_proxy[0]));
        EXPECT_EQ(-1,
                  constraint_result.expression().sensitivities().at(&variable));
        EXPECT_EQ(0, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Equal,
                  constraint_result.sense());
    }

    /// Variable <= VariableProxy
    {
        auto constraint_result = variable == variable_proxy;

        EXPECT_EQ(1,
                  constraint_result.expression().sensitivities().at(&variable));
        EXPECT_EQ(-1, constraint_result.expression().sensitivities().at(
                          &variable_proxy[0]));
        EXPECT_EQ(0, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Equal,
                  constraint_result.sense());
    }

    /// VariableProxy == Expression
    {
        auto constraint_result = variable_proxy == expression;

        EXPECT_EQ(1, constraint_result.expression().sensitivities().at(
                         &variable_proxy[0]));
        EXPECT_EQ(-1,
                  constraint_result.expression().sensitivities().at(&variable));
        EXPECT_EQ(0, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Equal,
                  constraint_result.sense());
    }

    /// Expression == VariableProxy
    {
        auto constraint_result = expression == variable_proxy;

        EXPECT_EQ(1,
                  constraint_result.expression().sensitivities().at(&variable));
        EXPECT_EQ(-1, constraint_result.expression().sensitivities().at(
                          &variable_proxy[0]));
        EXPECT_EQ(0, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Equal,
                  constraint_result.sense());
    }

    /// VariableProxy == ExpressionProxy
    {
        auto constraint_result = variable_proxy == expression_proxy;

        EXPECT_EQ(0, constraint_result.expression().sensitivities().at(
                         &variable_proxy[0]));
        EXPECT_EQ(0, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Equal,
                  constraint_result.sense());
    }

    /// ExpressionProxy == VariableProxy
    {
        auto constraint_result = expression_proxy == variable_proxy;

        EXPECT_EQ(0, constraint_result.expression().sensitivities().at(
                         &variable_proxy[0]));
        EXPECT_EQ(0, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Equal,
                  constraint_result.sense());
    }
}

/*****************************************************************************/
TEST_F(TestConstraintBinary, variable_proxy_upper) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy   = model.create_variable("x");
    auto& expression_proxy = model.create_expression("e");
    auto  variable = printemps::model::Variable<int, double>::create_instance();
    auto  expression =
        printemps::model::Expression<int, double>::create_instance();
    auto constant = random_integer();

    expression_proxy = variable_proxy;
    expression       = variable;

    /// VariableProxy >= VariableProxy
    {
        auto constraint_result = variable_proxy >= variable_proxy;

        EXPECT_EQ(0, constraint_result.expression().sensitivities().at(
                         &variable_proxy[0]));
        EXPECT_EQ(0, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Upper,
                  constraint_result.sense());
    }

    /// VariableProxy >= Integer
    {
        auto constraint_result = variable_proxy >= constant;

        EXPECT_EQ(1, constraint_result.expression().sensitivities().at(
                         &variable_proxy[0]));
        EXPECT_EQ(-constant, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Upper,
                  constraint_result.sense());
    }

    ///  Integer >= VariableProxy
    {
        auto constraint_result = constant >= variable_proxy;

        EXPECT_EQ(-1, constraint_result.expression().sensitivities().at(
                          &variable_proxy[0]));
        EXPECT_EQ(constant, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Upper,
                  constraint_result.sense());
    }

    /// VariableProxy <= Variable
    {
        auto constraint_result = variable_proxy >= variable;

        EXPECT_EQ(1, constraint_result.expression().sensitivities().at(
                         &variable_proxy[0]));
        EXPECT_EQ(-1,
                  constraint_result.expression().sensitivities().at(&variable));
        EXPECT_EQ(0, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Upper,
                  constraint_result.sense());
    }

    /// Variable <= VariableProxy
    {
        auto constraint_result = variable >= variable_proxy;

        EXPECT_EQ(1,
                  constraint_result.expression().sensitivities().at(&variable));
        EXPECT_EQ(-1, constraint_result.expression().sensitivities().at(
                          &variable_proxy[0]));
        EXPECT_EQ(0, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Upper,
                  constraint_result.sense());
    }

    /// VariableProxy >= Expression
    {
        auto constraint_result = variable_proxy >= expression;

        EXPECT_EQ(1, constraint_result.expression().sensitivities().at(
                         &variable_proxy[0]));
        EXPECT_EQ(-1,
                  constraint_result.expression().sensitivities().at(&variable));
        EXPECT_EQ(0, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Upper,
                  constraint_result.sense());
    }

    /// Expression >= VariableProxy
    {
        auto constraint_result = expression >= variable_proxy;

        EXPECT_EQ(1,
                  constraint_result.expression().sensitivities().at(&variable));
        EXPECT_EQ(-1, constraint_result.expression().sensitivities().at(
                          &variable_proxy[0]));
        EXPECT_EQ(0, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Upper,
                  constraint_result.sense());
    }

    /// VariableProxy >= ExpressionProxy
    {
        auto constraint_result = variable_proxy >= expression_proxy;

        EXPECT_EQ(0, constraint_result.expression().sensitivities().at(
                         &variable_proxy[0]));
        EXPECT_EQ(0, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Upper,
                  constraint_result.sense());
    }

    /// ExpressionProxy >= VariableProxy
    {
        auto constraint_result = expression_proxy >= variable_proxy;

        EXPECT_EQ(0, constraint_result.expression().sensitivities().at(
                         &variable_proxy[0]));
        EXPECT_EQ(0, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Upper,
                  constraint_result.sense());
    }
}

/*****************************************************************************/
TEST_F(TestConstraintBinary, expression_lower) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy   = model.create_variable("x");
    auto& expression_proxy = model.create_expression("e");
    auto  variable = printemps::model::Variable<int, double>::create_instance();
    auto  expression =
        printemps::model::Expression<int, double>::create_instance();
    auto constant = random_integer();

    expression_proxy = variable_proxy;
    expression       = variable;

    /// Expression <= Expression
    {
        auto constraint_result = expression <= expression;

        EXPECT_EQ(0,
                  constraint_result.expression().sensitivities().at(&variable));
        EXPECT_EQ(0, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Lower,
                  constraint_result.sense());
    }

    /// Expression <= Integer
    {
        auto constraint_result = expression <= constant;

        EXPECT_EQ(1,
                  constraint_result.expression().sensitivities().at(&variable));
        EXPECT_EQ(-constant, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Lower,
                  constraint_result.sense());
    }

    ///  Integer <= Expression
    {
        auto constraint_result = constant <= expression;

        EXPECT_EQ(-1,
                  constraint_result.expression().sensitivities().at(&variable));
        EXPECT_EQ(constant, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Lower,
                  constraint_result.sense());
    }

    /// Expression <= Variable
    {
        auto constraint_result = expression <= variable;

        EXPECT_EQ(0,
                  constraint_result.expression().sensitivities().at(&variable));
        EXPECT_EQ(0, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Lower,
                  constraint_result.sense());
    }

    /// Variable <= Expression
    {
        auto constraint_result = variable <= expression;

        EXPECT_EQ(0,
                  constraint_result.expression().sensitivities().at(&variable));
        EXPECT_EQ(0, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Lower,
                  constraint_result.sense());
    }

    /// Expression <= VariableProxy
    {
        auto constraint_result = expression <= variable_proxy;

        EXPECT_EQ(1,
                  constraint_result.expression().sensitivities().at(&variable));
        EXPECT_EQ(-1, constraint_result.expression().sensitivities().at(
                          &variable_proxy[0]));
        EXPECT_EQ(0, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Lower,
                  constraint_result.sense());
    }

    /// VariableProxy <= Expression
    {
        auto constraint_result = variable_proxy <= expression;

        EXPECT_EQ(1, constraint_result.expression().sensitivities().at(
                         &variable_proxy[0]));
        EXPECT_EQ(-1,
                  constraint_result.expression().sensitivities().at(&variable));
        EXPECT_EQ(0, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Lower,
                  constraint_result.sense());
    }

    /// Expression <= ExpressionProxy
    {
        auto constraint_result = expression <= expression_proxy;

        EXPECT_EQ(1,
                  constraint_result.expression().sensitivities().at(&variable));
        EXPECT_EQ(-1, constraint_result.expression().sensitivities().at(
                          &variable_proxy[0]));
        EXPECT_EQ(0, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Lower,
                  constraint_result.sense());
    }

    /// ExpressionProxy <= Expression
    {
        auto constraint_result = expression_proxy <= expression;

        EXPECT_EQ(1, constraint_result.expression().sensitivities().at(
                         &variable_proxy[0]));
        EXPECT_EQ(-1,
                  constraint_result.expression().sensitivities().at(&variable));
        EXPECT_EQ(0, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Lower,
                  constraint_result.sense());
    }
}

/*****************************************************************************/
TEST_F(TestConstraintBinary, expression_equal) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy   = model.create_variable("x");
    auto& expression_proxy = model.create_expression("e");
    auto  variable = printemps::model::Variable<int, double>::create_instance();
    auto  expression =
        printemps::model::Expression<int, double>::create_instance();
    auto constant = random_integer();

    expression_proxy = variable_proxy;
    expression       = variable;

    /// Expression == Expression
    {
        auto constraint_result = expression == expression;

        EXPECT_EQ(0,
                  constraint_result.expression().sensitivities().at(&variable));
        EXPECT_EQ(0, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Equal,
                  constraint_result.sense());
    }

    /// Expression == Integer
    {
        auto constraint_result = expression == constant;

        EXPECT_EQ(1,
                  constraint_result.expression().sensitivities().at(&variable));
        EXPECT_EQ(-constant, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Equal,
                  constraint_result.sense());
    }

    ///  Integer == Expression
    {
        auto constraint_result = constant == expression;

        EXPECT_EQ(-1,
                  constraint_result.expression().sensitivities().at(&variable));
        EXPECT_EQ(constant, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Equal,
                  constraint_result.sense());
    }

    /// Expression == Variable
    {
        auto constraint_result = expression == variable;

        EXPECT_EQ(0,
                  constraint_result.expression().sensitivities().at(&variable));
        EXPECT_EQ(0, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Equal,
                  constraint_result.sense());
    }

    /// Variable == Expression
    {
        auto constraint_result = variable == expression;

        EXPECT_EQ(0,
                  constraint_result.expression().sensitivities().at(&variable));
        EXPECT_EQ(0, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Equal,
                  constraint_result.sense());
    }

    /// Expression == VariableProxy
    {
        auto constraint_result = expression == variable_proxy;

        EXPECT_EQ(1,
                  constraint_result.expression().sensitivities().at(&variable));
        EXPECT_EQ(-1, constraint_result.expression().sensitivities().at(
                          &variable_proxy[0]));
        EXPECT_EQ(0, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Equal,
                  constraint_result.sense());
    }

    /// VariableProxy == Expression
    {
        auto constraint_result = variable_proxy == expression;

        EXPECT_EQ(1, constraint_result.expression().sensitivities().at(
                         &variable_proxy[0]));
        EXPECT_EQ(-1,
                  constraint_result.expression().sensitivities().at(&variable));
        EXPECT_EQ(0, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Equal,
                  constraint_result.sense());
    }

    /// Expression == ExpressionProxy
    {
        auto constraint_result = expression == expression_proxy;

        EXPECT_EQ(1,
                  constraint_result.expression().sensitivities().at(&variable));
        EXPECT_EQ(-1, constraint_result.expression().sensitivities().at(
                          &variable_proxy[0]));
        EXPECT_EQ(0, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Equal,
                  constraint_result.sense());
    }

    /// ExpressionProxy == Expression
    {
        auto constraint_result = expression_proxy == expression;

        EXPECT_EQ(1, constraint_result.expression().sensitivities().at(
                         &variable_proxy[0]));
        EXPECT_EQ(-1,
                  constraint_result.expression().sensitivities().at(&variable));
        EXPECT_EQ(0, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Equal,
                  constraint_result.sense());
    }
}

/*****************************************************************************/
TEST_F(TestConstraintBinary, expression_upper) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy   = model.create_variable("x");
    auto& expression_proxy = model.create_expression("e");
    auto  variable = printemps::model::Variable<int, double>::create_instance();
    auto  expression =
        printemps::model::Expression<int, double>::create_instance();
    auto constant = random_integer();

    expression_proxy = variable_proxy;
    expression       = variable;

    /// Expression >= Expression
    {
        auto constraint_result = expression >= expression;

        EXPECT_EQ(0,
                  constraint_result.expression().sensitivities().at(&variable));
        EXPECT_EQ(0, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Upper,
                  constraint_result.sense());
    }

    /// Expression >= Integer
    {
        auto constraint_result = expression >= constant;

        EXPECT_EQ(1,
                  constraint_result.expression().sensitivities().at(&variable));
        EXPECT_EQ(-constant, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Upper,
                  constraint_result.sense());
    }

    ///  Integer >= Expression
    {
        auto constraint_result = constant >= expression;

        EXPECT_EQ(-1,
                  constraint_result.expression().sensitivities().at(&variable));
        EXPECT_EQ(constant, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Upper,
                  constraint_result.sense());
    }

    /// Expression >= Variable
    {
        auto constraint_result = expression >= variable;

        EXPECT_EQ(0,
                  constraint_result.expression().sensitivities().at(&variable));
        EXPECT_EQ(0, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Upper,
                  constraint_result.sense());
    }

    /// Variable >= Expression
    {
        auto constraint_result = variable >= expression;

        EXPECT_EQ(0,
                  constraint_result.expression().sensitivities().at(&variable));
        EXPECT_EQ(0, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Upper,
                  constraint_result.sense());
    }

    /// Expression >= VariableProxy
    {
        auto constraint_result = expression >= variable_proxy;

        EXPECT_EQ(1,
                  constraint_result.expression().sensitivities().at(&variable));
        EXPECT_EQ(-1, constraint_result.expression().sensitivities().at(
                          &variable_proxy[0]));
        EXPECT_EQ(0, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Upper,
                  constraint_result.sense());
    }

    /// VariableProxy >= Expression
    {
        auto constraint_result = variable_proxy >= expression;

        EXPECT_EQ(1, constraint_result.expression().sensitivities().at(
                         &variable_proxy[0]));
        EXPECT_EQ(-1,
                  constraint_result.expression().sensitivities().at(&variable));
        EXPECT_EQ(0, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Upper,
                  constraint_result.sense());
    }

    /// Expression >= ExpressionProxy
    {
        auto constraint_result = expression >= expression_proxy;

        EXPECT_EQ(1,
                  constraint_result.expression().sensitivities().at(&variable));
        EXPECT_EQ(-1, constraint_result.expression().sensitivities().at(
                          &variable_proxy[0]));
        EXPECT_EQ(0, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Upper,
                  constraint_result.sense());
    }

    /// ExpressionProxy >= Expression
    {
        auto constraint_result = expression_proxy >= expression;

        EXPECT_EQ(1, constraint_result.expression().sensitivities().at(
                         &variable_proxy[0]));
        EXPECT_EQ(-1,
                  constraint_result.expression().sensitivities().at(&variable));
        EXPECT_EQ(0, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Upper,
                  constraint_result.sense());
    }
}

/*****************************************************************************/
TEST_F(TestConstraintBinary, expression_proxy_lower) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy   = model.create_variable("x");
    auto& expression_proxy = model.create_expression("e");
    auto  variable = printemps::model::Variable<int, double>::create_instance();
    auto  expression =
        printemps::model::Expression<int, double>::create_instance();
    auto constant = random_integer();

    expression_proxy = variable_proxy;
    expression       = variable;

    /// ExpressionProxy <= ExpressionProxy
    {
        auto constraint_result = expression_proxy <= expression_proxy;

        EXPECT_EQ(0, constraint_result.expression().sensitivities().at(
                         &variable_proxy[0]));
        EXPECT_EQ(0, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Lower,
                  constraint_result.sense());
    }

    /// ExpressionProxy <= Integer
    {
        auto constraint_result = expression_proxy <= constant;

        EXPECT_EQ(1, constraint_result.expression().sensitivities().at(
                         &variable_proxy[0]));
        EXPECT_EQ(-constant, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Lower,
                  constraint_result.sense());
    }

    ///  Integer <= ExpressionProxy
    {
        auto constraint_result = constant <= expression_proxy;

        EXPECT_EQ(-1, constraint_result.expression().sensitivities().at(
                          &variable_proxy[0]));
        EXPECT_EQ(constant, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Lower,
                  constraint_result.sense());
    }

    /// ExpressionProxy <= Variable
    {
        auto constraint_result = expression_proxy <= variable;

        EXPECT_EQ(1, constraint_result.expression().sensitivities().at(
                         &variable_proxy[0]));
        EXPECT_EQ(-1,
                  constraint_result.expression().sensitivities().at(&variable));
        EXPECT_EQ(0, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Lower,
                  constraint_result.sense());
    }

    /// Variable <= ExpressionProxy
    {
        auto constraint_result = variable <= expression_proxy;

        EXPECT_EQ(1,
                  constraint_result.expression().sensitivities().at(&variable));
        EXPECT_EQ(-1, constraint_result.expression().sensitivities().at(
                          &variable_proxy[0]));
        EXPECT_EQ(0, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Lower,
                  constraint_result.sense());
    }

    /// ExpressionProxy <= Expression
    {
        auto constraint_result = variable_proxy <= expression;

        EXPECT_EQ(1, constraint_result.expression().sensitivities().at(
                         &variable_proxy[0]));
        EXPECT_EQ(-1,
                  constraint_result.expression().sensitivities().at(&variable));
        EXPECT_EQ(0, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Lower,
                  constraint_result.sense());
    }

    /// Expression <= ExpressionProxy
    {
        auto constraint_result = expression <= variable_proxy;

        EXPECT_EQ(1,
                  constraint_result.expression().sensitivities().at(&variable));
        EXPECT_EQ(-1, constraint_result.expression().sensitivities().at(
                          &variable_proxy[0]));
        EXPECT_EQ(0, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Lower,
                  constraint_result.sense());
    }
}

/*****************************************************************************/
TEST_F(TestConstraintBinary, expression_proxy_equal) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy   = model.create_variable("x");
    auto& expression_proxy = model.create_expression("e");
    auto  variable = printemps::model::Variable<int, double>::create_instance();
    auto  expression =
        printemps::model::Expression<int, double>::create_instance();
    auto constant = random_integer();

    expression_proxy = variable_proxy;
    expression       = variable;

    /// ExpressionProxy == ExpressionProxy
    {
        auto constraint_result = expression_proxy == expression_proxy;

        EXPECT_EQ(0, constraint_result.expression().sensitivities().at(
                         &variable_proxy[0]));
        EXPECT_EQ(0, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Equal,
                  constraint_result.sense());
    }

    /// ExpressionProxy == Integer
    {
        auto constraint_result = expression_proxy == constant;

        EXPECT_EQ(1, constraint_result.expression().sensitivities().at(
                         &variable_proxy[0]));
        EXPECT_EQ(-constant, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Equal,
                  constraint_result.sense());
    }

    ///  Integer == ExpressionProxy
    {
        auto constraint_result = constant == expression_proxy;

        EXPECT_EQ(-1, constraint_result.expression().sensitivities().at(
                          &variable_proxy[0]));
        EXPECT_EQ(constant, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Equal,
                  constraint_result.sense());
    }

    /// ExpressionProxy == Variable
    {
        auto constraint_result = expression_proxy == variable;

        EXPECT_EQ(1, constraint_result.expression().sensitivities().at(
                         &variable_proxy[0]));
        EXPECT_EQ(-1,
                  constraint_result.expression().sensitivities().at(&variable));
        EXPECT_EQ(0, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Equal,
                  constraint_result.sense());
    }

    /// Variable == ExpressionProxy
    {
        auto constraint_result = variable == expression_proxy;

        EXPECT_EQ(1,
                  constraint_result.expression().sensitivities().at(&variable));
        EXPECT_EQ(-1, constraint_result.expression().sensitivities().at(
                          &variable_proxy[0]));
        EXPECT_EQ(0, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Equal,
                  constraint_result.sense());
    }

    /// ExpressionProxy == Expression
    {
        auto constraint_result = variable_proxy == expression;

        EXPECT_EQ(1, constraint_result.expression().sensitivities().at(
                         &variable_proxy[0]));
        EXPECT_EQ(-1,
                  constraint_result.expression().sensitivities().at(&variable));
        EXPECT_EQ(0, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Equal,
                  constraint_result.sense());
    }

    /// Expression == ExpressionProxy
    {
        auto constraint_result = expression == variable_proxy;

        EXPECT_EQ(1,
                  constraint_result.expression().sensitivities().at(&variable));
        EXPECT_EQ(-1, constraint_result.expression().sensitivities().at(
                          &variable_proxy[0]));
        EXPECT_EQ(0, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Equal,
                  constraint_result.sense());
    }
}

/*****************************************************************************/
TEST_F(TestConstraintBinary, expression_proxy_upper) {
    printemps::model::Model<int, double> model;

    auto& variable_proxy   = model.create_variable("x");
    auto& expression_proxy = model.create_expression("e");
    auto  variable = printemps::model::Variable<int, double>::create_instance();
    auto  expression =
        printemps::model::Expression<int, double>::create_instance();
    auto constant = random_integer();

    expression_proxy = variable_proxy;
    expression       = variable;

    /// ExpressionProxy >= ExpressionProxy
    {
        auto constraint_result = expression_proxy >= expression_proxy;

        EXPECT_EQ(0, constraint_result.expression().sensitivities().at(
                         &variable_proxy[0]));
        EXPECT_EQ(0, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Upper,
                  constraint_result.sense());
    }

    /// ExpressionProxy >= Integer
    {
        auto constraint_result = expression_proxy >= constant;

        EXPECT_EQ(1, constraint_result.expression().sensitivities().at(
                         &variable_proxy[0]));
        EXPECT_EQ(-constant, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Upper,
                  constraint_result.sense());
    }

    ///  Integer >= ExpressionProxy
    {
        auto constraint_result = constant >= expression_proxy;

        EXPECT_EQ(-1, constraint_result.expression().sensitivities().at(
                          &variable_proxy[0]));
        EXPECT_EQ(constant, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Upper,
                  constraint_result.sense());
    }

    /// ExpressionProxy >= Variable
    {
        auto constraint_result = expression_proxy >= variable;

        EXPECT_EQ(1, constraint_result.expression().sensitivities().at(
                         &variable_proxy[0]));
        EXPECT_EQ(-1,
                  constraint_result.expression().sensitivities().at(&variable));
        EXPECT_EQ(0, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Upper,
                  constraint_result.sense());
    }

    /// Variable >= ExpressionProxy
    {
        auto constraint_result = variable >= expression_proxy;

        EXPECT_EQ(1,
                  constraint_result.expression().sensitivities().at(&variable));
        EXPECT_EQ(-1, constraint_result.expression().sensitivities().at(
                          &variable_proxy[0]));
        EXPECT_EQ(0, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Upper,
                  constraint_result.sense());
    }

    /// ExpressionProxy >= Expression
    {
        auto constraint_result = variable_proxy >= expression;

        EXPECT_EQ(1, constraint_result.expression().sensitivities().at(
                         &variable_proxy[0]));
        EXPECT_EQ(-1,
                  constraint_result.expression().sensitivities().at(&variable));
        EXPECT_EQ(0, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Upper,
                  constraint_result.sense());
    }

    /// Expression >= ExpressionProxy
    {
        auto constraint_result = expression >= variable_proxy;

        EXPECT_EQ(1,
                  constraint_result.expression().sensitivities().at(&variable));
        EXPECT_EQ(-1, constraint_result.expression().sensitivities().at(
                          &variable_proxy[0]));
        EXPECT_EQ(0, constraint_result.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Upper,
                  constraint_result.sense());
    }
}

/*****************************************************************************/
}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/