/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <gtest/gtest.h>
#include <random>
#include <cppmh.h>

/*****************************************************************************/
namespace {
class TestModel : public ::testing::Test {
   protected:
    cppmh::utility::IntegerUniformRandom m_random_integer;
    cppmh::utility::IntegerUniformRandom m_random_positive_integer;

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
TEST_F(TestModel, initialize) {
    cppmh::model::Model<int, double> model;

    /// These declarations are for googletest constraint.
    /// http://opencv.jp/googletestdocs/FAQ.html#faq-the-compiler-complains-about-undefined-reference
    auto max_number_of_variable_proxies =
        cppmh::model::ModelConstant::MAX_NUMBER_OF_VARIABLE_PROXIES;
    auto max_number_of_expression_proxies =
        cppmh::model::ModelConstant::MAX_NUMBER_OF_EXPRESSION_PROXIES;
    auto max_number_of_constraint_proxies =
        cppmh::model::ModelConstant::MAX_NUMBER_OF_CONSTRAINT_PROXIES;

    EXPECT_EQ(max_number_of_variable_proxies,
              static_cast<int>(model.variable_proxies().capacity()));
    EXPECT_EQ(max_number_of_expression_proxies,
              static_cast<int>(model.expression_proxies().capacity()));
    EXPECT_EQ(max_number_of_constraint_proxies,
              static_cast<int>(model.constraint_proxies().capacity()));

    EXPECT_EQ(true, model.variable_names().empty());
    EXPECT_EQ(true, model.expression_names().empty());
    EXPECT_EQ(true, model.constraint_names().empty());

    EXPECT_EQ(true, model.is_minimization());
    EXPECT_EQ(1.0, model.sign());
    EXPECT_EQ(false, model.is_defined_objective());
}

/*****************************************************************************/
TEST_F(TestModel, create_variable_scalar_without_bound) {
    cppmh::model::Model<int, double> model;
    for (auto i = 0;
         i < cppmh::model::ModelConstant::MAX_NUMBER_OF_VARIABLE_PROXIES; i++) {
        auto  name           = "x" + std::to_string(i);
        auto& variable_proxy = model.create_variable(name);
        EXPECT_EQ(i + 1, static_cast<int>(model.variable_proxies().size()));
        EXPECT_EQ(i, variable_proxy.id());
        EXPECT_EQ(std::numeric_limits<int>::min() + 1,
                  variable_proxy.lower_bound());
        EXPECT_EQ(std::numeric_limits<int>::max() - 1,
                  variable_proxy.upper_bound());
        EXPECT_EQ(false, variable_proxy.is_defined_bounds());
        EXPECT_EQ(cppmh::model::VariableSense::Integer, variable_proxy.sense());
        EXPECT_EQ(&variable_proxy, &model.variable_proxies().back());
        EXPECT_EQ(name, model.variable_names().back());
    }
    ASSERT_THROW(model.create_variable("error"), std::logic_error);
}

/*****************************************************************************/
TEST_F(TestModel, create_variable_scalar_with_bound) {
    cppmh::model::Model<int, double> model;
    for (auto i = 0;
         i < cppmh::model::ModelConstant::MAX_NUMBER_OF_VARIABLE_PROXIES; i++) {
        auto  name           = "x" + std::to_string(i);
        auto& variable_proxy = model.create_variable(name, 0, 1);
        EXPECT_EQ(i + 1, static_cast<int>(model.variable_proxies().size()));
        EXPECT_EQ(i, variable_proxy.id());
        EXPECT_EQ(0, variable_proxy.lower_bound());
        EXPECT_EQ(1, variable_proxy.upper_bound());
        EXPECT_EQ(true, variable_proxy.is_defined_bounds());
        EXPECT_EQ(cppmh::model::VariableSense::Binary, variable_proxy.sense());
        EXPECT_EQ(&variable_proxy, &model.variable_proxies().back());
        EXPECT_EQ(name, model.variable_names().back());
    }
    ASSERT_THROW(model.create_variable("error", 0, 1), std::logic_error);
}

/*****************************************************************************/
TEST_F(TestModel, create_variable_one_dimensional_without_bound) {
    cppmh::model::Model<int, double> model;
    for (auto i = 0;
         i < cppmh::model::ModelConstant::MAX_NUMBER_OF_VARIABLE_PROXIES; i++) {
        auto  name           = "x" + std::to_string(i);
        auto& variable_proxy = model.create_variables(name, 2);
        EXPECT_EQ(i + 1, static_cast<int>(model.variable_proxies().size()));
        EXPECT_EQ(i, variable_proxy.id());
        EXPECT_EQ(std::numeric_limits<int>::min() + 1,
                  variable_proxy[0].lower_bound());
        EXPECT_EQ(std::numeric_limits<int>::max() - 1,
                  variable_proxy[0].upper_bound());
        EXPECT_EQ(false, variable_proxy[0].is_defined_bounds());
        EXPECT_EQ(cppmh::model::VariableSense::Integer,
                  variable_proxy[0].sense());
        EXPECT_EQ(&variable_proxy, &model.variable_proxies().back());
        EXPECT_EQ(name, model.variable_names().back());
    }
    ASSERT_THROW(model.create_variables("error", 2), std::logic_error);
}

/*****************************************************************************/
TEST_F(TestModel, create_variable_one_dimensional_with_bound) {
    cppmh::model::Model<int, double> model;
    for (auto i = 0;
         i < cppmh::model::ModelConstant::MAX_NUMBER_OF_VARIABLE_PROXIES; i++) {
        auto  name           = "x" + std::to_string(i);
        auto& variable_proxy = model.create_variables(name, 2, 0, 1);
        EXPECT_EQ(i + 1, static_cast<int>(model.variable_proxies().size()));
        EXPECT_EQ(i, variable_proxy.id());
        EXPECT_EQ(0, variable_proxy[0].lower_bound());
        EXPECT_EQ(1, variable_proxy[0].upper_bound());
        EXPECT_EQ(true, variable_proxy[0].is_defined_bounds());
        EXPECT_EQ(cppmh::model::VariableSense::Binary,
                  variable_proxy[0].sense());
        EXPECT_EQ(&variable_proxy, &model.variable_proxies().back());
        EXPECT_EQ(name, model.variable_names().back());
    }
    ASSERT_THROW(model.create_variables("error", 2, 0, 1), std::logic_error);
}

/*****************************************************************************/
TEST_F(TestModel, create_variable_two_dimensional_without_bound) {
    cppmh::model::Model<int, double> model;
    for (auto i = 0;
         i < cppmh::model::ModelConstant::MAX_NUMBER_OF_VARIABLE_PROXIES; i++) {
        auto  name           = "x" + std::to_string(i);
        auto& variable_proxy = model.create_variables(name, {2, 3});
        EXPECT_EQ(i + 1, static_cast<int>(model.variable_proxies().size()));
        EXPECT_EQ(i, variable_proxy.id());
        EXPECT_EQ(std::numeric_limits<int>::min() + 1,
                  variable_proxy[0].lower_bound());
        EXPECT_EQ(std::numeric_limits<int>::max() - 1,
                  variable_proxy[0].upper_bound());
        EXPECT_EQ(false, variable_proxy[0].is_defined_bounds());
        EXPECT_EQ(cppmh::model::VariableSense::Integer,
                  variable_proxy[0].sense());
        EXPECT_EQ(&variable_proxy, &model.variable_proxies().back());
        EXPECT_EQ(name, model.variable_names().back());
    }
    ASSERT_THROW(model.create_variables("error", {2, 3}), std::logic_error);
}

/*****************************************************************************/
TEST_F(TestModel, create_variable_two_dimensional_with_bound) {
    cppmh::model::Model<int, double> model;
    for (auto i = 0;
         i < cppmh::model::ModelConstant::MAX_NUMBER_OF_VARIABLE_PROXIES; i++) {
        auto  name           = "x" + std::to_string(i);
        auto& variable_proxy = model.create_variables(name, {2, 3}, 0, 1);
        EXPECT_EQ(i + 1, static_cast<int>(model.variable_proxies().size()));
        EXPECT_EQ(i, variable_proxy.id());
        EXPECT_EQ(0, variable_proxy[0].lower_bound());
        EXPECT_EQ(1, variable_proxy[0].upper_bound());
        EXPECT_EQ(true, variable_proxy[0].is_defined_bounds());
        EXPECT_EQ(cppmh::model::VariableSense::Binary,
                  variable_proxy[0].sense());
        EXPECT_EQ(&variable_proxy, &model.variable_proxies().back());
        EXPECT_EQ(name, model.variable_names().back());
    }
    ASSERT_THROW(model.create_variables("error", {2, 3}, 0, 1),
                 std::logic_error);
}

/*****************************************************************************/
TEST_F(TestModel, create_expression_scalar) {
    cppmh::model::Model<int, double> model;
    for (auto i = 0;
         i < cppmh::model::ModelConstant::MAX_NUMBER_OF_EXPRESSION_PROXIES;
         i++) {
        auto  name             = "e" + std::to_string(i);
        auto& expression_proxy = model.create_expression(name);
        EXPECT_EQ(i + 1, static_cast<int>(model.expression_proxies().size()));
        EXPECT_EQ(i, expression_proxy.id());
        EXPECT_EQ(&expression_proxy, &model.expression_proxies().back());
        EXPECT_EQ(name, model.expression_names().back());
    }
    ASSERT_THROW(model.create_expression("error"), std::logic_error);
}

/*****************************************************************************/
TEST_F(TestModel, create_expression_one_dimensional) {
    cppmh::model::Model<int, double> model;
    for (auto i = 0;
         i < cppmh::model::ModelConstant::MAX_NUMBER_OF_EXPRESSION_PROXIES;
         i++) {
        auto  name             = "e" + std::to_string(i);
        auto& expression_proxy = model.create_expressions(name, 2);
        EXPECT_EQ(i + 1, static_cast<int>(model.expression_proxies().size()));
        EXPECT_EQ(i, expression_proxy.id());
        EXPECT_EQ(&expression_proxy, &model.expression_proxies().back());
        EXPECT_EQ(name, model.expression_names().back());
    }
    ASSERT_THROW(model.create_expressions("error", 2), std::logic_error);
}

/*****************************************************************************/
TEST_F(TestModel, create_expression_two_dimensional) {
    cppmh::model::Model<int, double> model;
    for (auto i = 0;
         i < cppmh::model::ModelConstant::MAX_NUMBER_OF_EXPRESSION_PROXIES;
         i++) {
        auto  name             = "e" + std::to_string(i);
        auto& expression_proxy = model.create_expressions(name, {2, 3});
        EXPECT_EQ(i + 1, static_cast<int>(model.expression_proxies().size()));
        EXPECT_EQ(i, expression_proxy.id());
        EXPECT_EQ(&expression_proxy, &model.expression_proxies().back());
        EXPECT_EQ(name, model.expression_names().back());
    }
    ASSERT_THROW(model.create_expressions("error", {2, 3}), std::logic_error);
}

/*****************************************************************************/
TEST_F(TestModel, create_expression_arg_expression_like) {
    cppmh::model::Model<int, double> model;

    for (auto i = 0;
         i < cppmh::model::ModelConstant::MAX_NUMBER_OF_EXPRESSION_PROXIES;
         i++) {
        auto name     = "e" + std::to_string(i);
        auto variable = cppmh::model::Variable<int, double>::create_instance();

        auto& expression_proxy = model.create_expression(name, variable);
        EXPECT_EQ(i + 1, static_cast<int>(model.expression_proxies().size()));
        EXPECT_EQ(i, expression_proxy.id());
        EXPECT_EQ(&expression_proxy, &model.expression_proxies().back());
        EXPECT_EQ(name, model.expression_names().back());
    }

    auto variable = cppmh::model::Variable<int, double>::create_instance();
    ASSERT_THROW(model.create_expression("error", variable), std::logic_error);
}

/*****************************************************************************/
TEST_F(TestModel, create_expression_arg_expression) {
    cppmh::model::Model<int, double> model;

    for (auto i = 0;
         i < cppmh::model::ModelConstant::MAX_NUMBER_OF_EXPRESSION_PROXIES;
         i++) {
        auto name = "e" + std::to_string(i);
        auto expression =
            cppmh::model::Expression<int, double>::create_instance();

        auto& expression_proxy = model.create_expression(name, expression);
        EXPECT_EQ(i + 1, static_cast<int>(model.expression_proxies().size()));
        EXPECT_EQ(i, expression_proxy.id());
        EXPECT_EQ(&expression_proxy, &model.expression_proxies().back());
        EXPECT_EQ(name, model.expression_names().back());
    }

    auto expression = cppmh::model::Expression<int, double>::create_instance();
    ASSERT_THROW(model.create_expression("error", expression),
                 std::logic_error);
}

/*****************************************************************************/
TEST_F(TestModel, create_constraint_scalar) {
    cppmh::model::Model<int, double> model;
    for (auto i = 0;
         i < cppmh::model::ModelConstant::MAX_NUMBER_OF_CONSTRAINT_PROXIES;
         i++) {
        auto  name             = "c" + std::to_string(i);
        auto& constraint_proxy = model.create_constraint(name);
        EXPECT_EQ(i + 1, static_cast<int>(model.constraint_proxies().size()));
        EXPECT_EQ(i, constraint_proxy.id());
        EXPECT_EQ(&constraint_proxy, &model.constraint_proxies().back());
        EXPECT_EQ(name, model.constraint_names().back());
    }
    ASSERT_THROW(model.create_constraint("error"), std::logic_error);
}

/*****************************************************************************/
TEST_F(TestModel, create_constraint_one_dimensional) {
    cppmh::model::Model<int, double> model;
    for (auto i = 0;
         i < cppmh::model::ModelConstant::MAX_NUMBER_OF_CONSTRAINT_PROXIES;
         i++) {
        auto  name             = "c" + std::to_string(i);
        auto& constraint_proxy = model.create_constraints(name, 2);
        EXPECT_EQ(i + 1, static_cast<int>(model.constraint_proxies().size()));
        EXPECT_EQ(i, constraint_proxy.id());
        EXPECT_EQ(&constraint_proxy, &model.constraint_proxies().back());
        EXPECT_EQ(name, model.constraint_names().back());
    }
    ASSERT_THROW(model.create_constraints("error", 2), std::logic_error);
}

/*****************************************************************************/
TEST_F(TestModel, create_constraint_two_dimensional) {
    cppmh::model::Model<int, double> model;
    for (auto i = 0;
         i < cppmh::model::ModelConstant::MAX_NUMBER_OF_CONSTRAINT_PROXIES;
         i++) {
        auto  name             = "c" + std::to_string(i);
        auto& constraint_proxy = model.create_constraints(name, {2, 3});
        EXPECT_EQ(i + 1, static_cast<int>(model.constraint_proxies().size()));
        EXPECT_EQ(i, constraint_proxy.id());
        EXPECT_EQ(&constraint_proxy, &model.constraint_proxies().back());
        EXPECT_EQ(name, model.constraint_names().back());
    }
    ASSERT_THROW(model.create_constraints("error", {2, 3}), std::logic_error);
}

/*****************************************************************************/
TEST_F(TestModel, create_constraint_arg_expression) {
    cppmh::model::Model<int, double> model;

    for (auto i = 0;
         i < cppmh::model::ModelConstant::MAX_NUMBER_OF_CONSTRAINT_PROXIES;
         i++) {
        auto name = "c" + std::to_string(i);
        auto expression =
            cppmh::model::Expression<int, double>::create_instance();
        auto constraint = expression <= 1;

        auto& constraint_proxy = model.create_constraint(name, constraint);
        EXPECT_EQ(i + 1, static_cast<int>(model.constraint_proxies().size()));
        EXPECT_EQ(i, constraint_proxy.id());
        EXPECT_EQ(&constraint_proxy, &model.constraint_proxies().back());
        EXPECT_EQ(name, model.constraint_names().back());
    }

    auto expression = cppmh::model::Expression<int, double>::create_instance();
    auto constraint = expression <= 1;
    ASSERT_THROW(model.create_constraint("error", constraint),
                 std::logic_error);
}

/*****************************************************************************/
TEST_F(TestModel, minimize_arg_function) {
    cppmh::model::Model<int, double> model;
    auto& variable_proxy = model.create_variables("x", 10, -1, 1);
    auto& expression_proxy =
        model.create_expression("e", variable_proxy.sum() + 1);
    auto f =
        [&expression_proxy](const cppmh::model::Move<int, double>& a_MOVE) {
            return expression_proxy.evaluate(a_MOVE);
        };
    model.minimize(f);

    EXPECT_EQ(true, model.is_defined_objective());
    EXPECT_EQ(true, model.is_minimization());
    EXPECT_EQ(1.0, model.sign());
    EXPECT_EQ(true, model.objective().expression().sensitivities().empty());
    EXPECT_EQ(0, model.objective().expression().constant_value());
    EXPECT_EQ(false, model.objective().is_linear());

    for (auto&& variable : variable_proxy.flat_indexed_variables()) {
        variable = 1;
    }
    model.update();
    EXPECT_EQ(10 + 1, model.objective().value());
}

/*****************************************************************************/
TEST_F(TestModel, minimize_arg_expression_like) {
    cppmh::model::Model<int, double> model;
    auto& variable_proxy = model.create_variables("x", 10, -1, 1);
    auto& expression_proxy =
        model.create_expression("e", variable_proxy.sum() + 1);
    [[maybe_unused]] auto f =
        [&expression_proxy](const cppmh::model::Move<int, double>& a_MOVE) {
            return expression_proxy.evaluate(a_MOVE);
        };
    model.minimize(expression_proxy);

    EXPECT_EQ(true, model.is_defined_objective());
    EXPECT_EQ(true, model.is_minimization());
    EXPECT_EQ(1.0, model.sign());
    EXPECT_EQ(10, static_cast<int>(
                      model.objective().expression().sensitivities().size()));
    EXPECT_EQ(1, model.objective().expression().constant_value());
    EXPECT_EQ(true, model.objective().is_linear());

    for (auto&& variable : variable_proxy.flat_indexed_variables()) {
        variable = 1;
    }
    model.update();
    EXPECT_EQ(10 + 1, model.objective().value());
}

/*****************************************************************************/
TEST_F(TestModel, minimize_arg_expression) {
    cppmh::model::Model<int, double> model;
    auto& variable_proxy = model.create_variables("x", 10, -1, 1);
    auto& expression_proxy =
        model.create_expression("e", variable_proxy.sum() + 1);
    [[maybe_unused]] auto f =
        [&expression_proxy](const cppmh::model::Move<int, double>& a_MOVE) {
            return expression_proxy.evaluate(a_MOVE);
        };
    model.minimize(expression_proxy[0]);

    EXPECT_EQ(true, model.is_defined_objective());
    EXPECT_EQ(true, model.is_minimization());
    EXPECT_EQ(1.0, model.sign());
    EXPECT_EQ(10, static_cast<int>(
                      model.objective().expression().sensitivities().size()));
    EXPECT_EQ(1, model.objective().expression().constant_value());
    EXPECT_EQ(true, model.objective().is_linear());

    for (auto&& variable : variable_proxy.flat_indexed_variables()) {
        variable = 1;
    }
    model.update();
    EXPECT_EQ(10 + 1, model.objective().value());
}

/*****************************************************************************/
TEST_F(TestModel, maximize_arg_function) {
    cppmh::model::Model<int, double> model;
    auto& variable_proxy = model.create_variables("x", 10, -1, 1);
    auto& expression_proxy =
        model.create_expression("e", variable_proxy.sum() + 1);
    auto f =
        [&expression_proxy](const cppmh::model::Move<int, double>& a_MOVE) {
            return expression_proxy.evaluate(a_MOVE);
        };
    model.maximize(f);

    EXPECT_EQ(true, model.is_defined_objective());
    EXPECT_EQ(false, model.is_minimization());
    EXPECT_EQ(-1.0, model.sign());
    EXPECT_EQ(true, model.objective().expression().sensitivities().empty());
    EXPECT_EQ(0, model.objective().expression().constant_value());
    EXPECT_EQ(false, model.objective().is_linear());

    for (auto&& variable : variable_proxy.flat_indexed_variables()) {
        variable = 1;
    }
    model.update();
    EXPECT_EQ(10 + 1, model.objective().value());
}

/*****************************************************************************/
TEST_F(TestModel, maximize_arg_expression_like) {
    cppmh::model::Model<int, double> model;
    auto& variable_proxy = model.create_variables("x", 10, -1, 1);
    auto& expression_proxy =
        model.create_expression("e", variable_proxy.sum() + 1);
    [[maybe_unused]] auto f =
        [&expression_proxy](const cppmh::model::Move<int, double>& a_MOVE) {
            return expression_proxy.evaluate(a_MOVE);
        };
    model.maximize(expression_proxy);

    EXPECT_EQ(true, model.is_defined_objective());
    EXPECT_EQ(false, model.is_minimization());
    EXPECT_EQ(-1.0, model.sign());
    EXPECT_EQ(10, static_cast<int>(
                      model.objective().expression().sensitivities().size()));
    EXPECT_EQ(1, model.objective().expression().constant_value());
    EXPECT_EQ(true, model.objective().is_linear());

    for (auto&& variable : variable_proxy.flat_indexed_variables()) {
        variable = 1;
    }
    model.update();
    EXPECT_EQ(10 + 1, model.objective().value());
}

/*****************************************************************************/
TEST_F(TestModel, maximize_arg_expression) {
    cppmh::model::Model<int, double> model;
    auto& variable_proxy = model.create_variables("x", 10, -1, 1);
    auto& expression_proxy =
        model.create_expression("e", variable_proxy.sum() + 1);
    [[maybe_unused]] auto f =
        [&expression_proxy](const cppmh::model::Move<int, double>& a_MOVE) {
            return expression_proxy.evaluate(a_MOVE);
        };
    model.maximize(expression_proxy[0]);

    EXPECT_EQ(true, model.is_defined_objective());
    EXPECT_EQ(false, model.is_minimization());
    EXPECT_EQ(-1.0, model.sign());
    EXPECT_EQ(10, static_cast<int>(
                      model.objective().expression().sensitivities().size()));
    EXPECT_EQ(1, model.objective().expression().constant_value());
    EXPECT_EQ(true, model.objective().is_linear());

    for (auto&& variable : variable_proxy.flat_indexed_variables()) {
        variable = 1;
    }
    model.update();
    EXPECT_EQ(10 + 1, model.objective().value());
}

/*****************************************************************************/
TEST_F(TestModel, is_defined_objective) {
    /// This method is tested in minimize_arg_function() and so on.
}

/*****************************************************************************/
TEST_F(TestModel, is_minimization) {
    /// This method is tested in minimize_arg_function() and so on.
}

/*****************************************************************************/
TEST_F(TestModel, sign) {
    /// This method is tested in minimize_arg_function() and so on.
}

/*****************************************************************************/
TEST_F(TestModel, number_of_variables) {
    cppmh::model::Model<int, double> model;

    [[maybe_unused]] auto& x0 = model.create_variable("x0");
    [[maybe_unused]] auto& x1 = model.create_variables("x1", 10);
    [[maybe_unused]] auto& x2 = model.create_variables("x2", {20, 30});
    EXPECT_EQ(1 + 10 + 20 * 30, model.number_of_variables());
}

/*****************************************************************************/
TEST_F(TestModel, neighborhood) {
    /// This method is tested in test_neighborhood.h
}

/*****************************************************************************/
TEST_F(TestModel, setup_default_neighborhood) {
    /// This method is tested in test_neighborhood.h
}

/*****************************************************************************/
TEST_F(TestModel, setup_has_fixed_variables) {
    /// This method is tested in test_neighborhood.h
}

/*****************************************************************************/
TEST_F(TestModel, reset_variable_sense) {
    cppmh::model::Model<int, double> model;

    auto& variable_proxy = model.create_variables("x", 10, 0, 1);
    model.create_constraint("c", variable_proxy.selection());
    model.setup_default_neighborhood(false);
    for (const auto& variable : variable_proxy.flat_indexed_variables()) {
        EXPECT_EQ(cppmh::model::VariableSense::Selection, variable.sense());
    }

    model.reset_variable_sense();
    for (const auto& variable : variable_proxy.flat_indexed_variables()) {
        EXPECT_EQ(cppmh::model::VariableSense::Binary, variable.sense());
    }
}

/*****************************************************************************/
TEST_F(TestModel, verify_problem) {
    /// No decision variables.
    {
        cppmh::model::Model<int, double> model;
        ASSERT_THROW(model.verify_problem(), std::logic_error);
    }

    /// No constraint functions.
    {
        cppmh::model::Model<int, double> model;
        auto& variable_proxy = model.create_variable("x");
        model.minimize(variable_proxy);
        model.verify_problem();
    }

    /// No objective function.
    {
        cppmh::model::Model<int, double> model;
        auto& variable_proxy = model.create_variable("x");
        model.create_constraint("c", variable_proxy == 1);
        model.verify_problem();
    }

    /// No constraint functions and no objective function
    {
        cppmh::model::Model<int, double> model;
        [[maybe_unused]] auto& variable_proxy = model.create_variable("x");
        ASSERT_THROW(model.verify_problem(), std::logic_error);
    }
}

/*****************************************************************************/
TEST_F(TestModel, verify_bounds) {
    {
        cppmh::model::Model<int, double> model;
        [[maybe_unused]] auto&           variable_proxy =
            model.create_variable("x", 0, 1);
    }

    {
        cppmh::model::Model<int, double> model;
        [[maybe_unused]] auto& variable_proxy = model.create_variable("x");
        ASSERT_THROW(model.verify_bounds(), std::logic_error);
    }
}

/*****************************************************************************/
TEST_F(TestModel, verify_and_correct_selection_variables_initial_values) {
    /// There is a fixed variable with an invalid initial value.
    /// correction: true
    {
        cppmh::model::Model<int, double> model;
        auto& variable_proxy = model.create_variables("x", 10, 0, 1);
        model.create_constraint("c", variable_proxy.selection());
        variable_proxy[0].fix_by(2);

        model.setup_default_neighborhood(false);
        model.setup_has_fixed_variables();

        ASSERT_THROW(
            model.verify_and_correct_selection_variables_initial_values(true,
                                                                        false),
            std::logic_error);
    }

    /// There is a fixed variable with an invalid initial value.
    /// correction: false
    {
        cppmh::model::Model<int, double> model;
        auto& variable_proxy = model.create_variables("x", 10, 0, 1);
        model.create_constraint("c", variable_proxy.selection());
        variable_proxy[0].fix_by(2);

        model.setup_default_neighborhood(false);
        model.setup_has_fixed_variables();

        ASSERT_THROW(
            model.verify_and_correct_selection_variables_initial_values(false,
                                                                        false),
            std::logic_error);
    }

    /// There is one fixed selected variable.
    /// correction: true
    {
        cppmh::model::Model<int, double> model;
        auto& variable_proxy = model.create_variables("x", 10, 0, 1);
        model.create_constraint("c", variable_proxy.selection());
        variable_proxy[0].fix_by(1);

        model.setup_default_neighborhood(false);
        model.setup_has_fixed_variables();

        model.verify_and_correct_selection_variables_initial_values(true,
                                                                    false);
        EXPECT_EQ(1, variable_proxy[0].value());
    }

    /// There is one fixed selected variable.
    /// correction: false
    {
        cppmh::model::Model<int, double> model;
        auto& variable_proxy = model.create_variables("x", 10, 0, 1);
        model.create_constraint("c", variable_proxy.selection());
        variable_proxy[0].fix_by(1);

        model.setup_default_neighborhood(false);
        model.setup_has_fixed_variables();

        model.verify_and_correct_selection_variables_initial_values(false,
                                                                    false);
        EXPECT_EQ(1, variable_proxy[0].value());
    }

    /// There are two fixed selected variables.
    /// correction: true
    {
        cppmh::model::Model<int, double> model;
        auto& variable_proxy = model.create_variables("x", 10, 0, 1);
        model.create_constraint("c", variable_proxy.selection());
        variable_proxy[0].fix_by(1);
        variable_proxy[1].fix_by(1);

        model.setup_default_neighborhood(false);
        model.setup_has_fixed_variables();

        ASSERT_THROW(
            model.verify_and_correct_selection_variables_initial_values(true,
                                                                        false),
            std::logic_error);
    }

    /// There are two fixed selected variables.
    /// correction: true
    {
        cppmh::model::Model<int, double> model;
        auto& variable_proxy = model.create_variables("x", 10, 0, 1);
        model.create_constraint("c", variable_proxy.selection());
        variable_proxy[0].fix_by(1);
        variable_proxy[1].fix_by(1);

        model.setup_default_neighborhood(false);
        model.setup_has_fixed_variables();

        ASSERT_THROW(
            model.verify_and_correct_selection_variables_initial_values(false,
                                                                        false),
            std::logic_error);
    }

    /// There are two variables with invalid initial values.
    /// correction: true
    {
        cppmh::model::Model<int, double> model;
        auto& variable_proxy = model.create_variables("x", 10, 0, 1);
        model.create_constraint("c", variable_proxy.selection());
        variable_proxy[0] = 2;
        variable_proxy[1] = 3;

        model.setup_default_neighborhood(false);
        model.setup_has_fixed_variables();

        model.verify_and_correct_selection_variables_initial_values(true,
                                                                    false);

        EXPECT_EQ(0, variable_proxy[0].value());
        EXPECT_EQ(0, variable_proxy[1].value());
    }

    /// There are two variables with invalid initial values.
    /// correction: false
    {
        cppmh::model::Model<int, double> model;
        auto& variable_proxy = model.create_variables("x", 10, 0, 1);
        model.create_constraint("c", variable_proxy.selection());
        variable_proxy[0] = 2;
        variable_proxy[1] = 3;

        model.setup_default_neighborhood(false);
        model.setup_has_fixed_variables();

        ASSERT_THROW(
            model.verify_and_correct_selection_variables_initial_values(false,
                                                                        false),
            std::logic_error);
    }

    /// There is no selected variable.
    /// correction: true
    {
        cppmh::model::Model<int, double> model;
        auto& variable_proxy = model.create_variables("x", 10, 0, 1);
        model.create_constraint("c", variable_proxy.selection());

        model.setup_default_neighborhood(false);
        model.setup_has_fixed_variables();

        model.verify_and_correct_selection_variables_initial_values(true,
                                                                    false);

        /// selected_variable_ptr is not always &variable_proxy[0].
        auto selected_variable_ptr =
            model.neighborhood().selections().front().variable_ptrs.front();
        EXPECT_EQ(1, selected_variable_ptr->value());
    }

    /// There is no selected variable.
    /// correction: false
    {
        cppmh::model::Model<int, double> model;
        auto& variable_proxy = model.create_variables("x", 10, 0, 1);
        model.create_constraint("c", variable_proxy.selection());

        model.setup_default_neighborhood(false);
        model.setup_has_fixed_variables();

        ASSERT_THROW(
            model.verify_and_correct_selection_variables_initial_values(false,
                                                                        false),
            std::logic_error);
    }

    /// There is one selected variable.
    /// correction: true
    {
        cppmh::model::Model<int, double> model;
        auto& variable_proxy = model.create_variables("x", 10, 0, 1);
        model.create_constraint("c", variable_proxy.selection());
        variable_proxy[0] = 1;

        model.setup_default_neighborhood(false);
        model.setup_has_fixed_variables();

        model.verify_and_correct_selection_variables_initial_values(true,
                                                                    false);

        EXPECT_EQ(1, variable_proxy[0].value());
    }

    /// There is one selected variable.
    /// correction: false
    {
        cppmh::model::Model<int, double> model;
        auto& variable_proxy = model.create_variables("x", 10, 0, 1);
        model.create_constraint("c", variable_proxy.selection());
        variable_proxy[0] = 1;

        model.setup_default_neighborhood(false);
        model.setup_has_fixed_variables();

        model.verify_and_correct_selection_variables_initial_values(false,
                                                                    false);

        EXPECT_EQ(1, variable_proxy[0].value());
    }

    /// There are two unfixed selected variable.
    /// correction: true
    {
        cppmh::model::Model<int, double> model;
        auto& variable_proxy = model.create_variables("x", 10, 0, 1);
        model.create_constraint("c", variable_proxy.selection());
        variable_proxy[0] = 1;
        variable_proxy[1] = 1;

        model.setup_default_neighborhood(false);
        model.setup_has_fixed_variables();

        model.verify_and_correct_selection_variables_initial_values(true,
                                                                    false);

        /// selected_variable is not always variable_proxy[0].
        EXPECT_EQ(1, variable_proxy[0].value() + variable_proxy[1].value());
    }

    /// There are two unfixed selected variable.
    /// correction: false
    {
        cppmh::model::Model<int, double> model;
        auto& variable_proxy = model.create_variables("x", 10, 0, 1);
        model.create_constraint("c", variable_proxy.selection());
        variable_proxy[0] = 1;
        variable_proxy[1] = 1;

        model.setup_default_neighborhood(false);
        model.setup_has_fixed_variables();

        ASSERT_THROW(
            model.verify_and_correct_selection_variables_initial_values(false,
                                                                        false),
            std::logic_error);
    }

    /// There are 1 fixed and 1 unfixed selected variable.
    /// correction: true
    {
        cppmh::model::Model<int, double> model;
        auto& variable_proxy = model.create_variables("x", 10, 0, 1);
        model.create_constraint("c", variable_proxy.selection());
        variable_proxy[0] = 1;
        variable_proxy[1].fix_by(1);

        model.setup_default_neighborhood(false);
        model.setup_has_fixed_variables();

        model.verify_and_correct_selection_variables_initial_values(true,
                                                                    false);
        EXPECT_EQ(0, variable_proxy[0].value());
        EXPECT_EQ(1, variable_proxy[1].value());
    }

    /// There are 1 fixed and 1 unfixed selected variable.
    /// correction: false
    {
        cppmh::model::Model<int, double> model;
        auto& variable_proxy = model.create_variables("x", 10, 0, 1);
        model.create_constraint("c", variable_proxy.selection());
        variable_proxy[0] = 1;
        variable_proxy[1].fix_by(1);

        model.setup_default_neighborhood(false);
        model.setup_has_fixed_variables();

        ASSERT_THROW(
            model.verify_and_correct_selection_variables_initial_values(false,
                                                                        false),
            std::logic_error);
    }
}

/*****************************************************************************/
TEST_F(TestModel, verify_and_correct_binary_variables_initial_values) {
    /// There is a fixed variable with an invalid initial value.
    /// correction: true
    {
        cppmh::model::Model<int, double> model;
        auto& variable_proxy = model.create_variables("x", 10, 0, 1);
        variable_proxy[0].fix_by(2);

        model.setup_default_neighborhood(false);
        model.setup_has_fixed_variables();

        ASSERT_THROW(model.verify_and_correct_binary_variables_initial_values(
                         true, false),
                     std::logic_error);
    }

    /// There is a fixed variable with an invalid initial value.
    /// correction: false
    {
        cppmh::model::Model<int, double> model;
        auto& variable_proxy = model.create_variables("x", 10, 0, 1);
        variable_proxy[0].fix_by(-1);

        model.setup_default_neighborhood(false);
        model.setup_has_fixed_variables();

        ASSERT_THROW(model.verify_and_correct_binary_variables_initial_values(
                         false, false),
                     std::logic_error);
    }

    /// There is a variable with an invalid initial value.
    /// correction: true
    {
        cppmh::model::Model<int, double> model;
        auto& variable_proxy = model.create_variables("x", 10, 0, 1);
        variable_proxy[0]    = 2;
        variable_proxy[1]    = -1;

        model.setup_default_neighborhood(false);
        model.setup_has_fixed_variables();

        model.verify_and_correct_binary_variables_initial_values(true, false);
        EXPECT_EQ(1, variable_proxy[0].value());
        EXPECT_EQ(0, variable_proxy[1].value());
    }

    /// There is a variable with an invalid initial value.
    /// correction: false
    {
        cppmh::model::Model<int, double> model;
        auto& variable_proxy = model.create_variables("x", 10, 0, 1);
        variable_proxy[0]    = 2;
        variable_proxy[1]    = -1;

        model.setup_default_neighborhood(false);
        model.setup_has_fixed_variables();

        ASSERT_THROW(model.verify_and_correct_binary_variables_initial_values(
                         false, false),
                     std::logic_error);
    }
}

/*****************************************************************************/
TEST_F(TestModel, verify_and_correct_integer_variables_initial_values) {
    /// There is a fixed variable with an invalid initial value.
    /// correction: true
    {
        cppmh::model::Model<int, double> model;
        auto& variable_proxy = model.create_variables("x", 10, -10, 10);
        variable_proxy[0].fix_by(11);

        model.setup_default_neighborhood(false);
        model.setup_has_fixed_variables();

        ASSERT_THROW(model.verify_and_correct_integer_variables_initial_values(
                         true, false),
                     std::logic_error);
    }

    /// There is a fixed variable with an invalid initial value.
    /// correction: false
    {
        cppmh::model::Model<int, double> model;
        auto& variable_proxy = model.create_variables("x", 10, -10, 10);
        variable_proxy[0].fix_by(-11);

        model.setup_default_neighborhood(false);
        model.setup_has_fixed_variables();

        ASSERT_THROW(model.verify_and_correct_integer_variables_initial_values(
                         false, false),
                     std::logic_error);
    }

    /// There is a variable with an invalid initial value.
    /// correction: true
    {
        cppmh::model::Model<int, double> model;
        auto& variable_proxy = model.create_variables("x", 10, -10, 10);
        variable_proxy[0]    = 11;
        variable_proxy[1]    = -11;

        model.setup_default_neighborhood(false);
        model.setup_has_fixed_variables();

        model.verify_and_correct_integer_variables_initial_values(true, false);
        EXPECT_EQ(10, variable_proxy[0].value());
        EXPECT_EQ(-10, variable_proxy[1].value());
    }

    /// There is a variable with an invalid initial value.
    /// correction: false
    {
        cppmh::model::Model<int, double> model;
        auto& variable_proxy = model.create_variables("x", 10, -10, 10);
        variable_proxy[0]    = 11;
        variable_proxy[1]    = -11;

        model.setup_default_neighborhood(false);
        model.setup_has_fixed_variables();

        ASSERT_THROW(model.verify_and_correct_integer_variables_initial_values(
                         false, false),
                     std::logic_error);
    }
}

/*****************************************************************************/
TEST_F(TestModel, set_callback) {
    cppmh::model::Model<int, double> model;

    int  v = 1;
    auto f = [&v](void) { v++; };
    model.set_callback(f);
    model.callback();
    EXPECT_EQ(2, v);
}

/*****************************************************************************/
TEST_F(TestModel, callback) {
    /// This method is tested in set_callback.
}

/*****************************************************************************/
TEST_F(TestModel, import_variable_values) {
    cppmh::model::Model<int, double> model;

    auto& x0 = model.create_variable("x0");
    auto& x1 = model.create_variables("x1", 10);
    auto& x2 = model.create_variables("x2", {10, 10});

    cppmh::model::ValueProxy<int> x0_value(x0.id());
    cppmh::model::ValueProxy<int> x1_value(x1.id(), 10);
    cppmh::model::ValueProxy<int> x2_value(x2.id(), {10, 10});

    x0_value.value() = 1;

    for (auto i = 0; i < 10; i++) {
        x1_value(i) = 10 * i;
    }

    for (auto i = 0; i < 10; i++) {
        for (auto j = 0; j < 10; j++) {
            x2_value(i, j) = 100 * (i + j);
        }
    }

    model.import_variable_values({x0_value, x1_value, x2_value});

    EXPECT_EQ(1, x0.value());

    for (auto i = 0; i < 10; i++) {
        EXPECT_EQ(10 * i, x1(i).value());
    }

    for (auto i = 0; i < 10; i++) {
        for (auto j = 0; j < 10; j++) {
            EXPECT_EQ(100 * (i + j), x2(i, j).value());
        }
    }
}

/*****************************************************************************/
TEST_F(TestModel, update_arg_void) {
    cppmh::model::Model<int, double> model;

    auto sequence = cppmh::utility::sequence(10);

    auto& variable_proxy = model.create_variables("x", 10, 0, 1);
    auto& expression_proxy =
        model.create_expression("e", variable_proxy.dot(sequence) + 1);
    model.create_constraint("c", variable_proxy.selection());

    for (auto&& variable : variable_proxy.flat_indexed_variables()) {
        variable = 1;
    }
    model.minimize(expression_proxy);
    model.update();

    // 0 + 1 + 2 + ... + 9 + 1 = 46
    EXPECT_EQ(46, expression_proxy.value());
    EXPECT_EQ(46, expression_proxy[0].value());
    EXPECT_EQ(46, model.objective().value());
}

/*****************************************************************************/
TEST_F(TestModel, update_arg_move) {
    cppmh::model::Model<int, double> model;

    auto sequence = cppmh::utility::sequence(10);

    auto& variable_proxy = model.create_variables("x", 10, 0, 1);
    auto& expression_proxy =
        model.create_expression("e", variable_proxy.dot(sequence) + 1);
    model.create_constraint("c", variable_proxy.selection());

    variable_proxy[0] = 1;

    model.minimize(expression_proxy);
    model.setup_default_neighborhood(false);

    model.update();

    cppmh::model::Move<int, double> move;
    move.sense = cppmh::model::MoveSense::Selection;
    move.alterations.emplace_back(&variable_proxy[0], 0);
    move.alterations.emplace_back(&variable_proxy[9], 1);

    model.update(move);

    EXPECT_EQ(10, expression_proxy.value());
    EXPECT_EQ(10, expression_proxy[0].value());
    EXPECT_EQ(10, model.objective().value());
    EXPECT_EQ(&variable_proxy[9],
              model.neighborhood().selections().front().selected_variable_ptr);
}

/*****************************************************************************/
TEST_F(TestModel, evaluate) {
    /// minimize
    {
        cppmh::model::Model<int, double> model;

        auto sequence = cppmh::utility::sequence(10);

        auto& variable_proxy = model.create_variables("x", 10, 0, 1);
        auto& expression_proxy =
            model.create_expression("e", variable_proxy.dot(sequence) + 1);
        auto& constraint_proxy =
            model.create_constraint("c", variable_proxy.sum() <= 5);

        cppmh::model::ValueProxy<double> local_penalty_coefficient_proxy(
            constraint_proxy.id());
        local_penalty_coefficient_proxy.value() = 100;

        cppmh::model::ValueProxy<double> global_penalty_coefficient_proxy(
            constraint_proxy.id());
        global_penalty_coefficient_proxy.value() = 10000;

        std::vector<cppmh::model::ValueProxy<double>>
            local_penalty_coefficient_proxies = {
                local_penalty_coefficient_proxy};
        std::vector<cppmh::model::ValueProxy<double>>
            global_penalty_coefficient_proxies = {
                global_penalty_coefficient_proxy};

        model.minimize(expression_proxy);
        model.setup_default_neighborhood(false);

        for (auto&& variable : variable_proxy.flat_indexed_variables()) {
            variable = 1;
        }
        model.update();

        {
            cppmh::model::Move<int, double> move;
            for (auto&& variable : variable_proxy.flat_indexed_variables()) {
                move.alterations.emplace_back(&variable, 1);
            }

            auto score = model.evaluate(move, local_penalty_coefficient_proxies,
                                        global_penalty_coefficient_proxies);

            EXPECT_EQ(46, score.objective);
            EXPECT_EQ(5 * 100, score.local_penalty);
            EXPECT_EQ(5 * 10000, score.global_penalty);
            EXPECT_EQ(46 + 5 * 100, score.local_augmented_objective);
            EXPECT_EQ(46 + 5 * 10000, score.global_augmented_objective);
            EXPECT_EQ(false, score.is_objective_improvable);
            EXPECT_EQ(false, score.is_constraint_improvable);
            EXPECT_EQ(false, score.is_feasible);
            model.update(move);
        }
        {
            cppmh::model::Move<int, double> move;
            for (auto&& variable : variable_proxy.flat_indexed_variables()) {
                move.alterations.emplace_back(&variable, 0);
            }

            auto score = model.evaluate(move, local_penalty_coefficient_proxies,
                                        global_penalty_coefficient_proxies);

            EXPECT_EQ(1, score.objective);
            EXPECT_EQ(0, score.local_penalty);
            EXPECT_EQ(0, score.global_penalty);
            EXPECT_EQ(1, score.local_augmented_objective);
            EXPECT_EQ(1, score.global_augmented_objective);
            EXPECT_EQ(true, score.is_objective_improvable);
            EXPECT_EQ(true, score.is_constraint_improvable);
            EXPECT_EQ(true, score.is_feasible);
            model.update(move);
        }
    }

    /// maximize
    {
        cppmh::model::Model<int, double> model;

        auto sequence = cppmh::utility::sequence(10);

        auto& variable_proxy = model.create_variables("x", 10, 0, 1);
        auto& expression_proxy =
            model.create_expression("e", variable_proxy.dot(sequence) + 1);
        auto& constraint_proxy =
            model.create_constraint("c", variable_proxy.sum() <= 5);

        cppmh::model::ValueProxy<double> local_penalty_coefficient_proxy(
            constraint_proxy.id());
        local_penalty_coefficient_proxy.value() = 100;

        cppmh::model::ValueProxy<double> global_penalty_coefficient_proxy(
            constraint_proxy.id());
        global_penalty_coefficient_proxy.value() = 10000;

        std::vector<cppmh::model::ValueProxy<double>>
            local_penalty_coefficient_proxies = {
                local_penalty_coefficient_proxy};
        std::vector<cppmh::model::ValueProxy<double>>
            global_penalty_coefficient_proxies = {
                global_penalty_coefficient_proxy};

        model.maximize(expression_proxy);
        model.setup_default_neighborhood(false);

        for (auto&& variable : variable_proxy.flat_indexed_variables()) {
            variable = 1;
        }
        model.update();

        {
            cppmh::model::Move<int, double> move;
            for (auto&& variable : variable_proxy.flat_indexed_variables()) {
                move.alterations.emplace_back(&variable, 1);
            }

            auto score = model.evaluate(move, local_penalty_coefficient_proxies,
                                        global_penalty_coefficient_proxies);

            EXPECT_EQ(-46, score.objective);
            EXPECT_EQ(5 * 100, score.local_penalty);
            EXPECT_EQ(5 * 10000, score.global_penalty);
            EXPECT_EQ(-46 + 5 * 100, score.local_augmented_objective);
            EXPECT_EQ(-46 + 5 * 10000, score.global_augmented_objective);
            EXPECT_EQ(false, score.is_objective_improvable);
            EXPECT_EQ(false, score.is_constraint_improvable);
            EXPECT_EQ(false, score.is_feasible);
            model.update(move);
        }
        {
            cppmh::model::Move<int, double> move;
            for (auto&& variable : variable_proxy.flat_indexed_variables()) {
                move.alterations.emplace_back(&variable, 0);
            }

            auto score = model.evaluate(move, local_penalty_coefficient_proxies,
                                        global_penalty_coefficient_proxies);

            EXPECT_EQ(-1, score.objective);
            EXPECT_EQ(0, score.local_penalty);
            EXPECT_EQ(0, score.global_penalty);
            EXPECT_EQ(-1, score.local_augmented_objective);
            EXPECT_EQ(-1, score.global_augmented_objective);
            EXPECT_EQ(false, score.is_objective_improvable);
            EXPECT_EQ(true, score.is_constraint_improvable);
            EXPECT_EQ(true, score.is_feasible);
            model.update(move);
        }
    }

}  // namespace

/*****************************************************************************/
TEST_F(TestModel, generate_variable_parameter_proxies) {
    cppmh::model::Model<int, double> model;

    auto& x0 = model.create_variable("x0");
    auto& x1 = model.create_variables("x1", 10);
    auto& x2 = model.create_variables("x2", {10, 10});

    int fill_value = random_integer();

    auto parameter_proxies =
        model.generate_variable_parameter_proxies(fill_value);
    EXPECT_EQ(x0.id(), parameter_proxies[0].id());
    EXPECT_EQ(1, static_cast<int>(parameter_proxies[0].number_of_dimensions()));
    EXPECT_EQ(1, static_cast<int>(parameter_proxies[0].number_of_elements()));

    EXPECT_EQ(x1.id(), parameter_proxies[1].id());
    EXPECT_EQ(1, static_cast<int>(parameter_proxies[1].number_of_dimensions()));
    EXPECT_EQ(10, static_cast<int>(parameter_proxies[1].number_of_elements()));

    EXPECT_EQ(x2.id(), parameter_proxies[2].id());
    EXPECT_EQ(2, static_cast<int>(parameter_proxies[2].number_of_dimensions()));
    EXPECT_EQ(100, static_cast<int>(parameter_proxies[2].number_of_elements()));
    for (auto&& value : parameter_proxies[0].flat_indexed_values()) {
        EXPECT_EQ(fill_value, value);
    }
    for (auto&& value : parameter_proxies[1].flat_indexed_values()) {
        EXPECT_EQ(fill_value, value);
    }
    for (auto&& value : parameter_proxies[2].flat_indexed_values()) {
        EXPECT_EQ(fill_value, value);
    }
}

/*****************************************************************************/
TEST_F(TestModel, generate_expression_parameter_proxies) {
    cppmh::model::Model<int, double> model;

    auto& e0 = model.create_expression("e0");
    auto& e1 = model.create_expressions("e1", 10);
    auto& e2 = model.create_expressions("e2", {10, 10});

    int fill_value = random_integer();

    auto parameter_proxies =
        model.generate_expression_parameter_proxies(fill_value);
    EXPECT_EQ(e0.id(), parameter_proxies[0].id());
    EXPECT_EQ(1, static_cast<int>(parameter_proxies[0].number_of_dimensions()));
    EXPECT_EQ(1, static_cast<int>(parameter_proxies[0].number_of_elements()));

    EXPECT_EQ(e1.id(), parameter_proxies[1].id());
    EXPECT_EQ(1, static_cast<int>(parameter_proxies[1].number_of_dimensions()));
    EXPECT_EQ(10, static_cast<int>(parameter_proxies[1].number_of_elements()));

    EXPECT_EQ(e2.id(), parameter_proxies[2].id());
    EXPECT_EQ(2, static_cast<int>(parameter_proxies[2].number_of_dimensions()));
    EXPECT_EQ(100, static_cast<int>(parameter_proxies[2].number_of_elements()));
    for (auto&& value : parameter_proxies[0].flat_indexed_values()) {
        EXPECT_EQ(fill_value, value);
    }
    for (auto&& value : parameter_proxies[1].flat_indexed_values()) {
        EXPECT_EQ(fill_value, value);
    }
    for (auto&& value : parameter_proxies[2].flat_indexed_values()) {
        EXPECT_EQ(fill_value, value);
    }
}

/*****************************************************************************/
TEST_F(TestModel, generate_constraint_parameter_proxies) {
    cppmh::model::Model<int, double> model;

    auto& c0 = model.create_constraint("c0");
    auto& c1 = model.create_constraints("c1", 10);
    auto& c2 = model.create_constraints("c2", {10, 10});

    int fill_value = random_integer();

    auto parameter_proxies =
        model.generate_constraint_parameter_proxies(fill_value);
    EXPECT_EQ(c0.id(), parameter_proxies[0].id());
    EXPECT_EQ(1, static_cast<int>(parameter_proxies[0].number_of_dimensions()));
    EXPECT_EQ(1, static_cast<int>(parameter_proxies[0].number_of_elements()));

    EXPECT_EQ(c1.id(), parameter_proxies[1].id());
    EXPECT_EQ(1, static_cast<int>(parameter_proxies[1].number_of_dimensions()));
    EXPECT_EQ(10, static_cast<int>(parameter_proxies[1].number_of_elements()));

    EXPECT_EQ(c2.id(), parameter_proxies[2].id());
    EXPECT_EQ(2, static_cast<int>(parameter_proxies[2].number_of_dimensions()));
    EXPECT_EQ(100, static_cast<int>(parameter_proxies[2].number_of_elements()));
    for (auto&& value : parameter_proxies[0].flat_indexed_values()) {
        EXPECT_EQ(fill_value, value);
    }
    for (auto&& value : parameter_proxies[1].flat_indexed_values()) {
        EXPECT_EQ(fill_value, value);
    }
    for (auto&& value : parameter_proxies[2].flat_indexed_values()) {
        EXPECT_EQ(fill_value, value);
    }
}

/*****************************************************************************/
TEST_F(TestModel, export_solution) {
    cppmh::model::Model<int, double> model;

    auto& x0 = model.create_variable("x0");
    auto& x1 = model.create_variables("x1", 10);
    auto& x2 = model.create_variables("x2", {20, 30});

    auto& e0 = model.create_expression("e0");
    auto& e1 = model.create_expressions("e1", 10);
    auto& e2 = model.create_expressions("e2", {20, 30});

    auto& c0 = model.create_constraint("c0");
    auto& c1 = model.create_constraints("c1", 10);
    auto& c2 = model.create_constraints("c2", {20, 30});

    e0 = random_integer() * x0;
    for (auto i = 0; i < 10; i++) {
        e1(i) = random_integer() * x1(i);
    }

    for (auto i = 0; i < 20; i++) {
        for (auto j = 0; j < 30; j++) {
            e2(i, j) = random_integer() * x2(i, j) + random_integer();
            c2(i, j) = e2(i, j) == random_integer();
        }
    }
    model.minimize(random_integer() * e0 + random_integer() * e1.sum() +
                   random_integer() * e2.sum());

    x0 = random_integer();
    for (auto i = 0; i < 10; i++) {
        x1(i) = random_integer();
    }

    for (auto i = 0; i < 20; i++) {
        for (auto j = 0; j < 30; j++) {
            x2(i, j) = random_integer();
        }
    }

    model.update();

    /// Test Solution
    auto solution = model.export_solution();
    EXPECT_EQ(3, static_cast<int>(solution.variable_value_proxies.size()));
    EXPECT_EQ(3, static_cast<int>(solution.expression_value_proxies.size()));
    EXPECT_EQ(3, static_cast<int>(solution.constraint_value_proxies.size()));
    EXPECT_EQ(3, static_cast<int>(solution.violation_value_proxies.size()));

    EXPECT_EQ(x0.id(), solution.variable_value_proxies[0].id());
    EXPECT_EQ(x0.value(), solution.variable_value_proxies[0].value());

    EXPECT_EQ(x1.id(), solution.variable_value_proxies[1].id());
    for (auto i = 0; i < 10; i++) {
        EXPECT_EQ(x1(i).value(), solution.variable_value_proxies[1](i));
    }

    EXPECT_EQ(x2.id(), solution.variable_value_proxies[2].id());
    for (auto i = 0; i < 20; i++) {
        for (auto j = 0; j < 30; j++) {
            EXPECT_EQ(x2(i, j).value(),
                      solution.variable_value_proxies[2](i, j));
        }
    }

    EXPECT_EQ(e0.id(), solution.expression_value_proxies[0].id());
    EXPECT_EQ(e0.value(), solution.expression_value_proxies[0].value());

    EXPECT_EQ(e1.id(), solution.expression_value_proxies[1].id());
    for (auto i = 0; i < 10; i++) {
        EXPECT_EQ(e1(i).value(), solution.expression_value_proxies[1](i));
    }

    EXPECT_EQ(e2.id(), solution.expression_value_proxies[2].id());
    for (auto i = 0; i < 20; i++) {
        for (auto j = 0; j < 30; j++) {
            EXPECT_EQ(e2(i, j).value(),
                      solution.expression_value_proxies[2](i, j));
        }
    }

    EXPECT_EQ(c0.id(), solution.constraint_value_proxies[0].id());
    EXPECT_EQ(c0.constraint_value(),
              solution.constraint_value_proxies[0].value());

    EXPECT_EQ(c1.id(), solution.constraint_value_proxies[1].id());
    for (auto i = 0; i < 10; i++) {
        EXPECT_EQ(c1(i).constraint_value(),
                  solution.constraint_value_proxies[1](i));
    }

    EXPECT_EQ(c2.id(), solution.constraint_value_proxies[2].id());
    for (auto i = 0; i < 20; i++) {
        for (auto j = 0; j < 30; j++) {
            EXPECT_EQ(c2(i, j).constraint_value(),
                      solution.constraint_value_proxies[2](i, j));
        }
    }

    EXPECT_EQ(c0.id(), solution.violation_value_proxies[0].id());
    EXPECT_EQ(c0.violation_value(),
              solution.violation_value_proxies[0].value());

    EXPECT_EQ(c1.id(), solution.constraint_value_proxies[1].id());
    for (auto i = 0; i < 10; i++) {
        EXPECT_EQ(c1(i).violation_value(),
                  solution.violation_value_proxies[1](i));
    }

    EXPECT_EQ(c2.id(), solution.violation_value_proxies[2].id());
    for (auto i = 0; i < 20; i++) {
        for (auto j = 0; j < 30; j++) {
            EXPECT_EQ(c2(i, j).violation_value(),
                      solution.violation_value_proxies[2](i, j));
        }
    }

    /// Test NamedSolution
    auto named_solution = model.convert_to_named_solution(solution);
    EXPECT_EQ(3, static_cast<int>(named_solution.variables().size()));
    EXPECT_EQ(3, static_cast<int>(named_solution.expressions().size()));
    EXPECT_EQ(3, static_cast<int>(named_solution.constraints().size()));
    EXPECT_EQ(3, static_cast<int>(named_solution.violations().size()));

    EXPECT_EQ(x0.id(), named_solution.variables("x0").id());
    EXPECT_EQ(x0.id(), named_solution.variables().at("x0").id());
    EXPECT_EQ(x0.value(), named_solution.variables("x0").value());
    EXPECT_EQ(x0.value(), named_solution.variables().at("x0").value());

    EXPECT_EQ(x1.id(), named_solution.variables("x1").id());
    EXPECT_EQ(x1.id(), named_solution.variables().at("x1").id());
    for (auto i = 0; i < 10; i++) {
        EXPECT_EQ(x1(i).value(), named_solution.variables("x1")(i));
        EXPECT_EQ(x1(i).value(), named_solution.variables().at("x1")(i));
    }

    EXPECT_EQ(x2.id(), named_solution.variables("x2").id());
    EXPECT_EQ(x2.id(), named_solution.variables().at("x2").id());
    for (auto i = 0; i < 20; i++) {
        for (auto j = 0; j < 30; j++) {
            EXPECT_EQ(x2(i, j).value(), named_solution.variables("x2")(i, j));
            EXPECT_EQ(x2(i, j).value(),
                      named_solution.variables().at("x2")(i, j));
        }
    }

    EXPECT_EQ(e0.id(), named_solution.expressions("e0").id());
    EXPECT_EQ(e0.id(), named_solution.expressions().at("e0").id());
    EXPECT_EQ(e0.value(), named_solution.expressions("e0").value());
    EXPECT_EQ(e0.value(), named_solution.expressions().at("e0").value());

    EXPECT_EQ(e1.id(), named_solution.expressions("e1").id());
    EXPECT_EQ(e1.id(), named_solution.expressions().at("e1").id());
    for (auto i = 0; i < 10; i++) {
        EXPECT_EQ(e1(i).value(), named_solution.expressions("e1")(i));
        EXPECT_EQ(e1(i).value(), named_solution.expressions().at("e1")(i));
    }

    EXPECT_EQ(e2.id(), named_solution.expressions("e2").id());
    EXPECT_EQ(e2.id(), named_solution.expressions().at("e2").id());
    for (auto i = 0; i < 20; i++) {
        for (auto j = 0; j < 30; j++) {
            EXPECT_EQ(e2(i, j).value(), named_solution.expressions("e2")(i, j));
            EXPECT_EQ(e2(i, j).value(),
                      named_solution.expressions().at("e2")(i, j));
        }
    }

    EXPECT_EQ(c0.id(), named_solution.constraints("c0").id());
    EXPECT_EQ(c0.id(), named_solution.constraints().at("c0").id());
    EXPECT_EQ(c0.constraint_value(), named_solution.constraints("c0").value());
    EXPECT_EQ(c0.constraint_value(),
              named_solution.constraints().at("c0").value());

    EXPECT_EQ(c1.id(), named_solution.constraints("c1").id());
    EXPECT_EQ(c1.id(), named_solution.constraints().at("c1").id());
    for (auto i = 0; i < 10; i++) {
        EXPECT_EQ(c1(i).constraint_value(),
                  named_solution.constraints("c1")(i));
        EXPECT_EQ(c1(i).constraint_value(),
                  named_solution.constraints().at("c1")(i));
    }

    EXPECT_EQ(c2.id(), named_solution.constraints("c2").id());
    EXPECT_EQ(c2.id(), named_solution.constraints().at("c2").id());
    for (auto i = 0; i < 20; i++) {
        for (auto j = 0; j < 30; j++) {
            EXPECT_EQ(c2(i, j).constraint_value(),
                      named_solution.constraints("c2")(i, j));
            EXPECT_EQ(c2(i, j).constraint_value(),
                      named_solution.constraints().at("c2")(i, j));
        }
    }

    EXPECT_EQ(c0.id(), named_solution.violations("c0").id());
    EXPECT_EQ(c0.id(), named_solution.violations().at("c0").id());
    EXPECT_EQ(c0.violation_value(), named_solution.violations("c0").value());
    EXPECT_EQ(c0.violation_value(),
              named_solution.violations().at("c0").value());

    EXPECT_EQ(c1.id(), named_solution.violations("c1").id());
    EXPECT_EQ(c1.id(), named_solution.violations().at("c1").id());
    for (auto i = 0; i < 10; i++) {
        EXPECT_EQ(c1(i).violation_value(), named_solution.violations("c1")(i));
        EXPECT_EQ(c1(i).violation_value(),
                  named_solution.violations().at("c1")(i));
    }

    EXPECT_EQ(c2.id(), named_solution.violations("c2").id());
    EXPECT_EQ(c2.id(), named_solution.violations().at("c2").id());
    for (auto i = 0; i < 20; i++) {
        for (auto j = 0; j < 30; j++) {
            EXPECT_EQ(c2(i, j).violation_value(),
                      named_solution.violations("c2")(i, j));
            EXPECT_EQ(c2(i, j).violation_value(),
                      named_solution.violations().at("c2")(i, j));
        }
    }
}

/*****************************************************************************/
TEST_F(TestModel, convert_to_named_solution) {
    /// This method is tested in export_solution.
}

/*****************************************************************************/
TEST_F(TestModel, variable_proxies) {
    /// This method is tested in create_variable_scaler and so on.
}

/*****************************************************************************/
TEST_F(TestModel, expression_proxies) {
    /// This method is tested in create_expression_scaler and so on.
}

/*****************************************************************************/
TEST_F(TestModel, constraint_proxies) {
    /// This method is tested in create_constraint_scaler and so on.
}

/*****************************************************************************/
TEST_F(TestModel, objective) {
    /// This method is tested in minimize_arg_function and so on.
}

/*****************************************************************************/
TEST_F(TestModel, variable_names) {
    /// This method is tested in create_variable_scaler and so on.
}

/*****************************************************************************/
TEST_F(TestModel, expression_names) {
    /// This method is tested in create_expression_scaler and so on.
}

/*****************************************************************************/
TEST_F(TestModel, constraint_names) {
    /// This method is tested in create_constraint_scaler and so on.
}

/*****************************************************************************/
}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/