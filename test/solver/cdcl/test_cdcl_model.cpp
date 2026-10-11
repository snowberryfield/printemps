/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
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
TEST_F(TestCDCLModel, initialize) {
    CDCLModel model;

    EXPECT_EQ(model.number_of_variables(), 0);
    EXPECT_EQ(model.number_of_constraints(), 0);
    EXPECT_EQ(model.number_of_clauses(), 0);
    EXPECT_EQ(model.number_of_pseudo_boolean_constraints(), 0);
    EXPECT_EQ(model.number_of_cardinality_constraints(), 0);
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
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/