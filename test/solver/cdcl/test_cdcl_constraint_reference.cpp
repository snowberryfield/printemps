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
class TestCDCLConstraintReference : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown(void) {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestCDCLConstraintReference, constructor) {
    CDCLConstraintReference default_reference;
    EXPECT_EQ(default_reference.index, CDCLCoreConstant::NO_CONSTRAINT);
    EXPECT_EQ(default_reference.type, CDCLConstraintType::NONE);

    CDCLConstraintReference custom_reference(CDCLConstraintType::LEARNED, 42);
    EXPECT_EQ(custom_reference.index, 42);
    EXPECT_EQ(custom_reference.type, CDCLConstraintType::LEARNED);
}

/*****************************************************************************/
TEST_F(TestCDCLConstraintReference, operator_equal) {
    CDCLConstraintReference reference1(CDCLConstraintType::LEARNED, 10);
    CDCLConstraintReference reference2(CDCLConstraintType::LEARNED, 10);
    CDCLConstraintReference reference3(CDCLConstraintType::CARDINALITY, 10);
    CDCLConstraintReference reference4(CDCLConstraintType::LEARNED, 20);

    EXPECT_TRUE(reference1 == reference2);
    EXPECT_FALSE(reference1 != reference2);

    EXPECT_FALSE(reference1 == reference3);
    EXPECT_TRUE(reference1 != reference3);

    EXPECT_FALSE(reference1 == reference4);
    EXPECT_TRUE(reference1 != reference4);
}

/*****************************************************************************/
TEST_F(TestCDCLConstraintReference, operator_not_equal) {
    /// This method is tested in operator_equal().
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/