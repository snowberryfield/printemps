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

    EXPECT_TRUE(constraint.is_type(model_component::ConstraintType::Unknown));
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
    EXPECT_TRUE(constraint.is_type(model_component::ConstraintType::Singleton));
    EXPECT_EQ("Singleton", constraint.type_label());
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
        EXPECT_TRUE(
            constraint.is_type(model_component::ConstraintType::ExclusiveOR));
        EXPECT_EQ("Exclusive OR", constraint.type_label());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-x.sum() + 1, model_component::ConstraintSense::Equal);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_TRUE(
            constraint.is_type(model_component::ConstraintType::ExclusiveOR));
        EXPECT_EQ("Exclusive OR", constraint.type_label());
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
        EXPECT_TRUE(
            constraint.is_type(model_component::ConstraintType::ExclusiveNOR));
        EXPECT_EQ("Exclusive NOR", constraint.type_label());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-x(0) + x(1), model_component::ConstraintSense::Equal);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_TRUE(
            constraint.is_type(model_component::ConstraintType::ExclusiveNOR));
        EXPECT_EQ("Exclusive NOR", constraint.type_label());
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
        EXPECT_TRUE(constraint.is_type(
            model_component::ConstraintType::InvertedIntegers));
        EXPECT_EQ("Inverted Integers", constraint.type_label());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-x(0) - x(1), model_component::ConstraintSense::Equal);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_TRUE(constraint.is_type(
            model_component::ConstraintType::InvertedIntegers));
        EXPECT_EQ("Inverted Integers", constraint.type_label());
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
        EXPECT_TRUE(constraint.is_type(
            model_component::ConstraintType::BalancedIntegers));
        EXPECT_EQ("Balanced Integers", constraint.type_label());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-x(0) + x(1), model_component::ConstraintSense::Equal);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_TRUE(constraint.is_type(
            model_component::ConstraintType::BalancedIntegers));
        EXPECT_EQ("Balanced Integers", constraint.type_label());
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
        EXPECT_TRUE(constraint.is_type(
            model_component::ConstraintType::ConstantSumIntegers));
        EXPECT_EQ("Constant Sum Integers", constraint.type_label());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-x(0) - x(1) + 1,
                         model_component::ConstraintSense::Equal);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_TRUE(constraint.is_type(
            model_component::ConstraintType::ConstantSumIntegers));
        EXPECT_EQ("Constant Sum Integers", constraint.type_label());
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
        EXPECT_TRUE(constraint.is_type(
            model_component::ConstraintType::ConstantDifferenceIntegers));
        EXPECT_EQ("Constant Difference Integers", constraint.type_label());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-x(0) + x(1) + 1,
                         model_component::ConstraintSense::Equal);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_TRUE(constraint.is_type(
            model_component::ConstraintType::ConstantDifferenceIntegers));
        EXPECT_EQ("Constant Difference Integers", constraint.type_label());
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
        EXPECT_TRUE(constraint.is_type(
            model_component::ConstraintType::ConstantRatioIntegers));
        EXPECT_EQ("Constant Ratio Integers", constraint.type_label());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(x(0) + 2 * x(1),
                         model_component::ConstraintSense::Equal);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_TRUE(constraint.is_type(
            model_component::ConstraintType::ConstantRatioIntegers));
        EXPECT_EQ("Constant Ratio Integers", constraint.type_label());
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
    EXPECT_TRUE(
        constraint.is_type(model_component::ConstraintType::Aggregation));
    EXPECT_EQ("Aggregation", constraint.type_label());
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
        EXPECT_TRUE(
            constraint.is_type(model_component::ConstraintType::Precedence));
        EXPECT_EQ("Precedence", constraint.type_label());
    }
    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-2 * x + 2 * y - 5,
                         model_component::ConstraintSense::Less);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_TRUE(
            constraint.is_type(model_component::ConstraintType::Precedence));
        EXPECT_EQ("Precedence", constraint.type_label());
    }
    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(2 * x - 2 * y - 5,
                         model_component::ConstraintSense::Greater);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_TRUE(
            constraint.is_type(model_component::ConstraintType::Precedence));
        EXPECT_EQ("Precedence", constraint.type_label());
    }
    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-2 * x + 2 * y - 5,
                         model_component::ConstraintSense::Greater);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_TRUE(
            constraint.is_type(model_component::ConstraintType::Precedence));
        EXPECT_EQ("Precedence", constraint.type_label());
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
        EXPECT_TRUE(
            constraint.is_type(model_component::ConstraintType::VariableBound));
        EXPECT_EQ("Variable Bound", constraint.type_label());
    }
    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(2 * x + 3 * y - 5,
                         model_component::ConstraintSense::Greater);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_TRUE(
            constraint.is_type(model_component::ConstraintType::VariableBound));
        EXPECT_EQ("Variable Bound", constraint.type_label());
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
        EXPECT_TRUE(constraint.is_type(
            model_component::ConstraintType::TrinomialExclusiveNOR));
        EXPECT_EQ("Trinomial Exclusive NOR", constraint.type_label());
    }
    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-x - y + 2 * z,
                         model_component::ConstraintSense::Equal);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_TRUE(constraint.is_type(
            model_component::ConstraintType::TrinomialExclusiveNOR));
        EXPECT_EQ("Trinomial Exclusive NOR", constraint.type_label());
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
        EXPECT_TRUE(constraint.is_type(
            model_component::ConstraintType::SetPartitioning));
        EXPECT_EQ("Set Partitioning", constraint.type_label());
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
    EXPECT_TRUE(
        constraint.is_type(model_component::ConstraintType::SetPacking));
    EXPECT_EQ("Set Packing", constraint.type_label());
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
    EXPECT_TRUE(
        constraint.is_type(model_component::ConstraintType::SetCovering));
    EXPECT_EQ("Set Covering", constraint.type_label());
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
    EXPECT_TRUE(
        constraint.is_type(model_component::ConstraintType::Cardinality));
    EXPECT_EQ("Cardinality", constraint.type_label());
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
    EXPECT_TRUE(
        constraint.is_type(model_component::ConstraintType::InvariantKnapsack));
    EXPECT_EQ("Invariant Knapsack", constraint.type_label());
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
    EXPECT_TRUE(
        constraint.is_type(model_component::ConstraintType::MultipleCovering));
    EXPECT_EQ("Multiple Covering", constraint.type_label());
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
    EXPECT_TRUE(
        constraint.is_type(model_component::ConstraintType::BinaryFlow));
    EXPECT_EQ("Binary Flow", constraint.type_label());
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
    EXPECT_TRUE(
        constraint.is_type(model_component::ConstraintType::IntegerFlow));
    EXPECT_EQ("Integer Flow", constraint.type_label());
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
    EXPECT_TRUE(
        constraint.is_type(model_component::ConstraintType::SoftSelection));
    EXPECT_EQ("Soft Selection", constraint.type_label());
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
        EXPECT_TRUE(
            constraint.is_type(model_component::ConstraintType::MinMax));
        EXPECT_EQ("Min-Max", constraint.type_label());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(x - y - 2 * z - 5,
                         model_component::ConstraintSense::Greater);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_TRUE(
            constraint.is_type(model_component::ConstraintType::MinMax));
        EXPECT_EQ("Min-Max", constraint.type_label());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-x + 20 * y + 20 * z,
                         model_component::ConstraintSense::Less);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_TRUE(
            constraint.is_type(model_component::ConstraintType::MinMax));
        EXPECT_EQ("Min-Max", constraint.type_label());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-x - 20 * y - 20 * z,
                         model_component::ConstraintSense::Less);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_TRUE(
            constraint.is_type(model_component::ConstraintType::MinMax));
        EXPECT_EQ("Min-Max", constraint.type_label());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-x - 20 * y + 20 * z,
                         model_component::ConstraintSense::Less);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_TRUE(
            constraint.is_type(model_component::ConstraintType::MinMax));
        EXPECT_EQ("Min-Max", constraint.type_label());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(x + 20 * y - 20 * z,
                         model_component::ConstraintSense::Greater);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_TRUE(
            constraint.is_type(model_component::ConstraintType::MinMax));
        EXPECT_EQ("Min-Max", constraint.type_label());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-x + y + z + 5,
                         model_component::ConstraintSense::Less);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_FALSE(
            constraint.is_type(model_component::ConstraintType::MinMax));
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-x + 1.5 * y + 2 * z + 5,
                         model_component::ConstraintSense::Less);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_FALSE(
            constraint.is_type(model_component::ConstraintType::MinMax));
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-x + y + 2 * z + 5.5,
                         model_component::ConstraintSense::Less);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_FALSE(
            constraint.is_type(model_component::ConstraintType::MinMax));
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
        EXPECT_TRUE(
            constraint.is_type(model_component::ConstraintType::MaxMin));
        EXPECT_EQ("Max-Min", constraint.type_label());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(x - y - 2 * z + 5,
                         model_component::ConstraintSense::Less);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_TRUE(
            constraint.is_type(model_component::ConstraintType::MaxMin));
        EXPECT_EQ("Max-Min", constraint.type_label());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-x + 20 * y + 20 * z,
                         model_component::ConstraintSense::Greater);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_TRUE(
            constraint.is_type(model_component::ConstraintType::MaxMin));
        EXPECT_EQ("Max-Min", constraint.type_label());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-x - 20 * y - 20 * z,
                         model_component::ConstraintSense::Greater);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_TRUE(
            constraint.is_type(model_component::ConstraintType::MaxMin));
        EXPECT_EQ("Max-Min", constraint.type_label());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-x - 20 * y + 20 * z,
                         model_component::ConstraintSense::Greater);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_TRUE(
            constraint.is_type(model_component::ConstraintType::MaxMin));
        EXPECT_EQ("Max-Min", constraint.type_label());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(x + 20 * y - 20 * z,
                         model_component::ConstraintSense::Less);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_TRUE(
            constraint.is_type(model_component::ConstraintType::MaxMin));
        EXPECT_EQ("Max-Min", constraint.type_label());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-x + y + z - 5,
                         model_component::ConstraintSense::Greater);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_FALSE(
            constraint.is_type(model_component::ConstraintType::MaxMin));
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-x + 1.5 * y + 2 * z - 5,
                         model_component::ConstraintSense::Greater);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_FALSE(
            constraint.is_type(model_component::ConstraintType::MaxMin));
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-x + y + 2 * z - 5.5,
                         model_component::ConstraintSense::Greater);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_FALSE(
            constraint.is_type(model_component::ConstraintType::MaxMin));
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
        EXPECT_TRUE(
            constraint.is_type(model_component::ConstraintType::Intermediate));
        EXPECT_EQ(&x(0), constraint.key_variable_ptr());
        EXPECT_EQ("Intermediate", constraint.type_label());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(x + y + 2 * z + 5,
                         model_component::ConstraintSense::Equal);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_TRUE(
            constraint.is_type(model_component::ConstraintType::Intermediate));
        EXPECT_EQ(&x(0), constraint.key_variable_ptr());
        EXPECT_EQ("Intermediate", constraint.type_label());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(x + 20 * y + 20 * z,
                         model_component::ConstraintSense::Equal);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_TRUE(
            constraint.is_type(model_component::ConstraintType::Intermediate));
        EXPECT_EQ(&x(0), constraint.key_variable_ptr());
        EXPECT_EQ("Intermediate", constraint.type_label());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(x + 20 * y - 20 * z,
                         model_component::ConstraintSense::Equal);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_TRUE(
            constraint.is_type(model_component::ConstraintType::Intermediate));
        EXPECT_EQ(&x(0), constraint.key_variable_ptr());
        EXPECT_EQ("Intermediate", constraint.type_label());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-x + y + z + 5,
                         model_component::ConstraintSense::Equal);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_FALSE(
            constraint.is_type(model_component::ConstraintType::Intermediate));
        EXPECT_EQ(nullptr, constraint.key_variable_ptr());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-x + 1.5 * y + 2 * z + 5,
                         model_component::ConstraintSense::Equal);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_FALSE(
            constraint.is_type(model_component::ConstraintType::Intermediate));
        EXPECT_EQ(nullptr, constraint.key_variable_ptr());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-x + y + 2 * z + 5.5,
                         model_component::ConstraintSense::Equal);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_FALSE(
            constraint.is_type(model_component::ConstraintType::Intermediate));
        EXPECT_EQ(nullptr, constraint.key_variable_ptr());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-2 * x + y + 2 * z + 5,
                         model_component::ConstraintSense::Equal);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_FALSE(
            constraint.is_type(model_component::ConstraintType::Intermediate));
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
    EXPECT_TRUE(
        constraint.is_type(model_component::ConstraintType::EquationKnapsack));
    EXPECT_EQ("Equation Knapsack", constraint.type_label());
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
        EXPECT_TRUE(
            constraint.is_type(model_component::ConstraintType::BinPacking));
        EXPECT_EQ("Bin Packing", constraint.type_label());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-x.dot(coefficients) - 5 * y + 5,
                         model_component::ConstraintSense::Greater);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_TRUE(
            constraint.is_type(model_component::ConstraintType::BinPacking));
        EXPECT_EQ("Bin Packing", constraint.type_label());
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
        EXPECT_TRUE(
            constraint.is_type(model_component::ConstraintType::Knapsack));
        EXPECT_EQ("Knapsack", constraint.type_label());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-x.dot(coefficients) + 50,
                         model_component::ConstraintSense::Greater);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_TRUE(
            constraint.is_type(model_component::ConstraintType::Knapsack));
        EXPECT_EQ("Knapsack", constraint.type_label());
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
        EXPECT_TRUE(constraint.is_type(
            model_component::ConstraintType::IntegerKnapsack));
        EXPECT_EQ("Integer Knapsack", constraint.type_label());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-x.dot(coefficients) + 50,
                         model_component::ConstraintSense::Greater);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_TRUE(constraint.is_type(
            model_component::ConstraintType::IntegerKnapsack));
        EXPECT_EQ("Integer Knapsack", constraint.type_label());
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
        EXPECT_TRUE(constraint.is_type(model_component::ConstraintType::GF2));
        EXPECT_EQ(&y(0), constraint.key_variable_ptr());
        EXPECT_EQ("GF(2)", constraint.type_label());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-x.sum() + 2 * y,
                         model_component::ConstraintSense::Equal);

        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_TRUE(constraint.is_type(model_component::ConstraintType::GF2));
        EXPECT_EQ(&y(0), constraint.key_variable_ptr());
        EXPECT_EQ("GF(2)", constraint.type_label());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(x.sum() + 1 - 2 * y,
                         model_component::ConstraintSense::Equal);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_TRUE(constraint.is_type(model_component::ConstraintType::GF2));
        EXPECT_EQ(&y(0), constraint.key_variable_ptr());
        EXPECT_EQ("GF(2)", constraint.type_label());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-x.sum() - 1 + 2 * y,
                         model_component::ConstraintSense::Equal);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_TRUE(constraint.is_type(model_component::ConstraintType::GF2));
        EXPECT_EQ(&y(0), constraint.key_variable_ptr());
        EXPECT_EQ("GF(2)", constraint.type_label());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(x.sum() - 1 - 2 * y,
                         model_component::ConstraintSense::Equal);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_TRUE(constraint.is_type(model_component::ConstraintType::GF2));
        EXPECT_EQ(&y(0), constraint.key_variable_ptr());
        EXPECT_EQ("GF(2)", constraint.type_label());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-x.sum() + 1 + 2 * y,
                         model_component::ConstraintSense::Equal);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_TRUE(constraint.is_type(model_component::ConstraintType::GF2));
        EXPECT_EQ(&y(0), constraint.key_variable_ptr());
        EXPECT_EQ("GF(2)", constraint.type_label());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(x.sum() + 2 - 2 * y,
                         model_component::ConstraintSense::Equal);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_FALSE(constraint.is_type(model_component::ConstraintType::GF2));
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-x.sum() - 2 + 2 * y,
                         model_component::ConstraintSense::Equal);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_FALSE(constraint.is_type(model_component::ConstraintType::GF2));
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(x.sum() - 2 - 2 * y,
                         model_component::ConstraintSense::Equal);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_FALSE(constraint.is_type(model_component::ConstraintType::GF2));
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-x.sum() + 2 + 2 * y,
                         model_component::ConstraintSense::Equal);
        constraint.update_basic_structure();
        constraint.update_constraint_type();
        EXPECT_FALSE(constraint.is_type(model_component::ConstraintType::GF2));
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
    EXPECT_TRUE(
        constraint.is_type(model_component::ConstraintType::GeneralLinear));
    EXPECT_EQ("General Linear", constraint.type_label());
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
    /// This test is covered by evaluate_expression_arg_void().
}

/*****************************************************************************/
TEST_F(TestConstraint, update_arg_move) {
    /// This test is covered by evaluate_expression_arg_move().
}

/*****************************************************************************/
TEST_F(TestConstraint, expression) {
    /// This test is covered by constructor_arg_expression().
}

/*****************************************************************************/
TEST_F(TestConstraint, sense) {
    /// This test is covered by tested in other cases.
}

/*****************************************************************************/
TEST_F(TestConstraint, value) {
    /// This test is covered by tested in other cases.
}

/*****************************************************************************/
TEST_F(TestConstraint, violation_value) {
    /// This test is covered by tested in other cases.
}

/*****************************************************************************/
TEST_F(TestConstraint, margin_value) {
    /// This test is covered by tested in other cases.
}

/*****************************************************************************/
TEST_F(TestConstraint, positive_part) {
    /// This test is covered by tested in other cases.
}

/*****************************************************************************/
TEST_F(TestConstraint, negative_part) {
    /// This test is covered by tested in other cases.
}

/*****************************************************************************/
TEST_F(TestConstraint, max_abs_coefficient) {
    /// This test is covered by tested in other cases.
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
    /// This test is covered by update_constraint_type_intermediate().
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
    /// This test is covered by increment_violation_count().
}

/*****************************************************************************/
TEST_F(TestConstraint, violation_count) {
    /// This test is covered by increment_violation_count().
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
    /// This test is covered by set_is_user_defined_selection().
}

/*****************************************************************************/
TEST_F(TestConstraint, has_only_binary_coefficient) {
    /// This test is covered by other methods.
}

/*****************************************************************************/
TEST_F(TestConstraint, has_only_binary_variable) {
    /// This test is covered by other methods.
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
    /// This test is covered by constructor_arg_expression()
}

/*****************************************************************************/
TEST_F(TestConstraint, enable) {
    /// This test is covered by is_enabled().
}

/*****************************************************************************/
TEST_F(TestConstraint, disable) {
    /// This test is covered by is_enabled().
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
    /// This test is covered by other methods.
}

/*****************************************************************************/
TEST_F(TestConstraint, is_greater_or_equal) {
    /// This test is covered by other methods.
}

/*****************************************************************************/
TEST_F(TestConstraint, is_type) {
    /// This test is covered by other methods.
}

/*****************************************************************************/
TEST_F(TestConstraint, type) {
    /// This test is covered by other methods.
}

/*****************************************************************************/
TEST_F(TestConstraint, type_label) {
    /// This test is covered by other methods.
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/