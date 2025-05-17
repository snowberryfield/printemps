/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEPB_STANDALONE_PB_SOLVER_PB_SOLVER_ARG_PARSER_H__
#define PRINTEPB_STANDALONE_PB_SOLVER_PB_SOLVER_ARG_PARSER_H__

namespace printemps::standalone::pb_solver {
/*****************************************************************************/
struct PBSolverArgparserConstant {
    static constexpr bool DEFAULT_INCLUDE_PB_LOADING_TIME  = false;
    static constexpr bool DEFAULT_EXPORT_JSON_INSTANCE     = false;
    static constexpr bool DEFAULT_IS_MINIMIZATION_EXPLICIT = false;
    static constexpr bool DEFAULT_IS_MAXIMIZATION_EXPLICIT = false;

    static constexpr bool DEFAULT_IS_ITERATION_MAX_GIVEN = false;
    static constexpr bool DEFAULT_IS_TIME_MAX_GIVEN      = false;
    static constexpr bool DEFAULT_IS_VERBOSE_GIVEN       = false;
};

/*****************************************************************************/
struct PBSolverArgparser {
    std::string pb_file_name;
    std::string option_file_name;
    std::string initial_solution_file_name;

    bool include_pb_loading_time;
    bool export_json_instance;
    bool is_minimization_explicit;
    bool is_maximization_explicit;

    double                   iteration_max;
    double                   time_max;
    option::verbose::Verbose verbose;
    bool                     is_iteration_max_given;
    bool                     is_time_max_given;
    bool                     is_verbose_given;

    /*************************************************************************/
    PBSolverArgparser(void) {
        this->initialize();
    }

    /*************************************************************************/
    inline void initialize(void) {
        this->pb_file_name.clear();
        this->option_file_name.clear();
        this->initial_solution_file_name.clear();
        this->include_pb_loading_time =
            PBSolverArgparserConstant::DEFAULT_INCLUDE_PB_LOADING_TIME;
        this->export_json_instance =
            PBSolverArgparserConstant::DEFAULT_EXPORT_JSON_INSTANCE;
        this->is_minimization_explicit =
            PBSolverArgparserConstant::DEFAULT_IS_MINIMIZATION_EXPLICIT;
        this->is_maximization_explicit =
            PBSolverArgparserConstant::DEFAULT_IS_MAXIMIZATION_EXPLICIT;

        this->iteration_max =
            option::GeneralOptionConstant::DEFAULT_ITERATION_MAX;
        this->time_max = option::GeneralOptionConstant::DEFAULT_TIME_MAX;
        this->verbose  = option::OutputOptionConstant::DEFAULT_VERBOSE;

        this->is_iteration_max_given =
            PBSolverArgparserConstant::DEFAULT_IS_ITERATION_MAX_GIVEN;
        this->is_time_max_given =
            PBSolverArgparserConstant::DEFAULT_IS_TIME_MAX_GIVEN;
        this->is_verbose_given =
            PBSolverArgparserConstant::DEFAULT_IS_VERBOSE_GIVEN;
    }

    /*************************************************************************/
    inline void print_usage(void) const {
        std::cout << std::endl;
        std::cout << "PRINTEMPS " + constant::VERSION + " (" +
                         constant::PROJECT_URL + ")"
                  << std::endl;
        std::cout << std::endl;

        std::cout << "Usage: ./pb_solver "
                  << "[-p OPTION_FILE_NAME] "
                  << "[-i INITIAL_SOLUTION_FILE_NAME] "
                  << "[-k ITERATION_MAX] "
                  << "[-t TIME_MAX] "
                  << "[-v VERVOSE] "
                  << "[--include-pb-loading-time] "
                  << "[--export-json-instance] "
                  << "[--minimization] "
                  << "[--maximization] "
                  << "pb_file" << std::endl;
        std::cout << std::endl;
        std::cout  //
            << "  -p OPTION_FILE_NAME: Specify option file name." << std::endl;
        std::cout  //
            << "  -i INITIAL_SOLUTION_FILE_NAME: Specify initial solution "
               "file name."
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
            << "  --include-pb-loading-time: Include PB file loading time in "
               "the calculation time. "
            << std::endl;
        std::cout  //
            << "  --export-json-instance: Export the target instance as JSON "
               "format."
            << std::endl;
        std::cout  //
            << "  --minimization (or --minimize, --min): Minimize the "
               "objective function value regardless of the settings in the PB "
               "file."
            << std::endl;
        std::cout  //
            << "  --maximization (or --maximize, --max): Maximize the "
               "objective function value regardless of the settings in the PB "
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
                this->pb_file_name = args[i];
                i++;
            }
        }
    }
};
}  // namespace printemps::standalone::pb_solver
#endif
/*****************************************************************************/
// END
/*****************************************************************************/