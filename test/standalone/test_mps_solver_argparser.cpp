/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <gtest/gtest.h>
#include <printemps.h>

namespace {
using namespace printemps;
/*****************************************************************************/
class TestMPSSolverArgparser : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown() {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestMPSSolverArgparser, constructor) {
    standalone::mps_solver::MPSSolverArgparser argparser;

    EXPECT_TRUE(argparser.mps_file_name.empty());
    EXPECT_TRUE(argparser.option_file_name.empty());
    EXPECT_TRUE(argparser.initial_solution_file_name.empty());
    EXPECT_TRUE(argparser.mutable_variable_file_name.empty());
    EXPECT_TRUE(argparser.fixed_variable_file_name.empty());
    EXPECT_TRUE(argparser.selection_constraint_file_name.empty());
    EXPECT_TRUE(argparser.flippable_variable_pair_file_name.empty());

    EXPECT_EQ(standalone::mps_solver::MPSSolverArgparserConstant::
                  DEFAULT_MINIMUM_COMMON_ELEMENT,
              argparser.minimum_common_element);

    EXPECT_EQ(standalone::mps_solver::MPSSolverArgparserConstant::
                  DEFAULT_ACCEPT_CONTINUOUS_VARIABLES,
              argparser.accept_continuous_variables);

    EXPECT_EQ(standalone::mps_solver::MPSSolverArgparserConstant::
                  DEFAULT_EXTRACT_FLIPPABLE_VARIABLE_PAIRS,
              argparser.extract_flippable_variable_pairs);

    EXPECT_EQ(standalone::mps_solver::MPSSolverArgparserConstant::
                  DEFAULT_INCLUDE_MPS_LOADING_TIME,
              argparser.include_mps_loading_time);

    EXPECT_EQ(standalone::mps_solver::MPSSolverArgparserConstant::
                  DEFAULT_EXPORT_JSON_INSTANCE,
              argparser.export_json_instance);

    EXPECT_EQ(standalone::mps_solver::MPSSolverArgparserConstant::
                  DEFAULT_IS_MINIMIZATION_EXPLICIT,
              argparser.is_minimization_explicit);

    EXPECT_EQ(standalone::mps_solver::MPSSolverArgparserConstant::
                  DEFAULT_IS_MAXIMIZATION_EXPLICIT,
              argparser.is_maximization_explicit);

    EXPECT_FLOAT_EQ(option::GeneralOptionConstant::  //
                    DEFAULT_ITERATION_MAX,
                    argparser.iteration_max);

    EXPECT_FLOAT_EQ(option::GeneralOptionConstant::  //
                    DEFAULT_TIME_MAX,
                    argparser.time_max);

    EXPECT_EQ(option::OutputOptionConstant::  //
              DEFAULT_VERBOSE,
              argparser.verbose);

    EXPECT_EQ(standalone::mps_solver::MPSSolverArgparserConstant::  //
              DEFAULT_NUMBER_OF_THREADS,
              argparser.number_of_threads);

    EXPECT_FALSE(argparser.is_specified_iteration_max);
    EXPECT_FALSE(argparser.is_specified_time_max);
    EXPECT_FALSE(argparser.is_specified_verbose);
    EXPECT_FALSE(argparser.is_specified_number_of_threads);
}

/*****************************************************************************/
TEST_F(TestMPSSolverArgparser, initialize) {
    standalone::mps_solver::MPSSolverArgparser argparser;
    argparser.mps_file_name                     = "test";
    argparser.option_file_name                  = "test";
    argparser.initial_solution_file_name        = "test";
    argparser.mutable_variable_file_name        = "test";
    argparser.fixed_variable_file_name          = "test";
    argparser.selection_constraint_file_name    = "test";
    argparser.flippable_variable_pair_file_name = "test";

    argparser.minimum_common_element =
        standalone::mps_solver::MPSSolverArgparserConstant::
            DEFAULT_MINIMUM_COMMON_ELEMENT +
        1;
    argparser.accept_continuous_variables =
        !standalone::mps_solver::MPSSolverArgparserConstant::
            DEFAULT_ACCEPT_CONTINUOUS_VARIABLES;
    argparser.extract_flippable_variable_pairs =
        !standalone::mps_solver::MPSSolverArgparserConstant::
            DEFAULT_EXTRACT_FLIPPABLE_VARIABLE_PAIRS;
    argparser.include_mps_loading_time =
        !standalone::mps_solver::MPSSolverArgparserConstant::
            DEFAULT_INCLUDE_MPS_LOADING_TIME;
    argparser.export_json_instance =
        !standalone::mps_solver::MPSSolverArgparserConstant::
            DEFAULT_EXPORT_JSON_INSTANCE;
    argparser.is_minimization_explicit =
        !standalone::mps_solver::MPSSolverArgparserConstant::
            DEFAULT_IS_MINIMIZATION_EXPLICIT;
    argparser.is_maximization_explicit =
        !standalone::mps_solver::MPSSolverArgparserConstant::
            DEFAULT_IS_MAXIMIZATION_EXPLICIT;

    argparser.initialize();

    EXPECT_TRUE(argparser.mps_file_name.empty());
    EXPECT_TRUE(argparser.option_file_name.empty());
    EXPECT_TRUE(argparser.initial_solution_file_name.empty());
    EXPECT_TRUE(argparser.mutable_variable_file_name.empty());
    EXPECT_TRUE(argparser.fixed_variable_file_name.empty());
    EXPECT_TRUE(argparser.selection_constraint_file_name.empty());
    EXPECT_TRUE(argparser.flippable_variable_pair_file_name.empty());

    EXPECT_EQ(standalone::mps_solver::MPSSolverArgparserConstant::
                  DEFAULT_MINIMUM_COMMON_ELEMENT,
              argparser.minimum_common_element);

    EXPECT_FALSE(argparser.is_specified_number_of_threads);

    EXPECT_EQ(standalone::mps_solver::MPSSolverArgparserConstant::
                  DEFAULT_ACCEPT_CONTINUOUS_VARIABLES,
              argparser.accept_continuous_variables);

    EXPECT_EQ(standalone::mps_solver::MPSSolverArgparserConstant::
                  DEFAULT_EXTRACT_FLIPPABLE_VARIABLE_PAIRS,
              argparser.extract_flippable_variable_pairs);

    EXPECT_EQ(standalone::mps_solver::MPSSolverArgparserConstant::
                  DEFAULT_INCLUDE_MPS_LOADING_TIME,
              argparser.include_mps_loading_time);

    EXPECT_EQ(standalone::mps_solver::MPSSolverArgparserConstant::
                  DEFAULT_EXPORT_JSON_INSTANCE,
              argparser.export_json_instance);

    EXPECT_EQ(standalone::mps_solver::MPSSolverArgparserConstant::
                  DEFAULT_IS_MINIMIZATION_EXPLICIT,
              argparser.is_minimization_explicit);

    EXPECT_EQ(standalone::mps_solver::MPSSolverArgparserConstant::
                  DEFAULT_IS_MAXIMIZATION_EXPLICIT,
              argparser.is_maximization_explicit);

    EXPECT_FLOAT_EQ(option::GeneralOptionConstant::  //
                    DEFAULT_ITERATION_MAX,
                    argparser.iteration_max);

    EXPECT_FLOAT_EQ(option::GeneralOptionConstant::  //
                    DEFAULT_TIME_MAX,
                    argparser.time_max);

    EXPECT_EQ(option::OutputOptionConstant::  //
              DEFAULT_VERBOSE,
              argparser.verbose);

    EXPECT_EQ(standalone::mps_solver::MPSSolverArgparserConstant::  //
              DEFAULT_NUMBER_OF_THREADS,
              argparser.number_of_threads);

    EXPECT_FALSE(argparser.is_specified_iteration_max);
    EXPECT_FALSE(argparser.is_specified_time_max);
    EXPECT_FALSE(argparser.is_specified_verbose);
    EXPECT_FALSE(argparser.is_specified_number_of_threads);
}

/*****************************************************************************/
TEST_F(TestMPSSolverArgparser, parse) {
    {
        standalone::mps_solver::MPSSolverArgparser argparser;
        const char* argv[] = {"mps_solver", "test.mps"};
        argparser.parse(2, argv);
        EXPECT_EQ("test.mps", argparser.mps_file_name);
    }

    {
        standalone::mps_solver::MPSSolverArgparser argparser;
        const char* argv[] = {"mps_solver", "-p", "option.json"};
        argparser.parse(3, argv);
        EXPECT_EQ("option.json", argparser.option_file_name);
    }

    {
        standalone::mps_solver::MPSSolverArgparser argparser;
        const char* argv[] = {"mps_solver", "-i", "initial.sol"};
        argparser.parse(3, argv);
        EXPECT_EQ("initial.sol", argparser.initial_solution_file_name);
    }

    {
        standalone::mps_solver::MPSSolverArgparser argparser;
        const char* argv[] = {"mps_solver", "-m", "mutable.txt"};
        argparser.parse(3, argv);
        EXPECT_EQ("mutable.txt", argparser.mutable_variable_file_name);
    }

    {
        standalone::mps_solver::MPSSolverArgparser argparser;
        const char* argv[] = {"mps_solver", "-f", "fixed.txt"};
        argparser.parse(3, argv);
        EXPECT_EQ("fixed.txt", argparser.fixed_variable_file_name);
    }

    {
        standalone::mps_solver::MPSSolverArgparser argparser;
        const char* argv[] = {"mps_solver", "-s", "selection.txt"};
        argparser.parse(3, argv);
        EXPECT_EQ("selection.txt", argparser.selection_constraint_file_name);
    }

    {
        standalone::mps_solver::MPSSolverArgparser argparser;
        const char* argv[] = {"mps_solver", "-x", "flip.txt"};
        argparser.parse(3, argv);
        EXPECT_EQ("flip.txt", argparser.flippable_variable_pair_file_name);
    }

    {
        standalone::mps_solver::MPSSolverArgparser argparser;
        const char* argv[] = {"mps_solver", "-c", "10"};
        argparser.parse(3, argv);
        EXPECT_EQ(10, argparser.minimum_common_element);
    }

    {
        standalone::mps_solver::MPSSolverArgparser argparser;
        const char* argv[] = {"mps_solver", "-k", "10"};
        argparser.parse(3, argv);
        EXPECT_EQ(10, argparser.iteration_max);
    }

    {
        standalone::mps_solver::MPSSolverArgparser argparser;
        const char* argv[] = {"mps_solver", "-t", "10"};
        argparser.parse(3, argv);
        EXPECT_EQ(10, argparser.time_max);
    }

    {
        standalone::mps_solver::MPSSolverArgparser argparser;
        const char* argv[] = {"mps_solver", "-v", "Off"};
        argparser.parse(3, argv);
        EXPECT_EQ(option::verbose::Off, argparser.verbose);
    }

    {
        standalone::mps_solver::MPSSolverArgparser argparser;
        const char* argv[] = {"mps_solver", "-v", "Warning"};
        argparser.parse(3, argv);
        EXPECT_EQ(option::verbose::Warning, argparser.verbose);
    }

    {
        standalone::mps_solver::MPSSolverArgparser argparser;
        const char* argv[] = {"mps_solver", "-v", "Outer"};
        argparser.parse(3, argv);
        EXPECT_EQ(option::verbose::Outer, argparser.verbose);
    }

    {
        standalone::mps_solver::MPSSolverArgparser argparser;
        const char* argv[] = {"mps_solver", "-v", "Inner"};
        argparser.parse(3, argv);
        EXPECT_EQ(option::verbose::Inner, argparser.verbose);
    }

    {
        standalone::mps_solver::MPSSolverArgparser argparser;
        const char* argv[] = {"mps_solver", "-v", "Full"};
        argparser.parse(3, argv);
        EXPECT_EQ(option::verbose::Full, argparser.verbose);
    }

    {
        standalone::mps_solver::MPSSolverArgparser argparser;
        const char* argv[] = {"mps_solver", "-v", "Debug"};
        argparser.parse(3, argv);
        EXPECT_EQ(option::verbose::Debug, argparser.verbose);
    }

    {
        standalone::mps_solver::MPSSolverArgparser argparser;
        const char* argv[] = {"mps_solver", "-j", "10"};
        argparser.parse(3, argv);
        EXPECT_EQ(10, argparser.number_of_threads);
    }

    {
        standalone::mps_solver::MPSSolverArgparser argparser;
        const char* argv[] = {"mps_solver", "--accept-continuous"};
        argparser.parse(2, argv);
        EXPECT_TRUE(argparser.accept_continuous_variables);
    }

    {
        standalone::mps_solver::MPSSolverArgparser argparser;
        const char*                                argv[] = {"mps_solver",
                                                             "--extract-flippable-variable-pairs"};
        argparser.parse(2, argv);
        EXPECT_TRUE(argparser.extract_flippable_variable_pairs);
    }

    {
        standalone::mps_solver::MPSSolverArgparser argparser;
        const char* argv[] = {"mps_solver", "--include-mps-loading-time"};
        argparser.parse(2, argv);
        EXPECT_TRUE(argparser.include_mps_loading_time);
    }

    {
        standalone::mps_solver::MPSSolverArgparser argparser;
        const char* argv[] = {"mps_solver", "--export-json-instance"};
        argparser.parse(2, argv);
        EXPECT_TRUE(argparser.export_json_instance);
    }

    {
        standalone::mps_solver::MPSSolverArgparser argparser;
        const char* argv[] = {"mps_solver", "--minimization"};
        argparser.parse(2, argv);
        EXPECT_TRUE(argparser.is_minimization_explicit);
    }

    {
        standalone::mps_solver::MPSSolverArgparser argparser;
        const char* argv[] = {"mps_solver", "--minimize"};
        argparser.parse(2, argv);
        EXPECT_TRUE(argparser.is_minimization_explicit);
    }

    {
        standalone::mps_solver::MPSSolverArgparser argparser;
        const char* argv[] = {"mps_solver", "--min"};
        argparser.parse(2, argv);
        EXPECT_TRUE(argparser.is_minimization_explicit);
    }

    {
        standalone::mps_solver::MPSSolverArgparser argparser;
        const char* argv[] = {"mps_solver", "--maximization"};
        argparser.parse(2, argv);
        EXPECT_TRUE(argparser.is_maximization_explicit);
    }

    {
        standalone::mps_solver::MPSSolverArgparser argparser;
        const char* argv[] = {"mps_solver", "--maximize"};
        argparser.parse(2, argv);
        EXPECT_TRUE(argparser.is_maximization_explicit);
    }

    {
        standalone::mps_solver::MPSSolverArgparser argparser;
        const char* argv[] = {"mps_solver", "--max"};
        argparser.parse(2, argv);
        EXPECT_TRUE(argparser.is_maximization_explicit);
    }
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/