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
    static constexpr bool DEFAULT_INCLUDE_OPB_LOADING_TIME = false;
    static constexpr int  DEFAULT_NUMBER_OF_THREADS        = -1;
};

/*****************************************************************************/
struct PBCompetition2025SolverArgparser {
    std::string pb_file_name;

    double iteration_max;
    double time_max;
    int    number_of_threads;
    int    seed;

    bool is_specified_iteration_max;
    bool is_specified_time_max;
    bool is_specified_number_of_threads;
    bool is_specified_seed;

    /*************************************************************************/
    PBCompetition2025SolverArgparser(void) {
        this->initialize();
    }

    /*************************************************************************/
    inline void initialize(void) {
        this->pb_file_name.clear();

        this->iteration_max =
            option::GeneralOptionConstant::DEFAULT_ITERATION_MAX;
        this->time_max = option::GeneralOptionConstant::DEFAULT_TIME_MAX;
        this->number_of_threads =
            PBCompetition2025SolverArgparserConstant::DEFAULT_NUMBER_OF_THREADS;
        this->seed = option::GeneralOptionConstant::DEFAULT_SEED;

        this->is_specified_iteration_max     = false;
        this->is_specified_time_max          = false;
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

        std::cout << "Usage: ./pb_competition_2025_solver "
                  << "[-k ITERATION_MAX] "
                  << "[-t TIME_MAX] "
                  << "[-j NUMBER_OF_THREADS] "
                  << "[-r SEED] "
                  << "opb_file" << std::endl;
        std::cout << std::endl;
        std::cout  //
            << "  -k ITERATION_MAX: Specify the allowed maximum number of "
               "outer loop iterations. (default: "
            << option::GeneralOptionConstant::DEFAULT_ITERATION_MAX
            << "; negative value indicates no limit)" << std::endl;
        std::cout  //
            << "  -t TIME_MAX: Specify the allowed maximum computational "
               "time for optimization calculation (specified in seconds). "
               "(default: "
            << option::GeneralOptionConstant::DEFAULT_TIME_MAX
            << "; negative value indicates no limit)" << std::endl;
        std::cout  //
            << "  -j NUMBER_OF_THREADS: Specify the number of threads for "
               "parallelization. (default: "
            << PBCompetition2025SolverArgparserConstant::
                   DEFAULT_NUMBER_OF_THREADS
            << ", maximum value avaiable)" << std::endl;
        std::cout  //
            << "  -r SEED: Specify the random seed. (default: "
            << option::GeneralOptionConstant::DEFAULT_SEED << ")" << std::endl;
    }

    /*************************************************************************/
    inline void parse(const int argc, const char *argv[]) {
        std::vector<std::string> args(argv, argv + argc);
        int                      i = 1;
        while (i < static_cast<int>(args.size())) {
            if (args[i] == "-k") {
                this->iteration_max              = std::stoi(args[i + 1]);
                this->is_specified_iteration_max = true;
                i += 2;
            } else if (args[i] == "-t") {
                this->time_max              = std::stod(args[i + 1]);
                this->is_specified_time_max = true;
                i += 2;
            } else if (args[i] == "-j") {
                this->number_of_threads              = std::stoi(args[i + 1]);
                this->is_specified_number_of_threads = true;
                i += 2;
            } else if (args[i] == "-r") {
                this->seed = static_cast<int32_t>(
                    static_cast<uint32_t>(std::stol(args[i + 1])));
                this->is_specified_seed = true;
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