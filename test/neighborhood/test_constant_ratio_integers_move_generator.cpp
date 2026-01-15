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
class TestConstantRatioIntegersMoveGenerator : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestConstantRatioIntegersMoveGenerator, setup) {
    {
        model::Model<int, double> model;

        auto& x = model.create_variables("x", 2, -10, 10);
        auto& c = model.create_constraint("c", 2 * x[0] - x[1] == 0);

        model.builder().setup_unique_names();
        model.builder().setup_structure();

        auto& constant_ratio_integers_ptrs =
            model.reference().constraint_type.constant_ratio_integers_ptrs;
        model.neighborhood().constant_ratio_integers().setup(  //
            constant_ratio_integers_ptrs);
        model.neighborhood().constant_ratio_integers().update_moves(  //
            true, false, false, false, 1);

        auto& moves = model.neighborhood().constant_ratio_integers().moves();
        auto& flags = model.neighborhood().constant_ratio_integers().flags();

        EXPECT_EQ(2, static_cast<int>(moves.size()));
        EXPECT_EQ(2, static_cast<int>(flags.size()));

        EXPECT_EQ(&c[0], moves[0].associated_constraint_ptr);
        EXPECT_EQ(&c[0], moves[1].associated_constraint_ptr);

        /// (x0,x1) = (1,2)
        EXPECT_FALSE(moves[0].is_univariable_move);
        EXPECT_TRUE(moves[0].is_special_neighborhood_move);
        EXPECT_EQ(0, moves[0].overlap_rate);
        EXPECT_EQ(2, static_cast<int>(moves[0].alterations.size()));
        EXPECT_EQ(1, moves[0].alterations[0].second);
        EXPECT_EQ(2, moves[0].alterations[1].second);
        EXPECT_EQ(neighborhood::MoveType::ConstantRatioIntegers, moves[0].type);
        EXPECT_TRUE(std::find(moves[0].related_constraint_ptrs.begin(),
                              moves[0].related_constraint_ptrs.end(),
                              &c[0]) != moves[0].related_constraint_ptrs.end());
        /// (x0,x1) = (-1,-2)
        EXPECT_FALSE(moves[1].is_univariable_move);
        EXPECT_TRUE(moves[1].is_special_neighborhood_move);
        EXPECT_EQ(0, moves[1].overlap_rate);
        EXPECT_EQ(2, static_cast<int>(moves[0].alterations.size()));
        EXPECT_EQ(-1, moves[1].alterations[0].second);
        EXPECT_EQ(-2, moves[1].alterations[1].second);
        EXPECT_EQ(neighborhood::MoveType::ConstantRatioIntegers, moves[1].type);
        EXPECT_TRUE(std::find(moves[1].related_constraint_ptrs.begin(),
                              moves[1].related_constraint_ptrs.end(),
                              &c[0]) != moves[1].related_constraint_ptrs.end());
    }

    {
        model::Model<int, double> model;

        auto& x = model.create_variables("x", 2, -10, 10);
        auto& c = model.create_constraint("c", 2 * x[0] + x[1] == 0);

        model.builder().setup_unique_names();
        model.builder().setup_structure();

        auto& constant_ratio_integers_ptrs =
            model.reference().constraint_type.constant_ratio_integers_ptrs;
        model.neighborhood().constant_ratio_integers().setup(  //
            constant_ratio_integers_ptrs);
        model.neighborhood().constant_ratio_integers().update_moves(  //
            true, false, false, false, 1);

        auto& moves = model.neighborhood().constant_ratio_integers().moves();
        auto& flags = model.neighborhood().constant_ratio_integers().flags();

        EXPECT_EQ(2, static_cast<int>(moves.size()));
        EXPECT_EQ(2, static_cast<int>(flags.size()));

        /// (x0,x1) = (1,-2)
        EXPECT_FALSE(moves[0].is_univariable_move);
        EXPECT_TRUE(moves[0].is_special_neighborhood_move);
        EXPECT_EQ(0, moves[0].overlap_rate);
        EXPECT_EQ(2, static_cast<int>(moves[0].alterations.size()));
        EXPECT_EQ(1, moves[0].alterations[0].second);
        EXPECT_EQ(-2, moves[0].alterations[1].second);
        EXPECT_EQ(neighborhood::MoveType::ConstantRatioIntegers, moves[0].type);
        EXPECT_TRUE(std::find(moves[0].related_constraint_ptrs.begin(),
                              moves[0].related_constraint_ptrs.end(),
                              &c[0]) != moves[0].related_constraint_ptrs.end());

        /// (x0,x1) = (-1,2)
        EXPECT_FALSE(moves[1].is_univariable_move);
        EXPECT_TRUE(moves[1].is_special_neighborhood_move);
        EXPECT_EQ(0, moves[1].overlap_rate);
        EXPECT_EQ(2, static_cast<int>(moves[0].alterations.size()));
        EXPECT_EQ(-1, moves[1].alterations[0].second);
        EXPECT_EQ(2, moves[1].alterations[1].second);
        EXPECT_EQ(neighborhood::MoveType::ConstantRatioIntegers, moves[1].type);
        EXPECT_TRUE(std::find(moves[1].related_constraint_ptrs.begin(),
                              moves[1].related_constraint_ptrs.end(),
                              &c[0]) != moves[1].related_constraint_ptrs.end());
    }
}
}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/