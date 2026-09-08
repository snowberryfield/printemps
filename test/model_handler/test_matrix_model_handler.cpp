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
class TestMatrixModelHandler : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestMatrixModelHandler, export_matrix_model) {
    /// This test is covered by test_expression.h
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
    model.builder().build(option, false);

    auto mm_instance = model.matrix_model_handler().export_matrix_model();

    EXPECT_EQ(3, mm_instance.number_of_constraints);
    EXPECT_EQ(4, mm_instance.number_of_variables);
    EXPECT_FLOAT_EQ(0.0, mm_instance.objective_offset);
    EXPECT_FLOAT_EQ(1.0, mm_instance.objective_coefficients[0]);
    EXPECT_FLOAT_EQ(1.0, mm_instance.objective_coefficients[1]);
    EXPECT_FLOAT_EQ(1.0, mm_instance.objective_coefficients[2]);
    EXPECT_FLOAT_EQ(1.0, mm_instance.objective_coefficients[3]);

    EXPECT_FLOAT_EQ(printemps::constant::INT_HALF_MIN,
                    mm_instance.variable_lower_bounds[0]);
    EXPECT_FLOAT_EQ(-10, mm_instance.variable_lower_bounds[1]);
    EXPECT_FLOAT_EQ(-10, mm_instance.variable_lower_bounds[2]);
    EXPECT_FLOAT_EQ(-10, mm_instance.variable_lower_bounds[3]);

    EXPECT_FLOAT_EQ(printemps::constant::INT_HALF_MAX,
                    mm_instance.variable_upper_bounds[0]);
    EXPECT_FLOAT_EQ(10, mm_instance.variable_upper_bounds[1]);
    EXPECT_FLOAT_EQ(10, mm_instance.variable_upper_bounds[2]);
    EXPECT_FLOAT_EQ(10, mm_instance.variable_upper_bounds[3]);

    EXPECT_FLOAT_EQ(10, mm_instance.variable_initial_solution[0]);
    EXPECT_FLOAT_EQ(0, mm_instance.variable_initial_solution[1]);
    EXPECT_FLOAT_EQ(0, mm_instance.variable_initial_solution[2]);
    EXPECT_FLOAT_EQ(0, mm_instance.variable_initial_solution[3]);

    EXPECT_FLOAT_EQ(-4, mm_instance.constraint_coefficients.values[0]);
    EXPECT_FLOAT_EQ(-5, mm_instance.constraint_coefficients.values[1]);
    EXPECT_FLOAT_EQ(2, mm_instance.constraint_coefficients.values[2]);
    EXPECT_FLOAT_EQ(3, mm_instance.constraint_coefficients.values[3]);
    EXPECT_FLOAT_EQ(6, mm_instance.constraint_coefficients.values[4]);
    EXPECT_FLOAT_EQ(7, mm_instance.constraint_coefficients.values[5]);

    EXPECT_EQ(1, mm_instance.is_variable_lower_unbounded[0]);
    EXPECT_EQ(0, mm_instance.is_variable_lower_unbounded[3]);
    EXPECT_EQ(1, mm_instance.is_variable_upper_unbounded[0]);
    EXPECT_EQ(0, mm_instance.is_variable_upper_unbounded[3]);

    EXPECT_FLOAT_EQ(-20.0, mm_instance.dual_objective_coefficients[0]);
    EXPECT_FLOAT_EQ(10.0, mm_instance.dual_objective_coefficients[1]);
    EXPECT_FLOAT_EQ(30.0, mm_instance.dual_objective_coefficients[2]);

    EXPECT_FLOAT_EQ(0, mm_instance.dual_lower_bounds[0]);
    EXPECT_FLOAT_EQ(std::numeric_limits<double>::lowest(),
                    mm_instance.dual_lower_bounds[1]);
    EXPECT_FLOAT_EQ(0, mm_instance.dual_lower_bounds[2]);

    EXPECT_FLOAT_EQ(std::numeric_limits<double>::max(),
                    mm_instance.dual_upper_bounds[0]);
    EXPECT_FLOAT_EQ(std::numeric_limits<double>::max(),
                    mm_instance.dual_upper_bounds[1]);
    EXPECT_FLOAT_EQ(std::numeric_limits<double>::max(),
                    mm_instance.dual_upper_bounds[2]);

    EXPECT_FLOAT_EQ(0, mm_instance.dual_initial_solution[0]);
    EXPECT_FLOAT_EQ(0, mm_instance.dual_initial_solution[1]);
    EXPECT_FLOAT_EQ(0, mm_instance.dual_initial_solution[2]);
}

/*****************************************************************************/
TEST_F(TestMatrixModelHandler, import_solution_exceptions) {
    model::Model<int, double> model;

    auto& x = model.create_variables("x", 4, -10, 10);
    auto& f = model.create_constraints("f", 3);

    f(0) = 2 * x[0] + 3 * x[1] == 10;
    f(1) = 4 * x[1] + 5 * x[2] <= 20;
    f(2) = 6 * x[2] + 7 * x[3] >= 30;

    option::Option option;
    option.preprocess.is_enabled_presolve = false;
    model.builder().build(option, false);

    // Case 1: Calling import_solution before export_matrix_model should throw std::runtime_error
    std::vector<int> solution = {1, 2, 3, 4};
    EXPECT_THROW(model.matrix_model_handler().import_solution(solution), std::runtime_error);

    // Case 2: After export, calling with mismatched size should throw std::runtime_error
    auto mm_instance = model.matrix_model_handler().export_matrix_model();
    std::vector<int> mismatched_solution = {1, 2, 3};
    EXPECT_THROW(model.matrix_model_handler().import_solution(mismatched_solution), std::runtime_error);

    // Case 3: After export, calling with correct size should not throw and should update values
    std::vector<int> correct_solution = {2, 3, 4, 5};
    EXPECT_NO_THROW(model.matrix_model_handler().import_solution(correct_solution));
    EXPECT_EQ(2, x[0].value());
    EXPECT_EQ(3, x[1].value());
    EXPECT_EQ(4, x[2].value());
    EXPECT_EQ(5, x[3].value());
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/
