/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <gtest/gtest.h>
#include <random>

#include <printemps.h>

namespace {
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
            DEFAULT_IS_ENABLED_INITIAL_VALUE_CORRECTION,  //
        option.is_enabled_initial_value_correction);
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/