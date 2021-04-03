/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <gtest/gtest.h>
#include <printemps.h>

namespace {
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
        printemps::model::Model<int, double> model;

        auto& x = model.create_variables("x", 2, -10, 10);
        auto& c = model.create_constraint("c", 2 * x[0] + 3 * x[1] <= 10);

        model.categorize_variables();
        model.categorize_constraints();
        model.setup_variable_related_constraints();

        auto variable_bound_ptrs =
            model.constraint_type_reference().variable_bound_ptrs;

        model.neighborhood().variable_bound().setup(variable_bound_ptrs);
        model.neighborhood().variable_bound().update_moves(true, false, false,
                                                           false);

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
        EXPECT_EQ(printemps::neighborhood::MoveSense::VariableBound,
                  moves[0].sense);
        EXPECT_TRUE(moves[0].related_constraint_ptrs.find(&c[0]) !=
                    moves[0].related_constraint_ptrs.end());

        /// (x,y) = (0,0) -> (-1,4)
        EXPECT_TRUE(moves[1].is_special_neighborhood_move);
        EXPECT_EQ(0, moves[1].overlap_rate);
        EXPECT_EQ(2, static_cast<int>(moves[1].alterations.size()));
        EXPECT_EQ(-1, moves[1].alterations[0].second);
        EXPECT_EQ(4, moves[1].alterations[1].second);
        EXPECT_EQ(printemps::neighborhood::MoveSense::VariableBound,
                  moves[1].sense);
        EXPECT_TRUE(moves[1].related_constraint_ptrs.find(&c[0]) !=
                    moves[1].related_constraint_ptrs.end());

        /// (x,y) = (0,0) -> (3,1)
        EXPECT_TRUE(moves[2].is_special_neighborhood_move);
        EXPECT_EQ(0, moves[2].overlap_rate);
        EXPECT_EQ(2, static_cast<int>(moves[2].alterations.size()));
        EXPECT_EQ(3, moves[2].alterations[0].second);
        EXPECT_EQ(1, moves[2].alterations[1].second);
        EXPECT_EQ(printemps::neighborhood::MoveSense::VariableBound,
                  moves[2].sense);
        EXPECT_TRUE(moves[2].related_constraint_ptrs.find(&c[0]) !=
                    moves[2].related_constraint_ptrs.end());

        /// (x,y) = (0,0) -> (6,-1)
        EXPECT_TRUE(moves[3].is_special_neighborhood_move);
        EXPECT_EQ(0, moves[3].overlap_rate);
        EXPECT_EQ(2, static_cast<int>(moves[3].alterations.size()));
        EXPECT_EQ(6, moves[3].alterations[0].second);
        EXPECT_EQ(-1, moves[3].alterations[1].second);
        EXPECT_EQ(printemps::neighborhood::MoveSense::VariableBound,
                  moves[3].sense);
        EXPECT_TRUE(moves[3].related_constraint_ptrs.find(&c[0]) !=
                    moves[3].related_constraint_ptrs.end());
    }
    /// Greater
    {
        printemps::model::Model<int, double> model;

        auto& x = model.create_variables("x", 2, -10, 10);
        auto& c = model.create_constraint("c", 2 * x[0] + 3 * x[1] >= 10);

        model.categorize_variables();
        model.categorize_constraints();
        model.setup_variable_related_constraints();

        auto variable_bound_ptrs =
            model.constraint_type_reference().variable_bound_ptrs;

        model.neighborhood().variable_bound().setup(variable_bound_ptrs);
        model.neighborhood().variable_bound().update_moves(true, false, false,
                                                           false);

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
        EXPECT_EQ(printemps::neighborhood::MoveSense::VariableBound,
                  moves[0].sense);
        EXPECT_TRUE(moves[0].related_constraint_ptrs.find(&c[0]) !=
                    moves[0].related_constraint_ptrs.end());

        /// (x,y) = (0,0) -> (-1,4)
        EXPECT_TRUE(moves[1].is_special_neighborhood_move);
        EXPECT_EQ(0, moves[1].overlap_rate);
        EXPECT_EQ(2, static_cast<int>(moves[1].alterations.size()));
        EXPECT_EQ(-1, moves[1].alterations[0].second);
        EXPECT_EQ(4, moves[1].alterations[1].second);
        EXPECT_EQ(printemps::neighborhood::MoveSense::VariableBound,
                  moves[1].sense);
        EXPECT_TRUE(moves[1].related_constraint_ptrs.find(&c[0]) !=
                    moves[1].related_constraint_ptrs.end());

        /// (x,y) = (0,0) -> (4,1)
        EXPECT_TRUE(moves[2].is_special_neighborhood_move);
        EXPECT_EQ(0, moves[2].overlap_rate);
        EXPECT_EQ(2, static_cast<int>(moves[2].alterations.size()));
        EXPECT_EQ(4, moves[2].alterations[0].second);
        EXPECT_EQ(1, moves[2].alterations[1].second);
        EXPECT_EQ(printemps::neighborhood::MoveSense::VariableBound,
                  moves[2].sense);
        EXPECT_TRUE(moves[2].related_constraint_ptrs.find(&c[0]) !=
                    moves[2].related_constraint_ptrs.end());

        /// (x,y) = (0,0) -> (7,-1)
        EXPECT_TRUE(moves[3].is_special_neighborhood_move);
        EXPECT_EQ(0, moves[3].overlap_rate);
        EXPECT_EQ(2, static_cast<int>(moves[3].alterations.size()));
        EXPECT_EQ(7, moves[3].alterations[0].second);
        EXPECT_EQ(-1, moves[3].alterations[1].second);
        EXPECT_EQ(printemps::neighborhood::MoveSense::VariableBound,
                  moves[3].sense);
        EXPECT_TRUE(moves[3].related_constraint_ptrs.find(&c[0]) !=
                    moves[3].related_constraint_ptrs.end());
    }

}  // namespace

/*****************************************************************************/
}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/