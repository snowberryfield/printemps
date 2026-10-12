/*****************************************************************************/
// Copyright (c) 2020-2026 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <gtest/gtest.h>
#include <printemps.h>

namespace {
using namespace printemps;
using namespace printemps::solver::cdcl::core;

/*****************************************************************************/
class TestCDCLModel : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown(void) {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestCDCLModel, constructor) {
    CDCLModel model;
    EXPECT_EQ(model.number_of_variables(), 0);
    EXPECT_EQ(model.number_of_constraints(), 0);
}

/*****************************************************************************/
TEST_F(TestCDCLModel, initialize) {
    CDCLModel model;

    EXPECT_EQ(model.number_of_variables(), 0);
    EXPECT_EQ(model.number_of_constraints(), 0);
    EXPECT_EQ(model.number_of_clauses(), 0);
    EXPECT_EQ(model.number_of_initial_clauses(), 0);
    EXPECT_EQ(model.number_of_learned_clauses(), 0);
    EXPECT_EQ(model.number_of_pseudo_boolean_constraints(), 0);
    EXPECT_EQ(model.number_of_cardinality_constraints(), 0);
    EXPECT_EQ(model.number_of_learned_constraints(), 0);
    EXPECT_TRUE(model.variables().empty());
    EXPECT_TRUE(model.initial_activities().empty());
    EXPECT_TRUE(model.selection_variable_indices().empty());
}

/*****************************************************************************/
TEST_F(TestCDCLModel, setup) {
    // 2 variables: x0 + x1 >= 1
    matrix_model::MatrixModel matrix_model;
    matrix_model.number_of_variables   = 2;
    matrix_model.number_of_constraints = 1;

    matrix_model.constraint_coefficients.row_ptrs       = {0, 2};
    matrix_model.constraint_coefficients.column_indices = {0, 1};
    matrix_model.constraint_coefficients.values         = {1.0, 1.0};
    matrix_model.constraint_lower_bounds.values.push_back(1.0);
    matrix_model.constraint_upper_bounds.values.push_back(2.0);

    for (int i = 0; i < 2; i++) {
        matrix_model.variable_lower_bounds.values.push_back(0.0);
        matrix_model.variable_upper_bounds.values.push_back(1.0);
    }

    CDCLModel model;
    model.setup(&matrix_model);

    EXPECT_EQ(model.number_of_variables(), 2);
    EXPECT_GE(model.number_of_constraints(), 1);
    EXPECT_EQ(static_cast<int>(model.variables().size()), 2);
    EXPECT_EQ(model.variable(0).index, 0);
    EXPECT_EQ(model.variable(1).index, 1);
}

/*****************************************************************************/
TEST_F(TestCDCLModel, setup_variables) {
    /// This method is tested in setup().
}

/*****************************************************************************/
TEST_F(TestCDCLModel, setup_selections) {
    /// This method is tested in setup().
}

/*****************************************************************************/
TEST_F(TestCDCLModel, setup_constraints) {
    /// This method is tested in setup().
}

/*****************************************************************************/
TEST_F(TestCDCLModel, setup_initial_propagation_queue) {
    /// This method is tested in setup().
}

/*****************************************************************************/
TEST_F(TestCDCLModel, setup_initial_activities) {
    /// This method is tested in setup().
}

/*****************************************************************************/
TEST_F(TestCDCLModel, setup_initial_phases) {
    /// This method is tested in setup().
}

/*****************************************************************************/
TEST_F(TestCDCLModel, assign) {
    matrix_model::MatrixModel matrix_model;
    matrix_model.number_of_variables   = 2;
    matrix_model.number_of_constraints = 1;

    matrix_model.constraint_coefficients.row_ptrs       = {0, 2};
    matrix_model.constraint_coefficients.column_indices = {0, 1};
    matrix_model.constraint_coefficients.values         = {1.0, 1.0};
    matrix_model.constraint_lower_bounds.values.push_back(1.0);
    matrix_model.constraint_upper_bounds.values.push_back(2.0);

    for (int i = 0; i < 2; i++) {
        matrix_model.variable_lower_bounds.values.push_back(0.0);
        matrix_model.variable_upper_bounds.values.push_back(1.0);
    }

    CDCLModel model;
    model.setup(&matrix_model);

    CDCLConstraintReference reason;
    model.assign(0, 1, 1, reason);

    EXPECT_TRUE(model.variable(0).is_assigned());
    EXPECT_EQ(model.variable(0).value, 1);

    model.rollback(0);
    EXPECT_FALSE(model.variable(0).is_assigned());
}

/*****************************************************************************/
TEST_F(TestCDCLModel, rollback) {
    /// This method is tested in assign().
}

/*****************************************************************************/
TEST_F(TestCDCLModel, propagate) {
    /// This method is tested in test_cdcl_behavior.cpp.
}

/*****************************************************************************/
TEST_F(TestCDCLModel, add_learned_constraint) {
    /// This method is tested in test_cdcl_behavior.cpp.
}

/*****************************************************************************/
TEST_F(TestCDCLModel, reduce_learned_constraints) {
    /// This method is tested in test_cdcl_behavior.cpp.
}

/*****************************************************************************/
TEST_F(TestCDCLModel, clear_propagation_queue) {
    CDCLModel model;
    EXPECT_NO_THROW(model.clear_propagation_queue());
}

/*****************************************************************************/
TEST_F(TestCDCLModel, positive_variable_indices) {
    matrix_model::MatrixModel matrix_model;
    matrix_model.number_of_variables   = 2;
    matrix_model.number_of_constraints = 1;

    matrix_model.constraint_coefficients.row_ptrs       = {0, 2};
    matrix_model.constraint_coefficients.column_indices = {0, 1};
    matrix_model.constraint_coefficients.values         = {1.0, 1.0};
    matrix_model.constraint_lower_bounds.values.push_back(1.0);
    matrix_model.constraint_upper_bounds.values.push_back(2.0);

    for (int i = 0; i < 2; i++) {
        matrix_model.variable_lower_bounds.values.push_back(0.0);
        matrix_model.variable_upper_bounds.values.push_back(1.0);
    }

    CDCLModel model;
    model.setup(&matrix_model);

    CDCLConstraintReference reference(CDCLConstraintType::CARDINALITY, 0);
    EXPECT_EQ(
        static_cast<int>(model.positive_variable_indices(reference).size()), 2);
    EXPECT_TRUE(model.negative_variable_indices(reference).empty());
}

/*****************************************************************************/
TEST_F(TestCDCLModel, negative_variable_indices) {
    /// This method is tested in positive_variable_indices().
}

/*****************************************************************************/
TEST_F(TestCDCLModel, clause) {
    /// This method is tested in test_cdcl_behavior.cpp.
}

/*****************************************************************************/
TEST_F(TestCDCLModel, learned_clause) {
    /// This method is tested in test_cdcl_behavior.cpp.
}

/*****************************************************************************/
TEST_F(TestCDCLModel, variables) {
    matrix_model::MatrixModel matrix_model;
    matrix_model.number_of_variables   = 2;
    matrix_model.number_of_constraints = 1;

    matrix_model.constraint_coefficients.row_ptrs       = {0, 2};
    matrix_model.constraint_coefficients.column_indices = {0, 1};
    matrix_model.constraint_coefficients.values         = {1.0, 1.0};
    matrix_model.constraint_lower_bounds.values.push_back(1.0);
    matrix_model.constraint_upper_bounds.values.push_back(2.0);

    for (int i = 0; i < 2; i++) {
        matrix_model.variable_lower_bounds.values.push_back(0.0);
        matrix_model.variable_upper_bounds.values.push_back(1.0);
    }

    CDCLModel model;
    model.setup(&matrix_model);

    EXPECT_EQ(static_cast<int>(model.variables().size()), 2);
    EXPECT_EQ(model.variable(0).index, 0);
    model.mutable_variable(0).value = 1;
    EXPECT_EQ(model.variable(0).value, 1);
}

/*****************************************************************************/
TEST_F(TestCDCLModel, variable) {
    /// This method is tested in variables().
}

/*****************************************************************************/
TEST_F(TestCDCLModel, mutable_variable) {
    /// This method is tested in variables().
}

/*****************************************************************************/
TEST_F(TestCDCLModel, initial_activities) {
    matrix_model::MatrixModel matrix_model;
    matrix_model.number_of_variables   = 2;
    matrix_model.number_of_constraints = 1;

    matrix_model.constraint_coefficients.row_ptrs       = {0, 2};
    matrix_model.constraint_coefficients.column_indices = {0, 1};
    matrix_model.constraint_coefficients.values         = {1.0, 1.0};
    matrix_model.constraint_lower_bounds.values.push_back(1.0);
    matrix_model.constraint_upper_bounds.values.push_back(2.0);

    for (int i = 0; i < 2; i++) {
        matrix_model.variable_lower_bounds.values.push_back(0.0);
        matrix_model.variable_upper_bounds.values.push_back(1.0);
    }

    CDCLModel model;
    model.setup(&matrix_model);

    EXPECT_EQ(static_cast<int>(model.initial_activities().size()), 2);
}

/*****************************************************************************/
TEST_F(TestCDCLModel, number_of_variables) {
    /// This method is tested in initialize() and setup().
}

/*****************************************************************************/
TEST_F(TestCDCLModel, number_of_clauses) {
    /// This method is tested in initialize().
}

/*****************************************************************************/
TEST_F(TestCDCLModel, number_of_initial_clauses) {
    /// This method is tested in initialize().
}

/*****************************************************************************/
TEST_F(TestCDCLModel, number_of_learned_clauses) {
    /// This method is tested in initialize().
}

/*****************************************************************************/
TEST_F(TestCDCLModel, number_of_pseudo_boolean_constraints) {
    /// This method is tested in initialize().
}

/*****************************************************************************/
TEST_F(TestCDCLModel, number_of_cardinality_constraints) {
    /// This method is tested in initialize().
}

/*****************************************************************************/
TEST_F(TestCDCLModel, number_of_learned_constraints) {
    /// This method is tested in initialize().
}

/*****************************************************************************/
TEST_F(TestCDCLModel, number_of_constraints) {
    /// This method is tested in initialize() and setup().
}

/*****************************************************************************/
TEST_F(TestCDCLModel, is_selection_variable) {
    CDCLModel model;
    EXPECT_FALSE(model.is_selection_variable(0));
}

/*****************************************************************************/
TEST_F(TestCDCLModel, selection_variable_indices) {
    CDCLModel model;
    EXPECT_TRUE(model.selection_variable_indices().empty());
}

/*****************************************************************************/
TEST_F(TestCDCLModel, extract_current_solution) {
    matrix_model::MatrixModel matrix_model;
    matrix_model.number_of_variables   = 2;
    matrix_model.number_of_constraints = 1;

    matrix_model.constraint_coefficients.row_ptrs       = {0, 2};
    matrix_model.constraint_coefficients.column_indices = {0, 1};
    matrix_model.constraint_coefficients.values         = {1.0, 1.0};
    matrix_model.constraint_lower_bounds.values.push_back(1.0);
    matrix_model.constraint_upper_bounds.values.push_back(2.0);

    for (int i = 0; i < 2; i++) {
        matrix_model.variable_lower_bounds.values.push_back(0.0);
        matrix_model.variable_upper_bounds.values.push_back(1.0);
    }

    CDCLModel model;
    model.setup(&matrix_model);

    CDCLConstraintReference reason;
    model.assign(0, 1, 1, reason);
    model.assign(1, 0, 1, reason);

    const auto solution = model.extract_current_solution();
    EXPECT_EQ(static_cast<int>(solution.size()), 2);
    EXPECT_EQ(solution[0], 1);
    EXPECT_EQ(solution[1], 0);

    EXPECT_DOUBLE_EQ(model.compute_total_violation(solution), 0.0);

    const std::vector<int> infeasible_solution = {0, 0};
    EXPECT_DOUBLE_EQ(model.compute_total_violation(infeasible_solution), 1.0);
}

/*****************************************************************************/
TEST_F(TestCDCLModel, compute_total_violation) {
    /// This method is tested in extract_current_solution().
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/