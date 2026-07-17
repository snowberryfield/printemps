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
class TestAbstractMoveGenerator : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestAbstractMoveGenerator, initialize) {
    neighborhood::AbstractMoveGenerator<int, double> move_generator;

    EXPECT_TRUE(move_generator.moves().empty());
    EXPECT_TRUE(move_generator.flags().empty());
    EXPECT_FALSE(move_generator.is_enabled());
}

/*****************************************************************************/
TEST_F(TestAbstractMoveGenerator, update_moves) {
    /// The test for this method is omitted.
}

/*****************************************************************************/
TEST_F(TestAbstractMoveGenerator, moves) {
    neighborhood::AbstractMoveGenerator<int, double> move_generator;
    std::vector<neighborhood::Move<int, double>>     moves(10);
    move_generator.set_moves(moves);
    EXPECT_EQ(10, static_cast<int>(move_generator.moves().size()));
}

/*****************************************************************************/
TEST_F(TestAbstractMoveGenerator, set_moves) {
    /// This test is covered by moves().
}

/*****************************************************************************/
TEST_F(TestAbstractMoveGenerator, flags) {
    neighborhood::AbstractMoveGenerator<int, double> move_generator;
    std::vector<short>                               flags(10);
    move_generator.set_flags(flags);
    EXPECT_EQ(10, static_cast<int>(move_generator.flags().size()));
}

/*****************************************************************************/
TEST_F(TestAbstractMoveGenerator, set_flags) {
    /// This test is covered by flags().
}

/*****************************************************************************/
TEST_F(TestAbstractMoveGenerator, is_enabled) {
    neighborhood::AbstractMoveGenerator<int, double> move_generator;
    EXPECT_FALSE(move_generator.is_enabled());
    move_generator.enable();
    EXPECT_TRUE(move_generator.is_enabled());
    move_generator.disable();
    EXPECT_FALSE(move_generator.is_enabled());
}

/*****************************************************************************/
TEST_F(TestAbstractMoveGenerator, enable) {
    /// This test is covered by is_enabled().
}

/*****************************************************************************/
TEST_F(TestAbstractMoveGenerator, disable) {
    /// This test is covered by is_enabled().
}

/*****************************************************************************/
TEST_F(TestAbstractMoveGenerator, reset_availability) {
    neighborhood::AbstractMoveGenerator<int, double> move_generator;
    std::vector<neighborhood::Move<int, double>>     moves(10);

    move_generator.set_moves(moves);

    for (auto &&move : move_generator.moves()) {
        move.is_available = false;
    }
    move_generator.reset_availability();
    for (const auto &move : move_generator.moves()) {
        EXPECT_TRUE(move.is_available);
    }
}
}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/