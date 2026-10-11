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
class TestCardinalityConstraint : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown(void) {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestCardinalityConstraint, initialize) {
    CardinalityConstraint constraint;
    EXPECT_EQ(constraint.index, CDCLCoreConstant::NO_CONSTRAINT);
    EXPECT_EQ(constraint.rhs, 0);
    EXPECT_EQ(constraint.current_lhs_sum, 0);
    EXPECT_EQ(constraint.unassigned_count, 0);
    EXPECT_EQ(constraint.literal_block_distance, 0);
    EXPECT_TRUE(constraint.positive_variable_indices.empty());
    EXPECT_TRUE(constraint.negative_variable_indices.empty());
}

/*****************************************************************************/
TEST_F(TestCardinalityConstraint, constructor) {
    // x0 + x1 + (~x2) >= 2
    CardinalityConstraint constraint(1, 2, {0, 1}, {2});
    EXPECT_EQ(constraint.index, 1);
    EXPECT_EQ(constraint.rhs, 2);
    EXPECT_EQ(constraint.unassigned_count, 3);
    EXPECT_EQ(constraint.current_lhs_sum, 0);
    EXPECT_EQ(constraint.max_possible_lhs_sum(), 3);
    EXPECT_FALSE(constraint.is_satisfied());
}

/*****************************************************************************/
TEST_F(TestCardinalityConstraint, setup) {
    /// This method is tested in constructor.
}

/*****************************************************************************/
TEST_F(TestCardinalityConstraint, max_possible_lhs_sum) {
    /// This method is tested in constructor.
}

/*****************************************************************************/
TEST_F(TestCardinalityConstraint, is_satisfied) {
    /// This method is tested in on_variable_assigned().
}

/*****************************************************************************/
TEST_F(TestCardinalityConstraint, on_variable_assigned) {
    // x0 + x1 >= 2
    CardinalityConstraint constraint(0, 2, {0, 1}, {});
    EXPECT_FALSE(constraint.is_satisfied());

    // Assign x0 = 1 (contributes to LHS)
    constraint.on_variable_assigned(true);
    EXPECT_EQ(constraint.current_lhs_sum, 1);
    EXPECT_EQ(constraint.unassigned_count, 1);
    EXPECT_FALSE(constraint.is_satisfied());

    // Assign x1 = 1 (contributes to LHS)
    constraint.on_variable_assigned(true);
    EXPECT_EQ(constraint.current_lhs_sum, 2);
    EXPECT_EQ(constraint.unassigned_count, 0);
    EXPECT_TRUE(constraint.is_satisfied());

    // Backtrack x1
    constraint.on_variable_removed(true);
    EXPECT_EQ(constraint.current_lhs_sum, 1);
    EXPECT_EQ(constraint.unassigned_count, 1);
    EXPECT_FALSE(constraint.is_satisfied());
}

/*****************************************************************************/
TEST_F(TestCardinalityConstraint, on_variable_removed) {
    /// This method is tested in on_variable_assigned().
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/