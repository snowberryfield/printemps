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
class TestMPSVariable : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestMPSVariable, initialize) {
    mps::MPSVariable variable;
    EXPECT_EQ(mps::MPSVariableSense::Continuous, variable.sense);
    EXPECT_EQ("", variable.name);
    EXPECT_EQ(0, variable.integer_lower_bound);
    EXPECT_EQ(constant::INT_HALF_MAX, variable.integer_upper_bound);
    EXPECT_EQ(0, variable.integer_fixed_value);
    EXPECT_FLOAT_EQ(0.0, variable.continuous_lower_bound);
    EXPECT_FLOAT_EQ(HUGE_VAL, variable.continuous_upper_bound);
    EXPECT_FLOAT_EQ(0.0, variable.continuous_fixed_value);
    EXPECT_FALSE(variable.is_bound_defined);
    EXPECT_FALSE(variable.is_fixed);
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/