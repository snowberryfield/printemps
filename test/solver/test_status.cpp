/*****************************************************************************/
// Copyright (c) 2020-2026 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <gtest/gtest.h>
#include <printemps.h>

namespace {
using namespace printemps;
using namespace printemps::solver;

/*****************************************************************************/
class TestStatus : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown(void) {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestStatus, initialize) {
    Status<int, double> status;

    EXPECT_EQ(status.solver_ptr, nullptr);
    EXPECT_EQ(status.model_ptr, nullptr);
    EXPECT_DOUBLE_EQ(status.objective, 0.0);
    EXPECT_DOUBLE_EQ(status.total_violation, 0.0);
    EXPECT_FALSE(status.is_found_feasible_solution);
    EXPECT_EQ(status.termination_status, TerminationStatus::UNKNOWN);
    EXPECT_DOUBLE_EQ(status.elapsed_time, 0.0);
    EXPECT_FALSE(status.is_proven_infeasible());
}

/*****************************************************************************/
TEST_F(TestStatus, is_proven_infeasible) {
    Status<int, double> status;

    status.termination_status = TerminationStatus::INFEASIBLE;
    EXPECT_TRUE(status.is_proven_infeasible());

    status.termination_status = TerminationStatus::FEASIBLE;
    EXPECT_FALSE(status.is_proven_infeasible());

    status.termination_status = TerminationStatus::OPTIMAL;
    EXPECT_FALSE(status.is_proven_infeasible());

    status.termination_status = TerminationStatus::REACH_TARGET;
    EXPECT_FALSE(status.is_proven_infeasible());
}

/*****************************************************************************/
TEST_F(TestStatus, termination_status_maps) {
    EXPECT_EQ(TerminationStatus::REACH_TARGET,
              TerminationStatusMap.at("REACH_TARGET"));
    EXPECT_EQ("REACH_TARGET",
              TerminationStatusInverseMap.at(TerminationStatus::REACH_TARGET));
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/