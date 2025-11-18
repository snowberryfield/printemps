/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <gtest/gtest.h>
#include <printemps.h>

namespace {
using namespace printemps;
/*****************************************************************************/
class TestModelComponentCreator : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestModelComponentCreator, create_variable_scalar_without_bound) {
    {
        model::Model<int, double> model;
        for (auto i = 0; i < model_handler::ModelComponentCreatorConstant::
                                 MAX_NUMBER_OF_VARIABLE_PROXIES;
             i++) {
            auto  name = "x" + std::to_string(i);
            auto& x    = model.create_variable(name);
            EXPECT_EQ(i + 1, static_cast<int>(model.variable_proxies().size()));
            EXPECT_EQ(i, x.index());
            EXPECT_EQ(constant::INT_HALF_MIN, x.lower_bound());
            EXPECT_EQ(constant::INT_HALF_MAX, x.upper_bound());
            EXPECT_FALSE(x.has_bounds());
            EXPECT_EQ(model_component::VariableType::Integer, x.type());
            EXPECT_EQ(&x, &model.variable_proxies().back());
            EXPECT_EQ(name, model.variable_names().back());
        }
        ASSERT_THROW(model.create_variable("error"), std::runtime_error);
    }
    {
        model::Model<int, double> model;
        ASSERT_THROW(model.create_variable("s p a c e"), std::runtime_error);
    }
}

/*****************************************************************************/
TEST_F(TestModelComponentCreator, create_variable_scalar_with_bound) {
    {
        model::Model<int, double> model;
        for (auto i = 0; i < model_handler::ModelComponentCreatorConstant::
                                 MAX_NUMBER_OF_VARIABLE_PROXIES;
             i++) {
            auto  name = "x" + std::to_string(i);
            auto& x    = model.create_variable(name, 0, 1);
            EXPECT_EQ(i + 1, static_cast<int>(model.variable_proxies().size()));
            EXPECT_EQ(i, x.index());
            EXPECT_EQ(0, x.lower_bound());
            EXPECT_EQ(1, x.upper_bound());
            EXPECT_TRUE(x.has_bounds());
            EXPECT_EQ(model_component::VariableType::Binary, x.type());
            EXPECT_EQ(&x, &model.variable_proxies().back());
            EXPECT_EQ(name, model.variable_names().back());
        }
        ASSERT_THROW(model.create_variable("error", 0, 1), std::runtime_error);
    }
    {
        model::Model<int, double> model;
        ASSERT_THROW(model.create_variable("s p a c e", 0, 1),
                     std::runtime_error);
    }
}

/*****************************************************************************/
TEST_F(TestModelComponentCreator,
       create_variable_one_dimensional_without_bound) {
    {
        model::Model<int, double> model;
        for (auto i = 0; i < model_handler::ModelComponentCreatorConstant::
                                 MAX_NUMBER_OF_VARIABLE_PROXIES;
             i++) {
            auto  name = "x" + std::to_string(i);
            auto& x    = model.create_variables(name, 2);
            EXPECT_EQ(i + 1, static_cast<int>(model.variable_proxies().size()));
            EXPECT_EQ(i, x.index());
            EXPECT_EQ(constant::INT_HALF_MIN, x(0).lower_bound());
            EXPECT_EQ(constant::INT_HALF_MAX, x(0).upper_bound());
            EXPECT_FALSE(x(0).has_bounds());
            EXPECT_EQ(model_component::VariableType::Integer, x(0).type());
            EXPECT_EQ(&x, &model.variable_proxies().back());
            EXPECT_EQ(name, model.variable_names().back());
        }
        ASSERT_THROW(model.create_variables("error", 2), std::runtime_error);
    }
    {
        model::Model<int, double> model;
        ASSERT_THROW(model.create_variables("s p a c e", 2),
                     std::runtime_error);
    }
}

/*****************************************************************************/
TEST_F(TestModelComponentCreator, create_variable_one_dimensional_with_bound) {
    {
        model::Model<int, double> model;
        for (auto i = 0; i < model_handler::ModelComponentCreatorConstant::
                                 MAX_NUMBER_OF_VARIABLE_PROXIES;
             i++) {
            auto  name = "x" + std::to_string(i);
            auto& x    = model.create_variables(name, 2, 0, 1);
            EXPECT_EQ(i + 1, static_cast<int>(model.variable_proxies().size()));
            EXPECT_EQ(i, x.index());
            EXPECT_EQ(0, x(0).lower_bound());
            EXPECT_EQ(1, x(0).upper_bound());
            EXPECT_TRUE(x(0).has_bounds());
            EXPECT_EQ(model_component::VariableType::Binary, x(0).type());
            EXPECT_EQ(&x, &model.variable_proxies().back());
            EXPECT_EQ(name, model.variable_names().back());
        }
        ASSERT_THROW(model.create_variables("error", 2, 0, 1),
                     std::runtime_error);
    }
    {
        model::Model<int, double> model;
        ASSERT_THROW(model.create_variables("s p a c e", 2, 0, 1),
                     std::runtime_error);
    }
}

/*****************************************************************************/
TEST_F(TestModelComponentCreator,
       create_variable_two_dimensional_without_bound) {
    {
        model::Model<int, double> model;
        for (auto i = 0; i < model_handler::ModelComponentCreatorConstant::
                                 MAX_NUMBER_OF_VARIABLE_PROXIES;
             i++) {
            auto  name = "x" + std::to_string(i);
            auto& x    = model.create_variables(name, {2, 3});
            EXPECT_EQ(i + 1, static_cast<int>(model.variable_proxies().size()));
            EXPECT_EQ(i, x.index());
            EXPECT_EQ(constant::INT_HALF_MIN, x(0, 0).lower_bound());
            EXPECT_EQ(constant::INT_HALF_MAX, x(0, 0).upper_bound());
            EXPECT_FALSE(x(0, 0).has_bounds());
            EXPECT_EQ(model_component::VariableType::Integer, x(0, 0).type());
            EXPECT_EQ(&x, &model.variable_proxies().back());
            EXPECT_EQ(name, model.variable_names().back());
        }
        ASSERT_THROW(model.create_variables("error", {2, 3}),
                     std::runtime_error);
    }
    {
        model::Model<int, double> model;
        ASSERT_THROW(model.create_variables("s p a c e", {2, 3}),
                     std::runtime_error);
    }
}

/*****************************************************************************/
TEST_F(TestModelComponentCreator, create_variable_two_dimensional_with_bound) {
    {
        model::Model<int, double> model;
        for (auto i = 0; i < model_handler::ModelComponentCreatorConstant::
                                 MAX_NUMBER_OF_VARIABLE_PROXIES;
             i++) {
            auto  name = "x" + std::to_string(i);
            auto& x    = model.create_variables(name, {2, 3}, 0, 1);
            EXPECT_EQ(i + 1, static_cast<int>(model.variable_proxies().size()));
            EXPECT_EQ(i, x.index());
            EXPECT_EQ(0, x(0, 0).lower_bound());
            EXPECT_EQ(1, x(0, 0).upper_bound());
            EXPECT_TRUE(x(0, 0).has_bounds());
            EXPECT_EQ(model_component::VariableType::Binary, x(0, 0).type());
            EXPECT_EQ(&x, &model.variable_proxies().back());
            EXPECT_EQ(name, model.variable_names().back());
        }
        ASSERT_THROW(model.create_variables("error", {2, 3}, 0, 1),
                     std::runtime_error);
    }
    {
        model::Model<int, double> model;
        ASSERT_THROW(model.create_variables("s p a c e", {2, 3}, 0, 1),
                     std::runtime_error);
    }
}

/*****************************************************************************/
TEST_F(TestModelComponentCreator, create_expression_scalar) {
    {
        model::Model<int, double> model;
        for (auto i = 0; i < model_handler::ModelComponentCreatorConstant::
                                 MAX_NUMBER_OF_EXPRESSION_PROXIES;
             i++) {
            auto  name = "p" + std::to_string(i);
            auto& p    = model.create_expression(name);
            EXPECT_EQ(i + 1,
                      static_cast<int>(model.expression_proxies().size()));
            EXPECT_EQ(i, p.index());
            EXPECT_EQ(&p, &model.expression_proxies().back());
            EXPECT_EQ(name, model.expression_names().back());
        }
        ASSERT_THROW(model.create_expression("error"), std::runtime_error);
    }
    {
        model::Model<int, double> model;
        ASSERT_THROW(model.create_expression("s p a c e"), std::runtime_error);
    }
}

/*****************************************************************************/
TEST_F(TestModelComponentCreator, create_expression_one_dimensional) {
    {
        model::Model<int, double> model;
        for (auto i = 0; i < model_handler::ModelComponentCreatorConstant::
                                 MAX_NUMBER_OF_EXPRESSION_PROXIES;
             i++) {
            auto  name = "p" + std::to_string(i);
            auto& p    = model.create_expressions(name, 2);
            EXPECT_EQ(i + 1,
                      static_cast<int>(model.expression_proxies().size()));
            EXPECT_EQ(i, p.index());
            EXPECT_EQ(&p, &model.expression_proxies().back());
            EXPECT_EQ(name, model.expression_names().back());
        }
        ASSERT_THROW(model.create_expressions("error", 2), std::runtime_error);
    }
    {
        model::Model<int, double> model;
        ASSERT_THROW(model.create_expressions("s p a c e", 2),
                     std::runtime_error);
    }
}

/*****************************************************************************/
TEST_F(TestModelComponentCreator, create_expression_two_dimensional) {
    {
        model::Model<int, double> model;
        for (auto i = 0; i < model_handler::ModelComponentCreatorConstant::
                                 MAX_NUMBER_OF_EXPRESSION_PROXIES;
             i++) {
            auto  name = "p" + std::to_string(i);
            auto& p    = model.create_expressions(name, {2, 3});
            EXPECT_EQ(i + 1,
                      static_cast<int>(model.expression_proxies().size()));
            EXPECT_EQ(i, p.index());
            EXPECT_EQ(&p, &model.expression_proxies().back());
            EXPECT_EQ(name, model.expression_names().back());
        }
        ASSERT_THROW(model.create_expressions("error", {2, 3}),
                     std::runtime_error);
    }
    {
        model::Model<int, double> model;
        ASSERT_THROW(model.create_expressions("s p a c e", {2, 3}),
                     std::runtime_error);
    }
}

/*****************************************************************************/
TEST_F(TestModelComponentCreator, create_expression_arg_expression_like) {
    {
        model::Model<int, double> model;

        for (auto i = 0; i < model_handler::ModelComponentCreatorConstant::
                                 MAX_NUMBER_OF_EXPRESSION_PROXIES;
             i++) {
            auto name = "p" + std::to_string(i);
            auto variable =
                model_component::Variable<int, double>::create_instance();

            auto& p = model.create_expression(name, variable);
            EXPECT_EQ(i + 1,
                      static_cast<int>(model.expression_proxies().size()));
            EXPECT_EQ(i, p.index());
            EXPECT_EQ(&p, &model.expression_proxies().back());
            EXPECT_EQ(name, model.expression_names().back());
        }

        auto variable =
            model_component::Variable<int, double>::create_instance();
        ASSERT_THROW(model.create_expression("error", variable),
                     std::runtime_error);
    }
    {
        model::Model<int, double> model;
        auto                      variable =
            model_component::Variable<int, double>::create_instance();
        ASSERT_THROW(model.create_expression("s p a c e", variable),
                     std::runtime_error);
    }
}

/*****************************************************************************/
TEST_F(TestModelComponentCreator, create_expression_arg_expression) {
    {
        model::Model<int, double> model;

        for (auto i = 0; i < model_handler::ModelComponentCreatorConstant::
                                 MAX_NUMBER_OF_EXPRESSION_PROXIES;
             i++) {
            auto name = "p" + std::to_string(i);
            auto expression =
                model_component::Expression<int, double>::create_instance();

            auto& p = model.create_expression(name, expression);
            EXPECT_EQ(i + 1,
                      static_cast<int>(model.expression_proxies().size()));
            EXPECT_EQ(i, p.index());
            EXPECT_EQ(&p, &model.expression_proxies().back());
            EXPECT_EQ(name, model.expression_names().back());
        }

        auto expression =
            model_component::Expression<int, double>::create_instance();
        ASSERT_THROW(model.create_expression("error", expression),
                     std::runtime_error);
    }
    {
        model::Model<int, double> model;

        auto expression =
            model_component::Expression<int, double>::create_instance();
        ASSERT_THROW(model.create_expression("s p a c e", expression),
                     std::runtime_error);
    }
}

/*****************************************************************************/
TEST_F(TestModelComponentCreator, create_constraint_scalar) {
    {
        model::Model<int, double> model;
        for (auto i = 0; i < model_handler::ModelComponentCreatorConstant::
                                 MAX_NUMBER_OF_CONSTRAINT_PROXIES;
             i++) {
            auto  name = "g" + std::to_string(i);
            auto& g    = model.create_constraint(name);
            EXPECT_EQ(i + 1,
                      static_cast<int>(model.constraint_proxies().size()));
            EXPECT_EQ(i, g.index());
            EXPECT_EQ(&g, &model.constraint_proxies().back());
            EXPECT_EQ(name, model.constraint_names().back());
        }
        ASSERT_THROW(model.create_constraint("error"), std::runtime_error);
    }
    {
        model::Model<int, double> model;
        ASSERT_THROW(model.create_constraint("s p a c e"), std::runtime_error);
    }
}

/*****************************************************************************/
TEST_F(TestModelComponentCreator, create_constraint_one_dimensional) {
    {
        model::Model<int, double> model;
        for (auto i = 0; i < model_handler::ModelComponentCreatorConstant::
                                 MAX_NUMBER_OF_CONSTRAINT_PROXIES;
             i++) {
            auto  name = "g" + std::to_string(i);
            auto& g    = model.create_constraints(name, 2);
            EXPECT_EQ(i + 1,
                      static_cast<int>(model.constraint_proxies().size()));
            EXPECT_EQ(i, g.index());
            EXPECT_EQ(&g, &model.constraint_proxies().back());
            EXPECT_EQ(name, model.constraint_names().back());
        }
        ASSERT_THROW(model.create_constraints("error", 2), std::runtime_error);
    }
    {
        model::Model<int, double> model;
        ASSERT_THROW(model.create_constraints("s p a c e", 2),
                     std::runtime_error);
    }
}

/*****************************************************************************/
TEST_F(TestModelComponentCreator, create_constraint_two_dimensional) {
    {
        model::Model<int, double> model;
        for (auto i = 0; i < model_handler::ModelComponentCreatorConstant::
                                 MAX_NUMBER_OF_CONSTRAINT_PROXIES;
             i++) {
            auto  name = "g" + std::to_string(i);
            auto& g    = model.create_constraints(name, {2, 3});
            EXPECT_EQ(i + 1,
                      static_cast<int>(model.constraint_proxies().size()));
            EXPECT_EQ(i, g.index());
            EXPECT_EQ(&g, &model.constraint_proxies().back());
            EXPECT_EQ(name, model.constraint_names().back());
        }
        ASSERT_THROW(model.create_constraints("error", {2, 3}),
                     std::runtime_error);
    }
    {
        model::Model<int, double> model;
        ASSERT_THROW(model.create_constraints("s p a c e", {2, 3}),
                     std::runtime_error);
    }
}

/*****************************************************************************/
TEST_F(TestModelComponentCreator, create_constraint_arg_constraint) {
    {
        model::Model<int, double> model;

        for (auto i = 0; i < model_handler::ModelComponentCreatorConstant::
                                 MAX_NUMBER_OF_CONSTRAINT_PROXIES;
             i++) {
            auto name = "g" + std::to_string(i);
            auto expression =
                model_component::Expression<int, double>::create_instance();
            auto constraint = expression <= 1;

            auto& g = model.create_constraint(name, constraint);
            EXPECT_EQ(i + 1,
                      static_cast<int>(model.constraint_proxies().size()));
            EXPECT_EQ(i, g.index());
            EXPECT_EQ(&g, &model.constraint_proxies().back());
            EXPECT_EQ(name, model.constraint_names().back());
        }

        auto expression =
            model_component::Expression<int, double>::create_instance();
        auto constraint = expression <= 1;
        ASSERT_THROW(model.create_constraint("error", constraint),
                     std::runtime_error);
    }
    {
        model::Model<int, double> model;
        auto                      expression =
            model_component::Expression<int, double>::create_instance();
        auto constraint = expression <= 1;
        ASSERT_THROW(model.create_constraint("s p a c e", constraint),
                     std::runtime_error);
    }
}

/*****************************************************************************/
TEST_F(TestModelComponentCreator, minimize_arg_expression_like) {
    model::Model<int, double> model;

    auto& x = model.create_variables("x", 10, -1, 1);
    auto& p = model.create_expression("p", x.sum() + 1);

    [[maybe_unused]] auto f =
        [&p](const neighborhood::Move<int, double>& a_MOVE) {
            return p.evaluate(a_MOVE);
        };
    model.minimize(p);

    EXPECT_TRUE(model.is_defined_objective());
    EXPECT_TRUE(model.is_minimization());
    EXPECT_EQ(1.0, model.sign());
    EXPECT_EQ(10, static_cast<int>(
                      model.objective().expression().sensitivities().size()));
    EXPECT_EQ(1, model.objective().expression().constant_value());

    for (auto&& element : x.flat_indexed_variables()) {
        element = 1;
    }
    model.updater().update();
    EXPECT_EQ(10 + 1, model.objective().value());
}

/*****************************************************************************/
TEST_F(TestModelComponentCreator, minimize_arg_expression) {
    model::Model<int, double> model;

    auto& x = model.create_variables("x", 10, -1, 1);
    auto& p = model.create_expression("p", x.sum() + 1);

    [[maybe_unused]] auto f =
        [&p](const neighborhood::Move<int, double>& a_MOVE) {
            return p.evaluate(a_MOVE);
        };
    model.minimize(p(0));

    EXPECT_TRUE(model.is_defined_objective());
    EXPECT_TRUE(model.is_minimization());
    EXPECT_EQ(1.0, model.sign());
    EXPECT_EQ(10, static_cast<int>(
                      model.objective().expression().sensitivities().size()));
    EXPECT_EQ(1, model.objective().expression().constant_value());

    for (auto&& element : x.flat_indexed_variables()) {
        element = 1;
    }
    model.updater().update();
    EXPECT_EQ(10 + 1, model.objective().value());
}

/*****************************************************************************/
TEST_F(TestModelComponentCreator, maximize_arg_expression_like) {
    model::Model<int, double> model;

    auto& x = model.create_variables("x", 10, -1, 1);
    auto& p = model.create_expression("p", x.sum() + 1);

    [[maybe_unused]] auto f =
        [&p](const neighborhood::Move<int, double>& a_MOVE) {
            return p.evaluate(a_MOVE);
        };
    model.maximize(p);

    EXPECT_TRUE(model.is_defined_objective());
    EXPECT_FALSE(model.is_minimization());
    EXPECT_EQ(-1.0, model.sign());
    EXPECT_EQ(10, static_cast<int>(
                      model.objective().expression().sensitivities().size()));
    EXPECT_EQ(1, model.objective().expression().constant_value());

    for (auto&& element : x.flat_indexed_variables()) {
        element = 1;
    }
    model.updater().update();
    EXPECT_EQ(10 + 1, model.objective().value());
}

/*****************************************************************************/
TEST_F(TestModelComponentCreator, maximize_arg_expression) {
    model::Model<int, double> model;
    auto&                     x = model.create_variables("x", 10, -1, 1);
    auto&                     p = model.create_expression("p", x.sum() + 1);
    [[maybe_unused]] auto     f =
        [&p](const neighborhood::Move<int, double>& a_MOVE) {
            return p.evaluate(a_MOVE);
        };
    model.maximize(p(0));

    EXPECT_TRUE(model.is_defined_objective());
    EXPECT_FALSE(model.is_minimization());
    EXPECT_EQ(-1.0, model.sign());
    EXPECT_EQ(10, static_cast<int>(
                      model.objective().expression().sensitivities().size()));
    EXPECT_EQ(1, model.objective().expression().constant_value());

    for (auto&& element : x.flat_indexed_variables()) {
        element = 1;
    }
    model.updater().update();
    EXPECT_EQ(10 + 1, model.objective().value());
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/