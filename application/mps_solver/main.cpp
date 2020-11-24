/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/

#include <string>
#include <iostream>

#include "../printemps/utility/mps_utility.h"
#include "../printemps/utility/option_utility.h"

int main([[maybe_unused]] int argc, char *argv[]) {
    /**
     * If no argument is specified, the usage is displayed and the program
     * exits.
     */
    if (argv[1] == nullptr) {
        std::cout << "Usage: ./mps_solver.exe [-p OPTION_FILE_NAME] [-i "
                     "INITIAL_SOLUTION_FILE_NAME] [--separate] mps_file"
                  << std::endl;
        std::cout << std::endl;
        std::cout  //
            << "  -p OPTION_FILE_NAME: Specify option file name." << std::endl;
        std::cout  //
            << "  -i INITIAL_SOLUTION_FILE_NAME: Specify initial solution "
               "file name."
            << std::endl;
        std::cout  //
            << "  --separate: Separate equality constraints into lower "
               "and upper constraints."
            << std::endl;
        exit(1);
    }

    /**
     * Parse the arguments.
     */
    std::string mps_file_name;
    std::string option_file_name;
    std::string initial_solution_file_name;
    bool        is_enabled_separate_equality = false;

    std::vector<std::string> args(argv, argv + argc);
    int                      i = 1;
    while (i < static_cast<int>(args.size())) {
        if (args[i] == "-p") {
            option_file_name = args[i + 1];
            i += 2;
        } else if (args[i] == "-i") {
            initial_solution_file_name = args[i + 1];
            i += 2;
        } else if (args[i] == "--separate") {
            is_enabled_separate_equality = true;
            i++;
        } else {
            mps_file_name = args[i];
            i++;
        }
    }

    /**
     * Read the specified MPS file and convert to the model.
     */
    printemps::utility::MPSReader mps_reader;

    auto &model = mps_reader.create_model_from_mps(
        mps_file_name, is_enabled_separate_equality);
    model.set_name(printemps::utility::base_name(mps_file_name));

    /**
     * If the option file is given, the option values specified in the file will
     * be used for the calculation. Otherwise, the default values will be used.
     */
    printemps::solver::Option option;
    if (!option_file_name.empty()) {
        option = printemps::utility::read_option(option_file_name);
    }

    /**
     * If the initial solution file is given, the values of the decision
     * variables in the file will be used as the initial values. Otherwise, the
     * default values will be used.
     */
    if (!initial_solution_file_name.empty()) {
        auto solution =
            printemps::utility::read_solution(initial_solution_file_name);
        model.import_solution(solution);
    }

    /**
     * Run the solver.
     */
    auto result = printemps::solver::solve(&model, option);

    /**
     * Print the result summary.
     */
    printemps::utility::print_info(
        "status: " + std::to_string(result.solution.is_feasible()),
        option.verbose != printemps::solver::Verbose::None);

    printemps::utility::print_info(
        "objective: " + std::to_string(result.solution.objective()),
        option.verbose != printemps::solver::Verbose::None);

    printemps::utility::print_info(
        "total violation: " + std::to_string(result.solution.total_violation()),
        option.verbose != printemps::solver::Verbose::None);

    auto summary = model.export_summary();
    result.solution.write_json_by_name("incumbent.json", summary);
    result.solution.write_solution("incumbent.sol");
    result.status.write_json_by_name("status.json", summary);

    if (option.is_enabled_collect_historical_data) {
        result.solution_archive.write_solutions_json("feasible.json", summary);
    }

    return 0;
}
/*****************************************************************************/
// END
/*****************************************************************************/
