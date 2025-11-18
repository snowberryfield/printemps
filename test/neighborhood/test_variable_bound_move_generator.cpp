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
class TestVariableBoundMoveGenerator : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestVariableBoundMoveGenerator, setup) {
    /// Less
    {
        model::Model<int, double> model;

        auto& x = model.create_variables("x", 2, -10, 10);
        auto& c = model.create_constraint("c", 2 * x[0] + 3 * x[1] <= 10);

        model.builder().setup_structure();

        auto variable_bound_ptrs =
            model.reference().constraint_type.variable_bound_ptrs;

        model.neighborhood().variable_bound().setup(variable_bound_ptrs);
        model.neighborhood().variable_bound().update_moves(  //
            true, false, false, false, 1);

        auto& moves = model.neighborhood().variable_bound().moves();
        auto& flags = model.neighborhood().variable_bound().flags();
        EXPECT_EQ(4, static_cast<int>(moves.size()));
        EXPECT_EQ(4, static_cast<int>(flags.size()));

        /// (x,y) = (0,0) -> (1,2)
        EXPECT_TRUE(moves[0].is_special_neighborhood_move);
        EXPECT_EQ(0, moves[0].overlap_rate);
        EXPECT_EQ(2, static_cast<int>(moves[0].alterations.size()));
        EXPECT_EQ(1, moves[0].alterations[0].second);
        EXPECT_EQ(2, moves[0].alterations[1].second);
        EXPECT_FALSE(moves[0].is_univariable_move);
        EXPECT_EQ(neighborhood::MoveType::VariableBound, moves[0].type);
        EXPECT_TRUE(std::find(moves[0].related_constraint_ptrs.begin(),
                              moves[0].related_constraint_ptrs.end(),
                              &c[0]) != moves[0].related_constraint_ptrs.end());

        /// (x,y) = (0,0) -> (-1,4)
        EXPECT_TRUE(moves[1].is_special_neighborhood_move);
        EXPECT_EQ(0, moves[1].overlap_rate);
        EXPECT_EQ(2, static_cast<int>(moves[1].alterations.size()));
        EXPECT_EQ(-1, moves[1].alterations[0].second);
        EXPECT_EQ(4, moves[1].alterations[1].second);
        EXPECT_EQ(neighborhood::MoveType::VariableBound, moves[1].type);
        EXPECT_TRUE(std::find(moves[1].related_constraint_ptrs.begin(),
                              moves[1].related_constraint_ptrs.end(),
                              &c[0]) != moves[1].related_constraint_ptrs.end());

        /// (x,y) = (0,0) -> (3,1)
        EXPECT_TRUE(moves[2].is_special_neighborhood_move);
        EXPECT_EQ(0, moves[2].overlap_rate);
        EXPECT_EQ(2, static_cast<int>(moves[2].alterations.size()));
        EXPECT_EQ(3, moves[2].alterations[0].second);
        EXPECT_EQ(1, moves[2].alterations[1].second);
        EXPECT_EQ(neighborhood::MoveType::VariableBound, moves[2].type);
        EXPECT_TRUE(std::find(moves[2].related_constraint_ptrs.begin(),
                              moves[2].related_constraint_ptrs.end(),
                              &c[0]) != moves[2].related_constraint_ptrs.end());

        /// (x,y) = (0,0) -> (6,-1)
        EXPECT_TRUE(moves[3].is_special_neighborhood_move);
        EXPECT_EQ(0, moves[3].overlap_rate);
        EXPECT_EQ(2, static_cast<int>(moves[3].alterations.size()));
        EXPECT_EQ(6, moves[3].alterations[0].second);
        EXPECT_EQ(-1, moves[3].alterations[1].second);
        EXPECT_EQ(neighborhood::MoveType::VariableBound, moves[3].type);
        EXPECT_TRUE(std::find(moves[3].related_constraint_ptrs.begin(),
                              moves[3].related_constraint_ptrs.end(),
                              &c[0]) != moves[3].related_constraint_ptrs.end());
    }
    /// Greater
    {
        model::Model<int, double> model;

        auto& x = model.create_variables("x", 2, -10, 10);
        auto& c = model.create_constraint("c", 2 * x[0] + 3 * x[1] >= 10);

        model.builder().setup_structure();

        auto variable_bound_ptrs =
            model.reference().constraint_type.variable_bound_ptrs;

        model.neighborhood().variable_bound().setup(variable_bound_ptrs);
        model.neighborhood().variable_bound().update_moves(  //
            true, false, false, false, 1);

        auto& moves = model.neighborhood().variable_bound().moves();
        auto& flags = model.neighborhood().variable_bound().flags();
        EXPECT_EQ(4, static_cast<int>(moves.size()));
        EXPECT_EQ(4, static_cast<int>(flags.size()));

        /// (x,y) = (0,0) -> (1,3)
        EXPECT_TRUE(moves[0].is_special_neighborhood_move);
        EXPECT_EQ(0, moves[0].overlap_rate);
        EXPECT_EQ(2, static_cast<int>(moves[0].alterations.size()));
        EXPECT_EQ(1, moves[0].alterations[0].second);
        EXPECT_EQ(3, moves[0].alterations[1].second);
        EXPECT_EQ(neighborhood::MoveType::VariableBound, moves[0].type);
        EXPECT_TRUE(std::find(moves[0].related_constraint_ptrs.begin(),
                              moves[0].related_constraint_ptrs.end(),
                              &c[0]) != moves[0].related_constraint_ptrs.end());

        /// (x,y) = (0,0) -> (-1,4)
        EXPECT_TRUE(moves[1].is_special_neighborhood_move);
        EXPECT_EQ(0, moves[1].overlap_rate);
        EXPECT_EQ(2, static_cast<int>(moves[1].alterations.size()));
        EXPECT_EQ(-1, moves[1].alterations[0].second);
        EXPECT_EQ(4, moves[1].alterations[1].second);
        EXPECT_EQ(neighborhood::MoveType::VariableBound, moves[1].type);
        EXPECT_TRUE(std::find(moves[1].related_constraint_ptrs.begin(),
                              moves[1].related_constraint_ptrs.end(),
                              &c[0]) != moves[1].related_constraint_ptrs.end());

        /// (x,y) = (0,0) -> (4,1)
        EXPECT_TRUE(moves[2].is_special_neighborhood_move);
        EXPECT_EQ(0, moves[2].overlap_rate);
        EXPECT_EQ(2, static_cast<int>(moves[2].alterations.size()));
        EXPECT_EQ(4, moves[2].alterations[0].second);
        EXPECT_EQ(1, moves[2].alterations[1].second);
        EXPECT_EQ(neighborhood::MoveType::VariableBound, moves[2].type);
        EXPECT_TRUE(std::find(moves[2].related_constraint_ptrs.begin(),
                              moves[2].related_constraint_ptrs.end(),
                              &c[0]) != moves[2].related_constraint_ptrs.end());

        /// (x,y) = (0,0) -> (7,-1)
        EXPECT_TRUE(moves[3].is_special_neighborhood_move);
        EXPECT_EQ(0, moves[3].overlap_rate);
        EXPECT_EQ(2, static_cast<int>(moves[3].alterations.size()));
        EXPECT_EQ(7, moves[3].alterations[0].second);
        EXPECT_EQ(-1, moves[3].alterations[1].second);
        EXPECT_EQ(neighborhood::MoveType::VariableBound, moves[3].type);
        EXPECT_TRUE(std::find(moves[3].related_constraint_ptrs.begin(),
                              moves[3].related_constraint_ptrs.end(),
                              &c[0]) != moves[3].related_constraint_ptrs.end());
    }
}
}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/