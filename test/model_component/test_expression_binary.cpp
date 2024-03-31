/*****************************************************************************/
// Copyright (c) 2020-2024 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <gtest/gtest.h>
#include <printemps.h>

namespace {
using namespace printemps;
/*****************************************************************************/
class TestExpressionBinary : public ::testing::Test {
   protected:
    utility::UniformRandom<std::uniform_int_distribution<>, int>
        m_random_integer;
    utility::UniformRandom<std::uniform_int_distribution<>, int>
        m_random_positive_integer;

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
TEST_F(TestExpressionBinary, variable_operator_add) {
    model::Model<int, double> model;

    auto& variable_proxy   = model.create_variable("x");
    auto& expression_proxy = model.create_expression("e");
    auto  variable = model_component::Variable<int, double>::create_instance();
    auto  expression =
        model_component::Expression<int, double>::create_instance();

    auto constant = random_integer();

    expression_proxy = variable_proxy;
    expression       = variable;

    /// Variable + Variable
    {
        auto expression_result = variable + variable;

        EXPECT_EQ(2, expression_result.sensitivities().at(&variable));
        EXPECT_EQ(0, expression_result.constant_value());
    }

    /// Variable + VariableProxy
    {
        auto expression_result = variable + variable_proxy;

        EXPECT_EQ(1, expression_result.sensitivities().at(&variable));
        EXPECT_EQ(1, expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(0, expression_result.constant_value());
    }

    /// VariableProxy + Variable
    {
        auto expression_result = variable_proxy + variable;

        EXPECT_EQ(1, expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(1, expression_result.sensitivities().at(&(variable)));
        EXPECT_EQ(0, expression_result.constant_value());
    }

    /// Variable + Integer
    {
        auto expression_result = variable + constant;

        EXPECT_EQ(1, expression_result.sensitivities().at(&variable));
        EXPECT_EQ(constant, expression_result.constant_value());
    }

    ///  Integer + Variable
    {
        auto expression_result = constant + variable;

        EXPECT_EQ(1, expression_result.sensitivities().at(&variable));
        EXPECT_EQ(constant, expression_result.constant_value());
    }

    /// Variable + ExpressionProxy
    {
        auto expression_result = variable + expression_proxy;

        EXPECT_EQ(1, expression_result.sensitivities().at(&variable));
        EXPECT_EQ(1, expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(0, expression_result.constant_value());
    }

    /// ExpressionProxy + Variable
    {
        auto expression_result = expression_proxy + variable;

        EXPECT_EQ(1, expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(1, expression_result.sensitivities().at(&variable));
        EXPECT_EQ(0, expression_result.constant_value());
    }

    /// Variable + Expression
    {
        auto expression_result = variable + expression;

        EXPECT_EQ(2, expression_result.sensitivities().at(&variable));
        EXPECT_EQ(0, expression_result.constant_value());
    }

    /// Expression + Variable
    {
        auto expression_result = expression + variable;

        EXPECT_EQ(2, expression_result.sensitivities().at(&variable));
        EXPECT_EQ(0, expression_result.constant_value());
    }
}

/*****************************************************************************/
TEST_F(TestExpressionBinary, variable_proxy_operator_add) {
    model::Model<int, double> model;

    auto& variable_proxy   = model.create_variable("x");
    auto& expression_proxy = model.create_expression("e");
    auto  variable = model_component::Variable<int, double>::create_instance();
    auto  expression =
        model_component::Expression<int, double>::create_instance();
    auto constant = random_integer();

    expression_proxy = variable_proxy;
    expression       = variable;

    /// VariableProxy + VariableProxy
    {
        auto expression_result = variable_proxy + variable_proxy;

        EXPECT_EQ(2, expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(0, expression_result.constant_value());
    }

    /// VariableProxy + Variable
    {
        auto expression_result = variable_proxy + variable;

        EXPECT_EQ(1, expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(1, expression_result.sensitivities().at(&(variable)));
        EXPECT_EQ(0, expression_result.constant_value());
    }

    /// Variable + VariableProxy
    {
        auto expression_result = variable + variable_proxy;

        EXPECT_EQ(1, expression_result.sensitivities().at(&variable));
        EXPECT_EQ(1, expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(0, expression_result.constant_value());
    }

    /// VariableProxy + Integer
    {
        auto expression_result = variable_proxy + constant;

        EXPECT_EQ(1, expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(constant, expression_result.constant_value());
    }

    ///  Integer + VariableProxy
    {
        auto expression_result = constant + variable_proxy;

        EXPECT_EQ(1, expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(constant, expression_result.constant_value());
    }

    /// VariableProxy + ExpressionProxy
    {
        auto expression_result = variable_proxy + expression_proxy;

        EXPECT_EQ(2, expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(0, expression_result.constant_value());
    }

    /// ExpressionProxy + VariableProxy
    {
        auto expression_result = expression_proxy + variable_proxy;

        EXPECT_EQ(2, expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(0, expression_result.constant_value());
    }

    /// VariableProxy + Expression
    {
        auto expression_result = variable_proxy + expression;

        EXPECT_EQ(1, expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(1, expression_result.sensitivities().at(&variable));
        EXPECT_EQ(0, expression_result.constant_value());
    }

    /// Expression + VariableProxy
    {
        auto expression_result = expression + variable_proxy;

        EXPECT_EQ(1, expression_result.sensitivities().at(&variable));
        EXPECT_EQ(1, expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(0, expression_result.constant_value());
    }
}

/*****************************************************************************/
TEST_F(TestExpressionBinary, expression_operator_add) {
    model::Model<int, double> model;

    auto& variable_proxy   = model.create_variable("x");
    auto& expression_proxy = model.create_expression("e");
    auto  variable = model_component::Variable<int, double>::create_instance();
    auto  expression =
        model_component::Expression<int, double>::create_instance();
    auto constant = random_integer();

    expression_proxy = variable_proxy;
    expression       = variable;

    /// Expression + Expression
    {
        auto expression_result = expression + expression;

        EXPECT_EQ(2, expression_result.sensitivities().at(&variable));
        EXPECT_EQ(0, expression_result.constant_value());
    }

    /// Expression + Variable
    {
        auto expression_result = expression + variable;

        EXPECT_EQ(2, expression_result.sensitivities().at(&variable));
        EXPECT_EQ(0, expression_result.constant_value());
    }

    /// Variable + Expression
    {
        auto expression_result = variable + expression;

        EXPECT_EQ(2, expression_result.sensitivities().at(&variable));
        EXPECT_EQ(0, expression_result.constant_value());
    }

    /// Expression + Integer
    {
        auto expression_result = expression + constant;

        EXPECT_EQ(1, expression_result.sensitivities().at(&variable));
        EXPECT_EQ(constant, expression_result.constant_value());
    }

    ///  Integer + Expression
    {
        auto expression_result = constant + expression;

        EXPECT_EQ(1, expression_result.sensitivities().at(&variable));
        EXPECT_EQ(constant, expression_result.constant_value());
    }

    /// Expression + VariableProxy
    {
        auto expression_result = expression + variable_proxy;

        EXPECT_EQ(1, expression_result.sensitivities().at(&variable));
        EXPECT_EQ(1, expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(0, expression_result.constant_value());
    }

    /// VariableProxy + Expression
    {
        auto expression_result = variable_proxy + expression;

        EXPECT_EQ(1, expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(1, expression_result.sensitivities().at(&variable));
        EXPECT_EQ(0, expression_result.constant_value());
    }

    /// Expression + ExpressionProxy
    {
        auto expression_result = expression + expression_proxy;

        EXPECT_EQ(1, expression_result.sensitivities().at(&variable));
        EXPECT_EQ(1, expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(0, expression_result.constant_value());
    }

    /// ExpressionProxy + Expression
    {
        auto expression_result = expression_proxy + expression;

        EXPECT_EQ(1, expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(1, expression_result.sensitivities().at(&variable));
        EXPECT_EQ(0, expression_result.constant_value());
    }
}

/*****************************************************************************/
TEST_F(TestExpressionBinary, expression_proxy_operator_add) {
    model::Model<int, double> model;

    auto& variable_proxy   = model.create_variable("x");
    auto& expression_proxy = model.create_expression("e");
    auto  variable = model_component::Variable<int, double>::create_instance();
    auto  expression =
        model_component::Expression<int, double>::create_instance();
    auto constant = random_integer();

    expression_proxy = variable_proxy;
    expression       = variable;

    /// ExpressionProxy + ExpressionProxy
    {
        auto expression_result = expression_proxy + expression_proxy;

        EXPECT_EQ(2, expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(0, expression_result.constant_value());
    }

    /// ExpressionProxy + Variable
    {
        auto expression_result = expression_proxy + variable;

        EXPECT_EQ(1, expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(1, expression_result.sensitivities().at(&(variable)));
        EXPECT_EQ(0, expression_result.constant_value());
    }

    /// Variable + ExpressionProxy
    {
        auto expression_result = variable + expression_proxy;

        EXPECT_EQ(1, expression_result.sensitivities().at(&variable));
        EXPECT_EQ(1, expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(0, expression_result.constant_value());
    }

    /// ExpressionProxy + Integer
    {
        auto expression_result = expression_proxy + constant;

        EXPECT_EQ(1, expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(constant, expression_result.constant_value());
    }

    ///  Integer + ExpressionProxy
    {
        auto expression_result = constant + expression_proxy;

        EXPECT_EQ(1, expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(constant, expression_result.constant_value());
    }

    /// ExpressionProxy + VariableProxy
    {
        auto expression_result = expression_proxy + variable_proxy;

        EXPECT_EQ(2, expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(0, expression_result.constant_value());
    }

    /// VariableProxy + ExpressionProxy
    {
        auto expression_result = variable_proxy + expression_proxy;

        EXPECT_EQ(2, expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(0, expression_result.constant_value());
    }

    /// ExpressionProxy + Expression
    {
        auto expression_result = expression_proxy + expression;

        EXPECT_EQ(1, expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(1, expression_result.sensitivities().at(&variable));
        EXPECT_EQ(0, expression_result.constant_value());
    }

    /// Expression + ExpressionProxy
    {
        auto expression_result = expression + expression_proxy;

        EXPECT_EQ(1, expression_result.sensitivities().at(&variable));
        EXPECT_EQ(1, expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(0, expression_result.constant_value());
    }
}

/*****************************************************************************/
TEST_F(TestExpressionBinary, variable_operator_subtract) {
    model::Model<int, double> model;

    auto& variable_proxy   = model.create_variable("x");
    auto& expression_proxy = model.create_expression("e");
    auto  variable = model_component::Variable<int, double>::create_instance();
    auto  expression =
        model_component::Expression<int, double>::create_instance();

    auto constant = random_integer();

    expression_proxy = variable_proxy;
    expression       = variable;

    /// Variable - Variable
    {
        auto expression_result = variable - variable;

        EXPECT_EQ(0, expression_result.sensitivities().at(&variable));
        EXPECT_EQ(0, expression_result.constant_value());
    }

    /// Variable - VariableProxy

    {
        auto expression_result = variable - variable_proxy;

        EXPECT_EQ(1, expression_result.sensitivities().at(&variable));
        EXPECT_EQ(-1, expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(0, expression_result.constant_value());
    }

    /// VariableProxy - Variable
    {
        auto expression_result = variable_proxy - variable;

        EXPECT_EQ(1, expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(-1, expression_result.sensitivities().at(&(variable)));
        EXPECT_EQ(0, expression_result.constant_value());
    }

    /// Variable - Integer
    {
        auto expression_result = variable - constant;

        EXPECT_EQ(1, expression_result.sensitivities().at(&variable));
        EXPECT_EQ(-constant, expression_result.constant_value());
    }

    ///  Integer - Variable
    {
        auto expression_result = constant - variable;

        EXPECT_EQ(-1, expression_result.sensitivities().at(&variable));
        EXPECT_EQ(constant, expression_result.constant_value());
    }

    /// Variable - ExpressionProxy
    {
        auto expression_result = variable - expression_proxy;

        EXPECT_EQ(1, expression_result.sensitivities().at(&variable));
        EXPECT_EQ(-1, expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(0, expression_result.constant_value());
    }

    /// ExpressionProxy - Variable
    {
        auto expression_result = expression_proxy - variable;

        EXPECT_EQ(1, expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(-1, expression_result.sensitivities().at(&variable));
        EXPECT_EQ(0, expression_result.constant_value());
    }

    /// Variable - Expression
    {
        auto expression_result = variable - expression;

        EXPECT_EQ(0, expression_result.sensitivities().at(&variable));
        EXPECT_EQ(0, expression_result.constant_value());
    }

    /// Expression - Variable
    {
        auto expression_result = expression - variable;

        EXPECT_EQ(0, expression_result.sensitivities().at(&variable));
        EXPECT_EQ(0, expression_result.constant_value());
    }
}

/*****************************************************************************/
TEST_F(TestExpressionBinary, variable_proxy_operator_subtract) {
    model::Model<int, double> model;

    auto& variable_proxy   = model.create_variable("x");
    auto& expression_proxy = model.create_expression("e");
    auto  variable = model_component::Variable<int, double>::create_instance();
    auto  expression =
        model_component::Expression<int, double>::create_instance();
    auto constant = random_integer();

    expression_proxy = variable_proxy;
    expression       = variable;

    /// VariableProxy - VariableProxy
    {
        auto expression_result = variable_proxy - variable_proxy;

        EXPECT_EQ(0, expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(0, expression_result.constant_value());
    }

    /// VariableProxy - Variable
    {
        auto expression_result = variable_proxy - variable;

        EXPECT_EQ(1, expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(-1, expression_result.sensitivities().at(&(variable)));
        EXPECT_EQ(0, expression_result.constant_value());
    }

    /// Variable - VariableProxy
    {
        auto expression_result = variable - variable_proxy;

        EXPECT_EQ(1, expression_result.sensitivities().at(&variable));
        EXPECT_EQ(-1, expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(0, expression_result.constant_value());
    }

    /// VariableProxy - Integer
    {
        auto expression_result = variable_proxy - constant;

        EXPECT_EQ(1, expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(-constant, expression_result.constant_value());
    }

    ///  Integer - VariableProxy
    {
        auto expression_result = constant - variable_proxy;

        EXPECT_EQ(-1, expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(constant, expression_result.constant_value());
    }

    /// VariableProxy - ExpressionProxy
    {
        auto expression_result = variable_proxy - expression_proxy;

        EXPECT_EQ(0, expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(0, expression_result.constant_value());
    }

    /// ExpressionProxy - VariableProxy
    {
        auto expression_result = expression_proxy - variable_proxy;

        EXPECT_EQ(0, expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(0, expression_result.constant_value());
    }

    /// VariableProxy - Expression
    {
        auto expression_result = variable_proxy - expression;

        EXPECT_EQ(1, expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(-1, expression_result.sensitivities().at(&variable));
        EXPECT_EQ(0, expression_result.constant_value());
    }

    /// Expression - VariableProxy
    {
        auto expression_result = expression - variable_proxy;

        EXPECT_EQ(1, expression_result.sensitivities().at(&variable));
        EXPECT_EQ(-1, expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(0, expression_result.constant_value());
    }
}

/*****************************************************************************/
TEST_F(TestExpressionBinary, expression_operator_subtract) {
    model::Model<int, double> model;

    auto& variable_proxy   = model.create_variable("x");
    auto& expression_proxy = model.create_expression("e");
    auto  variable = model_component::Variable<int, double>::create_instance();
    auto  expression =
        model_component::Expression<int, double>::create_instance();
    auto constant = random_integer();

    expression_proxy = variable_proxy;
    expression       = variable;

    /// Expression - Expression
    {
        auto expression_result = expression - expression;

        EXPECT_EQ(0, expression_result.sensitivities().at(&variable));
        EXPECT_EQ(0, expression_result.constant_value());
    }

    /// Expression - Variable
    {
        auto expression_result = expression - variable;

        EXPECT_EQ(0, expression_result.sensitivities().at(&variable));
        EXPECT_EQ(0, expression_result.constant_value());
    }

    /// Variable - Expression
    {
        auto expression_result = variable - expression;

        EXPECT_EQ(0, expression_result.sensitivities().at(&variable));
        EXPECT_EQ(0, expression_result.constant_value());
    }

    /// Expression - Integer
    {
        auto expression_result = expression - constant;

        EXPECT_EQ(1, expression_result.sensitivities().at(&variable));
        EXPECT_EQ(-constant, expression_result.constant_value());
    }

    ///  Integer - Expression
    {
        auto expression_result = constant - expression;

        EXPECT_EQ(-1, expression_result.sensitivities().at(&variable));
        EXPECT_EQ(constant, expression_result.constant_value());
    }

    /// Expression - VariableProxy
    {
        auto expression_result = expression - variable_proxy;

        EXPECT_EQ(1, expression_result.sensitivities().at(&variable));
        EXPECT_EQ(-1, expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(0, expression_result.constant_value());
    }

    /// VariableProxy - Expression
    {
        auto expression_result = variable_proxy - expression;

        EXPECT_EQ(1, expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(-1, expression_result.sensitivities().at(&variable));
        EXPECT_EQ(0, expression_result.constant_value());
    }

    /// Expression - ExpressionProxy
    {
        auto expression_result = expression - expression_proxy;

        EXPECT_EQ(1, expression_result.sensitivities().at(&variable));
        EXPECT_EQ(-1, expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(0, expression_result.constant_value());
    }

    /// ExpressionProxy - Expression
    {
        auto expression_result = expression_proxy - expression;

        EXPECT_EQ(1, expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(-1, expression_result.sensitivities().at(&variable));
        EXPECT_EQ(0, expression_result.constant_value());
    }
}

/*****************************************************************************/
TEST_F(TestExpressionBinary, expression_proxy_operator_subtract) {
    model::Model<int, double> model;

    auto& variable_proxy   = model.create_variable("x");
    auto& expression_proxy = model.create_expression("e");
    auto  variable = model_component::Variable<int, double>::create_instance();
    auto  expression =
        model_component::Expression<int, double>::create_instance();
    auto constant = random_integer();

    expression_proxy = variable_proxy;
    expression       = variable;

    /// ExpressionProxy - ExpressionProxy
    {
        auto expression_result = expression_proxy - expression_proxy;

        EXPECT_EQ(0, expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(0, expression_result.constant_value());
    }

    /// ExpressionProxy - Variable
    {
        auto expression_result = expression_proxy - variable;

        EXPECT_EQ(1, expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(-1, expression_result.sensitivities().at(&(variable)));
        EXPECT_EQ(0, expression_result.constant_value());
    }

    /// Variable - ExpressionProxy
    {
        auto expression_result = variable - expression_proxy;

        EXPECT_EQ(1, expression_result.sensitivities().at(&variable));
        EXPECT_EQ(-1, expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(0, expression_result.constant_value());
    }

    /// ExpressionProxy - Integer
    {
        auto expression_result = expression_proxy - constant;

        EXPECT_EQ(1, expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(-constant, expression_result.constant_value());
    }

    ///  Integer - ExpressionProxy
    {
        auto expression_result = constant - expression_proxy;

        EXPECT_EQ(-1, expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(constant, expression_result.constant_value());
    }

    /// ExpressionProxy - VariableProxy
    {
        auto expression_result = expression_proxy - variable_proxy;

        EXPECT_EQ(0, expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(0, expression_result.constant_value());
    }

    /// VariableProxy - ExpressionProxy
    {
        auto expression_result = variable_proxy - expression_proxy;

        EXPECT_EQ(0, expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(0, expression_result.constant_value());
    }

    /// ExpressionProxy - Expression
    {
        auto expression_result = expression_proxy - expression;

        EXPECT_EQ(1, expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(-1, expression_result.sensitivities().at(&variable));
        EXPECT_EQ(0, expression_result.constant_value());
    }

    /// Expression - ExpressionProxy
    {
        auto expression_result = expression - expression_proxy;

        EXPECT_EQ(1, expression_result.sensitivities().at(&variable));
        EXPECT_EQ(-1, expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(0, expression_result.constant_value());
    }
}

/*****************************************************************************/
TEST_F(TestExpressionBinary, variable_combination_add_product) {
    model::Model<int, double> model;

    auto& variable_proxy   = model.create_variable("x");
    auto& expression_proxy = model.create_expression("e");
    auto  variable = model_component::Variable<int, double>::create_instance();
    auto  expression =
        model_component::Expression<int, double>::create_instance();

    auto sensitivity_0 = random_integer();
    auto sensitivity_1 = random_integer();
    auto constant      = random_integer();

    expression_proxy = variable_proxy;
    expression       = variable;

    /// Variable + Variable
    {
        auto expression_result =
            sensitivity_0 * variable + sensitivity_1 * variable;

        EXPECT_EQ(sensitivity_0 + sensitivity_1,
                  expression_result.sensitivities().at(&variable));
        EXPECT_EQ(0, expression_result.constant_value());
    }

    /// Variable + VariableProxy
    {
        auto expression_result =
            sensitivity_0 * variable + sensitivity_1 * variable_proxy;

        EXPECT_EQ(sensitivity_0,
                  expression_result.sensitivities().at(&variable));
        EXPECT_EQ(sensitivity_1,
                  expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(0, expression_result.constant_value());
    }

    /// VariableProxy + Variable
    {
        auto expression_result =
            sensitivity_0 * variable_proxy + sensitivity_1 * variable;

        EXPECT_EQ(sensitivity_0,
                  expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(sensitivity_1,
                  expression_result.sensitivities().at(&(variable)));
        EXPECT_EQ(0, expression_result.constant_value());
    }

    /// Variable + Integer
    {
        auto expression_result = sensitivity_0 * variable + constant;

        EXPECT_EQ(sensitivity_0,
                  expression_result.sensitivities().at(&variable));
        EXPECT_EQ(constant, expression_result.constant_value());
    }

    ///  Integer + Variable
    {
        auto expression_result = constant + sensitivity_0 * variable;

        EXPECT_EQ(sensitivity_0,
                  expression_result.sensitivities().at(&variable));
        EXPECT_EQ(constant, expression_result.constant_value());
    }

    /// Variable + ExpressionProxy
    {
        auto expression_result =
            sensitivity_0 * variable + sensitivity_1 * expression_proxy;

        EXPECT_EQ(sensitivity_0,
                  expression_result.sensitivities().at(&variable));
        EXPECT_EQ(sensitivity_1,
                  expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(0, expression_result.constant_value());
    }

    /// ExpressionProxy + Variable
    {
        auto expression_result =
            sensitivity_0 * expression_proxy + sensitivity_1 * variable;

        EXPECT_EQ(sensitivity_0,
                  expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(sensitivity_1,
                  expression_result.sensitivities().at(&variable));
        EXPECT_EQ(0, expression_result.constant_value());
    }

    /// Variable + Expression
    {
        auto expression_result =
            sensitivity_0 * variable + sensitivity_1 * expression;

        EXPECT_EQ(sensitivity_0 + sensitivity_1,
                  expression_result.sensitivities().at(&variable));
        EXPECT_EQ(0, expression_result.constant_value());
    }

    /// Expression + Variable
    {
        auto expression_result =
            sensitivity_0 * expression + sensitivity_1 * variable;

        EXPECT_EQ(sensitivity_0 + sensitivity_1,
                  expression_result.sensitivities().at(&variable));
        EXPECT_EQ(0, expression_result.constant_value());
    }
}

/*****************************************************************************/
TEST_F(TestExpressionBinary, variable_proxy_combination_add_product) {
    model::Model<int, double> model;

    auto& variable_proxy   = model.create_variable("x");
    auto& expression_proxy = model.create_expression("e");
    auto  variable = model_component::Variable<int, double>::create_instance();
    auto  expression =
        model_component::Expression<int, double>::create_instance();

    auto sensitivity_0 = random_integer();
    auto sensitivity_1 = random_integer();
    auto constant      = random_integer();

    expression_proxy = variable_proxy;
    expression       = variable;

    /// VariableProxy + VariableProxy
    {
        auto expression_result =
            sensitivity_0 * variable_proxy + sensitivity_1 * variable_proxy;

        EXPECT_EQ(sensitivity_0 + sensitivity_1,
                  expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(0, expression_result.constant_value());
    }

    /// VariableProxy + Variable
    {
        auto expression_result =
            sensitivity_0 * variable_proxy + sensitivity_1 * variable;

        EXPECT_EQ(sensitivity_0,
                  expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(sensitivity_1,
                  expression_result.sensitivities().at(&(variable)));
        EXPECT_EQ(0, expression_result.constant_value());
    }

    /// Variable + VariableProxy
    {
        auto expression_result =
            sensitivity_0 * variable + sensitivity_1 * variable_proxy;

        EXPECT_EQ(sensitivity_0,
                  expression_result.sensitivities().at(&variable));
        EXPECT_EQ(sensitivity_1,
                  expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(0, expression_result.constant_value());
    }

    /// VariableProxy + Integer
    {
        auto expression_result = sensitivity_0 * variable_proxy + constant;

        EXPECT_EQ(sensitivity_0,
                  expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(constant, expression_result.constant_value());
    }

    ///  Integer + VariableProxy
    {
        auto expression_result = constant + sensitivity_0 * variable_proxy;

        EXPECT_EQ(sensitivity_0,
                  expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(constant, expression_result.constant_value());
    }

    /// VariableProxy + ExpressionProxy
    {
        auto expression_result =
            sensitivity_0 * variable_proxy + sensitivity_1 * expression_proxy;

        EXPECT_EQ(sensitivity_0 + sensitivity_1,
                  expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(0, expression_result.constant_value());
    }

    /// ExpressionProxy + VariableProxy
    {
        auto expression_result =
            sensitivity_0 * expression_proxy + sensitivity_1 * variable_proxy;

        EXPECT_EQ(sensitivity_0 + sensitivity_1,
                  expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(0, expression_result.constant_value());
    }

    /// VariableProxy + Expression
    {
        auto expression_result =
            sensitivity_0 * variable_proxy + sensitivity_1 * expression;

        EXPECT_EQ(sensitivity_0,
                  expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(sensitivity_1,
                  expression_result.sensitivities().at(&variable));
        EXPECT_EQ(0, expression_result.constant_value());
    }

    /// Expression + VariableProxy
    {
        auto expression_result =
            sensitivity_0 * expression + sensitivity_1 * variable_proxy;

        EXPECT_EQ(sensitivity_0,
                  expression_result.sensitivities().at(&variable));
        EXPECT_EQ(sensitivity_1,
                  expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(0, expression_result.constant_value());
    }
}

/*****************************************************************************/
TEST_F(TestExpressionBinary, expression_combination_add_product) {
    model::Model<int, double> model;

    auto& variable_proxy   = model.create_variable("x");
    auto& expression_proxy = model.create_expression("e");
    auto  variable = model_component::Variable<int, double>::create_instance();
    auto  expression =
        model_component::Expression<int, double>::create_instance();

    auto sensitivity_0 = random_integer();
    auto sensitivity_1 = random_integer();
    auto constant      = random_integer();

    expression_proxy = variable_proxy;
    expression       = variable;

    /// Expression + Expression
    {
        auto expression_result =
            sensitivity_0 * expression + sensitivity_1 * expression;

        EXPECT_EQ(sensitivity_0 + sensitivity_1,
                  expression_result.sensitivities().at(&variable));
        EXPECT_EQ(0, expression_result.constant_value());
    }

    /// Expression + Variable
    {
        auto expression_result =
            sensitivity_0 * expression + sensitivity_1 * variable;

        EXPECT_EQ(sensitivity_0 + sensitivity_1,
                  expression_result.sensitivities().at(&variable));
        EXPECT_EQ(0, expression_result.constant_value());
    }

    /// Variable + Expression
    {
        auto expression_result =
            sensitivity_0 * variable + sensitivity_1 * expression;

        EXPECT_EQ(sensitivity_0 + sensitivity_1,
                  expression_result.sensitivities().at(&variable));
        EXPECT_EQ(0, expression_result.constant_value());
    }

    /// Expression + Integer
    {
        auto expression_result = sensitivity_0 * expression + constant;

        EXPECT_EQ(sensitivity_0,
                  expression_result.sensitivities().at(&variable));
        EXPECT_EQ(constant, expression_result.constant_value());
    }

    ///  Integer + Expression
    {
        auto expression_result = constant + sensitivity_0 * expression;

        EXPECT_EQ(sensitivity_0,
                  expression_result.sensitivities().at(&variable));
        EXPECT_EQ(constant, expression_result.constant_value());
    }

    /// Expression + VariableProxy
    {
        auto expression_result =
            sensitivity_0 * expression + sensitivity_1 * variable_proxy;

        EXPECT_EQ(sensitivity_0,
                  expression_result.sensitivities().at(&variable));
        EXPECT_EQ(sensitivity_1,
                  expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(0, expression_result.constant_value());
    }

    /// VariableProxy + Expression
    {
        auto expression_result =
            sensitivity_0 * variable_proxy + sensitivity_1 * expression;

        EXPECT_EQ(sensitivity_0,
                  expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(sensitivity_1,
                  expression_result.sensitivities().at(&variable));
        EXPECT_EQ(0, expression_result.constant_value());
    }

    /// Expression + ExpressionProxy
    {
        auto expression_result =
            sensitivity_0 * expression + sensitivity_1 * expression_proxy;

        EXPECT_EQ(sensitivity_0,
                  expression_result.sensitivities().at(&variable));
        EXPECT_EQ(sensitivity_1,
                  expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(0, expression_result.constant_value());
    }

    /// ExpressionProxy + Expression
    {
        auto expression_result =
            sensitivity_0 * expression_proxy + sensitivity_1 * expression;

        EXPECT_EQ(sensitivity_0,
                  expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(sensitivity_1,
                  expression_result.sensitivities().at(&variable));
        EXPECT_EQ(0, expression_result.constant_value());
    }
}

/*****************************************************************************/
TEST_F(TestExpressionBinary, expression_proxy_combination_add_product) {
    model::Model<int, double> model;

    auto& variable_proxy   = model.create_variable("x");
    auto& expression_proxy = model.create_expression("e");
    auto  variable = model_component::Variable<int, double>::create_instance();
    auto  expression =
        model_component::Expression<int, double>::create_instance();

    auto sensitivity_0 = random_integer();
    auto sensitivity_1 = random_integer();
    auto constant      = random_integer();

    expression_proxy = variable_proxy;
    expression       = variable;

    /// ExpressionProxy + ExpressionProxy
    {
        auto expression_result =
            sensitivity_0 * expression_proxy + sensitivity_1 * expression_proxy;

        EXPECT_EQ(sensitivity_0 + sensitivity_1,
                  expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(0, expression_result.constant_value());
    }

    /// ExpressionProxy + Variable
    {
        auto expression_result =
            sensitivity_0 * expression_proxy + sensitivity_1 * variable;

        EXPECT_EQ(sensitivity_0,
                  expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(sensitivity_1,
                  expression_result.sensitivities().at(&(variable)));
        EXPECT_EQ(0, expression_result.constant_value());
    }

    /// Variable + ExpressionProxy
    {
        auto expression_result =
            sensitivity_0 * variable + sensitivity_1 * expression_proxy;

        EXPECT_EQ(sensitivity_0,
                  expression_result.sensitivities().at(&variable));
        EXPECT_EQ(sensitivity_1,
                  expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(0, expression_result.constant_value());
    }

    /// ExpressionProxy + Integer
    {
        auto expression_result = sensitivity_0 * expression_proxy + constant;

        EXPECT_EQ(sensitivity_0,
                  expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(constant, expression_result.constant_value());
    }

    ///  Integer + ExpressionProxy
    {
        auto expression_result = constant + sensitivity_0 * expression_proxy;

        EXPECT_EQ(sensitivity_0,
                  expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(constant, expression_result.constant_value());
    }

    /// ExpressionProxy + VariableProxy
    {
        auto expression_result =
            sensitivity_0 * expression_proxy + sensitivity_1 * variable_proxy;

        EXPECT_EQ(sensitivity_0 + sensitivity_1,
                  expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(0, expression_result.constant_value());
    }

    /// VariableProxy + ExpressionProxy
    {
        auto expression_result =
            sensitivity_0 * variable_proxy + sensitivity_1 * expression_proxy;

        EXPECT_EQ(sensitivity_0 + sensitivity_1,
                  expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(0, expression_result.constant_value());
    }

    /// ExpressionProxy + Expression
    {
        auto expression_result =
            sensitivity_0 * expression_proxy + sensitivity_1 * expression;

        EXPECT_EQ(sensitivity_0,
                  expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(sensitivity_1,
                  expression_result.sensitivities().at(&variable));
        EXPECT_EQ(0, expression_result.constant_value());
    }

    /// Expression + ExpressionProxy
    {
        auto expression_result =
            sensitivity_0 * expression + sensitivity_1 * expression_proxy;

        EXPECT_EQ(sensitivity_0,
                  expression_result.sensitivities().at(&variable));
        EXPECT_EQ(sensitivity_1,
                  expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(0, expression_result.constant_value());
    }
}

/*****************************************************************************/
TEST_F(TestExpressionBinary, variable_combination_subtract_product) {
    model::Model<int, double> model;

    auto& variable_proxy   = model.create_variable("x");
    auto& expression_proxy = model.create_expression("e");
    auto  variable = model_component::Variable<int, double>::create_instance();
    auto  expression =
        model_component::Expression<int, double>::create_instance();

    auto sensitivity_0 = random_integer();
    auto sensitivity_1 = random_integer();
    auto constant      = random_integer();

    expression_proxy = variable_proxy;
    expression       = variable;

    /// Variable - Variable
    {
        auto expression_result =
            sensitivity_0 * variable - sensitivity_1 * variable;

        EXPECT_EQ(sensitivity_0 - sensitivity_1,
                  expression_result.sensitivities().at(&variable));
        EXPECT_EQ(0, expression_result.constant_value());
    }

    /// Variable - VariableProxy
    {
        auto expression_result =
            sensitivity_0 * variable - sensitivity_1 * variable_proxy;

        EXPECT_EQ(sensitivity_0,
                  expression_result.sensitivities().at(&variable));
        EXPECT_EQ(-sensitivity_1,
                  expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(0, expression_result.constant_value());
    }

    /// VariableProxy - Variable
    {
        auto expression_result =
            sensitivity_0 * variable_proxy - sensitivity_1 * variable;

        EXPECT_EQ(sensitivity_0,
                  expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(-sensitivity_1,
                  expression_result.sensitivities().at(&(variable)));
        EXPECT_EQ(0, expression_result.constant_value());
    }

    /// Variable - Integer
    {
        auto expression_result = sensitivity_0 * variable - constant;

        EXPECT_EQ(sensitivity_0,
                  expression_result.sensitivities().at(&variable));
        EXPECT_EQ(-constant, expression_result.constant_value());
    }

    ///  Integer - Variable
    {
        auto expression_result = constant - sensitivity_0 * variable;

        EXPECT_EQ(-sensitivity_0,
                  expression_result.sensitivities().at(&variable));
        EXPECT_EQ(constant, expression_result.constant_value());
    }

    /// Variable - ExpressionProxy
    {
        auto expression_result =
            sensitivity_0 * variable - sensitivity_1 * expression_proxy;

        EXPECT_EQ(sensitivity_0,
                  expression_result.sensitivities().at(&variable));
        EXPECT_EQ(-sensitivity_1,
                  expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(0, expression_result.constant_value());
    }

    /// ExpressionProxy - Variable
    {
        auto expression_result =
            sensitivity_0 * expression_proxy - sensitivity_1 * variable;

        EXPECT_EQ(sensitivity_0,
                  expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(-sensitivity_1,
                  expression_result.sensitivities().at(&variable));
        EXPECT_EQ(0, expression_result.constant_value());
    }

    /// Variable - Expression
    {
        auto expression_result =
            sensitivity_0 * variable - sensitivity_1 * expression;

        EXPECT_EQ(sensitivity_0 - sensitivity_1,
                  expression_result.sensitivities().at(&variable));
        EXPECT_EQ(0, expression_result.constant_value());
    }

    /// Expression - Variable
    {
        auto expression_result =
            sensitivity_0 * expression - sensitivity_1 * variable;

        EXPECT_EQ(sensitivity_0 - sensitivity_1,
                  expression_result.sensitivities().at(&variable));
        EXPECT_EQ(0, expression_result.constant_value());
    }
}

/*****************************************************************************/
TEST_F(TestExpressionBinary, variable_proxy_combination_subtract_product) {
    model::Model<int, double> model;

    auto& variable_proxy   = model.create_variable("x");
    auto& expression_proxy = model.create_expression("e");
    auto  variable = model_component::Variable<int, double>::create_instance();
    auto  expression =
        model_component::Expression<int, double>::create_instance();

    auto sensitivity_0 = random_integer();
    auto sensitivity_1 = random_integer();
    auto constant      = random_integer();

    expression_proxy = variable_proxy;
    expression       = variable;

    /// VariableProxy - VariableProxy
    {
        auto expression_result =
            sensitivity_0 * variable_proxy - sensitivity_1 * variable_proxy;

        EXPECT_EQ(sensitivity_0 - sensitivity_1,
                  expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(0, expression_result.constant_value());
    }

    /// VariableProxy - Variable
    {
        auto expression_result =
            sensitivity_0 * variable_proxy - sensitivity_1 * variable;

        EXPECT_EQ(sensitivity_0,
                  expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(-sensitivity_1,
                  expression_result.sensitivities().at(&(variable)));
        EXPECT_EQ(0, expression_result.constant_value());
    }

    /// Variable - VariableProxy
    {
        auto expression_result =
            sensitivity_0 * variable - sensitivity_1 * variable_proxy;

        EXPECT_EQ(sensitivity_0,
                  expression_result.sensitivities().at(&variable));
        EXPECT_EQ(-sensitivity_1,
                  expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(0, expression_result.constant_value());
    }

    /// VariableProxy - Integer
    {
        auto expression_result = sensitivity_0 * variable_proxy - constant;

        EXPECT_EQ(sensitivity_0,
                  expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(-constant, expression_result.constant_value());
    }

    ///  Integer - VariableProxy
    {
        auto expression_result = constant - sensitivity_0 * variable_proxy;

        EXPECT_EQ(-sensitivity_0,
                  expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(constant, expression_result.constant_value());
    }

    /// VariableProxy - ExpressionProxy
    {
        auto expression_result =
            sensitivity_0 * variable_proxy - sensitivity_1 * expression_proxy;

        EXPECT_EQ(sensitivity_0 - sensitivity_1,
                  expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(0, expression_result.constant_value());
    }

    /// ExpressionProxy - VariableProxy
    {
        auto expression_result =
            sensitivity_0 * expression_proxy - sensitivity_1 * variable_proxy;

        EXPECT_EQ(sensitivity_0 - sensitivity_1,
                  expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(0, expression_result.constant_value());
    }

    /// VariableProxy - Expression
    {
        auto expression_result =
            sensitivity_0 * variable_proxy - sensitivity_1 * expression;

        EXPECT_EQ(sensitivity_0,
                  expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(-sensitivity_1,
                  expression_result.sensitivities().at(&variable));
        EXPECT_EQ(0, expression_result.constant_value());
    }

    /// Expression - VariableProxy
    {
        auto expression_result =
            sensitivity_0 * expression - sensitivity_1 * variable_proxy;

        EXPECT_EQ(sensitivity_0,
                  expression_result.sensitivities().at(&variable));
        EXPECT_EQ(-sensitivity_1,
                  expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(0, expression_result.constant_value());
    }
}

/*****************************************************************************/
TEST_F(TestExpressionBinary, expression_combination_subtract_product) {
    model::Model<int, double> model;

    auto& variable_proxy   = model.create_variable("x");
    auto& expression_proxy = model.create_expression("e");
    auto  variable = model_component::Variable<int, double>::create_instance();
    auto  expression =
        model_component::Expression<int, double>::create_instance();

    auto sensitivity_0 = random_integer();
    auto sensitivity_1 = random_integer();
    auto constant      = random_integer();

    expression_proxy = variable_proxy;
    expression       = variable;

    /// Expression - Expression
    {
        auto expression_result =
            sensitivity_0 * expression - sensitivity_1 * expression;

        EXPECT_EQ(sensitivity_0 - sensitivity_1,
                  expression_result.sensitivities().at(&variable));
        EXPECT_EQ(0, expression_result.constant_value());
    }

    /// Expression - Variable
    {
        auto expression_result =
            sensitivity_0 * expression - sensitivity_1 * variable;

        EXPECT_EQ(sensitivity_0 - sensitivity_1,
                  expression_result.sensitivities().at(&variable));
        EXPECT_EQ(0, expression_result.constant_value());
    }

    /// Variable - Expression
    {
        auto expression_result =
            sensitivity_0 * variable - sensitivity_1 * expression;

        EXPECT_EQ(sensitivity_0 - sensitivity_1,
                  expression_result.sensitivities().at(&variable));
        EXPECT_EQ(0, expression_result.constant_value());
    }

    /// Expression - Integer
    {
        auto expression_result = sensitivity_0 * expression - constant;

        EXPECT_EQ(sensitivity_0,
                  expression_result.sensitivities().at(&variable));
        EXPECT_EQ(-constant, expression_result.constant_value());
    }

    ///  Integer - Expression
    {
        auto expression_result = constant - sensitivity_0 * expression;

        EXPECT_EQ(-sensitivity_0,
                  expression_result.sensitivities().at(&variable));
        EXPECT_EQ(constant, expression_result.constant_value());
    }

    /// Expression - VariableProxy
    {
        auto expression_result =
            sensitivity_0 * expression - sensitivity_1 * variable_proxy;

        EXPECT_EQ(sensitivity_0,
                  expression_result.sensitivities().at(&variable));
        EXPECT_EQ(-sensitivity_1,
                  expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(0, expression_result.constant_value());
    }

    /// VariableProxy - Expression
    {
        auto expression_result =
            sensitivity_0 * variable_proxy - sensitivity_1 * expression;

        EXPECT_EQ(sensitivity_0,
                  expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(-sensitivity_1,
                  expression_result.sensitivities().at(&variable));
        EXPECT_EQ(0, expression_result.constant_value());
    }

    /// Expression - ExpressionProxy
    {
        auto expression_result =
            sensitivity_0 * expression - sensitivity_1 * expression_proxy;

        EXPECT_EQ(sensitivity_0,
                  expression_result.sensitivities().at(&variable));
        EXPECT_EQ(-sensitivity_1,
                  expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(0, expression_result.constant_value());
    }

    /// ExpressionProxy - Expression
    {
        auto expression_result =
            sensitivity_0 * expression_proxy - sensitivity_1 * expression;

        EXPECT_EQ(sensitivity_0,
                  expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(-sensitivity_1,
                  expression_result.sensitivities().at(&variable));
        EXPECT_EQ(0, expression_result.constant_value());
    }
}

/*****************************************************************************/
TEST_F(TestExpressionBinary, expression_proxy_combination_subtract_product) {
    model::Model<int, double> model;

    auto& variable_proxy   = model.create_variable("x");
    auto& expression_proxy = model.create_expression("e");
    auto  variable = model_component::Variable<int, double>::create_instance();
    auto  expression =
        model_component::Expression<int, double>::create_instance();

    auto sensitivity_0 = random_integer();
    auto sensitivity_1 = random_integer();
    auto constant      = random_integer();

    expression_proxy = variable_proxy;
    expression       = variable;

    /// ExpressionProxy - ExpressionProxy
    {
        auto expression_result =
            sensitivity_0 * expression_proxy - sensitivity_1 * expression_proxy;

        EXPECT_EQ(sensitivity_0 - sensitivity_1,
                  expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(0, expression_result.constant_value());
    }

    /// ExpressionProxy - Variable
    {
        auto expression_result =
            sensitivity_0 * expression_proxy - sensitivity_1 * variable;

        EXPECT_EQ(sensitivity_0,
                  expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(-sensitivity_1,
                  expression_result.sensitivities().at(&(variable)));
        EXPECT_EQ(0, expression_result.constant_value());
    }

    /// Variable - ExpressionProxy
    {
        auto expression_result =
            sensitivity_0 * variable - sensitivity_1 * expression_proxy;

        EXPECT_EQ(sensitivity_0,
                  expression_result.sensitivities().at(&variable));
        EXPECT_EQ(-sensitivity_1,
                  expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(0, expression_result.constant_value());
    }

    /// ExpressionProxy - Integer
    {
        auto expression_result = sensitivity_0 * expression_proxy - constant;

        EXPECT_EQ(sensitivity_0,
                  expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(-constant, expression_result.constant_value());
    }

    ///  Integer - ExpressionProxy
    {
        auto expression_result = constant - sensitivity_0 * expression_proxy;

        EXPECT_EQ(-sensitivity_0,
                  expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(constant, expression_result.constant_value());
    }

    /// ExpressionProxy - VariableProxy
    {
        auto expression_result =
            sensitivity_0 * expression_proxy - sensitivity_1 * variable_proxy;

        EXPECT_EQ(sensitivity_0 - sensitivity_1,
                  expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(0, expression_result.constant_value());
    }

    /// VariableProxy - ExpressionProxy
    {
        auto expression_result =
            sensitivity_0 * variable_proxy - sensitivity_1 * expression_proxy;

        EXPECT_EQ(sensitivity_0 - sensitivity_1,
                  expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(0, expression_result.constant_value());
    }

    /// ExpressionProxy - Expression
    {
        auto expression_result =
            sensitivity_0 * expression_proxy - sensitivity_1 * expression;

        EXPECT_EQ(sensitivity_0,
                  expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(-sensitivity_1,
                  expression_result.sensitivities().at(&variable));
        EXPECT_EQ(0, expression_result.constant_value());
    }

    /// Expression - ExpressionProxy
    {
        auto expression_result =
            sensitivity_0 * expression - sensitivity_1 * expression_proxy;

        EXPECT_EQ(sensitivity_0,
                  expression_result.sensitivities().at(&variable));
        EXPECT_EQ(-sensitivity_1,
                  expression_result.sensitivities().at(&variable_proxy[0]));
        EXPECT_EQ(0, expression_result.constant_value());
    }
}

/*****************************************************************************/
TEST_F(TestExpressionBinary, operator_divide) {
    model::Model<int, double> model;

    auto& variable_proxy   = model.create_variable("x");
    auto& expression_proxy = model.create_expression("e");
    auto  variable = model_component::Variable<int, double>::create_instance();
    auto  expression =
        model_component::Expression<int, double>::create_instance();

    auto denominator = static_cast<double>(random_positive_integer());

    expression_proxy = variable_proxy;
    expression       = variable;

    /// Variable
    {
        auto expression_result = variable / denominator;

        EXPECT_DOUBLE_EQ(1.0 / denominator,
                         expression_result.sensitivities().at(&variable));
    }

    /// VariableProxy
    {
        auto expression_result = variable_proxy / denominator;

        EXPECT_DOUBLE_EQ(
            1.0 / denominator,
            expression_result.sensitivities().at(&variable_proxy[0]));
    }

    /// Expression
    {
        auto expression_result = expression / denominator;

        EXPECT_DOUBLE_EQ(1.0 / denominator,
                         expression_result.sensitivities().at(&variable));
    }

    /// ExpressionProxy
    {
        auto expression_result = expression_proxy / denominator;

        EXPECT_DOUBLE_EQ(
            1.0 / denominator,
            expression_result.sensitivities().at(&variable_proxy[0]));
    }
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/