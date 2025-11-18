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
class TestModelEvaluator : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestModelEvaluator, evaluate) {
    /// minimize
    {
        model::Model<int, double> model;

        auto sequence = utility::sequence(10);

        auto& x = model.create_variables("x", 10, 0, 1);
        auto& p = model.create_expression("p", x.dot(sequence) + 1);
        [[maybe_unused]] auto& g = model.create_constraint("g", x.sum() <= 5);
        [[maybe_unused]] auto& h =
            model.create_constraint("h", x(0) + x(1) <= 1);

        g(0).local_penalty_coefficient_less()    = 100;
        g(0).local_penalty_coefficient_greater() = 100;
        h(0).local_penalty_coefficient_less()    = 100;
        h(0).local_penalty_coefficient_greater() = 100;

        model.minimize(p);
        model.set_global_penalty_coefficient(10000);
        model.builder().setup_unique_names();
        model.builder().setup_structure();
        preprocess::SelectionExtractor<int, double> selection_extractor(&model);
        selection_extractor.extract_by_independent(false);
        model.builder().setup_fixed_sensitivities(false);

        for (auto&& element : x.flat_indexed_variables()) {
            element = 1;
        }

        model.updater().update();
        auto score_before = model.evaluator().evaluate({});

        {
            neighborhood::Move<int, double> move;
            for (auto&& element : x.flat_indexed_variables()) {
                move.alterations.emplace_back(&element, 1);
                for (auto&& constraint_ptr :
                     element.related_constraint_ptrs()) {
                    move.related_constraint_ptrs.push_back(constraint_ptr);
                }
            }
            move.sort_and_unique_related_constraint_ptrs();

            auto score_after_0 = model.evaluator().evaluate(move);
            auto score_after_1 = model.evaluator().evaluate(move, score_before);

            EXPECT_EQ(46, score_after_0.objective);
            EXPECT_EQ(5 + 1, score_after_0.total_violation);
            EXPECT_EQ(5 * 100 + 100, score_after_0.local_penalty);
            EXPECT_EQ(5 * 10000 + 10000, score_after_0.global_penalty);
            EXPECT_EQ(46 + 5 * 100 + 100,
                      score_after_0.local_augmented_objective);
            EXPECT_EQ(46 + 5 * 10000 + 10000,
                      score_after_0.global_augmented_objective);
            EXPECT_FALSE(score_after_0.is_objective_improvable);
            EXPECT_FALSE(score_after_0.is_feasibility_improvable);
            EXPECT_FALSE(score_after_0.is_feasible);

            EXPECT_EQ(46, score_after_1.objective);
            EXPECT_EQ(5 + 1, score_after_1.total_violation);
            EXPECT_EQ(5 * 100 + 100, score_after_1.local_penalty);
            EXPECT_EQ(5 * 10000 + 10000, score_after_1.global_penalty);
            EXPECT_EQ(46 + 5 * 100 + 100,
                      score_after_1.local_augmented_objective);
            EXPECT_EQ(46 + 5 * 10000 + 10000,
                      score_after_1.global_augmented_objective);
            EXPECT_FALSE(score_after_1.is_objective_improvable);
            EXPECT_FALSE(score_after_1.is_feasibility_improvable);
            EXPECT_FALSE(score_after_1.is_feasible);

            model.updater().update(move);
            score_before = score_after_1;
        }

        {
            neighborhood::Move<int, double> move;
            for (auto&& element : x.flat_indexed_variables()) {
                move.alterations.emplace_back(&element, 0);
                for (auto&& constraint_ptr :
                     element.related_constraint_ptrs()) {
                    move.related_constraint_ptrs.push_back(constraint_ptr);
                }
            }
            move.sort_and_unique_related_constraint_ptrs();

            auto score_after_0 = model.evaluator().evaluate(move);
            auto score_after_1 = model.evaluator().evaluate(move, score_before);

            EXPECT_EQ(1, score_after_0.objective);
            EXPECT_EQ(0, score_after_0.total_violation);
            EXPECT_EQ(0, score_after_0.local_penalty);
            EXPECT_EQ(0, score_after_0.global_penalty);
            EXPECT_EQ(1, score_after_0.local_augmented_objective);
            EXPECT_EQ(1, score_after_0.global_augmented_objective);
            EXPECT_TRUE(score_after_0.is_objective_improvable);
            EXPECT_TRUE(score_after_0.is_feasibility_improvable);
            EXPECT_TRUE(score_after_0.is_feasible);

            EXPECT_EQ(1, score_after_1.objective);
            EXPECT_EQ(0, score_after_1.total_violation);
            EXPECT_EQ(0, score_after_1.local_penalty);
            EXPECT_EQ(0, score_after_1.global_penalty);
            EXPECT_EQ(1, score_after_1.local_augmented_objective);
            EXPECT_EQ(1, score_after_1.global_augmented_objective);
            EXPECT_TRUE(score_after_1.is_objective_improvable);
            EXPECT_TRUE(score_after_1.is_feasibility_improvable);
            EXPECT_TRUE(score_after_1.is_feasible);

            model.updater().update(move);
            score_before = score_after_1;
        }

        {
            neighborhood::Move<int, double> move;
            for (auto i = 0; i < 5; i++) {
                move.alterations.emplace_back(&x(i), 1);
                for (auto&& constraint_ptr : x(i).related_constraint_ptrs()) {
                    move.related_constraint_ptrs.push_back(constraint_ptr);
                }
            }
            move.sort_and_unique_related_constraint_ptrs();

            auto score_after_0 = model.evaluator().evaluate(move);
            auto score_after_1 = model.evaluator().evaluate(move, score_before);

            EXPECT_EQ(11, score_after_0.objective);
            EXPECT_EQ(1, score_after_0.total_violation);
            EXPECT_EQ(100, score_after_0.local_penalty);
            EXPECT_EQ(10000, score_after_0.global_penalty);
            EXPECT_EQ(11 + 100, score_after_0.local_augmented_objective);
            EXPECT_EQ(11 + 10000, score_after_0.global_augmented_objective);
            EXPECT_FALSE(score_after_0.is_objective_improvable);
            EXPECT_FALSE(score_after_0.is_feasibility_improvable);
            EXPECT_FALSE(score_after_0.is_feasible);

            EXPECT_EQ(11, score_after_1.objective);
            EXPECT_EQ(1, score_after_1.total_violation);
            EXPECT_EQ(100, score_after_1.local_penalty);
            EXPECT_EQ(10000, score_after_1.global_penalty);
            EXPECT_EQ(11 + 100, score_after_1.local_augmented_objective);
            EXPECT_EQ(11 + 10000, score_after_1.global_augmented_objective);
            EXPECT_FALSE(score_after_1.is_objective_improvable);
            EXPECT_FALSE(score_after_1.is_feasibility_improvable);
            EXPECT_FALSE(score_after_1.is_feasible);

            model.updater().update(move);
            score_before = score_after_1;
        }

        {
            /// Single
            neighborhood::Move<int, double> move;
            move.alterations.emplace_back(&x(1), 0);
            move.related_constraint_ptrs = x(1).related_constraint_ptrs();

            auto score_after_0 = model.evaluator().evaluate(move);
            auto score_after_1 = model.evaluator().evaluate(move, score_before);

            EXPECT_EQ(10, score_after_0.objective);
            EXPECT_EQ(0, score_after_0.total_violation);
            EXPECT_EQ(0, score_after_0.local_penalty);
            EXPECT_EQ(0, score_after_0.global_penalty);
            EXPECT_EQ(10, score_after_0.local_augmented_objective);
            EXPECT_EQ(10, score_after_0.global_augmented_objective);
            EXPECT_TRUE(score_after_0.is_objective_improvable);
            EXPECT_TRUE(score_after_0.is_feasibility_improvable);
            EXPECT_TRUE(score_after_0.is_feasible);

            EXPECT_EQ(10, score_after_1.objective);
            EXPECT_EQ(0, score_after_1.total_violation);
            EXPECT_EQ(0, score_after_1.local_penalty);
            EXPECT_EQ(0, score_after_1.global_penalty);
            EXPECT_EQ(10, score_after_1.local_augmented_objective);
            EXPECT_EQ(10, score_after_1.global_augmented_objective);
            EXPECT_TRUE(score_after_1.is_objective_improvable);
            EXPECT_TRUE(score_after_1.is_feasibility_improvable);
            EXPECT_TRUE(score_after_1.is_feasible);

            model.updater().update(move);
            score_before = score_after_1;
        }
    }

    /// maximize
    {
        model::Model<int, double> model;

        auto sequence = utility::sequence(10);

        auto& x = model.create_variables("x", 10, 0, 1);
        auto& p = model.create_expression("p", x.dot(sequence) + 1);
        [[maybe_unused]] auto& g = model.create_constraint("g", x.sum() <= 5);
        [[maybe_unused]] auto& h =
            model.create_constraint("h", x(0) + x(1) <= 1);

        g(0).local_penalty_coefficient_less()    = 100;
        g(0).local_penalty_coefficient_greater() = 100;
        h(0).local_penalty_coefficient_less()    = 100;
        h(0).local_penalty_coefficient_greater() = 100;

        model.maximize(p);
        model.set_global_penalty_coefficient(10000);
        model.builder().setup_unique_names();
        model.builder().setup_structure();
        preprocess::SelectionExtractor<int, double> selection_extractor(&model);
        selection_extractor.extract_by_independent(false);
        model.builder().setup_fixed_sensitivities(false);

        for (auto&& element : x.flat_indexed_variables()) {
            element = 1;
        }
        model.updater().update();
        auto score_before = model.evaluator().evaluate({});

        {
            neighborhood::Move<int, double> move;
            for (auto&& element : x.flat_indexed_variables()) {
                move.alterations.emplace_back(&element, 1);
                for (auto&& constraint_ptr :
                     element.related_constraint_ptrs()) {
                    move.related_constraint_ptrs.push_back(constraint_ptr);
                }
            }
            move.sort_and_unique_related_constraint_ptrs();

            auto score_after_0 = model.evaluator().evaluate(move);
            auto score_after_1 = model.evaluator().evaluate(move, score_before);

            EXPECT_EQ(-46, score_after_0.objective);
            EXPECT_EQ(5 + 1, score_after_0.total_violation);
            EXPECT_EQ(5 * 100 + 100, score_after_0.local_penalty);
            EXPECT_EQ(5 * 10000 + 10000, score_after_0.global_penalty);
            EXPECT_EQ(-46 + 5 * 100 + 100,
                      score_after_0.local_augmented_objective);
            EXPECT_EQ(-46 + 5 * 10000 + 10000,
                      score_after_0.global_augmented_objective);
            EXPECT_FALSE(score_after_0.is_objective_improvable);
            EXPECT_FALSE(score_after_0.is_feasibility_improvable);
            EXPECT_FALSE(score_after_0.is_feasible);

            EXPECT_EQ(-46, score_after_1.objective);
            EXPECT_EQ(5 + 1, score_after_1.total_violation);
            EXPECT_EQ(5 * 100 + 100, score_after_1.local_penalty);
            EXPECT_EQ(5 * 10000 + 10000, score_after_1.global_penalty);
            EXPECT_EQ(-46 + 5 * 100 + 100,
                      score_after_1.local_augmented_objective);
            EXPECT_EQ(-46 + 5 * 10000 + 10000,
                      score_after_1.global_augmented_objective);
            EXPECT_FALSE(score_after_1.is_objective_improvable);
            EXPECT_FALSE(score_after_1.is_feasibility_improvable);
            EXPECT_FALSE(score_after_1.is_feasible);

            model.updater().update(move);
            score_before = score_after_1;
        }
        {
            neighborhood::Move<int, double> move;
            for (auto&& element : x.flat_indexed_variables()) {
                move.alterations.emplace_back(&element, 0);
                for (auto&& constraint_ptr :
                     element.related_constraint_ptrs()) {
                    move.related_constraint_ptrs.push_back(constraint_ptr);
                }
            }
            move.sort_and_unique_related_constraint_ptrs();

            auto score_after_0 = model.evaluator().evaluate(move);
            auto score_after_1 = model.evaluator().evaluate(move, score_before);

            EXPECT_EQ(-1, score_after_0.objective);
            EXPECT_EQ(0, score_after_0.total_violation);
            EXPECT_EQ(0, score_after_0.local_penalty);
            EXPECT_EQ(0, score_after_0.global_penalty);
            EXPECT_EQ(-1, score_after_0.local_augmented_objective);
            EXPECT_EQ(-1, score_after_0.global_augmented_objective);
            EXPECT_FALSE(score_after_0.is_objective_improvable);
            EXPECT_TRUE(score_after_0.is_feasibility_improvable);
            EXPECT_TRUE(score_after_0.is_feasible);

            EXPECT_EQ(-1, score_after_1.objective);
            EXPECT_EQ(0, score_after_0.total_violation);
            EXPECT_EQ(0, score_after_1.local_penalty);
            EXPECT_EQ(0, score_after_1.global_penalty);
            EXPECT_EQ(-1, score_after_1.local_augmented_objective);
            EXPECT_EQ(-1, score_after_1.global_augmented_objective);
            EXPECT_FALSE(score_after_1.is_objective_improvable);
            EXPECT_TRUE(score_after_1.is_feasibility_improvable);
            EXPECT_TRUE(score_after_1.is_feasible);

            model.updater().update(move);
            score_before = score_after_1;
        }

        {
            neighborhood::Move<int, double> move;
            for (auto i = 0; i < 5; i++) {
                move.alterations.emplace_back(&x(i), 1);
                for (auto&& constraint_ptr : x(i).related_constraint_ptrs()) {
                    move.related_constraint_ptrs.push_back(constraint_ptr);
                }
            }
            move.sort_and_unique_related_constraint_ptrs();

            auto score_after_0 = model.evaluator().evaluate(move);
            auto score_after_1 = model.evaluator().evaluate(move, score_before);

            EXPECT_EQ(-11, score_after_0.objective);
            EXPECT_EQ(1, score_after_0.total_violation);
            EXPECT_EQ(100, score_after_0.local_penalty);
            EXPECT_EQ(10000, score_after_0.global_penalty);
            EXPECT_EQ(-11 + 100, score_after_0.local_augmented_objective);
            EXPECT_EQ(-11 + 10000, score_after_0.global_augmented_objective);
            EXPECT_TRUE(score_after_0.is_objective_improvable);
            EXPECT_FALSE(score_after_0.is_feasibility_improvable);
            EXPECT_FALSE(score_after_0.is_feasible);

            EXPECT_EQ(-11, score_after_1.objective);
            EXPECT_EQ(1, score_after_1.total_violation);
            EXPECT_EQ(100, score_after_1.local_penalty);
            EXPECT_EQ(10000, score_after_1.global_penalty);
            EXPECT_EQ(-11 + 100, score_after_1.local_augmented_objective);
            EXPECT_EQ(-11 + 10000, score_after_1.global_augmented_objective);
            EXPECT_TRUE(score_after_1.is_objective_improvable);
            EXPECT_FALSE(score_after_1.is_feasibility_improvable);
            EXPECT_FALSE(score_after_1.is_feasible);

            model.updater().update(move);
            score_before = score_after_1;
        }

        {
            /// Single
            neighborhood::Move<int, double> move;
            move.alterations.emplace_back(&x(1), 0);
            move.related_constraint_ptrs = x(1).related_constraint_ptrs();

            auto score_after_0 = model.evaluator().evaluate(move);
            auto score_after_1 = model.evaluator().evaluate(move, score_before);

            EXPECT_EQ(-10, score_after_0.objective);
            EXPECT_EQ(0, score_after_0.total_violation);
            EXPECT_EQ(0, score_after_0.local_penalty);
            EXPECT_EQ(0, score_after_0.global_penalty);
            EXPECT_EQ(-10, score_after_0.local_augmented_objective);
            EXPECT_EQ(-10, score_after_0.global_augmented_objective);
            EXPECT_FALSE(score_after_0.is_objective_improvable);
            EXPECT_TRUE(score_after_0.is_feasibility_improvable);
            EXPECT_TRUE(score_after_0.is_feasible);

            EXPECT_EQ(-10, score_after_1.objective);
            EXPECT_EQ(0, score_after_1.total_violation);
            EXPECT_EQ(0, score_after_1.local_penalty);
            EXPECT_EQ(0, score_after_1.global_penalty);
            EXPECT_EQ(-10, score_after_1.local_augmented_objective);
            EXPECT_EQ(-10, score_after_1.global_augmented_objective);
            EXPECT_FALSE(score_after_1.is_objective_improvable);
            EXPECT_TRUE(score_after_1.is_feasibility_improvable);
            EXPECT_TRUE(score_after_1.is_feasible);

            model.updater().update(move);
            score_before = score_after_1;
        }
    }
}

/*****************************************************************************/
TEST_F(TestModelEvaluator, compute_lagrangian) {
    model::Model<int, double> model;

    auto sequence = utility::sequence(10);

    auto& x = model.create_variables("x", 10, 0, 1);
    auto& p = model.create_expression("p", x.dot(sequence) + 1);
    [[maybe_unused]] auto& g = model.create_constraint("g", x.sum() <= 5);
    [[maybe_unused]] auto& h = model.create_constraint("h", x(0) + x(1) <= 1);

    model.minimize(p);
    model.builder().setup_structure();

    multi_array::ValueProxy<double> dual_value_proxy(1);
    dual_value_proxy.value() = 100;

    std::vector<multi_array::ValueProxy<double>> dual = {dual_value_proxy,
                                                         dual_value_proxy};

    for (auto&& element : x.flat_indexed_variables()) {
        element = 1;
    }

    model.updater().update();
    auto lagrangian = model.evaluator().compute_lagrangian(dual);

    EXPECT_EQ(46 + 100 * (10 - 5) + 100 * (2 - 1), lagrangian);
}

/*****************************************************************************/
TEST_F(TestModelEvaluator, compute_naive_dual_bound) {
    {
        model::Model<int, double> model;

        auto& x = model.create_variables("x", 10, -1, 1);

        model.minimize(x.sum() + 1);
        model.builder().setup_structure();

        EXPECT_EQ(-10 + 1, model.evaluator().compute_naive_dual_bound());
    }

    {
        model::Model<int, double> model;

        auto& x = model.create_variables("x", 10, -1, 1);

        model.maximize(x.sum() + 1);
        model.builder().setup_structure();

        EXPECT_EQ(10 + 1, model.evaluator().compute_naive_dual_bound());
    }
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/