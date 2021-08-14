/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <gtest/gtest.h>
#include <printemps.h>

namespace {
/*****************************************************************************/
class TestBinaryMoveGenerator : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestBinaryMoveGenerator, setup) {
    printemps::model::Model<int, double> model;

    auto& x = model.create_variables("x", 10, 0, 1);
    x(0).fix_by(0);

    model.categorize_variables();
    model.categorize_constraints();

    auto binary_variable_ptrs = model.variable_reference().binary_variable_ptrs;

    model.neighborhood().binary().setup(binary_variable_ptrs);
    model.neighborhood().binary().update_moves(true, false, false, false);

    auto& moves = model.neighborhood().binary().moves();
    auto& flags = model.neighborhood().binary().flags();
    EXPECT_EQ(9, static_cast<int>(moves.size()));
    EXPECT_EQ(9, static_cast<int>(flags.size()));

    for (const auto& flag : flags) {
        EXPECT_EQ(1, flag);
    }

    for (const auto& move : moves) {
        EXPECT_EQ(printemps::neighborhood::MoveSense::Binary, move.sense);
        EXPECT_FALSE(move.alterations.front().first->is_fixed());
        EXPECT_EQ(move.alterations.front().second,
                  1 - move.alterations.front().first->value());

        EXPECT_TRUE(move.is_univariable_move);
        EXPECT_EQ(1, static_cast<int>(move.alterations.size()));

        for (auto& constraint_ptr :
             move.alterations.front().first->related_constraint_ptrs()) {
            EXPECT_TRUE(move.related_constraint_ptrs.find(constraint_ptr) !=
                        move.related_constraint_ptrs.end());
        }
    }
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/