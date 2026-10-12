/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <gtest/gtest.h>
#include <fstream>
#include <printemps.h>

namespace {
using namespace printemps;

inline std::string find_test_file(const std::string &relative_path) {
    if (std::ifstream(relative_path).good()) {
        return relative_path;
    }
    const std::string from_build = "./build/test/Release/" + relative_path;
    if (std::ifstream(from_build).good()) {
        return from_build;
    }
    const std::string from_root = "test/" + relative_path;
    if (std::ifstream(from_root).good()) {
        return from_root;
    }
    return relative_path;
}

/*****************************************************************************/
class TestStandalone : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestStandalone, option_iteration_and_time_limits_without_option_file) {
    const std::string mps_file = find_test_file("dat/mps/test_00.mps");

    /**
     * Case 1: Neither iteration_max nor time_max is specified.
     * Both should remain default.
     */
    {
        const char *argv[] = {"printemps", "--accept-continuous",
                              mps_file.c_str()};
        standalone::Standalone standalone(3, argv);
        EXPECT_EQ(option::GeneralOptionConstant::DEFAULT_ITERATION_MAX,
                  standalone.option().general.iteration_max);
        EXPECT_FLOAT_EQ(option::GeneralOptionConstant::DEFAULT_TIME_MAX,
                        standalone.option().general.time_max);
    }

    /**
     * Case 2: Only iteration_max (-k) is specified.
     * iteration_max should be the specified value, and time_max should be unlimited (-1).
     */
    {
        const char *argv[] = {"printemps", "--accept-continuous", "-k", "50",
                              mps_file.c_str()};
        standalone::Standalone standalone(5, argv);
        EXPECT_EQ(50, standalone.option().general.iteration_max);
        EXPECT_FLOAT_EQ(-1.0, standalone.option().general.time_max);
    }

    /**
     * Case 3: Only time_max (-t) is specified.
     * iteration_max should be unlimited (-1), and time_max should be the specified value.
     */
    {
        const char *argv[] = {"printemps", "--accept-continuous", "-t", "30.0",
                              mps_file.c_str()};
        standalone::Standalone standalone(5, argv);
        EXPECT_EQ(-1, standalone.option().general.iteration_max);
        EXPECT_FLOAT_EQ(30.0, standalone.option().general.time_max);
    }

    /**
     * Case 4: Both iteration_max (-k) and time_max (-t) are specified.
     * Both should be the specified values.
     */
    {
        const char *argv[] = {"printemps", "--accept-continuous", "-k", "50",
                              "-t",        "30.0",               mps_file.c_str()};
        standalone::Standalone standalone(7, argv);
        EXPECT_EQ(50, standalone.option().general.iteration_max);
        EXPECT_FLOAT_EQ(30.0, standalone.option().general.time_max);
    }
}

/*****************************************************************************/
TEST_F(TestStandalone, option_iteration_and_time_limits_with_option_file) {
    const std::string mps_file    = find_test_file("dat/mps/test_00.mps");
    const std::string option_file = find_test_file("dat/option/option_00.json");

    /**
     * Case 1: Option file is specified without -k or -t.
     * Values in the option file should be used.
     * In option_00.json: iteration_max = 1, time_max = 0.1
     */
    {
        const char *argv[] = {"printemps", "--accept-continuous", "-p",
                              option_file.c_str(), mps_file.c_str()};
        standalone::Standalone standalone(5, argv);
        EXPECT_EQ(1, standalone.option().general.iteration_max);
        EXPECT_FLOAT_EQ(0.1, standalone.option().general.time_max);
    }

    /**
     * Case 2: Option file is specified with only -k.
     * iteration_max is overwritten, and time_max remains the value from option file.
     */
    {
        const char *argv[] = {"printemps", "--accept-continuous", "-p",
                              option_file.c_str(), "-k", "50", mps_file.c_str()};
        standalone::Standalone standalone(7, argv);
        EXPECT_EQ(50, standalone.option().general.iteration_max);
        EXPECT_FLOAT_EQ(0.1, standalone.option().general.time_max);
    }

    /**
     * Case 3: Option file is specified with only -t.
     * time_max is overwritten, and iteration_max remains the value from option file.
     */
    {
        const char *argv[] = {"printemps", "--accept-continuous", "-p",
                              option_file.c_str(), "-t", "30.0", mps_file.c_str()};
        standalone::Standalone standalone(7, argv);
        EXPECT_EQ(1, standalone.option().general.iteration_max);
        EXPECT_FLOAT_EQ(30.0, standalone.option().general.time_max);
    }

    /**
     * Case 4: Option file is specified with both -k and -t.
     * Both should be overwritten.
     */
    {
        const char *argv[] = {"printemps", "--accept-continuous", "-p",
                              option_file.c_str(), "-k", "50", "-t", "30.0",
                              mps_file.c_str()};
        standalone::Standalone standalone(9, argv);
        EXPECT_EQ(50, standalone.option().general.iteration_max);
        EXPECT_FLOAT_EQ(30.0, standalone.option().general.time_max);
    }
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/
