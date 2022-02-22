/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/

#define _MPS_SOLVER

#include <string>
#include <iostream>

#include "../printemps/utility/option_utility.h"

int main([[maybe_unused]] int argc, char *argv[]) {
    /**
     * If no argument is specified, the usage is displayed and the program
     * exits.
     */
    if (argv[1] == nullptr) {
        std::cout << "Usage: ./mps_solver.exe "
                  << "[-p OPTION_FILE_NAME] "
                  << "[-i INITIAL_SOLUTION_FILE_NAME] "
                  << "[-m MUTABLE_VARIABLE_FILE_NAME] "
                  << "[-f FIXED_VARIABLE_FILE_NAME] "
                  << "[-x FLIPPABLE_VARIABLE_PAIR_FILE_NAME]"
                  << "[--accept-continuous] "
                  << "[--extract-flippable-variable-pairs] "
                  << "[-s MINIMUM_COMMON_ELEMENT] "
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
            << "  -x FLIP_VARIABLE_PAIR_FILE_NAME: Specify flippable variable "
               "pair file name."
            << std::endl;
        std::cout  //
            << "  --accept-continuous: Accept continuous variables as integer "
               "variables."
            << std::endl;
        std::cout  //
            << "  --extract-flippable-variable-pairs: Extract 2-flippable "
               "variable pairs."
            << std::endl;
        std::cout  //
            << "  --s MINIMUM_COMMON_ELEMENT: Specify the number of minimum "
               "common element between two constraints, which is used as the "
               "threshold for extracting flippable variable pairs. (default: 5)"
            << std::endl;
        exit(1);
    }

    /**
     * Parse the arguments.
     */
    std::string mps_file_name;
    std::string option_file_name;
    std::string initial_solution_file_name;
    std::string mutable_variable_file_name;
    std::string fixed_variable_file_name;
    std::string flippable_variable_pair_file_name;
    bool        accept_continuous_variables      = false;
    bool        extract_flippable_variable_pairs = false;
    int         minimum_common_element           = 5;

    std::vector<std::string> args(argv, argv + argc);
    int                      i = 1;
    while (i < static_cast<int>(args.size())) {
        if (args[i] == "-p") {
            option_file_name = args[i + 1];
            i += 2;
        } else if (args[i] == "-i") {
            initial_solution_file_name = args[i + 1];
            i += 2;
        } else if (args[i] == "-m") {
            mutable_variable_file_name = args[i + 1];
            i += 2;
        } else if (args[i] == "-f") {
            fixed_variable_file_name = args[i + 1];
            i += 2;
        } else if (args[i] == "-x") {
            flippable_variable_pair_file_name = args[i + 1];
            i += 2;
        } else if (args[i] == "-s") {
            minimum_common_element = atoi(args[i + 1].c_str());
            i += 2;
        } else if (args[i] == "--accept-continuous") {
            accept_continuous_variables = true;
            i++;
        } else if (args[i] == "--extract-flippable-variable-pairs") {
            extract_flippable_variable_pairs = true;
            i++;
        } else {
            mps_file_name = args[i];
            i++;
        }
    }

    if (!mutable_variable_file_name.empty() &&
        !fixed_variable_file_name.empty()) {
        throw std::logic_error(printemps::utility::format_error_location(
            __FILE__, __LINE__, __func__,
            "The flags -m and -v cannot be used simultaneously."));
    }

    /**
     * Read the specified MPS file and convert to the model.
     */
    printemps::model::IPModel model;
    {
        printemps::mps::MPS mps(mps_file_name);
        model.import_mps(mps, accept_continuous_variables);
    }

    model.set_name(printemps::utility::base_name(mps_file_name));

    /**
     * If the option file is given, the option values specified in the file will
     * be used for the calculation. Otherwise, the default values will be used.
     */
    printemps::option::Option option;
    if (!option_file_name.empty()) {
        option = printemps::utility::read_option(option_file_name);
    }

    /**
     * If the mutable variable file is given, only the variables listed in the
     * file can be changed.
     */
    if (!mutable_variable_file_name.empty()) {
        auto mutable_variable_names =
            printemps::helper::read_variable_names(mutable_variable_file_name);
        model.unfix_variables(mutable_variable_names);
    }

    /**
     * If the fixe variable file is given, the values of the variables will be
     * fixed at the specified values.
     */
    if (!fixed_variable_file_name.empty()) {
        auto solution = printemps::helper::read_variable_names_and_values(
            fixed_variable_file_name);
        model.fix_variables(solution);
    }

    /**
     * If the flippable variable pair file is given, register 2-flip moves as
     * user-defined neighborhood moves.
     */
    if (!flippable_variable_pair_file_name.empty()) {
        auto variable_name_pairs = printemps::helper::read_variable_name_pairs(
            flippable_variable_pair_file_name);
        option.is_enabled_user_defined_move = true;
        model.setup_flippable_variable_ptr_pairs(variable_name_pairs);
    }

    /**
     * If the initial solution file is given, the values of the variables in the
     * file will be used as the initial values. Otherwise, the default values
     * will be used.
     */
    if (!initial_solution_file_name.empty()) {
        auto solution = printemps::helper::read_variable_names_and_values(
            initial_solution_file_name);
        model.import_solution(solution);
    }

    if (!extract_flippable_variable_pairs) {
        /**
         * Run the solver.
         */
        printemps::solver::IPSolver solver(&model, option);
        auto                        result = solver.solve();

        /**
         * Print the result summary.
         */
        printemps::utility::print_info(
            "status: " + std::to_string(result.solution.is_feasible()),
            option.verbose >= printemps::option::verbose::Warning);

        printemps::utility::print_info(
            "objective: " + std::to_string(result.solution.objective()),
            option.verbose >= printemps::option::verbose::Warning);

        printemps::utility::print_info(
            "total violation: " +
                std::to_string(result.solution.total_violation()),
            option.verbose >= printemps::option::verbose::Warning);

        result.solution.write_json_by_name("incumbent.json");
        result.solution.write_solution("incumbent.sol");
        result.status.write_json_by_name("status.json");

        if (option.is_enabled_store_feasible_solutions) {
            result.solution_archive.write_solutions_json("feasible.json");
        }
    } else {
        /**
         * Extract flippable variable pairs.
         */
        printemps::solver::IPSolver solver(&model, option);
        solver.preprocess();

        printemps::presolver::IPFlippableVariablePairExtractor extractor(
            solver.model_ptr());
        extractor.extract_pairs(
            minimum_common_element,
            option.verbose >= printemps::option::verbose::Outer);
        extractor.write_pairs("flip.txt");
    }

    return 0;
}
/*****************************************************************************/
// END
/*****************************************************************************/
