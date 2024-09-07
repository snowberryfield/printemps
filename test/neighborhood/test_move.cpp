/*****************************************************************************/
// Copyright (c) 2020-2024 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <gtest/gtest.h>
#include <printemps.h>

namespace {
using namespace printemps;
/*****************************************************************************/
class TestMove : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestMove, constructor) {
    neighborhood::Move<int, double> move;

    EXPECT_TRUE(move.alterations.empty());
    EXPECT_EQ(neighborhood::MoveSense::General, move.sense);
    EXPECT_TRUE(move.related_constraint_ptrs.empty());
    EXPECT_FALSE(move.is_univariable_move);
    EXPECT_FALSE(move.is_special_neighborhood_move);
    EXPECT_TRUE(move.is_available);
    EXPECT_EQ(0, static_cast<int>(move.hash));
    EXPECT_EQ(0.0, move.overlap_rate);
}

/*****************************************************************************/
TEST_F(TestMove, sense_label) {
    neighborhood::Move<int, double> move;
    move.sense = neighborhood::MoveSense::Binary;
    EXPECT_EQ("Binary", move.sense_label());

    move.sense = neighborhood::MoveSense::Integer;
    EXPECT_EQ("Integer", move.sense_label());

    move.sense = neighborhood::MoveSense::Selection;
    EXPECT_EQ("Selection", move.sense_label());

    move.sense = neighborhood::MoveSense::ExclusiveOr;
    EXPECT_EQ("ExclusiveOr", move.sense_label());

    move.sense = neighborhood::MoveSense::ExclusiveNor;
    EXPECT_EQ("ExclusiveNor", move.sense_label());

    move.sense = neighborhood::MoveSense::InvertedIntegers;
    EXPECT_EQ("InvertedIntegers", move.sense_label());

    move.sense = neighborhood::MoveSense::BalancedIntegers;
    EXPECT_EQ("BalancedIntegers", move.sense_label());

    move.sense = neighborhood::MoveSense::ConstantSumIntegers;
    EXPECT_EQ("ConstantSumIntegers", move.sense_label());

    move.sense = neighborhood::MoveSense::ConstantDifferenceIntegers;
    EXPECT_EQ("ConstantDifferenceIntegers", move.sense_label());

    move.sense = neighborhood::MoveSense::ConstantRatioIntegers;
    EXPECT_EQ("ConstantRatioIntegers", move.sense_label());

    move.sense = neighborhood::MoveSense::Aggregation;
    EXPECT_EQ("Aggregation", move.sense_label());

    move.sense = neighborhood::MoveSense::Precedence;
    EXPECT_EQ("Precedence", move.sense_label());

    move.sense = neighborhood::MoveSense::VariableBound;
    EXPECT_EQ("VariableBound", move.sense_label());

    move.sense = neighborhood::MoveSense::SoftSelection;
    EXPECT_EQ("SoftSelection", move.sense_label());

    move.sense = neighborhood::MoveSense::TrinomialExclusiveNor;
    EXPECT_EQ("TrinomialExclusiveNor", move.sense_label());

    move.sense = neighborhood::MoveSense::Chain;
    EXPECT_EQ("Chain", move.sense_label());

    move.sense = neighborhood::MoveSense::TwoFlip;
    EXPECT_EQ("TwoFlip", move.sense_label());

    move.sense = neighborhood::MoveSense::UserDefined;
    EXPECT_EQ("UserDefined", move.sense_label());

    move.sense = neighborhood::MoveSense::General;
    EXPECT_EQ("General", move.sense_label());
}

/*****************************************************************************/
TEST_F(TestMove, has_fixed_variable) {
    auto variable_0 = model_component::Variable<int, double>::create_instance();
    auto variable_1 = model_component::Variable<int, double>::create_instance();
    variable_1.fix();

    /// The move does not have a fixed variable.
    {
        neighborhood::Move<int, double> move;
        move.alterations.emplace_back(&variable_0, 1);
        EXPECT_FALSE(move.has_fixed_variable());
    }

    /// The move have a fixed variable.
    {
        neighborhood::Move<int, double> move;
        move.alterations.emplace_back(&variable_0, 1);
        move.alterations.emplace_back(&variable_1, 1);
        EXPECT_TRUE(move.has_fixed_variable());
    }
}

/*****************************************************************************/
TEST_F(TestMove, has_selection_variable) {
    auto variable_0 = model_component::Variable<int, double>::create_instance();
    auto variable_1 = model_component::Variable<int, double>::create_instance();

    model_component::Selection<int, double> selection;
    variable_1.set_selection_ptr(&selection);

    /// The move does not have a selection variable.
    {
        neighborhood::Move<int, double> move;
        move.alterations.emplace_back(&variable_0, 1);
        EXPECT_FALSE(move.has_selection_variable());
    }

    /// The move has a selection variable.
    {
        neighborhood::Move<int, double> move;
        move.alterations.emplace_back(&variable_0, 1);
        move.alterations.emplace_back(&variable_1, 1);
        EXPECT_TRUE(move.has_selection_variable());
    }
}

/*****************************************************************************/
TEST_F(TestMove, has_bound_violation) {
    auto variable_0 = model_component::Variable<int, double>::create_instance();
    auto variable_1 = model_component::Variable<int, double>::create_instance();
    variable_0.set_bound(0, 10);
    variable_1.set_bound(0, 10);

    /// The move does not have a bound violation.
    {
        neighborhood::Move<int, double> move;
        move.alterations.emplace_back(&variable_0, 1);
        EXPECT_FALSE(move.has_bound_violation());
    }

    /// The move has a bound violation.
    {
        neighborhood::Move<int, double> move;
        move.alterations.emplace_back(&variable_0, 1);
        move.alterations.emplace_back(&variable_1, 20);
        EXPECT_TRUE(move.has_bound_violation());
    }
}

/*****************************************************************************/
TEST_F(TestMove, has_objective_improvable_variable) {
    auto variable_0 = model_component::Variable<int, double>::create_instance();
    auto variable_1 = model_component::Variable<int, double>::create_instance();
    variable_0.set_is_objective_improvable(false);
    variable_1.set_is_objective_improvable(true);

    /// The move does not have an objective improvable variable.
    {
        neighborhood::Move<int, double> move;
        move.alterations.emplace_back(&variable_0, 1);
        EXPECT_FALSE(move.has_objective_improvable_variable());
    }

    /// The move has an objective improvable variable.
    {
        neighborhood::Move<int, double> move;
        move.alterations.emplace_back(&variable_0, 1);
        move.alterations.emplace_back(&variable_1, 1);
        EXPECT_TRUE(move.has_objective_improvable_variable());
    }
}

/*****************************************************************************/
TEST_F(TestMove, has_feasibility_improvable_variable) {
    auto variable_0 = model_component::Variable<int, double>::create_instance();
    auto variable_1 = model_component::Variable<int, double>::create_instance();
    variable_0.set_is_feasibility_improvable(false);
    variable_1.set_is_feasibility_improvable(true);

    /// The move does not have a feasibility improvable variable.
    {
        neighborhood::Move<int, double> move;
        move.alterations.emplace_back(&variable_0, 1);
        EXPECT_FALSE(move.has_feasibility_improvable_variable());
    }

    /// The move has a feasibility improvable variable.
    {
        neighborhood::Move<int, double> move;
        move.alterations.emplace_back(&variable_0, 1);
        move.alterations.emplace_back(&variable_1, 1);
        EXPECT_TRUE(move.has_feasibility_improvable_variable());
    }
}

/*****************************************************************************/
TEST_F(TestMove, has_duplicate_variable) {
    auto variable_0 = model_component::Variable<int, double>::create_instance();
    auto variable_1 = model_component::Variable<int, double>::create_instance();

    /// The move does not have a duplicated variable.
    {
        neighborhood::Move<int, double> move;
        move.alterations.emplace_back(&variable_0, 1);
        move.alterations.emplace_back(&variable_1, 1);
        EXPECT_FALSE(move.has_duplicate_variable());
    }

    /// The move has a duplicated variable.
    {
        neighborhood::Move<int, double> move;
        move.alterations.emplace_back(&variable_0, 1);
        move.alterations.emplace_back(&variable_0, 1);
        EXPECT_TRUE(move.has_duplicate_variable());
    }
}

/*****************************************************************************/
TEST_F(TestMove, compute_overlap_rate) {
    model::Model<int, double> model;
    auto&                     x = model.create_variables("x", 4, 0, 1);
    auto&                     g = model.create_constraints("g", 3);

    g(0) = x(0) + x(1) + x(2) <= 1;
    g(1) = x(0) + x(1) + x(3) <= 1;
    g(2) = x(0) + x(2) + x(3) <= 1;

    model.categorize_constraints();
    model.setup_variable_related_constraints();
    model.setup_variable_related_binary_coefficient_constraints();

    /// x(0) and x(1) have two common constraints.
    {
        neighborhood::Move<int, double> move;
        move.alterations.emplace_back(&x(0), 1);
        move.alterations.emplace_back(&x(1), 1);
        move.setup_overlap_rate();

        EXPECT_FLOAT_EQ(2.0 / 3.0, move.overlap_rate);
    }

    /// x(0), x(1), and x(2) have one common constraint.
    {
        neighborhood::Move<int, double> move;
        move.alterations.emplace_back(&x(0), 1);
        move.alterations.emplace_back(&x(1), 1);
        move.alterations.emplace_back(&x(2), 1);
        move.setup_overlap_rate();

        EXPECT_FLOAT_EQ(pow(1.0 / 3.0, 1.0 / (3 - 1)), move.overlap_rate);
    }

    /// x(0), x(1), x(2), and x(3) has no common constraint.
    {
        neighborhood::Move<int, double> move;
        move.alterations.emplace_back(&x(0), 1);
        move.alterations.emplace_back(&x(1), 1);
        move.alterations.emplace_back(&x(2), 1);
        move.alterations.emplace_back(&x(3), 1);
        move.setup_overlap_rate();

        EXPECT_FLOAT_EQ(0.0, move.overlap_rate);
    }
}

/*****************************************************************************/
TEST_F(TestMove, compute_hash) {
    model::Model<int, double> model;
    auto&                     x = model.create_variables("x", 4, 0, 1);

    model.setup_structure();

    /// Case 1
    {
        neighborhood::Move<int, double> move;
        move.alterations.emplace_back(&x(0), 1);
        move.alterations.emplace_back(&x(1), 1);

        std::uint_fast64_t hash = 0;
        for (const auto& alteration : move.alterations) {
            hash =
                hash ^ reinterpret_cast<std::uint_fast64_t>(alteration.first);
        }

        move.setup_hash();
        EXPECT_EQ(hash, move.hash);
    }

    /// Case 2
    {
        neighborhood::Move<int, double> move;
        move.alterations.emplace_back(&x(0), 1);
        move.alterations.emplace_back(&x(1), 1);
        move.alterations.emplace_back(&x(2), 1);

        std::uint_fast64_t hash = 0;
        for (const auto& alteration : move.alterations) {
            hash =
                hash ^ reinterpret_cast<std::uint_fast64_t>(alteration.first);
        }

        move.setup_hash();
        EXPECT_EQ(hash, move.hash);
    }

    /// Case 3
    {
        neighborhood::Move<int, double> move;
        move.alterations.emplace_back(&x(0), 1);
        move.alterations.emplace_back(&x(1), 1);
        move.alterations.emplace_back(&x(2), 1);
        move.alterations.emplace_back(&x(3), 1);

        std::uint_fast64_t hash = 0;
        for (const auto& alteration : move.alterations) {
            hash =
                hash ^ reinterpret_cast<std::uint_fast64_t>(alteration.first);
        }

        move.setup_hash();
        EXPECT_EQ(hash, move.hash);
    }
}

/*****************************************************************************/
TEST_F(TestMove, operator_plus) {
    model::Model<int, double> model;

    auto& x = model.create_variable("x", 0, 10);
    auto& y = model.create_variable("y", 0, 10);
    auto& z = model.create_variable("z", 0, 10);

    [[maybe_unused]] auto& g = model.create_constraint("g", x + y <= 10);
    [[maybe_unused]] auto& h = model.create_constraint("h", y + z <= 10);
    [[maybe_unused]] auto& v = model.create_constraint("v", x + z <= 10);

    model.setup_unique_names();
    model.setup_structure();

    auto variable_ptrs = model.variable_reference().variable_ptrs;

    /// Single moves.
    neighborhood::Move<int, double> move_x;
    move_x.alterations.emplace_back(variable_ptrs[0], 1);
    move_x.related_constraint_ptrs =
        variable_ptrs[0]->related_constraint_ptrs();

    neighborhood::Move<int, double> move_y;
    move_y.alterations.emplace_back(variable_ptrs[1], 2);
    move_y.related_constraint_ptrs =
        variable_ptrs[1]->related_constraint_ptrs();

    neighborhood::Move<int, double> move_z;
    move_z.alterations.emplace_back(variable_ptrs[2], 3);
    move_z.related_constraint_ptrs =
        variable_ptrs[2]->related_constraint_ptrs();

    /// Combined move for x and y.
    auto move_x_y = move_x + move_y;
    EXPECT_FALSE(move_x_y.has_duplicate_variable());
    EXPECT_EQ(2, static_cast<int>(move_x_y.alterations.size()));
    EXPECT_EQ(3, static_cast<int>(move_x_y.related_constraint_ptrs.size()));
    EXPECT_EQ(neighborhood::MoveSense::Chain, move_x_y.sense);

    EXPECT_EQ(variable_ptrs[0], move_x_y.alterations[0].first);
    EXPECT_EQ(1, move_x_y.alterations[0].second);

    EXPECT_EQ(variable_ptrs[1], move_x_y.alterations[1].first);
    EXPECT_EQ(2, move_x_y.alterations[1].second);

    /// Combined move for x, y, and z.
    auto move_x_y_z = move_x_y + move_z;
    EXPECT_FALSE(move_x_y_z.has_duplicate_variable());
    EXPECT_EQ(3, static_cast<int>(move_x_y_z.alterations.size()));
    EXPECT_EQ(3, static_cast<int>(move_x_y_z.related_constraint_ptrs.size()));
    EXPECT_EQ(neighborhood::MoveSense::Chain, move_x_y_z.sense);

    EXPECT_EQ(variable_ptrs[0], move_x_y_z.alterations[0].first);
    EXPECT_EQ(1, move_x_y_z.alterations[0].second);

    EXPECT_EQ(variable_ptrs[1], move_x_y_z.alterations[1].first);
    EXPECT_EQ(2, move_x_y_z.alterations[1].second);

    EXPECT_EQ(variable_ptrs[2], move_x_y_z.alterations[2].first);
    EXPECT_EQ(3, move_x_y_z.alterations[2].second);

    /// Combined move for x, y, and z, which has duplication.
    auto move_x_y_z_z = move_x_y_z + move_z;
    EXPECT_TRUE(move_x_y_z_z.has_duplicate_variable());
    EXPECT_EQ(4, static_cast<int>(move_x_y_z_z.alterations.size()));
    EXPECT_EQ(3, static_cast<int>(move_x_y_z_z.related_constraint_ptrs.size()));
    EXPECT_EQ(neighborhood::MoveSense::Chain, move_x_y_z_z.sense);

    EXPECT_EQ(variable_ptrs[0], move_x_y_z_z.alterations[0].first);
    EXPECT_EQ(1, move_x_y_z_z.alterations[0].second);

    EXPECT_EQ(variable_ptrs[1], move_x_y_z_z.alterations[1].first);
    EXPECT_EQ(2, move_x_y_z_z.alterations[1].second);

    EXPECT_EQ(variable_ptrs[2], move_x_y_z_z.alterations[2].first);
    EXPECT_EQ(3, move_x_y_z_z.alterations[2].second);

    EXPECT_EQ(variable_ptrs[2], move_x_y_z_z.alterations[3].first);
    EXPECT_EQ(3, move_x_y_z_z.alterations[3].second);
}

/*****************************************************************************/
TEST_F(TestMove, operator_equal) {
    auto variable_0 = model_component::Variable<int, double>::create_instance();
    auto variable_1 = model_component::Variable<int, double>::create_instance();

    auto constraint =
        model_component::Constraint<int, double>::create_instance();

    /// Not equal: The number of alterations are different.
    {
        neighborhood::Move<int, double> move_0;
        move_0.alterations.emplace_back(&variable_0, 1);

        neighborhood::Move<int, double> move_1;
        move_1.alterations.emplace_back(&variable_0, 1);
        move_1.alterations.emplace_back(&variable_1, 1);

        EXPECT_FALSE(move_0 == move_1);
        EXPECT_TRUE(move_0 != move_1);
    }

    /// Not equal: The number of related constraints are different.
    {
        neighborhood::Move<int, double> move_0;
        move_0.alterations.emplace_back(&variable_0, 1);

        neighborhood::Move<int, double> move_1;
        move_1.alterations.emplace_back(&variable_0, 1);
        move_1.related_constraint_ptrs.push_back(&constraint);

        EXPECT_FALSE(move_0 == move_1);
        EXPECT_TRUE(move_0 != move_1);
    }

    /// Not equal: The overlap rates are different.
    {
        neighborhood::Move<int, double> move_0;
        move_0.alterations.emplace_back(&variable_0, 1);
        move_0.overlap_rate = 0.0;

        neighborhood::Move<int, double> move_1;
        move_1.alterations.emplace_back(&variable_0, 1);
        move_1.overlap_rate = 0.1;

        EXPECT_FALSE(move_0 == move_1);
        EXPECT_TRUE(move_0 != move_1);
    }

    /// Not equal: The included variables are different.
    {
        neighborhood::Move<int, double> move_0;
        move_0.alterations.emplace_back(&variable_0, 1);

        neighborhood::Move<int, double> move_1;
        move_1.alterations.emplace_back(&variable_1, 1);

        EXPECT_FALSE(move_0 == move_1);
        EXPECT_TRUE(move_0 != move_1);
    }

    /// Not equal: The target values are different.
    {
        neighborhood::Move<int, double> move_0;
        move_0.alterations.emplace_back(&variable_0, 0);

        neighborhood::Move<int, double> move_1;
        move_1.alterations.emplace_back(&variable_0, 1);

        EXPECT_FALSE(move_0 == move_1);
        EXPECT_TRUE(move_0 != move_1);
    }

    /// Not equal: The included variables are different.
    {
        neighborhood::Move<int, double> move_0;
        move_0.alterations.emplace_back(&variable_0, 0);
        move_0.alterations.emplace_back(&variable_1, 1);

        neighborhood::Move<int, double> move_1;
        move_0.alterations.emplace_back(&variable_1, 1);
        move_0.alterations.emplace_back(&variable_1, 1);

        EXPECT_FALSE(move_0 == move_1);
        EXPECT_TRUE(move_0 != move_1);
    }

    /// Equal
    {
        neighborhood::Move<int, double> move_0;
        move_0.alterations.emplace_back(&variable_0, 1);

        neighborhood::Move<int, double> move_1;
        move_1.alterations.emplace_back(&variable_0, 1);

        EXPECT_TRUE(move_0 == move_1);
        EXPECT_FALSE(move_0 != move_1);
    }

    /// Equal
    {
        neighborhood::Move<int, double> move_0;
        move_0.alterations.emplace_back(&variable_0, 1);
        move_0.alterations.emplace_back(&variable_1, 1);

        neighborhood::Move<int, double> move_1;
        move_1.alterations.emplace_back(&variable_0, 1);
        move_1.alterations.emplace_back(&variable_1, 1);

        EXPECT_TRUE(move_0 == move_1);
        EXPECT_FALSE(move_0 != move_1);
    }
}
}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/