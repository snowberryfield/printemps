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

/*****************************************************************************/
class TestNeighborhood : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestNeighborhood, enable_default_moves) {
    model::Model<int, double> model;

    auto&                  x = model.create_variables("x", 5, 0, 1);
    [[maybe_unused]] auto& y = model.create_variables("y", 5, 0, 10);
    [[maybe_unused]] auto& z = model.create_variables("z", 5, 0, 1);
    [[maybe_unused]] auto& f = model.create_constraint("f", x.selection());
    x(0) = 1;

    model.builder().setup_unique_names();
    model.builder().update_derived_components();

    preprocess::SelectionExtractor<int, double> selection_extractor(&model);
    selection_extractor.extract_by_number_of_variables_order(false, false);
    x(0).select();
    model.builder().update_derived_components();

    option::Option option;
    model.neighborhood().setup(&model, option);

    // Verify all default move generators have moves
    EXPECT_GT(model.neighborhood().binary().moves().size(), 0u);
    EXPECT_GT(model.neighborhood().integer().moves().size(), 0u);
    EXPECT_GT(model.neighborhood().selection().moves().size(), 0u);

    // Initially disabled
    EXPECT_FALSE(model.neighborhood().binary().is_enabled());
    EXPECT_FALSE(model.neighborhood().integer().is_enabled());
    EXPECT_FALSE(model.neighborhood().selection().is_enabled());
    EXPECT_FALSE(model.neighborhood().user_defined().is_enabled());

    // 1. Enable all default moves
    {
        option::NeighborhoodOption neighborhood_option;
        neighborhood_option.is_enabled_binary_move       = true;
        neighborhood_option.is_enabled_integer_move      = true;
        neighborhood_option.is_enabled_user_defined_move = true;
        neighborhood_option.selection_mode = option::selection_mode::Defined;

        model.neighborhood().enable_default_moves(neighborhood_option);

        EXPECT_TRUE(model.neighborhood().binary().is_enabled());
        EXPECT_TRUE(model.neighborhood().integer().is_enabled());
        EXPECT_TRUE(model.neighborhood().selection().is_enabled());
        EXPECT_TRUE(model.neighborhood().user_defined().is_enabled());
    }

    // 2. Selectively enable default moves
    {
        model.neighborhood().binary().disable();
        model.neighborhood().integer().disable();
        model.neighborhood().selection().disable();
        model.neighborhood().user_defined().disable();

        option::NeighborhoodOption neighborhood_option;
        neighborhood_option.is_enabled_binary_move       = true;
        neighborhood_option.is_enabled_integer_move      = false;
        neighborhood_option.is_enabled_user_defined_move = false;
        neighborhood_option.selection_mode = option::selection_mode::Off;

        model.neighborhood().enable_default_moves(neighborhood_option);

        EXPECT_TRUE(model.neighborhood().binary().is_enabled());
        EXPECT_FALSE(model.neighborhood().integer().is_enabled());
        EXPECT_FALSE(model.neighborhood().selection().is_enabled());
        EXPECT_FALSE(model.neighborhood().user_defined().is_enabled());
    }
}
}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/