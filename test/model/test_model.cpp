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
class TestModel : public ::testing::Test {
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
TEST_F(TestModel, initialize) {
    model::Model<int, double> model;

    /// These declarations are for googletest constraint.
    /// http://opencv.jp/googletestdocs/FAQ.html#faq-the-compiler-complains-about-undefined-reference
    auto max_number_of_variable_proxies =
        model::ModelConstant::MAX_NUMBER_OF_VARIABLE_PROXIES;
    auto max_number_of_expression_proxies =
        model::ModelConstant::MAX_NUMBER_OF_EXPRESSION_PROXIES;
    auto max_number_of_constraint_proxies =
        model::ModelConstant::MAX_NUMBER_OF_CONSTRAINT_PROXIES;

    EXPECT_EQ("", model.name());

    EXPECT_EQ(max_number_of_variable_proxies,
              static_cast<int>(model.variable_proxies().capacity()));
    EXPECT_EQ(max_number_of_expression_proxies,
              static_cast<int>(model.expression_proxies().capacity()));
    EXPECT_EQ(max_number_of_constraint_proxies,
              static_cast<int>(model.constraint_proxies().capacity()));

    EXPECT_TRUE(model.variable_names().empty());
    EXPECT_TRUE(model.expression_names().empty());
    EXPECT_TRUE(model.constraint_names().empty());

    EXPECT_FALSE(model.is_defined_objective());
    EXPECT_TRUE(model.is_minimization());
    EXPECT_EQ(1.0, model.sign());
    EXPECT_FALSE(model.is_solved());
    EXPECT_FALSE(model.is_feasible());
    EXPECT_EQ(0.0, model.global_penalty_coefficient());

    /// Variable Reference
    EXPECT_EQ(  //
        true, model.variable_reference().variable_ptrs.empty());
    EXPECT_EQ(  //
        true, model.variable_reference_original().variable_ptrs.empty());

    /// Variable Type Reference
    EXPECT_EQ(  //
        true, model.variable_type_reference().binary_variable_ptrs.empty());
    EXPECT_EQ(  //
        true,
        model.variable_type_reference_original().binary_variable_ptrs.empty());

    /// Constraint Reference
    EXPECT_EQ(  //
        true, model.constraint_reference().constraint_ptrs.empty());
    EXPECT_EQ(  //
        true, model.constraint_reference_original().constraint_ptrs.empty());

    /// Constraint Type Reference
    EXPECT_EQ(  //
        true, model.constraint_type_reference().singleton_ptrs.empty());
    EXPECT_EQ(  //
        true,
        model.constraint_type_reference_original().singleton_ptrs.empty());
}

/*****************************************************************************/
TEST_F(TestModel, constructor_arg_name) {
    model::Model<int, double> model("name");
    EXPECT_EQ("name", model.name());
}

/*****************************************************************************/
TEST_F(TestModel, set_name) {
    model::Model<int, double> model;
    EXPECT_EQ("", model.name());
    model.set_name("name");
    EXPECT_EQ("name", model.name());
}

/*****************************************************************************/
TEST_F(TestModel, name) {
    /// This method is tested in set_name().
}

/*****************************************************************************/
TEST_F(TestModel, create_variable_scalar_without_bound) {
    {
        model::Model<int, double> model;
        for (auto i = 0;
             i < model::ModelConstant::MAX_NUMBER_OF_VARIABLE_PROXIES; i++) {
            auto  name = "x" + std::to_string(i);
            auto& x    = model.create_variable(name);
            EXPECT_EQ(i + 1, static_cast<int>(model.variable_proxies().size()));
            EXPECT_EQ(i, x.index());
            EXPECT_EQ(constant::INT_HALF_MIN, x.lower_bound());
            EXPECT_EQ(constant::INT_HALF_MAX, x.upper_bound());
            EXPECT_FALSE(x.has_bounds());
            EXPECT_EQ(model_component::VariableSense::Integer, x.sense());
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
TEST_F(TestModel, create_variable_scalar_with_bound) {
    {
        model::Model<int, double> model;
        for (auto i = 0;
             i < model::ModelConstant::MAX_NUMBER_OF_VARIABLE_PROXIES; i++) {
            auto  name = "x" + std::to_string(i);
            auto& x    = model.create_variable(name, 0, 1);
            EXPECT_EQ(i + 1, static_cast<int>(model.variable_proxies().size()));
            EXPECT_EQ(i, x.index());
            EXPECT_EQ(0, x.lower_bound());
            EXPECT_EQ(1, x.upper_bound());
            EXPECT_TRUE(x.has_bounds());
            EXPECT_EQ(model_component::VariableSense::Binary, x.sense());
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
TEST_F(TestModel, create_variable_one_dimensional_without_bound) {
    {
        model::Model<int, double> model;
        for (auto i = 0;
             i < model::ModelConstant::MAX_NUMBER_OF_VARIABLE_PROXIES; i++) {
            auto  name = "x" + std::to_string(i);
            auto& x    = model.create_variables(name, 2);
            EXPECT_EQ(i + 1, static_cast<int>(model.variable_proxies().size()));
            EXPECT_EQ(i, x.index());
            EXPECT_EQ(constant::INT_HALF_MIN, x(0).lower_bound());
            EXPECT_EQ(constant::INT_HALF_MAX, x(0).upper_bound());
            EXPECT_FALSE(x(0).has_bounds());
            EXPECT_EQ(model_component::VariableSense::Integer, x(0).sense());
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
TEST_F(TestModel, create_variable_one_dimensional_with_bound) {
    {
        model::Model<int, double> model;
        for (auto i = 0;
             i < model::ModelConstant::MAX_NUMBER_OF_VARIABLE_PROXIES; i++) {
            auto  name = "x" + std::to_string(i);
            auto& x    = model.create_variables(name, 2, 0, 1);
            EXPECT_EQ(i + 1, static_cast<int>(model.variable_proxies().size()));
            EXPECT_EQ(i, x.index());
            EXPECT_EQ(0, x(0).lower_bound());
            EXPECT_EQ(1, x(0).upper_bound());
            EXPECT_TRUE(x(0).has_bounds());
            EXPECT_EQ(model_component::VariableSense::Binary, x(0).sense());
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
TEST_F(TestModel, create_variable_two_dimensional_without_bound) {
    {
        model::Model<int, double> model;
        for (auto i = 0;
             i < model::ModelConstant::MAX_NUMBER_OF_VARIABLE_PROXIES; i++) {
            auto  name = "x" + std::to_string(i);
            auto& x    = model.create_variables(name, {2, 3});
            EXPECT_EQ(i + 1, static_cast<int>(model.variable_proxies().size()));
            EXPECT_EQ(i, x.index());
            EXPECT_EQ(constant::INT_HALF_MIN, x(0, 0).lower_bound());
            EXPECT_EQ(constant::INT_HALF_MAX, x(0, 0).upper_bound());
            EXPECT_FALSE(x(0, 0).has_bounds());
            EXPECT_EQ(model_component::VariableSense::Integer, x(0, 0).sense());
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
TEST_F(TestModel, create_variable_two_dimensional_with_bound) {
    {
        model::Model<int, double> model;
        for (auto i = 0;
             i < model::ModelConstant::MAX_NUMBER_OF_VARIABLE_PROXIES; i++) {
            auto  name = "x" + std::to_string(i);
            auto& x    = model.create_variables(name, {2, 3}, 0, 1);
            EXPECT_EQ(i + 1, static_cast<int>(model.variable_proxies().size()));
            EXPECT_EQ(i, x.index());
            EXPECT_EQ(0, x(0, 0).lower_bound());
            EXPECT_EQ(1, x(0, 0).upper_bound());
            EXPECT_TRUE(x(0, 0).has_bounds());
            EXPECT_EQ(model_component::VariableSense::Binary, x(0, 0).sense());
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
TEST_F(TestModel, create_expression_scalar) {
    {
        model::Model<int, double> model;
        for (auto i = 0;
             i < model::ModelConstant::MAX_NUMBER_OF_EXPRESSION_PROXIES; i++) {
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
TEST_F(TestModel, create_expression_one_dimensional) {
    {
        model::Model<int, double> model;
        for (auto i = 0;
             i < model::ModelConstant::MAX_NUMBER_OF_EXPRESSION_PROXIES; i++) {
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
TEST_F(TestModel, create_expression_two_dimensional) {
    {
        model::Model<int, double> model;
        for (auto i = 0;
             i < model::ModelConstant::MAX_NUMBER_OF_EXPRESSION_PROXIES; i++) {
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
TEST_F(TestModel, create_expression_arg_expression_like) {
    {
        model::Model<int, double> model;

        for (auto i = 0;
             i < model::ModelConstant::MAX_NUMBER_OF_EXPRESSION_PROXIES; i++) {
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
TEST_F(TestModel, create_expression_arg_expression) {
    {
        model::Model<int, double> model;

        for (auto i = 0;
             i < model::ModelConstant::MAX_NUMBER_OF_EXPRESSION_PROXIES; i++) {
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
TEST_F(TestModel, create_constraint_scalar) {
    {
        model::Model<int, double> model;
        for (auto i = 0;
             i < model::ModelConstant::MAX_NUMBER_OF_CONSTRAINT_PROXIES; i++) {
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
TEST_F(TestModel, create_constraint_one_dimensional) {
    {
        model::Model<int, double> model;
        for (auto i = 0;
             i < model::ModelConstant::MAX_NUMBER_OF_CONSTRAINT_PROXIES; i++) {
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
TEST_F(TestModel, create_constraint_two_dimensional) {
    {
        model::Model<int, double> model;
        for (auto i = 0;
             i < model::ModelConstant::MAX_NUMBER_OF_CONSTRAINT_PROXIES; i++) {
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
TEST_F(TestModel, create_constraint_arg_constraint) {
    {
        model::Model<int, double> model;

        for (auto i = 0;
             i < model::ModelConstant::MAX_NUMBER_OF_CONSTRAINT_PROXIES; i++) {
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
TEST_F(TestModel, minimize_arg_expression_like) {
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
    model.update();
    EXPECT_EQ(10 + 1, model.objective().value());
}

/*****************************************************************************/
TEST_F(TestModel, minimize_arg_expression) {
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
    model.update();
    EXPECT_EQ(10 + 1, model.objective().value());
}

/*****************************************************************************/
TEST_F(TestModel, maximize_arg_expression_like) {
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
    model.update();
    EXPECT_EQ(10 + 1, model.objective().value());
}

/*****************************************************************************/
TEST_F(TestModel, maximize_arg_expression) {
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
    model.update();
    EXPECT_EQ(10 + 1, model.objective().value());
}

/*****************************************************************************/
TEST_F(TestModel, setup) {
    /// This method is tested in the following submethods.
}

/*****************************************************************************/
TEST_F(TestModel, setup_structure) {
    /// This method is tested in the following submethods.
}

/*****************************************************************************/
TEST_F(TestModel, setup_unique_names) {
    model::Model<int, double> model;

    auto& x = model.create_variable("x");
    auto& p = model.create_variables("p", {10});
    auto& g = model.create_variables("g", {20, 30});

    x.set_name("_x");
    p(0).set_name("_p_0");
    p(9).set_name("_p_9");
    g(0, 0).set_name("_g_0_0");
    g(19, 29).set_name("_g_19_29");
    model.setup_unique_names();

    EXPECT_EQ("_x", x.name());
    EXPECT_EQ("_p_0", p(0).name());
    EXPECT_EQ("p[ 1]", p(1).name());
    EXPECT_EQ("p[ 8]", p(8).name());
    EXPECT_EQ("_p_9", p(9).name());
    EXPECT_EQ("_g_0_0", g(0, 0).name());
    EXPECT_EQ("g[ 0,  1]", g(0, 1).name());
    EXPECT_EQ("g[19, 28]", g(19, 28).name());
    EXPECT_EQ("_g_19_29", g(19, 29).name());
}

/*****************************************************************************/
TEST_F(TestModel, setup_variable_related_constraints) {
    model::Model<int, double> model;

    auto& x = model.create_variables("x", 10, 0, 1);
    auto& y = model.create_variables("y", {20, 30}, 0, 1);

    auto& g = model.create_constraints("g", 3);
    g(0)    = x.selection();
    g(1)    = y.selection();
    g(2)    = x(0) + 2 * y.sum({0, model_component::Range::All}) >= 1;

    model.categorize_constraints();
    model.setup_variable_related_constraints();
    model.setup_variable_related_binary_coefficient_constraints();

    for (auto i = 0; i < 10; i++) {
        EXPECT_TRUE(std::find(x(i).related_constraint_ptrs().begin(),
                              x(i).related_constraint_ptrs().end(),
                              &g(0)) != x(i).related_constraint_ptrs().end());
        EXPECT_FALSE(std::find(x(i).related_constraint_ptrs().begin(),
                               x(i).related_constraint_ptrs().end(),
                               &g(1)) != x(i).related_constraint_ptrs().end());
        /// Only x(0) is related to g(2).
        EXPECT_EQ(i == 0,
                  std::find(x(i).related_constraint_ptrs().begin(),
                            x(i).related_constraint_ptrs().end(),
                            &g(2)) != x(i).related_constraint_ptrs().end());
    }

    for (auto i = 0; i < 20; i++) {
        for (auto j = 0; j < 30; j++) {
            EXPECT_FALSE(std::find(y(i, j).related_constraint_ptrs().begin(),
                                   y(i, j).related_constraint_ptrs().end(),
                                   &g(0)) !=
                         y(i, j).related_constraint_ptrs().end());
            EXPECT_TRUE(std::find(y(i, j).related_constraint_ptrs().begin(),
                                  y(i, j).related_constraint_ptrs().end(),
                                  &g(1)) !=
                        y(i, j).related_constraint_ptrs().end());
            /// Only y(0,*) is related to g(2).
            EXPECT_EQ(
                i == 0,
                std::find(y(i, j).related_constraint_ptrs().begin(),
                          y(i, j).related_constraint_ptrs().end(),
                          &g(2)) != y(i, j).related_constraint_ptrs().end());
        }
    }

    for (auto i = 0; i < 10; i++) {
        EXPECT_TRUE(
            std::find(x(i).related_binary_coefficient_constraint_ptrs().begin(),
                      x(i).related_binary_coefficient_constraint_ptrs().end(),
                      &g(0)) !=
            x(i).related_binary_coefficient_constraint_ptrs().end());
        EXPECT_FALSE(
            std::find(x(i).related_binary_coefficient_constraint_ptrs().begin(),
                      x(i).related_binary_coefficient_constraint_ptrs().end(),
                      &g(1)) !=
            x(i).related_binary_coefficient_constraint_ptrs().end());
    }

    for (auto i = 0; i < 20; i++) {
        for (auto j = 0; j < 30; j++) {
            EXPECT_FALSE(
                std::find(
                    y(i, j)
                        .related_binary_coefficient_constraint_ptrs()
                        .begin(),
                    y(i, j).related_binary_coefficient_constraint_ptrs().end(),
                    &g(0)) !=
                y(i, j).related_binary_coefficient_constraint_ptrs().end());
            EXPECT_TRUE(
                std::find(
                    y(i, j)
                        .related_binary_coefficient_constraint_ptrs()
                        .begin(),
                    y(i, j).related_binary_coefficient_constraint_ptrs().end(),
                    &g(1)) !=
                y(i, j).related_binary_coefficient_constraint_ptrs().end());
            EXPECT_FALSE(
                std::find(
                    y(i, j)
                        .related_binary_coefficient_constraint_ptrs()
                        .begin(),
                    y(i, j).related_binary_coefficient_constraint_ptrs().end(),
                    &g(2)) !=
                y(i, j).related_binary_coefficient_constraint_ptrs().end());
        }
    }
    EXPECT_FALSE(
        std::find(x(0).related_binary_coefficient_constraint_ptrs().begin(),
                  x(0).related_binary_coefficient_constraint_ptrs().end(),
                  &g(2)) !=
        x(0).related_binary_coefficient_constraint_ptrs().end());
}

/*****************************************************************************/
TEST_F(TestModel, setup_variable_objective_sensitivities) {
    model::Model<int, double> model;

    auto& x = model.create_variables("x", 10, 0, 1);
    auto& y = model.create_variables("y", {20, 30}, 0, 1);

    model.minimize(2 * x.sum() + 5 * y.sum());
    model.setup_variable_objective_sensitivities();

    for (auto i = 0; i < 10; i++) {
        EXPECT_EQ(2, x(i).objective_sensitivity());
    }

    for (auto i = 0; i < 20; i++) {
        for (auto j = 0; j < 30; j++) {
            EXPECT_EQ(5, y(i, j).objective_sensitivity());
        }
    }
}

/*****************************************************************************/
TEST_F(TestModel, setup_variable_constraint_sensitivities) {
    model::Model<int, double> model;

    auto& x = model.create_variables("x", 10, 0, 1);
    auto& y = model.create_variables("y", {20, 30}, 0, 1);
    auto& g = model.create_constraints("g", 2);
    auto& p = model.create_expressions("p", 2);
    for (auto i = 0; i < 10; i++) {
        p(0) += (i + 1) * x(i);
    }

    for (auto i = 0; i < 20; i++) {
        for (auto j = 0; j < 30; j++) {
            p(1) += (i + j + 1) * y(i, j);
        }
    }
    g(0) = p(0) <= 10000;
    g(1) = p(1) <= 10000;

    model.setup_variable_constraint_sensitivities();

    for (auto i = 0; i < 10; i++) {
        for (auto&& sensitivity : x(i).constraint_sensitivities()) {
            if (sensitivity.first == &g(0)) {
                EXPECT_EQ(i + 1, sensitivity.second);
            }
        }
    }

    for (auto i = 0; i < 20; i++) {
        for (auto j = 0; j < 30; j++) {
            for (auto&& sensitivity : y(i, j).constraint_sensitivities()) {
                if (sensitivity.first == &g(1)) {
                    EXPECT_EQ(i + j + 1, sensitivity.second);
                }
            }
        }
    }
}

/*****************************************************************************/
TEST_F(TestModel, setup_related_selection_constraint_ptr_index) {
    model::Model<int, double> model;

    auto& x = model.create_variables("x", 4, 0, 1);

    model.create_constraint("selection", x.selection());

    /**
     * To verify the sort process, constraints are defined in the order of c_1
     * -> c_2 -> c_0, which will be sorted in the order of c_0 - >c_1 -> c2.
     */
    auto& c_1 = model.create_constraint("c_1", x(1) + x(2) <= 1);
    auto& c_2 = model.create_constraint("c_2", x(2) + x(3) <= 1);
    auto& c_0 = model.create_constraint("c_0", x(0) + x(1) <= 1);

    model.setup_unique_names();
    model.setup_structure();
    preprocess::SelectionExtractor<int, double> selection_extractor(&model);
    selection_extractor.extract_by_defined_order(false);
    model.setup_structure();
    model.setup_variable_related_selection_constraint_ptr_index();

    EXPECT_EQ(1, static_cast<int>(model.selections().size()));

    EXPECT_EQ(3, static_cast<int>(
                     model.selections()[0].related_constraint_ptrs.size()));
    EXPECT_EQ(&c_0[0], model.selections()[0].related_constraint_ptrs[0]);
    EXPECT_EQ(&c_1[0], model.selections()[0].related_constraint_ptrs[1]);
    EXPECT_EQ(&c_2[0], model.selections()[0].related_constraint_ptrs[2]);

    EXPECT_EQ(0, x(0).related_selection_constraint_ptr_index_min());
    EXPECT_EQ(0, x(0).related_selection_constraint_ptr_index_max());

    EXPECT_EQ(0, x(1).related_selection_constraint_ptr_index_min());
    EXPECT_EQ(1, x(1).related_selection_constraint_ptr_index_max());

    EXPECT_EQ(1, x(2).related_selection_constraint_ptr_index_min());
    EXPECT_EQ(2, x(2).related_selection_constraint_ptr_index_max());

    EXPECT_EQ(2, x(3).related_selection_constraint_ptr_index_min());
    EXPECT_EQ(2, x(3).related_selection_constraint_ptr_index_max());
}

/*****************************************************************************/
TEST_F(TestModel, setup_variable_related_binary_coefficient_constraints) {
    /// This method is tested in setup_variable_related_constraints().
}

/*****************************************************************************/
TEST_F(TestModel, categorize_variables) {
    model::Model<int, double> model;

    auto& x = model.create_variable("x", 0, 1);
    auto& y = model.create_variables("y", 10, 0, 1);
    auto& z = model.create_variables("z", {20, 30}, -10, 10);
    auto& w = model.create_variable("w", -100, 100);

    x.fix_by(0);
    y(0).fix_by(0);
    z(0, 0).fix_by(0);
    model.create_constraint("f", y.selection());
    model.create_constraint("g", w == 2 * x + 3 * z(0, 0));

    model.categorize_variables();
    model.categorize_constraints();

    preprocess::DependentVariableExtractor<int, double>
                   dependent_variable_extractor(&model);
    option::Option option;
    dependent_variable_extractor.extract(option, false);
    preprocess::SelectionExtractor<int, double> selection_extractor(&model);
    selection_extractor.extract_by_independent(false);

    model.categorize_variables();
    model.categorize_constraints();

    EXPECT_EQ(1 + 10 + 20 * 30 + 1, model.number_of_variables());
    EXPECT_EQ(3, model.number_of_fixed_variables());
    EXPECT_EQ(1 + 10 + 20 * 30 + 1 - 3, model.number_of_mutable_variables());
    EXPECT_EQ(1, model.number_of_binary_variables());
    EXPECT_EQ(600, model.number_of_integer_variables());
    EXPECT_EQ(10, model.number_of_selection_variables());
    EXPECT_EQ(1, model.number_of_dependent_integer_variables());
}

/*****************************************************************************/
TEST_F(TestModel, categorize_constraints) {
    model::Model<int, double> model;
    auto                      coefficients = utility::sequence(10, 20);

    auto& x = model.create_variable("x", -10, 10);
    auto& y = model.create_variable("y", -10, 10);
    auto& z = model.create_variables("z", 10, 0, 1);
    auto& w = model.create_variable("w", 0, 1);
    auto& r = model.create_variables("r", 10, -10, 10);

    auto& singleton = model.create_constraint("singleton");
    singleton       = 2 * x <= 10;

    auto& aggregation = model.create_constraint("aggregation");
    aggregation       = 2 * x + 3 * y == 10;

    auto& precedence = model.create_constraints("precedence", 4);
    precedence(0)    = 2 * x - 2 * y <= 5;
    precedence(1)    = -2 * x + 2 * y <= 5;
    precedence(2)    = 2 * x - 2 * y >= 5;
    precedence(3)    = -2 * x + 2 * y >= 5;

    auto& variable_bound = model.create_constraints("variable_bound", 2);
    variable_bound(0)    = 2 * z(0) + 3 * z(1) >= 5;
    variable_bound(1)    = 2 * z(0) + 3 * z(1) <= 5;

    auto& set_partitioning = model.create_constraint("set_partitioning");
    set_partitioning       = z.selection();

    auto& set_packing = model.create_constraint("set_packing");
    set_packing       = z.sum() <= 1;

    auto& set_covering = model.create_constraint("set_covering");
    set_covering       = z.sum() >= 1;

    auto& cardinality = model.create_constraint("cardinality");
    cardinality       = z.sum() == 5;

    auto& invariant_knapsack = model.create_constraint("invariant_knapsack");
    invariant_knapsack       = z.sum() <= 5;

    auto& multiple_covering = model.create_constraint("multiple_covering");
    multiple_covering       = z.sum() >= 5;

    auto& binary_flow = model.create_constraint("binary_flow");
    binary_flow       = z(0) + z(1) + z(2) == z(3) + z(4) + z(5);

    auto& integer_flow = model.create_constraint("integer_flow");
    integer_flow       = r(0) + r(1) + r(2) == r(3) + r(4) + r(5);

    auto& equation_knapsack = model.create_constraint("equation_knapsack");
    equation_knapsack       = z.dot(coefficients) == 30;

    auto& bin_packing = model.create_constraints("bin_packing", 2);
    bin_packing(0)    = z.dot(coefficients) + 5 * w <= 5;
    bin_packing(1)    = -z.dot(coefficients) - 5 * w >= -5;

    auto& knapsack = model.create_constraints("knapsack", 2);
    knapsack(0)    = z.dot(coefficients) <= 50;
    knapsack(1)    = -z.dot(coefficients) >= -50;

    auto& integer_knapsack = model.create_constraints("integer_knapsack", 2);
    integer_knapsack(0)    = r.dot(coefficients) <= 50;
    integer_knapsack(1)    = -r.dot(coefficients) >= -50;

    auto& min_max = model.create_constraint("min_max");
    min_max       = 2 * z(0) + 3 * z(1) <= x;

    auto& max_min = model.create_constraint("max_min");
    max_min       = 2 * z(0) + 3 * z(1) >= x;

    auto& intermediate = model.create_constraint("intermediate");
    intermediate       = 2 * z(0) + 3 * z(1) == x;

    auto& general_liner = model.create_constraint("general_liner");
    general_liner       = 2 * x + 2 * r.sum() == 50;

    singleton.disable();

    model.setup_structure();
    preprocess::SelectionExtractor<int, double> selection_extractor(&model);
    selection_extractor.extract_by_defined_order(false);
    model.setup_structure();

    EXPECT_EQ(27, model.number_of_constraints());
    EXPECT_EQ(1, model.number_of_selection_constraints());
    EXPECT_EQ(25, model.number_of_enabled_constraints());
    EXPECT_EQ(2, model.number_of_disabled_constraints());

    auto reference = model.constraint_type_reference();
    EXPECT_EQ(1, static_cast<int>(reference.singleton_ptrs.size()));
    EXPECT_EQ(1, static_cast<int>(reference.aggregation_ptrs.size()));
    EXPECT_EQ(4, static_cast<int>(reference.precedence_ptrs.size()));
    EXPECT_EQ(2, static_cast<int>(reference.variable_bound_ptrs.size()));
    EXPECT_EQ(1, static_cast<int>(reference.set_partitioning_ptrs.size()));
    EXPECT_EQ(1, static_cast<int>(reference.set_packing_ptrs.size()));
    EXPECT_EQ(1, static_cast<int>(reference.set_covering_ptrs.size()));
    EXPECT_EQ(1, static_cast<int>(reference.invariant_knapsack_ptrs.size()));
    EXPECT_EQ(1, static_cast<int>(reference.multiple_covering_ptrs.size()));
    EXPECT_EQ(1, static_cast<int>(reference.binary_flow_ptrs.size()));
    EXPECT_EQ(1, static_cast<int>(reference.integer_flow_ptrs.size()));
    EXPECT_EQ(1, static_cast<int>(reference.equation_knapsack_ptrs.size()));
    EXPECT_EQ(2, static_cast<int>(reference.bin_packing_ptrs.size()));
    EXPECT_EQ(2, static_cast<int>(reference.knapsack_ptrs.size()));
    EXPECT_EQ(2, static_cast<int>(reference.integer_knapsack_ptrs.size()));
    EXPECT_EQ(1, static_cast<int>(reference.min_max_ptrs.size()));
    EXPECT_EQ(1, static_cast<int>(reference.max_min_ptrs.size()));
    EXPECT_EQ(1, static_cast<int>(reference.intermediate_ptrs.size()));
    EXPECT_EQ(1, static_cast<int>(reference.general_linear_ptrs.size()));
}

/*****************************************************************************/
TEST_F(TestModel, setup_neighborhood) {
    /// This method is tested in test_neighborhood.h
}

/*****************************************************************************/
TEST_F(TestModel, setup_fixed_sensitivities) {
    /// This method is tested in test_expression.h
}

/*****************************************************************************/
TEST_F(TestModel,
       setup_positive_and_negative_coefficient_mutable_variable_ptrs) {
    /// This method is tested in test_expression.h
}

/*****************************************************************************/
TEST_F(TestModel, export_lp_instance) {
    /// This method is tested in test_expression.h
    model::Model<int, double> model;

    auto& x = model.create_variables("x", 4, -10, 10);
    auto& f = model.create_constraints("f", 3);

    f(0) = 2 * x[0] + 3 * x[1] == 10;  // 1
    f(1) = 4 * x[1] + 5 * x[2] <= 20;  // 0
    f(2) = 6 * x[2] + 7 * x[3] >= 30;  // 2

    x[0].reset_bound();
    x[0] = 10;

    model.minimize(x.sum());
    option::Option option;
    option.preprocess.is_enabled_presolve = false;
    model.setup(option, false);

    auto lp_instance = model.export_lp_instance();

    EXPECT_EQ(3, lp_instance.number_of_rows);
    EXPECT_EQ(4, lp_instance.number_of_columns);
    EXPECT_FLOAT_EQ(0.0, lp_instance.objective_offset);
    EXPECT_FLOAT_EQ(1.0, lp_instance.primal_objective_coefficients[0]);
    EXPECT_FLOAT_EQ(1.0, lp_instance.primal_objective_coefficients[1]);
    EXPECT_FLOAT_EQ(1.0, lp_instance.primal_objective_coefficients[2]);
    EXPECT_FLOAT_EQ(1.0, lp_instance.primal_objective_coefficients[3]);

    EXPECT_FLOAT_EQ(printemps::constant::INT_HALF_MIN,
                    lp_instance.primal_lower_bounds[0]);
    EXPECT_FLOAT_EQ(-10, lp_instance.primal_lower_bounds[1]);
    EXPECT_FLOAT_EQ(-10, lp_instance.primal_lower_bounds[2]);
    EXPECT_FLOAT_EQ(-10, lp_instance.primal_lower_bounds[3]);

    EXPECT_FLOAT_EQ(printemps::constant::INT_HALF_MAX,
                    lp_instance.primal_upper_bounds[0]);
    EXPECT_FLOAT_EQ(10, lp_instance.primal_upper_bounds[1]);
    EXPECT_FLOAT_EQ(10, lp_instance.primal_upper_bounds[2]);
    EXPECT_FLOAT_EQ(10, lp_instance.primal_upper_bounds[3]);

    EXPECT_FLOAT_EQ(10, lp_instance.primal_initial_solution[0]);
    EXPECT_FLOAT_EQ(0, lp_instance.primal_initial_solution[1]);
    EXPECT_FLOAT_EQ(0, lp_instance.primal_initial_solution[2]);
    EXPECT_FLOAT_EQ(0, lp_instance.primal_initial_solution[3]);

    EXPECT_FLOAT_EQ(-4, lp_instance.primal_constraint_coefficients.values[0]);
    EXPECT_FLOAT_EQ(-5, lp_instance.primal_constraint_coefficients.values[1]);
    EXPECT_FLOAT_EQ(2, lp_instance.primal_constraint_coefficients.values[2]);
    EXPECT_FLOAT_EQ(3, lp_instance.primal_constraint_coefficients.values[3]);
    EXPECT_FLOAT_EQ(6, lp_instance.primal_constraint_coefficients.values[4]);
    EXPECT_FLOAT_EQ(7, lp_instance.primal_constraint_coefficients.values[5]);

    EXPECT_EQ(1, lp_instance.is_primal_lower_unbounded[0]);
    EXPECT_EQ(0, lp_instance.is_primal_lower_unbounded[3]);
    EXPECT_EQ(1, lp_instance.is_primal_upper_unbounded[0]);
    EXPECT_EQ(0, lp_instance.is_primal_upper_unbounded[3]);

    EXPECT_FLOAT_EQ(-20.0, lp_instance.dual_objective_coefficients[0]);
    EXPECT_FLOAT_EQ(10.0, lp_instance.dual_objective_coefficients[1]);
    EXPECT_FLOAT_EQ(30.0, lp_instance.dual_objective_coefficients[2]);

    EXPECT_FLOAT_EQ(0, lp_instance.dual_lower_bounds[0]);
    EXPECT_FLOAT_EQ(std::numeric_limits<double>::lowest(),
                    lp_instance.dual_lower_bounds[1]);
    EXPECT_FLOAT_EQ(0, lp_instance.dual_lower_bounds[2]);

    EXPECT_FLOAT_EQ(std::numeric_limits<double>::max(),
                    lp_instance.dual_upper_bounds[0]);
    EXPECT_FLOAT_EQ(std::numeric_limits<double>::max(),
                    lp_instance.dual_upper_bounds[1]);
    EXPECT_FLOAT_EQ(std::numeric_limits<double>::max(),
                    lp_instance.dual_upper_bounds[2]);

    EXPECT_FLOAT_EQ(0, lp_instance.dual_initial_solution[0]);
    EXPECT_FLOAT_EQ(0, lp_instance.dual_initial_solution[1]);
    EXPECT_FLOAT_EQ(0, lp_instance.dual_initial_solution[2]);
}

/*****************************************************************************/
TEST_F(TestModel, set_selections) {
    model::Model<int, double> model;
    auto&                     x = model.create_variables("x", 10, 0, 1);
    auto&                     y = model.create_variables("y", 5, 0, 1);
    std::vector<model_component::Selection<int, double>> selections;

    {
        model_component::Selection<int, double> selection;
        for (auto i = 0; i < 10; i++) {
            selection.variable_ptrs.push_back(&(x(i)));
        }
        selections.push_back(selection);
    }

    {
        model_component::Selection<int, double> selection;
        for (auto i = 0; i < 5; i++) {
            selection.variable_ptrs.push_back(&(y(i)));
        }
        selections.push_back(selection);
    }

    model.set_selections(selections);

    for (auto i = 0; i < 10; i++) {
        EXPECT_EQ(&(model.selections()[0]), x(i).selection_ptr());
    }

    for (auto i = 0; i < 5; i++) {
        EXPECT_EQ(&(model.selections()[1]), y(i).selection_ptr());
    }
}

/*****************************************************************************/
TEST_F(TestModel, update_variable_bounds) {
    {
        model::Model<int, double> model;

        auto& x = model.create_variable("x", 0, 200);
        auto& y = model.create_variable("y", 0, 200);
        model.minimize(x + 3 * y);
        model.problem_size_reducer().setup(&model);
        model.update_variable_bounds(100, true, false);

        EXPECT_EQ(100, x(0).upper_bound());
        EXPECT_EQ(33, y(0).upper_bound());
    }

    {
        model::Model<int, double> model;

        auto& x = model.create_variable("x", 0, 200);
        auto& y = model.create_variable("y", 0, 200);
        model.minimize(x - 3 * y);
        model.problem_size_reducer().setup(&model);
        model.update_variable_bounds(100, true, false);

        EXPECT_EQ(200, x(0).upper_bound());
        EXPECT_EQ(200, y(0).upper_bound());
    }

    {
        model::Model<int, double> model;

        auto& x = model.create_variable("x", 0, 200);
        auto& y = model.create_variable("y", 0, 200);
        model.maximize(x + 3 * y);
        model.problem_size_reducer().setup(&model);
        model.update_variable_bounds(100, true, false);

        EXPECT_EQ(0, x(0).lower_bound());
        EXPECT_EQ(0, y(0).lower_bound());
    }

    {
        model::Model<int, double> model;

        auto& x = model.create_variable("x", 0, 200);
        auto& y = model.create_variable("y", 0, 200);
        model.maximize(x - 3 * y);
        model.problem_size_reducer().setup(&model);
        model.update_variable_bounds(100, true, false);

        EXPECT_EQ(100, x(0).lower_bound());
        EXPECT_EQ(0, y(0).lower_bound());
    }
}

/*****************************************************************************/
TEST_F(TestModel, set_global_penalty_coefficient) {
    model::Model<int, double> model;
    model.set_global_penalty_coefficient(1E7);
    EXPECT_FLOAT_EQ(1E7, model.global_penalty_coefficient());
}

/*****************************************************************************/
TEST_F(TestModel, global_penalty_coefficient) {
    /// This method is tested in set_global_penalty_coefficient().
}

/*****************************************************************************/
TEST_F(TestModel, update_arg_void) {
    model::Model<int, double> model;

    auto sequence = utility::sequence(10);

    auto& x = model.create_variables("x", 10, 0, 1);
    auto& p = model.create_expression("p", x.dot(sequence) + 1);
    model.create_constraint("g", x.selection());

    for (auto&& element : x.flat_indexed_variables()) {
        element = 1;
    }
    model.minimize(p);
    model.update();

    // 0 + 1 + 2 + ... + 9 + 1 = 46
    EXPECT_EQ(46, p.value());
    EXPECT_EQ(46, p(0).value());
    EXPECT_EQ(46, model.objective().value());
}

/*****************************************************************************/
TEST_F(TestModel, update_arg_move) {
    model::Model<int, double> model;

    auto sequence = utility::sequence(10);

    auto& x = model.create_variables("x", 10, 0, 1);
    auto& p = model.create_expression("p", x.dot(sequence) + 1);
    model.create_constraint("g", x.selection());

    x(0) = 1;

    model.minimize(p);
    model.setup_structure();
    preprocess::SelectionExtractor<int, double> selection_extractor(&model);
    selection_extractor.extract_by_independent(false);
    model.setup_fixed_sensitivities(false);

    model.update();

    neighborhood::Move<int, double> move;
    move.sense = neighborhood::MoveSense::Selection;
    move.alterations.emplace_back(&x(0), 0);
    move.alterations.emplace_back(&x(9), 1);

    model.update(move);

    EXPECT_EQ(10, p.value());
    EXPECT_EQ(10, p(0).value());
    EXPECT_EQ(10, model.objective().value());
    EXPECT_EQ(&x(9), model.selections().front().selected_variable_ptr);
}

/*****************************************************************************/
TEST_F(TestModel, reset_variable_objective_improvabilities_arg_void) {
    model::Model<int, double> model;

    auto& x = model.create_variable("x", 0, 1);
    auto& y = model.create_variables("y", 10, 0, 1);
    model.setup_structure();

    x(0).set_is_objective_improvable(true);
    EXPECT_TRUE(x(0).is_objective_improvable());
    for (auto i = 0; i < 10; i++) {
        y(i).set_is_objective_improvable(true);
        EXPECT_TRUE(y(i).is_objective_improvable());
    }
    model.reset_variable_objective_improvabilities();

    EXPECT_FALSE(x(0).is_objective_improvable());
    for (auto i = 0; i < 10; i++) {
        EXPECT_FALSE(y(i).is_objective_improvable());
    }
}

/*****************************************************************************/
TEST_F(TestModel, reset_variable_objective_improvabilities_arg_variable_ptrs) {
    model::Model<int, double> model;

    auto& x = model.create_variable("x", 0, 1);
    auto& y = model.create_variables("y", 10, 0, 1);
    model.setup_structure();

    x(0).set_is_objective_improvable(true);
    EXPECT_TRUE(x(0).is_objective_improvable());
    for (auto i = 0; i < 10; i++) {
        y(i).set_is_objective_improvable(true);
        EXPECT_TRUE(y(i).is_objective_improvable());
    }
    model.reset_variable_objective_improvabilities({&x(0), &y(0), &y(9)});

    EXPECT_FALSE(x(0).is_objective_improvable());
    EXPECT_FALSE(y(0).is_objective_improvable());
    EXPECT_FALSE(y(9).is_objective_improvable());
    for (auto i = 1; i < 9; i++) {
        EXPECT_TRUE(y(i).is_objective_improvable());
    }
}

/*****************************************************************************/
TEST_F(TestModel, reset_variable_feasibility_improvabilities_arg_void) {
    model::Model<int, double> model;

    auto& x = model.create_variable("x", 0, 1);
    auto& y = model.create_variables("y", 10, 0, 1);
    model.setup_structure();

    x(0).set_is_feasibility_improvable(true);
    EXPECT_TRUE(x(0).is_feasibility_improvable());
    for (auto i = 0; i < 10; i++) {
        y(i).set_is_feasibility_improvable(true);
        EXPECT_TRUE(y(i).is_feasibility_improvable());
    }
    model.reset_variable_feasibility_improvabilities();

    EXPECT_FALSE(x(0).is_feasibility_improvable());
    for (auto i = 0; i < 10; i++) {
        EXPECT_FALSE(y(i).is_feasibility_improvable());
    }
}

/*****************************************************************************/
TEST_F(TestModel,
       reset_variable_feasibility_improvabilities_arg_variable_ptrs) {
    model::Model<int, double> model;

    auto& x = model.create_variable("x", 0, 1);
    auto& y = model.create_variables("y", 10, 0, 1);
    model.setup_structure();

    x(0).set_is_feasibility_improvable(true);
    EXPECT_TRUE(x(0).is_feasibility_improvable());
    for (auto i = 0; i < 10; i++) {
        y(i).set_is_feasibility_improvable(true);
        EXPECT_TRUE(y(i).is_feasibility_improvable());
    }
    model.reset_variable_feasibility_improvabilities({&x(0), &y(0), &y(9)});

    EXPECT_FALSE(x(0).is_feasibility_improvable());
    EXPECT_FALSE(y(0).is_feasibility_improvable());
    EXPECT_FALSE(y(9).is_feasibility_improvable());
    for (auto i = 1; i < 9; i++) {
        EXPECT_TRUE(y(i).is_feasibility_improvable());
    }
}

/*****************************************************************************/
TEST_F(TestModel,
       reset_variable_feasibility_improvabilities_arg_constraint_ptrs) {
    model::Model<int, double> model;

    auto& x = model.create_variable("x", 0, 1);
    auto& y = model.create_variables("y", 10, 0, 1);
    auto& g = model.create_constraints("g", 2);

    g(0) = x <= y(0);
    g(1) = y(1) == y(9);

    model.setup_structure();

    x(0).set_is_feasibility_improvable(true);
    EXPECT_TRUE(x(0).is_feasibility_improvable());
    for (auto i = 0; i < 10; i++) {
        y(i).set_is_feasibility_improvable(true);
        EXPECT_TRUE(y(i).is_feasibility_improvable());
    }
    std::vector<model_component::Constraint<int, double>*> constraint_ptrs = {
        &g(0), &g(1)};
    model.reset_variable_feasibility_improvabilities(constraint_ptrs);

    EXPECT_FALSE(x(0).is_feasibility_improvable());
    EXPECT_FALSE(y(0).is_feasibility_improvable());
    EXPECT_FALSE(y(1).is_feasibility_improvable());
    EXPECT_FALSE(y(9).is_feasibility_improvable());
    for (auto i = 2; i < 9; i++) {
        EXPECT_TRUE(y(i).is_feasibility_improvable());
    }
}

/*****************************************************************************/
TEST_F(TestModel, update_variable_improvability) {
    {
        model::Model<int, double> model;

        auto& x = model.create_variable("x", -10, 10);
        auto& y = model.create_variable("y", -10, 10);

        [[maybe_unused]] auto& g = model.create_constraint("g", x - y <= 0);

        model.minimize(-x + y);
        model.setup_structure();
        model.setup_fixed_sensitivities(false);
        model.setup_positive_and_negative_coefficient_mutable_variable_ptrs();

        x = -10;
        y = -10;
        model.update();
        model.update_variable_objective_improvabilities();
        model.update_variable_feasibility_improvabilities();
        EXPECT_TRUE(x(0).is_objective_improvable());
        EXPECT_FALSE(y(0).is_objective_improvable());
        EXPECT_FALSE(x(0).is_feasibility_improvable());
        EXPECT_FALSE(y(0).is_feasibility_improvable());

        x = 10;
        y = 10;
        model.update();
        model.update_variable_objective_improvabilities();
        model.update_variable_feasibility_improvabilities();
        EXPECT_FALSE(x(0).is_objective_improvable());
        EXPECT_TRUE(y(0).is_objective_improvable());
        EXPECT_FALSE(x(0).is_feasibility_improvable());
        EXPECT_FALSE(y(0).is_feasibility_improvable());

        x = 10;
        y = -10;
        model.update();
        model.update_variable_objective_improvabilities();
        model.update_variable_feasibility_improvabilities();
        EXPECT_FALSE(x(0).is_objective_improvable());
        EXPECT_FALSE(y(0).is_objective_improvable());
        EXPECT_TRUE(x(0).is_feasibility_improvable());
        EXPECT_TRUE(y(0).is_feasibility_improvable());
    }
    {
        model::Model<int, double> model;

        auto& x = model.create_variable("x", -10, 10);
        auto& y = model.create_variable("y", -10, 10);

        [[maybe_unused]] auto& g = model.create_constraint("g", x - y == 0);

        model.minimize(-x + y);
        model.setup_structure();
        model.setup_fixed_sensitivities(false);
        model.setup_positive_and_negative_coefficient_mutable_variable_ptrs();

        x = -10;
        y = -10;
        model.update();
        model.update_variable_objective_improvabilities();
        model.update_variable_feasibility_improvabilities();
        EXPECT_TRUE(x(0).is_objective_improvable());
        EXPECT_FALSE(y(0).is_objective_improvable());
        EXPECT_FALSE(x(0).is_feasibility_improvable());
        EXPECT_FALSE(y(0).is_feasibility_improvable());

        x = 10;
        y = 10;
        model.update();
        model.update_variable_objective_improvabilities();
        model.update_variable_feasibility_improvabilities();
        EXPECT_FALSE(x(0).is_objective_improvable());
        EXPECT_TRUE(y(0).is_objective_improvable());
        EXPECT_FALSE(x(0).is_feasibility_improvable());
        EXPECT_FALSE(y(0).is_feasibility_improvable());

        x = 10;
        y = -10;
        model.update();
        model.update_variable_objective_improvabilities();
        model.update_variable_feasibility_improvabilities();
        EXPECT_FALSE(x(0).is_objective_improvable());
        EXPECT_FALSE(y(0).is_objective_improvable());
        EXPECT_TRUE(x(0).is_feasibility_improvable());
        EXPECT_TRUE(y(0).is_feasibility_improvable());
    }
    {
        model::Model<int, double> model;

        auto& x = model.create_variable("x", -10, 10);
        auto& y = model.create_variable("y", -10, 10);

        [[maybe_unused]] auto& g = model.create_constraint("g", x - y >= 0);

        model.minimize(-x + y);
        model.setup_structure();
        model.setup_fixed_sensitivities(false);
        model.setup_positive_and_negative_coefficient_mutable_variable_ptrs();

        x = -10;
        y = -10;
        model.update();
        model.update_variable_objective_improvabilities();
        model.update_variable_feasibility_improvabilities();
        EXPECT_TRUE(x(0).is_objective_improvable());
        EXPECT_FALSE(y(0).is_objective_improvable());
        EXPECT_FALSE(x(0).is_feasibility_improvable());
        EXPECT_FALSE(y(0).is_feasibility_improvable());

        x = 10;
        y = 10;
        model.update();
        model.update_variable_objective_improvabilities();
        model.update_variable_feasibility_improvabilities();
        EXPECT_FALSE(x(0).is_objective_improvable());
        EXPECT_TRUE(y(0).is_objective_improvable());
        EXPECT_FALSE(x(0).is_feasibility_improvable());
        EXPECT_FALSE(y(0).is_feasibility_improvable());

        x = -10;
        y = 10;
        model.update();
        model.update_variable_objective_improvabilities();
        model.update_variable_feasibility_improvabilities();
        EXPECT_TRUE(x(0).is_objective_improvable());
        EXPECT_TRUE(y(0).is_objective_improvable());
        EXPECT_TRUE(x(0).is_feasibility_improvable());
        EXPECT_TRUE(y(0).is_feasibility_improvable());
    }
    {
        model::Model<int, double> model;

        auto& x = model.create_variable("x", -10, 10);
        auto& y = model.create_variable("y", -10, 10);

        [[maybe_unused]] auto& g = model.create_constraint("g", x - y <= 0);

        model.maximize(-x + y);
        model.setup_structure();
        model.setup_fixed_sensitivities(false);
        model.setup_positive_and_negative_coefficient_mutable_variable_ptrs();

        x = -10;
        y = -10;

        model.update();
        model.update_variable_objective_improvabilities();
        model.update_variable_feasibility_improvabilities();
        EXPECT_FALSE(x(0).is_objective_improvable());
        EXPECT_TRUE(y(0).is_objective_improvable());
        EXPECT_FALSE(x(0).is_feasibility_improvable());
        EXPECT_FALSE(y(0).is_feasibility_improvable());

        x = 10;
        y = 10;
        model.update();
        model.update_variable_objective_improvabilities();
        model.update_variable_feasibility_improvabilities();
        EXPECT_TRUE(x(0).is_objective_improvable());
        EXPECT_FALSE(y(0).is_objective_improvable());
        EXPECT_FALSE(x(0).is_feasibility_improvable());
        EXPECT_FALSE(y(0).is_feasibility_improvable());

        x = 10;
        y = -10;
        model.update();
        model.update_variable_objective_improvabilities();
        model.update_variable_feasibility_improvabilities();
        EXPECT_TRUE(x(0).is_objective_improvable());
        EXPECT_TRUE(y(0).is_objective_improvable());
        EXPECT_TRUE(x(0).is_feasibility_improvable());
        EXPECT_TRUE(y(0).is_feasibility_improvable());
    }
    {
        model::Model<int, double> model;

        auto& x = model.create_variable("x", -10, 10);
        auto& y = model.create_variable("y", -10, 10);

        [[maybe_unused]] auto& g = model.create_constraint("g", x - y == 0);

        model.maximize(-x + y);
        model.setup_structure();
        model.setup_fixed_sensitivities(false);
        model.setup_positive_and_negative_coefficient_mutable_variable_ptrs();

        x = -10;
        y = -10;

        model.update();
        model.update_variable_objective_improvabilities();
        model.update_variable_feasibility_improvabilities();
        EXPECT_FALSE(x(0).is_objective_improvable());
        EXPECT_TRUE(y(0).is_objective_improvable());
        EXPECT_FALSE(x(0).is_feasibility_improvable());
        EXPECT_FALSE(y(0).is_feasibility_improvable());

        x = 10;
        y = 10;
        model.update();
        model.update_variable_objective_improvabilities();
        model.update_variable_feasibility_improvabilities();
        EXPECT_TRUE(x(0).is_objective_improvable());
        EXPECT_FALSE(y(0).is_objective_improvable());
        EXPECT_FALSE(x(0).is_feasibility_improvable());
        EXPECT_FALSE(y(0).is_feasibility_improvable());

        x = 10;
        y = -10;
        model.update();
        model.update_variable_objective_improvabilities();
        model.update_variable_feasibility_improvabilities();
        EXPECT_TRUE(x(0).is_objective_improvable());
        EXPECT_TRUE(y(0).is_objective_improvable());
        EXPECT_TRUE(x(0).is_feasibility_improvable());
        EXPECT_TRUE(y(0).is_feasibility_improvable());
    }
    {
        model::Model<int, double> model;

        auto& x = model.create_variable("x", -10, 10);
        auto& y = model.create_variable("y", -10, 10);

        [[maybe_unused]] auto& g = model.create_constraint("g", x - y >= 0);

        model.maximize(-x + y);
        model.setup_structure();
        model.setup_fixed_sensitivities(false);
        model.setup_positive_and_negative_coefficient_mutable_variable_ptrs();

        x = -10;
        y = -10;

        model.update();
        model.update_variable_objective_improvabilities();
        model.update_variable_feasibility_improvabilities();
        EXPECT_FALSE(x(0).is_objective_improvable());
        EXPECT_TRUE(y(0).is_objective_improvable());
        EXPECT_FALSE(x(0).is_feasibility_improvable());
        EXPECT_FALSE(y(0).is_feasibility_improvable());

        x = 10;
        y = 10;
        model.update();
        model.update_variable_objective_improvabilities();
        model.update_variable_feasibility_improvabilities();
        EXPECT_TRUE(x(0).is_objective_improvable());
        EXPECT_FALSE(y(0).is_objective_improvable());
        EXPECT_FALSE(x(0).is_feasibility_improvable());
        EXPECT_FALSE(y(0).is_feasibility_improvable());

        x = -10;
        y = 10;
        model.update();
        model.update_variable_objective_improvabilities();
        model.update_variable_feasibility_improvabilities();
        EXPECT_FALSE(x(0).is_objective_improvable());
        EXPECT_FALSE(y(0).is_objective_improvable());
        EXPECT_TRUE(x(0).is_feasibility_improvable());
        EXPECT_TRUE(y(0).is_feasibility_improvable());
    }
}

/*****************************************************************************/
TEST_F(TestModel, update_violative_constraint_ptrs_and_feasibility) {
    model::Model<int, double> model;

    auto&                  x = model.create_variable("x", 0, 10);
    [[maybe_unused]] auto& g = model.create_constraint("g", x <= 5);

    model.setup_structure();

    x = 4;
    model.update();  // include update_feasibility()
    EXPECT_TRUE(model.is_feasible());
    EXPECT_TRUE(model.current_violative_constraint_ptrs().empty());

    x = 5;
    model.update();
    EXPECT_TRUE(model.is_feasible());
    EXPECT_TRUE(model.current_violative_constraint_ptrs().empty());

    x = 6;
    model.update();
    EXPECT_FALSE(model.is_feasible());
    EXPECT_EQ(
        1, static_cast<int>(model.current_violative_constraint_ptrs().size()));

    x = 5;
    model.update();
    EXPECT_TRUE(model.is_feasible());
    EXPECT_EQ(
        1, static_cast<int>(model.previous_violative_constraint_ptrs().size()));
    EXPECT_TRUE(model.current_violative_constraint_ptrs().empty());
}

/*****************************************************************************/
TEST_F(TestModel, evaluate) {
    /// minimize
    {
        model::Model<int, double> model;

        auto sequence = utility::sequence(10);

        auto& x = model.create_variables("x", 10, 0, 1);
        auto& p = model.create_expression("p", x.dot(sequence) + 1);
        [[maybe_unused]] auto& g = model.create_constraint("g", x.sum() <= 5);
        [[maybe_unused]] auto& h =
            model.create_constraint("h", x(0) + x(1) <= 1);

        g(0).local_penalty_coefficient_less()    = 100;
        g(0).local_penalty_coefficient_greater() = 100;
        h(0).local_penalty_coefficient_less()    = 100;
        h(0).local_penalty_coefficient_greater() = 100;

        model.minimize(p);
        model.set_global_penalty_coefficient(10000);
        model.setup_unique_names();
        model.setup_structure();
        preprocess::SelectionExtractor<int, double> selection_extractor(&model);
        selection_extractor.extract_by_independent(false);
        model.setup_fixed_sensitivities(false);

        for (auto&& element : x.flat_indexed_variables()) {
            element = 1;
        }

        model.update();
        auto score_before = model.evaluate({});

        {
            neighborhood::Move<int, double> move;
            for (auto&& element : x.flat_indexed_variables()) {
                move.alterations.emplace_back(&element, 1);
                for (auto&& constraint_ptr :
                     element.related_constraint_ptrs()) {
                    move.related_constraint_ptrs.push_back(constraint_ptr);
                }
            }
            move.sort_and_unique_related_constraint_ptrs();

            auto score_after_0 = model.evaluate(move);
            auto score_after_1 = model.evaluate(move, score_before);

            EXPECT_EQ(46, score_after_0.objective);
            EXPECT_EQ(5 + 1, score_after_0.total_violation);
            EXPECT_EQ(5 * 100 + 100, score_after_0.local_penalty);
            EXPECT_EQ(5 * 10000 + 10000, score_after_0.global_penalty);
            EXPECT_EQ(46 + 5 * 100 + 100,
                      score_after_0.local_augmented_objective);
            EXPECT_EQ(46 + 5 * 10000 + 10000,
                      score_after_0.global_augmented_objective);
            EXPECT_FALSE(score_after_0.is_objective_improvable);
            EXPECT_FALSE(score_after_0.is_feasibility_improvable);
            EXPECT_FALSE(score_after_0.is_feasible);

            EXPECT_EQ(46, score_after_1.objective);
            EXPECT_EQ(5 + 1, score_after_1.total_violation);
            EXPECT_EQ(5 * 100 + 100, score_after_1.local_penalty);
            EXPECT_EQ(5 * 10000 + 10000, score_after_1.global_penalty);
            EXPECT_EQ(46 + 5 * 100 + 100,
                      score_after_1.local_augmented_objective);
            EXPECT_EQ(46 + 5 * 10000 + 10000,
                      score_after_1.global_augmented_objective);
            EXPECT_FALSE(score_after_1.is_objective_improvable);
            EXPECT_FALSE(score_after_1.is_feasibility_improvable);
            EXPECT_FALSE(score_after_1.is_feasible);

            model.update(move);
            score_before = score_after_1;
        }

        {
            neighborhood::Move<int, double> move;
            for (auto&& element : x.flat_indexed_variables()) {
                move.alterations.emplace_back(&element, 0);
                for (auto&& constraint_ptr :
                     element.related_constraint_ptrs()) {
                    move.related_constraint_ptrs.push_back(constraint_ptr);
                }
            }
            move.sort_and_unique_related_constraint_ptrs();

            auto score_after_0 = model.evaluate(move);
            auto score_after_1 = model.evaluate(move, score_before);

            EXPECT_EQ(1, score_after_0.objective);
            EXPECT_EQ(0, score_after_0.total_violation);
            EXPECT_EQ(0, score_after_0.local_penalty);
            EXPECT_EQ(0, score_after_0.global_penalty);
            EXPECT_EQ(1, score_after_0.local_augmented_objective);
            EXPECT_EQ(1, score_after_0.global_augmented_objective);
            EXPECT_TRUE(score_after_0.is_objective_improvable);
            EXPECT_TRUE(score_after_0.is_feasibility_improvable);
            EXPECT_TRUE(score_after_0.is_feasible);

            EXPECT_EQ(1, score_after_1.objective);
            EXPECT_EQ(0, score_after_1.total_violation);
            EXPECT_EQ(0, score_after_1.local_penalty);
            EXPECT_EQ(0, score_after_1.global_penalty);
            EXPECT_EQ(1, score_after_1.local_augmented_objective);
            EXPECT_EQ(1, score_after_1.global_augmented_objective);
            EXPECT_TRUE(score_after_1.is_objective_improvable);
            EXPECT_TRUE(score_after_1.is_feasibility_improvable);
            EXPECT_TRUE(score_after_1.is_feasible);

            model.update(move);
            score_before = score_after_1;
        }

        {
            neighborhood::Move<int, double> move;
            for (auto i = 0; i < 5; i++) {
                move.alterations.emplace_back(&x(i), 1);
                for (auto&& constraint_ptr : x(i).related_constraint_ptrs()) {
                    move.related_constraint_ptrs.push_back(constraint_ptr);
                }
            }
            move.sort_and_unique_related_constraint_ptrs();

            auto score_after_0 = model.evaluate(move);
            auto score_after_1 = model.evaluate(move, score_before);

            EXPECT_EQ(11, score_after_0.objective);
            EXPECT_EQ(1, score_after_0.total_violation);
            EXPECT_EQ(100, score_after_0.local_penalty);
            EXPECT_EQ(10000, score_after_0.global_penalty);
            EXPECT_EQ(11 + 100, score_after_0.local_augmented_objective);
            EXPECT_EQ(11 + 10000, score_after_0.global_augmented_objective);
            EXPECT_FALSE(score_after_0.is_objective_improvable);
            EXPECT_FALSE(score_after_0.is_feasibility_improvable);
            EXPECT_FALSE(score_after_0.is_feasible);

            EXPECT_EQ(11, score_after_1.objective);
            EXPECT_EQ(1, score_after_1.total_violation);
            EXPECT_EQ(100, score_after_1.local_penalty);
            EXPECT_EQ(10000, score_after_1.global_penalty);
            EXPECT_EQ(11 + 100, score_after_1.local_augmented_objective);
            EXPECT_EQ(11 + 10000, score_after_1.global_augmented_objective);
            EXPECT_FALSE(score_after_1.is_objective_improvable);
            EXPECT_FALSE(score_after_1.is_feasibility_improvable);
            EXPECT_FALSE(score_after_1.is_feasible);

            model.update(move);
            score_before = score_after_1;
        }

        {
            /// Single
            neighborhood::Move<int, double> move;
            move.alterations.emplace_back(&x(1), 0);
            move.related_constraint_ptrs = x(1).related_constraint_ptrs();

            auto score_after_0 = model.evaluate(move);
            auto score_after_1 = model.evaluate(move, score_before);

            EXPECT_EQ(10, score_after_0.objective);
            EXPECT_EQ(0, score_after_0.total_violation);
            EXPECT_EQ(0, score_after_0.local_penalty);
            EXPECT_EQ(0, score_after_0.global_penalty);
            EXPECT_EQ(10, score_after_0.local_augmented_objective);
            EXPECT_EQ(10, score_after_0.global_augmented_objective);
            EXPECT_TRUE(score_after_0.is_objective_improvable);
            EXPECT_TRUE(score_after_0.is_feasibility_improvable);
            EXPECT_TRUE(score_after_0.is_feasible);

            EXPECT_EQ(10, score_after_1.objective);
            EXPECT_EQ(0, score_after_1.total_violation);
            EXPECT_EQ(0, score_after_1.local_penalty);
            EXPECT_EQ(0, score_after_1.global_penalty);
            EXPECT_EQ(10, score_after_1.local_augmented_objective);
            EXPECT_EQ(10, score_after_1.global_augmented_objective);
            EXPECT_TRUE(score_after_1.is_objective_improvable);
            EXPECT_TRUE(score_after_1.is_feasibility_improvable);
            EXPECT_TRUE(score_after_1.is_feasible);

            model.update(move);
            score_before = score_after_1;
        }
    }

    /// maximize
    {
        model::Model<int, double> model;

        auto sequence = utility::sequence(10);

        auto& x = model.create_variables("x", 10, 0, 1);
        auto& p = model.create_expression("p", x.dot(sequence) + 1);
        [[maybe_unused]] auto& g = model.create_constraint("g", x.sum() <= 5);
        [[maybe_unused]] auto& h =
            model.create_constraint("h", x(0) + x(1) <= 1);

        g(0).local_penalty_coefficient_less()    = 100;
        g(0).local_penalty_coefficient_greater() = 100;
        h(0).local_penalty_coefficient_less()    = 100;
        h(0).local_penalty_coefficient_greater() = 100;

        model.maximize(p);
        model.set_global_penalty_coefficient(10000);
        model.setup_unique_names();
        model.setup_structure();
        preprocess::SelectionExtractor<int, double> selection_extractor(&model);
        selection_extractor.extract_by_independent(false);
        model.setup_fixed_sensitivities(false);

        for (auto&& element : x.flat_indexed_variables()) {
            element = 1;
        }
        model.update();
        auto score_before = model.evaluate({});

        {
            neighborhood::Move<int, double> move;
            for (auto&& element : x.flat_indexed_variables()) {
                move.alterations.emplace_back(&element, 1);
                for (auto&& constraint_ptr :
                     element.related_constraint_ptrs()) {
                    move.related_constraint_ptrs.push_back(constraint_ptr);
                }
            }
            move.sort_and_unique_related_constraint_ptrs();

            auto score_after_0 = model.evaluate(move);
            auto score_after_1 = model.evaluate(move, score_before);

            EXPECT_EQ(-46, score_after_0.objective);
            EXPECT_EQ(5 + 1, score_after_0.total_violation);
            EXPECT_EQ(5 * 100 + 100, score_after_0.local_penalty);
            EXPECT_EQ(5 * 10000 + 10000, score_after_0.global_penalty);
            EXPECT_EQ(-46 + 5 * 100 + 100,
                      score_after_0.local_augmented_objective);
            EXPECT_EQ(-46 + 5 * 10000 + 10000,
                      score_after_0.global_augmented_objective);
            EXPECT_FALSE(score_after_0.is_objective_improvable);
            EXPECT_FALSE(score_after_0.is_feasibility_improvable);
            EXPECT_FALSE(score_after_0.is_feasible);

            EXPECT_EQ(-46, score_after_1.objective);
            EXPECT_EQ(5 + 1, score_after_1.total_violation);
            EXPECT_EQ(5 * 100 + 100, score_after_1.local_penalty);
            EXPECT_EQ(5 * 10000 + 10000, score_after_1.global_penalty);
            EXPECT_EQ(-46 + 5 * 100 + 100,
                      score_after_1.local_augmented_objective);
            EXPECT_EQ(-46 + 5 * 10000 + 10000,
                      score_after_1.global_augmented_objective);
            EXPECT_FALSE(score_after_1.is_objective_improvable);
            EXPECT_FALSE(score_after_1.is_feasibility_improvable);
            EXPECT_FALSE(score_after_1.is_feasible);

            model.update(move);
            score_before = score_after_1;
        }
        {
            neighborhood::Move<int, double> move;
            for (auto&& element : x.flat_indexed_variables()) {
                move.alterations.emplace_back(&element, 0);
                for (auto&& constraint_ptr :
                     element.related_constraint_ptrs()) {
                    move.related_constraint_ptrs.push_back(constraint_ptr);
                }
            }
            move.sort_and_unique_related_constraint_ptrs();

            auto score_after_0 = model.evaluate(move);
            auto score_after_1 = model.evaluate(move, score_before);

            EXPECT_EQ(-1, score_after_0.objective);
            EXPECT_EQ(0, score_after_0.total_violation);
            EXPECT_EQ(0, score_after_0.local_penalty);
            EXPECT_EQ(0, score_after_0.global_penalty);
            EXPECT_EQ(-1, score_after_0.local_augmented_objective);
            EXPECT_EQ(-1, score_after_0.global_augmented_objective);
            EXPECT_FALSE(score_after_0.is_objective_improvable);
            EXPECT_TRUE(score_after_0.is_feasibility_improvable);
            EXPECT_TRUE(score_after_0.is_feasible);

            EXPECT_EQ(-1, score_after_1.objective);
            EXPECT_EQ(0, score_after_0.total_violation);
            EXPECT_EQ(0, score_after_1.local_penalty);
            EXPECT_EQ(0, score_after_1.global_penalty);
            EXPECT_EQ(-1, score_after_1.local_augmented_objective);
            EXPECT_EQ(-1, score_after_1.global_augmented_objective);
            EXPECT_FALSE(score_after_1.is_objective_improvable);
            EXPECT_TRUE(score_after_1.is_feasibility_improvable);
            EXPECT_TRUE(score_after_1.is_feasible);

            model.update(move);
            score_before = score_after_1;
        }

        {
            neighborhood::Move<int, double> move;
            for (auto i = 0; i < 5; i++) {
                move.alterations.emplace_back(&x(i), 1);
                for (auto&& constraint_ptr : x(i).related_constraint_ptrs()) {
                    move.related_constraint_ptrs.push_back(constraint_ptr);
                }
            }
            move.sort_and_unique_related_constraint_ptrs();

            auto score_after_0 = model.evaluate(move);
            auto score_after_1 = model.evaluate(move, score_before);

            EXPECT_EQ(-11, score_after_0.objective);
            EXPECT_EQ(1, score_after_0.total_violation);
            EXPECT_EQ(100, score_after_0.local_penalty);
            EXPECT_EQ(10000, score_after_0.global_penalty);
            EXPECT_EQ(-11 + 100, score_after_0.local_augmented_objective);
            EXPECT_EQ(-11 + 10000, score_after_0.global_augmented_objective);
            EXPECT_TRUE(score_after_0.is_objective_improvable);
            EXPECT_FALSE(score_after_0.is_feasibility_improvable);
            EXPECT_FALSE(score_after_0.is_feasible);

            EXPECT_EQ(-11, score_after_1.objective);
            EXPECT_EQ(1, score_after_1.total_violation);
            EXPECT_EQ(100, score_after_1.local_penalty);
            EXPECT_EQ(10000, score_after_1.global_penalty);
            EXPECT_EQ(-11 + 100, score_after_1.local_augmented_objective);
            EXPECT_EQ(-11 + 10000, score_after_1.global_augmented_objective);
            EXPECT_TRUE(score_after_1.is_objective_improvable);
            EXPECT_FALSE(score_after_1.is_feasibility_improvable);
            EXPECT_FALSE(score_after_1.is_feasible);

            model.update(move);
            score_before = score_after_1;
        }

        {
            /// Single
            neighborhood::Move<int, double> move;
            move.alterations.emplace_back(&x(1), 0);
            move.related_constraint_ptrs = x(1).related_constraint_ptrs();

            auto score_after_0 = model.evaluate(move);
            auto score_after_1 = model.evaluate(move, score_before);

            EXPECT_EQ(-10, score_after_0.objective);
            EXPECT_EQ(0, score_after_0.total_violation);
            EXPECT_EQ(0, score_after_0.local_penalty);
            EXPECT_EQ(0, score_after_0.global_penalty);
            EXPECT_EQ(-10, score_after_0.local_augmented_objective);
            EXPECT_EQ(-10, score_after_0.global_augmented_objective);
            EXPECT_FALSE(score_after_0.is_objective_improvable);
            EXPECT_TRUE(score_after_0.is_feasibility_improvable);
            EXPECT_TRUE(score_after_0.is_feasible);

            EXPECT_EQ(-10, score_after_1.objective);
            EXPECT_EQ(0, score_after_1.total_violation);
            EXPECT_EQ(0, score_after_1.local_penalty);
            EXPECT_EQ(0, score_after_1.global_penalty);
            EXPECT_EQ(-10, score_after_1.local_augmented_objective);
            EXPECT_EQ(-10, score_after_1.global_augmented_objective);
            EXPECT_FALSE(score_after_1.is_objective_improvable);
            EXPECT_TRUE(score_after_1.is_feasibility_improvable);
            EXPECT_TRUE(score_after_1.is_feasible);

            model.update(move);
            score_before = score_after_1;
        }
    }
}

/*****************************************************************************/
TEST_F(TestModel, compute_lagrangian) {
    model::Model<int, double> model;

    auto sequence = utility::sequence(10);

    auto& x = model.create_variables("x", 10, 0, 1);
    auto& p = model.create_expression("p", x.dot(sequence) + 1);
    [[maybe_unused]] auto& g = model.create_constraint("g", x.sum() <= 5);
    [[maybe_unused]] auto& h = model.create_constraint("h", x(0) + x(1) <= 1);

    model.minimize(p);
    model.setup_structure();

    multi_array::ValueProxy<double> dual_value_proxy(1);
    dual_value_proxy.value() = 100;

    std::vector<multi_array::ValueProxy<double>> dual = {dual_value_proxy,
                                                         dual_value_proxy};

    for (auto&& element : x.flat_indexed_variables()) {
        element = 1;
    }

    model.update();
    auto lagrangian = model.compute_lagrangian(dual);

    EXPECT_EQ(46 + 100 * (10 - 5) + 100 * (2 - 1), lagrangian);
}

/*****************************************************************************/
TEST_F(TestModel, compute_naive_dual_bound) {
    {
        model::Model<int, double> model;

        auto& x = model.create_variables("x", 10, -1, 1);

        model.minimize(x.sum() + 1);
        model.setup_structure();

        EXPECT_EQ(-10 + 1, model.compute_naive_dual_bound());
    }

    {
        model::Model<int, double> model;

        auto& x = model.create_variables("x", 10, -1, 1);

        model.maximize(x.sum() + 1);
        model.setup_structure();

        EXPECT_EQ(10 + 1, model.compute_naive_dual_bound());
    }
}

/*****************************************************************************/
TEST_F(TestModel, generate_variable_parameter_proxies) {
    model::Model<int, double> model;

    auto& x = model.create_variable("x");
    auto& y = model.create_variables("y", 10);
    auto& z = model.create_variables("z", {10, 10});

    int fill_value = random_integer();
    model.setup_unique_names();

    auto parameter_proxies =
        model.generate_variable_parameter_proxies(fill_value);
    EXPECT_EQ(x.index(), parameter_proxies[0].index());
    EXPECT_EQ(1, parameter_proxies[0].number_of_dimensions());
    EXPECT_EQ(1, parameter_proxies[0].number_of_elements());
    EXPECT_EQ("x", parameter_proxies[0].flat_indexed_names(0));

    EXPECT_EQ(y.index(), parameter_proxies[1].index());
    EXPECT_EQ(1, parameter_proxies[1].number_of_dimensions());
    EXPECT_EQ(10, parameter_proxies[1].number_of_elements());
    EXPECT_EQ("y[ 0]", parameter_proxies[1].flat_indexed_names(0));
    EXPECT_EQ("y[ 9]", parameter_proxies[1].flat_indexed_names(10 - 1));

    EXPECT_EQ(z.index(), parameter_proxies[2].index());
    EXPECT_EQ(2, parameter_proxies[2].number_of_dimensions());
    EXPECT_EQ(100, parameter_proxies[2].number_of_elements());
    EXPECT_EQ("z[ 0,  0]", parameter_proxies[2].flat_indexed_names(0));
    EXPECT_EQ("z[ 9,  9]", parameter_proxies[2].flat_indexed_names(100 - 1));

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
    model::Model<int, double> model;

    auto& p = model.create_expression("p");
    auto& q = model.create_expressions("q", 10);
    auto& r = model.create_expressions("r", {10, 10});

    int fill_value = random_integer();
    model.setup_unique_names();

    auto parameter_proxies =
        model.generate_expression_parameter_proxies(fill_value);
    EXPECT_EQ(p.index(), parameter_proxies[0].index());
    EXPECT_EQ(1, parameter_proxies[0].number_of_dimensions());
    EXPECT_EQ(1, parameter_proxies[0].number_of_elements());
    EXPECT_EQ("p", parameter_proxies[0].flat_indexed_names(0));

    EXPECT_EQ(q.index(), parameter_proxies[1].index());
    EXPECT_EQ(1, parameter_proxies[1].number_of_dimensions());
    EXPECT_EQ(10, parameter_proxies[1].number_of_elements());
    EXPECT_EQ("q[ 0]", parameter_proxies[1].flat_indexed_names(0));
    EXPECT_EQ("q[ 9]", parameter_proxies[1].flat_indexed_names(10 - 1));

    EXPECT_EQ(r.index(), parameter_proxies[2].index());
    EXPECT_EQ(2, parameter_proxies[2].number_of_dimensions());
    EXPECT_EQ(100, parameter_proxies[2].number_of_elements());
    EXPECT_EQ("r[ 0,  0]", parameter_proxies[2].flat_indexed_names(0));
    EXPECT_EQ("r[ 9,  9]", parameter_proxies[2].flat_indexed_names(100 - 1));

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
    model::Model<int, double> model;

    auto& g = model.create_constraint("g");
    auto& h = model.create_constraints("h", 10);
    auto& v = model.create_constraints("v", {10, 10});

    int fill_value = random_integer();
    model.setup_unique_names();

    auto parameter_proxies =
        model.generate_constraint_parameter_proxies(fill_value);
    EXPECT_EQ(g.index(), parameter_proxies[0].index());
    EXPECT_EQ(1, parameter_proxies[0].number_of_dimensions());
    EXPECT_EQ(1, parameter_proxies[0].number_of_elements());
    EXPECT_EQ("g", parameter_proxies[0].flat_indexed_names(0));

    EXPECT_EQ(h.index(), parameter_proxies[1].index());
    EXPECT_EQ(1, parameter_proxies[1].number_of_dimensions());
    EXPECT_EQ(10, parameter_proxies[1].number_of_elements());
    EXPECT_EQ("h[ 0]", parameter_proxies[1].flat_indexed_names(0));
    EXPECT_EQ("h[ 9]", parameter_proxies[1].flat_indexed_names(10 - 1));

    EXPECT_EQ(v.index(), parameter_proxies[2].index());
    EXPECT_EQ(2, parameter_proxies[2].number_of_dimensions());
    EXPECT_EQ(100, parameter_proxies[2].number_of_elements());
    EXPECT_EQ("v[ 0,  0]", parameter_proxies[2].flat_indexed_names(0));
    EXPECT_EQ("v[ 9,  9]", parameter_proxies[2].flat_indexed_names(100 - 1));

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
TEST_F(TestModel, export_local_penalty_coefficient_proxies) {
    model::Model<int, double> model;

    auto& g = model.create_constraint("g");
    auto& h = model.create_constraints("h", 10);
    auto& v = model.create_constraints("v", {10, 10});

    model.setup_unique_names();

    g(0).local_penalty_coefficient_less()    = 1.0;
    g(0).local_penalty_coefficient_greater() = 10.0;

    for (auto i = 0; i < 10; i++) {
        h(i).local_penalty_coefficient_less()    = 20.0;
        h(i).local_penalty_coefficient_greater() = 2.0;
    }

    for (auto i = 0; i < 10; i++) {
        for (auto j = 0; j < 10; j++) {
            v(i, j).local_penalty_coefficient_less()    = 3.0;
            v(i, j).local_penalty_coefficient_greater() = 30.0;
        }
    }

    auto local_penalty_coefficient_proxies =
        model.export_local_penalty_coefficient_proxies();

    EXPECT_EQ(g.index(), local_penalty_coefficient_proxies[0].index());
    EXPECT_EQ(1, local_penalty_coefficient_proxies[0].number_of_dimensions());
    EXPECT_EQ(1, local_penalty_coefficient_proxies[0].number_of_elements());
    EXPECT_EQ("g", local_penalty_coefficient_proxies[0].flat_indexed_names(0));

    EXPECT_EQ(h.index(), local_penalty_coefficient_proxies[1].index());
    EXPECT_EQ(1, local_penalty_coefficient_proxies[1].number_of_dimensions());
    EXPECT_EQ(10, local_penalty_coefficient_proxies[1].number_of_elements());
    EXPECT_EQ("h[ 0]",
              local_penalty_coefficient_proxies[1].flat_indexed_names(0));
    EXPECT_EQ("h[ 9]",
              local_penalty_coefficient_proxies[1].flat_indexed_names(10 - 1));

    EXPECT_EQ(v.index(), local_penalty_coefficient_proxies[2].index());
    EXPECT_EQ(2, local_penalty_coefficient_proxies[2].number_of_dimensions());
    EXPECT_EQ(100, local_penalty_coefficient_proxies[2].number_of_elements());
    EXPECT_EQ("v[ 0,  0]",
              local_penalty_coefficient_proxies[2].flat_indexed_names(0));
    EXPECT_EQ("v[ 9,  9]",
              local_penalty_coefficient_proxies[2].flat_indexed_names(100 - 1));

    for (auto&& value :
         local_penalty_coefficient_proxies[0].flat_indexed_values()) {
        EXPECT_FLOAT_EQ(10.0, value);
    }
    for (auto&& value :
         local_penalty_coefficient_proxies[1].flat_indexed_values()) {
        EXPECT_FLOAT_EQ(20.0, value);
    }
    for (auto&& value :
         local_penalty_coefficient_proxies[2].flat_indexed_values()) {
        EXPECT_FLOAT_EQ(30.0, value);
    }
}

/*****************************************************************************/
TEST_F(TestModel, export_update_count_proxies) {
    model::Model<int, double> model;

    auto& x = model.create_variable("x");
    auto& y = model.create_variables("y", 10);
    auto& z = model.create_variables("z", {10, 10});

    model.setup_unique_names();
    x(0).increment_update_count();

    for (auto i = 0; i < 10; i++) {
        y(i).increment_update_count();
        y(i).increment_update_count();
    }

    for (auto i = 0; i < 10; i++) {
        for (auto j = 0; j < 10; j++) {
            z(i, j).increment_update_count();
            z(i, j).increment_update_count();
            z(i, j).increment_update_count();
        }
    }

    auto update_count_proxies = model.export_update_count_proxies();

    EXPECT_EQ(x.index(), update_count_proxies[0].index());
    EXPECT_EQ(1, update_count_proxies[0].number_of_dimensions());
    EXPECT_EQ(1, update_count_proxies[0].number_of_elements());
    EXPECT_EQ("x", update_count_proxies[0].flat_indexed_names(0));

    EXPECT_EQ(y.index(), update_count_proxies[1].index());
    EXPECT_EQ(1, update_count_proxies[1].number_of_dimensions());
    EXPECT_EQ(10, update_count_proxies[1].number_of_elements());
    EXPECT_EQ("y[ 0]", update_count_proxies[1].flat_indexed_names(0));
    EXPECT_EQ("y[ 9]", update_count_proxies[1].flat_indexed_names(10 - 1));

    EXPECT_EQ(z.index(), update_count_proxies[2].index());
    EXPECT_EQ(2, update_count_proxies[2].number_of_dimensions());
    EXPECT_EQ(100, update_count_proxies[2].number_of_elements());
    EXPECT_EQ("z[ 0,  0]", update_count_proxies[2].flat_indexed_names(0));
    EXPECT_EQ("z[ 9,  9]", update_count_proxies[2].flat_indexed_names(100 - 1));

    for (auto&& value : update_count_proxies[0].flat_indexed_values()) {
        EXPECT_EQ(1, value);
    }
    for (auto&& value : update_count_proxies[1].flat_indexed_values()) {
        EXPECT_EQ(2, value);
    }
    for (auto&& value : update_count_proxies[2].flat_indexed_values()) {
        EXPECT_EQ(3, value);
    }
}

/*****************************************************************************/
TEST_F(TestModel, export_violation_count_proxies) {
    model::Model<int, double> model;

    auto& g = model.create_constraint("g");
    auto& h = model.create_constraints("h", 10);
    auto& v = model.create_constraints("v", {10, 10});

    model.setup_unique_names();

    g(0).increment_violation_count();

    for (auto i = 0; i < 10; i++) {
        h(i).increment_violation_count();
        h(i).increment_violation_count();
    }

    for (auto i = 0; i < 10; i++) {
        for (auto j = 0; j < 10; j++) {
            v(i, j).increment_violation_count();
            v(i, j).increment_violation_count();
            v(i, j).increment_violation_count();
        }
    }

    auto violation_count_proxies = model.export_violation_count_proxies();

    EXPECT_EQ(g.index(), violation_count_proxies[0].index());
    EXPECT_EQ(1, violation_count_proxies[0].number_of_dimensions());
    EXPECT_EQ(1, violation_count_proxies[0].number_of_elements());
    EXPECT_EQ("g", violation_count_proxies[0].flat_indexed_names(0));

    EXPECT_EQ(h.index(), violation_count_proxies[1].index());
    EXPECT_EQ(1, violation_count_proxies[1].number_of_dimensions());
    EXPECT_EQ(10, violation_count_proxies[1].number_of_elements());
    EXPECT_EQ("h[ 0]", violation_count_proxies[1].flat_indexed_names(0));
    EXPECT_EQ("h[ 9]", violation_count_proxies[1].flat_indexed_names(10 - 1));

    EXPECT_EQ(v.index(), violation_count_proxies[2].index());
    EXPECT_EQ(2, violation_count_proxies[2].number_of_dimensions());
    EXPECT_EQ(100, violation_count_proxies[2].number_of_elements());
    EXPECT_EQ("v[ 0,  0]", violation_count_proxies[2].flat_indexed_names(0));
    EXPECT_EQ("v[ 9,  9]",
              violation_count_proxies[2].flat_indexed_names(100 - 1));

    for (auto&& value : violation_count_proxies[0].flat_indexed_values()) {
        EXPECT_EQ(1, value);
    }
    for (auto&& value : violation_count_proxies[1].flat_indexed_values()) {
        EXPECT_EQ(2, value);
    }
    for (auto&& value : violation_count_proxies[2].flat_indexed_values()) {
        EXPECT_EQ(3, value);
    }
}

/*****************************************************************************/
TEST_F(TestModel, export_dense_solution) {
    model::Model<int, double> model;

    auto& x = model.create_variable("x");
    auto& y = model.create_variables("y", 10);
    auto& z = model.create_variables("z", {20, 30});

    auto& p = model.create_expression("p");
    auto& q = model.create_expressions("q", 10);
    auto& r = model.create_expressions("r", {20, 30});

    auto& g = model.create_constraint("g");
    auto& h = model.create_constraints("h", 10);
    auto& v = model.create_constraints("v", {20, 30});

    p = random_integer() * x;
    for (auto i = 0; i < 10; i++) {
        q(i) = random_integer() * y(i);
    }

    for (auto i = 0; i < 20; i++) {
        for (auto j = 0; j < 30; j++) {
            r(i, j) = random_integer() * z(i, j) + random_integer();
            v(i, j) = r(i, j) == random_integer();
        }
    }
    model.minimize(random_integer() * p + random_integer() * q.sum() +
                   random_integer() * r.sum());

    x = random_integer();
    for (auto i = 0; i < 10; i++) {
        y(i) = random_integer();
    }

    for (auto i = 0; i < 20; i++) {
        for (auto j = 0; j < 30; j++) {
            z(i, j) = random_integer();
        }
    }

    model.update();

    auto solution = model.export_dense_solution();
    EXPECT_EQ(3, static_cast<int>(solution.variable_value_proxies.size()));
    EXPECT_EQ(3, static_cast<int>(solution.expression_value_proxies.size()));
    EXPECT_EQ(3, static_cast<int>(solution.constraint_value_proxies.size()));
    EXPECT_EQ(3, static_cast<int>(solution.violation_value_proxies.size()));

    EXPECT_EQ(model.objective().value(), solution.objective);
    EXPECT_EQ(model.is_feasible(), solution.is_feasible);

    EXPECT_EQ(x.index(), solution.variable_value_proxies[0].index());
    EXPECT_EQ(x.value(), solution.variable_value_proxies[0].value());

    EXPECT_EQ(y.index(), solution.variable_value_proxies[1].index());
    for (auto i = 0; i < 10; i++) {
        EXPECT_EQ(y(i).value(), solution.variable_value_proxies[1](i));
    }

    EXPECT_EQ(z.index(), solution.variable_value_proxies[2].index());
    for (auto i = 0; i < 20; i++) {
        for (auto j = 0; j < 30; j++) {
            EXPECT_EQ(z(i, j).value(),
                      solution.variable_value_proxies[2](i, j));
        }
    }

    EXPECT_EQ(p.index(), solution.expression_value_proxies[0].index());
    EXPECT_EQ(p.value(), solution.expression_value_proxies[0].value());

    EXPECT_EQ(q.index(), solution.expression_value_proxies[1].index());
    for (auto i = 0; i < 10; i++) {
        EXPECT_EQ(q(i).value(), solution.expression_value_proxies[1](i));
    }

    EXPECT_EQ(r.index(), solution.expression_value_proxies[2].index());
    for (auto i = 0; i < 20; i++) {
        for (auto j = 0; j < 30; j++) {
            EXPECT_EQ(r(i, j).value(),
                      solution.expression_value_proxies[2](i, j));
        }
    }

    EXPECT_EQ(g.index(), solution.constraint_value_proxies[0].index());
    EXPECT_EQ(g.constraint_value(),
              solution.constraint_value_proxies[0].value());

    EXPECT_EQ(h.index(), solution.constraint_value_proxies[1].index());
    for (auto i = 0; i < 10; i++) {
        EXPECT_EQ(h(i).constraint_value(),
                  solution.constraint_value_proxies[1](i));
    }

    EXPECT_EQ(v.index(), solution.constraint_value_proxies[2].index());
    for (auto i = 0; i < 20; i++) {
        for (auto j = 0; j < 30; j++) {
            EXPECT_EQ(v(i, j).constraint_value(),
                      solution.constraint_value_proxies[2](i, j));
        }
    }

    EXPECT_EQ(g.index(), solution.violation_value_proxies[0].index());
    EXPECT_EQ(g.violation_value(), solution.violation_value_proxies[0].value());

    EXPECT_EQ(h.index(), solution.constraint_value_proxies[1].index());
    for (auto i = 0; i < 10; i++) {
        EXPECT_EQ(h(i).violation_value(),
                  solution.violation_value_proxies[1](i));
    }

    EXPECT_EQ(v.index(), solution.violation_value_proxies[2].index());
    for (auto i = 0; i < 20; i++) {
        for (auto j = 0; j < 30; j++) {
            EXPECT_EQ(v(i, j).violation_value(),
                      solution.violation_value_proxies[2](i, j));
        }
    }
}

/*****************************************************************************/
TEST_F(TestModel, export_sparse_solution) {
    model::Model<int, double> model;

    auto& x = model.create_variable("x");
    auto& y = model.create_variables("y", 10);
    auto& z = model.create_variables("z", {20, 30});

    model.minimize(random_integer() * x.sum() + random_integer() * y.sum() +
                   random_integer() * z.sum());
    model.setup_unique_names();

    x = 10;
    for (auto i = 0; i < 10; i++) {
        y(i) = random_integer();
    }

    for (auto i = 0; i < 20; i++) {
        for (auto j = 0; j < 30; j++) {
            z(i, j) = random_integer();
        }
    }

    model.update();

    auto sparse_solution = model.export_sparse_solution();
    EXPECT_EQ(model.objective().value(), sparse_solution.objective);
    EXPECT_EQ(model.is_feasible(), sparse_solution.is_feasible);

    EXPECT_EQ(10, sparse_solution.variables[x[0].name()]);
}

/*****************************************************************************/
TEST_F(TestModel, export_named_solution) {
    model::Model<int, double> model;

    auto& x = model.create_variable("x");
    auto& y = model.create_variables("y", 10);
    auto& z = model.create_variables("z", {20, 30});

    auto& p = model.create_expression("p");
    auto& q = model.create_expressions("q", 10);
    auto& r = model.create_expressions("r", {20, 30});

    auto& g = model.create_constraint("g");
    auto& h = model.create_constraints("h", 10);
    auto& v = model.create_constraints("v", {20, 30});

    p = random_integer() * x;
    for (auto i = 0; i < 10; i++) {
        q(i) = random_integer() * y(i);
    }

    for (auto i = 0; i < 20; i++) {
        for (auto j = 0; j < 30; j++) {
            r(i, j) = random_integer() * z(i, j) + random_integer();
            v(i, j) = r(i, j) == random_integer();
        }
    }
    model.minimize(random_integer() * p + random_integer() * q.sum() +
                   random_integer() * r.sum());

    x = random_integer();
    for (auto i = 0; i < 10; i++) {
        y(i) = random_integer();
    }

    for (auto i = 0; i < 20; i++) {
        for (auto j = 0; j < 30; j++) {
            z(i, j) = random_integer();
        }
    }

    model.setup_structure();
    model.update();

    auto named_solution = model.export_named_solution();

    EXPECT_EQ(3, static_cast<int>(named_solution.variables().size()));
    EXPECT_EQ(3, static_cast<int>(named_solution.expressions().size()));
    EXPECT_EQ(3, static_cast<int>(named_solution.constraints().size()));
    EXPECT_EQ(3, static_cast<int>(named_solution.violations().size()));

    EXPECT_EQ(model.objective().value(), named_solution.objective());
    EXPECT_EQ(model.is_feasible(), named_solution.is_feasible());

    EXPECT_EQ(x.index(), named_solution.variables("x").index());
    EXPECT_EQ(x.index(), named_solution.variables().at("x").index());
    EXPECT_EQ(x.value(), named_solution.variables("x").value());
    EXPECT_EQ(x.value(), named_solution.variables().at("x").value());

    EXPECT_EQ(y.index(), named_solution.variables("y").index());
    EXPECT_EQ(y.index(), named_solution.variables().at("y").index());
    for (auto i = 0; i < 10; i++) {
        EXPECT_EQ(y(i).value(), named_solution.variables("y")(i));
        EXPECT_EQ(y(i).value(), named_solution.variables().at("y")(i));
    }

    EXPECT_EQ(z.index(), named_solution.variables("z").index());
    EXPECT_EQ(z.index(), named_solution.variables().at("z").index());
    for (auto i = 0; i < 20; i++) {
        for (auto j = 0; j < 30; j++) {
            EXPECT_EQ(z(i, j).value(), named_solution.variables("z")(i, j));
            EXPECT_EQ(z(i, j).value(),
                      named_solution.variables().at("z")(i, j));
        }
    }

    EXPECT_EQ(p.index(), named_solution.expressions("p").index());
    EXPECT_EQ(p.index(), named_solution.expressions().at("p").index());
    EXPECT_EQ(p.value(), named_solution.expressions("p").value());
    EXPECT_EQ(p.value(), named_solution.expressions().at("p").value());

    EXPECT_EQ(q.index(), named_solution.expressions("q").index());
    EXPECT_EQ(q.index(), named_solution.expressions().at("q").index());
    for (auto i = 0; i < 10; i++) {
        EXPECT_EQ(q(i).value(), named_solution.expressions("q")(i));
        EXPECT_EQ(q(i).value(), named_solution.expressions().at("q")(i));
    }

    EXPECT_EQ(r.index(), named_solution.expressions("r").index());
    EXPECT_EQ(r.index(), named_solution.expressions().at("r").index());
    for (auto i = 0; i < 20; i++) {
        for (auto j = 0; j < 30; j++) {
            EXPECT_EQ(r(i, j).value(), named_solution.expressions("r")(i, j));
            EXPECT_EQ(r(i, j).value(),
                      named_solution.expressions().at("r")(i, j));
        }
    }

    EXPECT_EQ(g.index(), named_solution.constraints("g").index());
    EXPECT_EQ(g.index(), named_solution.constraints().at("g").index());
    EXPECT_EQ(g.constraint_value(), named_solution.constraints("g").value());
    EXPECT_EQ(g.constraint_value(),
              named_solution.constraints().at("g").value());

    EXPECT_EQ(h.index(), named_solution.constraints("h").index());
    EXPECT_EQ(h.index(), named_solution.constraints().at("h").index());
    for (auto i = 0; i < 10; i++) {
        EXPECT_EQ(h(i).constraint_value(), named_solution.constraints("h")(i));
        EXPECT_EQ(h(i).constraint_value(),
                  named_solution.constraints().at("h")(i));
    }

    EXPECT_EQ(v.index(), named_solution.constraints("v").index());
    EXPECT_EQ(v.index(), named_solution.constraints().at("v").index());
    for (auto i = 0; i < 20; i++) {
        for (auto j = 0; j < 30; j++) {
            EXPECT_EQ(v(i, j).constraint_value(),
                      named_solution.constraints("v")(i, j));
            EXPECT_EQ(v(i, j).constraint_value(),
                      named_solution.constraints().at("v")(i, j));
        }
    }

    EXPECT_EQ(g.index(), named_solution.violations("g").index());
    EXPECT_EQ(g.index(), named_solution.violations().at("g").index());
    EXPECT_EQ(g.violation_value(), named_solution.violations("g").value());
    EXPECT_EQ(g.violation_value(), named_solution.violations().at("g").value());

    EXPECT_EQ(h.index(), named_solution.violations("h").index());
    EXPECT_EQ(h.index(), named_solution.violations().at("h").index());
    for (auto i = 0; i < 10; i++) {
        EXPECT_EQ(h(i).violation_value(), named_solution.violations("h")(i));
        EXPECT_EQ(h(i).violation_value(),
                  named_solution.violations().at("h")(i));
    }

    EXPECT_EQ(v.index(), named_solution.violations("v").index());
    EXPECT_EQ(v.index(), named_solution.violations().at("v").index());
    for (auto i = 0; i < 20; i++) {
        for (auto j = 0; j < 30; j++) {
            EXPECT_EQ(v(i, j).violation_value(),
                      named_solution.violations("v")(i, j));
            EXPECT_EQ(v(i, j).violation_value(),
                      named_solution.violations().at("v")(i, j));
        }
    }
}

/*****************************************************************************/
TEST_F(TestModel, export_named_penalty_coefficients) {
    model::Model<int, double> model;

    auto& g = model.create_constraint("g");
    auto& h = model.create_constraints("h", 10);
    auto& v = model.create_constraints("v", {10, 10});

    model.setup_unique_names();

    g(0).local_penalty_coefficient_less()    = 1.0;
    g(0).local_penalty_coefficient_greater() = 10.0;

    for (auto i = 0; i < 10; i++) {
        h(i).local_penalty_coefficient_less()    = 20.0;
        h(i).local_penalty_coefficient_greater() = 2.0;
    }

    for (auto i = 0; i < 10; i++) {
        for (auto j = 0; j < 10; j++) {
            v(i, j).local_penalty_coefficient_less()    = 3.0;
            v(i, j).local_penalty_coefficient_greater() = 30.0;
        }
    }

    auto named_penalty_coefficients = model.export_named_penalty_coefficients();

    EXPECT_FLOAT_EQ(10.0, named_penalty_coefficients["g"](0));
    EXPECT_FLOAT_EQ(20.0, named_penalty_coefficients["h"](0));
    EXPECT_FLOAT_EQ(20.0, named_penalty_coefficients["h"](9));
    EXPECT_FLOAT_EQ(30.0, named_penalty_coefficients["v"](0, 0));
    EXPECT_FLOAT_EQ(30.0, named_penalty_coefficients["v"](9, 9));
}

/*****************************************************************************/
TEST_F(TestModel, export_named_update_counts) {
    model::Model<int, double> model;

    auto& x = model.create_variable("x");
    auto& y = model.create_variables("y", 10);
    auto& z = model.create_variables("z", {10, 10});

    model.setup_unique_names();
    x(0).increment_update_count();

    for (auto i = 0; i < 10; i++) {
        y(i).increment_update_count();
        y(i).increment_update_count();
    }

    for (auto i = 0; i < 10; i++) {
        for (auto j = 0; j < 10; j++) {
            z(i, j).increment_update_count();
            z(i, j).increment_update_count();
            z(i, j).increment_update_count();
        }
    }

    auto named_update_counts = model.export_named_update_counts();

    EXPECT_EQ(1, named_update_counts["x"](0));
    EXPECT_EQ(2, named_update_counts["y"](0));
    EXPECT_EQ(2, named_update_counts["y"](9));
    EXPECT_EQ(3, named_update_counts["z"](0, 0));
    EXPECT_EQ(3, named_update_counts["z"](9, 9));
}

/*****************************************************************************/
TEST_F(TestModel, export_named_violation_counts) {
    model::Model<int, double> model;

    auto& g = model.create_constraint("g");
    auto& h = model.create_constraints("h", 10);
    auto& v = model.create_constraints("v", {10, 10});

    model.setup_unique_names();

    g(0).increment_violation_count();

    for (auto i = 0; i < 10; i++) {
        h(i).increment_violation_count();
        h(i).increment_violation_count();
    }

    for (auto i = 0; i < 10; i++) {
        for (auto j = 0; j < 10; j++) {
            v(i, j).increment_violation_count();
            v(i, j).increment_violation_count();
            v(i, j).increment_violation_count();
        }
    }

    auto violation_counts = model.export_named_violation_counts();

    EXPECT_EQ(1, violation_counts["g"](0));
    EXPECT_EQ(2, violation_counts["h"](0));
    EXPECT_EQ(2, violation_counts["h"](9));
    EXPECT_EQ(3, violation_counts["v"](0, 0));
    EXPECT_EQ(3, violation_counts["v"](9, 9));
}

/*****************************************************************************/
TEST_F(TestModel, import_solution_arg_dense_solution) {
    model::Model<int, double> model;

    auto& x = model.create_variable("x");
    auto& y = model.create_variables("y", 10);
    auto& z = model.create_variables("z", {10, 10});

    multi_array::ValueProxy<int> x_value(x.index());
    multi_array::ValueProxy<int> y_value(y.index(), 10);
    multi_array::ValueProxy<int> z_value(z.index(), {10, 10});

    x_value.value() = 1;

    for (auto i = 0; i < 10; i++) {
        y_value(i) = 10 * i;
    }

    for (auto i = 0; i < 10; i++) {
        for (auto j = 0; j < 10; j++) {
            z_value(i, j) = 100 * (i + j);
        }
    }

    solution::DenseSolution<int, double> solution;
    solution.variable_value_proxies.push_back(x_value);
    solution.variable_value_proxies.push_back(y_value);
    solution.variable_value_proxies.push_back(z_value);

    model.import_solution(solution);

    EXPECT_EQ(1, x.value());

    for (auto i = 0; i < 10; i++) {
        EXPECT_EQ(10 * i, y(i).value());
    }

    for (auto i = 0; i < 10; i++) {
        for (auto j = 0; j < 10; j++) {
            EXPECT_EQ(100 * (i + j), z(i, j).value());
        }
    }
}

/*****************************************************************************/
TEST_F(TestModel, import_solution_arg_sparse_solution) {
    model::Model<int, double> model;

    auto& x = model.create_variable("x");
    auto& y = model.create_variables("y", 10);
    auto& z = model.create_variables("z", {20, 30});

    model.setup_unique_names();

    solution::SparseSolution<int, double> solution;
    solution.variables["x"]         = 1;
    solution.variables["y[ 0]"]     = 2;
    solution.variables["y[ 9]"]     = 3;
    solution.variables["z[ 0,  0]"] = 4;
    solution.variables["z[19, 19]"] = 5;

    model.import_solution(solution);

    EXPECT_EQ(x.value(), 1);
    EXPECT_EQ(y(0).value(), 2);
    EXPECT_EQ(y(9).value(), 3);
    EXPECT_EQ(z(0, 0).value(), 4);
    EXPECT_EQ(z(19, 19).value(), 5);
}

/*****************************************************************************/
TEST_F(TestModel, import_solution_arg_unordered_map) {
    model::Model<int, double> model;

    auto& x = model.create_variable("x");
    auto& y = model.create_variables("y", 10);
    auto& z = model.create_variables("z", {20, 30});

    model.setup_unique_names();

    std::unordered_map<std::string, int> variables;
    variables["x"]         = 1;
    variables["y[ 0]"]     = 2;
    variables["y[ 9]"]     = 3;
    variables["z[ 0,  0]"] = 4;
    variables["z[19, 19]"] = 5;

    model.import_solution(variables);

    EXPECT_EQ(x.value(), 1);
    EXPECT_EQ(y(0).value(), 2);
    EXPECT_EQ(y(9).value(), 3);
    EXPECT_EQ(z(0, 0).value(), 4);
    EXPECT_EQ(z(19, 19).value(), 5);
}

/*****************************************************************************/
TEST_F(TestModel, fix_variables) {
    model::Model<int, double> model;

    auto& x = model.create_variable("x");
    auto& y = model.create_variables("y", 10);
    auto& z = model.create_variables("z", {20, 30});

    model.setup_unique_names();

    std::unordered_map<std::string, int> solution;
    solution["x"]         = 1;
    solution["y[ 0]"]     = 2;
    solution["y[ 9]"]     = 3;
    solution["z[ 0,  0]"] = 4;
    solution["z[19, 19]"] = 5;
    model.fix_variables(solution);

    EXPECT_EQ(1, x.value());
    EXPECT_EQ(2, y(0).value());
    EXPECT_EQ(3, y(9).value());
    EXPECT_EQ(4, z(0, 0).value());
    EXPECT_EQ(5, z(19, 19).value());

    EXPECT_TRUE(x.is_fixed());
    EXPECT_TRUE(y(0).is_fixed());
    EXPECT_TRUE(y(9).is_fixed());
    EXPECT_TRUE(z(0, 0).is_fixed());
    EXPECT_TRUE(z(19, 19).is_fixed());
}

/*****************************************************************************/
TEST_F(TestModel, unfix_variables) {
    model::Model<int, double> model;

    auto& x = model.create_variable("x");
    auto& y = model.create_variables("y", 10);
    auto& z = model.create_variables("z", {20, 30});

    model.setup_unique_names();

    std::unordered_set<std::string> mutable_variable_names;
    mutable_variable_names.insert("x");
    mutable_variable_names.insert("y[ 0]");
    mutable_variable_names.insert("y[ 9]");
    mutable_variable_names.insert("z[ 0,  0]");
    mutable_variable_names.insert("z[19, 19]");
    model.unfix_variables(mutable_variable_names);

    EXPECT_FALSE(x.is_fixed());
    EXPECT_FALSE(y(0).is_fixed());
    EXPECT_TRUE(y(1).is_fixed());
    EXPECT_TRUE(y(8).is_fixed());
    EXPECT_FALSE(y(9).is_fixed());
    EXPECT_FALSE(z(0, 0).is_fixed());
    EXPECT_TRUE(z(0, 1).is_fixed());
    EXPECT_TRUE(z(19, 18).is_fixed());
    EXPECT_FALSE(z(19, 19).is_fixed());
}

/*****************************************************************************/
TEST_F(TestModel, set_user_defined_selection_constraints) {
    /// To be implemented.
}

/*****************************************************************************/
TEST_F(TestModel, setup_flippable_variable_ptr_pairs) {
    /// To be implemented.
}

/*****************************************************************************/
TEST_F(TestModel, import_mps) {
    /// To be implemented.
}

/*****************************************************************************/
TEST_F(TestModel, import_pb) {
    /// To be implemented.
}

/*****************************************************************************/
TEST_F(TestModel, write_mps) {
    /// To be implemented.
}

/*****************************************************************************/
TEST_F(TestModel, write_json) {
    /// To be implemented.
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
    /// This method is tested in minimize_arg_expression and so on.
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
TEST_F(TestModel, selections) {
    /// This method is tested in extract_selections_larger() and so on.
}

/*****************************************************************************/
TEST_F(TestModel, variable_reference) {
    /// This method is tested in initialize() and so on.
}

/*****************************************************************************/
TEST_F(TestModel, variable_reference_original) {
    /// This method is tested in initialize() and so on.
}

/*****************************************************************************/
TEST_F(TestModel, variable_type_reference) {
    /// This method is tested in initialize() and so on.
}

/*****************************************************************************/
TEST_F(TestModel, variable_type_reference_original) {
    /// This method is tested in initialize() and so on.
}

/*****************************************************************************/
TEST_F(TestModel, constraint_reference) {
    /// This method is tested in initialize() and so on.
}

/*****************************************************************************/
TEST_F(TestModel, constraint_type_reference) {
    /// This method is tested in initialize() and so on.
}

/*****************************************************************************/
TEST_F(TestModel, is_defined_objective) {
    /// This method is tested in minimize_arg_expression() and so on.
}

/*****************************************************************************/
TEST_F(TestModel, is_minimization) {
    /// This method is tested in minimize_arg_expression() and so on.
}

/*****************************************************************************/
TEST_F(TestModel, sign) {
    /// This method is tested in minimize_arg_expression() and so on.
}

/*****************************************************************************/
TEST_F(TestModel, set_is_solved) {
    model::Model<int, double> model;
    EXPECT_FALSE(model.is_solved());
    model.set_is_solved(true);
    EXPECT_TRUE(model.is_solved());
    model.set_is_solved(false);
    EXPECT_FALSE(model.is_solved());
}

/*****************************************************************************/
TEST_F(TestModel, is_solved) {
    /// This method is tested in set_is_solved().
}

/*****************************************************************************/
TEST_F(TestModel, current_violative_constraint_ptrs) {
    /// This method is tested in update_violative_constraint_ptrs().
}

/*****************************************************************************/
TEST_F(TestModel, previous_violative_constraint_ptrs) {
    /// This method is tested in update_violative_constraint_ptrs().
}

/*****************************************************************************/
TEST_F(TestModel, is_feasible) {
    /// This method is tested in update_feasibility().
}

/*****************************************************************************/
TEST_F(TestModel, number_of_variables) {
    /// This method is tested in categorize_variables().
}

/*****************************************************************************/
TEST_F(TestModel, number_of_fixed_variables) {
    /// This method is tested in categorize_variables().
}

/*****************************************************************************/
TEST_F(TestModel, number_of_mutable_variables) {
    /// This method is tested in categorize_variables().
}

/*****************************************************************************/
TEST_F(TestModel, number_of_selection_variables) {
    /// This method is tested in extract_selections_larger() and so on.
}

/*****************************************************************************/
TEST_F(TestModel, number_of_binary_variables) {
    /// This method is tested in categorize_variables().
}

/*****************************************************************************/
TEST_F(TestModel, number_of_integer_variables) {
    /// This method is tested in categorize_variables().
}

/*****************************************************************************/
TEST_F(TestModel, number_of_constraints) {
    /// This method is tested in categorize_constraints().
}

/*****************************************************************************/
TEST_F(TestModel, number_of_selection_constraints) {
    /// This method is tested in extract_selections_larger() and so on.
}

/*****************************************************************************/
TEST_F(TestModel, number_of_enabled_constraints) {
    /// This method is tested in categorize_constraints().
}

/*****************************************************************************/
TEST_F(TestModel, number_of_disabled_constraints) {
    /// This method is tested in categorize_constraints().
}

/*****************************************************************************/
TEST_F(TestModel, has_chain_move_effective_constraints) {
    /// None
    {
        model::Model<int, double> model;

        [[maybe_unused]] auto& x = model.create_variables("x", 10, 0, 1);
        model.setup_structure();
        EXPECT_FALSE(model.has_chain_move_effective_constraints());
    }

    /// Set Partitioning
    {
        model::Model<int, double> model;

        auto&                  x = model.create_variables("x", 10, 0, 1);
        [[maybe_unused]] auto& f = model.create_constraint("f", x.sum() == 1);

        model.setup_structure();
        EXPECT_TRUE(model.has_chain_move_effective_constraints());
    }

    /// Set Packing
    {
        model::Model<int, double> model;

        auto&                  x = model.create_variables("x", 10, 0, 1);
        [[maybe_unused]] auto& f = model.create_constraint("f", x.sum() <= 1);

        model.setup_structure();
        EXPECT_TRUE(model.has_chain_move_effective_constraints());
    }

    /// Set Covering
    {
        model::Model<int, double> model;

        auto&                  x = model.create_variables("x", 10, 0, 1);
        [[maybe_unused]] auto& f = model.create_constraint("f", x.sum() >= 1);

        model.setup_structure();
        EXPECT_TRUE(model.has_chain_move_effective_constraints());
    }

    /// Cardinality
    {
        model::Model<int, double> model;

        auto&                  x = model.create_variables("x", 10, 0, 1);
        [[maybe_unused]] auto& f = model.create_constraint("f", x.sum() == 5);

        model.setup_structure();
        EXPECT_TRUE(model.has_chain_move_effective_constraints());
    }

    /// Invariant Knapsack
    {
        model::Model<int, double> model;

        auto&                  x = model.create_variables("x", 10, 0, 1);
        [[maybe_unused]] auto& f = model.create_constraint("f", x.sum() <= 5);

        model.setup_structure();
        EXPECT_TRUE(model.has_chain_move_effective_constraints());
    }

    /// Multiple Covering
    {
        model::Model<int, double> model;

        auto&                  x = model.create_variables("x", 10, 0, 1);
        [[maybe_unused]] auto& f = model.create_constraint("f", x.sum() >= 5);

        model.setup_structure();
        EXPECT_TRUE(model.has_chain_move_effective_constraints());
    }
}

/*****************************************************************************/
TEST_F(TestModel, neighborhood) {
    /// This method is tested in test_neighborhood.h
}

/*****************************************************************************/
TEST_F(TestModel, problem_size_reducer) {
    /// To be implemented.
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/