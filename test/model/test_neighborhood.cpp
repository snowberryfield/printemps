/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <gtest/gtest.h>
#include <random>

#include <printemps.h>

namespace {
/*****************************************************************************/
class TestNeighborhood : public ::testing::Test {
   protected:
    printemps::utility::IntegerUniformRandom m_random_integer;
    printemps::utility::IntegerUniformRandom m_random_positive_integer;

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
    printemps::model::Neighborhood<int, double> neighborhood;

    EXPECT_EQ(true, neighborhood.binary_moves().empty());
    EXPECT_EQ(true, neighborhood.integer_moves().empty());
    EXPECT_EQ(true, neighborhood.aggregation_moves().empty());
    EXPECT_EQ(true, neighborhood.precedence_moves().empty());
    EXPECT_EQ(true, neighborhood.variable_bound_moves().empty());
    EXPECT_EQ(true, neighborhood.user_defined_moves().empty());
    EXPECT_EQ(true, neighborhood.selection_moves().empty());

    EXPECT_EQ(true, neighborhood.move_ptrs().empty());

    EXPECT_EQ(false, neighborhood.has_selection_variables());
    EXPECT_EQ(false, neighborhood.has_fixed_variables());

    EXPECT_EQ(false, neighborhood.is_enabled_binary_move());
    EXPECT_EQ(false, neighborhood.is_enabled_integer_move());
    EXPECT_EQ(false, neighborhood.is_enabled_aggregation_move());
    EXPECT_EQ(false, neighborhood.is_enabled_precedence_move());
    EXPECT_EQ(false, neighborhood.is_enabled_variable_bound_move());
    EXPECT_EQ(false, neighborhood.is_enabled_user_defined_move());
    EXPECT_EQ(false, neighborhood.is_enabled_selection_move());

    /// Tests for updater functions are omitted.
}

/*****************************************************************************/
TEST_F(TestNeighborhood, set_has_fixed_variables) {
    printemps::model::Neighborhood<int, double> neighborhood;
    EXPECT_EQ(false, neighborhood.has_fixed_variables());

    neighborhood.set_has_fixed_variables(true);
    EXPECT_EQ(true, neighborhood.has_fixed_variables());

    neighborhood.set_has_fixed_variables(false);
    EXPECT_EQ(false, neighborhood.has_fixed_variables());
}

/*****************************************************************************/
TEST_F(TestNeighborhood, set_has_selection_variables) {
    printemps::model::Neighborhood<int, double> neighborhood;
    EXPECT_EQ(false, neighborhood.has_selection_variables());

    neighborhood.set_has_selection_variables(true);
    EXPECT_EQ(true, neighborhood.has_selection_variables());

    neighborhood.set_has_selection_variables(false);
    EXPECT_EQ(false, neighborhood.has_selection_variables());
}

/*****************************************************************************/
TEST_F(TestNeighborhood, setup_move_updater) {
    printemps::model::Model<int, double> model;

    auto& x_0 = model.create_variables("x_0", {10, 10}, 0, 1);
    auto& x_1 = model.create_variables("x_1", {20, 20}, 0, 1);
    auto& x_2 = model.create_variables("x_2", 2, 0, 1);

    auto& y = model.create_variables("y", {30, 30}, -10, 10);
    auto& z = model.create_variables("z", 2, -10, 10);

    /**
     * Set partitioning constraint with 10 decision variables. The priority of
     * this constraint is the third in determining selection neighborhoods,
     * and it will be employed.
     */
    model.create_constraint("c_0",
                            x_0.selection({0, printemps::model::Range::All}));

    /**
     * Set partitioning  constraint with 31 decision variables. The priority of
     * this constraint is the second in determining selection neighborhoods,
     * and it will NOT be employed because higher-priority constraint c_1 has
     * already covered x_1.
     */
    model.create_constraint(
        "c_1", (x_0.sum({1, printemps::model::Range::All}) +
                x_1.sum({1, printemps::model::Range::All}) + x_2(0)) == 1);

    /**
     * Set partitioning constraint with 400 decision variables. The priority of
     * this constraint is the first in determining selection neighborhoods,
     * and it will be employed.
     */
    model.create_constraint("c_2", x_1.selection());

    /**
     * Set partitioning constraint with 2 decision variables. This constraint is
     * not the candidate for a selection neighborhood.
     */
    model.create_constraint("c_3", x_2.selection());

    /// Aggregation constraints.
    model.create_constraint("c_4", x_2(0) + x_2(1) == 1);  // eff. : 0
    model.create_constraint("c_5", z(0) + 4 * z(1) == 8);  // eff. : 3

    /// Precedence constraints.
    model.create_constraint("c_6", x_2(0) - x_2(1) <= 1);   // eff. : 1
    model.create_constraint("c_7", -x_2(0) + x_2(1) <= 1);  // eff. : 1
    model.create_constraint("c_8", x_2(0) - x_2(1) >= 1);   // eff. : 1
    model.create_constraint("c_9", -x_2(0) + x_2(1) >= 1);  // eff. : 1
    model.create_constraint("c_10", z(0) - z(1) <= 10);     // eff. : 2
    model.create_constraint("c_11", -z(0) + z(1) <= 10);    // eff. : 2
    model.create_constraint("c_12", z(0) - z(1) >= 10);     // eff. : 2
    model.create_constraint("c_13", -z(0) + z(1) >= 10);    // eff. : 2

    /// Variable bound constraints.
    model.create_constraint("c_14", 3 * z(0) + 10 * z(1) <= 20);  // eff. : 4
    model.create_constraint("c_15", 3 * z(0) - 10 * z(1) <= 20);  // eff. : 4
    model.create_constraint("c_16", 3 * z(0) + 10 * z(1) >= 20);  // eff. : 4
    model.create_constraint("c_17", 3 * z(0) - 10 * z(1) >= 20);  // eff. : 4

    /// Set packing constraints.
    model.create_constraint("c_18",
                            x_0.sum({2, printemps::model::Range::All}) <= 1);

    y(0, 0).fix_by(0);
    y(0, 1) = -10;
    y(0, 2) = 10;

    model.setup_unique_name();
    model.categorize_variables();
    model.categorize_constraints();
    model.extract_selections(printemps::model::SelectionMode::Larger);

    model.setup_neighborhood(true, true, true, true, false, false, false);

    model.neighborhood().set_has_fixed_variables(true);
    model.neighborhood().set_has_selection_variables(true);

    model.neighborhood().enable_binary_move();
    model.neighborhood().enable_integer_move();
    model.neighborhood().enable_aggregation_move();
    model.neighborhood().enable_precedence_move();
    model.neighborhood().enable_variable_bound_move();
    model.neighborhood().enable_exclusive_move();
    model.neighborhood().enable_selection_move();

    /**
     * Set initial values for selection variables.
     */
    for (auto&& selection : model.selections()) {
        selection.variable_ptrs.front()->set_value_if_not_fixed(1);
        selection.variable_ptrs.front()->select();
    }

    model.neighborhood().update_moves(true, false, false, false);

    /**
     * Check the variable pointers and values in raw moves.
     */
    /// Binary
    {
        auto variable_ptrs = model.variable_reference().binary_variable_ptrs;

        std::vector<printemps::model::Variable<int, double>*>
            not_fixed_variable_ptrs;
        for (auto&& variable_ptr : variable_ptrs) {
            if (!variable_ptr->is_fixed()) {
                not_fixed_variable_ptrs.push_back(variable_ptr);
            }
        }

        auto moves = model.neighborhood().binary_moves();
        auto flags = model.neighborhood().binary_move_flags();
        EXPECT_EQ(not_fixed_variable_ptrs.size(), moves.size());

        for (auto& move : moves) {
            EXPECT_EQ(printemps::model::MoveSense::Binary, move.sense);
            EXPECT_EQ(false, move.alterations[0].first->is_fixed());

            EXPECT_EQ(move.alterations[0].second,
                      1 - move.alterations[0].first->value());

            EXPECT_EQ(1, static_cast<int>(move.alterations.size()));
            EXPECT_EQ(true, (move.alterations[0].first->value() == 0 ||
                             move.alterations[0].first->value() == 1));

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

        std::vector<printemps::model::Variable<int, double>*>
            not_fixed_variable_ptrs;
        for (auto&& variable_ptr : variable_ptrs) {
            if (!variable_ptr->is_fixed()) {
                not_fixed_variable_ptrs.push_back(variable_ptr);
            }
        }

        auto moves = model.neighborhood().integer_moves();
        auto flags = model.neighborhood().integer_move_flags();
        EXPECT_EQ(4 * static_cast<int>(not_fixed_variable_ptrs.size()),
                  static_cast<int>(moves.size()));

        int variable_size = not_fixed_variable_ptrs.size();
        for (auto i = 0; i < variable_size; i++) {
            for (auto j = 0; j < 4; j++) {
                EXPECT_EQ(
                    1, static_cast<int>(moves[4 * i + j].alterations.size()));
                EXPECT_EQ(printemps::model::MoveSense::Integer,
                          moves[4 * i + j].sense);
                EXPECT_EQ(false,
                          moves[4 * i + j].alterations[0].first->is_fixed());

                for (auto& constraint_ptr :
                     moves[4 * i + j]
                         .alterations[0]
                         .first->related_constraint_ptrs()) {
                    EXPECT_EQ(
                        true,
                        moves[4 * i + j].related_constraint_ptrs.find(
                            constraint_ptr) !=
                            moves[4 * i + j].related_constraint_ptrs.end());
                }
            }

            if (moves[4 * i].alterations.front().first->value() ==
                moves[4 * i].alterations.front().first->upper_bound()) {
                EXPECT_EQ(0, flags[4 * i]);
            } else {
                EXPECT_EQ(1, flags[4 * i]);
                EXPECT_EQ(moves[4 * i].alterations[0].second,
                          moves[4 * i].alterations[0].first->value() + 1);
            }

            if (moves[4 * i + 1].alterations.front().first->value() ==
                moves[4 * i + 1].alterations.front().first->lower_bound()) {
                EXPECT_EQ(0, flags[4 * i + 1]);
            } else {
                EXPECT_EQ(1, flags[4 * i + 1]);
                EXPECT_EQ(moves[4 * i + 1].alterations[0].second,
                          moves[4 * i + 1].alterations[0].first->value() - 1);
            }

            if (moves[4 * i + 2].alterations.front().first->value() ==
                moves[4 * i + 2].alterations.front().first->upper_bound()) {
                EXPECT_EQ(0, flags[4 * i + 2]);
            } else {
                EXPECT_EQ(1, flags[4 * i + 2]);
                EXPECT_EQ(
                    moves[4 * i + 2].alterations[0].second,
                    (moves[4 * i + 2].alterations[0].first->value() +
                     moves[4 * i + 2].alterations[0].first->upper_bound()) /
                        2);
            }

            if (moves[4 * i + 3].alterations.front().first->value() ==
                moves[4 * i + 3].alterations.front().first->lower_bound()) {
                EXPECT_EQ(0, flags[4 * i + 3]);
            } else {
                EXPECT_EQ(1, flags[4 * i + 3]);
                EXPECT_EQ(
                    moves[4 * i + 3].alterations[0].second,
                    (moves[4 * i + 3].alterations[0].first->value() +
                     moves[4 * i + 3].alterations[0].first->lower_bound()) /
                        2);
            }
        }
    }

    /// Aggregation
    {
        auto constraint_ptrs =
            model.constraint_type_reference().aggregation_ptrs;
        auto moves = model.neighborhood().aggregation_moves();
        auto flags = model.neighborhood().aggregation_move_flags();
        EXPECT_EQ(4 * constraint_ptrs.size(), moves.size());
        EXPECT_EQ(4 * constraint_ptrs.size(), flags.size());
    }

    /// Precedence
    {
        auto constraint_ptrs =
            model.constraint_type_reference().precedence_ptrs;
        auto moves = model.neighborhood().precedence_moves();
        auto flags = model.neighborhood().precedence_move_flags();
        EXPECT_EQ(2 * constraint_ptrs.size(), moves.size());
        EXPECT_EQ(2 * constraint_ptrs.size(), flags.size());
    }

    /// Variable Bound
    {
        auto constraint_ptrs =
            model.constraint_type_reference().variable_bound_ptrs;
        auto moves = model.neighborhood().variable_bound_moves();
        EXPECT_EQ(4 * constraint_ptrs.size(), moves.size());
    }

    /// Exclusive
    {
        auto moves = model.neighborhood().exclusive_moves();
        auto flags = model.neighborhood().exclusive_move_flags();

        /// x_0(1,0), ..., x(1,9),
        /// x_0(2,0), ..., x(1,9),
        /// x_2(0)
        EXPECT_EQ(21, static_cast<int>(moves.size()));
        EXPECT_EQ(21, static_cast<int>(flags.size()));
    }

    /// Selection
    {
        auto variable_ptrs = model.variable_reference().selection_variable_ptrs;
        auto moves         = model.neighborhood().selection_moves();
        auto flags         = model.neighborhood().selection_moves();
        EXPECT_EQ(variable_ptrs.size(), moves.size());
        EXPECT_EQ(variable_ptrs.size(), flags.size());
        for (auto& move : moves) {
            EXPECT_EQ(printemps::model::MoveSense::Selection, move.sense);
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

    /**
     * Check the numbers of filtered moves. *
     */
    {
        const int SELECTIONS_SIZE = model.selections().size();

        const int BINARY_VARIABLES_SIZE =
            model.variable_reference().binary_variable_ptrs.size();

        const int INTEGER_VARIABLES_SIZE =
            model.variable_reference().integer_variable_ptrs.size();

        const int AGGREGATIONS_SIZE =
            model.constraint_type_reference().aggregation_ptrs.size();

        const int PRECEDENCES_SIZE =
            model.constraint_type_reference().precedence_ptrs.size();

        const int VARIABLE_BOUNDS_SIZE =
            model.constraint_type_reference().variable_bound_ptrs.size();

        const int SELECTIONS_VARIABLES_SIZE =
            model.variable_reference().selection_variable_ptrs.size();

        EXPECT_EQ(                                          //
            (BINARY_VARIABLES_SIZE)                         // Binary
                + (4 * INTEGER_VARIABLES_SIZE - 4 - 2 - 2)  // Integer
                + (4 * AGGREGATIONS_SIZE - 5)               // Aggregation
                + (2 * PRECEDENCES_SIZE - 4)                // Precedence
                + (4 * VARIABLE_BOUNDS_SIZE)                // Variable Bound
                + (21)                                      // Exclusive
                + (SELECTIONS_VARIABLES_SIZE - SELECTIONS_SIZE),  // Selection
            static_cast<int>(model.neighborhood().move_ptrs().size()));
    }
}

/*****************************************************************************/
TEST_F(TestNeighborhood, register_chain_move) {
    printemps::model::Model<int, double> model;
    printemps::model::Move<int, double>  move;

    model.neighborhood().register_chain_move(move);

    EXPECT_EQ(1, static_cast<int>(model.neighborhood().chain_moves().size()));
    EXPECT_EQ(1,
              static_cast<int>(model.neighborhood().chain_move_flags().size()));

    model.neighborhood().clear_chain_moves();
    EXPECT_EQ(true, model.neighborhood().chain_moves().empty());
    EXPECT_EQ(true, model.neighborhood().chain_move_flags().empty());
}

/*****************************************************************************/
TEST_F(TestNeighborhood, clear_chain_moves) {
    /// This method is tested in register_chain_move().
}

/*****************************************************************************/
TEST_F(TestNeighborhood, deduplicate_chain_moves) {
    printemps::model::Model<int, double> model;

    printemps::model::Move<int, double> move_0;
    move_0.overlap_rate = 0.1;

    printemps::model::Move<int, double> move_1;
    move_1.overlap_rate = 0.2;

    printemps::model::Move<int, double> move_2;
    move_2.overlap_rate = 0.3;

    for (auto i = 0; i < 3; i++) {
        model.neighborhood().register_chain_move(move_0);
        model.neighborhood().register_chain_move(move_1);
        model.neighborhood().register_chain_move(move_2);
    }
    EXPECT_EQ(9, static_cast<int>(model.neighborhood().chain_moves().size()));
    EXPECT_EQ(9,
              static_cast<int>(model.neighborhood().chain_move_flags().size()));

    model.neighborhood().sort_chain_moves();
    EXPECT_EQ(0.3, model.neighborhood().chain_moves()[0].overlap_rate);
    EXPECT_EQ(0.3, model.neighborhood().chain_moves()[1].overlap_rate);
    EXPECT_EQ(0.3, model.neighborhood().chain_moves()[2].overlap_rate);
    EXPECT_EQ(0.2, model.neighborhood().chain_moves()[3].overlap_rate);
    EXPECT_EQ(0.2, model.neighborhood().chain_moves()[4].overlap_rate);
    EXPECT_EQ(0.2, model.neighborhood().chain_moves()[5].overlap_rate);
    EXPECT_EQ(0.1, model.neighborhood().chain_moves()[6].overlap_rate);
    EXPECT_EQ(0.1, model.neighborhood().chain_moves()[7].overlap_rate);
    EXPECT_EQ(0.1, model.neighborhood().chain_moves()[8].overlap_rate);

    model.neighborhood().deduplicate_chain_moves();
    EXPECT_EQ(3, static_cast<int>(model.neighborhood().chain_moves().size()));
    EXPECT_EQ(3,
              static_cast<int>(model.neighborhood().chain_move_flags().size()));
}

/*****************************************************************************/
TEST_F(TestNeighborhood, sort_chain_moves) {
    /// This method is tested in deduplicate_chain_moves().
}

/*****************************************************************************/
TEST_F(TestNeighborhood, shuffle_chain_moves) {
    /// This test is skipped.
}

/*****************************************************************************/
TEST_F(TestNeighborhood, reduce_chain_moves) {
    printemps::model::Model<int, double> model;
    printemps::model::Move<int, double>  move;

    for (auto i = 0; i < 5000; i++) {
        model.neighborhood().register_chain_move(move);
    }

    const int CHAIN_MOVE_CAPACITY = 10000;

    model.neighborhood().reduce_chain_moves(CHAIN_MOVE_CAPACITY);
    EXPECT_EQ(5000,
              static_cast<int>(model.neighborhood().chain_moves().size()));
    EXPECT_EQ(5000,
              static_cast<int>(model.neighborhood().chain_move_flags().size()));

    for (auto i = 0; i < 5000; i++) {
        model.neighborhood().register_chain_move(move);
    }

    model.neighborhood().reduce_chain_moves(CHAIN_MOVE_CAPACITY);
    EXPECT_EQ(CHAIN_MOVE_CAPACITY,
              static_cast<int>(model.neighborhood().chain_moves().size()));
    EXPECT_EQ(CHAIN_MOVE_CAPACITY,
              static_cast<int>(model.neighborhood().chain_move_flags().size()));

    for (auto i = 0; i < 5000; i++) {
        model.neighborhood().register_chain_move(move);
    }

    model.neighborhood().reduce_chain_moves(CHAIN_MOVE_CAPACITY);
    EXPECT_EQ(CHAIN_MOVE_CAPACITY,
              static_cast<int>(model.neighborhood().chain_moves().size()));
    EXPECT_EQ(CHAIN_MOVE_CAPACITY,
              static_cast<int>(model.neighborhood().chain_move_flags().size()));
}

/*****************************************************************************/
TEST_F(TestNeighborhood, set_user_defined_move_updater) {
    printemps::model::Model<int, double> model;

    int   n = 100;
    auto& x = model.create_variables("x", n, 0, 1);

    x(0).fix_by(0);
    x(1).fix_by(1);

    auto move_updater =
        [&x, n](std::vector<printemps::model::Move<int, double>>* a_moves) {
            a_moves->resize(n);
            for (auto i = 0; i < n; i++) {
                (*a_moves)[i].sense = printemps::model::MoveSense::UserDefined;
                (*a_moves)[i].alterations.clear();
                (*a_moves)[i].alterations.emplace_back(&x(i), 1 - x(i).value());
            }
        };

    model.neighborhood().set_user_defined_move_updater(move_updater);
    model.categorize_variables();
    model.categorize_constraints();

    model.setup_neighborhood(false, false, false, false, false, true, false);

    model.neighborhood().set_has_fixed_variables(true);
    model.neighborhood().set_has_selection_variables(false);

    model.neighborhood().enable_user_defined_move();
    model.neighborhood().update_moves(true, false, false, false);

    /// Check the variable pointers and values in raw moves, and the numbers of
    /// filtered moves.
    auto moves = model.neighborhood().user_defined_moves();
    EXPECT_EQ(n, static_cast<int>(moves.size()));
    for (auto& move : moves) {
        EXPECT_EQ(printemps::model::MoveSense::UserDefined, move.sense);
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
    printemps::model::Model<int, double> model;

    int                    n = 100;
    auto&                  x = model.create_variables("x", n, 0, 1);
    [[maybe_unused]] auto& c = model.create_constraint("c", x.selection());

    model.setup_neighborhood(true, true, true, true, false, false, false);
    model.neighborhood().update_moves(true, false, false, false);

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
TEST_F(TestNeighborhood, binary_moves) {
    /// This method is tested in move_updater().
}

/*****************************************************************************/
TEST_F(TestNeighborhood, binary_move_flags) {
    /// This method is tested in move_updater().
}

/*****************************************************************************/
TEST_F(TestNeighborhood, integer_moves) {
    /// This method is tested in move_updater().
}

/*****************************************************************************/
TEST_F(TestNeighborhood, integer_move_flags) {
    /// This method is tested in move_updater().
}

/*****************************************************************************/
TEST_F(TestNeighborhood, aggregation_moves) {
    /// This method is tested in move_updater().
}

/*****************************************************************************/
TEST_F(TestNeighborhood, aggregation_move_flags) {
    /// This method is tested in move_updater().
}

/*****************************************************************************/
TEST_F(TestNeighborhood, precedence_moves) {
    /// This method is tested in move_updater().
}

/*****************************************************************************/
TEST_F(TestNeighborhood, precedence_move_flags) {
    /// This method is tested in move_updater().
}

/*****************************************************************************/
TEST_F(TestNeighborhood, variable_bound_moves) {
    /// This method is tested in move_updater().
}

/*****************************************************************************/
TEST_F(TestNeighborhood, variable_bound_move_flags) {
    /// This method is tested in move_updater().
}

/*****************************************************************************/
TEST_F(TestNeighborhood, selection_moves) {
    /// This method is tested in move_updater().
}

/*****************************************************************************/
TEST_F(TestNeighborhood, selection_move_flags) {
    /// This method is tested in move_updater().
}

/*****************************************************************************/
TEST_F(TestNeighborhood, chain_moves) {
    /// This method is tested in register_chain_move().
}

/*****************************************************************************/
TEST_F(TestNeighborhood, chain_move_flags) {
    /// This method is tested in register_chain_move().
}

/*****************************************************************************/
TEST_F(TestNeighborhood, user_defined_moves) {
    /// This method is tested in set_user_defined_move_updater().
}

/*****************************************************************************/
TEST_F(TestNeighborhood, user_defined_move_flags) {
    /// This method is tested in set_user_defined_move_updater().
}

/*****************************************************************************/
TEST_F(TestNeighborhood, is_enabled_binary_move) {
    printemps::model::Neighborhood<int, double> neighborhood;

    /// initial status
    EXPECT_EQ(false, neighborhood.is_enabled_binary_move());

    neighborhood.enable_binary_move();
    EXPECT_EQ(true, neighborhood.is_enabled_binary_move());

    neighborhood.disable_binary_move();
    EXPECT_EQ(false, neighborhood.is_enabled_binary_move());
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
    printemps::model::Neighborhood<int, double> neighborhood;

    /// initial status
    EXPECT_EQ(false, neighborhood.is_enabled_integer_move());

    neighborhood.enable_integer_move();
    EXPECT_EQ(true, neighborhood.is_enabled_integer_move());

    neighborhood.disable_integer_move();
    EXPECT_EQ(false, neighborhood.is_enabled_integer_move());
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
    printemps::model::Neighborhood<int, double> neighborhood;

    /// initial status
    EXPECT_EQ(false, neighborhood.is_enabled_user_defined_move());

    neighborhood.disable_user_defined_move();
    EXPECT_EQ(false, neighborhood.is_enabled_user_defined_move());

    neighborhood.enable_user_defined_move();
    EXPECT_EQ(true, neighborhood.is_enabled_user_defined_move());
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
TEST_F(TestNeighborhood, is_enabled_aggregation_move) {
    printemps::model::Neighborhood<int, double> neighborhood;

    /// initial status
    EXPECT_EQ(false, neighborhood.is_enabled_aggregation_move());

    neighborhood.disable_aggregation_move();
    EXPECT_EQ(false, neighborhood.is_enabled_aggregation_move());

    neighborhood.enable_aggregation_move();
    EXPECT_EQ(true, neighborhood.is_enabled_aggregation_move());
}

/*****************************************************************************/
TEST_F(TestNeighborhood, enable_aggregation_move) {
    /// This method is tested in is_enabled_aggregation_move().
}

/*****************************************************************************/
TEST_F(TestNeighborhood, disable_aggregation_move) {
    /// This method is tested in is_enabled_aggregation_move().
}

/*****************************************************************************/
TEST_F(TestNeighborhood, is_enabled_precedence_move) {
    printemps::model::Neighborhood<int, double> neighborhood;

    /// initial status
    EXPECT_EQ(false, neighborhood.is_enabled_precedence_move());

    neighborhood.disable_precedence_move();
    EXPECT_EQ(false, neighborhood.is_enabled_precedence_move());

    neighborhood.enable_precedence_move();
    EXPECT_EQ(true, neighborhood.is_enabled_precedence_move());
}

/*****************************************************************************/
TEST_F(TestNeighborhood, enable_precedence_move) {
    /// This method is tested in is_enabled_precedence_move().
}

/*****************************************************************************/
TEST_F(TestNeighborhood, disable_precedence_move) {
    /// This method is tested in is_enabled_precedence_move().
}

/*****************************************************************************/
TEST_F(TestNeighborhood, is_enabled_variable_bound_move) {
    printemps::model::Neighborhood<int, double> neighborhood;

    /// initial status
    EXPECT_EQ(false, neighborhood.is_enabled_variable_bound_move());

    neighborhood.disable_variable_bound_move();
    EXPECT_EQ(false, neighborhood.is_enabled_variable_bound_move());

    neighborhood.enable_variable_bound_move();
    EXPECT_EQ(true, neighborhood.is_enabled_variable_bound_move());
}

/*****************************************************************************/
TEST_F(TestNeighborhood, enable_variable_bound_move) {
    /// This method is tested in is_enabled_variable_bound_move().
}

/*****************************************************************************/
TEST_F(TestNeighborhood, disable_variable_bound_move) {
    /// This method is tested in is_enabled_variable_bound_move().
}

/*****************************************************************************/
TEST_F(TestNeighborhood, is_enabled_selection_move) {
    printemps::model::Neighborhood<int, double> neighborhood;

    /// initial status
    EXPECT_EQ(false, neighborhood.is_enabled_selection_move());

    neighborhood.enable_selection_move();
    EXPECT_EQ(true, neighborhood.is_enabled_selection_move());

    neighborhood.disable_selection_move();
    EXPECT_EQ(false, neighborhood.is_enabled_selection_move());
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
TEST_F(TestNeighborhood, is_enabled_exclusive_move) {
    printemps::model::Neighborhood<int, double> neighborhood;

    /// initial status
    EXPECT_EQ(false, neighborhood.is_enabled_exclusive_move());

    neighborhood.disable_exclusive_move();
    EXPECT_EQ(false, neighborhood.is_enabled_exclusive_move());

    neighborhood.enable_exclusive_move();
    EXPECT_EQ(true, neighborhood.is_enabled_exclusive_move());
}

/*****************************************************************************/
TEST_F(TestNeighborhood, enable_exclusive_move) {
    /// This method is tested in is_enabled_exclusive_move().
}

/*****************************************************************************/
TEST_F(TestNeighborhood, disable_exclusive_move) {
    /// This method is tested in is_enabled_exclusive_move().
}

/*****************************************************************************/
TEST_F(TestNeighborhood, reset_special_neighborhood_moves_availability) {
    /// This test is skipped.
}

/*****************************************************************************/
TEST_F(TestNeighborhood, is_enabled_special_neighborhood_move) {
    printemps::model::Neighborhood<int, double> neighborhood;

    /// initial status
    EXPECT_EQ(false, neighborhood.is_enabled_special_neighborhood_move());

    /// Aggregation
    neighborhood.enable_aggregation_move();
    EXPECT_EQ(true, neighborhood.is_enabled_special_neighborhood_move());
    neighborhood.disable_aggregation_move();
    EXPECT_EQ(false, neighborhood.is_enabled_special_neighborhood_move());

    /// Precedence
    neighborhood.enable_precedence_move();
    EXPECT_EQ(true, neighborhood.is_enabled_special_neighborhood_move());
    neighborhood.disable_precedence_move();
    EXPECT_EQ(false, neighborhood.is_enabled_special_neighborhood_move());

    /// Variable Bound
    neighborhood.enable_variable_bound_move();
    EXPECT_EQ(true, neighborhood.is_enabled_special_neighborhood_move());
    neighborhood.disable_variable_bound_move();
    EXPECT_EQ(false, neighborhood.is_enabled_special_neighborhood_move());

    /// Exclusive
    neighborhood.enable_exclusive_move();
    EXPECT_EQ(true, neighborhood.is_enabled_special_neighborhood_move());
    neighborhood.disable_exclusive_move();
    EXPECT_EQ(false, neighborhood.is_enabled_special_neighborhood_move());

    /// Chain
    neighborhood.enable_chain_move();
    EXPECT_EQ(true, neighborhood.is_enabled_special_neighborhood_move());
    neighborhood.disable_chain_move();
    EXPECT_EQ(false, neighborhood.is_enabled_special_neighborhood_move());
}

/*****************************************************************************/
}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/