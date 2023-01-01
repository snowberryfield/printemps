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
class TestConstraintTypeReference : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestConstraintTypeReference, initialize) {
    model_component::ConstraintTypeReference<int, double> reference;

    EXPECT_TRUE(reference.singleton_ptrs.empty());
    EXPECT_TRUE(reference.exclusive_or_ptrs.empty());
    EXPECT_TRUE(reference.exclusive_nor_ptrs.empty());
    EXPECT_TRUE(reference.invariant_knapsack_ptrs.empty());
    EXPECT_TRUE(reference.balanced_integers_ptrs.empty());
    EXPECT_TRUE(reference.constant_sum_integers_ptrs.empty());
    EXPECT_TRUE(reference.constant_difference_integers_ptrs.empty());
    EXPECT_TRUE(reference.constant_ratio_integers_ptrs.empty());
    EXPECT_TRUE(reference.aggregation_ptrs.empty());
    EXPECT_TRUE(reference.precedence_ptrs.empty());

    EXPECT_TRUE(reference.variable_bound_ptrs.empty());
    EXPECT_TRUE(reference.trinomial_exclusive_nor_ptrs.empty());
    EXPECT_TRUE(reference.set_partitioning_ptrs.empty());
    EXPECT_TRUE(reference.set_packing_ptrs.empty());
    EXPECT_TRUE(reference.set_covering_ptrs.empty());
    EXPECT_TRUE(reference.cardinality_ptrs.empty());
    EXPECT_TRUE(reference.invariant_knapsack_ptrs.empty());
    EXPECT_TRUE(reference.multiple_covering_ptrs.empty());
    EXPECT_TRUE(reference.binary_flow_ptrs.empty());
    EXPECT_TRUE(reference.integer_flow_ptrs.empty());

    EXPECT_TRUE(reference.soft_selection_ptrs.empty());
    EXPECT_TRUE(reference.min_max_ptrs.empty());
    EXPECT_TRUE(reference.max_min_ptrs.empty());
    EXPECT_TRUE(reference.intermediate_ptrs.empty());

    EXPECT_TRUE(reference.equation_knapsack_ptrs.empty());
    EXPECT_TRUE(reference.bin_packing_ptrs.empty());
    EXPECT_TRUE(reference.knapsack_ptrs.empty());
    EXPECT_TRUE(reference.integer_knapsack_ptrs.empty());
    EXPECT_TRUE(reference.gf2_ptrs.empty());
    EXPECT_TRUE(reference.general_linear_ptrs.empty());
    EXPECT_TRUE(reference.nonlinear_ptrs.empty());
}
}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/