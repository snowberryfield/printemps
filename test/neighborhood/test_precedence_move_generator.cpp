/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <gtest/gtest.h>
#include <printemps.h>

namespace {
/*****************************************************************************/
class TestPrecedenceMoveGenerator : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestPrecedenceMoveGenerator, setup) {
    printemps::model::Model<int, double> model;

    auto& x = model.create_variables("x", 2, -10, 10);
    auto& c = model.create_constraint("c", x[0] <= x[1]);

    model.categorize_variables();
    model.categorize_constraints();
    model.setup_variable_related_constraints();

    auto precedence_ptrs = model.constraint_type_reference().precedence_ptrs;

    model.neighborhood().precedence().setup(precedence_ptrs);
    model.neighborhood().precedence().update_moves(true, false, false, false);

    auto& moves = model.neighborhood().precedence().moves();
    auto& flags = model.neighborhood().precedence().flags();
    EXPECT_EQ(2, static_cast<int>(moves.size()));
    EXPECT_EQ(2, static_cast<int>(flags.size()));

    /// (x,y) = (0,0) -> (1,1)
    EXPECT_TRUE(moves[0].is_special_neighborhood_move);
    EXPECT_EQ(0, moves[0].overlap_rate);
    EXPECT_EQ(2, static_cast<int>(moves[0].alterations.size()));
    EXPECT_EQ(1, moves[0].alterations[0].second);
    EXPECT_EQ(1, moves[0].alterations[1].second);
    EXPECT_FALSE(moves[0].is_univariable_move);
    EXPECT_EQ(printemps::neighborhood::MoveSense::Precedence, moves[0].sense);
    EXPECT_TRUE(moves[0].related_constraint_ptrs.find(&c[0]) !=
                moves[0].related_constraint_ptrs.end());

    /// (x,y) = (0,0) -> (-1,-1)
    EXPECT_TRUE(moves[1].is_special_neighborhood_move);
    EXPECT_EQ(0, moves[1].overlap_rate);
    EXPECT_EQ(2, static_cast<int>(moves[1].alterations.size()));
    EXPECT_EQ(-1, moves[1].alterations[0].second);
    EXPECT_EQ(-1, moves[1].alterations[1].second);
    EXPECT_FALSE(moves[1].is_univariable_move);
    EXPECT_EQ(printemps::neighborhood::MoveSense::Precedence, moves[1].sense);
    EXPECT_TRUE(moves[1].related_constraint_ptrs.find(&c[0]) !=
                moves[1].related_constraint_ptrs.end());
}  // namespace

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/