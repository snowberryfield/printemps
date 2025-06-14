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
class TestConstraint : public ::testing::Test {
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
TEST_F(TestConstraint, constructor_arg_expression) {
    auto expression =
        model_component::Expression<int, double>::create_instance();
    auto variable = model_component::Variable<int, double>::create_instance();

    auto sensitivity = random_integer();
    auto constant    = random_integer();
    auto target      = random_integer();

    expression = sensitivity * variable + constant;

    /// Less
    {
        model_component::Constraint<int, double> constraint(expression <=
                                                            target);

        EXPECT_EQ(sensitivity,
                  constraint.expression().sensitivities().at(&variable));

        EXPECT_EQ(constant - target, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Less, constraint.sense());
        EXPECT_EQ(0, constraint.constraint_value());
        EXPECT_EQ(0, constraint.violation_value());
        EXPECT_EQ(0, constraint.margin_value());
        EXPECT_EQ(0, constraint.positive_part());
        EXPECT_EQ(0, constraint.negative_part());
        EXPECT_EQ(sensitivity, constraint.max_abs_coefficient());
        EXPECT_TRUE(constraint.is_integer());
        EXPECT_TRUE(constraint.is_enabled());
        EXPECT_TRUE(constraint.is_less_or_equal());
        EXPECT_FALSE(constraint.is_greater_or_equal());
    }

    /// Equal
    {
        model_component::Constraint<int, double> constraint(expression ==
                                                            target);

        EXPECT_EQ(sensitivity,
                  constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(constant - target, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Equal, constraint.sense());
        EXPECT_EQ(0, constraint.constraint_value());
        EXPECT_EQ(0, constraint.violation_value());
        EXPECT_EQ(0, constraint.margin_value());
        EXPECT_EQ(0, constraint.positive_part());
        EXPECT_EQ(0, constraint.negative_part());
        EXPECT_EQ(sensitivity, constraint.max_abs_coefficient());
        EXPECT_TRUE(constraint.is_integer());
        EXPECT_TRUE(constraint.is_enabled());
        EXPECT_TRUE(constraint.is_less_or_equal());
        EXPECT_TRUE(constraint.is_greater_or_equal());
    }

    /// Greater
    {
        model_component::Constraint<int, double> constraint(expression >=
                                                            target);

        EXPECT_EQ(sensitivity,
                  constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(constant - target, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Greater,
                  constraint.sense());
        EXPECT_EQ(0, constraint.constraint_value());
        EXPECT_EQ(0, constraint.violation_value());
        EXPECT_EQ(0, constraint.margin_value());
        EXPECT_EQ(0, constraint.positive_part());
        EXPECT_EQ(0, constraint.negative_part());
        EXPECT_EQ(sensitivity, constraint.max_abs_coefficient());
        EXPECT_TRUE(constraint.is_integer());
        EXPECT_TRUE(constraint.is_enabled());
        EXPECT_FALSE(constraint.is_less_or_equal());
        EXPECT_TRUE(constraint.is_greater_or_equal());
    }

    /// Not Integer(1)
    {
        model_component::Constraint<int, double> constraint(1.1 * expression >=
                                                            target);

        EXPECT_FALSE(constraint.is_integer());
    }

    /// Not Integer(2)
    {
        model_component::Constraint<int, double> constraint(expression >=
                                                            1.1 * target);

        EXPECT_FALSE(constraint.is_integer());
    }

    /// Binary coefficient / Binary variable
    {
        auto variable =
            model_component::Variable<int, double>::create_instance();
        variable.set_bound(0, 1);

        model_component::Constraint<int, double> constraint(variable >= 1);

        EXPECT_TRUE(constraint.has_only_binary_coefficient());
        EXPECT_TRUE(constraint.has_only_binary_variable());
    }

    /// Not binary coefficient / Binary variable
    {
        auto variable =
            model_component::Variable<int, double>::create_instance();
        variable.set_bound(0, 1);

        model_component::Constraint<int, double> constraint(2 * variable >= 1);

        EXPECT_FALSE(constraint.has_only_binary_coefficient());
        EXPECT_TRUE(constraint.has_only_binary_variable());
    }

    /// Binary coefficient / not binary variable
    {
        auto variable =
            model_component::Variable<int, double>::create_instance();
        variable.set_bound(0, 2);

        model_component::Constraint<int, double> constraint(variable >= 1);

        EXPECT_TRUE(constraint.has_only_binary_coefficient());
        EXPECT_FALSE(constraint.has_only_binary_variable());
    }

    /// Not binary coefficient / Not binary variable
    {
        auto variable =
            model_component::Variable<int, double>::create_instance();
        variable.set_bound(0, 2);

        model_component::Constraint<int, double> constraint(2 * variable >= 1);

        EXPECT_FALSE(constraint.has_only_binary_coefficient());
        EXPECT_FALSE(constraint.has_only_binary_variable());
    }
}

/*****************************************************************************/
TEST_F(TestConstraint, operator_equal_expression) {
    auto expression =
        model_component::Expression<int, double>::create_instance();
    auto variable = model_component::Variable<int, double>::create_instance();

    auto sensitivity = random_integer();
    auto constant    = random_integer();
    auto target      = random_integer();

    expression = sensitivity * variable + constant;

    /// Less
    {
        auto constraint_source =
            model_component::Constraint<int, double>::create_instance();
        constraint_source.setup(expression - target,
                                model_component::ConstraintSense::Less);
        auto constraint =
            model_component::Constraint<int, double>::create_instance();

        constraint = constraint_source;
        EXPECT_EQ(sensitivity,
                  constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(constant - target, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Less, constraint.sense());
        EXPECT_EQ(0, constraint.constraint_value());
        EXPECT_EQ(0, constraint.violation_value());

        auto value = random_integer();
        variable   = value;
        constraint.update();

        auto expected_value = sensitivity * value + constant - target;
        EXPECT_EQ(expected_value, constraint.evaluate_constraint());
        EXPECT_EQ(expected_value, constraint.constraint_value());
    }

    /// Equal
    {
        auto constraint_source =
            model_component::Constraint<int, double>::create_instance();
        constraint_source.setup(expression - target,
                                model_component::ConstraintSense::Equal);
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint = constraint_source;

        EXPECT_EQ(sensitivity,
                  constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(constant - target, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Equal, constraint.sense());
        EXPECT_EQ(0, constraint.constraint_value());
        EXPECT_EQ(0, constraint.violation_value());

        auto value = random_integer();
        variable   = value;
        constraint.update();

        auto expected_value = sensitivity * value + constant - target;
        EXPECT_EQ(expected_value, constraint.evaluate_constraint());
        EXPECT_EQ(expected_value, constraint.constraint_value());
    }

    /// Greater
    {
        auto constraint_source =
            model_component::Constraint<int, double>::create_instance();
        constraint_source.setup(expression - target,
                                model_component::ConstraintSense::Greater);
        auto constraint =
            model_component::Constraint<int, double>::create_instance();

        constraint = constraint_source;

        EXPECT_EQ(sensitivity,
                  constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(constant - target, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Greater,
                  constraint.sense());
        EXPECT_EQ(0, constraint.constraint_value());
        EXPECT_EQ(0, constraint.violation_value());

        auto value = random_integer();
        variable   = value;
        constraint.update();

        auto expected_value = sensitivity * value + constant - target;
        EXPECT_EQ(expected_value, constraint.evaluate_constraint());
        EXPECT_EQ(expected_value, constraint.constraint_value());
    }
}

/*****************************************************************************/
TEST_F(TestConstraint, initialize) {
    auto constraint =
        model_component::Constraint<int, double>::create_instance();

    /// Check the initial values of the base class members.
    EXPECT_EQ(0, constraint.proxy_index());
    EXPECT_EQ(0, constraint.flat_index());
    EXPECT_EQ(0, constraint.multi_dimensional_index()[0]);
    EXPECT_EQ("", constraint.name());

    /// Check the initial values of the derived class members.
    EXPECT_EQ(0, constraint.evaluate_constraint());
    EXPECT_EQ(0, constraint.evaluate_constraint({}));
    EXPECT_TRUE(constraint.expression().sensitivities().empty());
    EXPECT_EQ(0, constraint.expression().constant_value());
    EXPECT_EQ(model_component::ConstraintSense::Less, constraint.sense());
    EXPECT_EQ(0, constraint.constraint_value());
    EXPECT_EQ(0, constraint.violation_value());
    EXPECT_EQ(0, constraint.positive_part());
    EXPECT_EQ(0, constraint.negative_part());
    EXPECT_EQ(HUGE_VALF, constraint.local_penalty_coefficient_less());
    EXPECT_EQ(HUGE_VALF, constraint.local_penalty_coefficient_greater());
    EXPECT_EQ(HUGE_VALF, constraint.global_penalty_coefficient());

    EXPECT_EQ(nullptr, constraint.key_variable_ptr());
    EXPECT_EQ(0, constraint.violation_count());

    EXPECT_FALSE(constraint.is_user_defined_selection());

    EXPECT_FALSE(constraint.is_integer());
    EXPECT_TRUE(constraint.is_enabled());
    EXPECT_FALSE(constraint.is_less_or_equal());
    EXPECT_FALSE(constraint.is_greater_or_equal());

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
    EXPECT_FALSE(constraint.has_only_binary_coefficient());
}

/*****************************************************************************/
TEST_F(TestConstraint, clear_constraint_type) {
    /// This method is tested in other methods.
}

/****************************************************************************/
TEST_F(TestConstraint, setup_arg_expression) {
    auto expression =
        model_component::Expression<int, double>::create_instance();
    auto variable = model_component::Variable<int, double>::create_instance();

    auto sensitivity = random_integer();
    auto constant    = random_integer();
    auto target      = random_integer();

    expression = sensitivity * variable + constant;

    /// Less
    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(expression - target,
                         model_component::ConstraintSense::Less);

        EXPECT_EQ(sensitivity,
                  constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(constant - target, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Less, constraint.sense());
        EXPECT_EQ(0, constraint.constraint_value());
        EXPECT_EQ(0, constraint.violation_value());
        EXPECT_EQ(0, constraint.positive_part());
        EXPECT_EQ(0, constraint.negative_part());
        EXPECT_TRUE(constraint.is_integer());
        EXPECT_TRUE(constraint.is_enabled());
        EXPECT_TRUE(constraint.is_less_or_equal());
        EXPECT_FALSE(constraint.is_greater_or_equal());
    }

    /// Equal
    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(expression - target,
                         model_component::ConstraintSense::Equal);

        EXPECT_EQ(sensitivity,
                  constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(constant - target, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Equal, constraint.sense());
        EXPECT_EQ(0, constraint.constraint_value());
        EXPECT_EQ(0, constraint.violation_value());
        EXPECT_EQ(0, constraint.positive_part());
        EXPECT_EQ(0, constraint.negative_part());
        EXPECT_TRUE(constraint.is_integer());
        EXPECT_TRUE(constraint.is_enabled());
        EXPECT_TRUE(constraint.is_less_or_equal());
        EXPECT_TRUE(constraint.is_greater_or_equal());
    }

    /// Greater
    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(expression - target,
                         model_component::ConstraintSense::Greater);

        EXPECT_EQ(sensitivity,
                  constraint.expression().sensitivities().at(&variable));
        EXPECT_EQ(constant - target, constraint.expression().constant_value());
        EXPECT_EQ(model_component::ConstraintSense::Greater,
                  constraint.sense());
        EXPECT_EQ(0, constraint.constraint_value());
        EXPECT_EQ(0, constraint.violation_value());
        EXPECT_EQ(0, constraint.positive_part());
        EXPECT_EQ(0, constraint.negative_part());
        EXPECT_TRUE(constraint.is_integer());
        EXPECT_TRUE(constraint.is_enabled());
        EXPECT_FALSE(constraint.is_less_or_equal());
        EXPECT_TRUE(constraint.is_greater_or_equal());
    }

    /// Not Integer(1)
    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(1.1 * expression - target,
                         model_component::ConstraintSense::Less);

        EXPECT_FALSE(constraint.is_integer());
    }

    /// Not Integer(2)
    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(expression - 1.1 * target,
                         model_component::ConstraintSense::Less);

        EXPECT_FALSE(constraint.is_integer());
    }
}

/*****************************************************************************/
TEST_F(TestConstraint, update_constraint_type_singleton) {
    model::Model<int, double> model;
    auto&                     x = model.create_variable("x", -10, 10);
    auto                      constraint =
        model_component::Constraint<int, double>::create_instance();
    constraint.setup(2 * x - 10, model_component::ConstraintSense::Less);
    constraint.update_basic_structure();
    constraint.update_constraint_type();
    EXPECT_TRUE(constraint.is_singleton());
    EXPECT_EQ("Singleton", constraint.type());
    constraint.clear_constraint_type();
    EXPECT_FALSE(constraint.is_singleton());
}

/*****************************************************************************/
TEST_F(TestConstraint, update_constraint_type_exclusive_or) {
    model::Model<int, double> model;
    auto&                     x = model.create_variables("x", 2, 0, 1);

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(x.sum() - 1, model_component::ConstraintSense::Equal);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_TRUE(constraint.is_exclusive_or());
        EXPECT_EQ("Exclusive OR", constraint.type());
        constraint.clear_constraint_type();
        EXPECT_FALSE(constraint.is_exclusive_or());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-x.sum() + 1, model_component::ConstraintSense::Equal);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_TRUE(constraint.is_exclusive_or());
        EXPECT_EQ("Exclusive OR", constraint.type());
        constraint.clear_constraint_type();
        EXPECT_FALSE(constraint.is_exclusive_or());
    }
}

/*****************************************************************************/
TEST_F(TestConstraint, update_constraint_type_exclusive_nor) {
    model::Model<int, double> model;
    auto&                     x = model.create_variables("x", 2, 0, 1);
    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(x(0) - x(1), model_component::ConstraintSense::Equal);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_TRUE(constraint.is_exclusive_nor());
        EXPECT_EQ("Exclusive NOR", constraint.type());
        constraint.clear_constraint_type();
        EXPECT_FALSE(constraint.is_exclusive_nor());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-x(0) + x(1), model_component::ConstraintSense::Equal);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_TRUE(constraint.is_exclusive_nor());
        EXPECT_EQ("Exclusive NOR", constraint.type());
        constraint.clear_constraint_type();
        EXPECT_FALSE(constraint.is_exclusive_nor());
    }
}

/*****************************************************************************/
TEST_F(TestConstraint, update_constraint_type_inverted_integers) {
    model::Model<int, double> model;
    auto&                     x = model.create_variables("x", 2, 0, 10);

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(x(0) + x(1), model_component::ConstraintSense::Equal);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_TRUE(constraint.is_inverted_integers());
        EXPECT_EQ("Inverted Integers", constraint.type());
        constraint.clear_constraint_type();
        EXPECT_FALSE(constraint.is_inverted_integers());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-x(0) - x(1), model_component::ConstraintSense::Equal);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_TRUE(constraint.is_inverted_integers());
        EXPECT_EQ("Inverted Integers", constraint.type());
        constraint.clear_constraint_type();
        EXPECT_FALSE(constraint.is_inverted_integers());
    }
}

/*****************************************************************************/
TEST_F(TestConstraint, update_constraint_type_balanced_integers) {
    model::Model<int, double> model;
    auto&                     x = model.create_variables("x", 2, 0, 10);

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(x(0) - x(1), model_component::ConstraintSense::Equal);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_TRUE(constraint.is_balanced_integers());
        EXPECT_EQ("Balanced Integers", constraint.type());
        constraint.clear_constraint_type();
        EXPECT_FALSE(constraint.is_balanced_integers());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-x(0) + x(1), model_component::ConstraintSense::Equal);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_TRUE(constraint.is_balanced_integers());
        EXPECT_EQ("Balanced Integers", constraint.type());
        constraint.clear_constraint_type();
        EXPECT_FALSE(constraint.is_balanced_integers());
    }
}

/*****************************************************************************/
TEST_F(TestConstraint, update_constraint_type_constant_sum_integers) {
    model::Model<int, double> model;
    auto&                     x = model.create_variables("x", 2, 0, 10);
    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(x(0) + x(1) - 1,
                         model_component::ConstraintSense::Equal);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_TRUE(constraint.is_constant_sum_integers());
        EXPECT_EQ("Constant Sum Integers", constraint.type());
        constraint.clear_constraint_type();
        EXPECT_FALSE(constraint.is_constant_sum_integers());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-x(0) - x(1) + 1,
                         model_component::ConstraintSense::Equal);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_TRUE(constraint.is_constant_sum_integers());
        EXPECT_EQ("Constant Sum Integers", constraint.type());
        constraint.clear_constraint_type();
        EXPECT_FALSE(constraint.is_constant_sum_integers());
    }
}

/*****************************************************************************/
TEST_F(TestConstraint, update_constraint_type_constant_difference_integers) {
    model::Model<int, double> model;
    auto&                     x = model.create_variables("x", 2, 0, 10);
    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(x(0) - x(1) - 1,
                         model_component::ConstraintSense::Equal);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_TRUE(constraint.is_constant_difference_integers());
        EXPECT_EQ("Constant Difference Integers", constraint.type());
        constraint.clear_constraint_type();
        EXPECT_FALSE(constraint.is_constant_difference_integers());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-x(0) + x(1) + 1,
                         model_component::ConstraintSense::Equal);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_TRUE(constraint.is_constant_difference_integers());
        EXPECT_EQ("Constant Difference Integers", constraint.type());
        constraint.clear_constraint_type();
        EXPECT_FALSE(constraint.is_constant_difference_integers());
    }
}

/*****************************************************************************/
TEST_F(TestConstraint, update_constraint_type_constant_ratio_integers) {
    model::Model<int, double> model;
    auto&                     x = model.create_variables("x", 2, 0, 10);
    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(2 * x(0) - x(1),
                         model_component::ConstraintSense::Equal);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_TRUE(constraint.is_constant_ratio_integers());
        EXPECT_EQ("Constant Ratio Integers", constraint.type());
        constraint.clear_constraint_type();
        EXPECT_FALSE(constraint.is_constant_ratio_integers());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(x(0) + 2 * x(1),
                         model_component::ConstraintSense::Equal);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_TRUE(constraint.is_constant_ratio_integers());
        EXPECT_EQ("Constant Ratio Integers", constraint.type());
        constraint.clear_constraint_type();
        EXPECT_FALSE(constraint.is_constant_ratio_integers());
    }
}

/*****************************************************************************/
TEST_F(TestConstraint, update_constraint_type_aggregation) {
    model::Model<int, double> model;
    auto&                     x = model.create_variable("x", -10, 10);
    auto&                     y = model.create_variable("y", -10, 10);
    auto                      constraint =
        model_component::Constraint<int, double>::create_instance();
    constraint.setup(2 * x + 3 * y - 10,
                     model_component::ConstraintSense::Equal);
    constraint.update_basic_structure();
    constraint.update_constraint_type();
    EXPECT_TRUE(constraint.is_aggregation());
    EXPECT_EQ("Aggregation", constraint.type());
    constraint.clear_constraint_type();
    EXPECT_FALSE(constraint.is_aggregation());
}

/*****************************************************************************/
TEST_F(TestConstraint, update_constraint_type_precedence) {
    model::Model<int, double> model;
    auto&                     x = model.create_variable("x", -10, 10);
    auto&                     y = model.create_variable("y", -10, 10);
    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(2 * x - 2 * y - 5,
                         model_component::ConstraintSense::Less);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_TRUE(constraint.is_precedence());
        EXPECT_EQ("Precedence", constraint.type());
        constraint.clear_constraint_type();
        EXPECT_FALSE(constraint.is_precedence());
    }
    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-2 * x + 2 * y - 5,
                         model_component::ConstraintSense::Less);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_TRUE(constraint.is_precedence());
        EXPECT_EQ("Precedence", constraint.type());
        constraint.clear_constraint_type();
        EXPECT_FALSE(constraint.is_precedence());
    }
    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(2 * x - 2 * y - 5,
                         model_component::ConstraintSense::Greater);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_TRUE(constraint.is_precedence());
        EXPECT_EQ("Precedence", constraint.type());
        constraint.clear_constraint_type();
        EXPECT_FALSE(constraint.is_precedence());
    }
    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-2 * x + 2 * y - 5,
                         model_component::ConstraintSense::Greater);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_TRUE(constraint.is_precedence());
        EXPECT_EQ("Precedence", constraint.type());
        constraint.clear_constraint_type();
        EXPECT_FALSE(constraint.is_precedence());
    }
}

/*****************************************************************************/
TEST_F(TestConstraint, update_constraint_type_variable_bound) {
    model::Model<int, double> model;

    auto& x = model.create_variable("x", 0, 1);
    auto& y = model.create_variable("y", 0, 1);

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(2 * x + 3 * y - 5,
                         model_component::ConstraintSense::Less);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_TRUE(constraint.is_variable_bound());
        EXPECT_EQ("Variable Bound", constraint.type());
        constraint.clear_constraint_type();
        EXPECT_FALSE(constraint.is_variable_bound());
    }
    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(2 * x + 3 * y - 5,
                         model_component::ConstraintSense::Greater);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_TRUE(constraint.is_variable_bound());
        EXPECT_EQ("Variable Bound", constraint.type());
        constraint.clear_constraint_type();
        EXPECT_FALSE(constraint.is_variable_bound());
    }
}

/*****************************************************************************/
TEST_F(TestConstraint, update_constraint_type_trinomial_exclusive_nor) {
    model::Model<int, double> model;

    auto& x = model.create_variable("x", 0, 1);
    auto& y = model.create_variable("y", 0, 1);
    auto& z = model.create_variable("z", 0, 1);

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(x + y - 2 * z,
                         model_component::ConstraintSense::Equal);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_TRUE(constraint.is_trinomial_exclusive_nor());
        EXPECT_EQ("Trinomial Exclusive NOR", constraint.type());
        constraint.clear_constraint_type();
        EXPECT_FALSE(constraint.is_trinomial_exclusive_nor());
    }
    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-x - y + 2 * z,
                         model_component::ConstraintSense::Equal);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_TRUE(constraint.is_trinomial_exclusive_nor());
        EXPECT_EQ("Trinomial Exclusive NOR", constraint.type());
        constraint.clear_constraint_type();
        EXPECT_FALSE(constraint.is_trinomial_exclusive_nor());
    }
}

/*****************************************************************************/
TEST_F(TestConstraint, update_constraint_type_set_partitioning) {
    model::Model<int, double> model;
    {
        auto& x = model.create_variables("x", 10, 0, 1);
        auto  constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(x.sum() - 1, model_component::ConstraintSense::Equal);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_TRUE(constraint.is_set_partitioning());
        EXPECT_EQ("Set Partitioning", constraint.type());
        constraint.clear_constraint_type();
        EXPECT_FALSE(constraint.is_set_partitioning());
    }
}

/*****************************************************************************/
TEST_F(TestConstraint, update_constraint_type_set_packing) {
    model::Model<int, double> model;
    auto&                     x = model.create_variables("x", 10, 0, 1);
    auto                      constraint =
        model_component::Constraint<int, double>::create_instance();
    constraint.setup(x.sum() - 1, model_component::ConstraintSense::Less);
    constraint.update_basic_structure();
    constraint.update_constraint_type();
    EXPECT_TRUE(constraint.is_set_packing());
    EXPECT_EQ("Set Packing", constraint.type());
    constraint.clear_constraint_type();
    EXPECT_FALSE(constraint.is_set_packing());
}

/*****************************************************************************/
TEST_F(TestConstraint, update_constraint_type_set_covering) {
    model::Model<int, double> model;
    auto&                     x = model.create_variables("x", 10, 0, 1);
    auto                      constraint =
        model_component::Constraint<int, double>::create_instance();
    constraint.setup(x.sum() - 1, model_component::ConstraintSense::Greater);
    constraint.update_basic_structure();
    constraint.update_constraint_type();
    EXPECT_TRUE(constraint.is_set_covering());
    EXPECT_EQ("Set Covering", constraint.type());
}

/*****************************************************************************/
TEST_F(TestConstraint, update_constraint_type_cardinality) {
    model::Model<int, double> model;
    auto&                     x = model.create_variables("x", 10, 0, 1);
    auto                      constraint =
        model_component::Constraint<int, double>::create_instance();
    constraint.setup(x.sum() - 5, model_component::ConstraintSense::Equal);
    constraint.update_basic_structure();
    constraint.update_constraint_type();
    EXPECT_TRUE(constraint.is_cardinality());
    EXPECT_EQ("Cardinality", constraint.type());
    constraint.clear_constraint_type();
    EXPECT_FALSE(constraint.is_cardinality());
}

/*****************************************************************************/
TEST_F(TestConstraint, update_constraint_type_invariant_knapsack) {
    model::Model<int, double> model;
    auto&                     x = model.create_variables("x", 10, 0, 1);
    auto                      constraint =
        model_component::Constraint<int, double>::create_instance();
    constraint.setup(x.sum() - 5, model_component::ConstraintSense::Less);
    constraint.update_basic_structure();
    constraint.update_constraint_type();
    EXPECT_TRUE(constraint.is_invariant_knapsack());
    EXPECT_EQ("Invariant Knapsack", constraint.type());
    constraint.clear_constraint_type();
    EXPECT_FALSE(constraint.is_invariant_knapsack());
}

/*****************************************************************************/
TEST_F(TestConstraint, update_constraint_type_multiple_covering) {
    model::Model<int, double> model;
    auto&                     x = model.create_variables("x", 10, 0, 1);
    auto                      constraint =
        model_component::Constraint<int, double>::create_instance();
    constraint.setup(x.sum() - 5, model_component::ConstraintSense::Greater);
    constraint.update_basic_structure();
    constraint.update_constraint_type();
    EXPECT_TRUE(constraint.is_multiple_covering());
    EXPECT_EQ("Multiple Covering", constraint.type());
    constraint.clear_constraint_type();
    EXPECT_FALSE(constraint.is_multiple_covering());
}

/*****************************************************************************/
TEST_F(TestConstraint, update_constraint_type_binary_flow) {
    model::Model<int, double> model;
    auto&                     x = model.create_variables("x", 10, 0, 1);
    auto                      constraint =
        model_component::Constraint<int, double>::create_instance();
    constraint.setup(x(0) + x(1) + x(2) - x(3) - x(4) - x(5),
                     model_component::ConstraintSense::Equal);
    constraint.update_basic_structure();
    constraint.update_constraint_type();
    EXPECT_TRUE(constraint.is_binary_flow());
    EXPECT_EQ("Binary Flow", constraint.type());
    constraint.clear_constraint_type();
    EXPECT_FALSE(constraint.is_binary_flow());
}

/*****************************************************************************/
TEST_F(TestConstraint, update_constraint_type_integer_flow) {
    model::Model<int, double> model;
    auto&                     x = model.create_variables("x", 10, 0, 10);
    auto                      constraint =
        model_component::Constraint<int, double>::create_instance();
    constraint.setup(x(0) + x(1) + x(2) - x(3) - x(4) - x(5),
                     model_component::ConstraintSense::Equal);
    constraint.update_basic_structure();
    constraint.update_constraint_type();
    EXPECT_TRUE(constraint.is_integer_flow());
    EXPECT_EQ("Integer Flow", constraint.type());
    constraint.clear_constraint_type();
    EXPECT_FALSE(constraint.is_integer_flow());
}

/*****************************************************************************/
TEST_F(TestConstraint, update_constraint_type_soft_selection) {
    model::Model<int, double> model;
    auto&                     x = model.create_variables("x", 10, 0, 1);
    auto                      constraint =
        model_component::Constraint<int, double>::create_instance();
    constraint.setup(x(0) + x(1) + x(2) + x(3) + x(4) - x(5),
                     model_component::ConstraintSense::Equal);
    constraint.update_basic_structure();
    constraint.update_constraint_type();
    EXPECT_TRUE(constraint.is_soft_selection());
    EXPECT_EQ("Soft Selection", constraint.type());
}

/*****************************************************************************/
TEST_F(TestConstraint, update_constraint_type_min_max) {
    model::Model<int, double> model;

    auto& x = model.create_variable("x", -10, 10);
    auto& y = model.create_variable("y", 0, 1);
    auto& z = model.create_variable("z", 0, 2);
    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-x + y + 2 * z + 5,
                         model_component::ConstraintSense::Less);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_TRUE(constraint.is_min_max());
        EXPECT_EQ("Min-Max", constraint.type());
        constraint.clear_constraint_type();
        EXPECT_FALSE(constraint.is_min_max());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(x - y - 2 * z - 5,
                         model_component::ConstraintSense::Greater);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_TRUE(constraint.is_min_max());
        EXPECT_EQ("Min-Max", constraint.type());
        constraint.clear_constraint_type();
        EXPECT_FALSE(constraint.is_min_max());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-x + 20 * y + 20 * z,
                         model_component::ConstraintSense::Less);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_TRUE(constraint.is_min_max());
        EXPECT_EQ("Min-Max", constraint.type());
        constraint.clear_constraint_type();
        EXPECT_FALSE(constraint.is_min_max());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-x - 20 * y - 20 * z,
                         model_component::ConstraintSense::Less);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_TRUE(constraint.is_min_max());
        EXPECT_EQ("Min-Max", constraint.type());
        constraint.clear_constraint_type();
        EXPECT_FALSE(constraint.is_min_max());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-x - 20 * y + 20 * z,
                         model_component::ConstraintSense::Less);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_TRUE(constraint.is_min_max());
        EXPECT_EQ("Min-Max", constraint.type());
        constraint.clear_constraint_type();
        EXPECT_FALSE(constraint.is_min_max());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(x + 20 * y - 20 * z,
                         model_component::ConstraintSense::Greater);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_TRUE(constraint.is_min_max());
        EXPECT_EQ("Min-Max", constraint.type());
        constraint.clear_constraint_type();
        EXPECT_FALSE(constraint.is_min_max());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-x + y + z + 5,
                         model_component::ConstraintSense::Less);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_FALSE(constraint.is_min_max());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-x + 1.5 * y + 2 * z + 5,
                         model_component::ConstraintSense::Less);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_FALSE(constraint.is_min_max());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-x + y + 2 * z + 5.5,
                         model_component::ConstraintSense::Less);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_FALSE(constraint.is_min_max());
    }
}

/*****************************************************************************/
TEST_F(TestConstraint, update_constraint_type_max_min) {
    model::Model<int, double> model;

    auto& x = model.create_variable("x", -10, 10);
    auto& y = model.create_variable("y", 0, 1);
    auto& z = model.create_variable("z", 0, 2);
    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-x + y + 2 * z - 5,
                         model_component::ConstraintSense::Greater);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_TRUE(constraint.is_max_min());
        EXPECT_EQ("Max-Min", constraint.type());
        constraint.clear_constraint_type();
        EXPECT_FALSE(constraint.is_max_min());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(x - y - 2 * z + 5,
                         model_component::ConstraintSense::Less);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_TRUE(constraint.is_max_min());
        EXPECT_EQ("Max-Min", constraint.type());
        constraint.clear_constraint_type();
        EXPECT_FALSE(constraint.is_max_min());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-x + 20 * y + 20 * z,
                         model_component::ConstraintSense::Greater);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_TRUE(constraint.is_max_min());
        EXPECT_EQ("Max-Min", constraint.type());
        constraint.clear_constraint_type();
        EXPECT_FALSE(constraint.is_max_min());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-x - 20 * y - 20 * z,
                         model_component::ConstraintSense::Greater);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_TRUE(constraint.is_max_min());
        EXPECT_EQ("Max-Min", constraint.type());
        constraint.clear_constraint_type();
        EXPECT_FALSE(constraint.is_max_min());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-x - 20 * y + 20 * z,
                         model_component::ConstraintSense::Greater);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_TRUE(constraint.is_max_min());
        EXPECT_EQ("Max-Min", constraint.type());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(x + 20 * y - 20 * z,
                         model_component::ConstraintSense::Less);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_TRUE(constraint.is_max_min());
        EXPECT_EQ("Max-Min", constraint.type());
        constraint.clear_constraint_type();
        EXPECT_FALSE(constraint.is_max_min());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-x + y + z - 5,
                         model_component::ConstraintSense::Greater);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_FALSE(constraint.is_max_min());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-x + 1.5 * y + 2 * z - 5,
                         model_component::ConstraintSense::Greater);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_FALSE(constraint.is_max_min());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-x + y + 2 * z - 5.5,
                         model_component::ConstraintSense::Greater);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_FALSE(constraint.is_max_min());
    }
}

/*****************************************************************************/
TEST_F(TestConstraint, update_constraint_type_intermediate) {
    model::Model<int, double> model;

    auto& x = model.create_variable("x", -10, 10);
    auto& y = model.create_variable("y", 0, 1);
    auto& z = model.create_variable("z", 0, 2);

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-x + y + 2 * z + 5,
                         model_component::ConstraintSense::Equal);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_TRUE(constraint.is_intermediate());
        EXPECT_EQ(&x(0), constraint.key_variable_ptr());
        EXPECT_EQ("Intermediate", constraint.type());
        constraint.clear_constraint_type();
        EXPECT_FALSE(constraint.is_intermediate());
        EXPECT_EQ(nullptr, constraint.key_variable_ptr());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(x + y + 2 * z + 5,
                         model_component::ConstraintSense::Equal);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_TRUE(constraint.is_intermediate());
        EXPECT_EQ(&x(0), constraint.key_variable_ptr());
        EXPECT_EQ("Intermediate", constraint.type());
        constraint.clear_constraint_type();
        EXPECT_FALSE(constraint.is_intermediate());
        EXPECT_EQ(nullptr, constraint.key_variable_ptr());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(x + 20 * y + 20 * z,
                         model_component::ConstraintSense::Equal);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_TRUE(constraint.is_intermediate());
        EXPECT_EQ(&x(0), constraint.key_variable_ptr());
        EXPECT_EQ("Intermediate", constraint.type());
        constraint.clear_constraint_type();
        EXPECT_FALSE(constraint.is_intermediate());
        EXPECT_EQ(nullptr, constraint.key_variable_ptr());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(x + 20 * y - 20 * z,
                         model_component::ConstraintSense::Equal);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_TRUE(constraint.is_intermediate());
        EXPECT_EQ(&x(0), constraint.key_variable_ptr());
        EXPECT_EQ("Intermediate", constraint.type());
        constraint.clear_constraint_type();
        EXPECT_FALSE(constraint.is_intermediate());
        EXPECT_EQ(nullptr, constraint.key_variable_ptr());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-x + y + z + 5,
                         model_component::ConstraintSense::Equal);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_FALSE(constraint.is_intermediate());
        EXPECT_EQ(nullptr, constraint.key_variable_ptr());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-x + 1.5 * y + 2 * z + 5,
                         model_component::ConstraintSense::Equal);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_FALSE(constraint.is_intermediate());
        EXPECT_EQ(nullptr, constraint.key_variable_ptr());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-x + y + 2 * z + 5.5,
                         model_component::ConstraintSense::Equal);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_FALSE(constraint.is_intermediate());
        EXPECT_EQ(nullptr, constraint.key_variable_ptr());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-2 * x + y + 2 * z + 5,
                         model_component::ConstraintSense::Equal);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_FALSE(constraint.is_intermediate());
        EXPECT_EQ(nullptr, constraint.key_variable_ptr());
    }
}

/*****************************************************************************/
TEST_F(TestConstraint, update_constraint_type_equation_knapsack) {
    model::Model<int, double> model;
    auto                      coefficients = utility::sequence(10);

    auto& x = model.create_variables("x", 10, 0, 1);
    auto  constraint =
        model_component::Constraint<int, double>::create_instance();
    constraint.setup(x.dot(coefficients) - 30,
                     model_component::ConstraintSense::Equal);
    constraint.update_basic_structure();
    constraint.update_constraint_type();
    EXPECT_TRUE(constraint.is_equation_knapsack());
    EXPECT_EQ("Equation Knapsack", constraint.type());
    constraint.clear_constraint_type();
    EXPECT_FALSE(constraint.is_equation_knapsack());
}

/*****************************************************************************/
TEST_F(TestConstraint, update_constraint_type_bin_packing) {
    model::Model<int, double> model;
    auto                      coefficients = utility::sequence(10);

    auto& x = model.create_variables("x", 10, 0, 1);
    auto& y = model.create_variable("y", 0, 1);
    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(x.dot(coefficients) + 5 * y - 5,
                         model_component::ConstraintSense::Less);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_TRUE(constraint.is_bin_packing());
        EXPECT_EQ("Bin Packing", constraint.type());
        constraint.clear_constraint_type();
        EXPECT_FALSE(constraint.is_bin_packing());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-x.dot(coefficients) - 5 * y + 5,
                         model_component::ConstraintSense::Greater);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_TRUE(constraint.is_bin_packing());
        EXPECT_EQ("Bin Packing", constraint.type());
        constraint.clear_constraint_type();
        EXPECT_FALSE(constraint.is_bin_packing());
    }
}

/*****************************************************************************/
TEST_F(TestConstraint, update_constraint_type_knapsack) {
    model::Model<int, double> model;
    auto                      coefficients = utility::sequence(10);

    auto& x = model.create_variables("x", 10, 0, 1);
    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(x.dot(coefficients) - 50,
                         model_component::ConstraintSense::Less);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_TRUE(constraint.is_knapsack());
        EXPECT_EQ("Knapsack", constraint.type());
        constraint.clear_constraint_type();
        EXPECT_FALSE(constraint.is_knapsack());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-x.dot(coefficients) + 50,
                         model_component::ConstraintSense::Greater);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_TRUE(constraint.is_knapsack());
        EXPECT_EQ("Knapsack", constraint.type());
        constraint.clear_constraint_type();
        EXPECT_FALSE(constraint.is_knapsack());
    }
}

/*****************************************************************************/
TEST_F(TestConstraint, update_constraint_type_integer_knapsack) {
    model::Model<int, double> model;
    auto                      coefficients = utility::sequence(10, 20);

    auto& x = model.create_variables("x", 10, 0, 10);
    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(x.dot(coefficients) - 50,
                         model_component::ConstraintSense::Less);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_TRUE(constraint.is_integer_knapsack());
        EXPECT_EQ("Integer Knapsack", constraint.type());
        constraint.clear_constraint_type();
        EXPECT_FALSE(constraint.is_integer_knapsack());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-x.dot(coefficients) + 50,
                         model_component::ConstraintSense::Greater);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_TRUE(constraint.is_integer_knapsack());
        EXPECT_EQ("Integer Knapsack", constraint.type());
        constraint.clear_constraint_type();
        EXPECT_FALSE(constraint.is_integer_knapsack());
    }
}

/*****************************************************************************/
TEST_F(TestConstraint, update_constraint_type_gf2) {
    model::Model<int, double> model;

    auto& x = model.create_variables("x", 10, 0, 1);
    auto& y = model.create_variable("y", 0, 5);

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(x.sum() - 2 * y,
                         model_component::ConstraintSense::Equal);

        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_TRUE(constraint.is_gf2());
        EXPECT_EQ(&y(0), constraint.key_variable_ptr());
        EXPECT_EQ("GF(2)", constraint.type());
        constraint.clear_constraint_type();
        EXPECT_FALSE(constraint.is_gf2());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-x.sum() + 2 * y,
                         model_component::ConstraintSense::Equal);

        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_TRUE(constraint.is_gf2());
        EXPECT_EQ(&y(0), constraint.key_variable_ptr());
        EXPECT_EQ("GF(2)", constraint.type());
        constraint.clear_constraint_type();
        EXPECT_FALSE(constraint.is_gf2());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(x.sum() + 1 - 2 * y,
                         model_component::ConstraintSense::Equal);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_TRUE(constraint.is_gf2());
        EXPECT_EQ(&y(0), constraint.key_variable_ptr());
        EXPECT_EQ("GF(2)", constraint.type());
        constraint.clear_constraint_type();
        EXPECT_FALSE(constraint.is_gf2());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-x.sum() - 1 + 2 * y,
                         model_component::ConstraintSense::Equal);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_TRUE(constraint.is_gf2());
        EXPECT_EQ(&y(0), constraint.key_variable_ptr());
        EXPECT_EQ("GF(2)", constraint.type());
        constraint.clear_constraint_type();
        EXPECT_FALSE(constraint.is_gf2());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(x.sum() - 1 - 2 * y,
                         model_component::ConstraintSense::Equal);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_TRUE(constraint.is_gf2());
        EXPECT_EQ(&y(0), constraint.key_variable_ptr());
        EXPECT_EQ("GF(2)", constraint.type());
        constraint.clear_constraint_type();
        EXPECT_FALSE(constraint.is_gf2());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-x.sum() + 1 + 2 * y,
                         model_component::ConstraintSense::Equal);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_TRUE(constraint.is_gf2());
        EXPECT_EQ(&y(0), constraint.key_variable_ptr());
        EXPECT_EQ("GF(2)", constraint.type());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(x.sum() + 2 - 2 * y,
                         model_component::ConstraintSense::Equal);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_FALSE(constraint.is_gf2());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-x.sum() - 2 + 2 * y,
                         model_component::ConstraintSense::Equal);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_FALSE(constraint.is_gf2());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(x.sum() - 2 - 2 * y,
                         model_component::ConstraintSense::Equal);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_FALSE(constraint.is_gf2());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-x.sum() + 2 + 2 * y,
                         model_component::ConstraintSense::Equal);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_FALSE(constraint.is_gf2());
    }
}

/*****************************************************************************/
TEST_F(TestConstraint, update_constraint_type_general_linear) {
    model::Model<int, double> model;
    auto                      coefficients = utility::sequence(10);

    auto& x = model.create_variable("x", 0, 1);
    auto& y = model.create_variables("y", 10, -10, 10);

    auto constraint =
        model_component::Constraint<int, double>::create_instance();
    constraint.setup(2 * x + 2 * y.sum() - 50,
                     model_component::ConstraintSense::Equal);
    constraint.update_basic_structure();
    constraint.update_constraint_type();
    EXPECT_TRUE(constraint.is_general_linear());
    EXPECT_EQ("General Linear", constraint.type());
    constraint.clear_constraint_type();
    EXPECT_FALSE(constraint.is_general_linear());
}

/*****************************************************************************/
TEST_F(TestConstraint, evaluate_expression_arg_void) {
    auto expression =
        model_component::Expression<int, double>::create_instance();
    auto variable = model_component::Variable<int, double>::create_instance();

    auto sensitivity = random_integer();
    auto constant    = random_integer();
    auto target      = random_integer();

    expression = sensitivity * variable + constant;

    auto constraint =
        model_component::Constraint<int, double>::create_instance();
    constraint.setup(expression - target,
                     model_component::ConstraintSense::Less);
    constraint.expression().setup_fixed_sensitivities();

    auto value = random_integer();
    variable   = value;
    constraint.update();

    auto expected_value = sensitivity * value + constant - target;
    EXPECT_EQ(expected_value, constraint.evaluate_constraint());
    EXPECT_EQ(expected_value, constraint.constraint_value());

    EXPECT_EQ(std::max(expected_value, 0), constraint.positive_part());
    EXPECT_EQ(-std::min(expected_value, 0), constraint.negative_part());
}

/*****************************************************************************/
TEST_F(TestConstraint, evaluate_expression_arg_move) {
    auto expression =
        model_component::Expression<int, double>::create_instance();
    auto variable = model_component::Variable<int, double>::create_instance();

    auto sensitivity = random_integer();
    auto constant    = random_integer();
    auto target      = random_integer();

    expression = sensitivity * variable + constant;

    auto constraint =
        model_component::Constraint<int, double>::create_instance();
    constraint.setup(expression - target,
                     model_component::ConstraintSense::Less);
    constraint.expression().setup_fixed_sensitivities();
    /// initial
    {
        auto value = random_integer();
        variable   = value;
        constraint.update();

        auto expected_value = sensitivity * value + constant - target;
        EXPECT_EQ(expected_value, constraint.evaluate_constraint());
        EXPECT_EQ(expected_value, constraint.constraint_value());

        EXPECT_EQ(std::max(expected_value, 0), constraint.positive_part());
        EXPECT_EQ(-std::min(expected_value, 0), constraint.negative_part());
    }

    /// after move
    {
        auto value = random_integer();

        neighborhood::Move<int, double> move;
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
TEST_F(TestConstraint, update_arg_void) {
    /// This method is tested in evaluate_expression_arg_void().
}

/*****************************************************************************/
TEST_F(TestConstraint, update_arg_move) {
    /// This method is tested in evaluate_expression_arg_move().
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
TEST_F(TestConstraint, margin_value) {
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
TEST_F(TestConstraint, max_abs_coefficient) {
    /// This method is tested in tested in other cases.
}

/*****************************************************************************/
TEST_F(TestConstraint, local_penalty_coefficient_less) {
    auto constraint =
        model_component::Constraint<int, double>::create_instance();
    EXPECT_EQ(HUGE_VAL, constraint.local_penalty_coefficient_less());
    constraint.local_penalty_coefficient_less() = 10.0;
    EXPECT_EQ(10.0, constraint.local_penalty_coefficient_less());
}

/*****************************************************************************/
TEST_F(TestConstraint, local_penalty_coefficient_greater) {
    auto constraint =
        model_component::Constraint<int, double>::create_instance();
    EXPECT_EQ(HUGE_VAL, constraint.local_penalty_coefficient_greater());
    constraint.local_penalty_coefficient_greater() = 10.0;
    EXPECT_EQ(10.0, constraint.local_penalty_coefficient_greater());
}

/*****************************************************************************/
TEST_F(TestConstraint, global_penalty_coefficient) {
    auto constraint =
        model_component::Constraint<int, double>::create_instance();
    EXPECT_EQ(HUGE_VAL, constraint.global_penalty_coefficient());
    constraint.global_penalty_coefficient() = 100.0;
    EXPECT_EQ(100.0, constraint.global_penalty_coefficient());
}

/*****************************************************************************/
TEST_F(TestConstraint, reset_local_penalty_coefficient) {
    auto constraint =
        model_component::Constraint<int, double>::create_instance();
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
TEST_F(TestConstraint, key_variable_ptr) {
    /// This method is tested in update_constraint_type_intermediate().
}

/*****************************************************************************/
TEST_F(TestConstraint, increment_violation_count) {
    auto constraint =
        model_component::Constraint<int, double>::create_instance();

    EXPECT_EQ(0, constraint.violation_count());
    constraint.increment_violation_count();
    EXPECT_EQ(1, constraint.violation_count());
    constraint.increment_violation_count();
    EXPECT_EQ(2, constraint.violation_count());
    constraint.reset_violation_count();
    EXPECT_EQ(0, constraint.violation_count());
}

/*****************************************************************************/
TEST_F(TestConstraint, reset_violation_count) {
    /// This method is tested in increment_violation_count().
}

/*****************************************************************************/
TEST_F(TestConstraint, violation_count) {
    /// This method is tested in increment_violation_count().
}

/*****************************************************************************/
TEST_F(TestConstraint, set_is_user_defined_selection) {
    auto constraint =
        model_component::Constraint<int, double>::create_instance();
    constraint.set_is_user_defined_selection(true);
    EXPECT_TRUE(constraint.is_user_defined_selection());
    constraint.set_is_user_defined_selection(false);
    EXPECT_FALSE(constraint.is_user_defined_selection());
}

/*****************************************************************************/
TEST_F(TestConstraint, is_user_defined_selection) {
    /// This method is tested in set_is_user_defined_selection().
}

/*****************************************************************************/
TEST_F(TestConstraint, has_only_binary_coefficient) {
    /// This method is tested in other methods.
}

/*****************************************************************************/
TEST_F(TestConstraint, has_only_binary_variable) {
    /// This method is tested in other methods.
}

/*****************************************************************************/
TEST_F(TestConstraint, is_evaluation_ignorable) {
    model::Model<int, double> model;

    auto& x = model.create_variable("x", 0, 1);
    auto& y = model.create_variable("y", 0, 1);

    /// Less
    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(x + y - 1, model_component::ConstraintSense::Less);
        x = 0;
        y = 0;
        constraint.update();
        EXPECT_TRUE(constraint.is_evaluation_ignorable());

        x = 1;
        y = 0;
        constraint.update();
        EXPECT_FALSE(constraint.is_evaluation_ignorable());

        x = 0;
        y = 1;
        constraint.update();
        EXPECT_FALSE(constraint.is_evaluation_ignorable());

        x = 1;
        y = 1;
        constraint.update();
        EXPECT_FALSE(constraint.is_evaluation_ignorable());
    }

    /// Equal
    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(x + y - 1, model_component::ConstraintSense::Equal);
        x = 0;
        y = 0;
        constraint.update();
        EXPECT_FALSE(constraint.is_evaluation_ignorable());

        x = 1;
        y = 0;
        constraint.update();
        EXPECT_FALSE(constraint.is_evaluation_ignorable());

        x = 0;
        y = 1;
        constraint.update();
        EXPECT_FALSE(constraint.is_evaluation_ignorable());

        x = 1;
        y = 1;
        constraint.update();
        EXPECT_FALSE(constraint.is_evaluation_ignorable());
    }

    /// Greater
    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(x + y - 1, model_component::ConstraintSense::Greater);
        x = 0;
        y = 0;
        constraint.update();
        EXPECT_FALSE(constraint.is_evaluation_ignorable());

        x = 1;
        y = 0;
        constraint.update();
        EXPECT_FALSE(constraint.is_evaluation_ignorable());

        x = 0;
        y = 1;
        constraint.update();
        EXPECT_FALSE(constraint.is_evaluation_ignorable());

        x = 1;
        y = 1;
        constraint.update();
        EXPECT_TRUE(constraint.is_evaluation_ignorable());
    }
}

/*****************************************************************************/
TEST_F(TestConstraint, is_integer) {
    /// This method is tested in constructor_arg_expression()
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
TEST_F(TestConstraint, is_enabled) {
    auto constraint =
        model_component::Constraint<int, double>::create_instance();
    constraint.disable();
    EXPECT_FALSE(constraint.is_enabled());

    constraint.enable();
    EXPECT_TRUE(constraint.is_enabled());

    constraint.disable();
    EXPECT_FALSE(constraint.is_enabled());
}

/*****************************************************************************/
TEST_F(TestConstraint, is_less_or_equal) {
    /// This method is tested in other methods.
}

/*****************************************************************************/
TEST_F(TestConstraint, is_greater_or_equal) {
    /// This method is tested in other methods.
}

/*****************************************************************************/
TEST_F(TestConstraint, is_singleton) {
    /// This method is tested in update_constraint_type_singleton().
}

/*****************************************************************************/
TEST_F(TestConstraint, is_aggregation) {
    /// This method is tested in update_constraint_type_aggregation().
}

/*****************************************************************************/
TEST_F(TestConstraint, is_precedence) {
    /// This method is tested in update_constraint_type_precedence().
}

/*****************************************************************************/
TEST_F(TestConstraint, is_variable_bound) {
    /// This method is tested in update_constraint_type_variable_bound().
}

/*****************************************************************************/
TEST_F(TestConstraint, is_set_partitioning) {
    /// This method is tested in update_constraint_type_set_partitioning().
}

/*****************************************************************************/
TEST_F(TestConstraint, is_set_packing) {
    /// This method is tested in update_constraint_type_set_packing().
}

/*****************************************************************************/
TEST_F(TestConstraint, is_set_covering) {
    /// This method is tested in update_constraint_type_set_covering().
}

/*****************************************************************************/
TEST_F(TestConstraint, is_cardinality) {
    /// This method is tested in update_constraint_type_cardinality().
}

/*****************************************************************************/
TEST_F(TestConstraint, is_invariant_knapsack) {
    /// This method is tested in update_constraint_type_invariant_knapsack().
}

/*****************************************************************************/
TEST_F(TestConstraint, is_multiple_covering) {
    /// This method is tested in update_constraint_type_multiple_covering().
}

/*****************************************************************************/
TEST_F(TestConstraint, is_binary_flow) {
    /// This method is tested in update_constraint_type_binary_flow().
}

/*****************************************************************************/
TEST_F(TestConstraint, is_integer_flow) {
    /// This method is tested in update_constraint_type_integer_flow().
}

/*****************************************************************************/
TEST_F(TestConstraint, is_soft_selection) {
    /// This method is tested in
    /// update_constraint_type_soft_selection().
}

/*****************************************************************************/
TEST_F(TestConstraint, is_min_max) {
    /// This method is tested in update_constraint_type_min_max().
}

/*****************************************************************************/
TEST_F(TestConstraint, is_max_min) {
    /// This method is tested in update_constraint_type_max_min().
}

/*****************************************************************************/
TEST_F(TestConstraint, is_intermediate) {
    /// This method is tested in update_constraint_type_intermediate().
}

/*****************************************************************************/
TEST_F(TestConstraint, is_equation_knapsack) {
    /// This method is tested in update_constraint_type_equation_knapsack().
}

/*****************************************************************************/
TEST_F(TestConstraint, is_bin_packing) {
    /// This method is tested in update_constraint_type_bin_packing().
}

/*****************************************************************************/
TEST_F(TestConstraint, is_knapsack) {
    /// This method is tested in update_constraint_type_knapsack().
}

/*****************************************************************************/
TEST_F(TestConstraint, is_integer_knapsack) {
    /// This method is tested in update_constraint_type_integer_knapsack().
}

/*****************************************************************************/
TEST_F(TestConstraint, is_gf2) {
    /// This method is tested in update_constraint_type_gf2().
}

/*****************************************************************************/
TEST_F(TestConstraint, is_general_linear) {
    /// This method is tested in update_constraint_type_general_linear().
}

/*****************************************************************************/
TEST_F(TestConstraint, type) {
    /// This method is tested in other methods.
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/