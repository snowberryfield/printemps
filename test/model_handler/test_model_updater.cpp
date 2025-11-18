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
class TestModelUpdater : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestModelUpdater, update_arg_void) {
    model::Model<int, double> model;

    auto sequence = utility::sequence(10);

    auto& x = model.create_variables("x", 10, 0, 1);
    auto& p = model.create_expression("p", x.dot(sequence) + 1);
    model.create_constraint("g", x.selection());

    for (auto&& element : x.flat_indexed_variables()) {
        element = 1;
    }
    model.minimize(p);
    model.updater().update();

    // 0 + 1 + 2 + ... + 9 + 1 = 46
    EXPECT_EQ(46, p.value());
    EXPECT_EQ(46, p(0).value());
    EXPECT_EQ(46, model.objective().value());
}

/*****************************************************************************/
TEST_F(TestModelUpdater, update_arg_move) {
    model::Model<int, double> model;

    auto sequence = utility::sequence(10);

    auto& x = model.create_variables("x", 10, 0, 1);
    auto& p = model.create_expression("p", x.dot(sequence) + 1);
    model.create_constraint("g", x.selection());

    x(0) = 1;

    model.minimize(p);
    model.builder().setup_structure();
    preprocess::SelectionExtractor<int, double> selection_extractor(&model);
    selection_extractor.extract_by_independent(false);
    model.builder().setup_fixed_sensitivities(false);

    model.updater().update();

    neighborhood::Move<int, double> move;
    move.type = neighborhood::MoveType::Selection;
    move.alterations.emplace_back(&x(0), 0);
    move.alterations.emplace_back(&x(9), 1);

    model.updater().update(move);

    EXPECT_EQ(10, p.value());
    EXPECT_EQ(10, p(0).value());
    EXPECT_EQ(10, model.objective().value());
    EXPECT_EQ(&x(9), model.selections().front().selected_variable_ptr);
}

/*****************************************************************************/
TEST_F(TestModelUpdater, update_dependent_variables_and_disabled_constraints) {
    model::Model<int, double> model;
    option::Option            option;

    auto& x = model.create_variable("x", 0, 10);
    auto& y = model.create_variable("y", 0, 10);

    model.create_constraint("g", x == 2 * y);
    auto& h = model.create_constraint("h", x + y <= 100);

    h.disable();

    model.builder().setup_structure();
    preprocess::DependentVariableExtractor<int, double>
        dependent_variable_extractor(&model);
    dependent_variable_extractor.extract(option, false);
    model.builder().setup_structure();

    x(0) = 0;
    y(0) = 10;
    EXPECT_EQ(0, x.value());
    EXPECT_EQ(0, h.constraint_value());
    model.updater().update_dependent_variables_and_disabled_constraints();
    EXPECT_EQ(20, x.value());
    EXPECT_EQ(-70, h.constraint_value());
}

/*****************************************************************************/
TEST_F(TestModelUpdater, update_variable_bounds) {
    {
        model::Model<int, double> model;

        auto& x = model.create_variable("x", 0, 200);
        auto& y = model.create_variable("y", 0, 200);
        model.minimize(x + 3 * y);
        model.problem_size_reducer().setup(&model);
        model.updater().update_variable_bounds(100, true, false);

        EXPECT_EQ(100, x(0).upper_bound());
        EXPECT_EQ(33, y(0).upper_bound());
    }

    {
        model::Model<int, double> model;

        auto& x = model.create_variable("x", 0, 200);
        auto& y = model.create_variable("y", 0, 200);
        model.minimize(x - 3 * y);
        model.problem_size_reducer().setup(&model);
        model.updater().update_variable_bounds(100, true, false);

        EXPECT_EQ(200, x(0).upper_bound());
        EXPECT_EQ(200, y(0).upper_bound());
    }

    {
        model::Model<int, double> model;

        auto& x = model.create_variable("x", 0, 200);
        auto& y = model.create_variable("y", 0, 200);
        model.maximize(x + 3 * y);
        model.problem_size_reducer().setup(&model);
        model.updater().update_variable_bounds(100, true, false);

        EXPECT_EQ(0, x(0).lower_bound());
        EXPECT_EQ(0, y(0).lower_bound());
    }

    {
        model::Model<int, double> model;

        auto& x = model.create_variable("x", 0, 200);
        auto& y = model.create_variable("y", 0, 200);
        model.maximize(x - 3 * y);
        model.problem_size_reducer().setup(&model);
        model.updater().update_variable_bounds(100, true, false);

        EXPECT_EQ(100, x(0).lower_bound());
        EXPECT_EQ(0, y(0).lower_bound());
    }
}

/*****************************************************************************/
TEST_F(TestModelUpdater, update_violative_constraint_ptrs_and_feasibility) {
    model::Model<int, double> model;

    auto&                  x = model.create_variable("x", 0, 10);
    [[maybe_unused]] auto& g = model.create_constraint("g", x <= 5);

    model.builder().setup_structure();

    x = 4;
    model.updater().update();  // include update_feasibility()
    EXPECT_TRUE(model.is_feasible());
    EXPECT_TRUE(model.current_violative_constraint_ptrs().empty());

    x = 5;
    model.updater().update();
    EXPECT_TRUE(model.is_feasible());
    EXPECT_TRUE(model.current_violative_constraint_ptrs().empty());

    x = 6;
    model.updater().update();
    EXPECT_FALSE(model.is_feasible());
    EXPECT_EQ(
        1, static_cast<int>(model.current_violative_constraint_ptrs().size()));

    x = 5;
    model.updater().update();
    EXPECT_TRUE(model.is_feasible());
    EXPECT_EQ(
        1, static_cast<int>(model.previous_violative_constraint_ptrs().size()));
    EXPECT_TRUE(model.current_violative_constraint_ptrs().empty());
}

/*****************************************************************************/
TEST_F(TestModelUpdater, update_variable_improvability) {
    {
        model::Model<int, double> model;

        auto& x = model.create_variable("x", -10, 10);
        auto& y = model.create_variable("y", -10, 10);

        [[maybe_unused]] auto& g = model.create_constraint("g", x - y <= 0);

        model.minimize(-x + y);
        model.builder().setup_structure();
        model.builder().setup_fixed_sensitivities(false);
        model.builder()
            .setup_positive_and_negative_coefficient_mutable_variable_ptrs();

        x = -10;
        y = -10;
        model.updater().update();
        model.updater().update_variable_objective_improvabilities();
        model.updater().update_variable_feasibility_improvabilities();
        EXPECT_TRUE(x(0).is_objective_improvable());
        EXPECT_FALSE(y(0).is_objective_improvable());
        EXPECT_FALSE(x(0).is_feasibility_improvable());
        EXPECT_FALSE(y(0).is_feasibility_improvable());

        x = 10;
        y = 10;
        model.updater().update();
        model.updater().update_variable_objective_improvabilities();
        model.updater().update_variable_feasibility_improvabilities();
        EXPECT_FALSE(x(0).is_objective_improvable());
        EXPECT_TRUE(y(0).is_objective_improvable());
        EXPECT_FALSE(x(0).is_feasibility_improvable());
        EXPECT_FALSE(y(0).is_feasibility_improvable());

        x = 10;
        y = -10;
        model.updater().update();
        model.updater().update_variable_objective_improvabilities();
        model.updater().update_variable_feasibility_improvabilities();
        EXPECT_FALSE(x(0).is_objective_improvable());
        EXPECT_FALSE(y(0).is_objective_improvable());
        EXPECT_TRUE(x(0).is_feasibility_improvable());
        EXPECT_TRUE(y(0).is_feasibility_improvable());
    }
    {
        model::Model<int, double> model;

        auto& x = model.create_variable("x", -10, 10);
        auto& y = model.create_variable("y", -10, 10);

        [[maybe_unused]] auto& g = model.create_constraint("g", x - y == 0);

        model.minimize(-x + y);
        model.builder().setup_structure();
        model.builder().setup_fixed_sensitivities(false);
        model.builder()
            .setup_positive_and_negative_coefficient_mutable_variable_ptrs();

        x = -10;
        y = -10;
        model.updater().update();
        model.updater().update_variable_objective_improvabilities();
        model.updater().update_variable_feasibility_improvabilities();
        EXPECT_TRUE(x(0).is_objective_improvable());
        EXPECT_FALSE(y(0).is_objective_improvable());
        EXPECT_FALSE(x(0).is_feasibility_improvable());
        EXPECT_FALSE(y(0).is_feasibility_improvable());

        x = 10;
        y = 10;
        model.updater().update();
        model.updater().update_variable_objective_improvabilities();
        model.updater().update_variable_feasibility_improvabilities();
        EXPECT_FALSE(x(0).is_objective_improvable());
        EXPECT_TRUE(y(0).is_objective_improvable());
        EXPECT_FALSE(x(0).is_feasibility_improvable());
        EXPECT_FALSE(y(0).is_feasibility_improvable());

        x = 10;
        y = -10;
        model.updater().update();
        model.updater().update_variable_objective_improvabilities();
        model.updater().update_variable_feasibility_improvabilities();
        EXPECT_FALSE(x(0).is_objective_improvable());
        EXPECT_FALSE(y(0).is_objective_improvable());
        EXPECT_TRUE(x(0).is_feasibility_improvable());
        EXPECT_TRUE(y(0).is_feasibility_improvable());
    }
    {
        model::Model<int, double> model;

        auto& x = model.create_variable("x", -10, 10);
        auto& y = model.create_variable("y", -10, 10);

        [[maybe_unused]] auto& g = model.create_constraint("g", x - y >= 0);

        model.minimize(-x + y);
        model.builder().setup_structure();
        model.builder().setup_fixed_sensitivities(false);
        model.builder()
            .setup_positive_and_negative_coefficient_mutable_variable_ptrs();

        x = -10;
        y = -10;
        model.updater().update();
        model.updater().update_variable_objective_improvabilities();
        model.updater().update_variable_feasibility_improvabilities();
        EXPECT_TRUE(x(0).is_objective_improvable());
        EXPECT_FALSE(y(0).is_objective_improvable());
        EXPECT_FALSE(x(0).is_feasibility_improvable());
        EXPECT_FALSE(y(0).is_feasibility_improvable());

        x = 10;
        y = 10;
        model.updater().update();
        model.updater().update_variable_objective_improvabilities();
        model.updater().update_variable_feasibility_improvabilities();
        EXPECT_FALSE(x(0).is_objective_improvable());
        EXPECT_TRUE(y(0).is_objective_improvable());
        EXPECT_FALSE(x(0).is_feasibility_improvable());
        EXPECT_FALSE(y(0).is_feasibility_improvable());

        x = -10;
        y = 10;
        model.updater().update();
        model.updater().update_variable_objective_improvabilities();
        model.updater().update_variable_feasibility_improvabilities();
        EXPECT_TRUE(x(0).is_objective_improvable());
        EXPECT_TRUE(y(0).is_objective_improvable());
        EXPECT_TRUE(x(0).is_feasibility_improvable());
        EXPECT_TRUE(y(0).is_feasibility_improvable());
    }
    {
        model::Model<int, double> model;

        auto& x = model.create_variable("x", -10, 10);
        auto& y = model.create_variable("y", -10, 10);

        [[maybe_unused]] auto& g = model.create_constraint("g", x - y <= 0);

        model.maximize(-x + y);
        model.builder().setup_structure();
        model.builder().setup_fixed_sensitivities(false);
        model.builder()
            .setup_positive_and_negative_coefficient_mutable_variable_ptrs();

        x = -10;
        y = -10;

        model.updater().update();
        model.updater().update_variable_objective_improvabilities();
        model.updater().update_variable_feasibility_improvabilities();
        EXPECT_FALSE(x(0).is_objective_improvable());
        EXPECT_TRUE(y(0).is_objective_improvable());
        EXPECT_FALSE(x(0).is_feasibility_improvable());
        EXPECT_FALSE(y(0).is_feasibility_improvable());

        x = 10;
        y = 10;
        model.updater().update();
        model.updater().update_variable_objective_improvabilities();
        model.updater().update_variable_feasibility_improvabilities();
        EXPECT_TRUE(x(0).is_objective_improvable());
        EXPECT_FALSE(y(0).is_objective_improvable());
        EXPECT_FALSE(x(0).is_feasibility_improvable());
        EXPECT_FALSE(y(0).is_feasibility_improvable());

        x = 10;
        y = -10;
        model.updater().update();
        model.updater().update_variable_objective_improvabilities();
        model.updater().update_variable_feasibility_improvabilities();
        EXPECT_TRUE(x(0).is_objective_improvable());
        EXPECT_TRUE(y(0).is_objective_improvable());
        EXPECT_TRUE(x(0).is_feasibility_improvable());
        EXPECT_TRUE(y(0).is_feasibility_improvable());
    }
    {
        model::Model<int, double> model;

        auto& x = model.create_variable("x", -10, 10);
        auto& y = model.create_variable("y", -10, 10);

        [[maybe_unused]] auto& g = model.create_constraint("g", x - y == 0);

        model.maximize(-x + y);
        model.builder().setup_structure();
        model.builder().setup_fixed_sensitivities(false);
        model.builder()
            .setup_positive_and_negative_coefficient_mutable_variable_ptrs();

        x = -10;
        y = -10;

        model.updater().update();
        model.updater().update_variable_objective_improvabilities();
        model.updater().update_variable_feasibility_improvabilities();
        EXPECT_FALSE(x(0).is_objective_improvable());
        EXPECT_TRUE(y(0).is_objective_improvable());
        EXPECT_FALSE(x(0).is_feasibility_improvable());
        EXPECT_FALSE(y(0).is_feasibility_improvable());

        x = 10;
        y = 10;
        model.updater().update();
        model.updater().update_variable_objective_improvabilities();
        model.updater().update_variable_feasibility_improvabilities();
        EXPECT_TRUE(x(0).is_objective_improvable());
        EXPECT_FALSE(y(0).is_objective_improvable());
        EXPECT_FALSE(x(0).is_feasibility_improvable());
        EXPECT_FALSE(y(0).is_feasibility_improvable());

        x = 10;
        y = -10;
        model.updater().update();
        model.updater().update_variable_objective_improvabilities();
        model.updater().update_variable_feasibility_improvabilities();
        EXPECT_TRUE(x(0).is_objective_improvable());
        EXPECT_TRUE(y(0).is_objective_improvable());
        EXPECT_TRUE(x(0).is_feasibility_improvable());
        EXPECT_TRUE(y(0).is_feasibility_improvable());
    }
    {
        model::Model<int, double> model;

        auto& x = model.create_variable("x", -10, 10);
        auto& y = model.create_variable("y", -10, 10);

        [[maybe_unused]] auto& g = model.create_constraint("g", x - y >= 0);

        model.maximize(-x + y);
        model.builder().setup_structure();
        model.builder().setup_fixed_sensitivities(false);
        model.builder()
            .setup_positive_and_negative_coefficient_mutable_variable_ptrs();

        x = -10;
        y = -10;

        model.updater().update();
        model.updater().update_variable_objective_improvabilities();
        model.updater().update_variable_feasibility_improvabilities();
        EXPECT_FALSE(x(0).is_objective_improvable());
        EXPECT_TRUE(y(0).is_objective_improvable());
        EXPECT_FALSE(x(0).is_feasibility_improvable());
        EXPECT_FALSE(y(0).is_feasibility_improvable());

        x = 10;
        y = 10;
        model.updater().update();
        model.updater().update_variable_objective_improvabilities();
        model.updater().update_variable_feasibility_improvabilities();
        EXPECT_TRUE(x(0).is_objective_improvable());
        EXPECT_FALSE(y(0).is_objective_improvable());
        EXPECT_FALSE(x(0).is_feasibility_improvable());
        EXPECT_FALSE(y(0).is_feasibility_improvable());

        x = -10;
        y = 10;
        model.updater().update();
        model.updater().update_variable_objective_improvabilities();
        model.updater().update_variable_feasibility_improvabilities();
        EXPECT_FALSE(x(0).is_objective_improvable());
        EXPECT_FALSE(y(0).is_objective_improvable());
        EXPECT_TRUE(x(0).is_feasibility_improvable());
        EXPECT_TRUE(y(0).is_feasibility_improvable());
    }
}

/*****************************************************************************/
TEST_F(TestModelUpdater, reset_variable_objective_improvabilities_arg_void) {
    model::Model<int, double> model;

    auto& x = model.create_variable("x", 0, 1);
    auto& y = model.create_variables("y", 10, 0, 1);
    model.builder().setup_structure();

    x(0).set_is_objective_improvable(true);
    EXPECT_TRUE(x(0).is_objective_improvable());
    for (auto i = 0; i < 10; i++) {
        y(i).set_is_objective_improvable(true);
        EXPECT_TRUE(y(i).is_objective_improvable());
    }
    model.updater().reset_variable_objective_improvabilities();

    EXPECT_FALSE(x(0).is_objective_improvable());
    for (auto i = 0; i < 10; i++) {
        EXPECT_FALSE(y(i).is_objective_improvable());
    }
}

/*****************************************************************************/
TEST_F(TestModelUpdater,
       reset_variable_objective_improvabilities_arg_variable_ptrs) {
    model::Model<int, double> model;

    auto& x = model.create_variable("x", 0, 1);
    auto& y = model.create_variables("y", 10, 0, 1);
    model.builder().setup_structure();

    x(0).set_is_objective_improvable(true);
    EXPECT_TRUE(x(0).is_objective_improvable());
    for (auto i = 0; i < 10; i++) {
        y(i).set_is_objective_improvable(true);
        EXPECT_TRUE(y(i).is_objective_improvable());
    }
    model.updater().reset_variable_objective_improvabilities(
        {&x(0), &y(0), &y(9)});

    EXPECT_FALSE(x(0).is_objective_improvable());
    EXPECT_FALSE(y(0).is_objective_improvable());
    EXPECT_FALSE(y(9).is_objective_improvable());
    for (auto i = 1; i < 9; i++) {
        EXPECT_TRUE(y(i).is_objective_improvable());
    }
}

/*****************************************************************************/
TEST_F(TestModelUpdater, reset_variable_feasibility_improvabilities_arg_void) {
    model::Model<int, double> model;

    auto& x = model.create_variable("x", 0, 1);
    auto& y = model.create_variables("y", 10, 0, 1);
    model.builder().setup_structure();

    x(0).set_is_feasibility_improvable(true);
    EXPECT_TRUE(x(0).is_feasibility_improvable());
    for (auto i = 0; i < 10; i++) {
        y(i).set_is_feasibility_improvable(true);
        EXPECT_TRUE(y(i).is_feasibility_improvable());
    }
    model.updater().reset_variable_feasibility_improvabilities();

    EXPECT_FALSE(x(0).is_feasibility_improvable());
    for (auto i = 0; i < 10; i++) {
        EXPECT_FALSE(y(i).is_feasibility_improvable());
    }
}

/*****************************************************************************/
TEST_F(TestModelUpdater,
       reset_variable_feasibility_improvabilities_arg_variable_ptrs) {
    model::Model<int, double> model;

    auto& x = model.create_variable("x", 0, 1);
    auto& y = model.create_variables("y", 10, 0, 1);
    model.builder().setup_structure();

    x(0).set_is_feasibility_improvable(true);
    EXPECT_TRUE(x(0).is_feasibility_improvable());
    for (auto i = 0; i < 10; i++) {
        y(i).set_is_feasibility_improvable(true);
        EXPECT_TRUE(y(i).is_feasibility_improvable());
    }
    model.updater().reset_variable_feasibility_improvabilities(
        {&x(0), &y(0), &y(9)});

    EXPECT_FALSE(x(0).is_feasibility_improvable());
    EXPECT_FALSE(y(0).is_feasibility_improvable());
    EXPECT_FALSE(y(9).is_feasibility_improvable());
    for (auto i = 1; i < 9; i++) {
        EXPECT_TRUE(y(i).is_feasibility_improvable());
    }
}

/*****************************************************************************/
TEST_F(TestModelUpdater,
       reset_variable_feasibility_improvabilities_arg_constraint_ptrs) {
    model::Model<int, double> model;

    auto& x = model.create_variable("x", 0, 1);
    auto& y = model.create_variables("y", 10, 0, 1);
    auto& g = model.create_constraints("g", 2);

    g(0) = x <= y(0);
    g(1) = y(1) == y(9);

    model.builder().setup_structure();

    x(0).set_is_feasibility_improvable(true);
    EXPECT_TRUE(x(0).is_feasibility_improvable());
    for (auto i = 0; i < 10; i++) {
        y(i).set_is_feasibility_improvable(true);
        EXPECT_TRUE(y(i).is_feasibility_improvable());
    }
    std::vector<model_component::Constraint<int, double>*> constraint_ptrs = {
        &g(0), &g(1)};
    model.updater().reset_variable_feasibility_improvabilities(constraint_ptrs);

    EXPECT_FALSE(x(0).is_feasibility_improvable());
    EXPECT_FALSE(y(0).is_feasibility_improvable());
    EXPECT_FALSE(y(1).is_feasibility_improvable());
    EXPECT_FALSE(y(9).is_feasibility_improvable());
    for (auto i = 2; i < 9; i++) {
        EXPECT_TRUE(y(i).is_feasibility_improvable());
    }
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/