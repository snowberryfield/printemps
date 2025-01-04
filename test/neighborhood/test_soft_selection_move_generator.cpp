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
class TestSoftSelectionMoveGenerator : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestSoftSelectionMoveGenerator, setup) {
    model::Model<int, double> model;

    auto& x = model.create_variables("x", 10, 0, 1);
    auto& y = model.create_variable("y", 0, 1);
    auto& c = model.create_constraint("c", -x.sum() == -y);

    model.setup_unique_names();
    model.setup_structure();

    auto soft_selection_ptrs =
        model.constraint_type_reference().soft_selection_ptrs;

    model.neighborhood().soft_selection().setup(soft_selection_ptrs);

    auto& moves = model.neighborhood().soft_selection().moves();
    auto& flags = model.neighborhood().soft_selection().flags();
    EXPECT_EQ(20, static_cast<int>(moves.size()));
    EXPECT_EQ(20, static_cast<int>(flags.size()));

    /// (x_0,y) = (0,0) -> (1,1)
    EXPECT_FALSE(moves[0].is_univariable_move);
    EXPECT_TRUE(moves[0].is_special_neighborhood_move);
    EXPECT_EQ(0, moves[0].overlap_rate);
    EXPECT_EQ(2, static_cast<int>(moves[0].alterations.size()));
    EXPECT_EQ(&(y[0]), moves[0].alterations[1].first);
    EXPECT_EQ(0, moves[0].alterations[0].second);
    EXPECT_EQ(0, moves[0].alterations[1].second);
    EXPECT_EQ(neighborhood::MoveSense::SoftSelection, moves[0].sense);
    EXPECT_TRUE(std::find(moves[0].related_constraint_ptrs.begin(),
                          moves[0].related_constraint_ptrs.end(),
                          &c[0]) != moves[0].related_constraint_ptrs.end());

    /// (x_0,y) = (1,1) -> (0,0)
    EXPECT_FALSE(moves[1].is_univariable_move);
    EXPECT_TRUE(moves[1].is_special_neighborhood_move);
    EXPECT_EQ(0, moves[1].overlap_rate);
    EXPECT_EQ(2, static_cast<int>(moves[1].alterations.size()));
    EXPECT_EQ(&(y[0]), moves[1].alterations[1].first);
    EXPECT_EQ(1, moves[1].alterations[0].second);
    EXPECT_EQ(1, moves[1].alterations[1].second);
    EXPECT_EQ(neighborhood::MoveSense::SoftSelection, moves[1].sense);
    EXPECT_TRUE(std::find(moves[1].related_constraint_ptrs.begin(),
                          moves[1].related_constraint_ptrs.end(),
                          &c[0]) != moves[1].related_constraint_ptrs.end());
}
}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/