/*****************************************************************************/
// Copyright (c) 2020-2026 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <gtest/gtest.h>
#include <printemps.h>

namespace {
using namespace printemps;
using namespace printemps::solver::cdcl::core;

/*****************************************************************************/
class TestCDCLCoreResult : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown(void) {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestCDCLCoreResult, initialize) {
    CDCLCoreResult result;

    EXPECT_EQ(result.termination_status, CDCLCoreTerminationStatus::NOT_SOLVED);
    EXPECT_TRUE(result.solution.empty());
    EXPECT_TRUE(result.best_solution.empty());
    EXPECT_DOUBLE_EQ(result.best_total_violation,
                     std::numeric_limits<double>::infinity());
    EXPECT_EQ(result.number_of_conflicts, 0);
    EXPECT_EQ(result.number_of_restarts, 0);
    EXPECT_EQ(result.number_of_decisions, 0);
    EXPECT_EQ(result.number_of_propagations, 0);
    EXPECT_EQ(result.total_learned_count, 0);
    EXPECT_EQ(result.unique_conflict_constraints_count, 0);
    EXPECT_EQ(result.repeated_conflict_constraints_count, 0);
    EXPECT_DOUBLE_EQ(result.average_learned_literal_block_distance, 0.0);
    EXPECT_DOUBLE_EQ(result.average_learned_length, 0.0);
    EXPECT_DOUBLE_EQ(result.decision_intensity, 0.0);
    EXPECT_DOUBLE_EQ(result.conflict_intensity, 0.0);
    EXPECT_DOUBLE_EQ(result.elapsed_time, 0.0);
}

/*****************************************************************************/
TEST_F(TestCDCLCoreResult, constructor) {
    CDCLCoreState state;
    state.termination_status     = CDCLCoreTerminationStatus::FEASIBLE;
    state.solution               = {1, 0, 1};
    state.best_solution          = {1, 0, 1};
    state.best_total_violation   = 0.0;
    state.number_of_conflicts    = 10;
    state.number_of_restarts     = 2;
    state.number_of_decisions    = 50;
    state.number_of_propagations = 100;
    state.total_learned_count    = 5;
    state.total_learned_literal_block_distance = 15;
    state.total_learned_length                 = 25;
    state.repeated_conflict_constraints_count  = 3;
    state.elapsed_time                         = 1.5;

    CDCLCoreResult result(state);

    EXPECT_EQ(result.termination_status, CDCLCoreTerminationStatus::FEASIBLE);
    EXPECT_EQ(result.solution.size(), 3U);
    EXPECT_EQ(result.best_solution.size(), 3U);
    EXPECT_DOUBLE_EQ(result.best_total_violation, 0.0);
    EXPECT_EQ(result.number_of_conflicts, 10);
    EXPECT_EQ(result.number_of_restarts, 2);
    EXPECT_EQ(result.number_of_decisions, 50);
    EXPECT_EQ(result.number_of_propagations, 100);
    EXPECT_EQ(result.total_learned_count, 5);
    EXPECT_EQ(result.repeated_conflict_constraints_count, 3);
    EXPECT_DOUBLE_EQ(result.average_learned_literal_block_distance, 3.0);
    EXPECT_DOUBLE_EQ(result.average_learned_length, 5.0);
    EXPECT_DOUBLE_EQ(result.elapsed_time, 1.5);
}

/*****************************************************************************/
TEST_F(TestCDCLCoreResult, setup) {
    /// This method is tested in constructor.
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/