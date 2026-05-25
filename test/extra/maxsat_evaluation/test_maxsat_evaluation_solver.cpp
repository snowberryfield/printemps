/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <gtest/gtest.h>
#include <printemps.h>
#include "extra/maxsat_evaluation/maxsat_evaluation_solver.h"

namespace {
using namespace printemps;
namespace mse = printemps::extra::maxsat_evaluation;

/*****************************************************************************/
class TestMaxSATEvaluationSolver : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }

    /**
     * Drive MaxSATEvaluationSolver end-to-end against an on-disk WCNF
     * instance, capturing stdout. The argv layout matches the MSE 2026
     * anytime track: argv[0] = solver, argv[1] = wcnf path,
     * argv[2] = timeout in seconds.
     */
    int run_and_capture(const std::string &a_INSTANCE_PATH,
                        const std::string &a_TIMEOUT,
                        std::string       *a_out_stdout) {
        const char *FAKE_ARGV0 = "maxsat_evaluation_solver";
        std::array<const char *, 3> argv = {FAKE_ARGV0,
                                            a_INSTANCE_PATH.c_str(),
                                            a_TIMEOUT.c_str()};

        std::stringstream     captured;
        std::streambuf *const ORIGINAL_BUF = std::cout.rdbuf(captured.rdbuf());

        /**
         * The driver reads the namespace-local interrupt flag; reset it so a
         * previous test does not leak state.
         */
        mse::interrupted = false;

        int exit_code = 0;
        try {
            mse::MaxSATEvaluationSolver app;
            app.setup(static_cast<int>(argv.size()), argv.data());
            exit_code = app.solve();
        } catch (...) {
            std::cout.rdbuf(ORIGINAL_BUF);
            throw;
        }
        std::cout.rdbuf(ORIGINAL_BUF);

        if (a_out_stdout != nullptr) {
            *a_out_stdout = captured.str();
        }
        return exit_code;
    }
};

/*****************************************************************************/
TEST_F(TestMaxSATEvaluationSolver, solve_test_00b) {
    /**
     * test_00b: hard "x1", soft "~x1" w=1 -- optimal cost = 1, x1 = 1.
     */
    std::string captured;
    const int   EXIT_CODE =
        run_and_capture("./dat/wcnf/test_00b.wcnf", "2", &captured);

    EXPECT_EQ(10, EXIT_CODE);
    EXPECT_NE(std::string::npos, captured.find("o 1"));
    EXPECT_NE(std::string::npos, captured.find("v 1"));
    EXPECT_NE(std::string::npos, captured.find("s SATISFIABLE"));
}

/*****************************************************************************/
TEST_F(TestMaxSATEvaluationSolver, solve_test_00a) {
    /**
     * test_00a: optimal cost = 0 (all soft clauses can be satisfied
     * simultaneously while satisfying the hard clause).
     */
    std::string captured;
    const int   EXIT_CODE =
        run_and_capture("./dat/wcnf/test_00a.wcnf", "2", &captured);

    EXPECT_EQ(10, EXIT_CODE);
    EXPECT_NE(std::string::npos, captured.find("o 0"));
    EXPECT_NE(std::string::npos, captured.find("s SATISFIABLE"));

    /// Locate the v line and confirm its bitstring length equals #variables.
    const auto V_POS = captured.find("\nv ");
    ASSERT_NE(std::string::npos, V_POS);
    const auto LINE_END = captured.find('\n', V_POS + 1);
    ASSERT_NE(std::string::npos, LINE_END);
    const std::string V_LINE =
        captured.substr(V_POS + 1, LINE_END - V_POS - 1);
    /// "v <bits>" -- 7 variables, so 7 chars after "v ".
    ASSERT_EQ(2u + 7u, V_LINE.size());
    for (size_t i = 2; i < V_LINE.size(); i++) {
        EXPECT_TRUE(V_LINE[i] == '0' || V_LINE[i] == '1');
    }
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/
