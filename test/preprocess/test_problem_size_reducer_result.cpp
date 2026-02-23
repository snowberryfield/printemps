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
class ProblemSizeReducerResult : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(ProblemSizeReducerResult, initialize) {
    preprocess::ProblemSizeReducerResult result(1, 1, 1);
    result.initialize();

    EXPECT_EQ(0, result.number_of_fixed_variables);
    EXPECT_EQ(0, result.number_of_disabled_constraints);
    EXPECT_EQ(0, result.number_of_updated_variable_bounds);
}

/*****************************************************************************/
TEST_F(ProblemSizeReducerResult, reset) {
    preprocess::ProblemSizeReducerResult result(1, 1, 1);
    result.reset();
    EXPECT_EQ(0, result.number_of_fixed_variables);
    EXPECT_EQ(0, result.number_of_disabled_constraints);
    EXPECT_EQ(0, result.number_of_updated_variable_bounds);
}

/*****************************************************************************/
TEST_F(ProblemSizeReducerResult, operator_plus_equal) {
    preprocess::ProblemSizeReducerResult result_0(1, 2, 3);
    preprocess::ProblemSizeReducerResult result_1(4, 5, 6);
    result_0 += result_1;
    EXPECT_EQ(5, result_0.number_of_fixed_variables);
    EXPECT_EQ(7, result_0.number_of_disabled_constraints);
    EXPECT_EQ(9, result_0.number_of_updated_variable_bounds);
}

/*****************************************************************************/
TEST_F(ProblemSizeReducerResult, is_reduced) {
    preprocess::ProblemSizeReducerResult result;
    EXPECT_FALSE(result.is_reduced());
    result.number_of_fixed_variables = 1;
    EXPECT_TRUE(result.is_reduced());
    result.number_of_fixed_variables      = 0;
    result.number_of_disabled_constraints = 1;
    EXPECT_TRUE(result.is_reduced());
    result.number_of_disabled_constraints    = 0;
    result.number_of_updated_variable_bounds = 1;
    EXPECT_TRUE(result.is_reduced());
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/