/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_STANDALONE_MPS_SOLVER_MPS_SOLVER_ARG_PARSER_H__
#define PRINTEMPS_STANDALONE_MPS_SOLVER_MPS_SOLVER_ARG_PARSER_H__

namespace printemps::standalone::mps_solver {
/*****************************************************************************/
struct MPSSolverArgparserConstant {
    static constexpr int  DEFAULT_MINIMUM_COMMON_ELEMENT           = 5;
    static constexpr bool DEFAULT_ACCEPT_CONTINUOUS_VARIABLES      = false;
    static constexpr bool DEFAULT_EXTRACT_FLIPPABLE_VARIABLE_PAIRS = false;
    static constexpr bool DEFAULT_INCLUDE_MPS_LOADING_TIME         = false;
    static constexpr bool DEFAULT_EXPORT_JSON_INSTANCE             = false;
    static constexpr bool DEFAULT_IS_MINIMIZATION_EXPLICIT         = false;
    static constexpr bool DEFAULT_IS_MAXIMIZATION_EXPLICIT         = false;

    static constexpr bool DEFAULT_IS_ITERATION_MAX_GIVEN = false;
    static constexpr bool DEFAULT_IS_TIME_MAX_GIVEN      = false;
    static constexpr bool DEFAULT_IS_VERBOSE_GIVEN       = false;
};

/*****************************************************************************/
struct MPSSolverArgparser {
    std::string mps_file_name;
    std::string option_file_name;
    std::string initial_solution_file_name;
    std::string mutable_variable_file_name;
    std::string fixed_variable_file_name;
    std::string selection_constraint_file_name;
    std::string flippable_variable_pair_file_name;
    int         minimum_common_element;
    bool        accept_continuous_variables;
    bool        extract_flippable_variable_pairs;
    bool        include_mps_loading_time;
    bool        export_json_instance;
    bool        is_minimization_explicit;
    bool        is_maximization_explicit;

    double                   iteration_max;
    double                   time_max;
    option::verbose::Verbose verbose;
    bool                     is_iteration_max_given;
    bool                     is_time_max_given;
    bool                     is_verbose_given;

    /*************************************************************************/
    MPSSolverArgparser(void) {
        this->initialize();
    }

    /*************************************************************************/
    inline void initialize(void) {
        this->mps_file_name.clear();
        this->option_file_name.clear();
        this->initial_solution_file_name.clear();
        this->mutable_variable_file_name.clear();
        this->fixed_variable_file_name.clear();
        this->selection_constraint_file_name.clear();
        this->flippable_variable_pair_file_name.clear();
        this->minimum_common_element =
            MPSSolverArgparserConstant::DEFAULT_MINIMUM_COMMON_ELEMENT;
        this->accept_continuous_variables =
            MPSSolverArgparserConstant::DEFAULT_ACCEPT_CONTINUOUS_VARIABLES;
        this->extract_flippable_variable_pairs = MPSSolverArgparserConstant::
            DEFAULT_EXTRACT_FLIPPABLE_VARIABLE_PAIRS;
        this->include_mps_loading_time =
            MPSSolverArgparserConstant::DEFAULT_INCLUDE_MPS_LOADING_TIME;
        this->export_json_instance =
            MPSSolverArgparserConstant::DEFAULT_EXPORT_JSON_INSTANCE;
        this->is_minimization_explicit =
            MPSSolverArgparserConstant::DEFAULT_IS_MINIMIZATION_EXPLICIT;
        this->is_maximization_explicit =
            MPSSolverArgparserConstant::DEFAULT_IS_MAXIMIZATION_EXPLICIT;

        this->iteration_max =
            option::GeneralOptionConstant::DEFAULT_ITERATION_MAX;
        this->time_max = option::GeneralOptionConstant::DEFAULT_TIME_MAX;
        this->verbose  = option::OutputOptionConstant::DEFAULT_VERBOSE;

        this->is_iteration_max_given =
            MPSSolverArgparserConstant::DEFAULT_IS_ITERATION_MAX_GIVEN;
        this->is_time_max_given =
            MPSSolverArgparserConstant::DEFAULT_IS_TIME_MAX_GIVEN;
        this->is_verbose_given =
            MPSSolverArgparserConstant::DEFAULT_IS_VERBOSE_GIVEN;
    }

    /*************************************************************************/
    inline void print_usage(void) const {
        std::cout << std::endl;
        std::cout << "PRINTEMPS " + constant::VERSION + " (" +
                         constant::PROJECT_URL + ")"
                  << std::endl;
        std::cout << std::endl;

        std::cout << "Usage: ./mps_solver "
                  << "[-p OPTION_FILE_NAME] "
                  << "[-i INITIAL_SOLUTION_FILE_NAME] "
                  << "[-m MUTABLE_VARIABLE_FILE_NAME] "
                  << "[-f FIXED_VARIABLE_FILE_NAME] "
                  << "[-s SELECTION_CONSTRAINT_FILE_NAME] "
                  << "[-x FLIPPABLE_VARIABLE_PAIR_FILE_NAME]"
                  << "[-c MINIMUM_COMMON_ELEMENT] "
                  << "[-k ITERATION_MAX] "
                  << "[-t TIME_MAX] "
                  << "[-v VERVOSE] "
                  << "[--accept-continuous] "
                  << "[--extract-flippable-variable-pairs] "
                  << "[--include-mps-loading-time] "
                  << "[--export-json-instance] "
                  << "[--minimization] "
                  << "[--maximization] "
                  << "mps_file" << std::endl;
        std::cout << std::endl;
        std::cout  //
            << "  -p OPTION_FILE_NAME: Specify option file name." << std::endl;
        std::cout  //
            << "  -i INITIAL_SOLUTION_FILE_NAME: Specify initial solution "
               "file name."
            << std::endl;
        std::cout  //
            << "  -m MUTABLE_VARIABLE_FILE_NAME: Specify mutable variable file "
               "name."
            << std::endl;
        std::cout  //
            << "  -f FIXED_VARIABLE_FILE_NAME: Specify fixed variable file "
               "name."
            << std::endl;
        std::cout  //
            << "  -s SELECTION_CONSTRAINT_FILE_NAME: Specify user-defined "
               "selection constraint file name."
            << std::endl;
        std::cout  //
            << "  -x FLIP_VARIABLE_PAIR_FILE_NAME: Specify flippable variable "
               "pair file name."
            << std::endl;
        std::cout  //
            << "  -c MINIMUM_COMMON_ELEMENT: Specify the number of minimum "
               "common element between two constraints, which is used as the "
               "threshold for extracting flippable variable pairs. (default: "
            << MPSSolverArgparserConstant::DEFAULT_MINIMUM_COMMON_ELEMENT << ")"
            << std::endl;
        std::cout  //
            << "  -k ITERATION_MAX: Specify the allowed maximum number of "
               "outer loop iterations. (default: "
            << option::GeneralOptionConstant::DEFAULT_ITERATION_MAX << ")"
            << std::endl;
        std::cout  //
            << "  -t TIME_MAX: Specity the allowed maximum computational time "
               "for optimization calculation (specified in seconds). (default: "
            << option::GeneralOptionConstant::DEFAULT_TIME_MAX << ")"
            << std::endl;
        std::cout  //
            << "  -v VERBOSE: Specity the log level of standard output (Off, "
               "Warning, Outer, Inner, or Full). (default: "
            << option::verbose::VerboseInverseMap.at(
                   option::OutputOptionConstant::DEFAULT_VERBOSE)
            << ")" << std::endl;
        std::cout  //
            << "  --accept-continuous: Accept continuous variables as integer "
               "variables."
            << std::endl;
        std::cout  //
            << "  --extract-flippable-variable-pairs: Extract 2-flippable "
               "variable pairs."
            << std::endl;
        std::cout  //
            << "  --include-mps-loading-time: Include MPS file loading time in "
               "the calculation time. "
            << std::endl;
        std::cout  //
            << "  --export-json-instance: Export the target instance as JSON "
               "format."
            << std::endl;
        std::cout  //
            << "  --minimization (or --minimize, --min): Minimize the "
               "objective function value regardless of the settings in the MPS "
               "file."
            << std::endl;
        std::cout  //
            << "  --maximization (or --maximize, --max): Maximize the "
               "objective function value regardless of the settings in the MPS "
               "file."
            << std::endl;
    }

    /*************************************************************************/
    inline void parse(const int argc, const char *argv[]) {
        std::vector<std::string> args(argv, argv + argc);
        int                      i = 1;
        while (i < static_cast<int>(args.size())) {
            if (args[i] == "-p") {
                this->option_file_name = args[i + 1];
                i += 2;
            } else if (args[i] == "-i") {
                this->initial_solution_file_name = args[i + 1];
                i += 2;
            } else if (args[i] == "-m") {
                this->mutable_variable_file_name = args[i + 1];
                i += 2;
            } else if (args[i] == "-f") {
                this->fixed_variable_file_name = args[i + 1];
                i += 2;
            } else if (args[i] == "-s") {
                this->selection_constraint_file_name = args[i + 1];
                i += 2;
            } else if (args[i] == "-x") {
                this->flippable_variable_pair_file_name = args[i + 1];
                i += 2;
            } else if (args[i] == "-c") {
                this->minimum_common_element = atoi(args[i + 1].c_str());
                i += 2;
            } else if (args[i] == "-k") {
                this->iteration_max          = atof(args[i + 1].c_str());
                this->is_iteration_max_given = true;
                i += 2;
            } else if (args[i] == "-t") {
                this->time_max          = atof(args[i + 1].c_str());
                this->is_time_max_given = true;
                i += 2;
            } else if (args[i] == "-v") {
                this->verbose =
                    option::verbose::VerboseMap.at(args[i + 1].c_str());
                this->is_verbose_given = true;
                i += 2;
            } else if (args[i] == "--accept-continuous") {
                this->accept_continuous_variables = true;
                i++;
            } else if (args[i] == "--extract-flippable-variable-pairs") {
                this->extract_flippable_variable_pairs = true;
                i++;
            } else if (args[i] == "--include-mps-loading-time") {
                this->include_mps_loading_time = true;
                i++;
            } else if (args[i] == "--export-json-instance") {
                this->export_json_instance = true;
                i++;
            } else if (args[i] == "--minimization" || args[i] == "--minimize" ||
                       args[i] == "--min") {
                this->is_minimization_explicit = true;
                i++;
            } else if (args[i] == "--maximization" || args[i] == "--maximize" ||
                       args[i] == "--max") {
                this->is_maximization_explicit = true;
                i++;
            } else {
                this->mps_file_name = args[i];
                i++;
            }
        }
    }
};
}  // namespace printemps::standalone::mps_solver
#endif
/*****************************************************************************/
// END
/*****************************************************************************/