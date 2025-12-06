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
class TestReference : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestReference, update_variable_reference) {
    model::Model<int, double> model;

    auto& x = model.create_variable("x", 0, 1);
    auto& y = model.create_variables("y", 10, 0, 1);
    auto& z = model.create_variables("z", {20, 30}, -10, 10);
    auto& w = model.create_variable("w", -100, 100);

    x.fix_by(0);
    y(0).fix_by(0);
    z(0, 0).fix_by(0);
    model.create_constraint("f", y.selection());
    model.create_constraint("g", w == 2 * x + 3 * z(0, 0) + 5 * z(0, 1));

    model.builder().setup_structure();

    preprocess::DependentVariableExtractor<int, double>
                   dependent_variable_extractor(&model);
    option::Option option;
    dependent_variable_extractor.extract(option, false);
    preprocess::SelectionExtractor<int, double> selection_extractor(&model);
    selection_extractor.extract_by_independent(false);

    model.builder().setup_structure();

    EXPECT_EQ(1 + 10 + 20 * 30 + 1, model.reference().number_of_variables());
    EXPECT_EQ(3, model.reference().number_of_fixed_variables());
    EXPECT_EQ(1 + 10 + 20 * 30 + 1 - 3,
              model.reference().number_of_mutable_variables());
    EXPECT_EQ(1, model.reference().number_of_binary_variables());
    EXPECT_EQ(600, model.reference().number_of_integer_variables());
    EXPECT_EQ(10, model.reference().number_of_selection_variables());
    EXPECT_EQ(1, model.reference().number_of_dependent_integer_variables());
}

/*****************************************************************************/
TEST_F(TestReference, update_constraint_reference) {
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

    model.builder().setup_structure();
    preprocess::SelectionExtractor<int, double> selection_extractor(&model);
    selection_extractor.extract_by_defined_order(false);
    model.builder().setup_structure();

    EXPECT_EQ(27, model.reference().number_of_constraints());
    EXPECT_EQ(1, static_cast<int>(model.selections().size()));
    EXPECT_EQ(25, model.reference().number_of_enabled_constraints());
    EXPECT_EQ(2, model.reference().number_of_disabled_constraints());

    auto reference = model.reference().constraint_type;
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
TEST_F(TestReference, update_variable_name_map) {
    model::Model<int, double> model;

    auto& x = model.create_variable("x");
    auto& y = model.create_variable("y");

    model.builder().setup_unique_names();
    model.builder().setup_structure();  // include update_variable_name_map()

    EXPECT_EQ(&x(0), model.reference().variable_name_map.at("x"));
    EXPECT_EQ(&y(0), model.reference().variable_name_map.at("y"));
}

/*****************************************************************************/
TEST_F(TestReference, update_contraint_name_map) {
    model::Model<int, double> model;

    auto& c_1 = model.create_constraint("c_1");
    auto& c_2 = model.create_constraint("c_2");

    model.builder().setup_unique_names();
    model.builder().setup_structure();  // include update_contraint_name_map()

    EXPECT_EQ(&c_1(0), model.reference().constraint_name_map.at("c_1"));
    EXPECT_EQ(&c_2(0), model.reference().constraint_name_map.at("c_2"));
}

/*****************************************************************************/
TEST_F(TestReference, number_of_variables) {
    /// This test is covered by TestReference.update_variable_reference.
}

/*****************************************************************************/
TEST_F(TestReference, number_of_fixed_variables) {
    /// This test is covered by TestReference.update_variable_reference.
}

/*****************************************************************************/
TEST_F(TestReference, number_of_mutable_variables) {
    /// This test is covered by TestReference.update_variable_reference.
}

/*****************************************************************************/
TEST_F(TestReference, number_of_binary_variables) {
    /// This test is covered by TestReference.update_variable_reference.
}

/*****************************************************************************/
TEST_F(TestReference, number_of_integer_variables) {
    /// This test is covered by TestReference.update_variable_reference.
}

/*****************************************************************************/
TEST_F(TestReference, number_of_selection_variables) {
    /// This test is covered by TestReference.update_variable_reference.
}

/*****************************************************************************/
TEST_F(TestReference, number_of_dependent_binary_variables) {
    /// This test is covered by TestReference.update_variable_reference.
}

/*****************************************************************************/
TEST_F(TestReference, number_of_dependent_integer_variables) {
    /// This test is covered by TestReference.update_variable_reference.
}

/*****************************************************************************/
TEST_F(TestReference, number_of_constraints) {
    /// This test is covered by TestReference.update_constraint_reference.
}

/*****************************************************************************/
TEST_F(TestReference, number_of_enabled_constraints) {
    /// This test is covered by TestReference.update_constraint_reference.
}

/*****************************************************************************/
TEST_F(TestReference, number_of_disabled_constraints) {
    /// This test is covered by TestReference.update_constraint_reference.
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/