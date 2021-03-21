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
class TestModel : public ::testing::Test {
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
TEST_F(TestModel, initialize) {
    printemps::model::Model<int, double> model;

    /// These declarations are for googletest constraint.
    /// http://opencv.jp/googletestdocs/FAQ.html#faq-the-compiler-complains-about-undefined-reference
    auto max_number_of_variable_proxies =
        printemps::model::ModelConstant::MAX_NUMBER_OF_VARIABLE_PROXIES;
    auto max_number_of_expression_proxies =
        printemps::model::ModelConstant::MAX_NUMBER_OF_EXPRESSION_PROXIES;
    auto max_number_of_constraint_proxies =
        printemps::model::ModelConstant::MAX_NUMBER_OF_CONSTRAINT_PROXIES;

    EXPECT_EQ("", model.name());

    EXPECT_EQ(max_number_of_variable_proxies,
              static_cast<int>(model.variable_proxies().capacity()));
    EXPECT_EQ(max_number_of_expression_proxies,
              static_cast<int>(model.expression_proxies().capacity()));
    EXPECT_EQ(max_number_of_constraint_proxies,
              static_cast<int>(model.constraint_proxies().capacity()));

    EXPECT_EQ(true, model.variable_names().empty());
    EXPECT_EQ(true, model.expression_names().empty());
    EXPECT_EQ(true, model.constraint_names().empty());

    EXPECT_EQ(false, model.is_defined_objective());
    EXPECT_EQ(true, model.is_enabled_fast_evaluation());
    EXPECT_EQ(true, model.is_linear());
    EXPECT_EQ(true, model.is_minimization());
    EXPECT_EQ(1.0, model.sign());
    EXPECT_EQ(false, model.is_solved());
    EXPECT_EQ(false, model.is_feasible());

    /// Variable Reference
    EXPECT_EQ(  //
        true, model.variable_reference().variable_ptrs.empty());
    EXPECT_EQ(  //
        true, model.variable_reference().fixed_variable_ptrs.empty());
    EXPECT_EQ(  //
        true, model.variable_reference().selection_variable_ptrs.empty());
    EXPECT_EQ(  //
        true, model.variable_reference().binary_variable_ptrs.empty());
    EXPECT_EQ(  //
        true, model.variable_reference().integer_variable_ptrs.empty());
    EXPECT_EQ(  //
        true,   //
        model.variable_reference().variable_ptrs.empty());

    /// Constraint Reference
    EXPECT_EQ(  //
        true,   //
        model.constraint_reference().constraint_ptrs.empty());
    EXPECT_EQ(  //
        true, model.constraint_reference().selection_constraint_ptrs.empty());
    EXPECT_EQ(  //
        true, model.constraint_reference().disabled_constraint_ptrs.empty());

    /// Constraint Type Reference
    EXPECT_EQ(  //
        true, model.constraint_type_reference().singleton_ptrs.empty());
    EXPECT_EQ(  //
        true, model.constraint_type_reference().aggregation_ptrs.empty());
    EXPECT_EQ(  //
        true, model.constraint_type_reference().precedence_ptrs.empty());
    EXPECT_EQ(  //
        true, model.constraint_type_reference().variable_bound_ptrs.empty());
    EXPECT_EQ(  //
        true,

        model.constraint_type_reference().set_partitioning_ptrs.empty());
    EXPECT_EQ(  //
        true, model.constraint_type_reference().set_packing_ptrs.empty());
    EXPECT_EQ(  //
        true, model.constraint_type_reference().set_covering_ptrs.empty());
    EXPECT_EQ(  //
        true, model.constraint_type_reference().cardinality_ptrs.empty());
    EXPECT_EQ(  //
        true,
        model.constraint_type_reference().invariant_knapsack_ptrs.empty());
    EXPECT_EQ(  //
        true, model.constraint_type_reference().equation_knapsack_ptrs.empty());
    EXPECT_EQ(  //
        true, model.constraint_type_reference().bin_packing_ptrs.empty());
    EXPECT_EQ(  //
        true, model.constraint_type_reference().knapsack_ptrs.empty());
    EXPECT_EQ(  //
        true, model.constraint_type_reference().integer_knapsack_ptrs.empty());
    EXPECT_EQ(  //
        true, model.constraint_type_reference().general_linear_ptrs.empty());
    EXPECT_EQ(  //
        true, model.constraint_type_reference().nonlinear_ptrs.empty());
}

/*****************************************************************************/
TEST_F(TestModel, constructor_arg_name) {
    printemps::model::Model<int, double> model("name");
    EXPECT_EQ("name", model.name());
}

/*****************************************************************************/
TEST_F(TestModel, set_name) {
    printemps::model::Model<int, double> model;
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
        printemps::model::Model<int, double> model;
        for (auto i = 0;
             i <
             printemps::model::ModelConstant::MAX_NUMBER_OF_VARIABLE_PROXIES;
             i++) {
            auto  name = "x" + std::to_string(i);
            auto& x    = model.create_variable(name);
            EXPECT_EQ(i + 1, static_cast<int>(model.variable_proxies().size()));
            EXPECT_EQ(i, x.index());
            EXPECT_EQ(printemps::constant::INT_HALF_MIN, x.lower_bound());
            EXPECT_EQ(printemps::constant::INT_HALF_MAX, x.upper_bound());
            EXPECT_EQ(false, x.has_bounds());
            EXPECT_EQ(printemps::model::VariableSense::Integer, x.sense());
            EXPECT_EQ(&x, &model.variable_proxies().back());
            EXPECT_EQ(name, model.variable_names().back());
        }
        ASSERT_THROW(model.create_variable("error"), std::logic_error);
    }
    {
        printemps::model::Model<int, double> model;
        ASSERT_THROW(model.create_variable("s p a c e"), std::logic_error);
    }
}

/*****************************************************************************/
TEST_F(TestModel, create_variable_scalar_with_bound) {
    {
        printemps::model::Model<int, double> model;
        for (auto i = 0;
             i <
             printemps::model::ModelConstant::MAX_NUMBER_OF_VARIABLE_PROXIES;
             i++) {
            auto  name = "x" + std::to_string(i);
            auto& x    = model.create_variable(name, 0, 1);
            EXPECT_EQ(i + 1, static_cast<int>(model.variable_proxies().size()));
            EXPECT_EQ(i, x.index());
            EXPECT_EQ(0, x.lower_bound());
            EXPECT_EQ(1, x.upper_bound());
            EXPECT_EQ(true, x.has_bounds());
            EXPECT_EQ(printemps::model::VariableSense::Binary, x.sense());
            EXPECT_EQ(&x, &model.variable_proxies().back());
            EXPECT_EQ(name, model.variable_names().back());
        }
        ASSERT_THROW(model.create_variable("error", 0, 1), std::logic_error);
    }
    {
        printemps::model::Model<int, double> model;
        ASSERT_THROW(model.create_variable("s p a c e", 0, 1),
                     std::logic_error);
    }
}

/*****************************************************************************/
TEST_F(TestModel, create_variable_one_dimensional_without_bound) {
    {
        printemps::model::Model<int, double> model;
        for (auto i = 0;
             i <
             printemps::model::ModelConstant::MAX_NUMBER_OF_VARIABLE_PROXIES;
             i++) {
            auto  name = "x" + std::to_string(i);
            auto& x    = model.create_variables(name, 2);
            EXPECT_EQ(i + 1, static_cast<int>(model.variable_proxies().size()));
            EXPECT_EQ(i, x.index());
            EXPECT_EQ(printemps::constant::INT_HALF_MIN, x(0).lower_bound());
            EXPECT_EQ(printemps::constant::INT_HALF_MAX, x(0).upper_bound());
            EXPECT_EQ(false, x(0).has_bounds());
            EXPECT_EQ(printemps::model::VariableSense::Integer, x(0).sense());
            EXPECT_EQ(&x, &model.variable_proxies().back());
            EXPECT_EQ(name, model.variable_names().back());
        }
        ASSERT_THROW(model.create_variables("error", 2), std::logic_error);
    }
    {
        printemps::model::Model<int, double> model;
        ASSERT_THROW(model.create_variables("s p a c e", 2), std::logic_error);
    }
}

/*****************************************************************************/
TEST_F(TestModel, create_variable_one_dimensional_with_bound) {
    {
        printemps::model::Model<int, double> model;
        for (auto i = 0;
             i <
             printemps::model::ModelConstant::MAX_NUMBER_OF_VARIABLE_PROXIES;
             i++) {
            auto  name = "x" + std::to_string(i);
            auto& x    = model.create_variables(name, 2, 0, 1);
            EXPECT_EQ(i + 1, static_cast<int>(model.variable_proxies().size()));
            EXPECT_EQ(i, x.index());
            EXPECT_EQ(0, x(0).lower_bound());
            EXPECT_EQ(1, x(0).upper_bound());
            EXPECT_EQ(true, x(0).has_bounds());
            EXPECT_EQ(printemps::model::VariableSense::Binary, x(0).sense());
            EXPECT_EQ(&x, &model.variable_proxies().back());
            EXPECT_EQ(name, model.variable_names().back());
        }
        ASSERT_THROW(model.create_variables("error", 2, 0, 1),
                     std::logic_error);
    }
    {
        printemps::model::Model<int, double> model;
        ASSERT_THROW(model.create_variables("s p a c e", 2, 0, 1),
                     std::logic_error);
    }
}

/*****************************************************************************/
TEST_F(TestModel, create_variable_two_dimensional_without_bound) {
    {
        printemps::model::Model<int, double> model;
        for (auto i = 0;
             i <
             printemps::model::ModelConstant::MAX_NUMBER_OF_VARIABLE_PROXIES;
             i++) {
            auto  name = "x" + std::to_string(i);
            auto& x    = model.create_variables(name, {2, 3});
            EXPECT_EQ(i + 1, static_cast<int>(model.variable_proxies().size()));
            EXPECT_EQ(i, x.index());
            EXPECT_EQ(printemps::constant::INT_HALF_MIN, x(0, 0).lower_bound());
            EXPECT_EQ(printemps::constant::INT_HALF_MAX, x(0, 0).upper_bound());
            EXPECT_EQ(false, x(0, 0).has_bounds());
            EXPECT_EQ(printemps::model::VariableSense::Integer,
                      x(0, 0).sense());
            EXPECT_EQ(&x, &model.variable_proxies().back());
            EXPECT_EQ(name, model.variable_names().back());
        }
        ASSERT_THROW(model.create_variables("error", {2, 3}), std::logic_error);
    }
    {
        printemps::model::Model<int, double> model;
        ASSERT_THROW(model.create_variables("s p a c e", {2, 3}),
                     std::logic_error);
    }
}

/*****************************************************************************/
TEST_F(TestModel, create_variable_two_dimensional_with_bound) {
    {
        printemps::model::Model<int, double> model;
        for (auto i = 0;
             i <
             printemps::model::ModelConstant::MAX_NUMBER_OF_VARIABLE_PROXIES;
             i++) {
            auto  name = "x" + std::to_string(i);
            auto& x    = model.create_variables(name, {2, 3}, 0, 1);
            EXPECT_EQ(i + 1, static_cast<int>(model.variable_proxies().size()));
            EXPECT_EQ(i, x.index());
            EXPECT_EQ(0, x(0, 0).lower_bound());
            EXPECT_EQ(1, x(0, 0).upper_bound());
            EXPECT_EQ(true, x(0, 0).has_bounds());
            EXPECT_EQ(printemps::model::VariableSense::Binary, x(0, 0).sense());
            EXPECT_EQ(&x, &model.variable_proxies().back());
            EXPECT_EQ(name, model.variable_names().back());
        }
        ASSERT_THROW(model.create_variables("error", {2, 3}, 0, 1),
                     std::logic_error);
    }
    {
        printemps::model::Model<int, double> model;
        ASSERT_THROW(model.create_variables("s p a c e", {2, 3}, 0, 1),
                     std::logic_error);
    }
}

/*****************************************************************************/
TEST_F(TestModel, create_expression_scalar) {
    {
        printemps::model::Model<int, double> model;
        for (auto i = 0;
             i <
             printemps::model::ModelConstant::MAX_NUMBER_OF_EXPRESSION_PROXIES;
             i++) {
            auto  name = "p" + std::to_string(i);
            auto& p    = model.create_expression(name);
            EXPECT_EQ(i + 1,
                      static_cast<int>(model.expression_proxies().size()));
            EXPECT_EQ(i, p.index());
            EXPECT_EQ(&p, &model.expression_proxies().back());
            EXPECT_EQ(name, model.expression_names().back());
        }
        ASSERT_THROW(model.create_expression("error"), std::logic_error);
    }
    {
        printemps::model::Model<int, double> model;
        ASSERT_THROW(model.create_expression("s p a c e"), std::logic_error);
    }
}

/*****************************************************************************/
TEST_F(TestModel, create_expression_one_dimensional) {
    {
        printemps::model::Model<int, double> model;
        for (auto i = 0;
             i <
             printemps::model::ModelConstant::MAX_NUMBER_OF_EXPRESSION_PROXIES;
             i++) {
            auto  name = "p" + std::to_string(i);
            auto& p    = model.create_expressions(name, 2);
            EXPECT_EQ(i + 1,
                      static_cast<int>(model.expression_proxies().size()));
            EXPECT_EQ(i, p.index());
            EXPECT_EQ(&p, &model.expression_proxies().back());
            EXPECT_EQ(name, model.expression_names().back());
        }
        ASSERT_THROW(model.create_expressions("error", 2), std::logic_error);
    }
    {
        printemps::model::Model<int, double> model;
        ASSERT_THROW(model.create_expressions("s p a c e", 2),
                     std::logic_error);
    }
}

/*****************************************************************************/
TEST_F(TestModel, create_expression_two_dimensional) {
    {
        printemps::model::Model<int, double> model;
        for (auto i = 0;
             i <
             printemps::model::ModelConstant::MAX_NUMBER_OF_EXPRESSION_PROXIES;
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
                     std::logic_error);
    }
    {
        printemps::model::Model<int, double> model;
        ASSERT_THROW(model.create_expressions("s p a c e", {2, 3}),
                     std::logic_error);
    }
}

/*****************************************************************************/
TEST_F(TestModel, create_expression_arg_expression_like) {
    {
        printemps::model::Model<int, double> model;

        for (auto i = 0;
             i <
             printemps::model::ModelConstant::MAX_NUMBER_OF_EXPRESSION_PROXIES;
             i++) {
            auto name = "p" + std::to_string(i);
            auto variable =
                printemps::model::Variable<int, double>::create_instance();

            auto& p = model.create_expression(name, variable);
            EXPECT_EQ(i + 1,
                      static_cast<int>(model.expression_proxies().size()));
            EXPECT_EQ(i, p.index());
            EXPECT_EQ(&p, &model.expression_proxies().back());
            EXPECT_EQ(name, model.expression_names().back());
        }

        auto variable =
            printemps::model::Variable<int, double>::create_instance();
        ASSERT_THROW(model.create_expression("error", variable),
                     std::logic_error);
    }
    {
        printemps::model::Model<int, double> model;
        auto                                 variable =
            printemps::model::Variable<int, double>::create_instance();
        ASSERT_THROW(model.create_expression("s p a c e", variable),
                     std::logic_error);
    }
}

/*****************************************************************************/
TEST_F(TestModel, create_expression_arg_expression) {
    {
        printemps::model::Model<int, double> model;

        for (auto i = 0;
             i <
             printemps::model::ModelConstant::MAX_NUMBER_OF_EXPRESSION_PROXIES;
             i++) {
            auto name = "p" + std::to_string(i);
            auto expression =
                printemps::model::Expression<int, double>::create_instance();

            auto& p = model.create_expression(name, expression);
            EXPECT_EQ(i + 1,
                      static_cast<int>(model.expression_proxies().size()));
            EXPECT_EQ(i, p.index());
            EXPECT_EQ(&p, &model.expression_proxies().back());
            EXPECT_EQ(name, model.expression_names().back());
        }

        auto expression =
            printemps::model::Expression<int, double>::create_instance();
        ASSERT_THROW(model.create_expression("error", expression),
                     std::logic_error);
    }
    {
        printemps::model::Model<int, double> model;

        auto expression =
            printemps::model::Expression<int, double>::create_instance();
        ASSERT_THROW(model.create_expression("s p a c e", expression),
                     std::logic_error);
    }
}

/*****************************************************************************/
TEST_F(TestModel, create_constraint_scalar) {
    {
        printemps::model::Model<int, double> model;
        for (auto i = 0;
             i <
             printemps::model::ModelConstant::MAX_NUMBER_OF_CONSTRAINT_PROXIES;
             i++) {
            auto  name = "g" + std::to_string(i);
            auto& g    = model.create_constraint(name);
            EXPECT_EQ(i + 1,
                      static_cast<int>(model.constraint_proxies().size()));
            EXPECT_EQ(i, g.index());
            EXPECT_EQ(&g, &model.constraint_proxies().back());
            EXPECT_EQ(name, model.constraint_names().back());
        }
        ASSERT_THROW(model.create_constraint("error"), std::logic_error);
    }
    {
        printemps::model::Model<int, double> model;
        ASSERT_THROW(model.create_constraint("s p a c e"), std::logic_error);
    }
}

/*****************************************************************************/
TEST_F(TestModel, create_constraint_one_dimensional) {
    {
        printemps::model::Model<int, double> model;
        for (auto i = 0;
             i <
             printemps::model::ModelConstant::MAX_NUMBER_OF_CONSTRAINT_PROXIES;
             i++) {
            auto  name = "g" + std::to_string(i);
            auto& g    = model.create_constraints(name, 2);
            EXPECT_EQ(i + 1,
                      static_cast<int>(model.constraint_proxies().size()));
            EXPECT_EQ(i, g.index());
            EXPECT_EQ(&g, &model.constraint_proxies().back());
            EXPECT_EQ(name, model.constraint_names().back());
        }
        ASSERT_THROW(model.create_constraints("error", 2), std::logic_error);
    }
    {
        printemps::model::Model<int, double> model;
        ASSERT_THROW(model.create_constraints("s p a c e", 2),
                     std::logic_error);
    }
}

/*****************************************************************************/
TEST_F(TestModel, create_constraint_two_dimensional) {
    {
        printemps::model::Model<int, double> model;
        for (auto i = 0;
             i <
             printemps::model::ModelConstant::MAX_NUMBER_OF_CONSTRAINT_PROXIES;
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
                     std::logic_error);
    }
    {
        printemps::model::Model<int, double> model;
        ASSERT_THROW(model.create_constraints("s p a c e", {2, 3}),
                     std::logic_error);
    }
}

/*****************************************************************************/
TEST_F(TestModel, create_constraint_arg_constraint) {
    {
        printemps::model::Model<int, double> model;

        for (auto i = 0;
             i <
             printemps::model::ModelConstant::MAX_NUMBER_OF_CONSTRAINT_PROXIES;
             i++) {
            auto name = "g" + std::to_string(i);
            auto expression =
                printemps::model::Expression<int, double>::create_instance();
            auto constraint = expression <= 1;

            auto& g = model.create_constraint(name, constraint);
            EXPECT_EQ(i + 1,
                      static_cast<int>(model.constraint_proxies().size()));
            EXPECT_EQ(i, g.index());
            EXPECT_EQ(&g, &model.constraint_proxies().back());
            EXPECT_EQ(name, model.constraint_names().back());
        }

        auto expression =
            printemps::model::Expression<int, double>::create_instance();
        auto constraint = expression <= 1;
        ASSERT_THROW(model.create_constraint("error", constraint),
                     std::logic_error);
    }
    {
        printemps::model::Model<int, double> model;
        auto                                 expression =
            printemps::model::Expression<int, double>::create_instance();
        auto constraint = expression <= 1;
        ASSERT_THROW(model.create_constraint("s p a c e", constraint),
                     std::logic_error);
    }
}

/*****************************************************************************/
TEST_F(TestModel, minimize_arg_function) {
    printemps::model::Model<int, double> model;

    auto& x = model.create_variables("x", 10, -1, 1);
    auto& p = model.create_expression("p", x.sum() + 1);

    auto f = [&p](const printemps::neighborhood::Move<int, double>& a_MOVE) {
        return p.evaluate(a_MOVE);
    };
    model.minimize(f);

    EXPECT_EQ(true, model.is_defined_objective());
    EXPECT_EQ(true, model.is_minimization());
    EXPECT_EQ(1.0, model.sign());
    EXPECT_EQ(true, model.objective().expression().sensitivities().empty());
    EXPECT_EQ(0, model.objective().expression().constant_value());
    EXPECT_EQ(false, model.objective().is_linear());

    for (auto&& element : x.flat_indexed_variables()) {
        element = 1;
    }
    model.update();
    EXPECT_EQ(10 + 1, model.objective().value());
}

/*****************************************************************************/
TEST_F(TestModel, minimize_arg_expression_like) {
    printemps::model::Model<int, double> model;

    auto& x = model.create_variables("x", 10, -1, 1);
    auto& p = model.create_expression("p", x.sum() + 1);

    [[maybe_unused]] auto f =
        [&p](const printemps::neighborhood::Move<int, double>& a_MOVE) {
            return p.evaluate(a_MOVE);
        };
    model.minimize(p);

    EXPECT_EQ(true, model.is_defined_objective());
    EXPECT_EQ(true, model.is_minimization());
    EXPECT_EQ(1.0, model.sign());
    EXPECT_EQ(10, static_cast<int>(
                      model.objective().expression().sensitivities().size()));
    EXPECT_EQ(1, model.objective().expression().constant_value());
    EXPECT_EQ(true, model.objective().is_linear());

    for (auto&& element : x.flat_indexed_variables()) {
        element = 1;
    }
    model.update();
    EXPECT_EQ(10 + 1, model.objective().value());
}

/*****************************************************************************/
TEST_F(TestModel, minimize_arg_expression) {
    printemps::model::Model<int, double> model;

    auto& x = model.create_variables("x", 10, -1, 1);
    auto& p = model.create_expression("p", x.sum() + 1);

    [[maybe_unused]] auto f =
        [&p](const printemps::neighborhood::Move<int, double>& a_MOVE) {
            return p.evaluate(a_MOVE);
        };
    model.minimize(p(0));

    EXPECT_EQ(true, model.is_defined_objective());
    EXPECT_EQ(true, model.is_minimization());
    EXPECT_EQ(1.0, model.sign());
    EXPECT_EQ(10, static_cast<int>(
                      model.objective().expression().sensitivities().size()));
    EXPECT_EQ(1, model.objective().expression().constant_value());
    EXPECT_EQ(true, model.objective().is_linear());

    for (auto&& element : x.flat_indexed_variables()) {
        element = 1;
    }
    model.update();
    EXPECT_EQ(10 + 1, model.objective().value());
}

/*****************************************************************************/
TEST_F(TestModel, maximize_arg_function) {
    printemps::model::Model<int, double> model;

    auto& x = model.create_variables("x", 10, -1, 1);
    auto& p = model.create_expression("p", x.sum() + 1);

    auto f = [&p](const printemps::neighborhood::Move<int, double>& a_MOVE) {
        return p.evaluate(a_MOVE);
    };
    model.maximize(f);

    EXPECT_EQ(true, model.is_defined_objective());
    EXPECT_EQ(false, model.is_minimization());
    EXPECT_EQ(-1.0, model.sign());
    EXPECT_EQ(true, model.objective().expression().sensitivities().empty());
    EXPECT_EQ(0, model.objective().expression().constant_value());
    EXPECT_EQ(false, model.objective().is_linear());

    for (auto&& element : x.flat_indexed_variables()) {
        element = 1;
    }
    model.update();
    EXPECT_EQ(10 + 1, model.objective().value());
}

/*****************************************************************************/
TEST_F(TestModel, maximize_arg_expression_like) {
    printemps::model::Model<int, double> model;

    auto& x = model.create_variables("x", 10, -1, 1);
    auto& p = model.create_expression("p", x.sum() + 1);

    [[maybe_unused]] auto f =
        [&p](const printemps::neighborhood::Move<int, double>& a_MOVE) {
            return p.evaluate(a_MOVE);
        };
    model.maximize(p);

    EXPECT_EQ(true, model.is_defined_objective());
    EXPECT_EQ(false, model.is_minimization());
    EXPECT_EQ(-1.0, model.sign());
    EXPECT_EQ(10, static_cast<int>(
                      model.objective().expression().sensitivities().size()));
    EXPECT_EQ(1, model.objective().expression().constant_value());
    EXPECT_EQ(true, model.objective().is_linear());

    for (auto&& element : x.flat_indexed_variables()) {
        element = 1;
    }
    model.update();
    EXPECT_EQ(10 + 1, model.objective().value());
}

/*****************************************************************************/
TEST_F(TestModel, maximize_arg_expression) {
    printemps::model::Model<int, double> model;
    auto&                 x = model.create_variables("x", 10, -1, 1);
    auto&                 p = model.create_expression("p", x.sum() + 1);
    [[maybe_unused]] auto f =
        [&p](const printemps::neighborhood::Move<int, double>& a_MOVE) {
            return p.evaluate(a_MOVE);
        };
    model.maximize(p(0));

    EXPECT_EQ(true, model.is_defined_objective());
    EXPECT_EQ(false, model.is_minimization());
    EXPECT_EQ(-1.0, model.sign());
    EXPECT_EQ(10, static_cast<int>(
                      model.objective().expression().sensitivities().size()));
    EXPECT_EQ(1, model.objective().expression().constant_value());
    EXPECT_EQ(true, model.objective().is_linear());

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
TEST_F(TestModel, setup_variable_related_constraints) {
    printemps::model::Model<int, double> model;

    auto& x = model.create_variables("x", 10, 0, 1);
    auto& y = model.create_variables("y", {20, 30}, 0, 1);

    auto& g = model.create_constraints("g", 3);
    g(0)    = x.selection();
    g(1)    = y.selection();
    g(2)    = x(0) + y.sum({0, printemps::model::All}) >= 1;

    model.setup_variable_related_constraints();

    for (auto i = 0; i < 10; i++) {
        EXPECT_EQ(true, x(i).related_constraint_ptrs().find(&g(0)) !=
                            x(i).related_constraint_ptrs().end());
        EXPECT_EQ(false, x(i).related_constraint_ptrs().find(&g(1)) !=
                             x(i).related_constraint_ptrs().end());
        /// Only x(0) is related to g(2).
        EXPECT_EQ(i == 0, x(i).related_constraint_ptrs().find(&g(2)) !=
                              x(i).related_constraint_ptrs().end());
    }

    for (auto i = 0; i < 20; i++) {
        for (auto j = 0; j < 30; j++) {
            EXPECT_EQ(false, y(i, j).related_constraint_ptrs().find(&g(0)) !=
                                 y(i, j).related_constraint_ptrs().end());
            EXPECT_EQ(true, y(i, j).related_constraint_ptrs().find(&g(1)) !=
                                y(i, j).related_constraint_ptrs().end());
            /// Only y(0,*) is related to g(2).
            EXPECT_EQ(i == 0, y(i, j).related_constraint_ptrs().find(&g(2)) !=
                                  y(i, j).related_constraint_ptrs().end());
        }
    }
}

/*****************************************************************************/
TEST_F(TestModel, setup_unique_name) {
    printemps::model::Model<int, double> model;

    auto& x = model.create_variable("x");
    auto& p = model.create_variables("p", {10});
    auto& g = model.create_variables("g", {20, 30});

    x.set_name("_x");
    p(0).set_name("_p_0");
    p(9).set_name("_p_9");
    g(0, 0).set_name("_g_0_0");
    g(19, 29).set_name("_g_19_29");
    model.setup_unique_name();

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
TEST_F(TestModel, setup_is_linear) {
    /// Constraint: linear
    /// Objective: linear
    {
        printemps::model::Model<int, double> model;

        auto& x = model.create_variable("x");

        model.create_constraint("g", x <= 0);
        model.minimize(x);

        model.setup_is_linear();

        EXPECT_EQ(true, model.is_linear());
    }

    /// Constraint: nonlinear (user-defined lambda)
    /// Objective: linear
    {
        printemps::model::Model<int, double> model;

        auto& x = model.create_variable("x");

        std::function<double(const printemps::neighborhood::Move<int, double>&)>
            g = [&x](const printemps::neighborhood::Move<int, double>& a_MOVE) {
                return x.evaluate(a_MOVE);
            };

        model.create_constraint("g", g <= 0);
        model.minimize(x);

        model.setup_is_linear();

        EXPECT_EQ(false, model.is_linear());
    }

    /// Constraint: linear
    /// Objective: nonlinear (user-defined lambda)
    {
        printemps::model::Model<int, double> model;

        auto& x = model.create_variable("x");

        std::function<double(const printemps::neighborhood::Move<int, double>&)>
            f = [&x](const printemps::neighborhood::Move<int, double>& a_MOVE) {
                return x.evaluate(a_MOVE);
            };

        model.create_constraint("g", x <= 0);
        model.minimize(f);

        model.setup_is_linear();

        EXPECT_EQ(false, model.is_linear());
    }

    /// Constraint: nonlinear
    /// Objective: nonlinear
    {
        printemps::model::Model<int, double> model;

        auto& x = model.create_variable("x");

        std::function<double(const printemps::neighborhood::Move<int, double>&)>
            g = [&x](const printemps::neighborhood::Move<int, double>& a_MOVE) {
                return x.evaluate(a_MOVE);
            };

        std::function<double(const printemps::neighborhood::Move<int, double>&)>
            f = [&x](const printemps::neighborhood::Move<int, double>& a_MOVE) {
                return x.evaluate(a_MOVE);
            };

        model.create_constraint("g", g <= 0);
        model.minimize(f);

        model.setup_is_linear();

        EXPECT_EQ(false, model.is_linear());
    }
}

/*****************************************************************************/
TEST_F(TestModel, setup_is_enabled_fast_evaluation) {
    /// Constraint: linear
    /// Objective: linear
    /// User-defined neighborhood: None
    {
        printemps::model::Model<int, double> model;

        auto& x = model.create_variable("x");

        model.create_constraint("g", x <= 0);
        model.minimize(x);

        model.setup_is_enabled_fast_evaluation();

        EXPECT_EQ(true, model.is_enabled_fast_evaluation());
    }

    /// Constraint: nonlinear (user-defined lambda)
    /// Objective: linear
    /// User-defined neighborhood: None
    {
        printemps::model::Model<int, double> model;

        auto& x = model.create_variable("x");

        std::function<double(const printemps::neighborhood::Move<int, double>&)>
            g = [&x](const printemps::neighborhood::Move<int, double>& a_MOVE) {
                return x.evaluate(a_MOVE);
            };

        model.create_constraint("g", g <= 0);
        model.minimize(x);

        model.setup_is_enabled_fast_evaluation();

        EXPECT_EQ(false, model.is_enabled_fast_evaluation());
    }

    /// Constraint: linear
    /// Objective: nonlinear (user-defined lambda)
    /// User-defined neighborhood: None
    {
        printemps::model::Model<int, double> model;

        auto& x = model.create_variable("x");

        std::function<double(const printemps::neighborhood::Move<int, double>&)>
            f = [&x](const printemps::neighborhood::Move<int, double>& a_MOVE) {
                return x.evaluate(a_MOVE);
            };

        model.create_constraint("g", x <= 0);
        model.minimize(f);

        model.setup_is_enabled_fast_evaluation();

        EXPECT_EQ(true, model.is_enabled_fast_evaluation());
    }

    /// Constraint: linear
    /// Objective: linear
    /// User-defined neighborhood: Yes
    {
        printemps::model::Model<int, double> model;

        auto& x = model.create_variable("x");

        model.create_constraint("g", x <= 0);
        model.minimize(x);

        auto move_updater =
            [&x]([[maybe_unused]] std::vector<
                 printemps::neighborhood::Move<int, double>>* a_moves) { ; };

        model.neighborhood().set_user_defined_move_updater(move_updater);
        model.neighborhood().enable_user_defined_move();
        model.setup_is_enabled_fast_evaluation();

        EXPECT_EQ(false, model.is_enabled_fast_evaluation());
    }
}

/*****************************************************************************/
TEST_F(TestModel, setup_variable_sensitivity) {
    printemps::model::Model<int, double> model;

    auto& x = model.create_variables("x", 10, 0, 1);
    auto& y = model.create_variables("y", {20, 30}, 0, 1);
    auto& g = model.create_constraints("g", 3);

    auto& p = model.create_expressions("p", 3);
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

    model.minimize(2 * x.sum() + 5 * y.sum());

    model.setup_variable_sensitivity();

    for (auto i = 0; i < 10; i++) {
        EXPECT_EQ(i + 1, x(i).constraint_sensitivities().at(&g(0)));
        EXPECT_EQ(2, x(i).objective_sensitivity());
    }

    for (auto i = 0; i < 20; i++) {
        for (auto j = 0; j < 30; j++) {
            EXPECT_EQ(i + j + 1, y(i, j).constraint_sensitivities().at(&g(1)));
            EXPECT_EQ(5, y(i, j).objective_sensitivity());
        }
    }
}

/*****************************************************************************/
TEST_F(TestModel, categorize_variables) {
    printemps::model::Model<int, double> model;

    auto& x = model.create_variable("x", 0, 1);
    auto& y = model.create_variables("y", 10, 0, 1);
    auto& z = model.create_variables("z", {20, 30}, -10, 10);

    x.fix_by(0);
    y(0).fix_by(0);
    z(0, 0).fix_by(0);

    model.categorize_variables();

    EXPECT_EQ(1 + 10 + 20 * 30, model.number_of_variables());
    EXPECT_EQ(3, model.number_of_fixed_variables());
    EXPECT_EQ(1 + 10 + 20 * 30 - 3, model.number_of_not_fixed_variables());
    EXPECT_EQ(11, model.number_of_binary_variables());
    EXPECT_EQ(600, model.number_of_integer_variables());
}

/*****************************************************************************/
TEST_F(TestModel, categorize_constraints) {
    printemps::model::Model<int, double> model;
    auto coefficients = printemps::utility::sequence(10);

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

    auto& equation_knapsack = model.create_constraint("equation_knapsack");
    equation_knapsack       = z.dot(coefficients) == 30;

    auto& bin_packing = model.create_constraints("bin_packing", 2);
    bin_packing(0)    = z.dot(coefficients) + 5 * w <= 5;
    bin_packing(1)    = z.dot(coefficients) - 5 * w >= -5;

    auto& knapsack = model.create_constraints("knapsack", 2);
    knapsack(0)    = z.dot(coefficients) <= 50;
    knapsack(1)    = z.dot(coefficients) >= -50;

    auto& integer_knapsack = model.create_constraints("integer_knapsack", 2);
    integer_knapsack(0)    = r.dot(coefficients) <= 50;
    integer_knapsack(1)    = r.dot(coefficients) >= -50;

    auto& general_liner = model.create_constraint("general_liner");
    general_liner       = x + r.sum() == 50;

    auto& nonlinear = model.create_constraint("nonlinear");
    std::function<double(const printemps::neighborhood::Move<int, double>&)> f =
        [&x](const printemps::neighborhood::Move<int, double>& a_MOVE) {
            return x.evaluate(a_MOVE) - 1;
        };
    nonlinear = f <= 5;

    singleton.disable();

    model.categorize_variables();
    model.categorize_constraints();
    model.extract_selections(printemps::model::SelectionMode::Defined);

    EXPECT_EQ(22, model.number_of_constraints());
    EXPECT_EQ(1, model.number_of_selection_constraints());
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
    EXPECT_EQ(1, static_cast<int>(reference.equation_knapsack_ptrs.size()));
    EXPECT_EQ(2, static_cast<int>(reference.bin_packing_ptrs.size()));
    EXPECT_EQ(2, static_cast<int>(reference.knapsack_ptrs.size()));
    EXPECT_EQ(2, static_cast<int>(reference.integer_knapsack_ptrs.size()));
    EXPECT_EQ(1, static_cast<int>(reference.general_linear_ptrs.size()));
    EXPECT_EQ(1, static_cast<int>(reference.nonlinear_ptrs.size()));
}

/*****************************************************************************/
TEST_F(TestModel, setup_variable_related_monic_constraint_ptrs) {
    printemps::model::Model<int, double> model;

    auto& x = model.create_variables("x", 10, 0, 1);
    auto& y = model.create_variables("y", 10, 0, 10);

    auto& g = model.create_constraints("g", 4);
    g(0)    = x.selection();
    g(1)    = 2 * x.sum() <= 2;
    g(2)    = y.sum() <= 2;
    g(3)    = x.sum() + y.sum() >= 1;

    model.setup_variable_related_constraints();
    model.categorize_constraints();
    model.setup_variable_related_monic_constraints();

    for (auto i = 0; i < 10; i++) {
        EXPECT_EQ(true, x(i).related_monic_constraint_ptrs().find(&g(0)) !=
                            x(i).related_monic_constraint_ptrs().end());
        EXPECT_EQ(false, x(i).related_monic_constraint_ptrs().find(&g(1)) !=
                             x(i).related_monic_constraint_ptrs().end());
        EXPECT_EQ(false, x(i).related_monic_constraint_ptrs().find(&g(2)) !=
                             x(i).related_monic_constraint_ptrs().end());
        EXPECT_EQ(false, x(i).related_monic_constraint_ptrs().find(&g(3)) !=
                             x(i).related_monic_constraint_ptrs().end());
    }

    for (auto i = 0; i < 10; i++) {
        EXPECT_EQ(false, y(i).related_monic_constraint_ptrs().find(&g(0)) !=
                             y(i).related_monic_constraint_ptrs().end());
        EXPECT_EQ(false, y(i).related_monic_constraint_ptrs().find(&g(1)) !=
                             y(i).related_monic_constraint_ptrs().end());
        EXPECT_EQ(false, y(i).related_monic_constraint_ptrs().find(&g(2)) !=
                             y(i).related_monic_constraint_ptrs().end());
        EXPECT_EQ(false, y(i).related_monic_constraint_ptrs().find(&g(3)) !=
                             y(i).related_monic_constraint_ptrs().end());
    }

    for (auto i = 0; i < 10; i++) {
        x(i).reset_related_monic_constraint_ptrs();
        EXPECT_EQ(true, x(i).related_monic_constraint_ptrs().empty());
    }
}

/*****************************************************************************/
TEST_F(TestModel, extract_selections_larger) {
    printemps::model::Model<int, double> model;

    auto& x_0 = model.create_variables("x_0", {10, 10}, 0, 1);
    auto& x_1 = model.create_variables("x_1", {20, 20}, 0, 1);
    auto& x_2 = model.create_variables("x_2", 2, 0, 1);

    auto& y = model.create_variables("y", {30, 30}, -10, 10);

    /**
     * Selection constraint with 10 decision variables. The priority of this
     * constraint is the third, and it will be employed for a swap
     * neighborhood.
     */
    model.create_constraint("c_0",
                            x_0.selection({0, printemps::model::Range::All}));

    /**
     * Selection constraint with 32 decision variables. The priority of this
     * constraint is the second, and it will NOT be employed for a swap
     * neighborhood because higher-priority constraint c_1 covers x_1.
     */
    model.create_constraint(
        "c_1", (x_0.sum({1, printemps::model::Range::All}) +
                x_1.sum({1, printemps::model::Range::All}) + x_2(0)) == 1);

    /**
     * Selection constraint with 400 decision variables. The priority of this
     * constraint is the first, and it will be employed for a swap
     * neighborhood.
     */
    model.create_constraint("c_2", x_1.selection());

    /**
     * Selection constraint with 2 decision variables. The priority of this
     * constraint is the fourth, and it will NOT be employed for a swap
     * neighborhood.
     */
    model.create_constraint("c_3", x_2.selection());

    model.categorize_variables();
    model.categorize_constraints();
    model.extract_selections(printemps::model::SelectionMode::Larger);

    EXPECT_EQ(2, model.number_of_selection_constraints());
    EXPECT_EQ(2, static_cast<int>(model.selections().size()));

    /**
     * Check the numbers of covered variables and variable pointers.
     */
    {
        /// Constraint c_2
        auto variable_ptrs = model.selections()[0].variable_ptrs;
        EXPECT_EQ(400, static_cast<int>(variable_ptrs.size()));

        EXPECT_EQ(true, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                   &x_1(0, 0)) != variable_ptrs.end()));
        EXPECT_EQ(true, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                   &x_1(19, 19)) != variable_ptrs.end()));
    }

    {
        /// Constraint c_0
        auto variable_ptrs = model.selections()[1].variable_ptrs;
        EXPECT_EQ(10, static_cast<int>(variable_ptrs.size()));

        EXPECT_EQ(true, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                   &x_0(0, 0)) != variable_ptrs.end()));
        EXPECT_EQ(true, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                   &x_0(0, 9)) != variable_ptrs.end()));
        EXPECT_EQ(false, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                    &x_0(1, 0)) != variable_ptrs.end()));
    }

    /**
     * Check whether the corresponding constraint is enabled or not.
     */
    /// Constraint c_2
    EXPECT_EQ(false, model.selections()[0].constraint_ptr->is_enabled());

    /// Constraint c_0
    EXPECT_EQ(false, model.selections()[1].constraint_ptr->is_enabled());

    /**
     * Check the number of covered variables and variable pointers for each
     * category. */

    /// Selection
    {
        auto variable_ptrs = model.variable_reference().selection_variable_ptrs;
        EXPECT_EQ(20 * 20 + 1 * 10, model.number_of_selection_variables());

        /// Constraint c_2
        EXPECT_EQ(true, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                   &x_1(0, 0)) != variable_ptrs.end()));
        EXPECT_EQ(true, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                   &x_1(19, 19)) != variable_ptrs.end()));

        /// Constraint c_0
        EXPECT_EQ(true, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                   &x_0(0, 0)) != variable_ptrs.end()));
        EXPECT_EQ(true, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                   &x_0(0, 9)) != variable_ptrs.end()));
        EXPECT_EQ(false, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                    &x_0(1, 0)) != variable_ptrs.end()));
    }

    /// Binary
    {
        auto variable_ptrs = model.variable_reference().binary_variable_ptrs;
        EXPECT_EQ(10 * 10 + 20 * 20 + 2 - (20 * 20 + 1 * 10),
                  model.number_of_binary_variables());

        /// Constraint c_2
        EXPECT_EQ(false, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                    &x_1(0, 0)) != variable_ptrs.end()));
        EXPECT_EQ(false, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                    &x_1(19, 19)) != variable_ptrs.end()));

        /// Constraint c_0
        EXPECT_EQ(false, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                    &x_0(0, 0)) != variable_ptrs.end()));
        EXPECT_EQ(false, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                    &x_0(0, 9)) != variable_ptrs.end()));
        EXPECT_EQ(true, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                   &x_0(1, 0)) != variable_ptrs.end()));
    }

    /// Integer
    {
        auto variable_ptrs = model.variable_reference().integer_variable_ptrs;
        EXPECT_EQ(30 * 30, model.number_of_integer_variables());

        EXPECT_EQ(true, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                   &y(0, 0)) != variable_ptrs.end()));
        EXPECT_EQ(true, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                   &y(29, 29)) != variable_ptrs.end()));
    }
}

/*****************************************************************************/
TEST_F(TestModel, extract_selections_independent) {
    printemps::model::Model<int, double> model;

    auto& x_0 = model.create_variables("x_0", {10, 10}, 0, 1);
    auto& x_1 = model.create_variables("x_1", {20, 20}, 0, 1);
    auto& x_2 = model.create_variables("x_2", 2, 0, 1);

    auto& y = model.create_variables("y", {30, 30}, -10, 10);

    /**
     * Selection constraint with 10 decision variables (no overlap).
     */
    model.create_constraint("c_0",
                            x_0.selection({0, printemps::model::Range::All}));

    /**
     * Selection constraint with 32 decision variables (overlap).
     */
    model.create_constraint(
        "c_1", (x_0.sum({1, printemps::model::Range::All}) +
                x_1.sum({1, printemps::model::Range::All}) + x_2(0)) == 1);

    /**
     * Selection constraint with 400 decision variables (overlap).
     */
    model.create_constraint("c_2", x_1.selection());

    /**
     * Selection constraint with 2 decision variables (overlap).
     */
    model.create_constraint("c_3", x_2.selection());

    model.categorize_variables();
    model.categorize_constraints();
    model.extract_selections(printemps::model::SelectionMode::Independent);

    EXPECT_EQ(1, model.number_of_selection_constraints());
    EXPECT_EQ(1, static_cast<int>(model.selections().size()));

    /**
     * Check the numbers of covered variables and variable pointers.
     */
    {
        /// Constraint c_0
        auto variable_ptrs = model.selections()[0].variable_ptrs;
        EXPECT_EQ(10, static_cast<int>(variable_ptrs.size()));

        EXPECT_EQ(true, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                   &x_0(0, 0)) != variable_ptrs.end()));
        EXPECT_EQ(true, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                   &x_0(0, 9)) != variable_ptrs.end()));
        EXPECT_EQ(false, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                    &x_0(1, 0)) != variable_ptrs.end()));
    }

    /**
     * Check whether the corresponding constraint is enabled or not.
     */
    /// Constraint c_0
    EXPECT_EQ(false, model.selections()[0].constraint_ptr->is_enabled());

    /**
     * Check the number of covered variables and variable pointers for each
     * category. */
    /// Selection
    {
        auto variable_ptrs = model.variable_reference().selection_variable_ptrs;
        EXPECT_EQ(10, model.number_of_selection_variables());

        /// Constraint c_0
        EXPECT_EQ(true, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                   &x_0(0, 0)) != variable_ptrs.end()));
        EXPECT_EQ(true, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                   &x_0(0, 9)) != variable_ptrs.end()));
        EXPECT_EQ(false, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                    &x_0(1, 0)) != variable_ptrs.end()));
    }

    /// Binary
    {
        auto variable_ptrs = model.variable_reference().binary_variable_ptrs;
        EXPECT_EQ(10 * 10 + 20 * 20 + 2 - 10,
                  model.number_of_binary_variables());

        /// Constraint c_0
        EXPECT_EQ(false, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                    &x_0(0, 0)) != variable_ptrs.end()));
        EXPECT_EQ(false, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                    &x_0(0, 9)) != variable_ptrs.end()));
        EXPECT_EQ(true, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                   &x_0(1, 0)) != variable_ptrs.end()));
    }

    /// Integer
    {
        auto variable_ptrs = model.variable_reference().integer_variable_ptrs;
        EXPECT_EQ(30 * 30, model.number_of_integer_variables());

        EXPECT_EQ(true, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                   &y(0, 0)) != variable_ptrs.end()));
        EXPECT_EQ(true, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                   &y(29, 29)) != variable_ptrs.end()));
    }
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
TEST_F(TestModel, set_callback) {
    printemps::model::Model<int, double> model;

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
    printemps::model::Model<int, double> model;

    auto& x = model.create_variable("x");
    auto& y = model.create_variables("y", 10);
    auto& z = model.create_variables("z", {10, 10});

    printemps::model::ValueProxy<int> x_value(x.index());
    printemps::model::ValueProxy<int> y_value(y.index(), 10);
    printemps::model::ValueProxy<int> z_value(z.index(), {10, 10});

    x_value.value() = 1;

    for (auto i = 0; i < 10; i++) {
        y_value(i) = 10 * i;
    }

    for (auto i = 0; i < 10; i++) {
        for (auto j = 0; j < 10; j++) {
            z_value(i, j) = 100 * (i + j);
        }
    }

    model.import_variable_values({x_value, y_value, z_value});

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
TEST_F(TestModel, update_arg_void) {
    printemps::model::Model<int, double> model;

    auto sequence = printemps::utility::sequence(10);

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
    printemps::model::Model<int, double> model;

    auto sequence = printemps::utility::sequence(10);

    auto& x = model.create_variables("x", 10, 0, 1);
    auto& p = model.create_expression("p", x.dot(sequence) + 1);
    model.create_constraint("g", x.selection());

    x(0) = 1;

    model.minimize(p);
    model.categorize_variables();
    model.categorize_constraints();
    model.extract_selections(printemps::model::SelectionMode::Defined);
    model.setup_fixed_sensitivities(false);

    model.update();

    printemps::neighborhood::Move<int, double> move;
    move.sense = printemps::neighborhood::MoveSense::Selection;
    move.alterations.emplace_back(&x(0), 0);
    move.alterations.emplace_back(&x(9), 1);

    model.update(move);

    EXPECT_EQ(10, p.value());
    EXPECT_EQ(10, p(0).value());
    EXPECT_EQ(10, model.objective().value());
    EXPECT_EQ(&x(9), model.selections().front().selected_variable_ptr);
}

/*****************************************************************************/
TEST_F(TestModel, reset_variable_objective_improvability_arg_void) {
    printemps::model::Model<int, double> model;

    auto& x = model.create_variable("x", 0, 1);
    auto& y = model.create_variables("y", 10, 0, 1);
    model.categorize_variables();

    x(0).set_is_objective_improvable(true);
    EXPECT_EQ(true, x(0).is_objective_improvable());
    for (auto i = 0; i < 10; i++) {
        y(i).set_is_objective_improvable(true);
        EXPECT_EQ(true, y(i).is_objective_improvable());
    }
    model.reset_variable_objective_improvability();

    EXPECT_EQ(false, x(0).is_objective_improvable());
    for (auto i = 0; i < 10; i++) {
        EXPECT_EQ(false, y(i).is_objective_improvable());
    }
}

/*****************************************************************************/
TEST_F(TestModel, reset_variable_objective_improvability_arg_variable_ptrs) {
    printemps::model::Model<int, double> model;

    auto& x = model.create_variable("x", 0, 1);
    auto& y = model.create_variables("y", 10, 0, 1);
    model.categorize_variables();

    x(0).set_is_objective_improvable(true);
    EXPECT_EQ(true, x(0).is_objective_improvable());
    for (auto i = 0; i < 10; i++) {
        y(i).set_is_objective_improvable(true);
        EXPECT_EQ(true, y(i).is_objective_improvable());
    }
    model.reset_variable_objective_improvability({&x(0), &y(0), &y(9)});

    EXPECT_EQ(false, x(0).is_objective_improvable());
    EXPECT_EQ(false, y(0).is_objective_improvable());
    EXPECT_EQ(false, y(9).is_objective_improvable());
    for (auto i = 1; i < 9; i++) {
        EXPECT_EQ(true, y(i).is_objective_improvable());
    }
}

/*****************************************************************************/
TEST_F(TestModel, reset_variable_feasibility_improvability_arg_void) {
    printemps::model::Model<int, double> model;

    auto& x = model.create_variable("x", 0, 1);
    auto& y = model.create_variables("y", 10, 0, 1);
    model.categorize_variables();

    x(0).set_is_feasibility_improvable(true);
    EXPECT_EQ(true, x(0).is_feasibility_improvable());
    for (auto i = 0; i < 10; i++) {
        y(i).set_is_feasibility_improvable(true);
        EXPECT_EQ(true, y(i).is_feasibility_improvable());
    }
    model.reset_variable_feasibility_improvability();

    EXPECT_EQ(false, x(0).is_feasibility_improvable());
    for (auto i = 0; i < 10; i++) {
        EXPECT_EQ(false, y(i).is_feasibility_improvable());
    }
}

/*****************************************************************************/
TEST_F(TestModel, reset_variable_feasibility_improvability_arg_variable_ptrs) {
    printemps::model::Model<int, double> model;

    auto& x = model.create_variable("x", 0, 1);
    auto& y = model.create_variables("y", 10, 0, 1);
    model.categorize_variables();

    x(0).set_is_feasibility_improvable(true);
    EXPECT_EQ(true, x(0).is_feasibility_improvable());
    for (auto i = 0; i < 10; i++) {
        y(i).set_is_feasibility_improvable(true);
        EXPECT_EQ(true, y(i).is_feasibility_improvable());
    }
    model.reset_variable_feasibility_improvability({&x(0), &y(0), &y(9)});

    EXPECT_EQ(false, x(0).is_feasibility_improvable());
    EXPECT_EQ(false, y(0).is_feasibility_improvable());
    EXPECT_EQ(false, y(9).is_feasibility_improvable());
    for (auto i = 1; i < 9; i++) {
        EXPECT_EQ(true, y(i).is_feasibility_improvable());
    }
}

/*****************************************************************************/
TEST_F(TestModel,
       reset_variable_feasibility_improvability_arg_constraint_ptrs) {
    printemps::model::Model<int, double> model;

    auto& x = model.create_variable("x", 0, 1);
    auto& y = model.create_variables("y", 10, 0, 1);
    auto& g = model.create_constraints("g", 2);

    g(0) = x <= y(0);
    g(1) = y(1) == y(9);

    model.categorize_variables();
    model.categorize_constraints();

    x(0).set_is_feasibility_improvable(true);
    EXPECT_EQ(true, x(0).is_feasibility_improvable());
    for (auto i = 0; i < 10; i++) {
        y(i).set_is_feasibility_improvable(true);
        EXPECT_EQ(true, y(i).is_feasibility_improvable());
    }
    std::vector<printemps::model::Constraint<int, double>*> constraint_ptrs = {
        &g(0), &g(1)};
    model.reset_variable_feasibility_improvability(constraint_ptrs);

    EXPECT_EQ(false, x(0).is_feasibility_improvable());
    EXPECT_EQ(false, y(0).is_feasibility_improvable());
    EXPECT_EQ(false, y(1).is_feasibility_improvable());
    EXPECT_EQ(false, y(9).is_feasibility_improvable());
    for (auto i = 2; i < 9; i++) {
        EXPECT_EQ(true, y(i).is_feasibility_improvable());
    }
}

/*****************************************************************************/
TEST_F(TestModel, update_variable_improvability) {
    {
        printemps::model::Model<int, double> model;

        auto& x = model.create_variable("x", -10, 10);
        auto& y = model.create_variable("y", -10, 10);

        [[maybe_unused]] auto& g = model.create_constraint("g", x - y <= 0);

        model.minimize(-x + y);
        model.categorize_variables();
        model.categorize_constraints();
        model.setup_variable_sensitivity();
        model.setup_fixed_sensitivities(false);

        x = -10;
        y = -10;
        model.update();
        model.update_variable_objective_improvability();
        model.update_variable_feasibility_improvability();
        EXPECT_EQ(true, x(0).is_objective_improvable());
        EXPECT_EQ(false, y(0).is_objective_improvable());
        EXPECT_EQ(false, x(0).is_feasibility_improvable());
        EXPECT_EQ(false, y(0).is_feasibility_improvable());

        x = 10;
        y = 10;
        model.update();
        model.update_variable_objective_improvability();
        model.update_variable_feasibility_improvability();
        EXPECT_EQ(false, x(0).is_objective_improvable());
        EXPECT_EQ(true, y(0).is_objective_improvable());
        EXPECT_EQ(false, x(0).is_feasibility_improvable());
        EXPECT_EQ(false, y(0).is_feasibility_improvable());

        x = 10;
        y = -10;
        model.update();
        model.update_variable_objective_improvability();
        model.update_variable_feasibility_improvability();
        EXPECT_EQ(false, x(0).is_objective_improvable());
        EXPECT_EQ(false, y(0).is_objective_improvable());
        EXPECT_EQ(true, x(0).is_feasibility_improvable());
        EXPECT_EQ(true, y(0).is_feasibility_improvable());
    }
    {
        printemps::model::Model<int, double> model;

        auto& x = model.create_variable("x", -10, 10);
        auto& y = model.create_variable("y", -10, 10);

        [[maybe_unused]] auto& g = model.create_constraint("g", x - y == 0);

        model.minimize(-x + y);
        model.categorize_variables();
        model.categorize_constraints();
        model.setup_variable_sensitivity();
        model.setup_fixed_sensitivities(false);

        x = -10;
        y = -10;
        model.update();
        model.update_variable_objective_improvability();
        model.update_variable_feasibility_improvability();
        EXPECT_EQ(true, x(0).is_objective_improvable());
        EXPECT_EQ(false, y(0).is_objective_improvable());
        EXPECT_EQ(false, x(0).is_feasibility_improvable());
        EXPECT_EQ(false, y(0).is_feasibility_improvable());

        x = 10;
        y = 10;
        model.update();
        model.update_variable_objective_improvability();
        model.update_variable_feasibility_improvability();
        EXPECT_EQ(false, x(0).is_objective_improvable());
        EXPECT_EQ(true, y(0).is_objective_improvable());
        EXPECT_EQ(false, x(0).is_feasibility_improvable());
        EXPECT_EQ(false, y(0).is_feasibility_improvable());

        x = 10;
        y = -10;
        model.update();
        model.update_variable_objective_improvability();
        model.update_variable_feasibility_improvability();
        EXPECT_EQ(false, x(0).is_objective_improvable());
        EXPECT_EQ(false, y(0).is_objective_improvable());
        EXPECT_EQ(true, x(0).is_feasibility_improvable());
        EXPECT_EQ(true, y(0).is_feasibility_improvable());
    }
    {
        printemps::model::Model<int, double> model;

        auto& x = model.create_variable("x", -10, 10);
        auto& y = model.create_variable("y", -10, 10);

        [[maybe_unused]] auto& g = model.create_constraint("g", x - y >= 0);

        model.minimize(-x + y);
        model.categorize_variables();
        model.categorize_constraints();
        model.setup_variable_sensitivity();
        model.setup_fixed_sensitivities(false);

        x = -10;
        y = -10;
        model.update();
        model.update_variable_objective_improvability();
        model.update_variable_feasibility_improvability();
        EXPECT_EQ(true, x(0).is_objective_improvable());
        EXPECT_EQ(false, y(0).is_objective_improvable());
        EXPECT_EQ(false, x(0).is_feasibility_improvable());
        EXPECT_EQ(false, y(0).is_feasibility_improvable());

        x = 10;
        y = 10;
        model.update();
        model.update_variable_objective_improvability();
        model.update_variable_feasibility_improvability();
        EXPECT_EQ(false, x(0).is_objective_improvable());
        EXPECT_EQ(true, y(0).is_objective_improvable());
        EXPECT_EQ(false, x(0).is_feasibility_improvable());
        EXPECT_EQ(false, y(0).is_feasibility_improvable());

        x = -10;
        y = 10;
        model.update();
        model.update_variable_objective_improvability();
        model.update_variable_feasibility_improvability();
        EXPECT_EQ(true, x(0).is_objective_improvable());
        EXPECT_EQ(true, y(0).is_objective_improvable());
        EXPECT_EQ(true, x(0).is_feasibility_improvable());
        EXPECT_EQ(true, y(0).is_feasibility_improvable());
    }
    {
        printemps::model::Model<int, double> model;

        auto& x = model.create_variable("x", -10, 10);
        auto& y = model.create_variable("y", -10, 10);

        [[maybe_unused]] auto& g = model.create_constraint("g", x - y <= 0);

        model.maximize(-x + y);
        model.categorize_variables();
        model.categorize_constraints();
        model.setup_variable_sensitivity();
        model.setup_fixed_sensitivities(false);

        x = -10;
        y = -10;

        model.update();
        model.update_variable_objective_improvability();
        model.update_variable_feasibility_improvability();
        EXPECT_EQ(false, x(0).is_objective_improvable());
        EXPECT_EQ(true, y(0).is_objective_improvable());
        EXPECT_EQ(false, x(0).is_feasibility_improvable());
        EXPECT_EQ(false, y(0).is_feasibility_improvable());

        x = 10;
        y = 10;
        model.update();
        model.update_variable_objective_improvability();
        model.update_variable_feasibility_improvability();
        EXPECT_EQ(true, x(0).is_objective_improvable());
        EXPECT_EQ(false, y(0).is_objective_improvable());
        EXPECT_EQ(false, x(0).is_feasibility_improvable());
        EXPECT_EQ(false, y(0).is_feasibility_improvable());

        x = 10;
        y = -10;
        model.update();
        model.update_variable_objective_improvability();
        model.update_variable_feasibility_improvability();
        EXPECT_EQ(true, x(0).is_objective_improvable());
        EXPECT_EQ(true, y(0).is_objective_improvable());
        EXPECT_EQ(true, x(0).is_feasibility_improvable());
        EXPECT_EQ(true, y(0).is_feasibility_improvable());
    }
    {
        printemps::model::Model<int, double> model;

        auto& x = model.create_variable("x", -10, 10);
        auto& y = model.create_variable("y", -10, 10);

        [[maybe_unused]] auto& g = model.create_constraint("g", x - y == 0);

        model.maximize(-x + y);
        model.categorize_variables();
        model.categorize_constraints();
        model.setup_variable_sensitivity();
        model.setup_fixed_sensitivities(false);

        x = -10;
        y = -10;

        model.update();
        model.update_variable_objective_improvability();
        model.update_variable_feasibility_improvability();
        EXPECT_EQ(false, x(0).is_objective_improvable());
        EXPECT_EQ(true, y(0).is_objective_improvable());
        EXPECT_EQ(false, x(0).is_feasibility_improvable());
        EXPECT_EQ(false, y(0).is_feasibility_improvable());

        x = 10;
        y = 10;
        model.update();
        model.update_variable_objective_improvability();
        model.update_variable_feasibility_improvability();
        EXPECT_EQ(true, x(0).is_objective_improvable());
        EXPECT_EQ(false, y(0).is_objective_improvable());
        EXPECT_EQ(false, x(0).is_feasibility_improvable());
        EXPECT_EQ(false, y(0).is_feasibility_improvable());

        x = 10;
        y = -10;
        model.update();
        model.update_variable_objective_improvability();
        model.update_variable_feasibility_improvability();
        EXPECT_EQ(true, x(0).is_objective_improvable());
        EXPECT_EQ(true, y(0).is_objective_improvable());
        EXPECT_EQ(true, x(0).is_feasibility_improvable());
        EXPECT_EQ(true, y(0).is_feasibility_improvable());
    }
    {
        printemps::model::Model<int, double> model;

        auto& x = model.create_variable("x", -10, 10);
        auto& y = model.create_variable("y", -10, 10);

        [[maybe_unused]] auto& g = model.create_constraint("g", x - y >= 0);

        model.maximize(-x + y);
        model.categorize_variables();
        model.categorize_constraints();
        model.setup_variable_sensitivity();
        model.setup_fixed_sensitivities(false);

        x = -10;
        y = -10;

        model.update();
        model.update_variable_objective_improvability();
        model.update_variable_feasibility_improvability();
        EXPECT_EQ(false, x(0).is_objective_improvable());
        EXPECT_EQ(true, y(0).is_objective_improvable());
        EXPECT_EQ(false, x(0).is_feasibility_improvable());
        EXPECT_EQ(false, y(0).is_feasibility_improvable());

        x = 10;
        y = 10;
        model.update();
        model.update_variable_objective_improvability();
        model.update_variable_feasibility_improvability();
        EXPECT_EQ(true, x(0).is_objective_improvable());
        EXPECT_EQ(false, y(0).is_objective_improvable());
        EXPECT_EQ(false, x(0).is_feasibility_improvable());
        EXPECT_EQ(false, y(0).is_feasibility_improvable());

        x = -10;
        y = 10;
        model.update();
        model.update_variable_objective_improvability();
        model.update_variable_feasibility_improvability();
        EXPECT_EQ(false, x(0).is_objective_improvable());
        EXPECT_EQ(false, y(0).is_objective_improvable());
        EXPECT_EQ(true, x(0).is_feasibility_improvable());
        EXPECT_EQ(true, y(0).is_feasibility_improvable());
    }
}

/*****************************************************************************/
TEST_F(TestModel, update_feasibility) {
    printemps::model::Model<int, double> model;

    auto&                  x = model.create_variable("x", 0, 10);
    [[maybe_unused]] auto& g = model.create_constraint("g", x <= 5);

    x = 4;
    model.update();  // include update_feasibility()
    EXPECT_EQ(true, model.is_feasible());

    x = 5;
    model.update();
    EXPECT_EQ(true, model.is_feasible());

    x = 6;
    model.update();
    EXPECT_EQ(false, model.is_feasible());
}

/*****************************************************************************/
TEST_F(TestModel, evaluate) {
    /// minimize
    {
        printemps::model::Model<int, double> model;

        auto sequence = printemps::utility::sequence(10);

        auto& x = model.create_variables("x", 10, 0, 1);
        auto& p = model.create_expression("p", x.dot(sequence) + 1);
        [[maybe_unused]] auto& g = model.create_constraint("g", x.sum() <= 5);
        [[maybe_unused]] auto& h =
            model.create_constraint("h", x(0) + x(1) <= 1);

        g(0).local_penalty_coefficient() = 100;
        h(0).local_penalty_coefficient() = 100;

        g(0).global_penalty_coefficient() = 10000;
        h(0).global_penalty_coefficient() = 10000;

        model.minimize(p);

        model.setup_variable_related_constraints();
        model.categorize_variables();
        model.categorize_constraints();
        model.extract_selections(printemps::model::SelectionMode::Defined);
        model.setup_fixed_sensitivities(false);

        for (auto&& element : x.flat_indexed_variables()) {
            element = 1;
        }

        model.update();
        auto score_before = model.evaluate({});

        {
            printemps::neighborhood::Move<int, double> move;
            for (auto&& element : x.flat_indexed_variables()) {
                move.alterations.emplace_back(&element, 1);
                for (auto&& constraint_ptr :
                     element.related_constraint_ptrs()) {
                    move.related_constraint_ptrs.insert(constraint_ptr);
                }
            }

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
            EXPECT_EQ(false, score_after_0.is_objective_improvable);
            EXPECT_EQ(false, score_after_0.is_feasibility_improvable);
            EXPECT_EQ(false, score_after_0.is_feasible);

            EXPECT_EQ(46, score_after_1.objective);
            EXPECT_EQ(5 + 1, score_after_1.total_violation);
            EXPECT_EQ(5 * 100 + 100, score_after_1.local_penalty);
            EXPECT_EQ(5 * 10000 + 10000, score_after_1.global_penalty);
            EXPECT_EQ(46 + 5 * 100 + 100,
                      score_after_1.local_augmented_objective);
            EXPECT_EQ(46 + 5 * 10000 + 10000,
                      score_after_1.global_augmented_objective);
            EXPECT_EQ(false, score_after_1.is_objective_improvable);
            EXPECT_EQ(false, score_after_1.is_feasibility_improvable);
            EXPECT_EQ(false, score_after_1.is_feasible);

            model.update(move);
            score_before = score_after_1;
        }

        {
            printemps::neighborhood::Move<int, double> move;
            for (auto&& element : x.flat_indexed_variables()) {
                move.alterations.emplace_back(&element, 0);
                for (auto&& constraint_ptr :
                     element.related_constraint_ptrs()) {
                    move.related_constraint_ptrs.insert(constraint_ptr);
                }
            }

            auto score_after_0 = model.evaluate(move);
            auto score_after_1 = model.evaluate(move, score_before);

            EXPECT_EQ(1, score_after_0.objective);
            EXPECT_EQ(0, score_after_0.total_violation);
            EXPECT_EQ(0, score_after_0.local_penalty);
            EXPECT_EQ(0, score_after_0.global_penalty);
            EXPECT_EQ(1, score_after_0.local_augmented_objective);
            EXPECT_EQ(1, score_after_0.global_augmented_objective);
            EXPECT_EQ(true, score_after_0.is_objective_improvable);
            EXPECT_EQ(true, score_after_0.is_feasibility_improvable);
            EXPECT_EQ(true, score_after_0.is_feasible);

            EXPECT_EQ(1, score_after_1.objective);
            EXPECT_EQ(0, score_after_1.total_violation);
            EXPECT_EQ(0, score_after_1.local_penalty);
            EXPECT_EQ(0, score_after_1.global_penalty);
            EXPECT_EQ(1, score_after_1.local_augmented_objective);
            EXPECT_EQ(1, score_after_1.global_augmented_objective);
            EXPECT_EQ(true, score_after_1.is_objective_improvable);
            EXPECT_EQ(true, score_after_1.is_feasibility_improvable);
            EXPECT_EQ(true, score_after_1.is_feasible);

            model.update(move);
            score_before = score_after_1;
        }

        {
            printemps::neighborhood::Move<int, double> move;
            for (auto i = 0; i < 5; i++) {
                move.alterations.emplace_back(&x(i), 1);
                for (auto&& constraint_ptr : x(i).related_constraint_ptrs()) {
                    move.related_constraint_ptrs.insert(constraint_ptr);
                }
            }

            auto score_after_0 = model.evaluate(move);
            auto score_after_1 = model.evaluate(move, score_before);

            EXPECT_EQ(11, score_after_0.objective);
            EXPECT_EQ(1, score_after_0.total_violation);
            EXPECT_EQ(100, score_after_0.local_penalty);
            EXPECT_EQ(10000, score_after_0.global_penalty);
            EXPECT_EQ(11 + 100, score_after_0.local_augmented_objective);
            EXPECT_EQ(11 + 10000, score_after_0.global_augmented_objective);
            EXPECT_EQ(false, score_after_0.is_objective_improvable);
            EXPECT_EQ(false, score_after_0.is_feasibility_improvable);
            EXPECT_EQ(false, score_after_0.is_feasible);

            EXPECT_EQ(11, score_after_1.objective);
            EXPECT_EQ(1, score_after_1.total_violation);
            EXPECT_EQ(100, score_after_1.local_penalty);
            EXPECT_EQ(10000, score_after_1.global_penalty);
            EXPECT_EQ(11 + 100, score_after_1.local_augmented_objective);
            EXPECT_EQ(11 + 10000, score_after_1.global_augmented_objective);
            EXPECT_EQ(false, score_after_1.is_objective_improvable);
            EXPECT_EQ(false, score_after_1.is_feasibility_improvable);
            EXPECT_EQ(false, score_after_1.is_feasible);

            model.update(move);
            score_before = score_after_1;
        }
    }

    /// maximize
    {
        printemps::model::Model<int, double> model;

        auto sequence = printemps::utility::sequence(10);

        auto& x = model.create_variables("x", 10, 0, 1);
        auto& p = model.create_expression("p", x.dot(sequence) + 1);
        [[maybe_unused]] auto& g = model.create_constraint("g", x.sum() <= 5);
        [[maybe_unused]] auto& h =
            model.create_constraint("h", x(0) + x(1) <= 1);

        g(0).local_penalty_coefficient() = 100;
        h(0).local_penalty_coefficient() = 100;

        g(0).global_penalty_coefficient() = 10000;
        h(0).global_penalty_coefficient() = 10000;

        model.maximize(p);

        model.setup_variable_related_constraints();
        model.categorize_variables();
        model.categorize_constraints();
        model.extract_selections(printemps::model::SelectionMode::Defined);
        model.setup_fixed_sensitivities(false);

        for (auto&& element : x.flat_indexed_variables()) {
            element = 1;
        }
        model.update();
        auto score_before = model.evaluate({});

        {
            printemps::neighborhood::Move<int, double> move;
            for (auto&& element : x.flat_indexed_variables()) {
                move.alterations.emplace_back(&element, 1);
                for (auto&& constraint_ptr :
                     element.related_constraint_ptrs()) {
                    move.related_constraint_ptrs.insert(constraint_ptr);
                }
            }

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
            EXPECT_EQ(false, score_after_0.is_objective_improvable);
            EXPECT_EQ(false, score_after_0.is_feasibility_improvable);
            EXPECT_EQ(false, score_after_0.is_feasible);

            EXPECT_EQ(-46, score_after_1.objective);
            EXPECT_EQ(5 + 1, score_after_1.total_violation);
            EXPECT_EQ(5 * 100 + 100, score_after_1.local_penalty);
            EXPECT_EQ(5 * 10000 + 10000, score_after_1.global_penalty);
            EXPECT_EQ(-46 + 5 * 100 + 100,
                      score_after_1.local_augmented_objective);
            EXPECT_EQ(-46 + 5 * 10000 + 10000,
                      score_after_1.global_augmented_objective);
            EXPECT_EQ(false, score_after_1.is_objective_improvable);
            EXPECT_EQ(false, score_after_1.is_feasibility_improvable);
            EXPECT_EQ(false, score_after_1.is_feasible);

            model.update(move);
            score_before = score_after_1;
        }
        {
            printemps::neighborhood::Move<int, double> move;
            for (auto&& element : x.flat_indexed_variables()) {
                move.alterations.emplace_back(&element, 0);
                for (auto&& constraint_ptr :
                     element.related_constraint_ptrs()) {
                    move.related_constraint_ptrs.insert(constraint_ptr);
                }
            }

            auto score_after_0 = model.evaluate(move);
            auto score_after_1 = model.evaluate(move, score_before);

            EXPECT_EQ(-1, score_after_0.objective);
            EXPECT_EQ(0, score_after_0.total_violation);
            EXPECT_EQ(0, score_after_0.local_penalty);
            EXPECT_EQ(0, score_after_0.global_penalty);
            EXPECT_EQ(-1, score_after_0.local_augmented_objective);
            EXPECT_EQ(-1, score_after_0.global_augmented_objective);
            EXPECT_EQ(false, score_after_0.is_objective_improvable);
            EXPECT_EQ(true, score_after_0.is_feasibility_improvable);
            EXPECT_EQ(true, score_after_0.is_feasible);

            EXPECT_EQ(-1, score_after_1.objective);
            EXPECT_EQ(0, score_after_0.total_violation);
            EXPECT_EQ(0, score_after_1.local_penalty);
            EXPECT_EQ(0, score_after_1.global_penalty);
            EXPECT_EQ(-1, score_after_1.local_augmented_objective);
            EXPECT_EQ(-1, score_after_1.global_augmented_objective);
            EXPECT_EQ(false, score_after_1.is_objective_improvable);
            EXPECT_EQ(true, score_after_1.is_feasibility_improvable);
            EXPECT_EQ(true, score_after_1.is_feasible);

            model.update(move);
            score_before = score_after_1;
        }

        {
            printemps::neighborhood::Move<int, double> move;
            for (auto i = 0; i < 5; i++) {
                move.alterations.emplace_back(&x(i), 1);
                for (auto&& constraint_ptr : x(i).related_constraint_ptrs()) {
                    move.related_constraint_ptrs.insert(constraint_ptr);
                }
            }

            auto score_after_0 = model.evaluate(move);
            auto score_after_1 = model.evaluate(move, score_before);

            EXPECT_EQ(-11, score_after_0.objective);
            EXPECT_EQ(1, score_after_0.total_violation);
            EXPECT_EQ(100, score_after_0.local_penalty);
            EXPECT_EQ(10000, score_after_0.global_penalty);
            EXPECT_EQ(-11 + 100, score_after_0.local_augmented_objective);
            EXPECT_EQ(-11 + 10000, score_after_0.global_augmented_objective);
            EXPECT_EQ(true, score_after_0.is_objective_improvable);
            EXPECT_EQ(false, score_after_0.is_feasibility_improvable);
            EXPECT_EQ(false, score_after_0.is_feasible);

            EXPECT_EQ(-11, score_after_1.objective);
            EXPECT_EQ(1, score_after_1.total_violation);
            EXPECT_EQ(100, score_after_1.local_penalty);
            EXPECT_EQ(10000, score_after_1.global_penalty);
            EXPECT_EQ(-11 + 100, score_after_1.local_augmented_objective);
            EXPECT_EQ(-11 + 10000, score_after_1.global_augmented_objective);
            EXPECT_EQ(true, score_after_1.is_objective_improvable);
            EXPECT_EQ(false, score_after_1.is_feasibility_improvable);
            EXPECT_EQ(false, score_after_1.is_feasible);

            model.update(move);
            score_before = score_after_1;
        }
    }
}

/*****************************************************************************/
TEST_F(TestModel, compute_lagrangian) {
    printemps::model::Model<int, double> model;

    auto sequence = printemps::utility::sequence(10);

    auto& x = model.create_variables("x", 10, 0, 1);
    auto& p = model.create_expression("p", x.dot(sequence) + 1);
    [[maybe_unused]] auto& g = model.create_constraint("g", x.sum() <= 5);
    [[maybe_unused]] auto& h = model.create_constraint("h", x(0) + x(1) <= 1);

    model.minimize(p);
    model.categorize_variables();
    model.categorize_constraints();

    printemps::model::ValueProxy<double> dual_value_proxy(1);
    dual_value_proxy.value() = 100;

    std::vector<printemps::model::ValueProxy<double>> dual_value_proxies = {
        dual_value_proxy, dual_value_proxy};

    for (auto&& element : x.flat_indexed_variables()) {
        element = 1;
    }

    model.update();
    auto lagrangian = model.compute_lagrangian(dual_value_proxies);

    EXPECT_EQ(46 + 100 * (10 - 5) + 100 * (2 - 1), lagrangian);
}

/*****************************************************************************/
TEST_F(TestModel, generate_variable_parameter_proxies) {
    printemps::model::Model<int, double> model;

    auto& x = model.create_variable("x");
    auto& y = model.create_variables("y", 10);
    auto& z = model.create_variables("z", {10, 10});

    int fill_value = random_integer();
    model.setup_unique_name();

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
    printemps::model::Model<int, double> model;

    auto& p = model.create_expression("p");
    auto& q = model.create_expressions("q", 10);
    auto& r = model.create_expressions("r", {10, 10});

    int fill_value = random_integer();
    model.setup_unique_name();

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
    printemps::model::Model<int, double> model;

    auto& g = model.create_constraint("g");
    auto& h = model.create_constraints("h", 10);
    auto& v = model.create_constraints("v", {10, 10});

    int fill_value = random_integer();
    model.setup_unique_name();

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
TEST_F(TestModel, export_solution) {
    printemps::model::Model<int, double> model;

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
    double total_violation = 0.0;
    for (auto i = 0; i < 20; i++) {
        for (auto j = 0; j < 30; j++) {
            total_violation += v(i, j).violation_value();
        }
    }

    auto solution = model.export_solution();
    EXPECT_EQ(3, static_cast<int>(solution.variable_value_proxies.size()));
    EXPECT_EQ(3, static_cast<int>(solution.expression_value_proxies.size()));
    EXPECT_EQ(3, static_cast<int>(solution.constraint_value_proxies.size()));
    EXPECT_EQ(3, static_cast<int>(solution.violation_value_proxies.size()));

    EXPECT_EQ(model.objective().value(), solution.objective);
    EXPECT_EQ(total_violation, solution.total_violation);
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
TEST_F(TestModel, export_named_solution) {
    printemps::model::Model<int, double> model;

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

    model.set_name("name");
    model.categorize_variables();
    model.categorize_constraints();
    model.update();
    double total_violation = 0.0;
    for (auto i = 0; i < 20; i++) {
        for (auto j = 0; j < 30; j++) {
            total_violation += v(i, j).violation_value();
        }
    }

    auto named_solution = model.export_named_solution();

    EXPECT_EQ(3, static_cast<int>(named_solution.variables().size()));
    EXPECT_EQ(3, static_cast<int>(named_solution.expressions().size()));
    EXPECT_EQ(3, static_cast<int>(named_solution.constraints().size()));
    EXPECT_EQ(3, static_cast<int>(named_solution.violations().size()));

    EXPECT_EQ(model.objective().value(), named_solution.objective());
    EXPECT_EQ(total_violation, named_solution.total_violation());
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
TEST_F(TestModel, convert_to_named_solution) {
    /// This method is tested in export_named_solution.
}

/*****************************************************************************/
TEST_F(TestModel, export_plain_solution) {
    printemps::model::Model<int, double> model;

    auto& x = model.create_variable("x");
    auto& y = model.create_variables("y", 10);
    auto& z = model.create_variables("z", {20, 30});

    model.minimize(random_integer() * x.sum() + random_integer() * y.sum() +
                   random_integer() * z.sum());

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

    auto plain_solution = model.export_plain_solution();
    EXPECT_EQ(model.objective().value(), plain_solution.objective);
    EXPECT_EQ(model.is_feasible(), plain_solution.is_feasible);

    int index = 0;
    EXPECT_EQ(x.value(), plain_solution.variables[index++]);

    for (auto i = 0; i < 10; i++) {
        EXPECT_EQ(y(i).value(), plain_solution.variables[index++]);
    }

    for (auto i = 0; i < 20; i++) {
        for (auto j = 0; j < 30; j++) {
            EXPECT_EQ(z(i, j).value(), plain_solution.variables[index++]);
        }
    }
}

/*****************************************************************************/
TEST_F(TestModel, convert_to_plain_solution) {
    printemps::model::Model<int, double> model;

    auto& x = model.create_variable("x");
    auto& y = model.create_variables("y", 10);
    auto& z = model.create_variables("z", {20, 30});

    auto& p = model.create_expression("p");
    auto& q = model.create_expressions("q", 10);
    auto& r = model.create_expressions("r", {20, 30});

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
    double total_violation = 0.0;
    for (auto i = 0; i < 20; i++) {
        for (auto j = 0; j < 30; j++) {
            total_violation += v(i, j).violation_value();
        }
    }

    auto solution       = model.export_solution();
    auto plain_solution = model.convert_to_plain_solution(solution);
    EXPECT_EQ(model.objective().value(), plain_solution.objective);
    EXPECT_EQ(total_violation, plain_solution.total_violation);
    EXPECT_EQ(model.is_feasible(), plain_solution.is_feasible);

    int index = 0;
    EXPECT_EQ(x.value(), plain_solution.variables[index++]);

    for (auto i = 0; i < 10; i++) {
        EXPECT_EQ(y(i).value(), plain_solution.variables[index++]);
    }

    for (auto i = 0; i < 20; i++) {
        for (auto j = 0; j < 30; j++) {
            EXPECT_EQ(z(i, j).value(), plain_solution.variables[index++]);
        }
    }
}

/*****************************************************************************/
TEST_F(TestModel, import_solution) {
    printemps::model::Model<int, double> model;

    auto& x = model.create_variable("x");
    auto& y = model.create_variables("y", 10);
    auto& z = model.create_variables("z", {20, 30});

    model.setup_unique_name();

    std::unordered_map<std::string, int> solution;
    solution["x"]         = 1;
    solution["y[ 0]"]     = 2;
    solution["y[ 9]"]     = 3;
    solution["z[ 0,  0]"] = 4;
    solution["z[19, 19]"] = 5;
    model.import_solution(solution);

    EXPECT_EQ(x.value(), 1);
    EXPECT_EQ(y(0).value(), 2);
    EXPECT_EQ(y(9).value(), 3);
    EXPECT_EQ(z(0, 0).value(), 4);
    EXPECT_EQ(z(19, 19).value(), 5);
}

/*****************************************************************************/
TEST_F(TestModel, fix_variables) {
    printemps::model::Model<int, double> model;

    auto& x = model.create_variable("x");
    auto& y = model.create_variables("y", 10);
    auto& z = model.create_variables("z", {20, 30});

    model.setup_unique_name();

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

    EXPECT_EQ(true, x.is_fixed());
    EXPECT_EQ(true, y(0).is_fixed());
    EXPECT_EQ(true, y(9).is_fixed());
    EXPECT_EQ(true, z(0, 0).is_fixed());
    EXPECT_EQ(true, z(19, 19).is_fixed());
}

/*****************************************************************************/
TEST_F(TestModel, unfix_variables) {
    printemps::model::Model<int, double> model;

    auto& x = model.create_variable("x");
    auto& y = model.create_variables("y", 10);
    auto& z = model.create_variables("z", {20, 30});

    model.setup_unique_name();

    std::unordered_set<std::string> mutable_variable_names;
    mutable_variable_names.insert("x");
    mutable_variable_names.insert("y[ 0]");
    mutable_variable_names.insert("y[ 9]");
    mutable_variable_names.insert("z[ 0,  0]");
    mutable_variable_names.insert("z[19, 19]");
    model.unfix_variables(mutable_variable_names);

    EXPECT_EQ(false, x.is_fixed());
    EXPECT_EQ(false, y(0).is_fixed());
    EXPECT_EQ(true, y(1).is_fixed());
    EXPECT_EQ(true, y(8).is_fixed());
    EXPECT_EQ(false, y(9).is_fixed());
    EXPECT_EQ(false, z(0, 0).is_fixed());
    EXPECT_EQ(true, z(0, 1).is_fixed());
    EXPECT_EQ(true, z(19, 18).is_fixed());
    EXPECT_EQ(false, z(19, 19).is_fixed());
}

/*****************************************************************************/
TEST_F(TestModel, export_summary) {
    printemps::model::Model<int, double> model("name");

    [[maybe_unused]] auto& x = model.create_variable("x");
    [[maybe_unused]] auto& y = model.create_variables("y", 10);
    [[maybe_unused]] auto& z = model.create_variables("z", {20, 30});

    [[maybe_unused]] auto& g = model.create_constraint("g");
    [[maybe_unused]] auto& h = model.create_constraints("h", 10);
    [[maybe_unused]] auto& v = model.create_constraints("v", {20, 30});

    model.categorize_variables();
    model.categorize_constraints();

    auto summary = model.export_summary();
    EXPECT_EQ("name", summary.name);
    EXPECT_EQ(1 + 10 + 20 * 30, summary.number_of_variables);
    EXPECT_EQ(1 + 10 + 20 * 30, summary.number_of_constraints);
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
TEST_F(TestModel, selections) {
    /// This method is tested in extract_selections_larger() and so on.
}

/*****************************************************************************/
TEST_F(TestModel, variable_reference) {
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
    /// This method is tested in minimize_arg_function() and so on.
}

/*****************************************************************************/
TEST_F(TestModel, is_enabled_fast_evaluation) {
    /// This method is tested in setup_is_enabled_fast_evaluation().
}

/*****************************************************************************/
TEST_F(TestModel, is_linear) {
    /// This method is tested in setup_is_linear().
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
TEST_F(TestModel, set_is_solved) {
    printemps::model::Model<int, double> model;
    EXPECT_EQ(false, model.is_solved());
    model.set_is_solved(true);
    EXPECT_EQ(true, model.is_solved());
    model.set_is_solved(false);
    EXPECT_EQ(false, model.is_solved());
}

/*****************************************************************************/
TEST_F(TestModel, is_solved) {
    /// This method is tested in set_is_solved().
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
TEST_F(TestModel, number_of_not_fixed_variables) {
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
TEST_F(TestModel, number_of_disabled_constraints) {
    /// This method is tested in categorize_constraints().
}

/*****************************************************************************/
TEST_F(TestModel, neighborhood) {
    /// This method is tested in test_neighborhood.h
}

/*****************************************************************************/
}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/