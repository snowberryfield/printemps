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
        run_and_capture("./test/dat/wcnf/test_00b.wcnf", "2", &captured);

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
        run_and_capture("./test/dat/wcnf/test_00a.wcnf", "2", &captured);

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

/*****************************************************************************/
TEST_F(TestMaxSATEvaluationSolver, solve_test_00e_precision) {
    /**
     * test_00e: precision stress. Soft clauses carry weights ~2^60 so that
     * the double-typed internal objective can no longer distinguish costs
     * that differ by 1. The o-line must still match the exact uint64_t
     * cost of the returned assignment.
     *
     * Layout:
     *   hard:  (x1 OR ~x1)             -- always satisfied
     *   soft:  w=2^60  (~x1)
     *   soft:  w=1     (~x1)
     *   soft:  w=2^60  ( x1)
     *
     *   x1 = 0  ->  violated weight = 2^60       (= 1152921504606846976)
     *   x1 = 1  ->  violated weight = 2^60 + 1   (= 1152921504606846977)
     *
     * Under double accumulation 2^60 + 1 rounds to 2^60, so a
     * double-derived o-line would print 1152921504606846976 for x1 = 1.
     * The exact uint64_t recomputation must print 1152921504606846977.
     */
    std::string captured;
    const int   EXIT_CODE =
        run_and_capture("./test/dat/wcnf/test_00e.wcnf", "2", &captured);

    EXPECT_EQ(10, EXIT_CODE);
    EXPECT_NE(std::string::npos, captured.find("s SATISFIABLE"));

    /**
     * Parse the final (last) o-line and v-line and verify their mutual
     * consistency in uint64_t, since the solver is free to settle on
     * either x1 = 0 or x1 = 1 (the double objective sees them as equal).
     */
    auto find_last_line_starting_with =
        [&captured](const std::string &a_PREFIX) -> size_t {
        size_t pos     = std::string::npos;
        size_t cursor  = 0;
        while (true) {
            const size_t HIT = captured.find(a_PREFIX, cursor);
            if (HIT == std::string::npos) break;
            if (HIT == 0 || captured[HIT - 1] == '\n') {
                pos = HIT;
            }
            cursor = HIT + 1;
        }
        return pos;
    };

    const auto LAST_O = find_last_line_starting_with("o ");
    const auto LAST_V = find_last_line_starting_with("v ");
    ASSERT_NE(std::string::npos, LAST_O);
    ASSERT_NE(std::string::npos, LAST_V);

    const auto O_END = captured.find('\n', LAST_O);
    const auto V_END = captured.find('\n', LAST_V);
    ASSERT_NE(std::string::npos, O_END);
    ASSERT_NE(std::string::npos, V_END);

    const std::string O_VALUE =
        captured.substr(LAST_O + 2, O_END - (LAST_O + 2));
    const std::string V_BITS =
        captured.substr(LAST_V + 2, V_END - (LAST_V + 2));

    ASSERT_EQ(1u, V_BITS.size());
    ASSERT_TRUE(V_BITS[0] == '0' || V_BITS[0] == '1');

    const uint64_t W       = static_cast<uint64_t>(1) << 60;
    const uint64_t EXPECTED = (V_BITS[0] == '1') ? (W + 1) : W;

    EXPECT_EQ(std::to_string(EXPECTED), O_VALUE);

    /**
     * Sanity: ensure the printed o-value is not the double-truncated
     * version (this is the exact bug the test is guarding against).
     */
    if (V_BITS[0] == '1') {
        EXPECT_NE(std::to_string(W), O_VALUE);
    }
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/
