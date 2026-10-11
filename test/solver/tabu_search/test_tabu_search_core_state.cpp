/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <gtest/gtest.h>
#include <printemps.h>

namespace {
using namespace printemps;
using namespace printemps::solver::tabu_search::core;

/*****************************************************************************/
class TestTabuSearchCoreState : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown(void) {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestTabuSearchCoreState, initialize) {
    TabuSearchCoreState<int, double> state;

    EXPECT_EQ(state.update_status, 0);
    EXPECT_EQ(state.total_update_status, 0);
    EXPECT_EQ(state.iteration, 0);
    EXPECT_DOUBLE_EQ(state.elapsed_time, 0.0);
    EXPECT_EQ(state.termination_status,
              TabuSearchCoreTerminationStatus::ITERATION_OVER);

    EXPECT_EQ(state.number_of_effective_updates, 0);
    EXPECT_EQ(state.number_of_ineffective_updates, 0);
    EXPECT_EQ(state.number_of_moves, 0);
    EXPECT_EQ(state.number_of_evaluated_moves, 0);
    EXPECT_EQ(state.number_of_updated_moves, 0);

    EXPECT_FALSE(state.is_few_permissible_neighborhood);
    EXPECT_FALSE(state.is_found_new_feasible_solution);
    EXPECT_FALSE(state.is_aspirated);
    EXPECT_FALSE(state.is_objective_improved);
    EXPECT_FALSE(state.is_feasibility_improved);
    EXPECT_FALSE(state.is_improved);

    EXPECT_EQ(state.last_local_augmented_incumbent_update_iteration, 0);
    EXPECT_EQ(state.last_global_augmented_incumbent_update_iteration, 0);
    EXPECT_EQ(state.last_feasible_incumbent_update_iteration, 0);

    EXPECT_EQ(state.number_of_all_neighborhoods, 0);
    EXPECT_EQ(state.number_of_feasible_neighborhoods, 0);
    EXPECT_EQ(state.number_of_permissible_neighborhoods, 0);
    EXPECT_EQ(state.number_of_improvable_neighborhoods, 0);
}

}  // namespace
