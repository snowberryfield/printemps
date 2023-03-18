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
class TestExclusiveOrMoveGenerator : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestExclusiveOrMoveGenerator, setup) {
    model::Model<int, double> model;

    auto& x = model.create_variables("x", 2, 0, 1);
    auto& c = model.create_constraint("c", x[0] + x[1] == 1);

    model.setup_unique_names();
    model.setup_structure();

    auto& exclusive_or_ptrs =
        model.constraint_type_reference().exclusive_or_ptrs;

    model.neighborhood().exclusive_or().setup(exclusive_or_ptrs);
    model.neighborhood().exclusive_or().update_moves(true, false, false, false,
                                                     1);

    auto& moves = model.neighborhood().exclusive_or().moves();
    auto& flags = model.neighborhood().exclusive_or().flags();
    EXPECT_EQ(2, static_cast<int>(moves.size()));
    EXPECT_EQ(2, static_cast<int>(flags.size()));

    /// (x0,x1) = (0,1)
    EXPECT_FALSE(moves[0].is_univariable_move);
    EXPECT_TRUE(moves[0].is_special_neighborhood_move);
    EXPECT_EQ(0, moves[0].overlap_rate);
    EXPECT_EQ(2, static_cast<int>(moves[0].alterations.size()));
    EXPECT_EQ(0, moves[0].alterations[0].second);
    EXPECT_EQ(1, moves[0].alterations[1].second);
    EXPECT_EQ(neighborhood::MoveSense::ExclusiveOr, moves[0].sense);
    EXPECT_TRUE(std::find(moves[0].related_constraint_ptrs.begin(),
                          moves[0].related_constraint_ptrs.end(),
                          &c[0]) != moves[0].related_constraint_ptrs.end());

    /// (x0,x1) = (1,0)
    EXPECT_FALSE(moves[1].is_univariable_move);
    EXPECT_TRUE(moves[1].is_special_neighborhood_move);
    EXPECT_EQ(0, moves[1].overlap_rate);
    EXPECT_EQ(2, static_cast<int>(moves[0].alterations.size()));
    EXPECT_EQ(1, moves[1].alterations[0].second);
    EXPECT_EQ(0, moves[1].alterations[1].second);
    EXPECT_EQ(neighborhood::MoveSense::ExclusiveOr, moves[1].sense);
    EXPECT_TRUE(std::find(moves[1].related_constraint_ptrs.begin(),
                          moves[1].related_constraint_ptrs.end(),
                          &c[0]) != moves[1].related_constraint_ptrs.end());
}
}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/