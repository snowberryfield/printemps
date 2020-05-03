/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <gtest/gtest.h>
#include <random>
#include <cppmh.h>

/*****************************************************************************/
namespace {
class TestNeighborhood : public ::testing::Test {
   protected:
    cppmh::utility::IntegerUniformRandom m_random_integer;
    cppmh::utility::IntegerUniformRandom m_random_positive_integer;

    virtual void SetUp(void) {
        m_random_integer.setup(-1000, 1000, 0);
        m_random_positive_integer.setup(1, 1000, 0);
    }
    virtual void TearDown() {
        /// nothing to do
    }
    int random_integer(void) {
        return m_random_integer.generate_random();
    }

    int random_positive_integer(void) {
        return m_random_positive_integer.generate_random();
    }
};

/*****************************************************************************/
TEST_F(TestNeighborhood, initialize) {
    cppmh::model::Neighborhood<int, double> neighborhood;
    EXPECT_EQ(true, neighborhood.selections().empty());
    EXPECT_EQ(true, neighborhood.selection_variable_ptrs().empty());
    EXPECT_EQ(true, neighborhood.binary_variable_ptrs().empty());
    EXPECT_EQ(true, neighborhood.integer_variable_ptrs().empty());
    EXPECT_EQ(true, neighborhood.selection_moves().empty());
    EXPECT_EQ(true, neighborhood.binary_moves().empty());
    EXPECT_EQ(true, neighborhood.integer_moves().empty());
    EXPECT_EQ(true, neighborhood.user_defined_moves().empty());
    EXPECT_EQ(true, neighborhood.move_ptrs().empty());
    EXPECT_EQ(false, neighborhood.has_fixed_variables());
    EXPECT_EQ(true, neighborhood.is_enabled_selection_move());
    EXPECT_EQ(true, neighborhood.is_enabled_binary_move());
    EXPECT_EQ(true, neighborhood.is_enabled_integer_move());
    EXPECT_EQ(false, neighborhood.is_enabled_user_defined_move());

    /// Tests for updater functions are omitted.
}

/*****************************************************************************/
TEST_F(TestNeighborhood, setup_has_fixed_variables) {
    /// Scaler
    {
        cppmh::model::Model<int, double> model;

        [[maybe_unused]] auto& x = model.create_variable("x", 0, 1);
        [[maybe_unused]] auto& y = model.create_variable("y", 0, 1);

        model.setup_default_neighborhood(false);
        model.setup_has_fixed_variables();
        EXPECT_EQ(false, model.neighborhood().has_fixed_variables());
    }

    {
        cppmh::model::Model<int, double> model;

        auto&                  x = model.create_variable("x", 0, 1);
        [[maybe_unused]] auto& y = model.create_variable("y", 0, 1);
        x.fix_by(1);

        model.setup_default_neighborhood(false);
        model.setup_has_fixed_variables();
        EXPECT_EQ(true, model.neighborhood().has_fixed_variables());
    }

    {
        cppmh::model::Model<int, double> model;

        [[maybe_unused]] auto& x = model.create_variable("x", 0, 1);
        auto&                  y = model.create_variable("y", 0, 1);
        y.fix_by(1);

        model.setup_default_neighborhood(false);
        model.setup_has_fixed_variables();
        EXPECT_EQ(true, model.neighborhood().has_fixed_variables());
    }

    {
        cppmh::model::Model<int, double> model;

        auto& x = model.create_variable("x", 0, 1);
        auto& y = model.create_variable("y", 0, 1);
        x.fix_by(1);
        y.fix_by(1);
        x.unfix();
        y.unfix();

        model.setup_default_neighborhood(false);
        model.setup_has_fixed_variables();
        EXPECT_EQ(false, model.neighborhood().has_fixed_variables());
    }

    /// One-dimensional
    {
        cppmh::model::Model<int, double> model;

        [[maybe_unused]] auto& x = model.create_variables("x", 10, 0, 1);
        [[maybe_unused]] auto& y = model.create_variables("y", 10, 0, 1);

        model.setup_default_neighborhood(false);
        model.setup_has_fixed_variables();
        EXPECT_EQ(false, model.neighborhood().has_fixed_variables());
    }

    {
        cppmh::model::Model<int, double> model;

        auto&                  x = model.create_variables("x", 10, 0, 1);
        [[maybe_unused]] auto& y = model.create_variables("y", 10, 0, 1);
        x(0).fix_by(1);

        model.setup_default_neighborhood(false);
        model.setup_has_fixed_variables();
        EXPECT_EQ(true, model.neighborhood().has_fixed_variables());
    }

    {
        cppmh::model::Model<int, double> model;

        [[maybe_unused]] auto& x = model.create_variables("x", 10, 0, 1);
        auto&                  y = model.create_variables("y", 10, 0, 1);
        y(0).fix_by(1);

        model.setup_default_neighborhood(false);
        model.setup_has_fixed_variables();
        EXPECT_EQ(true, model.neighborhood().has_fixed_variables());
    }

    {
        cppmh::model::Model<int, double> model;

        auto& x = model.create_variables("x", 10, 0, 1);
        auto& y = model.create_variables("y", 10, 0, 1);
        x(0).fix_by(1);
        y(0).fix_by(1);
        x(0).unfix();
        y(0).unfix();

        model.setup_default_neighborhood(false);
        model.setup_has_fixed_variables();
        EXPECT_EQ(false, model.neighborhood().has_fixed_variables());
    }

    /// Two-dimensional
    {
        cppmh::model::Model<int, double> model;

        [[maybe_unused]] auto& x = model.create_variables("x", {10, 10}, 0, 1);
        [[maybe_unused]] auto& y = model.create_variables("y", {10, 10}, 0, 1);

        model.setup_default_neighborhood(false);
        model.setup_has_fixed_variables();
        EXPECT_EQ(false, model.neighborhood().has_fixed_variables());
    }

    {
        cppmh::model::Model<int, double> model;

        auto&                  x = model.create_variables("x", {10, 10}, 0, 1);
        [[maybe_unused]] auto& y = model.create_variables("y", {10, 10}, 0, 1);
        x(0, 0).fix_by(1);

        model.setup_default_neighborhood(false);
        model.setup_has_fixed_variables();
        EXPECT_EQ(true, model.neighborhood().has_fixed_variables());
    }

    {
        cppmh::model::Model<int, double> model;

        [[maybe_unused]] auto& x = model.create_variables("x", {10, 10}, 0, 1);
        auto&                  y = model.create_variables("y", {10, 10}, 0, 1);
        y(0, 0).fix_by(1);

        model.setup_default_neighborhood(false);
        model.setup_has_fixed_variables();
        EXPECT_EQ(true, model.neighborhood().has_fixed_variables());
    }

    {
        cppmh::model::Model<int, double> model;

        auto& x = model.create_variables("x", {10, 10}, 0, 1);
        auto& y = model.create_variables("y", {10, 10}, 0, 1);
        x(0, 0).fix_by(1);
        y(0, 0).fix_by(1);
        x(0, 0).unfix();
        y(0, 0).unfix();

        model.setup_default_neighborhood(false);
        model.setup_has_fixed_variables();

        EXPECT_EQ(false, model.neighborhood().has_fixed_variables());
    }
}

/*****************************************************************************/
TEST_F(TestNeighborhood, categorize_variables_and_constraints) {
    cppmh::model::Model<int, double> model;

    auto&                  x0 = model.create_variables("x0", {10, 10}, 0, 1);
    auto&                  x1 = model.create_variables("x1", {20, 20}, 0, 1);
    auto&                  x2 = model.create_variables("x2", 1, 0, 1);
    auto&                  x3 = model.create_variables("x3", 2, 0, 1);
    auto&                  x4 = model.create_variables("x4", 3, 0, 1);
    [[maybe_unused]] auto& y  = model.create_variables("y", {30, 30}, -10, 10);

    /// This constraint is parsed as a selection with no overlap. The
    /// corresponding constraint will be disabled.
    model.create_constraint("c0", x0.selection({0, cppmh::model::Range::All}));

    /// This constraint beyond two variable proxies is parsed as a selection
    /// with no overlap. The corresponding constraint will be disabled.
    model.create_constraint(
        "c1", (x0.sum({1, cppmh::model::Range::All}) +
               x1.sum({1, cppmh::model::Range::All}) + x3(0) + x4(0)) == 1);

    /// This constraint is parsed as a selection 20 overlap variables. The
    /// corresponding constraint will be still enabled.
    model.create_constraint("c2", x0.selection());

    /// This constraint is parsed as a selection 20 overlap variables. The
    /// corresponding constraint will be still enabled.
    model.create_constraint("c3", x1.selection());

    /// This constraint is not parsed as a selection because the number of
    /// covered variables is 1. The corresponding constraint will be still
    /// enabled.
    model.create_constraint("c4", x2.selection());

    /// This constraint is not parsed as a selection because the number of
    /// covered variables is 1 due to overlap. The corresponding constraint will
    /// be still enabled.
    model.create_constraint("c5", x3.selection());

    /// This constraint is parsed as a selection with 2 variables. The
    /// corresponding constraint will be still enabled.
    model.create_constraint("c6", x4.selection());

    model.setup_default_neighborhood(false);
    model.setup_has_fixed_variables();

    EXPECT_EQ(5, static_cast<int>(model.neighborhood().selections().size()));

    /// Check the numbers of covered variables and  variable pointers.
    {
        /// Constraint c0
        auto variable_ptrs = model.neighborhood().selections()[0].variable_ptrs;
        EXPECT_EQ(10, static_cast<int>(variable_ptrs.size()));

        EXPECT_EQ(true, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                   &x0(0, 0)) != variable_ptrs.end()));
        EXPECT_EQ(true, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                   &x0(0, 9)) != variable_ptrs.end()));
        EXPECT_EQ(false, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                    &x0(1, 0)) != variable_ptrs.end()));
    }

    {
        /// Constraint c1
        auto variable_ptrs = model.neighborhood().selections()[1].variable_ptrs;
        EXPECT_EQ(10 + 20 + 1 + 1, static_cast<int>(variable_ptrs.size()));

        EXPECT_EQ(false, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                    &x0(0, 9)) != variable_ptrs.end()));
        EXPECT_EQ(true, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                   &x0(1, 0)) != variable_ptrs.end()));
        EXPECT_EQ(true, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                   &x0(1, 9)) != variable_ptrs.end()));

        EXPECT_EQ(false, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                    &x1(0, 9)) != variable_ptrs.end()));
        EXPECT_EQ(true, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                   &x1(1, 0)) != variable_ptrs.end()));
        EXPECT_EQ(true, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                   &x1(1, 19)) != variable_ptrs.end()));
        EXPECT_EQ(false, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                    &x1(2, 0)) != variable_ptrs.end()));

        EXPECT_EQ(true, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                   &x3(0)) != variable_ptrs.end()));
        EXPECT_EQ(false, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                    &x3(1)) != variable_ptrs.end()));

        EXPECT_EQ(true, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                   &x4(0)) != variable_ptrs.end()));
        EXPECT_EQ(false, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                    &x4(1)) != variable_ptrs.end()));
    }

    {
        /// Constraint c2
        auto variable_ptrs = model.neighborhood().selections()[2].variable_ptrs;
        EXPECT_EQ(10 * 10 - 20, static_cast<int>(variable_ptrs.size()));
        EXPECT_EQ(false, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                    &x0(1, 9)) != variable_ptrs.end()));
        EXPECT_EQ(true, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                   &x0(2, 0)) != variable_ptrs.end()));
        EXPECT_EQ(true, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                   &x0(9, 9)) != variable_ptrs.end()));
    }

    {
        /// Constraint c3
        auto variable_ptrs = model.neighborhood().selections()[3].variable_ptrs;
        EXPECT_EQ(20 * 20 - 20, static_cast<int>(variable_ptrs.size()));
        EXPECT_EQ(true, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                   &x1(0, 0)) != variable_ptrs.end()));
        EXPECT_EQ(true, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                   &x1(0, 19)) != variable_ptrs.end()));
        EXPECT_EQ(false, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                    &x1(1, 0)) != variable_ptrs.end()));
        EXPECT_EQ(false, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                    &x1(1, 19)) != variable_ptrs.end()));
        EXPECT_EQ(true, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                   &x1(2, 0)) != variable_ptrs.end()));
        EXPECT_EQ(true, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                   &x1(19, 19)) != variable_ptrs.end()));
    }

    {
        /// Constraint c6
        auto variable_ptrs = model.neighborhood().selections()[4].variable_ptrs;
        EXPECT_EQ(3 - 1, static_cast<int>(variable_ptrs.size()));

        EXPECT_EQ(false, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                    &x4(0)) != variable_ptrs.end()));
        EXPECT_EQ(true, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                   &x4(1)) != variable_ptrs.end()));
        EXPECT_EQ(true, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                   &x4(2)) != variable_ptrs.end()));
    }

    /// Check whether the corresponding constraint is enabled or not.
    /// Constraint c0
    EXPECT_EQ(
        false,
        model.neighborhood().selections()[0].constraint_ptr->is_enabled());

    /// Constraint c1
    EXPECT_EQ(
        false,
        model.neighborhood().selections()[1].constraint_ptr->is_enabled());

    /// Constraint c2
    EXPECT_EQ(
        true,
        model.neighborhood().selections()[2].constraint_ptr->is_enabled());

    /// Constraint c3
    EXPECT_EQ(
        true,
        model.neighborhood().selections()[3].constraint_ptr->is_enabled());

    /// Constraint c6
    EXPECT_EQ(
        true,
        model.neighborhood().selections()[4].constraint_ptr->is_enabled());

    /// Check the number of covered variables and variable pointers for each
    /// category.

    /// Selection
    {
        auto variable_ptrs = model.neighborhood().selection_variable_ptrs();
        EXPECT_EQ(10 + 32 + 80 + 380 + 2,
                  static_cast<int>(variable_ptrs.size()));

        /// Constraint c0
        EXPECT_EQ(true, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                   &x0(0, 0)) != variable_ptrs.end()));
        EXPECT_EQ(true, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                   &x0(0, 9)) != variable_ptrs.end()));

        /// Constraint c1
        EXPECT_EQ(true, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                   &x0(1, 0)) != variable_ptrs.end()));
        EXPECT_EQ(true, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                   &x0(1, 9)) != variable_ptrs.end()));

        EXPECT_EQ(true, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                   &x1(1, 0)) != variable_ptrs.end()));
        EXPECT_EQ(true, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                   &x1(1, 19)) != variable_ptrs.end()));

        EXPECT_EQ(true, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                   &x3(0)) != variable_ptrs.end()));

        EXPECT_EQ(true, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                   &x4(0)) != variable_ptrs.end()));

        /// Constraint c2
        EXPECT_EQ(true, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                   &x0(2, 0)) != variable_ptrs.end()));
        EXPECT_EQ(true, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                   &x0(9, 9)) != variable_ptrs.end()));

        /// Constraint c3
        EXPECT_EQ(true, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                   &x1(0, 0)) != variable_ptrs.end()));
        EXPECT_EQ(true, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                   &x1(0, 19)) != variable_ptrs.end()));
        EXPECT_EQ(true, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                   &x1(2, 0)) != variable_ptrs.end()));
        EXPECT_EQ(true, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                   &x1(19, 19)) != variable_ptrs.end()));

        /// Constraint c6
        EXPECT_EQ(true, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                   &x4(1)) != variable_ptrs.end()));
        EXPECT_EQ(true, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                   &x4(2)) != variable_ptrs.end()));
    }

    /// Binary
    {
        auto variable_ptrs = model.neighborhood().binary_variable_ptrs();
        EXPECT_EQ(10 * 10 + 20 * 20 + 1 + 2 + 3 - (10 + 32 + 80 + 380 + 2),
                  static_cast<int>(variable_ptrs.size()));

        /// Constraint c0
        EXPECT_EQ(!true, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                    &x0(0, 0)) != variable_ptrs.end()));
        EXPECT_EQ(!true, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                    &x0(0, 9)) != variable_ptrs.end()));

        /// Constraint c1
        EXPECT_EQ(!true, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                    &x0(1, 0)) != variable_ptrs.end()));
        EXPECT_EQ(!true, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                    &x0(1, 9)) != variable_ptrs.end()));

        EXPECT_EQ(!true, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                    &x1(1, 0)) != variable_ptrs.end()));
        EXPECT_EQ(!true, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                    &x1(1, 19)) != variable_ptrs.end()));

        EXPECT_EQ(!true, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                    &x3(0)) != variable_ptrs.end()));

        EXPECT_EQ(!true, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                    &x4(0)) != variable_ptrs.end()));
        /// Constraint c2
        EXPECT_EQ(!true, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                    &x0(2, 0)) != variable_ptrs.end()));
        EXPECT_EQ(!true, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                    &x0(9, 9)) != variable_ptrs.end()));

        /// Constraint c3
        EXPECT_EQ(!true, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                    &x1(0, 0)) != variable_ptrs.end()));
        EXPECT_EQ(!true, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                    &x1(0, 19)) != variable_ptrs.end()));
        EXPECT_EQ(!true, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                    &x1(2, 0)) != variable_ptrs.end()));
        EXPECT_EQ(!true, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                    &x1(19, 19)) != variable_ptrs.end()));

        /// Constraint c4
        EXPECT_EQ(!true, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                    &x4(1)) != variable_ptrs.end()));
        EXPECT_EQ(!true, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                    &x4(2)) != variable_ptrs.end()));
    }

    /// Integer
    {
        auto variable_ptrs = model.neighborhood().integer_variable_ptrs();
        EXPECT_EQ(30 * 30, static_cast<int>(variable_ptrs.size()));

        EXPECT_EQ(true, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                   &y(0, 0)) != variable_ptrs.end()));
        EXPECT_EQ(true, (std::find(variable_ptrs.begin(), variable_ptrs.end(),
                                   &y(29, 29)) != variable_ptrs.end()));
    }
}

/*****************************************************************************/
TEST_F(TestNeighborhood, setup_move_updater) {
    cppmh::model::Model<int, double> model;

    auto&                  x0 = model.create_variables("x0", {10, 10}, 0, 1);
    auto&                  x1 = model.create_variables("x1", {20, 20}, 0, 1);
    auto&                  x2 = model.create_variables("x2", 1, 0, 1);
    auto&                  x3 = model.create_variables("x3", 2, 0, 1);
    auto&                  x4 = model.create_variables("x4", 3, 0, 1);
    [[maybe_unused]] auto& y  = model.create_variables("y", {30, 30}, -10, 10);

    /// This constraint is parsed as a selection with no overlap. The
    /// corresponding constraint will be disabled.
    model.create_constraint("c0", x0.selection({0, cppmh::model::Range::All}));

    /// This constraint beyond two variable proxies is parsed as a selection
    /// with no overlap. The corresponding constraint will be disabled.
    model.create_constraint(
        "c1", (x0.sum({1, cppmh::model::Range::All}) +
               x1.sum({1, cppmh::model::Range::All}) + x3(0) + x4(0)) == 1);

    /// This constraint is parsed as a selection 20 overlap variables. The
    /// corresponding constraint will be still enabled.
    model.create_constraint("c2", x0.selection());

    /// This constraint is parsed as a selection 20 overlap variables. The
    /// corresponding constraint will be still enabled.
    model.create_constraint("c3", x1.selection());

    /// This constraint is not parsed as a selection because the number of
    /// covered variables is 1. The corresponding constraint will be still
    /// enabled.
    model.create_constraint("c4", x2.selection());

    /// This constraint is not parsed as a selection because the number of
    /// covered variables is 1 due to overlap. The corresponding constraint will
    /// be still enabled.
    model.create_constraint("c5", x3.selection());

    /// This constraint is parsed as a selection with 2 variables. The
    /// corresponding constraint will be still enabled.
    model.create_constraint("c6", x4.selection());

    y(0, 0).fix_by(0);
    y(0, 1) = -10;
    y(0, 2) = 10;

    model.setup_default_neighborhood(false);
    model.setup_has_fixed_variables();

    EXPECT_EQ(false, model.neighborhood().is_enabled_user_defined_move());

    /// Set initial values for selection variables.
    for (auto&& selection : model.neighborhood().selections()) {
        selection.variable_ptrs.front()->set_value_if_not_fixed(1);
        selection.variable_ptrs.front()->select();
    }

    model.neighborhood().update_moves();

    /// Check the variable pointers and values in raw moves.
    /// Selection
    {
        auto variable_ptrs = model.neighborhood().selection_variable_ptrs();
        auto moves         = model.neighborhood().selection_moves();
        EXPECT_EQ(variable_ptrs.size(), moves.size());
        for (auto& move : moves) {
            EXPECT_EQ(cppmh::model::MoveSense::Selection, move.sense);
            EXPECT_EQ(2, static_cast<int>(move.alterations.size()));
            EXPECT_EQ(1, move.alterations[0].first->value());
            EXPECT_EQ(0, move.alterations[0].second);
            if (move.alterations[0].first != move.alterations[1].first) {
                EXPECT_EQ(0, move.alterations[1].first->value());
                EXPECT_EQ(1, move.alterations[1].second);
            }
        }
    }

    /// Binary
    {
        auto variable_ptrs = model.neighborhood().binary_variable_ptrs();
        auto moves         = model.neighborhood().binary_moves();
        EXPECT_EQ(variable_ptrs.size(), moves.size());
        for (auto& move : moves) {
            EXPECT_EQ(cppmh::model::MoveSense::Binary, move.sense);
            EXPECT_EQ(1, static_cast<int>(move.alterations.size()));
            EXPECT_EQ(true, (move.alterations[0].first->value() == 0 ||
                             move.alterations[0].first->value() == 1));
            EXPECT_EQ(move.alterations[0].second,
                      1 - move.alterations[0].first->value());
        }
    }

    /// Integer
    {
        auto variable_ptrs = model.neighborhood().integer_variable_ptrs();
        auto moves         = model.neighborhood().integer_moves();
        EXPECT_EQ(2 * static_cast<int>(variable_ptrs.size()),
                  static_cast<int>(moves.size()));

        for (std::size_t i = 0; i < variable_ptrs.size(); i++) {
            EXPECT_EQ(1, static_cast<int>(moves[2 * i].alterations.size()));
            EXPECT_EQ(cppmh::model::MoveSense::Integer, moves[2 * i].sense);

            EXPECT_EQ(moves[2 * i].alterations[0].second,
                      moves[2 * i].alterations[0].first->value() + 1);

            EXPECT_EQ(1, static_cast<int>(moves[2 * i + 1].alterations.size()));
            EXPECT_EQ(cppmh::model::MoveSense::Integer, moves[2 * i + 1].sense);

            EXPECT_EQ(moves[2 * i + 1].alterations[0].second,
                      moves[2 * i + 1].alterations[0].first->value() - 1);
        }
    }

    /// Check the numbers of filtered moves.
    {
        int number_of_selections = model.neighborhood().selections().size();

        int number_of_selection_variables =
            model.neighborhood().selection_variable_ptrs().size();

        int number_of_binary_variables =
            model.neighborhood().binary_variable_ptrs().size();

        int number_of_integer_variables =
            model.neighborhood().integer_variable_ptrs().size();

        EXPECT_EQ(
            (number_of_selection_variables - number_of_selections)  // Selection
                + (number_of_binary_variables)                      // Binary
                + (2 * number_of_integer_variables - 2 - 1 - 1),    // Integer
            static_cast<int>(model.neighborhood().move_ptrs().size()));
    }
}

/*****************************************************************************/
TEST_F(TestNeighborhood, set_user_defined_move_updater) {
    cppmh::model::Model<int, double> model;

    int                    n = 100;
    auto&                  x = model.create_variables("x", n, 0, 1);
    [[maybe_unused]] auto& y = model.create_variables("y", n, 0, 100);
    [[maybe_unused]] auto& c = model.create_constraint("c", x.selection());

    x(0).fix_by(0);
    x(1).fix_by(1);

    auto move_updater =
        [&x, n](std::vector<cppmh::model::Move<int, double>>* a_moves) {
            a_moves->resize(n);
            for (auto i = 0; i < n; i++) {
                (*a_moves)[i].sense = cppmh::model::MoveSense::UserDefined;
                (*a_moves)[i].alterations.clear();
                (*a_moves)[i].alterations.emplace_back(&x(i), 1 - x(i).value());
            }
        };

    model.neighborhood().set_user_defined_move_updater(move_updater);
    model.neighborhood().disable_selection_move();
    model.neighborhood().disable_binary_move();
    model.neighborhood().disable_integer_move();

    model.setup_has_fixed_variables();

    model.neighborhood().update_moves();

    EXPECT_EQ(false, model.neighborhood().is_enabled_selection_move());
    EXPECT_EQ(false, model.neighborhood().is_enabled_binary_move());
    EXPECT_EQ(false, model.neighborhood().is_enabled_integer_move());
    EXPECT_EQ(true, model.neighborhood().is_enabled_user_defined_move());

    /// Check the variable pointers and values in raw moves, and the numbers of
    /// filtered moves.
    auto moves = model.neighborhood().user_defined_moves();
    EXPECT_EQ(n, static_cast<int>(moves.size()));
    for (auto& move : moves) {
        EXPECT_EQ(cppmh::model::MoveSense::UserDefined, move.sense);
        EXPECT_EQ(1, static_cast<int>(move.alterations.size()));
        EXPECT_EQ(true, (move.alterations[0].first->value() == 0 ||
                         move.alterations[0].first->value() == 1));
        EXPECT_EQ(move.alterations[0].second,
                  1 - move.alterations[0].first->value());
    }

    EXPECT_EQ(n - 2, static_cast<int>(model.neighborhood().move_ptrs().size()));
}

/*****************************************************************************/
TEST_F(TestNeighborhood, shuffle_moves) {
    cppmh::model::Model<int, double> model;

    int                    n = 100;
    auto&                  x = model.create_variables("x", n, 0, 1);
    [[maybe_unused]] auto& c = model.create_constraint("c", x.selection());

    model.setup_default_neighborhood(false);
    model.setup_has_fixed_variables();
    model.neighborhood().update_moves();

    auto         before_move_ptrs = model.neighborhood().move_ptrs();
    std::mt19937 rand;
    rand.seed(1);

    model.neighborhood().shuffle_moves(&rand);
    auto after_move_ptrs = model.neighborhood().move_ptrs();

    for (const auto& ptr : before_move_ptrs) {
        EXPECT_EQ(true,
                  std::find(after_move_ptrs.begin(), after_move_ptrs.end(),
                            ptr) != after_move_ptrs.end());
    }
}

/*****************************************************************************/
TEST_F(TestNeighborhood, selection_variable_ptrs) {
    /// This method is tested in
    /// categorize_variables_and_constraints().
}

/*****************************************************************************/
TEST_F(TestNeighborhood, binary_variable_ptrs) {
    /// This method is tested in
    /// categorize_variables_and_constraints().
}

/*****************************************************************************/
TEST_F(TestNeighborhood, integer_variable_ptrs) {
    /// This method is tested in
    /// categorize_variables_and_constraints().
}

/*****************************************************************************/
TEST_F(TestNeighborhood, selection_moves) {
    /// This method is tested in move_updater().
}

/*****************************************************************************/
TEST_F(TestNeighborhood, binary_moves) {
    /// This method is tested in move_updater().
}

/*****************************************************************************/
TEST_F(TestNeighborhood, integer_moves) {
    /// This method is tested in move_updater.
}

/*****************************************************************************/
TEST_F(TestNeighborhood, user_defined_moves) {
    /// This method is tested in set_user_defined_move_updater().
}

/*****************************************************************************/
TEST_F(TestNeighborhood, is_enabled_selection_move) {
    cppmh::model::Neighborhood<int, double> neighborhood;

    /// initial status
    EXPECT_EQ(true, neighborhood.is_enabled_selection_move());

    neighborhood.disable_selection_move();
    EXPECT_EQ(false, neighborhood.is_enabled_selection_move());

    neighborhood.enable_selection_move();
    EXPECT_EQ(true, neighborhood.is_enabled_selection_move());
}

/*****************************************************************************/
TEST_F(TestNeighborhood, enable_selection_move) {
    /// This method is tested in is_enabled_selection_move().
}

/*****************************************************************************/
TEST_F(TestNeighborhood, disable_selection_move) {
    /// This method is tested in is_enabled_selection_move().
}

/*****************************************************************************/
TEST_F(TestNeighborhood, is_enabled_binary_move) {
    cppmh::model::Neighborhood<int, double> neighborhood;

    /// initial status
    EXPECT_EQ(true, neighborhood.is_enabled_binary_move());

    neighborhood.disable_binary_move();
    EXPECT_EQ(false, neighborhood.is_enabled_binary_move());

    neighborhood.enable_binary_move();
    EXPECT_EQ(true, neighborhood.is_enabled_binary_move());
}

/*****************************************************************************/
TEST_F(TestNeighborhood, enable_binary_move) {
    /// This method is tested in is_enabled_binary_move().
}

/*****************************************************************************/
TEST_F(TestNeighborhood, disable_binary_move) {
    /// This method is tested in is_enabled_binary_move().
}

/*****************************************************************************/
TEST_F(TestNeighborhood, is_enabled_integer_move) {
    cppmh::model::Neighborhood<int, double> neighborhood;

    /// initial status
    EXPECT_EQ(true, neighborhood.is_enabled_integer_move());

    neighborhood.disable_integer_move();
    EXPECT_EQ(false, neighborhood.is_enabled_integer_move());

    neighborhood.enable_integer_move();
    EXPECT_EQ(true, neighborhood.is_enabled_integer_move());
}

/*****************************************************************************/
TEST_F(TestNeighborhood, enable_integer_move) {
    /// This method is tested in is_enabled_integer_move().
}

/*****************************************************************************/
TEST_F(TestNeighborhood, disable_integer_move) {
    /// This method is tested in is_enabled_integer_move().
}

/*****************************************************************************/
TEST_F(TestNeighborhood, is_enabled_user_defined_move) {
    cppmh::model::Neighborhood<int, double> neighborhood;

    /// initial status
    EXPECT_EQ(false, neighborhood.is_enabled_user_defined_move());

    neighborhood.enable_user_defined_move();
    EXPECT_EQ(true, neighborhood.is_enabled_user_defined_move());

    neighborhood.disable_user_defined_move();
    EXPECT_EQ(false, neighborhood.is_enabled_user_defined_move());
}

/*****************************************************************************/
TEST_F(TestNeighborhood, enable_user_defined_move) {
    /// This method is tested in is_enabled_user_defined_move().
}

/*****************************************************************************/
TEST_F(TestNeighborhood, disable_user_defined_move) {
    /// This method is tested in is_enabled_user_defined_move().
}

/*****************************************************************************/
}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/