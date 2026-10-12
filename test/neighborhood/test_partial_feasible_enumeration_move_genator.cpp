/*****************************************************************************/
// Copyright (c) 2020-2026 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <gtest/gtest.h>
#include <printemps.h>

namespace {
using namespace printemps;
/*****************************************************************************/
class TestPartialFeasibleEnumerationMoveGenerator : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestPartialFeasibleEnumerationMoveGenerator, setup) {
    model::Model<int, double> model;

    auto& x = model.create_variables("x", 4, 0, 1);
    auto& f = model.create_constraints("f", 2);
    f(0)    = x(0) + x(1) - x(2) - x(3) == 1;
    f(1)    = x(0) + x(1) + x(2) + x(3) == 3;
    model.builder().setup_unique_names();
    model.builder().update_derived_components();
    model.partial_feasible_enumerator().run(false);

    const auto small_constraint_groups =
        model.partial_feasible_enumerator().small_constraint_groups();

    EXPECT_EQ(1, static_cast<int>(small_constraint_groups.size()));
    EXPECT_EQ(
        2, static_cast<int>(small_constraint_groups[0].constraint_ptrs.size()));
    EXPECT_EQ(
        4, static_cast<int>(small_constraint_groups[0].variable_ptrs.size()));
    EXPECT_EQ(2, static_cast<int>(small_constraint_groups[0].solutions.size()));

    model.neighborhood().partial_feasible_enumeration().setup(
        small_constraint_groups);
    model.neighborhood().partial_feasible_enumeration().update_moves(
        true, false, false, false, 1);

    auto& moves = model.neighborhood().partial_feasible_enumeration().moves();
    auto& flags = model.neighborhood().partial_feasible_enumeration().flags();
    EXPECT_EQ(2, static_cast<int>(moves.size()));
    EXPECT_EQ(2, static_cast<int>(flags.size()));

    EXPECT_TRUE(moves[0].is_special_neighborhood_move);
    EXPECT_EQ(4, static_cast<int>(moves[0].alterations.size()));
    EXPECT_FALSE(moves[0].is_univariable_move);
    EXPECT_EQ(neighborhood::MoveType::PartialFeasibleEnumeration,
              moves[0].type);
}
}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/