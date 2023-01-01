/*****************************************************************************/
// Copyright (c) 2020-2023 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <gtest/gtest.h>
#include <printemps.h>

namespace {
using namespace printemps;
/*****************************************************************************/
class TestPreprocessOption : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestPreprocessOption, initialize) {
    using namespace printemps::option;
    PreprocessOption option;
    EXPECT_EQ(                                                  //
        PreprocessOptionConstant::DEFAULT_IS_ENABLED_PRESOLVE,  //
        option.is_enabled_presolve);

    EXPECT_EQ(  //
        PreprocessOptionConstant::
            DEFAULT_IS_ENABLED_REMOVE_DUPLICATED_CONSTRAINTS,  //
        option.is_enabled_remove_duplicated_constraints);

    EXPECT_EQ(  //
        PreprocessOptionConstant::
            DEFAULT_IS_ENABLED_REMOVE_REDUNDANT_SET_VARIABLES,  //
        option.is_enabled_remove_redundant_set_variables);

    EXPECT_EQ(  //
        PreprocessOptionConstant::
            DEFAULT_IS_ENABLED_REMOVE_REDUNDANT_SET_CONSTRAINTS,  //
        option.is_enabled_remove_redundant_set_constraints);

    EXPECT_EQ(  //
        PreprocessOptionConstant::
            DEFAULT_IS_ENABLED_EXTRACT_IMPLICIT_EQUALITY_CONSTRAINTS,  //
        option.is_enabled_extract_implicit_equality_constraints);

    EXPECT_EQ(                                                         //
        PreprocessOptionConstant::DEFAULT_IS_ENABLED_ONLINE_BOUNDING,  //
        option.is_enabled_online_bounding);

    EXPECT_EQ(  //
        PreprocessOptionConstant::
            DEFAULT_IS_ENABLED_INITIAL_VALUE_CORRECTION,  //
        option.is_enabled_initial_value_correction);

    EXPECT_EQ(  //
        PreprocessOptionConstant::
            DEFAULT_IS_ENABLED_EXTRACT_DEPENDENT_EXCLUSIVE_OR,  //
        option.is_enabled_extract_dependent_exclusive_or);

    EXPECT_EQ(  //
        PreprocessOptionConstant::
            DEFAULT_IS_ENABLED_EXTRACT_DEPENDENT_EXCLUSIVE_NOR,  //
        option.is_enabled_extract_dependent_exclusive_nor);

    EXPECT_EQ(  //
        PreprocessOptionConstant::
            DEFAULT_IS_ENABLED_EXTRACT_DEPENDENT_INVERTED_INTEGERS,  //
        option.is_enabled_extract_dependent_inverted_integers);

    EXPECT_EQ(  //
        PreprocessOptionConstant::
            DEFAULT_IS_ENABLED_EXTRACT_DEPENDENT_BALANCED_INTEGERS,  //
        option.is_enabled_extract_dependent_balanced_integers);

    EXPECT_EQ(  //
        PreprocessOptionConstant::
            DEFAULT_IS_ENABLED_EXTRACT_DEPENDENT_CONSTANT_SUM_INTEGERS,  //
        option.is_enabled_extract_dependent_constant_sum_integers);

    EXPECT_EQ(  //
        PreprocessOptionConstant::
            DEFAULT_IS_ENABLED_EXTRACT_DEPENDENT_CONSTANT_DIFFERENCE_INTEGERS,  //
        option.is_enabled_extract_dependent_constant_difference_integers);

    EXPECT_EQ(  //
        PreprocessOptionConstant::
            DEFAULT_IS_ENABLED_EXTRACT_DEPENDENT_CONSTANT_RATIO_INTEGERS,  //
        option.is_enabled_extract_dependent_constant_ratio_integers);

    EXPECT_EQ(  //
        PreprocessOptionConstant::
            DEFAULT_IS_ENABLED_EXTRACT_DEPENDENT_INTERMEDIATE,  //
        option.is_enabled_extract_dependent_intermediate);
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/