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
class TestModelLinearProgrammingHandler : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestModelLinearProgrammingHandler, export_lp_instance) {
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

    auto lp_instance = model.linear_programming_handler().export_lp_instance();

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

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/