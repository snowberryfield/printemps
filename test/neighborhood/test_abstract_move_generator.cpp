/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <gtest/gtest.h>
#include <printemps.h>

namespace {
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
    printemps::neighborhood::AbstractMoveGenerator<int, double> move_generator;

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
    printemps::neighborhood::AbstractMoveGenerator<int, double> move_generator;
    std::vector<printemps::neighborhood::Move<int, double>>     moves(10);
    move_generator.set_moves(moves);
    EXPECT_EQ(10, static_cast<int>(move_generator.moves().size()));
}

/*****************************************************************************/
TEST_F(TestAbstractMoveGenerator, set_moves) {
    /// This method is tested in moves().
}

/*****************************************************************************/
TEST_F(TestAbstractMoveGenerator, flags) {
    printemps::neighborhood::AbstractMoveGenerator<int, double> move_generator;
    std::vector<int>                                            flags(10);
    move_generator.set_flags(flags);
    EXPECT_EQ(10, static_cast<int>(move_generator.flags().size()));
}

/*****************************************************************************/
TEST_F(TestAbstractMoveGenerator, set_flags) {
    /// This method is tested in flags().
}

/*****************************************************************************/
TEST_F(TestAbstractMoveGenerator, is_enabled) {
    printemps::neighborhood::AbstractMoveGenerator<int, double> move_generator;
    EXPECT_FALSE(move_generator.is_enabled());
    move_generator.enable();
    EXPECT_TRUE(move_generator.is_enabled());
    move_generator.disable();
    EXPECT_FALSE(move_generator.is_enabled());
}

/*****************************************************************************/
TEST_F(TestAbstractMoveGenerator, enable) {
    /// This method is tested in is_enabled().
}

/*****************************************************************************/
TEST_F(TestAbstractMoveGenerator, disable) {
    /// This method is tested in is_enabled().
}

/*****************************************************************************/
TEST_F(TestAbstractMoveGenerator, reset_availability) {
    printemps::neighborhood::AbstractMoveGenerator<int, double> move_generator;
    std::vector<printemps::neighborhood::Move<int, double>>     moves(10);

    move_generator.set_moves(moves);

    for (auto &&move : move_generator.moves()) {
        move.is_available = false;
    }
    move_generator.reset_availability();
    for (const auto &move : move_generator.moves()) {
        EXPECT_TRUE(move.is_available);
    }
}
/*****************************************************************************/
}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/