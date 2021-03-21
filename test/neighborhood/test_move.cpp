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
class TestMove : public ::testing::Test {
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
TEST_F(TestMove, constructor) {
    printemps::neighborhood::Move<int, double> move;
    EXPECT_EQ(false, move.is_special_neighborhood_move);
    EXPECT_EQ(true, move.is_available);
    EXPECT_EQ(0.0, move.overlap_rate);
}

/*****************************************************************************/
TEST_F(TestMove, has_duplicate_variable) {
    auto variable_0 =
        printemps::model::Variable<int, double>::create_instance();
    auto variable_1 =
        printemps::model::Variable<int, double>::create_instance();

    {
        printemps::neighborhood::Move<int, double> move;
        move.alterations.emplace_back(&variable_0, 1);
        move.alterations.emplace_back(&variable_0, 1);
        EXPECT_EQ(true, printemps::neighborhood::has_duplicate_variable(move));
    }

    {
        printemps::neighborhood::Move<int, double> move;
        move.alterations.emplace_back(&variable_0, 1);
        move.alterations.emplace_back(&variable_1, 1);
        EXPECT_EQ(false, printemps::neighborhood::has_duplicate_variable(move));
    }
}

/*****************************************************************************/
TEST_F(TestMove, compute_overlap_rate) {
    printemps::model::Model<int, double> model;
    auto& x = model.create_variables("x", 4, 0, 1);
    auto& g = model.create_constraints("g", 3);

    g(0) = x(0) + x(1) + x(2) <= 1;
    g(1) = x(0) + x(1) + x(3) <= 1;
    g(2) = x(0) + x(2) + x(3) <= 1;

    model.setup_variable_related_constraints();
    model.categorize_constraints();
    model.setup_variable_related_monic_constraints();

    {
        printemps::neighborhood::Move<int, double> move;
        move.alterations.emplace_back(&x(0), 1);
        move.alterations.emplace_back(&x(1), 1);
        double overlap_rate =
            printemps::neighborhood::compute_overlap_rate(move.alterations);

        EXPECT_FLOAT_EQ(2.0 / 3.0, overlap_rate);
    }

    {
        printemps::neighborhood::Move<int, double> move;
        move.alterations.emplace_back(&x(0), 1);
        move.alterations.emplace_back(&x(1), 1);
        move.alterations.emplace_back(&x(2), 1);
        double overlap_rate =
            printemps::neighborhood::compute_overlap_rate(move.alterations);

        EXPECT_FLOAT_EQ(pow(1.0 / 3.0, 1.0 / (3 - 1)), overlap_rate);
    }

    {
        printemps::neighborhood::Move<int, double> move;
        move.alterations.emplace_back(&x(0), 1);
        move.alterations.emplace_back(&x(1), 1);
        move.alterations.emplace_back(&x(2), 1);
        move.alterations.emplace_back(&x(3), 1);
        double overlap_rate =
            printemps::neighborhood::compute_overlap_rate(move.alterations);

        EXPECT_FLOAT_EQ(0.0, overlap_rate);
    }
}

/*****************************************************************************/
TEST_F(TestMove, compute_hash) {
    printemps::model::Model<int, double> model;
    auto& x = model.create_variables("x", 4, 0, 1);

    model.setup_variable_related_constraints();
    model.categorize_variables();
    model.categorize_constraints();

    {
        printemps::neighborhood::Move<int, double> move;
        move.alterations.emplace_back(&x(0), 1);
        move.alterations.emplace_back(&x(1), 1);
        double hash = printemps::neighborhood::compute_hash(move.alterations);

        EXPECT_EQ(true, hash < 1.0);
    }

    {
        printemps::neighborhood::Move<int, double> move;
        move.alterations.emplace_back(&x(0), 1);
        move.alterations.emplace_back(&x(1), 1);
        move.alterations.emplace_back(&x(2), 1);
        double hash = printemps::neighborhood::compute_hash(move.alterations);

        EXPECT_EQ(true, hash < 1.0);
    }

    {
        printemps::neighborhood::Move<int, double> move;
        move.alterations.emplace_back(&x(0), 1);
        move.alterations.emplace_back(&x(1), 1);
        move.alterations.emplace_back(&x(2), 1);
        move.alterations.emplace_back(&x(3), 1);
        double hash = printemps::neighborhood::compute_hash(move.alterations);

        EXPECT_EQ(true, hash < 1.0);
    }
}

/*****************************************************************************/
TEST_F(TestMove, operator_plus) {
    printemps::model::Model<int, double> model;

    auto& x = model.create_variable("x", 0, 10);
    auto& y = model.create_variable("y", 0, 10);
    auto& z = model.create_variable("z", 0, 10);

    [[maybe_unused]] auto& g = model.create_constraint("g", x + y <= 10);
    [[maybe_unused]] auto& h = model.create_constraint("h", y + z <= 10);
    [[maybe_unused]] auto& v = model.create_constraint("v", x + z <= 10);

    model.setup_variable_related_constraints();
    model.categorize_variables();
    model.categorize_constraints();

    auto variable_ptrs = model.variable_reference().variable_ptrs;

    printemps::neighborhood::Move<int, double> move_x;
    move_x.alterations.emplace_back(variable_ptrs[0], 1);
    move_x.related_constraint_ptrs =
        variable_ptrs[0]->related_constraint_ptrs();

    printemps::neighborhood::Move<int, double> move_y;
    move_y.alterations.emplace_back(variable_ptrs[1], 2);
    move_y.related_constraint_ptrs =
        variable_ptrs[1]->related_constraint_ptrs();

    printemps::neighborhood::Move<int, double> move_z;
    move_z.alterations.emplace_back(variable_ptrs[2], 3);
    move_z.related_constraint_ptrs =
        variable_ptrs[2]->related_constraint_ptrs();

    auto move_x_y = move_x + move_y;
    EXPECT_EQ(false, printemps::neighborhood::has_duplicate_variable(move_x_y));
    EXPECT_EQ(2, static_cast<int>(move_x_y.alterations.size()));
    EXPECT_EQ(3, static_cast<int>(move_x_y.related_constraint_ptrs.size()));
    EXPECT_EQ(printemps::neighborhood::MoveSense::Chain, move_x_y.sense);

    EXPECT_EQ(variable_ptrs[0], move_x_y.alterations[0].first);
    EXPECT_EQ(1, move_x_y.alterations[0].second);

    EXPECT_EQ(variable_ptrs[1], move_x_y.alterations[1].first);
    EXPECT_EQ(2, move_x_y.alterations[1].second);

    auto move_x_y_z = move_x_y + move_z;
    EXPECT_EQ(false,
              printemps::neighborhood::has_duplicate_variable(move_x_y_z));
    EXPECT_EQ(3, static_cast<int>(move_x_y_z.alterations.size()));
    EXPECT_EQ(3, static_cast<int>(move_x_y_z.related_constraint_ptrs.size()));
    EXPECT_EQ(printemps::neighborhood::MoveSense::Chain, move_x_y_z.sense);

    EXPECT_EQ(variable_ptrs[0], move_x_y_z.alterations[0].first);
    EXPECT_EQ(1, move_x_y_z.alterations[0].second);

    EXPECT_EQ(variable_ptrs[1], move_x_y_z.alterations[1].first);
    EXPECT_EQ(2, move_x_y_z.alterations[1].second);

    EXPECT_EQ(variable_ptrs[2], move_x_y_z.alterations[2].first);
    EXPECT_EQ(3, move_x_y_z.alterations[2].second);

    auto move_x_y_z_z = move_x_y_z + move_z;
    EXPECT_EQ(true,
              printemps::neighborhood::has_duplicate_variable(move_x_y_z_z));
    EXPECT_EQ(4, static_cast<int>(move_x_y_z_z.alterations.size()));
    EXPECT_EQ(3, static_cast<int>(move_x_y_z_z.related_constraint_ptrs.size()));
    EXPECT_EQ(printemps::neighborhood::MoveSense::Chain, move_x_y_z_z.sense);

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
}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/