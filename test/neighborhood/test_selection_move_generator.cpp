/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <gtest/gtest.h>
#include <printemps.h>

namespace {
/*****************************************************************************/
class TestSelectionMoveGenerator : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestSelectionMoveGenerator, setup) {
    printemps::model::Model<int, double> model;

    const int N = 10;
    auto&     x = model.create_variables("x", N, 0, 1);
    model.create_constraint("c", x.selection());
    x(0) = 1;

    model.setup_structure();

    printemps::preprocess::SelectionExtractor<int, double> selection_extractor(
        &model);
    selection_extractor.extract_by_number_of_variables_order(false, false);
    x(0).select();

    model.setup_structure();

    auto selection_variable_ptrs =
        model.variable_reference().selection_variable_ptrs;

    model.neighborhood().selection().setup(selection_variable_ptrs);
    model.neighborhood().selection().update_moves(true, false, false, false);

    auto& moves = model.neighborhood().selection().moves();
    auto& flags = model.neighborhood().selection().flags();
    EXPECT_EQ(N, static_cast<int>(moves.size()));
    EXPECT_EQ(N, static_cast<int>(flags.size()));

    for (auto i = 0; i < N; i++) {
        if (moves[i].alterations[1].first->value() == 1) {
            EXPECT_EQ(0, flags[i]);
        } else {
            EXPECT_EQ(1, flags[i]);
        }
    }

    for (auto& move : moves) {
        EXPECT_EQ(printemps::neighborhood::MoveSense::Selection, move.sense);
        EXPECT_EQ(2, static_cast<int>(move.alterations.size()));

        EXPECT_EQ(1, move.alterations.front().first->value());
        EXPECT_EQ(0, move.alterations.front().second);

        if (move.alterations[0].first != move.alterations[1].first) {
            EXPECT_EQ(0, move.alterations[1].first->value());
            EXPECT_EQ(1, move.alterations[1].second);
        }
        EXPECT_FALSE(move.is_univariable_move);

        for (auto& constraint_ptr :
             move.alterations[0].first->related_constraint_ptrs()) {
            EXPECT_TRUE(move.related_constraint_ptrs.find(constraint_ptr) !=
                        move.related_constraint_ptrs.end());
        }

        for (auto& constraint_ptr :
             move.alterations[1].first->related_constraint_ptrs()) {
            EXPECT_TRUE(move.related_constraint_ptrs.find(constraint_ptr) !=
                        move.related_constraint_ptrs.end());
        }
    }

}  // namespace

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/