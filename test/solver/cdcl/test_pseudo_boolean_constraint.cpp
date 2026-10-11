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
class TestPseudoBooleanConstraint : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown(void) {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestPseudoBooleanConstraint, initialize) {
    PseudoBooleanConstraint constraint;
    EXPECT_EQ(constraint.index, CDCLCoreConstant::NO_CONSTRAINT);
    EXPECT_EQ(constraint.rhs, 0);
    EXPECT_EQ(constraint.current_lhs_sum, 0);
    EXPECT_EQ(constraint.unassigned_max_contribution, 0);
    EXPECT_EQ(constraint.literal_block_distance, 0);
    EXPECT_TRUE(constraint.positive_terms.empty());
    EXPECT_TRUE(constraint.negative_terms.empty());
}

/*****************************************************************************/
TEST_F(TestPseudoBooleanConstraint, constructor) {
    // 2*x0 + 3*x1 + (~x2) >= 4
    // Positive terms: (0, 2), (1, 3)
    // Negative terms: (2, 1)
    // unassigned_max_contribution = 2 + 3 + 1 = 6
    std::vector<std::pair<int, int64_t>> positive_terms = {{0, 2}, {1, 3}};
    std::vector<std::pair<int, int64_t>> negative_terms = {{2, 1}};

    PseudoBooleanConstraint constraint(10, 4, 6, positive_terms,
                                       negative_terms);

    EXPECT_EQ(constraint.index, 10);
    EXPECT_EQ(constraint.rhs, 4);
    EXPECT_EQ(constraint.unassigned_max_contribution, 6);
    EXPECT_EQ(constraint.current_lhs_sum, 0);
    EXPECT_EQ(constraint.positive_variable_indices.size(), 2u);
    EXPECT_EQ(constraint.negative_variable_indices.size(), 1u);
}

/*****************************************************************************/
TEST_F(TestPseudoBooleanConstraint, is_satisfied) {
    std::vector<std::pair<int, int64_t>> positive_terms = {{0, 3}};
    std::vector<std::pair<int, int64_t>> negative_terms = {};

    PseudoBooleanConstraint constraint(0, 3, 3, positive_terms, negative_terms);
    EXPECT_FALSE(constraint.is_satisfied());
    EXPECT_EQ(constraint.max_possible_lhs_sum(), 3);

    // Assign x0 = 1 (contributes to LHS, coefficient 3)
    constraint.on_variable_assigned(true, 3);
    EXPECT_TRUE(constraint.is_satisfied());
    EXPECT_EQ(constraint.current_lhs_sum, 3);
    EXPECT_EQ(constraint.unassigned_max_contribution, 0);
    EXPECT_EQ(constraint.max_possible_lhs_sum(), 3);
}

/*****************************************************************************/
TEST_F(TestPseudoBooleanConstraint, max_possible_lhs_sum) {
    /// This method is tested in is_satisfied().
}

/*****************************************************************************/
TEST_F(TestPseudoBooleanConstraint, on_variable_assigned) {
    std::vector<std::pair<int, int64_t>> positive_terms = {{0, 5}};
    std::vector<std::pair<int, int64_t>> negative_terms = {};

    PseudoBooleanConstraint constraint(0, 4, 5, positive_terms, negative_terms);

    // Assign x0 = 0 (does not contribute to LHS, coefficient 5)
    constraint.on_variable_assigned(false, 5);
    EXPECT_EQ(constraint.current_lhs_sum, 0);
    EXPECT_EQ(constraint.unassigned_max_contribution, 0);
    EXPECT_FALSE(constraint.is_satisfied());

    // Backtrack / remove assignment
    constraint.on_variable_removed(false, 5);
    EXPECT_EQ(constraint.current_lhs_sum, 0);
    EXPECT_EQ(constraint.unassigned_max_contribution, 5);
    EXPECT_EQ(constraint.max_possible_lhs_sum(), 5);
}

/*****************************************************************************/
TEST_F(TestPseudoBooleanConstraint, on_variable_removed) {
    /// This method is tested in on_variable_assigned().
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/