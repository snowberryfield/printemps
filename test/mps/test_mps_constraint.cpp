/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <gtest/gtest.h>
#include <printemps.h>

namespace {
using namespace printemps;
/*****************************************************************************/
class TestMPSConstraint : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestMPSConstraint, initialize) {
    mps::MPSConstraint constraint;
    EXPECT_EQ(mps::MPSConstraintSense::Less, constraint.sense);
    EXPECT_EQ("", constraint.name);
    EXPECT_TRUE(constraint.sensitivities.empty());
    EXPECT_FLOAT_EQ(0.0, constraint.rhs);
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/