/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_EXTRA_PB_COMPETITION_2025_PB_COMPETITION_2025_SOLVER_ARGPARSER_H__
#define PRINTEMPS_EXTRA_PB_COMPETITION_2025_PB_COMPETITION_2025_SOLVER_ARGPARSER_H__

namespace printemps::extra::pb_competition_2025 {
/*****************************************************************************/
struct PBCompetition2025SolverArgparserConstant {
    static constexpr bool DEFAULT_INCLUDE_PB_LOADING_TIME = false;
    static constexpr bool DEFAULT_IS_ITERATION_MAX_GIVEN  = false;
    static constexpr bool DEFAULT_IS_TIME_MAX_GIVEN       = false;
    static constexpr bool DEFAULT_IS_VERBOSE_GIVEN        = false;
};

/*****************************************************************************/
struct PBCompetition2025SolverArgparser {
    std::string pb_file_name;
    std::string option_file_name;

    bool include_pb_loading_time;

    double                   iteration_max;
    double                   time_max;
    option::verbose::Verbose verbose;
    bool                     is_iteration_max_given;
    bool                     is_time_max_given;
    bool                     is_verbose_given;

    /*************************************************************************/
    PBCompetition2025SolverArgparser(void) {
        this->initialize();
    }

    /*************************************************************************/
    inline void initialize(void) {
        this->pb_file_name.clear();
        this->option_file_name.clear();
        this->include_pb_loading_time =
            PBCompetition2025SolverArgparserConstant::
                DEFAULT_INCLUDE_PB_LOADING_TIME;

        this->iteration_max =
            option::GeneralOptionConstant::DEFAULT_ITERATION_MAX;
        this->time_max = option::GeneralOptionConstant::DEFAULT_TIME_MAX;
        this->verbose  = option::OutputOptionConstant::DEFAULT_VERBOSE;

        this->is_iteration_max_given =
            PBCompetition2025SolverArgparserConstant::
                DEFAULT_IS_ITERATION_MAX_GIVEN;
        this->is_time_max_given =
            PBCompetition2025SolverArgparserConstant::DEFAULT_IS_TIME_MAX_GIVEN;
        this->is_verbose_given =
            PBCompetition2025SolverArgparserConstant::DEFAULT_IS_VERBOSE_GIVEN;
    }

    /*************************************************************************/
    inline void print_usage(void) const {
        std::cout << std::endl;
        std::cout << "PRINTEMPS " + constant::VERSION + " (" +
                         constant::PROJECT_URL + ")"
                  << std::endl;
        std::cout << std::endl;

        std::cout << "Usage: ./pb_competition_2025_solver "
                  << "[-p OPTION_FILE_NAME] "
                  << "[-k ITERATION_MAX] "
                  << "[-t TIME_MAX] "
                  << "[-v VERVOSE] "
                  << "pb_file" << std::endl;
        std::cout << std::endl;
        std::cout  //
            << "  -p OPTION_FILE_NAME: Specify option file name." << std::endl;
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
    }

    /*************************************************************************/
    inline void parse(const int argc, const char *argv[]) {
        std::vector<std::string> args(argv, argv + argc);
        int                      i = 1;
        while (i < static_cast<int>(args.size())) {
            if (args[i] == "-p") {
                this->option_file_name = args[i + 1];
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
            } else {
                this->pb_file_name = args[i];
                i++;
            }
        }
    }
};
}  // namespace printemps::extra::pb_competition_2025
#endif
/*****************************************************************************/
// END
/*****************************************************************************/