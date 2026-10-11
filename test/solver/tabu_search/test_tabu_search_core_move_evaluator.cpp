/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <gtest/gtest.h>
#include <printemps.h>

namespace {
using namespace printemps;
using namespace printemps::solver;
using namespace printemps::solver::tabu_search::core;

/*****************************************************************************/
class TestTabuSearchCoreMoveEvaluator : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown(void) {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestTabuSearchCoreMoveEvaluator, constructor) {
    /// This method is tested in compute_permissibility().
}

/*****************************************************************************/
TEST_F(TestTabuSearchCoreMoveEvaluator, initialize) {
    TabuSearchCoreMoveEvaluator<int, double> evaluator;
    EXPECT_NO_THROW(evaluator.initialize());
}

/*****************************************************************************/
TEST_F(TestTabuSearchCoreMoveEvaluator, setup) {
    model::IPModel model;
    auto&          x = model.create_variable("x", 0, 1);
    model.minimize(x);

    option::Option                      option;
    model_handler::Builder<int, double> builder(&model);
    builder.build(option, false);

    Memory<int, double> memory(&model);

    TabuSearchCoreMoveEvaluator<int, double> evaluator;
    EXPECT_NO_THROW(evaluator.setup(&model, &memory, option));

    neighborhood::Move<int, double> move;
    EXPECT_EQ(evaluator.compute_total_update_count(move), 0);
}

/*****************************************************************************/
TEST_F(TestTabuSearchCoreMoveEvaluator, compute_permissibility) {
    model::IPModel model;
    auto&          x = model.create_variable("x", 0, 1);
    model.minimize(x);

    option::Option                      option;
    model_handler::Builder<int, double> builder(&model);
    builder.build(option, false);

    Memory<int, double> memory(&model);

    TabuSearchCoreMoveEvaluator<int, double> evaluator(&model, &memory, option);

    neighborhood::Move<int, double> move;
    move.alterations.emplace_back(x.flat_indexed_variables(0).reference(), 1);

    // Initial local_last_update_iteration is -1000, duration 0 >= -1000 ->
    // permissible
    EXPECT_TRUE(evaluator.compute_permissibility(move, 0));
}

/*****************************************************************************/
TEST_F(TestTabuSearchCoreMoveEvaluator, compute_total_update_count) {
    model::IPModel model;
    auto&          x = model.create_variable("x", 0, 1);
    model.minimize(x);

    option::Option                      option;
    model_handler::Builder<int, double> builder(&model);
    builder.build(option, false);

    Memory<int, double> memory(&model);

    TabuSearchCoreMoveEvaluator<int, double> evaluator(&model, &memory, option);

    neighborhood::Move<int, double> move;
    move.alterations.emplace_back(x.flat_indexed_variables(0).reference(), 1);

    EXPECT_EQ(evaluator.compute_total_update_count(move), 0);
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/