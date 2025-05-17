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
class TestOPBConstraint : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestOPBConstraint, initialize) {
    opb::OPBConstraint constraint;
    EXPECT_EQ(opb::OPBConstraintSense::Less, constraint.sense);
    EXPECT_EQ("", constraint.name);
    EXPECT_TRUE(constraint.terms.empty());
    EXPECT_EQ(0.0, constraint.rhs);
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/