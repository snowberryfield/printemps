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
    EXPECT_EQ(printemps::model::ConstraintSense::Less, constraint.sense());
    EXPECT_EQ(0, constraint.constraint_value());
    EXPECT_EQ(0, constraint.violation_value());
    EXPECT_EQ(0, constraint.positive_part());
    EXPECT_EQ(0, constraint.negative_part());
    EXPECT_TRUE(constraint.is_linear());
    EXPECT_TRUE(constraint.is_enabled());
    EXPECT_FALSE(constraint.is_less_or_equal());
    EXPECT_FALSE(constraint.is_greater_or_equal());
    EXPECT_EQ(HUGE_VALF, constraint.local_penalty_coefficient_less());
    EXPECT_EQ(HUGE_VALF, constraint.local_penalty_coefficient_greater());
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
    EXPECT_FALSE(constraint.is_min_max());
    EXPECT_FALSE(constraint.is_max_min());
    EXPECT_FALSE(constraint.is_intermediate());
    EXPECT_FALSE(constraint.is_general_linear());
    EXPECT_EQ(nullptr, constraint.intermediate_variable_ptr());
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

    /// Less
    {
        printemps::model::Constraint<int, double> constraint(f <= target);

        EXPECT_TRUE(constraint.expression().sensitivities().empty());
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Less, constraint.sense());
        EXPECT_EQ(0, constraint.constraint_value());
        EXPECT_EQ(0, constraint.violation_value());
        EXPECT_EQ(0, constraint.positive_part());
        EXPECT_EQ(0, constraint.negative_part());
        EXPECT_FALSE(constraint.is_linear());
        EXPECT_TRUE(constraint.is_enabled());
        EXPECT_TRUE(constraint.is_less_or_equal());
        EXPECT_FALSE(constraint.is_greater_or_equal());
    }

    /// Equal
    {
        printemps::model::Constraint<int, double> constraint(f == target);

        EXPECT_TRUE(constraint.expression().sensitivities().empty());
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Equal, constraint.sense());
        EXPECT_EQ(0, constraint.constraint_value());
        EXPECT_EQ(0, constraint.violation_value());
        EXPECT_EQ(0, constraint.positive_part());
        EXPECT_EQ(0, constraint.negative_part());
        EXPECT_FALSE(constraint.is_linear());
        EXPECT_TRUE(constraint.is_enabled());
        EXPECT_TRUE(constraint.is_less_or_equal());
        EXPECT_TRUE(constraint.is_greater_or_equal());
    }

    /// Greater
    {
        printemps::model::Constraint<int, double> constraint(f >= target);

        EXPECT_TRUE(constraint.expression().sensitivities().empty());
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Greater,
                  constraint.sense());
        EXPECT_EQ(0, constraint.constraint_value());
        EXPECT_EQ(0, constraint.violation_value());
        EXPECT_EQ(0, constraint.positive_part());
        EXPECT_EQ(0, constraint.negative_part());
        EXPECT_FALSE(constraint.is_linear());
        EXPECT_TRUE(constraint.is_enabled());
        EXPECT_FALSE(constraint.is_less_or_equal());
        EXPECT_TRUE(constraint.is_greater_or_equal());
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

    /// Less
    {
        printemps::model::Constraint<int, double> constraint(expression <=
                                                             target);

        EXPECT_EQ(sensitivity,
                  constraint.expression().sensitivities().at(&variable));

        EXPECT_EQ(constant - target, constraint.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Less, constraint.sense());
        EXPECT_EQ(0, constraint.constraint_value());
        EXPECT_EQ(0, constraint.violation_value());
        EXPECT_EQ(0, constraint.positive_part());
        EXPECT_EQ(0, constraint.negative_part());
        EXPECT_TRUE(constraint.is_linear());
        EXPECT_TRUE(constraint.is_enabled());
        EXPECT_TRUE(constraint.is_less_or_equal());
        EXPECT_FALSE(constraint.is_greater_or_equal());
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
        EXPECT_EQ(0, constraint.positive_part());
        EXPECT_EQ(0, constraint.negative_part());
        EXPECT_TRUE(constraint.is_linear());
        EXPECT_TRUE(constraint.is_enabled());
        EXPECT_TRUE(constraint.is_less_or_equal());
        EXPECT_TRUE(constraint.is_greater_or_equal());
    }

    /// Greater
    {
        printemps::model::Constraint<int, double> constraint(expression >=
                                                             target);

        EXPECT_EQ(sensitivity,
                  constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(constant - target, constraint.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Greater,
                  constraint.sense());
        EXPECT_EQ(0, constraint.constraint_value());
        EXPECT_EQ(0, constraint.violation_value());
        EXPECT_EQ(0, constraint.positive_part());
        EXPECT_EQ(0, constraint.negative_part());
        EXPECT_TRUE(constraint.is_linear());
        EXPECT_TRUE(constraint.is_enabled());
        EXPECT_FALSE(constraint.is_less_or_equal());
        EXPECT_TRUE(constraint.is_greater_or_equal());
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

    /// Less
    {
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();

        constraint.setup(f, printemps::model::ConstraintSense::Less);

        EXPECT_TRUE(constraint.expression().sensitivities().empty());
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Less, constraint.sense());
        EXPECT_EQ(0, constraint.constraint_value());
        EXPECT_EQ(0, constraint.violation_value());
        EXPECT_EQ(0, constraint.positive_part());
        EXPECT_EQ(0, constraint.negative_part());
        EXPECT_FALSE(constraint.is_linear());
        EXPECT_TRUE(constraint.is_enabled());
        EXPECT_TRUE(constraint.is_less_or_equal());
        EXPECT_FALSE(constraint.is_greater_or_equal());
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
        EXPECT_EQ(0, constraint.positive_part());
        EXPECT_EQ(0, constraint.negative_part());
        EXPECT_FALSE(constraint.is_linear());
        EXPECT_TRUE(constraint.is_enabled());
        EXPECT_TRUE(constraint.is_less_or_equal());
        EXPECT_TRUE(constraint.is_greater_or_equal());
    }

    /// Greater
    {
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();
        ;
        constraint.setup(f, printemps::model::ConstraintSense::Greater);

        EXPECT_TRUE(constraint.expression().sensitivities().empty());
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Greater,
                  constraint.sense());
        EXPECT_EQ(0, constraint.constraint_value());
        EXPECT_EQ(0, constraint.violation_value());
        EXPECT_EQ(0, constraint.positive_part());
        EXPECT_EQ(0, constraint.negative_part());
        EXPECT_FALSE(constraint.is_linear());
        EXPECT_TRUE(constraint.is_enabled());
        EXPECT_FALSE(constraint.is_less_or_equal());
        EXPECT_TRUE(constraint.is_greater_or_equal());
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

    /// Less
    {
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();
        constraint.setup(expression - target,
                         printemps::model::ConstraintSense::Less);

        EXPECT_EQ(sensitivity,
                  constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(constant - target, constraint.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Less, constraint.sense());
        EXPECT_EQ(0, constraint.constraint_value());
        EXPECT_EQ(0, constraint.violation_value());
        EXPECT_EQ(0, constraint.positive_part());
        EXPECT_EQ(0, constraint.negative_part());
        EXPECT_TRUE(constraint.is_linear());
        EXPECT_TRUE(constraint.is_enabled());
        EXPECT_TRUE(constraint.is_less_or_equal());
        EXPECT_FALSE(constraint.is_greater_or_equal());
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
        EXPECT_EQ(0, constraint.positive_part());
        EXPECT_EQ(0, constraint.negative_part());
        EXPECT_TRUE(constraint.is_linear());
        EXPECT_TRUE(constraint.is_enabled());
        EXPECT_TRUE(constraint.is_less_or_equal());
        EXPECT_TRUE(constraint.is_greater_or_equal());
    }

    /// Greater
    {
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();
        constraint.setup(expression - target,
                         printemps::model::ConstraintSense::Greater);

        EXPECT_EQ(sensitivity,
                  constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(constant - target, constraint.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Greater,
                  constraint.sense());
        EXPECT_EQ(0, constraint.constraint_value());
        EXPECT_EQ(0, constraint.violation_value());
        EXPECT_EQ(0, constraint.positive_part());
        EXPECT_EQ(0, constraint.negative_part());
        EXPECT_TRUE(constraint.is_linear());
        EXPECT_TRUE(constraint.is_enabled());
        EXPECT_FALSE(constraint.is_less_or_equal());
        EXPECT_TRUE(constraint.is_greater_or_equal());
    }
}

/*****************************************************************************/
TEST_F(TestConstraint, setup_constraint_type_singleton) {
    printemps::model::Model<int, double> model;
    auto& x = model.create_variable("x", -10, 10);
    auto  constraint =
        printemps::model::Constraint<int, double>::create_instance();
    constraint.setup(2 * x - 10, printemps::model::ConstraintSense::Less);
    constraint.setup_constraint_type();
    EXPECT_TRUE(constraint.is_singleton());
}

/*****************************************************************************/
TEST_F(TestConstraint, setup_constraint_type_aggregation) {
    printemps::model::Model<int, double> model;
    auto& x = model.create_variable("x", -10, 10);
    auto& y = model.create_variable("y", -10, 10);
    auto  constraint =
        printemps::model::Constraint<int, double>::create_instance();
    constraint.setup(2 * x + 3 * y - 10,
                     printemps::model::ConstraintSense::Equal);
    constraint.setup_constraint_type();
    EXPECT_TRUE(constraint.is_aggregation());
}

/*****************************************************************************/
TEST_F(TestConstraint, setup_constraint_type_precedence) {
    printemps::model::Model<int, double> model;
    auto& x = model.create_variable("x", -10, 10);
    auto& y = model.create_variable("y", -10, 10);
    {
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();
        constraint.setup(2 * x - 2 * y - 5,
                         printemps::model::ConstraintSense::Less);
        constraint.setup_constraint_type();
        EXPECT_TRUE(constraint.is_precedence());
    }
    {
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();
        constraint.setup(-2 * x + 2 * y - 5,
                         printemps::model::ConstraintSense::Less);
        constraint.setup_constraint_type();
        EXPECT_TRUE(constraint.is_precedence());
    }
    {
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();
        constraint.setup(2 * x - 2 * y - 5,
                         printemps::model::ConstraintSense::Greater);
        constraint.setup_constraint_type();
        EXPECT_TRUE(constraint.is_precedence());
    }
    {
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();
        constraint.setup(-2 * x + 2 * y - 5,
                         printemps::model::ConstraintSense::Greater);
        constraint.setup_constraint_type();
        EXPECT_TRUE(constraint.is_precedence());
    }
}

/*****************************************************************************/
TEST_F(TestConstraint, setup_constraint_type_variable_bound) {
    printemps::model::Model<int, double> model;
    auto&                                x = model.create_variable("x", 0, 1);
    auto&                                y = model.create_variable("y", 0, 1);
    /// Variable Bound
    {
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();
        constraint.setup(2 * x + 3 * y - 5,
                         printemps::model::ConstraintSense::Less);
        constraint.setup_constraint_type();
        EXPECT_TRUE(constraint.is_variable_bound());
    }
    {
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();
        constraint.setup(2 * x + 3 * y - 5,
                         printemps::model::ConstraintSense::Greater);
        constraint.setup_constraint_type();
        EXPECT_TRUE(constraint.is_variable_bound());
    }
}

/*****************************************************************************/
TEST_F(TestConstraint, setup_constraint_type_set_partitioning) {
    printemps::model::Model<int, double> model;
    auto& x = model.create_variables("x", 10, 0, 1);
    auto  constraint =
        printemps::model::Constraint<int, double>::create_instance();
    constraint.setup(x.sum() - 1, printemps::model::ConstraintSense::Equal);
    constraint.setup_constraint_type();
    EXPECT_TRUE(constraint.is_set_partitioning());
}

/*****************************************************************************/
TEST_F(TestConstraint, setup_constraint_type_set_packing) {
    printemps::model::Model<int, double> model;
    auto& x = model.create_variables("x", 10, 0, 1);
    auto  constraint =
        printemps::model::Constraint<int, double>::create_instance();
    constraint.setup(x.sum() - 1, printemps::model::ConstraintSense::Less);
    constraint.setup_constraint_type();
    EXPECT_TRUE(constraint.is_set_packing());
}

/*****************************************************************************/
TEST_F(TestConstraint, setup_constraint_type_set_covering) {
    printemps::model::Model<int, double> model;
    auto& x = model.create_variables("x", 10, 0, 1);
    auto  constraint =
        printemps::model::Constraint<int, double>::create_instance();
    constraint.setup(x.sum() - 1, printemps::model::ConstraintSense::Greater);
    constraint.setup_constraint_type();
    EXPECT_TRUE(constraint.is_set_covering());
}

/*****************************************************************************/
TEST_F(TestConstraint, setup_constraint_type_cardinality) {
    printemps::model::Model<int, double> model;
    auto& x = model.create_variables("x", 10, 0, 1);
    auto  constraint =
        printemps::model::Constraint<int, double>::create_instance();
    constraint.setup(x.sum() - 5, printemps::model::ConstraintSense::Equal);
    constraint.setup_constraint_type();
    EXPECT_TRUE(constraint.is_cardinality());
}

/*****************************************************************************/
TEST_F(TestConstraint, setup_constraint_type_invariant_knapsack) {
    printemps::model::Model<int, double> model;
    auto& x = model.create_variables("x", 10, 0, 1);
    auto  constraint =
        printemps::model::Constraint<int, double>::create_instance();
    constraint.setup(x.sum() - 5, printemps::model::ConstraintSense::Less);
    constraint.setup_constraint_type();
    EXPECT_TRUE(constraint.is_invariant_knapsack());
}

/*****************************************************************************/
TEST_F(TestConstraint, setup_constraint_type_equation_knapsack) {
    printemps::model::Model<int, double> model;
    auto coefficients = printemps::utility::sequence(10);

    auto& x = model.create_variables("x", 10, 0, 1);
    auto  constraint =
        printemps::model::Constraint<int, double>::create_instance();
    constraint.setup(x.dot(coefficients) - 30,
                     printemps::model::ConstraintSense::Equal);
    constraint.setup_constraint_type();
    EXPECT_TRUE(constraint.is_equation_knapsack());
}

/*****************************************************************************/
TEST_F(TestConstraint, setup_constraint_type_bin_packing) {
    printemps::model::Model<int, double> model;
    auto coefficients = printemps::utility::sequence(10);

    auto& x = model.create_variables("x", 10, 0, 1);
    auto& y = model.create_variable("y", 0, 1);
    {
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();
        constraint.setup(x.dot(coefficients) + 5 * y - 5,
                         printemps::model::ConstraintSense::Less);
        constraint.setup_constraint_type();
        EXPECT_TRUE(constraint.is_bin_packing());
    }

    {
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();
        constraint.setup(x.dot(coefficients) - 5 * y + 5,
                         printemps::model::ConstraintSense::Greater);
        constraint.setup_constraint_type();
        EXPECT_TRUE(constraint.is_bin_packing());
    }
}

/*****************************************************************************/
TEST_F(TestConstraint, setup_constraint_type_knapsack) {
    printemps::model::Model<int, double> model;
    auto coefficients = printemps::utility::sequence(10);

    auto& x = model.create_variables("x", 10, 0, 1);
    {
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();
        constraint.setup(x.dot(coefficients) - 50,
                         printemps::model::ConstraintSense::Less);
        constraint.setup_constraint_type();
        EXPECT_TRUE(constraint.is_knapsack());
    }

    {
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();
        constraint.setup(x.dot(coefficients) + 50,
                         printemps::model::ConstraintSense::Greater);
        constraint.setup_constraint_type();
        EXPECT_TRUE(constraint.is_knapsack());
    }
}

/*****************************************************************************/
TEST_F(TestConstraint, setup_constraint_type_integer_knapsack) {
    printemps::model::Model<int, double> model;
    auto coefficients = printemps::utility::sequence(10);

    auto& x = model.create_variables("x", 10, 0, 10);
    {
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();
        constraint.setup(x.dot(coefficients) - 50,
                         printemps::model::ConstraintSense::Less);
        constraint.setup_constraint_type();
        EXPECT_TRUE(constraint.is_integer_knapsack());
    }

    {
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();
        constraint.setup(x.dot(coefficients) + 50,
                         printemps::model::ConstraintSense::Greater);
        constraint.setup_constraint_type();
        EXPECT_TRUE(constraint.is_integer_knapsack());
    }
}

/*****************************************************************************/
TEST_F(TestConstraint, setup_constraint_type_min_max) {
    printemps::model::Model<int, double> model;

    auto& x = model.create_variable("x", -10, 10);
    auto& y = model.create_variable("y", 0, 1);
    auto& z = model.create_variable("z", 0, 2);
    {
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();
        constraint.setup(-x + y + 2 * z + 5,
                         printemps::model::ConstraintSense::Less);
        constraint.setup_constraint_type();
        EXPECT_TRUE(constraint.is_min_max());
        EXPECT_FALSE(constraint.has_intermediate_lower_bound());
        EXPECT_FALSE(constraint.has_intermediate_upper_bound());
    }

    {
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();
        constraint.setup(x - y - 2 * z - 5,
                         printemps::model::ConstraintSense::Greater);
        constraint.setup_constraint_type();
        EXPECT_TRUE(constraint.is_min_max());
        EXPECT_FALSE(constraint.has_intermediate_lower_bound());
        EXPECT_FALSE(constraint.has_intermediate_upper_bound());
    }

    {
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();
        constraint.setup(-x + 20 * y + 20 * z,
                         printemps::model::ConstraintSense::Less);
        constraint.setup_constraint_type();
        EXPECT_TRUE(constraint.is_min_max());
        EXPECT_FALSE(constraint.has_intermediate_lower_bound());
        EXPECT_TRUE(constraint.has_intermediate_upper_bound());
    }

    {
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();
        constraint.setup(-x - 20 * y - 20 * z,
                         printemps::model::ConstraintSense::Less);
        constraint.setup_constraint_type();
        EXPECT_TRUE(constraint.is_min_max());
        EXPECT_TRUE(constraint.has_intermediate_lower_bound());
        EXPECT_FALSE(constraint.has_intermediate_upper_bound());
    }

    {
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();
        constraint.setup(-x - 20 * y + 20 * z,
                         printemps::model::ConstraintSense::Less);
        constraint.setup_constraint_type();
        EXPECT_TRUE(constraint.is_min_max());
        EXPECT_TRUE(constraint.has_intermediate_lower_bound());
        EXPECT_TRUE(constraint.has_intermediate_upper_bound());
    }

    {
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();
        constraint.setup(x + 20 * y - 20 * z,
                         printemps::model::ConstraintSense::Greater);
        constraint.setup_constraint_type();
        EXPECT_TRUE(constraint.is_min_max());
        EXPECT_TRUE(constraint.has_intermediate_lower_bound());
        EXPECT_TRUE(constraint.has_intermediate_upper_bound());
    }

    {
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();
        constraint.setup(-x + y + z + 5,
                         printemps::model::ConstraintSense::Less);
        constraint.setup_constraint_type();
        EXPECT_FALSE(constraint.is_min_max());
        EXPECT_FALSE(constraint.has_intermediate_lower_bound());
        EXPECT_FALSE(constraint.has_intermediate_upper_bound());
    }

    {
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();
        constraint.setup(-x + 1.5 * y + 2 * z + 5,
                         printemps::model::ConstraintSense::Less);
        constraint.setup_constraint_type();
        EXPECT_FALSE(constraint.is_min_max());
        EXPECT_FALSE(constraint.has_intermediate_lower_bound());
        EXPECT_FALSE(constraint.has_intermediate_upper_bound());
    }

    {
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();
        constraint.setup(-x + y + 2 * z + 5.5,
                         printemps::model::ConstraintSense::Less);
        constraint.setup_constraint_type();
        EXPECT_FALSE(constraint.is_min_max());
        EXPECT_FALSE(constraint.has_intermediate_lower_bound());
        EXPECT_FALSE(constraint.has_intermediate_upper_bound());
    }
}

/*****************************************************************************/
TEST_F(TestConstraint, setup_constraint_type_max_min) {
    printemps::model::Model<int, double> model;

    auto& x = model.create_variable("x", -10, 10);
    auto& y = model.create_variable("y", 0, 1);
    auto& z = model.create_variable("z", 0, 2);
    {
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();
        constraint.setup(-x + y + 2 * z - 5,
                         printemps::model::ConstraintSense::Greater);
        constraint.setup_constraint_type();
        EXPECT_TRUE(constraint.is_max_min());
        EXPECT_FALSE(constraint.has_intermediate_lower_bound());
        EXPECT_FALSE(constraint.has_intermediate_upper_bound());
    }

    {
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();
        constraint.setup(x - y - 2 * z + 5,
                         printemps::model::ConstraintSense::Less);
        constraint.setup_constraint_type();
        EXPECT_TRUE(constraint.is_max_min());
        EXPECT_FALSE(constraint.has_intermediate_lower_bound());
        EXPECT_FALSE(constraint.has_intermediate_upper_bound());
    }

    {
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();
        constraint.setup(-x + 20 * y + 20 * z,
                         printemps::model::ConstraintSense::Greater);
        constraint.setup_constraint_type();
        EXPECT_TRUE(constraint.is_max_min());
        EXPECT_FALSE(constraint.has_intermediate_lower_bound());
        EXPECT_TRUE(constraint.has_intermediate_upper_bound());
    }

    {
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();
        constraint.setup(-x - 20 * y - 20 * z,
                         printemps::model::ConstraintSense::Greater);
        constraint.setup_constraint_type();
        EXPECT_TRUE(constraint.is_max_min());
        EXPECT_TRUE(constraint.has_intermediate_lower_bound());
        EXPECT_FALSE(constraint.has_intermediate_upper_bound());
    }

    {
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();
        constraint.setup(-x - 20 * y + 20 * z,
                         printemps::model::ConstraintSense::Greater);
        constraint.setup_constraint_type();
        EXPECT_TRUE(constraint.is_max_min());
        EXPECT_TRUE(constraint.has_intermediate_lower_bound());
        EXPECT_TRUE(constraint.has_intermediate_upper_bound());
    }

    {
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();
        constraint.setup(x + 20 * y - 20 * z,
                         printemps::model::ConstraintSense::Less);
        constraint.setup_constraint_type();
        EXPECT_TRUE(constraint.is_max_min());
        EXPECT_TRUE(constraint.has_intermediate_lower_bound());
        EXPECT_TRUE(constraint.has_intermediate_upper_bound());
    }

    {
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();
        constraint.setup(-x + y + z - 5,
                         printemps::model::ConstraintSense::Greater);
        constraint.setup_constraint_type();
        EXPECT_FALSE(constraint.is_max_min());
        EXPECT_FALSE(constraint.has_intermediate_lower_bound());
        EXPECT_FALSE(constraint.has_intermediate_upper_bound());
    }

    {
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();
        constraint.setup(-x + 1.5 * y + 2 * z - 5,
                         printemps::model::ConstraintSense::Greater);
        constraint.setup_constraint_type();
        EXPECT_FALSE(constraint.is_max_min());
        EXPECT_FALSE(constraint.has_intermediate_lower_bound());
        EXPECT_FALSE(constraint.has_intermediate_upper_bound());
    }

    {
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();
        constraint.setup(-x + y + 2 * z - 5.5,
                         printemps::model::ConstraintSense::Greater);
        constraint.setup_constraint_type();
        EXPECT_FALSE(constraint.is_max_min());
        EXPECT_FALSE(constraint.has_intermediate_lower_bound());
        EXPECT_FALSE(constraint.has_intermediate_upper_bound());
    }
}

/*****************************************************************************/
TEST_F(TestConstraint, setup_constraint_type_intermediate) {
    printemps::model::Model<int, double> model;

    auto& x = model.create_variable("x", -10, 10);
    auto& y = model.create_variable("y", 0, 1);
    auto& z = model.create_variable("z", 0, 2);

    {
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();
        constraint.setup(-x + y + 2 * z + 5,
                         printemps::model::ConstraintSense::Equal);
        constraint.setup_constraint_type();
        EXPECT_TRUE(constraint.is_intermediate());
        EXPECT_FALSE(constraint.has_intermediate_lower_bound());
        EXPECT_FALSE(constraint.has_intermediate_upper_bound());
        EXPECT_EQ(&x(0), constraint.intermediate_variable_ptr());
    }

    {
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();
        constraint.setup(x + y + 2 * z + 5,
                         printemps::model::ConstraintSense::Equal);
        constraint.setup_constraint_type();
        EXPECT_TRUE(constraint.is_intermediate());
        EXPECT_FALSE(constraint.has_intermediate_lower_bound());
        EXPECT_FALSE(constraint.has_intermediate_upper_bound());
        EXPECT_EQ(&x(0), constraint.intermediate_variable_ptr());
    }

    {
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();
        constraint.setup(x + 20 * y + 20 * z,
                         printemps::model::ConstraintSense::Equal);
        constraint.setup_constraint_type();
        EXPECT_TRUE(constraint.is_intermediate());
        EXPECT_TRUE(constraint.has_intermediate_lower_bound());
        EXPECT_FALSE(constraint.has_intermediate_upper_bound());
        EXPECT_EQ(&x(0), constraint.intermediate_variable_ptr());
    }

    {
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();
        constraint.setup(x + 20 * y - 20 * z,
                         printemps::model::ConstraintSense::Equal);
        constraint.setup_constraint_type();
        EXPECT_TRUE(constraint.is_intermediate());
        EXPECT_TRUE(constraint.has_intermediate_lower_bound());
        EXPECT_TRUE(constraint.has_intermediate_upper_bound());
        EXPECT_EQ(&x(0), constraint.intermediate_variable_ptr());
    }

    {
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();
        constraint.setup(-x + y + z + 5,
                         printemps::model::ConstraintSense::Equal);
        constraint.setup_constraint_type();
        EXPECT_FALSE(constraint.is_intermediate());
        EXPECT_FALSE(constraint.has_intermediate_lower_bound());
        EXPECT_FALSE(constraint.has_intermediate_upper_bound());
        EXPECT_EQ(nullptr, constraint.intermediate_variable_ptr());
    }

    {
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();
        constraint.setup(-x + 1.5 * y + 2 * z + 5,
                         printemps::model::ConstraintSense::Equal);
        constraint.setup_constraint_type();
        EXPECT_FALSE(constraint.is_intermediate());
        EXPECT_FALSE(constraint.has_intermediate_lower_bound());
        EXPECT_FALSE(constraint.has_intermediate_upper_bound());
        EXPECT_EQ(nullptr, constraint.intermediate_variable_ptr());
    }

    {
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();
        constraint.setup(-x + y + 2 * z + 5.5,
                         printemps::model::ConstraintSense::Equal);
        constraint.setup_constraint_type();
        EXPECT_FALSE(constraint.is_intermediate());
        EXPECT_FALSE(constraint.has_intermediate_lower_bound());
        EXPECT_FALSE(constraint.has_intermediate_upper_bound());
        EXPECT_EQ(nullptr, constraint.intermediate_variable_ptr());
    }

    {
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();
        constraint.setup(-2 * x + y + 2 * z + 5,
                         printemps::model::ConstraintSense::Equal);
        constraint.setup_constraint_type();
        EXPECT_FALSE(constraint.is_intermediate());
        EXPECT_FALSE(constraint.has_intermediate_lower_bound());
        EXPECT_FALSE(constraint.has_intermediate_upper_bound());
        EXPECT_EQ(nullptr, constraint.intermediate_variable_ptr());
    }
}

/*****************************************************************************/
TEST_F(TestConstraint, setup_constraint_type_general_linear) {
    printemps::model::Model<int, double> model;
    auto coefficients = printemps::utility::sequence(10);

    auto& x = model.create_variable("x", 0, 1);
    auto& y = model.create_variables("y", 10, -10, 10);

    auto constraint =
        printemps::model::Constraint<int, double>::create_instance();
    constraint.setup(x + y.sum() - 50,
                     printemps::model::ConstraintSense::Equal);
    constraint.setup_constraint_type();
    EXPECT_TRUE(constraint.is_general_linear());
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

    constraint.setup(f, printemps::model::ConstraintSense::Less);

    auto value = random_integer();
    variable   = value;

    auto expected_value = sensitivity * value + constant - target;
    EXPECT_EQ(expected_value, constraint.evaluate_constraint());
    constraint.update();
    EXPECT_EQ(expected_value, constraint.constraint_value());

    EXPECT_EQ(std::max(expected_value, 0), constraint.positive_part());
    EXPECT_EQ(-std::min(expected_value, 0), constraint.negative_part());
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
                     printemps::model::ConstraintSense::Less);
    constraint.expression().setup_fixed_sensitivities();

    auto value = random_integer();
    variable   = value;

    auto expected_value = sensitivity * value + constant - target;
    EXPECT_EQ(expected_value, constraint.evaluate_constraint());
    constraint.update();
    EXPECT_EQ(expected_value, constraint.constraint_value());

    EXPECT_EQ(std::max(expected_value, 0), constraint.positive_part());
    EXPECT_EQ(-std::min(expected_value, 0), constraint.negative_part());
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
    constraint.setup(f, printemps::model::ConstraintSense::Less);

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

        EXPECT_EQ(std::max(expected_value, 0), constraint.positive_part());
        EXPECT_EQ(-std::min(expected_value, 0), constraint.negative_part());
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

        EXPECT_EQ(std::max(expected_value, 0), constraint.positive_part());
        EXPECT_EQ(-std::min(expected_value, 0), constraint.negative_part());
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
                     printemps::model::ConstraintSense::Less);
    constraint.expression().setup_fixed_sensitivities();
    /// initial
    {
        auto value = random_integer();
        variable   = value;
        constraint.update();

        auto expected_value = sensitivity * value + constant - target;
        EXPECT_EQ(expected_value, constraint.evaluate_constraint());
        constraint.update();
        EXPECT_EQ(expected_value, constraint.constraint_value());

        EXPECT_EQ(std::max(expected_value, 0), constraint.positive_part());
        EXPECT_EQ(-std::min(expected_value, 0), constraint.negative_part());
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

        EXPECT_EQ(std::max(expected_value, 0), constraint.positive_part());
        EXPECT_EQ(-std::min(expected_value, 0), constraint.negative_part());
    }
}

/*****************************************************************************/
TEST_F(TestConstraint, evaluate_constraint_with_mask) {
    auto constraint =
        printemps::model::Constraint<int, double>::create_instance();

    auto variable_0 =
        printemps::model::Variable<int, double>::create_instance();
    auto variable_1 =
        printemps::model::Variable<int, double>::create_instance();
    auto variable_2 =
        printemps::model::Variable<int, double>::create_instance();

    constraint = variable_0 - variable_1 + 2 * variable_2 + 3 == 0;

    constraint.expression().setup_fixed_sensitivities();
    constraint.expression().setup_mask();

    variable_0 = 0;
    variable_1 = 0;
    variable_2 = 0;

    constraint.update();

    EXPECT_EQ(4, constraint.evaluate_constraint_with_mask(&variable_0, 1));
    EXPECT_EQ(2, constraint.evaluate_constraint_with_mask(&variable_1, 1));
    EXPECT_EQ(5, constraint.evaluate_constraint_with_mask(&variable_2, 1));
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

    /// Less
    {
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();
        constraint.setup(f, printemps::model::ConstraintSense::Less);

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

    /// Greater
    {
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();
        constraint.setup(f, printemps::model::ConstraintSense::Greater);

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

    /// Less
    {
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();
        constraint.setup(expression - target,
                         printemps::model::ConstraintSense::Less);
        constraint.expression().setup_fixed_sensitivities();

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
        constraint.expression().setup_fixed_sensitivities();

        auto value = random_integer();
        variable   = value;

        auto expected_value = std::abs(sensitivity * value + constant - target);
        EXPECT_EQ(expected_value, constraint.evaluate_violation());
        constraint.update();
        EXPECT_EQ(expected_value, constraint.violation_value());
    }

    /// Greater
    {
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();
        constraint.setup(expression - target,
                         printemps::model::ConstraintSense::Greater);
        constraint.expression().setup_fixed_sensitivities();

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

    /// Less
    {
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();
        constraint.setup(f, printemps::model::ConstraintSense::Less);

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

    /// Greater
    {
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();
        constraint.setup(f, printemps::model::ConstraintSense::Greater);

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

    /// Less
    {
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();
        constraint.setup(expression - target,
                         printemps::model::ConstraintSense::Less);
        constraint.expression().setup_fixed_sensitivities();

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
        constraint.expression().setup_fixed_sensitivities();

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

    /// Greater
    {
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();
        constraint.setup(expression - target,
                         printemps::model::ConstraintSense::Greater);
        constraint.expression().setup_fixed_sensitivities();

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
    /// - evaluate_function_arg_void()
    /// - evaluate_expression_arg_void()
    /// - evaluate_violation_function_arg_void()
    /// - evaluate_violation_expression_arg_void()
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
TEST_F(TestConstraint, positive_part) {
    /// This method is tested in tested in other cases.
}

/*****************************************************************************/
TEST_F(TestConstraint, negative_part) {
    /// This method is tested in tested in other cases.
}

/*****************************************************************************/
TEST_F(TestConstraint, local_penalty_coefficient_less) {
    auto constraint =
        printemps::model::Constraint<int, double>::create_instance();
    EXPECT_EQ(HUGE_VAL, constraint.local_penalty_coefficient_less());
    constraint.local_penalty_coefficient_less() = 10.0;
    EXPECT_EQ(10.0, constraint.local_penalty_coefficient_less());
}

/*****************************************************************************/
TEST_F(TestConstraint, local_penalty_coefficient_greater) {
    auto constraint =
        printemps::model::Constraint<int, double>::create_instance();
    EXPECT_EQ(HUGE_VAL, constraint.local_penalty_coefficient_greater());
    constraint.local_penalty_coefficient_greater() = 10.0;
    EXPECT_EQ(10.0, constraint.local_penalty_coefficient_greater());
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
    EXPECT_EQ(HUGE_VAL, constraint.local_penalty_coefficient_less());
    EXPECT_EQ(HUGE_VAL, constraint.local_penalty_coefficient_greater());
    EXPECT_EQ(HUGE_VAL, constraint.global_penalty_coefficient());
    constraint.local_penalty_coefficient_less()    = 10.0;
    constraint.local_penalty_coefficient_greater() = 10.0;
    constraint.global_penalty_coefficient()        = 100.0;
    EXPECT_EQ(10.0, constraint.local_penalty_coefficient_less());
    EXPECT_EQ(10.0, constraint.local_penalty_coefficient_greater());
    EXPECT_EQ(100.0, constraint.global_penalty_coefficient());
    constraint.reset_local_penalty_coefficient();
    EXPECT_EQ(100.0, constraint.local_penalty_coefficient_less());
    EXPECT_EQ(100.0, constraint.local_penalty_coefficient_greater());
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
    /// This method is tested in setup_constraint_type_singleton().
}

/*****************************************************************************/
TEST_F(TestConstraint, is_aggregation) {
    /// This method is tested in setup_constraint_type_aggregation().
}

/*****************************************************************************/
TEST_F(TestConstraint, is_precedence) {
    /// This method is tested in setup_constraint_type_precedence().
}

/*****************************************************************************/
TEST_F(TestConstraint, is_variable_bound) {
    /// This method is tested in setup_constraint_type_variable_bound().
}

/*****************************************************************************/
TEST_F(TestConstraint, is_set_partitioning) {
    /// This method is tested in setup_constraint_type_set_partitioning().
}

/*****************************************************************************/
TEST_F(TestConstraint, is_set_packing) {
    /// This method is tested in setup_constraint_type_set_packing().
}

/*****************************************************************************/
TEST_F(TestConstraint, is_set_covering) {
    /// This method is tested in setup_constraint_type_set_covering().
}

/*****************************************************************************/
TEST_F(TestConstraint, is_cardinality) {
    /// This method is tested in setup_constraint_type_cardinality().
}

/*****************************************************************************/
TEST_F(TestConstraint, is_invariant_knapsack) {
    /// This method is tested in setup_constraint_type_invariant_knapsack().
}

/*****************************************************************************/
TEST_F(TestConstraint, is_equation_knapsack) {
    /// This method is tested in setup_constraint_type_equation_knapsack().
}

/*****************************************************************************/
TEST_F(TestConstraint, is_bin_packing) {
    /// This method is tested in setup_constraint_type_bin_packing().
}

/*****************************************************************************/
TEST_F(TestConstraint, is_knapsack) {
    /// This method is tested in setup_constraint_type_knapsack().
}

/*****************************************************************************/
TEST_F(TestConstraint, is_integer_knapsack) {
    /// This method is tested in setup_constraint_type_integer_knapsack().
}

/*****************************************************************************/
TEST_F(TestConstraint, is_min_max) {
    /// This method is tested in setup_constraint_type_min_max().
}

/*****************************************************************************/
TEST_F(TestConstraint, is_max_min) {
    /// This method is tested in setup_constraint_type_max_min().
}

/*****************************************************************************/
TEST_F(TestConstraint, is_intermediate) {
    /// This method is tested in setup_constraint_type_intermediate().
}

/*****************************************************************************/
TEST_F(TestConstraint, intermediate_variable_ptr) {
    /// This method is tested in setup_constraint_type_intermediate().
}

/*****************************************************************************/
TEST_F(TestConstraint, is_general_linear) {
    /// This method is tested in setup_constraint_type_general_linear().
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

    /// Less
    {
        auto constraint_source =
            printemps::model::Constraint<int, double>::create_instance();
        constraint_source.setup(f, printemps::model::ConstraintSense::Less);

        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();

        EXPECT_FALSE((constraint = constraint_source).is_linear());

        EXPECT_TRUE(constraint.expression().sensitivities().empty());
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Less, constraint.sense());
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

    /// Greater
    {
        auto constraint_source =
            printemps::model::Constraint<int, double>::create_instance();
        constraint_source.setup(f, printemps::model::ConstraintSense::Greater);
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();

        EXPECT_FALSE((constraint = constraint_source).is_linear());

        EXPECT_TRUE(constraint.expression().sensitivities().empty());
        EXPECT_EQ(0, constraint.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Greater,
                  constraint.sense());
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

    /// Less
    {
        auto constraint_source =
            printemps::model::Constraint<int, double>::create_instance();
        constraint_source.setup(expression - target,
                                printemps::model::ConstraintSense::Less);
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();

        EXPECT_TRUE((constraint = constraint_source).is_linear());

        EXPECT_EQ(sensitivity,
                  constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(constant - target, constraint.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Less, constraint.sense());
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

    /// Greater
    {
        auto constraint_source =
            printemps::model::Constraint<int, double>::create_instance();
        constraint_source.setup(expression - target,
                                printemps::model::ConstraintSense::Greater);
        auto constraint =
            printemps::model::Constraint<int, double>::create_instance();

        EXPECT_TRUE((constraint = constraint_source).is_linear());

        EXPECT_EQ(sensitivity,
                  constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(constant - target, constraint.expression().constant_value());
        EXPECT_EQ(printemps::model::ConstraintSense::Greater,
                  constraint.sense());
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