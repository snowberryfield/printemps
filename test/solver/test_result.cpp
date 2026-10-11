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
class TestResult : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown(void) {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestResult, initialize) {
    Result<int, double> result;

    EXPECT_EQ(result.status.termination_status, TerminationStatus::UNKNOWN);
    EXPECT_TRUE(result.feasible_solution_archive.solutions().empty());
}

/*****************************************************************************/
TEST_F(TestResult, constructor) {
    solution::NamedSolution<int, double>   solution;
    Status<int, double>                    status;
    solution::SolutionArchive<int, double> archive;

    status.termination_status = TerminationStatus::OPTIMAL;
    Result<int, double> result(solution, status, archive);

    EXPECT_EQ(result.status.termination_status, TerminationStatus::OPTIMAL);
    EXPECT_TRUE(result.feasible_solution_archive.solutions().empty());
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/
