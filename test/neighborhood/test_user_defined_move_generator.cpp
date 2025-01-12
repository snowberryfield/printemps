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
class TestUserDefinedMoveGenerator : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestUserDefinedMoveGenerator, setup) {
    model::Model<int, double> model;

    const int N = 100;
    auto&     x = model.create_variables("x", N, 0, 1);

    x(0).fix_by(0);
    x(1).fix_by(1);
    for (auto i = 2; i < N; i++) {
        x(i) = i % 2;
    }

    auto move_updater =
        [&x, N](std::vector<neighborhood::Move<int, double>>* a_moves_ptr) {
            a_moves_ptr->resize(N);
            for (auto i = 0; i < N; i++) {
                (*a_moves_ptr)[i].sense = neighborhood::MoveSense::UserDefined;
                (*a_moves_ptr)[i].alterations.clear();
                (*a_moves_ptr)[i].alterations.emplace_back(&x(i),
                                                           1 - x(i).value());
            }
        };

    model.neighborhood().user_defined().set_move_updater(move_updater);
    model.neighborhood().user_defined().setup();
    model.neighborhood().user_defined().update_moves(true, false, false, false,
                                                     1);

    auto& moves = model.neighborhood().user_defined().moves();
    auto& flags = model.neighborhood().user_defined().flags();
    EXPECT_EQ(N, static_cast<int>(moves.size()));
    EXPECT_EQ(N, static_cast<int>(flags.size()));

    for (auto i = 0; i < N; i++) {
        EXPECT_EQ(neighborhood::MoveSense::UserDefined, moves[i].sense);
        EXPECT_EQ(1, static_cast<int>(moves[i].alterations.size()));
        EXPECT_EQ(moves[i].alterations[0].second,
                  1 - moves[i].alterations[0].first->value());
        if (i < 2) {
            EXPECT_EQ(0, flags[i]);
        } else {
            EXPECT_EQ(1, flags[i]);
        }
    }
}
}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/