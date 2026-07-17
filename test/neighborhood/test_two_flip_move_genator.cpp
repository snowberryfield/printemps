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
class TestTwoFlipMoveGenerator : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestTwoFlipMoveGenerator, setup) {
    model::Model<int, double> model;

    auto& x = model.create_variables("x", 3, 0, 1);
    model.builder().setup_unique_names();

    std::vector<std::pair<model_component::Variable<int, double>*,
                          model_component::Variable<int, double>*>>
        flippable_variable_pairs;
    flippable_variable_pairs.push_back({&x(0), &x(1)});
    flippable_variable_pairs.push_back({&x(1), &x(2)});

    model.neighborhood().two_flip().setup(flippable_variable_pairs);
    model.neighborhood().two_flip().update_moves(true, false, false, false, 1);

    auto& moves = model.neighborhood().two_flip().moves();
    auto& flags = model.neighborhood().two_flip().flags();
    EXPECT_EQ(4, static_cast<int>(moves.size()));
    EXPECT_EQ(4, static_cast<int>(flags.size()));

    EXPECT_TRUE(moves[0].is_special_neighborhood_move);
    EXPECT_EQ(2, static_cast<int>(moves[0].alterations.size()));
    EXPECT_EQ(&x(0), moves[0].alterations[0].first);
    EXPECT_EQ(1, moves[0].alterations[0].second);
    EXPECT_EQ(&x(1), moves[0].alterations[1].first);
    EXPECT_EQ(0, moves[0].alterations[1].second);
    EXPECT_FALSE(moves[0].is_univariable_move);
    EXPECT_EQ(neighborhood::MoveType::TwoFlip, moves[0].type);

    EXPECT_TRUE(moves[1].is_special_neighborhood_move);
    EXPECT_EQ(2, static_cast<int>(moves[1].alterations.size()));
    EXPECT_EQ(&x(0), moves[1].alterations[0].first);
    EXPECT_EQ(0, moves[1].alterations[0].second);
    EXPECT_EQ(&x(1), moves[1].alterations[1].first);
    EXPECT_EQ(1, moves[1].alterations[1].second);
    EXPECT_FALSE(moves[1].is_univariable_move);
    EXPECT_EQ(neighborhood::MoveType::TwoFlip, moves[1].type);

    EXPECT_TRUE(moves[2].is_special_neighborhood_move);
    EXPECT_EQ(2, static_cast<int>(moves[2].alterations.size()));
    EXPECT_EQ(&x(1), moves[2].alterations[0].first);
    EXPECT_EQ(1, moves[2].alterations[0].second);
    EXPECT_EQ(&x(2), moves[2].alterations[1].first);
    EXPECT_EQ(0, moves[2].alterations[1].second);
    EXPECT_FALSE(moves[2].is_univariable_move);
    EXPECT_EQ(neighborhood::MoveType::TwoFlip, moves[2].type);

    EXPECT_TRUE(moves[3].is_special_neighborhood_move);
    EXPECT_EQ(2, static_cast<int>(moves[3].alterations.size()));
    EXPECT_EQ(&x(1), moves[3].alterations[0].first);
    EXPECT_EQ(0, moves[3].alterations[0].second);
    EXPECT_EQ(&x(2), moves[3].alterations[1].first);
    EXPECT_EQ(1, moves[3].alterations[1].second);
    EXPECT_FALSE(moves[3].is_univariable_move);
    EXPECT_EQ(neighborhood::MoveType::TwoFlip, moves[3].type);
}
}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/