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

/*****************************************************************************/
class TestMemory : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown(void) {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestMemory, initialize) {
    Memory<int, double> memory;

    EXPECT_DOUBLE_EQ(memory.primal_intensity(), 0.0);
    EXPECT_DOUBLE_EQ(memory.dual_intensity(), 0.0);
    EXPECT_EQ(memory.total_update_count(), 0);
    EXPECT_DOUBLE_EQ(memory.total_update_count_reciprocal(), 0.0);
    EXPECT_EQ(memory.total_violation_count(), 0);
    EXPECT_DOUBLE_EQ(memory.total_violation_count_reciprocal(), 0.0);
}

/*****************************************************************************/
TEST_F(TestMemory, setup) {
    model::IPModel model;
    auto&          x = model.create_variables("x", 2, 0, 1);
    model.minimize(x(0) + x(1));

    option::Option                      option;
    model_handler::Builder<int, double> builder(&model);
    builder.build(option, false);

    Memory<int, double> memory(&model);
    EXPECT_DOUBLE_EQ(memory.primal_intensity(), 0.0);
    EXPECT_DOUBLE_EQ(memory.dual_intensity(), 0.0);
    EXPECT_EQ(x(0).local_last_update_iteration(),
              MemoryConstant::INITIAL_LOCAL_LAST_UPDATE_ITERATION);
}

/*****************************************************************************/
TEST_F(TestMemory, update) {
    model::IPModel model;
    auto&          x = model.create_variables("x", 2, 0, 1);
    model.minimize(x(0) + x(1));

    option::Option                      option;
    model_handler::Builder<int, double> builder(&model);
    builder.build(option, false);

    Memory<int, double> memory(&model);

    neighborhood::Move<int, double> move;
    move.alterations.emplace_back(x(0).reference(), 1);

    memory.update(move, 1);

    EXPECT_GT(memory.primal_intensity(), 0.0);
    EXPECT_EQ(memory.total_update_count(), 1);
    EXPECT_DOUBLE_EQ(memory.total_update_count_reciprocal(), 1.0);
    EXPECT_EQ(x(0).local_last_update_iteration(), 1);
    EXPECT_EQ(x(0).update_count(), 1);
}

/*****************************************************************************/
TEST_F(TestMemory, reset_local_last_update_iterations) {
    model::IPModel model;
    auto&          x = model.create_variables("x", 2, 0, 1);
    model.minimize(x(0) + x(1));

    option::Option                      option;
    model_handler::Builder<int, double> builder(&model);
    builder.build(option, false);

    Memory<int, double> memory(&model);

    neighborhood::Move<int, double> move;
    move.alterations.emplace_back(x(0).reference(), 1);
    memory.update(move, 5);
    EXPECT_EQ(x(0).local_last_update_iteration(), 5);

    memory.reset_local_last_update_iterations();
    EXPECT_EQ(x(0).local_last_update_iteration(),
              MemoryConstant::INITIAL_LOCAL_LAST_UPDATE_ITERATION);
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/