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
TEST_F(TestNeighborhood, set_has_fixed_variables) {
    cppmh::model::Neighborhood<int, double> neighborhood;
    EXPECT_EQ(false, neighborhood.has_fixed_variables());

    neighborhood.set_has_fixed_variables(true);
    EXPECT_EQ(true, neighborhood.has_fixed_variables());

    neighborhood.set_has_fixed_variables(false);
    EXPECT_EQ(false, neighborhood.has_fixed_variables());
}

/*****************************************************************************/
TEST_F(TestNeighborhood, setup_move_updater) {
    cppmh::model::Model<int, double> model;

    auto& x0 = model.create_variables("x0", {10, 10}, 0, 1);
    auto& x1 = model.create_variables("x1", {20, 20}, 0, 1);
    auto& x2 = model.create_variables("x2", 2, 0, 1);

    auto& y = model.create_variables("y", {30, 30}, -10, 10);

    /**
     * Selection constraint with 10 decision variables. The priority of this
     * constraint is the third, and it will be employed for a swap
     * neighborhood.
     */
    model.create_constraint("c0", x0.selection({0, cppmh::model::Range::All}));

    /**
     * Selection constraint with 32 decision variables. The priority of this
     * constraint is the second, and it will NOT be employed for a swap
     * neighborhood because higher-priority constraint c1 covers x1.
     */
    model.create_constraint(
        "c1", (x0.sum({1, cppmh::model::Range::All}) +
               x1.sum({1, cppmh::model::Range::All}) + x2(0)) == 1);

    /**
     * Selection constraint with 400 decision variables. The priority of this
     * constraint is the first, and it will be employed for a swap
     * neighborhood.
     */
    model.create_constraint("c2", x1.selection());

    /**
     * Selection constraint with 2 decision variables. The priority NOT be
     * employed for a swap neighborhood.
     */
    model.create_constraint("c3", x2.selection());

    y(0, 0).fix_by(0);
    y(0, 1) = -10;
    y(0, 2) = 10;

    model.categorize_variables();
    model.categorize_constraints();
    model.extract_selections(cppmh::model::SelectionMode::Larger);
    model.setup_default_neighborhood(false, false);

    model.neighborhood().set_has_fixed_variables(true);
    EXPECT_EQ(false, model.neighborhood().is_enabled_user_defined_move());

    /**
     * Set initial values for selection variables.
     */
    for (auto&& selection : model.selections()) {
        selection.variable_ptrs.front()->set_value_if_not_fixed(1);
        selection.variable_ptrs.front()->select();
    }

    model.neighborhood().update_moves();

    /**
     * Check the variable pointers and values in raw moves.
     */
    /// Selection
    {
        auto variable_ptrs = model.variable_reference().selection_variable_ptrs;
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

            for (auto& constraint_ptr :
                 move.alterations[0].first->related_constraint_ptrs()) {
                EXPECT_EQ(true,
                          move.related_constraint_ptrs.find(constraint_ptr) !=
                              move.related_constraint_ptrs.end());
            }

            for (auto& constraint_ptr :
                 move.alterations[1].first->related_constraint_ptrs()) {
                EXPECT_EQ(true,
                          move.related_constraint_ptrs.find(constraint_ptr) !=
                              move.related_constraint_ptrs.end());
            }
        }
    }

    /// Binary
    {
        auto variable_ptrs = model.variable_reference().binary_variable_ptrs;
        auto moves         = model.neighborhood().binary_moves();
        EXPECT_EQ(variable_ptrs.size(), moves.size());

        for (auto& move : moves) {
            EXPECT_EQ(cppmh::model::MoveSense::Binary, move.sense);
            EXPECT_EQ(1, static_cast<int>(move.alterations.size()));
            EXPECT_EQ(true, (move.alterations[0].first->value() == 0 ||
                             move.alterations[0].first->value() == 1));
            EXPECT_EQ(move.alterations[0].second,
                      1 - move.alterations[0].first->value());

            EXPECT_EQ(true, move.alterations[0].second);

            for (auto& constraint_ptr :
                 move.alterations[0].first->related_constraint_ptrs()) {
                EXPECT_EQ(true,
                          move.related_constraint_ptrs.find(constraint_ptr) !=
                              move.related_constraint_ptrs.end());
            }
        }
    }

    /// Integer
    {
        auto variable_ptrs = model.variable_reference().integer_variable_ptrs;
        auto moves         = model.neighborhood().integer_moves();
        EXPECT_EQ(2 * static_cast<int>(variable_ptrs.size()),
                  static_cast<int>(moves.size()));

        int variable_ptrs_size = variable_ptrs.size();
        for (auto i = 0; i < variable_ptrs_size; i++) {
            EXPECT_EQ(1, static_cast<int>(moves[2 * i].alterations.size()));
            EXPECT_EQ(cppmh::model::MoveSense::Integer, moves[2 * i].sense);

            EXPECT_EQ(moves[2 * i].alterations[0].second,
                      moves[2 * i].alterations[0].first->value() + 1);

            for (auto& constraint_ptr :
                 moves[2 * i].alterations[0].first->related_constraint_ptrs()) {
                EXPECT_EQ(true, moves[2 * i].related_constraint_ptrs.find(
                                    constraint_ptr) !=
                                    moves[2 * i].related_constraint_ptrs.end());
            }

            EXPECT_EQ(1, static_cast<int>(moves[2 * i + 1].alterations.size()));
            EXPECT_EQ(cppmh::model::MoveSense::Integer, moves[2 * i + 1].sense);

            EXPECT_EQ(moves[2 * i + 1].alterations[0].second,
                      moves[2 * i + 1].alterations[0].first->value() - 1);

            for (auto& constraint_ptr : moves[2 * i + 1]
                                            .alterations[0]
                                            .first->related_constraint_ptrs()) {
                EXPECT_EQ(true,
                          moves[2 * i + 1].related_constraint_ptrs.find(
                              constraint_ptr) !=
                              moves[2 * i + 1].related_constraint_ptrs.end());
            }
        }
    }

    /**
     * Check the numbers of filtered moves. *
     */
    {
        int selections_size = model.selections().size();

        int selection_variables_size =
            model.variable_reference().selection_variable_ptrs.size();

        int binary_variables_size =
            model.variable_reference().binary_variable_ptrs.size();

        int integer_variables_size =
            model.variable_reference().integer_variable_ptrs.size();

        EXPECT_EQ((selection_variables_size - selections_size)     // Selection
                      + (binary_variables_size)                    // Binary
                      + (2 * integer_variables_size - 2 - 1 - 1),  // Integer
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

    model.categorize_variables();
    model.categorize_constraints();
    model.extract_selections(cppmh::model::SelectionMode::Larger);

    model.neighborhood().set_has_fixed_variables(true);
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

    model.setup_default_neighborhood(false, false);
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