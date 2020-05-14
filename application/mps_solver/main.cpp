/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/

#include <string>
#include <iostream>

#include "../cppmh/utility/mps_utility.h"
#include "../cppmh/utility/option_utility.h"

int main([[maybe_unused]] int argc, char *argv[]) {
    /**
     * If no argument is specified, the usage is displayed and the program
     * exits.
     */
    if (argv[1] == nullptr) {
        std::cout << "Usage: ./mps_solver.exe mps_file [option_file]"
                  << std::endl;
        exit(1);
    }

    /**
     * Read the specified MPS file and convert to the model.
     */
    std::string               mps_file_name = argv[1];
    cppmh::utility::MPSReader mps_reader;
    auto &model = mps_reader.create_model_from_mps(mps_file_name);

    /**
     * If the option file is given, the option values specified in the file will
     * be used for the calculation. Otherwise, the default values will be used.
     */
    cppmh::solver::Option option;
    option.verbose = cppmh::solver::Full;
    if (argv[2] != nullptr) {
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
        "status: " + std::to_string(result.is_feasible()), true);

    cppmh::utility::print_info(
        "objective: " + std::to_string(result.objective()), true);

    result.write_json_by_name("result.json");

    return 0;
}
/*****************************************************************************/
// END
/*****************************************************************************/
