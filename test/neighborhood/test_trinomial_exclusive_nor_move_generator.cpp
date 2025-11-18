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
class TestTrinomialExclusiveNorMoveGenerator : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestTrinomialExclusiveNorMoveGenerator, setup) {
    model::Model<int, double> model;

    auto& x = model.create_variables("x", 3, 0, 1);
    auto& c = model.create_constraint("c", x[0] + x[1] == 2 * x[2]);

    model.builder().setup_unique_names();
    model.builder().setup_structure();

    auto& trinomial_exclusive_nor_ptrs =
        model.reference().constraint_type.trinomial_exclusive_nor_ptrs;

    model.neighborhood().trinomial_exclusive_nor().setup(
        trinomial_exclusive_nor_ptrs);
    model.neighborhood().trinomial_exclusive_nor().update_moves(  //
        true, false, false, false, 1);

    auto& moves = model.neighborhood().trinomial_exclusive_nor().moves();
    auto& flags = model.neighborhood().trinomial_exclusive_nor().flags();
    EXPECT_EQ(2, static_cast<int>(moves.size()));
    EXPECT_EQ(2, static_cast<int>(flags.size()));

    /// (x0,x1,x2) = (0,0,0)
    EXPECT_FALSE(moves[0].is_univariable_move);
    EXPECT_TRUE(moves[0].is_special_neighborhood_move);
    EXPECT_EQ(0, moves[0].overlap_rate);
    EXPECT_EQ(3, static_cast<int>(moves[0].alterations.size()));
    EXPECT_EQ(0, moves[0].alterations[0].second);
    EXPECT_EQ(0, moves[0].alterations[1].second);
    EXPECT_EQ(0, moves[0].alterations[2].second);
    EXPECT_EQ(neighborhood::MoveSense::TrinomialExclusiveNor, moves[0].sense);
    EXPECT_TRUE(std::find(moves[0].related_constraint_ptrs.begin(),
                          moves[0].related_constraint_ptrs.end(),
                          &c[0]) != moves[0].related_constraint_ptrs.end());
    /// (x0,x1,x2) = (1,1,1)
    EXPECT_FALSE(moves[1].is_univariable_move);
    EXPECT_TRUE(moves[1].is_special_neighborhood_move);
    EXPECT_EQ(0, moves[1].overlap_rate);
    EXPECT_EQ(3, static_cast<int>(moves[0].alterations.size()));
    EXPECT_EQ(1, moves[1].alterations[0].second);
    EXPECT_EQ(1, moves[1].alterations[1].second);
    EXPECT_EQ(1, moves[1].alterations[2].second);
    EXPECT_EQ(neighborhood::MoveSense::TrinomialExclusiveNor, moves[1].sense);
    EXPECT_TRUE(std::find(moves[1].related_constraint_ptrs.begin(),
                          moves[1].related_constraint_ptrs.end(),
                          &c[0]) != moves[1].related_constraint_ptrs.end());
}
}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/