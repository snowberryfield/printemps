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
class TestPBConstraint : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestPBConstraint, initialize) {
    pb::PBConstraint constraint;
    EXPECT_EQ(pb::PBConstraintSense::Less, constraint.sense);
    EXPECT_EQ("", constraint.name);
    EXPECT_TRUE(constraint.terms.empty());
    EXPECT_EQ(0.0, constraint.rhs);
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/