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
class TestChainMoveGenerator : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestChainMoveGenerator, setup) {
}  // namespace

/*****************************************************************************/
TEST_F(TestChainMoveGenerator, register_move) {
    model::Model<int, double>       model;
    neighborhood::Move<int, double> move;

    model.neighborhood().chain().register_move(move);

    EXPECT_EQ(1, static_cast<int>(model.neighborhood().chain().moves().size()));
    EXPECT_EQ(1, static_cast<int>(model.neighborhood().chain().flags().size()));

    model.neighborhood().chain().clear_moves();
    EXPECT_TRUE(model.neighborhood().chain().moves().empty());
    EXPECT_TRUE(model.neighborhood().chain().flags().empty());
}

/*****************************************************************************/
TEST_F(TestChainMoveGenerator, clear_moves) {
    /// This method is tested in register_move().
}

/*****************************************************************************/
TEST_F(TestChainMoveGenerator, deduplicate_moves) {
    model::Model<int, double> model;

    neighborhood::Move<int, double> move_0;
    move_0.overlap_rate = 0.1;

    neighborhood::Move<int, double> move_1;
    move_1.overlap_rate = 0.2;

    neighborhood::Move<int, double> move_2;
    move_2.overlap_rate = 0.3;

    for (auto i = 0; i < 3; i++) {
        model.neighborhood().chain().register_move(move_0);
        model.neighborhood().chain().register_move(move_1);
        model.neighborhood().chain().register_move(move_2);
    }
    EXPECT_EQ(9, static_cast<int>(model.neighborhood().chain().moves().size()));
    EXPECT_EQ(9, static_cast<int>(model.neighborhood().chain().flags().size()));

    model.neighborhood().chain().sort_moves();
    auto& moves = model.neighborhood().chain().moves();
    EXPECT_FLOAT_EQ(0.3, moves[0].overlap_rate);
    EXPECT_FLOAT_EQ(0.3, moves[1].overlap_rate);
    EXPECT_FLOAT_EQ(0.3, moves[2].overlap_rate);
    EXPECT_FLOAT_EQ(0.2, moves[3].overlap_rate);
    EXPECT_FLOAT_EQ(0.2, moves[4].overlap_rate);
    EXPECT_FLOAT_EQ(0.2, moves[5].overlap_rate);
    EXPECT_FLOAT_EQ(0.1, moves[6].overlap_rate);
    EXPECT_FLOAT_EQ(0.1, moves[7].overlap_rate);
    EXPECT_FLOAT_EQ(0.1, moves[8].overlap_rate);

    model.neighborhood().chain().deduplicate_moves();
    EXPECT_EQ(3, static_cast<int>(model.neighborhood().chain().moves().size()));
    EXPECT_EQ(3, static_cast<int>(model.neighborhood().chain().flags().size()));
}

/*****************************************************************************/
TEST_F(TestChainMoveGenerator, sort_moves) {
    /// This method is tested in deduplicate_moves().
}

/*****************************************************************************/
TEST_F(TestChainMoveGenerator, shuffle_moves) {
    /// This test is skipped.
}

/*****************************************************************************/
TEST_F(TestChainMoveGenerator, reduce_moves) {
    model::Model<int, double>       model;
    neighborhood::Move<int, double> move;

    const int N                   = 5000;
    const int CHAIN_MOVE_CAPACITY = 10000;

    for (auto i = 0; i < N; i++) {
        model.neighborhood().chain().register_move(move);
    }

    model.neighborhood().chain().reduce_moves(CHAIN_MOVE_CAPACITY);
    EXPECT_EQ(N, static_cast<int>(model.neighborhood().chain().moves().size()));
    EXPECT_EQ(N, static_cast<int>(model.neighborhood().chain().flags().size()));

    for (auto i = 0; i < N; i++) {
        model.neighborhood().chain().register_move(move);
    }

    model.neighborhood().chain().reduce_moves(CHAIN_MOVE_CAPACITY);
    EXPECT_EQ(CHAIN_MOVE_CAPACITY,
              static_cast<int>(model.neighborhood().chain().moves().size()));
    EXPECT_EQ(CHAIN_MOVE_CAPACITY,
              static_cast<int>(model.neighborhood().chain().flags().size()));

    for (auto i = 0; i < N; i++) {
        model.neighborhood().chain().register_move(move);
    }

    model.neighborhood().chain().reduce_moves(CHAIN_MOVE_CAPACITY);
    EXPECT_EQ(CHAIN_MOVE_CAPACITY,
              static_cast<int>(model.neighborhood().chain().moves().size()));
    EXPECT_EQ(CHAIN_MOVE_CAPACITY,
              static_cast<int>(model.neighborhood().chain().flags().size()));
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/