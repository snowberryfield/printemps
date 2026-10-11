/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <gtest/gtest.h>
#include <printemps.h>

namespace {
using namespace printemps;
using namespace printemps::solver::pdlp::core;

/*****************************************************************************/
class TestPDLPCoreResult : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown(void) {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestPDLPCoreResult, initialize) {
    PDLPCoreResult result;

    EXPECT_EQ(result.number_of_iterations, 0);
    EXPECT_DOUBLE_EQ(result.elapsed_time, 0.0);
    EXPECT_EQ(result.termination_status,
              PDLPCoreTerminationStatus::ITERATION_OVER);
    EXPECT_EQ(result.primal.solution.size(), 0U);
    EXPECT_EQ(result.dual.solution.size(), 0U);
}

/*****************************************************************************/
TEST_F(TestPDLPCoreResult, constructor) {
    PDLPCoreState state;
    state.total_iteration    = 100;
    state.elapsed_time       = 3.2;
    state.termination_status = PDLPCoreTerminationStatus::OPTIMAL;

    option::Option option;
    PDLPCoreResult result(state, option);

    EXPECT_EQ(result.number_of_iterations, 100);
    EXPECT_DOUBLE_EQ(result.elapsed_time, 3.2);
    EXPECT_EQ(result.termination_status, PDLPCoreTerminationStatus::OPTIMAL);
}

/*****************************************************************************/
TEST_F(TestPDLPCoreResult, scaling) {
    PDLPCoreState state;
    state.primal = PrimalDualState(2);
    state.dual   = PrimalDualState(2);

    state.primal.solution.values = {2.0, 4.0};
    state.dual.solution.values   = {3.0, 6.0};

    option::Option option;
    PDLPCoreResult result(state, option);

    utility::sparse::Diagonal row_scaler(2);
    utility::sparse::Diagonal column_scaler(2);
    column_scaler[0] = 2.0;
    column_scaler[1] = 2.0;

    result.scaling(row_scaler, column_scaler);

    // column scaler divides primal solution: 2.0/2.0 = 1.0, 4.0/2.0 = 2.0
    EXPECT_DOUBLE_EQ(result.primal.solution.values[0], 1.0);
    EXPECT_DOUBLE_EQ(result.primal.solution.values[1], 2.0);
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/