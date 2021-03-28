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
class TestConstraint : public ::testing::Test {
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
TEST_F(TestConstraint, initialize) {
    auto constraint =
        printemps::model::Constraint<int, double>::create_instance();

    /// Check the initial values of the base class members.
    EXPECT_EQ(0, constraint.proxy_index());
    EXPECT_EQ(0, constraint.flat_index());
    EXPECT_EQ(0, constraint.multi_dimensional_index()[0]);
    EXPECT_EQ("", constraint.name());

    /// Check the initial values of the derived class members.
    EXPECT_EQ(0, constraint.evaluate_constraint());
    EXPECT_EQ(0, constraint.evaluate_constraint({}));
    EXPECT_EQ(0, constraint.evaluate_violation());
    EXPECT_EQ(0, constraint.evaluate_violation({}));
    EXPECT_TRUE(constraint.expression().sensitivities().empty());
    EXPECT_EQ(0, constraint.expression().constant_value());
    EXPECT_EQ(printemps::model::ConstraintSense::Lower, constraint.sense());
    EXPECT_EQ(0, constraint.constraint_value());
    EXPECT_EQ(0, constraint.violation_value());
    EXPECT_TRUE(constraint.is_linear());
    EXPECT_TRUE(constraint.is_enabled());
    EXPECT_EQ(HUGE_VALF, constraint.local_penalty_coefficient());
    EXPECT_EQ(HUGE_VALF, constraint.global_penalty_coefficient());

    EXPECT_FALSE(constraint.is_singleton());
    EXPECT_FALSE(constraint.is_aggregation());
    EXPECT_FALSE(constraint.is_precedence());
    EXPECT_FALSE(constraint.is_variable_bound());
    EXPECT_FALSE(constraint.is_set_partitioning());
    EXPECT_FALSE(constraint.is_set_packing());
    EXPECT_FALSE(constraint.is_set_covering());
    EXPECT_FALSE(constraint.is_cardinality());
    EXPECT_FALSE(constraint.is_invariant_knapsack());
    EXPECT_FALSE(constraint.is_equation_knapsack());
    EXPECT_FALSE(constraint.is_bin_packing());
    EXPECT_FALSE(constraint.is_knapsack());
    EXPECT_FALSE(constraint.is_integer_knapsack());
    EXPECT_FALSE(constraint.is_general_linear());
}

/*****************************************************************************/
TEST_F(TestConstraint, constructor_arg_function) {
    auto expression =
        printemps::model::Expression<int, double>::create_instance();
    auto variable = printemps::model::Variable<int, double>::create_instance();

    auto sensitivity = random_integer();
    auto constant    = random_integer();
    auto target      = random_integer();

    expression = sensitivity * variable + constant;
    std::function<double(const printemps::neighborhood::Move<int, double>&)> f =
        [&expression](const auto& a_MOVE) {
            return expression.evaluate(a_MOVE);
        };

    /// Lower
    {
        printemps::model::Constraint<int, double> constraint(f <= target);

        EXPECT_TRUE(constraint.expression().sensitivities().empty());
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Lower, constraint.sense());
        EXPECT_EQ(0, constraint.constraint_value());
        EXPECT_EQ(0, constraint.violation_value());
        EXPECT_FALSE(constraint.is_linear());
        EXPECT_TRUE(constraint.is_enabled());
    }

    /// Equal
    {
        printemps::model::Constraint<int, double> constraint(f == target);

        EXPECT_TRUE(constraint.expression().sensitivities().empty());
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Equal, constraint.sense());
        EXPECT_EQ(0, constraint.constraint_value());
        EXPECT_EQ(0, constraint.violation_value());
        EXPECT_FALSE(constraint.is_linear());
        EXPECT_TRUE(constraint.is_enabled());
    }

    /// Upper
    {
        printemps::model::Constraint<int, double> constraint(f >= target);

        EXPECT_TRUE(constraint.expression().sensitivities().empty());
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Upper, constraint.sense());
        EXPECT_EQ(0, constraint.constraint_value());
        EXPECT_EQ(0, constraint.violation_value());
        EXPECT_FALSE(constraint.is_linear());
        EXPECT_TRUE(constraint.is_enabled());
    }
}

/*****************************************************************************/
TEST_F(TestConstraint, constructor_arg_expression) {
    auto expression =
        printemps::model::Expression<int, double>::create_instance();
    auto variable = printemps::model::Variable<int, double>::create_instance();

    auto sensitivity = random_integer();
    auto constant    = random_integer();
    auto target      = random_integer();

    expression = sensitivity * variable + constant;

    /// Lower
    {
        printemps::model::Constraint<int, double> constraint(expression <=
                                                             target);

        EXPECT_EQ(sensitivity,
                  constraint.expression().sensitivities().at(&variable));

        EXPECT_EQ(constant - target, constraint.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Lower, constraint.sense());
        EXPECT_EQ(0, constraint.constraint_value());
        EXPECT_EQ(0, constraint.violation_value());
        EXPECT_TRUE(constraint.is_linear());
        EXPECT_TRUE(constraint.is_enabled());
    }

    /// Equal
    {
        printemps::model::Constraint<int, double> constraint(expression ==
                                                             target);

        EXPECT_EQ(sensitivity,
                  constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(constant - target, constraint.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Equal, constraint.sense());
        EXPECT_EQ(0, constraint.constraint_value());
        EXPECT_EQ(0, constraint.violation_value());
        EXPECT_TRUE(constraint.is_linear());
        EXPECT_TRUE(constraint.is_enabled());
    }

    /// Upper
    {
        printemps::model::Constraint<int, double> constraint(expression >=
                                                             target);

        EXPECT_EQ(sensitivity,
                  constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(constant - target, constraint.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Upper, constraint.sense());
        EXPECT_EQ(0, constraint.constraint_value());
        EXPECT_EQ(0, constraint.violation_value());
        EXPECT_TRUE(constraint.is_linear());
        EXPECT_TRUE(constraint.is_enabled());
    }
}

/*****************************************************************************/
TEST_F(TestConstraint, setup_arg_function) {
    auto expression =
        printemps::model::Expression<int, double>::create_instance();
    auto variable = printemps::model::Variable<int, double>::create_instance();

    auto sensitivity = random_integer();
    auto constant    = random_integer();
    auto target      = random_integer();

    expression = sensitivity * variable + constant;

    std::function<double(const printemps::neighborhood::Move<int, double>&)> f =
        [&expression,
         target](const printemps::neighborhood::Move<int, double>& a_MOVE) {
            return expression.evaluate(a_MOVE) - target;
        };

    /// Lower
    {
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();

        constraint.setup(f, printemps::model::ConstraintSense::Lower);

        EXPECT_TRUE(constraint.expression().sensitivities().empty());
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Lower, constraint.sense());
        EXPECT_EQ(0, constraint.constraint_value());
        EXPECT_EQ(0, constraint.violation_value());
        EXPECT_FALSE(constraint.is_linear());
        EXPECT_TRUE(constraint.is_enabled());
    }

    /// Equal
    {
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();
        constraint.setup(f, printemps::model::ConstraintSense::Equal);

        EXPECT_TRUE(constraint.expression().sensitivities().empty());
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Equal, constraint.sense());
        EXPECT_EQ(0, constraint.constraint_value());
        EXPECT_EQ(0, constraint.violation_value());
        EXPECT_FALSE(constraint.is_linear());
        EXPECT_TRUE(constraint.is_enabled());
    }

    /// Upper
    {
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();
        ;
        constraint.setup(f, printemps::model::ConstraintSense::Upper);

        EXPECT_TRUE(constraint.expression().sensitivities().empty());
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Upper, constraint.sense());
        EXPECT_EQ(0, constraint.constraint_value());
        EXPECT_EQ(0, constraint.violation_value());
        EXPECT_FALSE(constraint.is_linear());
        EXPECT_TRUE(constraint.is_enabled());
    }
}

/*****************************************************************************/
TEST_F(TestConstraint, setup_arg_expression) {
    auto expression =
        printemps::model::Expression<int, double>::create_instance();
    auto variable = printemps::model::Variable<int, double>::create_instance();

    auto sensitivity = random_integer();
    auto constant    = random_integer();
    auto target      = random_integer();

    expression = sensitivity * variable + constant;

    /// Lower
    {
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();
        constraint.setup(expression - target,
                         printemps::model::ConstraintSense::Lower);

        EXPECT_EQ(sensitivity,
                  constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(constant - target, constraint.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Lower, constraint.sense());
        EXPECT_EQ(0, constraint.constraint_value());
        EXPECT_EQ(0, constraint.violation_value());
        EXPECT_TRUE(constraint.is_linear());
        EXPECT_TRUE(constraint.is_enabled());
    }

    /// Equal
    {
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();
        ;
        constraint.setup(expression - target,
                         printemps::model::ConstraintSense::Equal);

        EXPECT_EQ(sensitivity,
                  constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(constant - target, constraint.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Equal, constraint.sense());
        EXPECT_EQ(0, constraint.constraint_value());
        EXPECT_EQ(0, constraint.violation_value());
        EXPECT_TRUE(constraint.is_linear());
        EXPECT_TRUE(constraint.is_enabled());
    }

    /// Upper
    {
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();
        constraint.setup(expression - target,
                         printemps::model::ConstraintSense::Upper);

        EXPECT_EQ(sensitivity,
                  constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(constant - target, constraint.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Upper, constraint.sense());
        EXPECT_EQ(0, constraint.constraint_value());
        EXPECT_EQ(0, constraint.violation_value());
        EXPECT_TRUE(constraint.is_linear());
        EXPECT_TRUE(constraint.is_enabled());
    }
}

/*****************************************************************************/
TEST_F(TestConstraint, setup_constraint_type) {
    printemps::model::Model<int, double> model;
    auto coefficients = printemps::utility::sequence(10);

    auto& x = model.create_variable("x", -10, 10);
    auto& y = model.create_variable("y", -10, 10);
    auto& z = model.create_variables("z", 10, 0, 1);
    auto& w = model.create_variable("w", 0, 1);
    auto& r = model.create_variables("r", 10, -10, 10);

    /// Singleton
    {
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();
        constraint.setup(2 * x - 10, printemps::model::ConstraintSense::Lower);
        constraint.setup_constraint_type();
        EXPECT_TRUE(constraint.is_singleton());
    }

    /// Aggregation
    {
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();
        constraint.setup(2 * x + 3 * y - 10,
                         printemps::model::ConstraintSense::Equal);
        constraint.setup_constraint_type();
        EXPECT_TRUE(constraint.is_aggregation());
    }

    /// Precedence
    {
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();
        constraint.setup(2 * x - 2 * y - 5,
                         printemps::model::ConstraintSense::Lower);
        constraint.setup_constraint_type();
        EXPECT_TRUE(constraint.is_precedence());
    }
    {
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();
        constraint.setup(-2 * x + 2 * y - 5,
                         printemps::model::ConstraintSense::Lower);
        constraint.setup_constraint_type();
        EXPECT_TRUE(constraint.is_precedence());
    }
    {
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();
        constraint.setup(2 * x - 2 * y - 5,
                         printemps::model::ConstraintSense::Upper);
        constraint.setup_constraint_type();
        EXPECT_TRUE(constraint.is_precedence());
    }
    {
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();
        constraint.setup(-2 * x + 2 * y - 5,
                         printemps::model::ConstraintSense::Upper);
        constraint.setup_constraint_type();
        EXPECT_TRUE(constraint.is_precedence());
    }

    /// Variable Bound
    {
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();
        constraint.setup(2 * z(0) + 3 * z(1) - 5,
                         printemps::model::ConstraintSense::Lower);
        constraint.setup_constraint_type();
        EXPECT_TRUE(constraint.is_variable_bound());
    }
    {
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();
        constraint.setup(2 * z(0) + 3 * z(1) - 5,
                         printemps::model::ConstraintSense::Upper);
        constraint.setup_constraint_type();
        EXPECT_TRUE(constraint.is_variable_bound());
    }

    /// Set Partitioning
    {
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();
        constraint.setup(z.sum() - 1, printemps::model::ConstraintSense::Equal);
        constraint.setup_constraint_type();
        EXPECT_TRUE(constraint.is_set_partitioning());
    }

    /// Set Packing
    {
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();
        constraint.setup(z.sum() - 1, printemps::model::ConstraintSense::Lower);
        constraint.setup_constraint_type();
        EXPECT_TRUE(constraint.is_set_packing());
    }

    /// Set Covering
    {
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();
        constraint.setup(z.sum() - 1, printemps::model::ConstraintSense::Upper);
        constraint.setup_constraint_type();
        EXPECT_TRUE(constraint.is_set_covering());
    }

    /// Cardinality
    {
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();
        constraint.setup(z.sum() - 5, printemps::model::ConstraintSense::Equal);
        constraint.setup_constraint_type();
        EXPECT_TRUE(constraint.is_cardinality());
    }

    /// Invariant Knapsack
    {
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();
        constraint.setup(z.sum() - 5, printemps::model::ConstraintSense::Lower);
        constraint.setup_constraint_type();
        EXPECT_TRUE(constraint.is_invariant_knapsack());
    }

    /// Equation Knapsack
    {
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();
        constraint.setup(z.dot(coefficients) - 30,
                         printemps::model::ConstraintSense::Equal);
        constraint.setup_constraint_type();
        EXPECT_TRUE(constraint.is_equation_knapsack());
    }

    /// Bin Packing
    {
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();
        constraint.setup(z.dot(coefficients) + 5 * w - 5,
                         printemps::model::ConstraintSense::Lower);
        constraint.setup_constraint_type();
        EXPECT_TRUE(constraint.is_bin_packing());
    }

    {
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();
        constraint.setup(z.dot(coefficients) - 5 * w + 5,
                         printemps::model::ConstraintSense::Upper);
        constraint.setup_constraint_type();
        EXPECT_TRUE(constraint.is_bin_packing());
    }

    /// Knapsack
    {
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();
        constraint.setup(z.dot(coefficients) - 50,
                         printemps::model::ConstraintSense::Lower);
        constraint.setup_constraint_type();
        EXPECT_TRUE(constraint.is_knapsack());
    }

    {
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();
        constraint.setup(z.dot(coefficients) + 50,
                         printemps::model::ConstraintSense::Upper);
        constraint.setup_constraint_type();
        EXPECT_TRUE(constraint.is_knapsack());
    }

    /// Integer Knapsack
    {
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();
        constraint.setup(r.dot(coefficients) - 50,
                         printemps::model::ConstraintSense::Lower);
        constraint.setup_constraint_type();
        EXPECT_TRUE(constraint.is_integer_knapsack());
    }

    {
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();
        constraint.setup(r.dot(coefficients) + 50,
                         printemps::model::ConstraintSense::Upper);
        constraint.setup_constraint_type();
        EXPECT_TRUE(constraint.is_integer_knapsack());
    }

    /// General Linear
    {
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();
        constraint.setup(x + r.sum() - 50,
                         printemps::model::ConstraintSense::Equal);
        constraint.setup_constraint_type();
        EXPECT_TRUE(constraint.is_general_linear());
    }
}

/*****************************************************************************/
TEST_F(TestConstraint, evaluate_function_arg_void) {
    auto expression =
        printemps::model::Expression<int, double>::create_instance();
    auto variable = printemps::model::Variable<int, double>::create_instance();

    auto sensitivity = random_integer();
    auto constant    = random_integer();
    auto target      = random_integer();

    expression = sensitivity * variable + constant;

    std::function<double(const printemps::neighborhood::Move<int, double>&)> f =
        [&expression,
         target](const printemps::neighborhood::Move<int, double>& a_MOVE) {
            return expression.evaluate(a_MOVE) - target;
        };

    auto constraint =
        printemps::model::Constraint<int, double>::create_instance();

    constraint.setup(f, printemps::model::ConstraintSense::Lower);

    auto value = random_integer();
    variable   = value;

    auto expected_value = sensitivity * value + constant - target;
    EXPECT_EQ(expected_value, constraint.evaluate_constraint());
    constraint.update();
    EXPECT_EQ(expected_value, constraint.constraint_value());
}

/*****************************************************************************/
TEST_F(TestConstraint, evaluate_expression_arg_void) {
    auto expression =
        printemps::model::Expression<int, double>::create_instance();
    auto variable = printemps::model::Variable<int, double>::create_instance();

    auto sensitivity = random_integer();
    auto constant    = random_integer();
    auto target      = random_integer();

    expression = sensitivity * variable + constant;

    auto constraint =
        printemps::model::Constraint<int, double>::create_instance();
    constraint.setup(expression - target,
                     printemps::model::ConstraintSense::Lower);

    auto value = random_integer();
    variable   = value;

    auto expected_value = sensitivity * value + constant - target;
    EXPECT_EQ(expected_value, constraint.evaluate_constraint());
    constraint.update();
    EXPECT_EQ(expected_value, constraint.constraint_value());
}

/*****************************************************************************/
TEST_F(TestConstraint, evaluate_function_arg_move) {
    auto expression =
        printemps::model::Expression<int, double>::create_instance();
    auto variable = printemps::model::Variable<int, double>::create_instance();

    auto sensitivity = random_integer();
    auto constant    = random_integer();
    auto target      = random_integer();

    expression = sensitivity * variable + constant;
    expression.setup_fixed_sensitivities();

    std::function<double(const printemps::neighborhood::Move<int, double>&)> f =
        [&expression,
         target](const printemps::neighborhood::Move<int, double>& a_MOVE) {
            return expression.evaluate(a_MOVE) - target;
        };

    auto constraint =
        printemps::model::Constraint<int, double>::create_instance();
    constraint.setup(f, printemps::model::ConstraintSense::Lower);

    /// initial
    {
        auto value = random_integer();
        variable   = value;

        auto expected_value = sensitivity * value + constant - target;
        EXPECT_EQ(expected_value, constraint.evaluate_constraint());
        constraint.update();
        EXPECT_EQ(expected_value, constraint.constraint_value());

        /// expression.update() must be called after constraint.update();
        expression.update();
    }
    /// after move
    {
        auto value = random_integer();

        printemps::neighborhood::Move<int, double> move;
        move.alterations.emplace_back(&variable, value);

        auto expected_value = sensitivity * value + constant - target;
        EXPECT_EQ(expected_value, constraint.evaluate_constraint(move));
        constraint.update(move);
        EXPECT_EQ(expected_value, constraint.constraint_value());
    }
}

/*****************************************************************************/
TEST_F(TestConstraint, evaluate_expression_arg_move) {
    auto expression =
        printemps::model::Expression<int, double>::create_instance();
    auto variable = printemps::model::Variable<int, double>::create_instance();

    auto sensitivity = random_integer();
    auto constant    = random_integer();
    auto target      = random_integer();

    expression = sensitivity * variable + constant;

    auto constraint =
        printemps::model::Constraint<int, double>::create_instance();
    constraint.setup(expression - target,
                     printemps::model::ConstraintSense::Lower);

    /// initial
    {
        auto value = random_integer();
        variable   = value;
        constraint.update();

        auto expected_value = sensitivity * value + constant - target;
        EXPECT_EQ(expected_value, constraint.evaluate_constraint());
        constraint.update();
        EXPECT_EQ(expected_value, constraint.constraint_value());
    }

    /// after move
    {
        auto value = random_integer();

        printemps::neighborhood::Move<int, double> move;
        move.alterations.emplace_back(&variable, value);

        auto expected_value = sensitivity * value + constant - target;
        EXPECT_EQ(expected_value, constraint.evaluate_constraint(move));
        constraint.update(move);
        EXPECT_EQ(expected_value, constraint.constraint_value());
    }
}

/*****************************************************************************/
TEST_F(TestConstraint, evaluate_violation_function_arg_void) {
    auto expression =
        printemps::model::Expression<int, double>::create_instance();
    auto variable = printemps::model::Variable<int, double>::create_instance();

    auto sensitivity = random_integer();
    auto constant    = random_integer();
    auto target      = random_integer();

    expression = sensitivity * variable + constant;

    std::function<double(const printemps::neighborhood::Move<int, double>&)> f =
        [&expression,
         target](const printemps::neighborhood::Move<int, double>& a_MOVE) {
            return expression.evaluate(a_MOVE) - target;
        };

    /// Lower
    {
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();
        constraint.setup(f, printemps::model::ConstraintSense::Lower);

        auto value = random_integer();
        variable   = value;

        auto expected_value =
            std::max(sensitivity * value + constant - target, 0);
        EXPECT_EQ(expected_value, constraint.evaluate_violation());
        constraint.update();
        EXPECT_EQ(expected_value, constraint.violation_value());
    }

    /// Equal
    {
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();
        constraint.setup(f, printemps::model::ConstraintSense::Equal);

        auto value = random_integer();
        variable   = value;

        auto expected_value = std::abs(sensitivity * value + constant - target);
        EXPECT_EQ(expected_value, constraint.evaluate_violation());
        constraint.update();
        EXPECT_EQ(expected_value, constraint.violation_value());
    }

    /// Upper
    {
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();
        constraint.setup(f, printemps::model::ConstraintSense::Upper);

        auto value = random_integer();
        variable   = value;

        auto expected_value =
            std::max(-(sensitivity * value + constant - target), 0);
        EXPECT_EQ(expected_value, constraint.evaluate_violation());
        constraint.update();
        EXPECT_EQ(expected_value, constraint.violation_value());
    }
}

/*****************************************************************************/
TEST_F(TestConstraint, evaluate_violation_expression_arg_void) {
    auto expression =
        printemps::model::Expression<int, double>::create_instance();
    auto variable = printemps::model::Variable<int, double>::create_instance();

    auto sensitivity = random_integer();
    auto constant    = random_integer();
    auto target      = random_integer();

    expression = sensitivity * variable + constant;

    /// Lower
    {
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();
        constraint.setup(expression - target,
                         printemps::model::ConstraintSense::Lower);

        auto value = random_integer();
        variable   = value;

        auto expected_value =
            std::max(sensitivity * value + constant - target, 0);
        EXPECT_EQ(expected_value, constraint.evaluate_violation());
        constraint.update();
        EXPECT_EQ(expected_value, constraint.violation_value());
    }

    /// Equal
    {
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();
        constraint.setup(expression - target,
                         printemps::model::ConstraintSense::Equal);

        auto value = random_integer();
        variable   = value;

        auto expected_value = std::abs(sensitivity * value + constant - target);
        EXPECT_EQ(expected_value, constraint.evaluate_violation());
        constraint.update();
        EXPECT_EQ(expected_value, constraint.violation_value());
    }

    /// Upper
    {
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();
        constraint.setup(expression - target,
                         printemps::model::ConstraintSense::Upper);

        auto value = random_integer();
        variable   = value;

        auto expected_value =
            std::max(-(sensitivity * value + constant - target), 0);
        EXPECT_EQ(expected_value, constraint.evaluate_violation());
        constraint.update();
        EXPECT_EQ(expected_value, constraint.violation_value());
    }
}

/*****************************************************************************/
TEST_F(TestConstraint, evaluate_violation_function_arg_move) {
    auto expression =
        printemps::model::Expression<int, double>::create_instance();
    auto variable = printemps::model::Variable<int, double>::create_instance();

    auto sensitivity = random_integer();
    auto constant    = random_integer();
    auto target      = random_integer();

    expression = sensitivity * variable + constant;
    expression.setup_fixed_sensitivities();

    std::function<double(const printemps::neighborhood::Move<int, double>&)> f =
        [&expression,
         target](const printemps::neighborhood::Move<int, double>& a_MOVE) {
            return expression.evaluate(a_MOVE) - target;
        };

    /// Lower
    {
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();
        constraint.setup(f, printemps::model::ConstraintSense::Lower);

        /// initial
        {
            auto value = random_integer();
            variable   = value;

            auto expected_value =
                std::max(sensitivity * value + constant - target, 0);
            EXPECT_EQ(expected_value, constraint.evaluate_violation());
            constraint.update();
            EXPECT_EQ(expected_value, constraint.violation_value());

            /// expression.update() must be called after constraint.update();
            expression.update();
        }

        /// after move
        {
            auto value = random_integer();

            printemps::neighborhood::Move<int, double> move;
            move.alterations.emplace_back(&variable, value);

            auto expected_value =
                std::max(sensitivity * value + constant - target, 0);
            EXPECT_EQ(expected_value, constraint.evaluate_violation(move));
            constraint.update(move);
            EXPECT_EQ(expected_value, constraint.violation_value());
        }
    }

    /// Equal
    {
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();
        constraint.setup(f, printemps::model::ConstraintSense::Equal);

        /// initial
        {
            auto value = random_integer();
            variable   = value;

            auto expected_value =
                std::abs(sensitivity * value + constant - target);
            EXPECT_EQ(expected_value, constraint.evaluate_violation());
            constraint.update();
            EXPECT_EQ(expected_value, constraint.violation_value());

            /// expression.update() must be called after constraint.update();
            expression.update();
        }

        /// after move
        {
            auto value = random_integer();

            printemps::neighborhood::Move<int, double> move;
            move.alterations.emplace_back(&variable, value);

            auto expected_value =
                std::abs(sensitivity * value + constant - target);
            EXPECT_EQ(expected_value, constraint.evaluate_violation(move));
            constraint.update(move);
            EXPECT_EQ(expected_value, constraint.violation_value());
        }
    }

    /// Upper
    {
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();
        constraint.setup(f, printemps::model::ConstraintSense::Upper);

        /// initial
        {
            auto value = random_integer();
            variable   = value;

            auto expected_value =
                std::max(-(sensitivity * value + constant - target), 0);
            EXPECT_EQ(expected_value, constraint.evaluate_violation());
            constraint.update();
            EXPECT_EQ(expected_value, constraint.violation_value());

            /// expression.update() must be called after constraint.update();
            expression.update();
        }

        /// after move
        {
            auto value = random_integer();

            printemps::neighborhood::Move<int, double> move;
            move.alterations.emplace_back(&variable, value);

            auto expected_value =
                std::max(-(sensitivity * value + constant - target), 0);
            EXPECT_EQ(expected_value, constraint.evaluate_violation(move));
            constraint.update(move);
            EXPECT_EQ(expected_value, constraint.violation_value());
        }
    }
}

/*****************************************************************************/
TEST_F(TestConstraint, evaluate_violation_expression_arg_move) {
    auto expression =
        printemps::model::Expression<int, double>::create_instance();
    auto variable = printemps::model::Variable<int, double>::create_instance();

    auto sensitivity = random_integer();
    auto constant    = random_integer();
    auto target      = random_integer();

    expression = sensitivity * variable + constant;

    /// Lower
    {
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();
        constraint.setup(expression - target,
                         printemps::model::ConstraintSense::Lower);

        /// initial
        {
            auto value = random_integer();
            variable   = value;
            constraint.update();

            auto expected_value =
                std::max(sensitivity * value + constant - target, 0);
            EXPECT_EQ(expected_value, constraint.evaluate_violation());
            constraint.update();
            EXPECT_EQ(expected_value, constraint.violation_value());
        }

        /// after move
        {
            auto value = random_integer();

            printemps::neighborhood::Move<int, double> move;
            move.alterations.emplace_back(&variable, value);

            auto expected_value =
                std::max(sensitivity * value + constant - target, 0);
            EXPECT_EQ(expected_value, constraint.evaluate_violation(move));
            constraint.update(move);
            EXPECT_EQ(expected_value, constraint.violation_value());
        }
    }

    /// Equal
    {
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();
        constraint.setup(expression - target,
                         printemps::model::ConstraintSense::Equal);

        /// initial
        {
            auto value = random_integer();
            variable   = value;
            constraint.update();

            auto expected_value =
                std::abs(sensitivity * value + constant - target);
            EXPECT_EQ(expected_value, constraint.evaluate_violation());
            constraint.update();
            EXPECT_EQ(expected_value, constraint.violation_value());
        }

        /// after move
        {
            auto value = random_integer();

            printemps::neighborhood::Move<int, double> move;
            move.alterations.emplace_back(&variable, value);

            auto expected_value =
                std::abs(sensitivity * value + constant - target);
            EXPECT_EQ(expected_value, constraint.evaluate_violation(move));
            constraint.update(move);
            EXPECT_EQ(expected_value, constraint.violation_value());
        }
    }

    /// Upper
    {
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();
        constraint.setup(expression - target,
                         printemps::model::ConstraintSense::Upper);

        /// initial
        {
            auto value = random_integer();
            variable   = value;
            constraint.update();

            auto expected_value =
                std::max(-(sensitivity * value + constant - target), 0);
            EXPECT_EQ(expected_value, constraint.evaluate_violation());
            constraint.update();
            EXPECT_EQ(expected_value, constraint.violation_value());
        }

        /// after move
        {
            auto                                       value = random_integer();
            printemps::neighborhood::Move<int, double> move;
            move.alterations.emplace_back(&variable, value);

            auto expected_value =
                std::max(-(sensitivity * value + constant - target), 0);
            EXPECT_EQ(expected_value, constraint.evaluate_violation(move));
            constraint.update(move);
            EXPECT_EQ(expected_value, constraint.violation_value());
        }
    }
}

/*****************************************************************************/
TEST_F(TestConstraint, evaluate_violation_diff) {
    /// The test for this function is omitted.
}

/*****************************************************************************/
TEST_F(TestConstraint, update_arg_void) {
    /// This method is tested in following tests:
    /// - evaluate_function_arg_voindex()
    /// - evaluate_expression_arg_voindex()
    /// - evaluate_violation_function_arg_voindex()
    /// - evaluate_violation_expression_arg_voindex()
}

/*****************************************************************************/
TEST_F(TestConstraint, update_arg_move) {
    /// This method is tested in following tests:
    /// - evaluate_function_arg_move()
    /// - evaluate_expression_arg_move()
    /// - evaluate_violation_function_arg_move()
    /// - evaluate_violation_expression_arg_move()
}

/*****************************************************************************/
TEST_F(TestConstraint, expression) {
    /// This method is tested in constructor_arg_expression().
}

/*****************************************************************************/
TEST_F(TestConstraint, sense) {
    /// This method is tested in tested in other cases.
}

/*****************************************************************************/
TEST_F(TestConstraint, value) {
    /// This method is tested in tested in other cases.
}

/*****************************************************************************/
TEST_F(TestConstraint, violation_value) {
    /// This method is tested in tested in other cases.
}

/*****************************************************************************/
TEST_F(TestConstraint, local_penalty_coefficient) {
    auto constraint =
        printemps::model::Constraint<int, double>::create_instance();
    EXPECT_EQ(HUGE_VAL, constraint.local_penalty_coefficient());
    constraint.local_penalty_coefficient() = 10.0;
    EXPECT_EQ(10.0, constraint.local_penalty_coefficient());
}

/*****************************************************************************/
TEST_F(TestConstraint, global_penalty_coefficient) {
    auto constraint =
        printemps::model::Constraint<int, double>::create_instance();
    EXPECT_EQ(HUGE_VAL, constraint.global_penalty_coefficient());
    constraint.global_penalty_coefficient() = 100.0;
    EXPECT_EQ(100.0, constraint.global_penalty_coefficient());
}

/*****************************************************************************/
TEST_F(TestConstraint, reset_local_penalty_coefficient) {
    auto constraint =
        printemps::model::Constraint<int, double>::create_instance();
    EXPECT_EQ(HUGE_VAL, constraint.local_penalty_coefficient());
    EXPECT_EQ(HUGE_VAL, constraint.global_penalty_coefficient());
    constraint.local_penalty_coefficient()  = 10.0;
    constraint.global_penalty_coefficient() = 100.0;
    EXPECT_EQ(10.0, constraint.local_penalty_coefficient());
    EXPECT_EQ(100.0, constraint.global_penalty_coefficient());
    constraint.reset_local_penalty_coefficient();
    EXPECT_EQ(100.0, constraint.local_penalty_coefficient());
    EXPECT_EQ(100.0, constraint.global_penalty_coefficient());
}

/*****************************************************************************/
TEST_F(TestConstraint, is_linear) {
    /// This method is tested in following tests:
    /// - constructor_arg_function()
    /// - constructor_arg_expression()
}

/*****************************************************************************/
TEST_F(TestConstraint, is_singleton) {
    /// This method is tested in setup_constraint_type().
}

/*****************************************************************************/
TEST_F(TestConstraint, is_aggregation) {
    /// This method is tested in setup_constraint_type().
}

/*****************************************************************************/
TEST_F(TestConstraint, is_precedence) {
    /// This method is tested in setup_constraint_type().
}

/*****************************************************************************/
TEST_F(TestConstraint, is_variable_bound) {
    /// This method is tested in setup_constraint_type().
}

/*****************************************************************************/
TEST_F(TestConstraint, is_set_partitioning) {
    /// This method is tested in setup_constraint_type().
}

/*****************************************************************************/
TEST_F(TestConstraint, is_set_packing) {
    /// This method is tested in setup_constraint_type().
}

/*****************************************************************************/
TEST_F(TestConstraint, is_set_covering) {
    /// This method is tested in setup_constraint_type().
}

/*****************************************************************************/
TEST_F(TestConstraint, is_cardinality) {
    /// This method is tested in setup_constraint_type().
}

/*****************************************************************************/
TEST_F(TestConstraint, is_invariant_knapsack) {
    /// This method is tested in setup_constraint_type().
}

/*****************************************************************************/
TEST_F(TestConstraint, is_equation_knapsack) {
    /// This method is tested in setup_constraint_type().
}

/*****************************************************************************/
TEST_F(TestConstraint, is_bin_packing) {
    /// This method is tested in setup_constraint_type().
}

/*****************************************************************************/
TEST_F(TestConstraint, is_knapsack) {
    /// This method is tested in setup_constraint_type().
}

/*****************************************************************************/
TEST_F(TestConstraint, is_integer_knapsack) {
    /// This method is tested in setup_constraint_type().
}

/*****************************************************************************/
TEST_F(TestConstraint, is_general_linear) {
    /// This method is tested in setup_constraint_type().
}

/*****************************************************************************/
TEST_F(TestConstraint, is_enabled) {
    auto constraint =
        printemps::model::Constraint<int, double>::create_instance();
    constraint.disable();
    EXPECT_FALSE(constraint.is_enabled());

    constraint.enable();
    EXPECT_TRUE(constraint.is_enabled());

    constraint.disable();
    EXPECT_FALSE(constraint.is_enabled());
}

/*****************************************************************************/
TEST_F(TestConstraint, enable) {
    /// This method is tested in is_enabled().
}

/*****************************************************************************/
TEST_F(TestConstraint, disable) {
    /// This method is tested in is_enabled().
}

/*****************************************************************************/
TEST_F(TestConstraint, operator_equal_function) {
    auto expression =
        printemps::model::Expression<int, double>::create_instance();
    auto variable = printemps::model::Variable<int, double>::create_instance();

    auto sensitivity = random_integer();
    auto constant    = random_integer();
    auto target      = random_integer();

    expression = sensitivity * variable + constant;

    std::function<double(const printemps::neighborhood::Move<int, double>&)> f =
        [&expression,
         target](const printemps::neighborhood::Move<int, double>& a_MOVE) {
            return expression.evaluate(a_MOVE) - target;
        };

    /// Lower
    {
        auto constraint_source =
            printemps::model::Constraint<int, double>::create_instance();
        constraint_source.setup(f, printemps::model::ConstraintSense::Lower);

        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();

        EXPECT_FALSE((constraint = constraint_source).is_linear());

        EXPECT_TRUE(constraint.expression().sensitivities().empty());
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Lower, constraint.sense());
        EXPECT_EQ(0, constraint.constraint_value());
        EXPECT_EQ(0, constraint.violation_value());
        EXPECT_FALSE(constraint.is_linear());

        auto value = random_integer();
        variable   = value;

        auto expected_value = sensitivity * value + constant - target;
        EXPECT_EQ(expected_value, constraint.evaluate_constraint());
        constraint.update();
        EXPECT_EQ(expected_value, constraint.constraint_value());
    }

    /// Equal
    {
        auto constraint_source =
            printemps::model::Constraint<int, double>::create_instance();
        constraint_source.setup(f, printemps::model::ConstraintSense::Equal);
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();

        EXPECT_FALSE((constraint = constraint_source).is_linear());

        EXPECT_TRUE(constraint.expression().sensitivities().empty());
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Equal, constraint.sense());
        EXPECT_EQ(0, constraint.constraint_value());
        EXPECT_EQ(0, constraint.violation_value());
        EXPECT_FALSE(constraint.is_linear());

        auto value = random_integer();
        variable   = value;

        auto expected_value = sensitivity * value + constant - target;
        EXPECT_EQ(expected_value, constraint.evaluate_constraint());
        constraint.update();
        EXPECT_EQ(expected_value, constraint.constraint_value());
    }

    /// Upper
    {
        auto constraint_source =
            printemps::model::Constraint<int, double>::create_instance();
        constraint_source.setup(f, printemps::model::ConstraintSense::Upper);
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();

        EXPECT_FALSE((constraint = constraint_source).is_linear());

        EXPECT_TRUE(constraint.expression().sensitivities().empty());
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Upper, constraint.sense());
        EXPECT_EQ(0, constraint.constraint_value());
        EXPECT_EQ(0, constraint.violation_value());
        EXPECT_FALSE(constraint.is_linear());

        auto value = random_integer();
        variable   = value;

        auto expected_value = sensitivity * value + constant - target;
        EXPECT_EQ(expected_value, constraint.evaluate_constraint());
        constraint.update();
        EXPECT_EQ(expected_value, constraint.constraint_value());
    }
}

/*****************************************************************************/
TEST_F(TestConstraint, operator_equal_expression) {
    auto expression =
        printemps::model::Expression<int, double>::create_instance();
    auto variable = printemps::model::Variable<int, double>::create_instance();

    auto sensitivity = random_integer();
    auto constant    = random_integer();
    auto target      = random_integer();

    expression = sensitivity * variable + constant;

    /// Lower
    {
        auto constraint_source =
            printemps::model::Constraint<int, double>::create_instance();
        constraint_source.setup(expression - target,
                                printemps::model::ConstraintSense::Lower);
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();

        EXPECT_TRUE((constraint = constraint_source).is_linear());

        EXPECT_EQ(sensitivity,
                  constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(constant - target, constraint.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Lower, constraint.sense());
        EXPECT_EQ(0, constraint.constraint_value());
        EXPECT_EQ(0, constraint.violation_value());
        EXPECT_TRUE(constraint.is_linear());

        auto value = random_integer();
        variable   = value;

        auto expected_value = sensitivity * value + constant - target;
        EXPECT_EQ(expected_value, constraint.evaluate_constraint());
        constraint.update();
        EXPECT_EQ(expected_value, constraint.constraint_value());
    }

    /// Equal
    {
        auto constraint_source =
            printemps::model::Constraint<int, double>::create_instance();
        constraint_source.setup(expression - target,
                                printemps::model::ConstraintSense::Equal);
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();

        EXPECT_TRUE((constraint = constraint_source).is_linear());

        EXPECT_EQ(sensitivity,
                  constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(constant - target, constraint.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Equal, constraint.sense());
        EXPECT_EQ(0, constraint.constraint_value());
        EXPECT_EQ(0, constraint.violation_value());
        EXPECT_TRUE(constraint.is_linear());

        auto value = random_integer();
        variable   = value;

        auto expected_value = sensitivity * value + constant - target;
        EXPECT_EQ(expected_value, constraint.evaluate_constraint());
        constraint.update();
        EXPECT_EQ(expected_value, constraint.constraint_value());
    }

    /// Upper
    {
        auto constraint_source =
            printemps::model::Constraint<int, double>::create_instance();
        constraint_source.setup(expression - target,
                                printemps::model::ConstraintSense::Upper);
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();

        EXPECT_TRUE((constraint = constraint_source).is_linear());

        EXPECT_EQ(sensitivity,
                  constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(constant - target, constraint.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Upper, constraint.sense());
        EXPECT_EQ(0, constraint.constraint_value());
        EXPECT_EQ(0, constraint.violation_value());
        EXPECT_TRUE(constraint.is_linear());

        auto value = random_integer();
        variable   = value;

        auto expected_value = sensitivity * value + constant - target;
        EXPECT_EQ(expected_value, constraint.evaluate_constraint());
        constraint.update();
        EXPECT_EQ(expected_value, constraint.constraint_value());
    }
}

/*****************************************************************************/
}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/