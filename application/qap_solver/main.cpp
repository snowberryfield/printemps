/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/

#include <string>
#include <iostream>

#include "../cppmh/utility/qap_utility.h"
#include "../cppmh/utility/option_utility.h"

int main([[maybe_unused]] int argc, char *argv[]) {
    /**
     * If no argument is specified, the usage is displayed and the program
     * exits.
     */
    if (argv[1] == nullptr) {
        std::cout << "Usage: ./aqp_solver.exe [OPTIONS] [MPS_FILE]"
                  << std::endl;
        std::cout << std::endl;
        std::cout  //
            << "  -p [OPTION_FILE]: Specifies option file." << std::endl;
        exit(1);
    }

    /**
     * Parse the arguments.
     */
    std::string qap_file_name;
    std::string option_file_name;

    std::vector<std::string> args(argv, argv + argc);
    int                      i = 1;
    while (i < static_cast<int>(args.size())) {
        if (args[i] == "-p") {
            option_file_name = args[i + 1];
            i += 2;
        } else {
            qap_file_name = args[i];
            i++;
        }
    }

    /**
     * Read the specified QAPLIB file and convert to the model.
     */
    cppmh::utility::QAPLIBReader qaplib_reader;
    auto &model = qaplib_reader.create_model_from_qaplib(qap_file_name);
    model.set_name(cppmh::utility::base_name(qap_file_name));

    /**
     * If the option file is given, the option values specified in the file will
     * be used for the calculation. Otherwise, the default values will be used.
     */
    cppmh::solver::Option option;
    option.verbose = cppmh::solver::Full;
    if (!option_file_name.empty()) {
        std::string option_file_name = argv[2];
        option = cppmh::utility::read_option(option_file_name);
    }

    /**
     * Run the solver.
     */
    auto result = cppmh::solver::solve(&model, option);

    /**
     * Print the result summary.
     */
    cppmh::utility::print_info(
        "status: " + std::to_string(result.solution.is_feasible()), true);

    cppmh::utility::print_info(
        "objective: " + std::to_string(result.solution.objective()), true);

    result.solution.write_json_by_name("incumbent.json");
    result.solution.write_solution("incumbent.sol");

    result.status.write_json_by_name("status.json");

    if (option.is_enabled_collect_historical_data) {
        result.history.write_feasible_solutions_json("feasible.json");
    }

    return 0;
}
/*****************************************************************************/
// END
/*****************************************************************************/
