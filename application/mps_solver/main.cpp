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
        std::cout << "Usage: ./mps_solver.exe mps_file [option_file] "
                     "[initial_solution_file]"
                  << std::endl;
        exit(1);
    }

    /**
     * Read the specified MPS file and convert to the model.
     */
    std::string               mps_file_name = argv[1];
    cppmh::utility::MPSReader mps_reader;
    auto &model = mps_reader.create_model_from_mps(mps_file_name);
    model.set_name(cppmh::utility::base_name(mps_file_name));

    /**
     * If the option file is given, the option values specified in the file will
     * be used for the calculation. Otherwise, the default values will be used.
     */
    cppmh::solver::Option option;
    option.verbose = cppmh::solver::Full;
    if (argc > 2) {
        std::string option_file_name = argv[2];
        option = cppmh::utility::read_option(option_file_name);
    }

    /**
     * If the solution file is given, the values of the decision variables in
     * the file will be used as the initial values. Otherwise, the default
     * values will be used.
     */
    if (argc > 3) {
        std::string solution_file_name = argv[3];
        auto solution = cppmh::utility::read_solution(solution_file_name);
        model.import_solution(solution);
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
