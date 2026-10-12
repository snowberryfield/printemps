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
class TestCDCLCoreState : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown(void) {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestCDCLCoreState, initialize) {
    CDCLCoreState state;

    EXPECT_EQ(state.number_of_conflicts, 0);
    EXPECT_EQ(state.current_decision_level, 0);
    EXPECT_EQ(state.iteration, 0);
    EXPECT_DOUBLE_EQ(state.elapsed_time, 0.0);
    EXPECT_EQ(state.termination_status, CDCLCoreTerminationStatus::NOT_SOLVED);

    EXPECT_TRUE(state.trail.empty());
    EXPECT_TRUE(state.trail_level_offsets.empty());
    EXPECT_EQ(state.propagation_queue_head, 0);
    EXPECT_EQ(state.conflict_constraint_index.index,
              CDCLCoreConstant::NO_CONSTRAINT);
    EXPECT_EQ(state.last_conflict_constraint.index,
              CDCLCoreConstant::NO_CONFLICT);
    EXPECT_EQ(state.repeated_conflict_constraints_count, 0);

    EXPECT_TRUE(state.activities.empty());
    EXPECT_DOUBLE_EQ(state.variable_activity_increment, 1.0);
    EXPECT_EQ(state.conflict_limit, 100);

    EXPECT_TRUE(state.seen_marker.empty());
    EXPECT_EQ(state.seen_conflict_id, 0);
    EXPECT_TRUE(state.seen_level_marker.empty());
    EXPECT_EQ(state.seen_level_id, 0);

    EXPECT_EQ(state.last_literal_block_distance, 0);
    EXPECT_EQ(state.last_learned_length, 0);
    EXPECT_EQ(state.number_of_assigned_zeros, 0);
    EXPECT_EQ(state.number_of_assigned_ones, 0);
    EXPECT_EQ(state.total_learned_literal_block_distance, 0);
    EXPECT_EQ(state.total_learned_length, 0);
    EXPECT_EQ(state.total_learned_count, 0);
    EXPECT_EQ(state.number_of_restarts, 0);
    EXPECT_EQ(state.number_of_decisions, 0);
    EXPECT_EQ(state.number_of_propagations, 0);

    EXPECT_EQ(state.restart_luby_count, 0);
    EXPECT_EQ(state.literal_block_distance_queue.size(), 0);

    EXPECT_TRUE(state.solution.empty());
    EXPECT_TRUE(state.best_solution.empty());
    EXPECT_DOUBLE_EQ(state.best_total_violation,
                     std::numeric_limits<double>::infinity());
    EXPECT_EQ(state.max_trail_size, 0);
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/