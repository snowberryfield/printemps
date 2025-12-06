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
class TestConstraintTypeClassifier : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestConstraintTypeClassifier, check_empty) {
    model::Model<int, double> model;

    auto& f = model.create_expression("f");
    f       = 0;

    auto constraint =
        model_component::Constraint<int, double>::create_instance();
    constraint.setup(f(0), model_component::ConstraintSense::Less);
    constraint.update_structure();
    constraint.update_constraint_type();

    EXPECT_TRUE(constraint.is_type(model_component::ConstraintType::Empty));
    EXPECT_EQ(model_component::ConstraintType::Empty, constraint.type());
    EXPECT_EQ("Empty", constraint.type_label());
    EXPECT_EQ(nullptr, constraint.key_variable_ptr());
}

/*****************************************************************************/
TEST_F(TestConstraintTypeClassifier, check_singleton) {
    model::Model<int, double> model;
    auto&                     x = model.create_variable("x", -10, 10);
    auto                      constraint =
        model_component::Constraint<int, double>::create_instance();
    constraint.setup(2 * x - 10, model_component::ConstraintSense::Less);
    constraint.update_structure();
    constraint.update_constraint_type();

    EXPECT_TRUE(constraint.is_type(model_component::ConstraintType::Singleton));
    EXPECT_EQ(model_component::ConstraintType::Singleton, constraint.type());
    EXPECT_EQ("Singleton", constraint.type_label());
    EXPECT_EQ(nullptr, constraint.key_variable_ptr());
}

/*****************************************************************************/
TEST_F(TestConstraintTypeClassifier, check_exclusive_or) {
    model::Model<int, double> model;
    auto&                     x = model.create_variables("x", 2, 0, 1);

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(x.sum() - 1, model_component::ConstraintSense::Equal);
        constraint.update_structure();
        constraint.update_constraint_type();

        EXPECT_TRUE(
            constraint.is_type(model_component::ConstraintType::ExclusiveOR));
        EXPECT_EQ(model_component::ConstraintType::ExclusiveOR,
                  constraint.type());
        EXPECT_EQ("Exclusive OR", constraint.type_label());
        EXPECT_EQ(&x(0), constraint.key_variable_ptr());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-x.sum() + 1, model_component::ConstraintSense::Equal);
        constraint.update_structure();
        constraint.update_constraint_type();

        EXPECT_TRUE(
            constraint.is_type(model_component::ConstraintType::ExclusiveOR));
        EXPECT_EQ(model_component::ConstraintType::ExclusiveOR,
                  constraint.type());
        EXPECT_EQ("Exclusive OR", constraint.type_label());
        EXPECT_EQ(&x(0), constraint.key_variable_ptr());
    }
}

/*****************************************************************************/
TEST_F(TestConstraintTypeClassifier, check_exclusive_nor) {
    model::Model<int, double> model;
    auto&                     x = model.create_variables("x", 2, 0, 1);
    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(x(0) - x(1), model_component::ConstraintSense::Equal);
        constraint.update_structure();
        constraint.update_constraint_type();

        EXPECT_TRUE(
            constraint.is_type(model_component::ConstraintType::ExclusiveNOR));
        EXPECT_EQ(model_component::ConstraintType::ExclusiveNOR,
                  constraint.type());
        EXPECT_EQ("Exclusive NOR", constraint.type_label());
        EXPECT_EQ(&x(0), constraint.key_variable_ptr());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-x(0) + x(1), model_component::ConstraintSense::Equal);
        constraint.update_structure();
        constraint.update_constraint_type();

        EXPECT_TRUE(
            constraint.is_type(model_component::ConstraintType::ExclusiveNOR));
        EXPECT_EQ(model_component::ConstraintType::ExclusiveNOR,
                  constraint.type());
        EXPECT_EQ("Exclusive NOR", constraint.type_label());
        EXPECT_EQ(&x(0), constraint.key_variable_ptr());
    }
}

/*****************************************************************************/
TEST_F(TestConstraintTypeClassifier, check_inverted_integers) {
    model::Model<int, double> model;
    auto&                     x = model.create_variables("x", 2, 0, 10);

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(x(0) + x(1), model_component::ConstraintSense::Equal);
        constraint.update_structure();
        constraint.update_constraint_type();

        EXPECT_TRUE(constraint.is_type(
            model_component::ConstraintType::InvertedIntegers));
        EXPECT_EQ(model_component::ConstraintType::InvertedIntegers,
                  constraint.type());
        EXPECT_EQ("Inverted Integers", constraint.type_label());
        EXPECT_EQ(&x(0), constraint.key_variable_ptr());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-x(0) - x(1), model_component::ConstraintSense::Equal);
        constraint.update_structure();
        constraint.update_constraint_type();

        EXPECT_TRUE(constraint.is_type(
            model_component::ConstraintType::InvertedIntegers));
        EXPECT_EQ(model_component::ConstraintType::InvertedIntegers,
                  constraint.type());
        EXPECT_EQ("Inverted Integers", constraint.type_label());
        EXPECT_EQ(&x(0), constraint.key_variable_ptr());
    }
}

/*****************************************************************************/
TEST_F(TestConstraintTypeClassifier, check_balanced_integers) {
    model::Model<int, double> model;
    auto&                     x = model.create_variables("x", 2, 0, 10);

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(x(0) - x(1), model_component::ConstraintSense::Equal);
        constraint.update_structure();
        constraint.update_constraint_type();

        EXPECT_TRUE(constraint.is_type(
            model_component::ConstraintType::BalancedIntegers));
        EXPECT_EQ(model_component::ConstraintType::BalancedIntegers,
                  constraint.type());
        EXPECT_EQ("Balanced Integers", constraint.type_label());
        EXPECT_EQ(&x(0), constraint.key_variable_ptr());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-x(0) + x(1), model_component::ConstraintSense::Equal);
        constraint.update_structure();
        constraint.update_constraint_type();

        EXPECT_TRUE(constraint.is_type(
            model_component::ConstraintType::BalancedIntegers));
        EXPECT_EQ(model_component::ConstraintType::BalancedIntegers,
                  constraint.type());
        EXPECT_EQ("Balanced Integers", constraint.type_label());
        EXPECT_EQ(&x(0), constraint.key_variable_ptr());
    }
}

/*****************************************************************************/
TEST_F(TestConstraintTypeClassifier, check_constant_sum_integers) {
    model::Model<int, double> model;
    auto&                     x = model.create_variables("x", 2, 0, 10);
    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(x(0) + x(1) - 1,
                         model_component::ConstraintSense::Equal);
        constraint.update_structure();
        constraint.update_constraint_type();

        EXPECT_TRUE(constraint.is_type(
            model_component::ConstraintType::ConstantSumIntegers));
        EXPECT_EQ(model_component::ConstraintType::ConstantSumIntegers,
                  constraint.type());
        EXPECT_EQ("Constant Sum Integers", constraint.type_label());
        EXPECT_EQ(&x(0), constraint.key_variable_ptr());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-x(0) - x(1) + 1,
                         model_component::ConstraintSense::Equal);
        constraint.update_structure();
        constraint.update_constraint_type();

        EXPECT_TRUE(constraint.is_type(
            model_component::ConstraintType::ConstantSumIntegers));
        EXPECT_EQ(model_component::ConstraintType::ConstantSumIntegers,
                  constraint.type());
        EXPECT_EQ("Constant Sum Integers", constraint.type_label());
        EXPECT_EQ(&x(0), constraint.key_variable_ptr());
    }
}

/*****************************************************************************/
TEST_F(TestConstraintTypeClassifier, check_constant_difference_integers) {
    model::Model<int, double> model;
    auto&                     x = model.create_variables("x", 2, 0, 10);
    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(x(0) - x(1) - 1,
                         model_component::ConstraintSense::Equal);
        constraint.update_structure();
        constraint.update_constraint_type();

        EXPECT_TRUE(constraint.is_type(
            model_component::ConstraintType::ConstantDifferenceIntegers));
        EXPECT_EQ(model_component::ConstraintType::ConstantDifferenceIntegers,
                  constraint.type());
        EXPECT_EQ("Constant Difference Integers", constraint.type_label());
        EXPECT_EQ(&x(0), constraint.key_variable_ptr());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-x(0) + x(1) + 1,
                         model_component::ConstraintSense::Equal);
        constraint.update_structure();
        constraint.update_constraint_type();

        EXPECT_TRUE(constraint.is_type(
            model_component::ConstraintType::ConstantDifferenceIntegers));
        EXPECT_EQ(model_component::ConstraintType::ConstantDifferenceIntegers,
                  constraint.type());
        EXPECT_EQ("Constant Difference Integers", constraint.type_label());
        EXPECT_EQ(&x(0), constraint.key_variable_ptr());
    }
}

/*****************************************************************************/
TEST_F(TestConstraintTypeClassifier, check_constant_ratio_integers) {
    model::Model<int, double> model;
    auto&                     x = model.create_variables("x", 2, 0, 10);
    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(2 * x(0) - x(1),
                         model_component::ConstraintSense::Equal);
        constraint.update_structure();
        constraint.update_constraint_type();

        EXPECT_TRUE(constraint.is_type(
            model_component::ConstraintType::ConstantRatioIntegers));
        EXPECT_EQ(model_component::ConstraintType::ConstantRatioIntegers,
                  constraint.type());
        EXPECT_EQ("Constant Ratio Integers", constraint.type_label());
        EXPECT_EQ(&x(1), constraint.key_variable_ptr());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(x(0) + 2 * x(1),
                         model_component::ConstraintSense::Equal);
        constraint.update_structure();
        constraint.update_constraint_type();

        EXPECT_TRUE(constraint.is_type(
            model_component::ConstraintType::ConstantRatioIntegers));
        EXPECT_EQ(model_component::ConstraintType::ConstantRatioIntegers,
                  constraint.type());
        EXPECT_EQ("Constant Ratio Integers", constraint.type_label());
        EXPECT_EQ(&x(0), constraint.key_variable_ptr());
    }
}

/*****************************************************************************/
TEST_F(TestConstraintTypeClassifier, check_aggregation) {
    model::Model<int, double> model;
    auto&                     x = model.create_variable("x", -10, 10);
    auto&                     y = model.create_variable("y", -10, 10);
    auto                      constraint =
        model_component::Constraint<int, double>::create_instance();
    constraint.setup(2 * x + 3 * y - 10,
                     model_component::ConstraintSense::Equal);
    constraint.update_structure();
    constraint.update_constraint_type();
    EXPECT_TRUE(
        constraint.is_type(model_component::ConstraintType::Aggregation));
    EXPECT_EQ(model_component::ConstraintType::Aggregation, constraint.type());
    EXPECT_EQ("Aggregation", constraint.type_label());
    EXPECT_EQ(nullptr, constraint.key_variable_ptr());
}

/*****************************************************************************/
TEST_F(TestConstraintTypeClassifier, check_precedence) {
    model::Model<int, double> model;
    auto&                     x = model.create_variable("x", -10, 10);
    auto&                     y = model.create_variable("y", -10, 10);
    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(2 * x - 2 * y - 5,
                         model_component::ConstraintSense::Less);
        constraint.update_structure();
        constraint.update_constraint_type();

        EXPECT_TRUE(
            constraint.is_type(model_component::ConstraintType::Precedence));
        EXPECT_EQ(model_component::ConstraintType::Precedence,
                  constraint.type());
        EXPECT_EQ("Precedence", constraint.type_label());
        EXPECT_EQ(nullptr, constraint.key_variable_ptr());
    }
    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-2 * x + 2 * y - 5,
                         model_component::ConstraintSense::Less);
        constraint.update_structure();
        constraint.update_constraint_type();

        EXPECT_TRUE(
            constraint.is_type(model_component::ConstraintType::Precedence));
        EXPECT_EQ(model_component::ConstraintType::Precedence,
                  constraint.type());
        EXPECT_EQ("Precedence", constraint.type_label());
        EXPECT_EQ(nullptr, constraint.key_variable_ptr());
    }
    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(2 * x - 2 * y - 5,
                         model_component::ConstraintSense::Greater);
        constraint.update_structure();
        constraint.update_constraint_type();

        EXPECT_TRUE(
            constraint.is_type(model_component::ConstraintType::Precedence));
        EXPECT_EQ(model_component::ConstraintType::Precedence,
                  constraint.type());
        EXPECT_EQ("Precedence", constraint.type_label());
        EXPECT_EQ(nullptr, constraint.key_variable_ptr());
    }
    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-2 * x + 2 * y - 5,
                         model_component::ConstraintSense::Greater);
        constraint.update_structure();
        constraint.update_constraint_type();

        EXPECT_TRUE(
            constraint.is_type(model_component::ConstraintType::Precedence));
        EXPECT_EQ(model_component::ConstraintType::Precedence,
                  constraint.type());
        EXPECT_EQ("Precedence", constraint.type_label());
        EXPECT_EQ(nullptr, constraint.key_variable_ptr());
    }
}

/*****************************************************************************/
TEST_F(TestConstraintTypeClassifier, check_variable_bound) {
    model::Model<int, double> model;

    auto& x = model.create_variable("x", 0, 1);
    auto& y = model.create_variable("y", 0, 1);

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(2 * x + 3 * y - 5,
                         model_component::ConstraintSense::Less);
        constraint.update_structure();
        constraint.update_constraint_type();

        EXPECT_TRUE(
            constraint.is_type(model_component::ConstraintType::VariableBound));
        EXPECT_EQ(model_component::ConstraintType::VariableBound,
                  constraint.type());
        EXPECT_EQ("Variable Bound", constraint.type_label());
        EXPECT_EQ(nullptr, constraint.key_variable_ptr());
    }
    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(2 * x + 3 * y - 5,
                         model_component::ConstraintSense::Greater);
        constraint.update_structure();
        constraint.update_constraint_type();

        EXPECT_TRUE(
            constraint.is_type(model_component::ConstraintType::VariableBound));
        EXPECT_EQ(model_component::ConstraintType::VariableBound,
                  constraint.type());
        EXPECT_EQ("Variable Bound", constraint.type_label());
        EXPECT_EQ(nullptr, constraint.key_variable_ptr());
    }
}

/*****************************************************************************/
TEST_F(TestConstraintTypeClassifier, check_trinomial_exclusive_nor) {
    model::Model<int, double> model;

    auto& x = model.create_variable("x", 0, 1);
    auto& y = model.create_variable("y", 0, 1);
    auto& z = model.create_variable("z", 0, 1);

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(x + y - 2 * z,
                         model_component::ConstraintSense::Equal);
        constraint.update_structure();
        constraint.update_constraint_type();

        EXPECT_TRUE(constraint.is_type(
            model_component::ConstraintType::TrinomialExclusiveNOR));
        EXPECT_EQ(model_component::ConstraintType::TrinomialExclusiveNOR,
                  constraint.type());
        EXPECT_EQ("Trinomial Exclusive NOR", constraint.type_label());
        EXPECT_EQ(&z(0), constraint.key_variable_ptr());
    }
    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-x - y + 2 * z,
                         model_component::ConstraintSense::Equal);
        constraint.update_structure();
        constraint.update_constraint_type();

        EXPECT_TRUE(constraint.is_type(
            model_component::ConstraintType::TrinomialExclusiveNOR));
        EXPECT_EQ(model_component::ConstraintType::TrinomialExclusiveNOR,
                  constraint.type());
        EXPECT_EQ("Trinomial Exclusive NOR", constraint.type_label());
        EXPECT_EQ(&z(0), constraint.key_variable_ptr());
    }
}

/*****************************************************************************/
TEST_F(TestConstraintTypeClassifier, check_set_partitioning) {
    model::Model<int, double> model;
    auto&                     x = model.create_variables("x", 10, 0, 1);
    auto                      constraint =
        model_component::Constraint<int, double>::create_instance();
    constraint.setup(x.sum() - 1, model_component::ConstraintSense::Equal);
    constraint.update_structure();
    constraint.update_constraint_type();

    EXPECT_TRUE(
        constraint.is_type(model_component::ConstraintType::SetPartitioning));
    EXPECT_EQ(model_component::ConstraintType::SetPartitioning,
              constraint.type());
    EXPECT_EQ("Set Partitioning", constraint.type_label());
    EXPECT_EQ(nullptr, constraint.key_variable_ptr());
}

/*****************************************************************************/
TEST_F(TestConstraintTypeClassifier, check_set_packing) {
    model::Model<int, double> model;
    auto&                     x = model.create_variables("x", 10, 0, 1);
    auto                      constraint =
        model_component::Constraint<int, double>::create_instance();
    constraint.setup(x.sum() - 1, model_component::ConstraintSense::Less);
    constraint.update_structure();
    constraint.update_constraint_type();

    EXPECT_TRUE(
        constraint.is_type(model_component::ConstraintType::SetPacking));
    EXPECT_EQ(model_component::ConstraintType::SetPacking, constraint.type());
    EXPECT_EQ("Set Packing", constraint.type_label());
    EXPECT_EQ(nullptr, constraint.key_variable_ptr());
}

/*****************************************************************************/
TEST_F(TestConstraintTypeClassifier, check_set_covering) {
    model::Model<int, double> model;
    auto&                     x = model.create_variables("x", 10, 0, 1);
    auto                      constraint =
        model_component::Constraint<int, double>::create_instance();
    constraint.setup(x.sum() - 1, model_component::ConstraintSense::Greater);
    constraint.update_structure();
    constraint.update_constraint_type();

    EXPECT_TRUE(
        constraint.is_type(model_component::ConstraintType::SetCovering));
    EXPECT_EQ(model_component::ConstraintType::SetCovering, constraint.type());
    EXPECT_EQ("Set Covering", constraint.type_label());
    EXPECT_EQ(nullptr, constraint.key_variable_ptr());
}

/*****************************************************************************/
TEST_F(TestConstraintTypeClassifier, check_cardinality) {
    model::Model<int, double> model;
    auto&                     x = model.create_variables("x", 10, 0, 1);
    auto                      constraint =
        model_component::Constraint<int, double>::create_instance();
    constraint.setup(x.sum() - 5, model_component::ConstraintSense::Equal);
    constraint.update_structure();
    constraint.update_constraint_type();

    EXPECT_TRUE(
        constraint.is_type(model_component::ConstraintType::Cardinality));
    EXPECT_EQ(model_component::ConstraintType::Cardinality, constraint.type());
    EXPECT_EQ("Cardinality", constraint.type_label());
    EXPECT_EQ(nullptr, constraint.key_variable_ptr());
}

/*****************************************************************************/
TEST_F(TestConstraintTypeClassifier, check_invariant_knapsack) {
    model::Model<int, double> model;
    auto&                     x = model.create_variables("x", 10, 0, 1);
    auto                      constraint =
        model_component::Constraint<int, double>::create_instance();
    constraint.setup(x.sum() - 5, model_component::ConstraintSense::Less);
    constraint.update_structure();
    constraint.update_constraint_type();

    EXPECT_TRUE(
        constraint.is_type(model_component::ConstraintType::InvariantKnapsack));
    EXPECT_EQ(model_component::ConstraintType::InvariantKnapsack,
              constraint.type());
    EXPECT_EQ("Invariant Knapsack", constraint.type_label());
    EXPECT_EQ(nullptr, constraint.key_variable_ptr());
}

/*****************************************************************************/
TEST_F(TestConstraintTypeClassifier, check_multiple_covering) {
    model::Model<int, double> model;
    auto&                     x = model.create_variables("x", 10, 0, 1);
    auto                      constraint =
        model_component::Constraint<int, double>::create_instance();
    constraint.setup(x.sum() - 5, model_component::ConstraintSense::Greater);
    constraint.update_structure();
    constraint.update_constraint_type();

    EXPECT_TRUE(
        constraint.is_type(model_component::ConstraintType::MultipleCovering));
    EXPECT_EQ(model_component::ConstraintType::MultipleCovering,
              constraint.type());
    EXPECT_EQ("Multiple Covering", constraint.type_label());
    EXPECT_EQ(nullptr, constraint.key_variable_ptr());
}

/*****************************************************************************/
TEST_F(TestConstraintTypeClassifier, check_binary_flow) {
    model::Model<int, double> model;
    auto&                     x = model.create_variables("x", 10, 0, 1);
    auto                      constraint =
        model_component::Constraint<int, double>::create_instance();
    constraint.setup(x(0) + x(1) + x(2) - x(3) - x(4) - x(5),
                     model_component::ConstraintSense::Equal);
    constraint.update_structure();
    constraint.update_constraint_type();

    EXPECT_TRUE(
        constraint.is_type(model_component::ConstraintType::BinaryFlow));
    EXPECT_EQ(model_component::ConstraintType::BinaryFlow, constraint.type());
    EXPECT_EQ("Binary Flow", constraint.type_label());
    EXPECT_EQ(nullptr, constraint.key_variable_ptr());
}

/*****************************************************************************/
TEST_F(TestConstraintTypeClassifier, check_integer_flow) {
    model::Model<int, double> model;
    auto&                     x = model.create_variables("x", 10, 0, 10);
    auto                      constraint =
        model_component::Constraint<int, double>::create_instance();
    constraint.setup(x(0) + x(1) + x(2) - x(3) - x(4) - x(5),
                     model_component::ConstraintSense::Equal);
    constraint.update_structure();
    constraint.update_constraint_type();

    EXPECT_TRUE(
        constraint.is_type(model_component::ConstraintType::IntegerFlow));
    EXPECT_EQ(model_component::ConstraintType::IntegerFlow, constraint.type());
    EXPECT_EQ("Integer Flow", constraint.type_label());
    EXPECT_EQ(nullptr, constraint.key_variable_ptr());
}

/*****************************************************************************/
TEST_F(TestConstraintTypeClassifier, check_soft_selection) {
    model::Model<int, double> model;
    auto&                     x = model.create_variables("x", 10, 0, 1);
    auto                      constraint =
        model_component::Constraint<int, double>::create_instance();
    constraint.setup(x(0) + x(1) + x(2) + x(3) + x(4) - x(5),
                     model_component::ConstraintSense::Equal);
    constraint.update_structure();
    constraint.update_constraint_type();

    EXPECT_TRUE(
        constraint.is_type(model_component::ConstraintType::SoftSelection));
    EXPECT_EQ(model_component::ConstraintType::SoftSelection,
              constraint.type());
    EXPECT_EQ("Soft Selection", constraint.type_label());
    EXPECT_EQ(&x(5), constraint.key_variable_ptr());
}

/*****************************************************************************/
TEST_F(TestConstraintTypeClassifier, check_min_max) {
    model::Model<int, double> model;

    auto& x = model.create_variable("x", -10, 10);
    auto& y = model.create_variable("y", 0, 1);
    auto& z = model.create_variable("z", 0, 2);
    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-x + y + 2 * z + 5,
                         model_component::ConstraintSense::Less);
        constraint.update_structure();
        constraint.update_constraint_type();

        EXPECT_TRUE(
            constraint.is_type(model_component::ConstraintType::MinMax));
        EXPECT_EQ(model_component::ConstraintType::MinMax, constraint.type());
        EXPECT_EQ("Min-Max", constraint.type_label());
        EXPECT_EQ(&x(0), constraint.key_variable_ptr());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(x - y - 2 * z - 5,
                         model_component::ConstraintSense::Greater);
        constraint.update_structure();
        constraint.update_constraint_type();

        EXPECT_TRUE(
            constraint.is_type(model_component::ConstraintType::MinMax));
        EXPECT_EQ(model_component::ConstraintType::MinMax, constraint.type());
        EXPECT_EQ("Min-Max", constraint.type_label());
        EXPECT_EQ(&x(0), constraint.key_variable_ptr());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-x + 20 * y + 20 * z,
                         model_component::ConstraintSense::Less);
        constraint.update_structure();
        constraint.update_constraint_type();

        EXPECT_TRUE(
            constraint.is_type(model_component::ConstraintType::MinMax));
        EXPECT_EQ(model_component::ConstraintType::MinMax, constraint.type());
        EXPECT_EQ("Min-Max", constraint.type_label());
        EXPECT_EQ(&x(0), constraint.key_variable_ptr());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-x - 20 * y - 20 * z,
                         model_component::ConstraintSense::Less);
        constraint.update_structure();
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
        constraint.update_structure();
        constraint.update_constraint_type();

        EXPECT_TRUE(
            constraint.is_type(model_component::ConstraintType::MinMax));
        EXPECT_EQ(model_component::ConstraintType::MinMax, constraint.type());
        EXPECT_EQ("Min-Max", constraint.type_label());
        EXPECT_EQ(&x(0), constraint.key_variable_ptr());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(x + 20 * y - 20 * z,
                         model_component::ConstraintSense::Greater);
        constraint.update_structure();
        constraint.update_constraint_type();

        EXPECT_TRUE(
            constraint.is_type(model_component::ConstraintType::MinMax));
        EXPECT_EQ(model_component::ConstraintType::MinMax, constraint.type());
        EXPECT_EQ("Min-Max", constraint.type_label());
        EXPECT_EQ(&x(0), constraint.key_variable_ptr());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-x + y + z + 5,
                         model_component::ConstraintSense::Less);
        constraint.update_structure();
        constraint.update_constraint_type();

        EXPECT_FALSE(
            constraint.is_type(model_component::ConstraintType::MinMax));
        EXPECT_EQ(nullptr, constraint.key_variable_ptr());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-x + 1.5 * y + 2 * z + 5,
                         model_component::ConstraintSense::Less);
        constraint.update_structure();
        constraint.update_constraint_type();

        EXPECT_FALSE(
            constraint.is_type(model_component::ConstraintType::MinMax));
        EXPECT_EQ(nullptr, constraint.key_variable_ptr());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-x + y + 2 * z + 5.5,
                         model_component::ConstraintSense::Less);
        constraint.update_structure();
        constraint.update_constraint_type();

        EXPECT_FALSE(
            constraint.is_type(model_component::ConstraintType::MinMax));
        EXPECT_EQ(nullptr, constraint.key_variable_ptr());
    }
}

/*****************************************************************************/
TEST_F(TestConstraintTypeClassifier, check_max_min) {
    model::Model<int, double> model;

    auto& x = model.create_variable("x", -10, 10);
    auto& y = model.create_variable("y", 0, 1);
    auto& z = model.create_variable("z", 0, 2);
    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-x + y + 2 * z - 5,
                         model_component::ConstraintSense::Greater);
        constraint.update_structure();
        constraint.update_constraint_type();

        EXPECT_TRUE(
            constraint.is_type(model_component::ConstraintType::MaxMin));
        EXPECT_EQ(model_component::ConstraintType::MaxMin, constraint.type());
        EXPECT_EQ("Max-Min", constraint.type_label());
        EXPECT_EQ(&x(0), constraint.key_variable_ptr());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(x - y - 2 * z + 5,
                         model_component::ConstraintSense::Less);
        constraint.update_structure();
        constraint.update_constraint_type();

        EXPECT_TRUE(
            constraint.is_type(model_component::ConstraintType::MaxMin));
        EXPECT_EQ(model_component::ConstraintType::MaxMin, constraint.type());
        EXPECT_EQ("Max-Min", constraint.type_label());
        EXPECT_EQ(&x(0), constraint.key_variable_ptr());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-x + 20 * y + 20 * z,
                         model_component::ConstraintSense::Greater);
        constraint.update_structure();
        constraint.update_constraint_type();

        EXPECT_TRUE(
            constraint.is_type(model_component::ConstraintType::MaxMin));
        EXPECT_EQ(model_component::ConstraintType::MaxMin, constraint.type());
        EXPECT_EQ("Max-Min", constraint.type_label());
        EXPECT_EQ(&x(0), constraint.key_variable_ptr());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-x - 20 * y - 20 * z,
                         model_component::ConstraintSense::Greater);
        constraint.update_structure();
        constraint.update_constraint_type();

        EXPECT_TRUE(
            constraint.is_type(model_component::ConstraintType::MaxMin));
        EXPECT_EQ(model_component::ConstraintType::MaxMin, constraint.type());
        EXPECT_EQ("Max-Min", constraint.type_label());
        EXPECT_EQ(&x(0), constraint.key_variable_ptr());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-x - 20 * y + 20 * z,
                         model_component::ConstraintSense::Greater);
        constraint.update_structure();
        constraint.update_constraint_type();

        EXPECT_TRUE(
            constraint.is_type(model_component::ConstraintType::MaxMin));
        EXPECT_EQ(model_component::ConstraintType::MaxMin, constraint.type());
        EXPECT_EQ("Max-Min", constraint.type_label());
        EXPECT_EQ(&x(0), constraint.key_variable_ptr());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(x + 20 * y - 20 * z,
                         model_component::ConstraintSense::Less);
        constraint.update_structure();
        constraint.update_constraint_type();

        EXPECT_TRUE(
            constraint.is_type(model_component::ConstraintType::MaxMin));
        EXPECT_EQ(model_component::ConstraintType::MaxMin, constraint.type());
        EXPECT_EQ("Max-Min", constraint.type_label());
        EXPECT_EQ(&x(0), constraint.key_variable_ptr());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-x + y + z - 5,
                         model_component::ConstraintSense::Greater);
        constraint.update_structure();
        constraint.update_constraint_type();

        EXPECT_FALSE(
            constraint.is_type(model_component::ConstraintType::MaxMin));
        EXPECT_EQ(nullptr, constraint.key_variable_ptr());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-x + 1.5 * y + 2 * z - 5,
                         model_component::ConstraintSense::Greater);
        constraint.update_structure();
        constraint.update_constraint_type();

        EXPECT_FALSE(
            constraint.is_type(model_component::ConstraintType::MaxMin));
        EXPECT_EQ(nullptr, constraint.key_variable_ptr());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-x + y + 2 * z - 5.5,
                         model_component::ConstraintSense::Greater);
        constraint.update_structure();
        constraint.update_constraint_type();

        EXPECT_FALSE(
            constraint.is_type(model_component::ConstraintType::MaxMin));
        EXPECT_EQ(nullptr, constraint.key_variable_ptr());
    }
}

/*****************************************************************************/
TEST_F(TestConstraintTypeClassifier, check_intermediate) {
    model::Model<int, double> model;

    auto& x = model.create_variable("x", -10, 10);
    auto& y = model.create_variable("y", 0, 1);
    auto& z = model.create_variable("z", 0, 2);

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-x + y + 2 * z + 5,
                         model_component::ConstraintSense::Equal);
        constraint.update_structure();
        constraint.update_constraint_type();

        EXPECT_TRUE(
            constraint.is_type(model_component::ConstraintType::Intermediate));
        EXPECT_EQ(model_component::ConstraintType::Intermediate,
                  constraint.type());
        EXPECT_EQ("Intermediate", constraint.type_label());
        EXPECT_EQ(&x(0), constraint.key_variable_ptr());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(x + y + 2 * z + 5,
                         model_component::ConstraintSense::Equal);
        constraint.update_structure();
        constraint.update_constraint_type();

        EXPECT_TRUE(
            constraint.is_type(model_component::ConstraintType::Intermediate));
        EXPECT_EQ(model_component::ConstraintType::Intermediate,
                  constraint.type());
        EXPECT_EQ("Intermediate", constraint.type_label());
        EXPECT_EQ(&x(0), constraint.key_variable_ptr());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(x + 20 * y + 20 * z,
                         model_component::ConstraintSense::Equal);
        constraint.update_structure();
        constraint.update_constraint_type();

        EXPECT_TRUE(
            constraint.is_type(model_component::ConstraintType::Intermediate));
        EXPECT_EQ(model_component::ConstraintType::Intermediate,
                  constraint.type());
        EXPECT_EQ("Intermediate", constraint.type_label());
        EXPECT_EQ(&x(0), constraint.key_variable_ptr());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(x + 20 * y - 20 * z,
                         model_component::ConstraintSense::Equal);
        constraint.update_structure();
        constraint.update_constraint_type();

        EXPECT_TRUE(
            constraint.is_type(model_component::ConstraintType::Intermediate));
        EXPECT_EQ(model_component::ConstraintType::Intermediate,
                  constraint.type());
        EXPECT_EQ("Intermediate", constraint.type_label());
        EXPECT_EQ(&x(0), constraint.key_variable_ptr());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-x + y + z + 5,
                         model_component::ConstraintSense::Equal);
        constraint.update_structure();
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
        constraint.update_structure();
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
        constraint.update_structure();
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
        constraint.update_structure();
        constraint.update_constraint_type();
        EXPECT_FALSE(
            constraint.is_type(model_component::ConstraintType::Intermediate));
        EXPECT_EQ(nullptr, constraint.key_variable_ptr());
    }
}

/*****************************************************************************/
TEST_F(TestConstraintTypeClassifier, check_equation_knapsack) {
    model::Model<int, double> model;
    auto                      coefficients = utility::sequence(10);

    auto& x = model.create_variables("x", 10, 0, 1);
    auto  constraint =
        model_component::Constraint<int, double>::create_instance();
    constraint.setup(x.dot(coefficients) - 30,
                     model_component::ConstraintSense::Equal);
    constraint.update_structure();
    constraint.update_constraint_type();

    EXPECT_TRUE(
        constraint.is_type(model_component::ConstraintType::EquationKnapsack));
    EXPECT_EQ(model_component::ConstraintType::EquationKnapsack,
              constraint.type());
    EXPECT_EQ("Equation Knapsack", constraint.type_label());
    EXPECT_EQ(nullptr, constraint.key_variable_ptr());
}

/*****************************************************************************/
TEST_F(TestConstraintTypeClassifier, check_bin_packing) {
    model::Model<int, double> model;
    auto                      coefficients = utility::sequence(10);

    auto& x = model.create_variables("x", 10, 0, 1);
    auto& y = model.create_variable("y", 0, 1);
    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(x.dot(coefficients) + 5 * y - 5,
                         model_component::ConstraintSense::Less);
        constraint.update_structure();
        constraint.update_constraint_type();

        EXPECT_TRUE(
            constraint.is_type(model_component::ConstraintType::BinPacking));
        EXPECT_EQ(model_component::ConstraintType::BinPacking,
                  constraint.type());
        EXPECT_EQ("Bin Packing", constraint.type_label());
        EXPECT_EQ(nullptr, constraint.key_variable_ptr());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-x.dot(coefficients) - 5 * y + 5,
                         model_component::ConstraintSense::Greater);
        constraint.update_structure();
        constraint.update_constraint_type();

        EXPECT_TRUE(
            constraint.is_type(model_component::ConstraintType::BinPacking));
        EXPECT_EQ(model_component::ConstraintType::BinPacking,
                  constraint.type());
        EXPECT_EQ("Bin Packing", constraint.type_label());
        EXPECT_EQ(nullptr, constraint.key_variable_ptr());
    }
}

/*****************************************************************************/
TEST_F(TestConstraintTypeClassifier, check_knapsack) {
    model::Model<int, double> model;
    auto                      coefficients = utility::sequence(10);

    auto& x = model.create_variables("x", 10, 0, 1);
    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(x.dot(coefficients) - 50,
                         model_component::ConstraintSense::Less);
        constraint.update_structure();
        constraint.update_constraint_type();

        EXPECT_TRUE(
            constraint.is_type(model_component::ConstraintType::Knapsack));
        EXPECT_EQ(model_component::ConstraintType::Knapsack, constraint.type());
        EXPECT_EQ("Knapsack", constraint.type_label());
        EXPECT_EQ(nullptr, constraint.key_variable_ptr());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-x.dot(coefficients) + 50,
                         model_component::ConstraintSense::Greater);
        constraint.update_structure();
        constraint.update_constraint_type();

        EXPECT_TRUE(
            constraint.is_type(model_component::ConstraintType::Knapsack));
        EXPECT_EQ(model_component::ConstraintType::Knapsack, constraint.type());
        EXPECT_EQ("Knapsack", constraint.type_label());
        EXPECT_EQ(nullptr, constraint.key_variable_ptr());
    }
}

/*****************************************************************************/
TEST_F(TestConstraintTypeClassifier, check_integer_knapsack) {
    model::Model<int, double> model;
    auto                      coefficients = utility::sequence(10, 20);

    auto& x = model.create_variables("x", 10, 0, 10);
    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(x.dot(coefficients) - 50,
                         model_component::ConstraintSense::Less);
        constraint.update_structure();
        constraint.update_constraint_type();

        EXPECT_TRUE(constraint.is_type(
            model_component::ConstraintType::IntegerKnapsack));
        EXPECT_EQ(model_component::ConstraintType::IntegerKnapsack,
                  constraint.type());
        EXPECT_EQ("Integer Knapsack", constraint.type_label());
        EXPECT_EQ(nullptr, constraint.key_variable_ptr());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-x.dot(coefficients) + 50,
                         model_component::ConstraintSense::Greater);
        constraint.update_structure();
        constraint.update_constraint_type();

        EXPECT_TRUE(constraint.is_type(
            model_component::ConstraintType::IntegerKnapsack));
        EXPECT_EQ(model_component::ConstraintType::IntegerKnapsack,
                  constraint.type());
        EXPECT_EQ("Integer Knapsack", constraint.type_label());
        EXPECT_EQ(nullptr, constraint.key_variable_ptr());
    }
}

/*****************************************************************************/
TEST_F(TestConstraintTypeClassifier, check_gf2) {
    model::Model<int, double> model;

    auto& x = model.create_variables("x", 10, 0, 1);
    auto& y = model.create_variable("y", 0, 5);

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(x.sum() - 2 * y,
                         model_component::ConstraintSense::Equal);

        constraint.update_structure();
        constraint.update_constraint_type();

        EXPECT_TRUE(constraint.is_type(model_component::ConstraintType::GF2));
        EXPECT_EQ(model_component::ConstraintType::GF2, constraint.type());
        EXPECT_EQ("GF(2)", constraint.type_label());
        EXPECT_EQ(&y(0), constraint.key_variable_ptr());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-x.sum() + 2 * y,
                         model_component::ConstraintSense::Equal);

        constraint.update_structure();
        constraint.update_constraint_type();

        EXPECT_TRUE(constraint.is_type(model_component::ConstraintType::GF2));
        EXPECT_EQ(model_component::ConstraintType::GF2, constraint.type());
        EXPECT_EQ("GF(2)", constraint.type_label());
        EXPECT_EQ(&y(0), constraint.key_variable_ptr());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(x.sum() + 1 - 2 * y,
                         model_component::ConstraintSense::Equal);
        constraint.update_structure();
        constraint.update_constraint_type();

        EXPECT_TRUE(constraint.is_type(model_component::ConstraintType::GF2));
        EXPECT_EQ(model_component::ConstraintType::GF2, constraint.type());
        EXPECT_EQ("GF(2)", constraint.type_label());
        EXPECT_EQ(&y(0), constraint.key_variable_ptr());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-x.sum() - 1 + 2 * y,
                         model_component::ConstraintSense::Equal);
        constraint.update_structure();
        constraint.update_constraint_type();

        EXPECT_TRUE(constraint.is_type(model_component::ConstraintType::GF2));
        EXPECT_EQ(model_component::ConstraintType::GF2, constraint.type());
        EXPECT_EQ("GF(2)", constraint.type_label());
        EXPECT_EQ(&y(0), constraint.key_variable_ptr());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(x.sum() - 1 - 2 * y,
                         model_component::ConstraintSense::Equal);
        constraint.update_structure();
        constraint.update_constraint_type();

        EXPECT_TRUE(constraint.is_type(model_component::ConstraintType::GF2));
        EXPECT_EQ(model_component::ConstraintType::GF2, constraint.type());
        EXPECT_EQ("GF(2)", constraint.type_label());
        EXPECT_EQ(&y(0), constraint.key_variable_ptr());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-x.sum() + 1 + 2 * y,
                         model_component::ConstraintSense::Equal);
        constraint.update_structure();
        constraint.update_constraint_type();

        EXPECT_TRUE(constraint.is_type(model_component::ConstraintType::GF2));
        EXPECT_EQ(model_component::ConstraintType::GF2, constraint.type());
        EXPECT_EQ("GF(2)", constraint.type_label());
        EXPECT_EQ(&y(0), constraint.key_variable_ptr());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(x.sum() + 2 - 2 * y,
                         model_component::ConstraintSense::Equal);
        constraint.update_structure();
        constraint.update_constraint_type();

        EXPECT_FALSE(constraint.is_type(model_component::ConstraintType::GF2));
        EXPECT_EQ(nullptr, constraint.key_variable_ptr());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-x.sum() - 2 + 2 * y,
                         model_component::ConstraintSense::Equal);
        constraint.update_structure();
        constraint.update_constraint_type();

        EXPECT_FALSE(constraint.is_type(model_component::ConstraintType::GF2));
        EXPECT_EQ(nullptr, constraint.key_variable_ptr());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(x.sum() - 2 - 2 * y,
                         model_component::ConstraintSense::Equal);
        constraint.update_structure();
        constraint.update_constraint_type();

        EXPECT_FALSE(constraint.is_type(model_component::ConstraintType::GF2));
        EXPECT_EQ(nullptr, constraint.key_variable_ptr());
    }

    {
        auto constraint =
            model_component::Constraint<int, double>::create_instance();
        constraint.setup(-x.sum() + 2 + 2 * y,
                         model_component::ConstraintSense::Equal);
        constraint.update_structure();
        constraint.update_constraint_type();

        EXPECT_FALSE(constraint.is_type(model_component::ConstraintType::GF2));
        EXPECT_EQ(nullptr, constraint.key_variable_ptr());
    }
}

/*****************************************************************************/
TEST_F(TestConstraintTypeClassifier, check_general_linear) {
    model::Model<int, double> model;
    auto                      coefficients = utility::sequence(10);

    auto& x = model.create_variable("x", 0, 1);
    auto& y = model.create_variables("y", 10, -10, 10);

    auto constraint =
        model_component::Constraint<int, double>::create_instance();
    constraint.setup(2 * x + 2 * y.sum() - 50,
                     model_component::ConstraintSense::Equal);
    constraint.update_structure();
    constraint.update_constraint_type();

    EXPECT_TRUE(
        constraint.is_type(model_component::ConstraintType::GeneralLinear));
    EXPECT_EQ(model_component::ConstraintType::GeneralLinear,
              constraint.type());
    EXPECT_EQ("General Linear", constraint.type_label());
    EXPECT_EQ(nullptr, constraint.key_variable_ptr());
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/