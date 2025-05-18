/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_STANDALONE_OPB_SOLVER_OPB_SOLVER_ARGPARSER_H__
#define PRINTEMPS_STANDALONE_OPB_SOLVER_OPB_SOLVER_ARGPARSER_H__

namespace printemps::standalone::opb_solver {
/*****************************************************************************/
struct OPBSolverArgparserConstant {
    static constexpr int  DEFAULT_MINIMUM_COMMON_ELEMENT           = 5;
    static constexpr bool DEFAULT_EXTRACT_FLIPPABLE_VARIABLE_PAIRS = false;
    static constexpr bool DEFAULT_INCLUDE_OPB_LOADING_TIME         = false;
    static constexpr bool DEFAULT_EXPORT_JSON_INSTANCE             = false;
    static constexpr bool DEFAULT_IS_MINIMIZATION_EXPLICIT         = false;
    static constexpr bool DEFAULT_IS_MAXIMIZATION_EXPLICIT         = false;
    static constexpr int  DEFAULT_NUMBER_OF_THREADS                = -1;
};

/*****************************************************************************/
struct OPBSolverArgparser {
    std::string opb_file_name;
    std::string option_file_name;
    std::string initial_solution_file_name;
    std::string mutable_variable_file_name;
    std::string fixed_variable_file_name;
    std::string selection_constraint_file_name;
    std::string flippable_variable_pair_file_name;

    int  minimum_common_element;
    bool extract_flippable_variable_pairs;
    bool include_opb_loading_time;
    bool export_json_instance;
    bool is_minimization_explicit;
    bool is_maximization_explicit;

    double                   iteration_max;
    double                   time_max;
    option::verbose::Verbose verbose;
    int                      number_of_threads;
    int                      seed;

    bool is_specified_iteration_max;
    bool is_specified_time_max;
    bool is_specified_verbose;
    bool is_specified_number_of_threads;
    bool is_specified_seed;

    /*************************************************************************/
    OPBSolverArgparser(void) {
        this->initialize();
    }

    /*************************************************************************/
    inline void initialize(void) {
        this->opb_file_name.clear();
        this->option_file_name.clear();
        this->initial_solution_file_name.clear();
        this->mutable_variable_file_name.clear();
        this->fixed_variable_file_name.clear();
        this->selection_constraint_file_name.clear();
        this->flippable_variable_pair_file_name.clear();
        this->minimum_common_element =
            OPBSolverArgparserConstant::DEFAULT_MINIMUM_COMMON_ELEMENT;
        this->extract_flippable_variable_pairs = OPBSolverArgparserConstant::
            DEFAULT_EXTRACT_FLIPPABLE_VARIABLE_PAIRS;
        this->include_opb_loading_time =
            OPBSolverArgparserConstant::DEFAULT_INCLUDE_OPB_LOADING_TIME;
        this->export_json_instance =
            OPBSolverArgparserConstant::DEFAULT_EXPORT_JSON_INSTANCE;
        this->is_minimization_explicit =
            OPBSolverArgparserConstant::DEFAULT_IS_MINIMIZATION_EXPLICIT;
        this->is_maximization_explicit =
            OPBSolverArgparserConstant::DEFAULT_IS_MAXIMIZATION_EXPLICIT;

        this->iteration_max =
            option::GeneralOptionConstant::DEFAULT_ITERATION_MAX;
        this->time_max = option::GeneralOptionConstant::DEFAULT_TIME_MAX;
        this->verbose  = option::OutputOptionConstant::DEFAULT_VERBOSE;
        this->number_of_threads =
            OPBSolverArgparserConstant::DEFAULT_NUMBER_OF_THREADS;
        this->seed = option::GeneralOptionConstant::DEFAULT_SEED;

        this->is_specified_iteration_max     = false;
        this->is_specified_time_max          = false;
        this->is_specified_verbose           = false;
        this->is_specified_number_of_threads = false;
        this->is_specified_seed              = false;
    }

    /*************************************************************************/
    inline void print_usage(void) const {
        std::cout << std::endl;
        std::cout << "PRINTEMPS " + constant::VERSION + " (" +
                         constant::PROJECT_URL + ")"
                  << std::endl;
        std::cout << std::endl;

        std::cout << "Usage: ./opb_solver "
                  << "[-p OPTION_FILE_NAME] "
                  << "[-i INITIAL_SOLUTION_FILE_NAME] "
                  << "[-m MUTABLE_VARIABLE_FILE_NAME] "
                  << "[-f FIXED_VARIABLE_FILE_NAME] "
                  << "[-s SELECTION_CONSTRAINT_FILE_NAME] "
                  << "[-x FLIPPABLE_VARIABLE_PAIR_FILE_NAME] "
                  << "[-c MINIMUM_COMMON_ELEMENT] "
                  << "[-k ITERATION_MAX] "
                  << "[-t TIME_MAX] "
                  << "[-v VERVOSE] "
                  << "[-j NUMBER_OF_THREADS] "
                  << "[-r SEED] "
                  << "[--extract-flippable-variable-pairs] "
                  << "[--include-opb-loading-time] "
                  << "[--export-json-instance] "
                  << "[--minimization] "
                  << "[--maximization] "
                  << "opb_file" << std::endl;
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
            << OPBSolverArgparserConstant::DEFAULT_MINIMUM_COMMON_ELEMENT << ")"
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
            << "  -j NUMBER_OF_THREADS: Specity the number of threads for "
               "parallelization. (default: "
            << OPBSolverArgparserConstant::DEFAULT_NUMBER_OF_THREADS
            << ", maximum value avaiable)" << std::endl;
        std::cout  //
            << "  -r SEED: Specity the random seed. (default: "
            << option::GeneralOptionConstant::DEFAULT_SEED << ")" << std::endl;
        std::cout  //
            << "  --extract-flippable-variable-pairs: Extract 2-flippable "
               "variable pairs."
            << std::endl;
        std::cout  //
            << "  --include-opb-loading-time: Include OPB file loading time "
               "in the calculation time. "
            << std::endl;
        std::cout  //
            << "  --export-json-instance: Export the target instance as JSON "
               "format."
            << std::endl;
        std::cout  //
            << "  --minimization (or --minimize, --min): Minimize the "
               "objective function value regardless of the settings in the OPB "
               "file."
            << std::endl;
        std::cout  //
            << "  --maximization (or --maximize, --max): Maximize the "
               "objective function value regardless of the settings in the OPB "
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
                this->iteration_max              = atof(args[i + 1].c_str());
                this->is_specified_iteration_max = true;
                i += 2;
            } else if (args[i] == "-t") {
                this->time_max              = atof(args[i + 1].c_str());
                this->is_specified_time_max = true;
                i += 2;
            } else if (args[i] == "-v") {
                this->verbose =
                    option::verbose::VerboseMap.at(args[i + 1].c_str());
                this->is_specified_verbose = true;
                i += 2;
            } else if (args[i] == "-j") {
                this->number_of_threads = atof(args[i + 1].c_str());
                this->is_specified_number_of_threads = true;
                i += 2;
            } else if (args[i] == "-r") {
                this->seed = static_cast<int32_t>(
                    static_cast<uint32_t>(atol(args[i + 1].c_str())));
                this->is_specified_seed = true;
                i += 2;
            } else if (args[i] == "--extract-flippable-variable-pairs") {
                this->extract_flippable_variable_pairs = true;
                i++;
            } else if (args[i] == "--include-opb-loading-time") {
                this->include_opb_loading_time = true;
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
                this->opb_file_name = args[i];
                i++;
            }
        }
    }
};
}  // namespace printemps::standalone::opb_solver
#endif
/*****************************************************************************/
// END
/*****************************************************************************/