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
class TestCDCLVariable : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown(void) {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestCDCLVariable, initialize) {
    CDCLVariable variable;

    EXPECT_EQ(variable.index, CDCLCoreConstant::NO_VARIABLE);
    EXPECT_EQ(variable.decision_level, CDCLCoreConstant::UNASSIGNED_LEVEL);
    EXPECT_EQ(variable.reason_constraint.index,
              CDCLCoreConstant::NO_CONSTRAINT);
    EXPECT_EQ(variable.reason_constraint.type, CDCLConstraintType::NONE);
    EXPECT_EQ(variable.value, CDCLCoreConstant::UNASSIGNED);
    EXPECT_EQ(variable.phase, CDCLCoreConstant::NO_PHASE);
    EXPECT_FALSE(variable.is_assigned());
}

/*****************************************************************************/
TEST_F(TestCDCLVariable, assign) {
    CDCLVariable variable;
    variable.index = 0;

    CDCLConstraintReference reason(CDCLConstraintType::LEARNED, 5);
    variable.assign(1, 2, reason);

    EXPECT_TRUE(variable.is_assigned());
    EXPECT_EQ(variable.value, 1);
    EXPECT_EQ(variable.decision_level, 2);
    EXPECT_EQ(variable.phase, 1);
    EXPECT_EQ(variable.reason_constraint, reason);
}

/*****************************************************************************/
TEST_F(TestCDCLVariable, is_assigned) {
    /// This method is tested in assign().
}

/*****************************************************************************/
TEST_F(TestCDCLVariable, remove) {
    CDCLVariable variable;
    variable.index = 0;

    CDCLConstraintReference reason(CDCLConstraintType::LEARNED, 5);
    variable.assign(1, 2, reason);
    EXPECT_TRUE(variable.is_assigned());

    variable.remove();

    EXPECT_FALSE(variable.is_assigned());
    EXPECT_EQ(variable.value, CDCLCoreConstant::UNASSIGNED);
    EXPECT_EQ(variable.decision_level, CDCLCoreConstant::UNASSIGNED_LEVEL);
    EXPECT_EQ(variable.reason_constraint.index,
              CDCLCoreConstant::NO_CONSTRAINT);
    EXPECT_EQ(variable.reason_constraint.type, CDCLConstraintType::NONE);
    // Phase should be preserved after remove() for phase saving heuristic
    EXPECT_EQ(variable.phase, 1);
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/