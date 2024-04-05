/*****************************************************************************/
// Copyright (c) 2020-2024 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_STANDALONE_MPS_SOLVER_H__
#define PRINTEMPS_STANDALONE_MPS_SOLVER_H__

namespace printemps::standalone {
/*****************************************************************************/
struct MPSSolverConstant {
    static constexpr int  DEFAULT_MINIMUM_COMMON_ELEMENT           = 5;
    static constexpr bool DEFAULT_ACCEPT_CONTINUOUS_VARIABLES      = false;
    static constexpr bool DEFAULT_EXTRACT_FLIPPABLE_VARIABLE_PAIRS = false;
    static constexpr bool DEFAULT_INCLUDE_MPS_LOADING_TIME         = false;
    static constexpr bool DEFAULT_EXPORT_JSON_INSTANCE             = false;
};

/*****************************************************************************/
class MPSSolver {
   private:
    std::string m_mps_file_name;
    std::string m_option_file_name;
    std::string m_initial_solution_file_name;
    std::string m_mutable_variable_file_name;
    std::string m_fixed_variable_file_name;
    std::string m_selection_constraint_file_name;
    std::string m_flippable_variable_pair_file_name;
    int         m_minimum_common_element;
    bool        m_accept_continuous_variables;
    bool        m_extract_flippable_variable_pairs;
    bool        m_include_mps_loading_time;
    bool        m_export_json_instance;

    printemps::model::IPModel m_model;
    printemps::option::Option m_option;

    utility::TimeKeeper m_time_keeper;

    /*************************************************************************/
    inline void print_usage(void) const {
        std::cout << std::endl;
        std::cout << "PRINTEMPS " + constant::VERSION + " (" +
                         constant::PROJECT_URL + ")"
                  << std::endl;
        std::cout << std::endl;

        std::cout << "Usage: ./mps_solver.exe "
                  << "[-p OPTION_FILE_NAME] "
                  << "[-i INITIAL_SOLUTION_FILE_NAME] "
                  << "[-m MUTABLE_VARIABLE_FILE_NAME] "
                  << "[-f FIXED_VARIABLE_FILE_NAME] "
                  << "[-s SELECTION_CONSTRAINT_FILE_NAME] "
                  << "[-x FLIPPABLE_VARIABLE_PAIR_FILE_NAME]"
                  << "[-c MINIMUM_COMMON_ELEMENT] "
                  << "[--accept-continuous] "
                  << "[--extract-flippable-variable-pairs] "
                  << "[--include-mps-loading-time] "
                  << "[--export-json-instance] "
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
            << MPSSolverConstant::DEFAULT_MINIMUM_COMMON_ELEMENT << ")"
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
            << "  --include-mps-loading-time: Include MPS file loading time in "
               "the calculation time. "
            << std::endl;
        std::cout  //
            << "  --export-json-instance: Export the target instance as JSON "
               "format."
            << std::endl;
    }

   public:
    /*************************************************************************/
    MPSSolver(void) {
        this->initialize();
    }

    /*************************************************************************/
    MPSSolver(int argc, char *argv[]) {
        this->initialize();
        this->setup(argc, argv);
    }

    /*************************************************************************/
    inline void initialize(void) {
        m_mps_file_name.clear();
        m_option_file_name.clear();
        m_initial_solution_file_name.clear();
        m_mutable_variable_file_name.clear();
        m_fixed_variable_file_name.clear();
        m_selection_constraint_file_name.clear();
        m_flippable_variable_pair_file_name.clear();
        m_minimum_common_element =
            MPSSolverConstant::DEFAULT_MINIMUM_COMMON_ELEMENT;
        m_accept_continuous_variables =
            MPSSolverConstant::DEFAULT_ACCEPT_CONTINUOUS_VARIABLES;
        m_extract_flippable_variable_pairs =
            MPSSolverConstant::DEFAULT_EXTRACT_FLIPPABLE_VARIABLE_PAIRS;
        m_include_mps_loading_time =
            MPSSolverConstant::DEFAULT_INCLUDE_MPS_LOADING_TIME;
        m_export_json_instance =
            MPSSolverConstant::DEFAULT_EXPORT_JSON_INSTANCE;

        m_model.initialize();
        m_option.initialize();
    }

    /*************************************************************************/
    inline void parse_command_line_arguments(int argc, char *argv[]) {
        std::vector<std::string> args(argv, argv + argc);
        int                      i = 1;
        while (i < static_cast<int>(args.size())) {
            if (args[i] == "-p") {
                m_option_file_name = args[i + 1];
                i += 2;
            } else if (args[i] == "-i") {
                m_initial_solution_file_name = args[i + 1];
                i += 2;
            } else if (args[i] == "-m") {
                m_mutable_variable_file_name = args[i + 1];
                i += 2;
            } else if (args[i] == "-f") {
                m_fixed_variable_file_name = args[i + 1];
                i += 2;
            } else if (args[i] == "-s") {
                m_selection_constraint_file_name = args[i + 1];
                i += 2;
            } else if (args[i] == "-x") {
                m_flippable_variable_pair_file_name = args[i + 1];
                i += 2;
            } else if (args[i] == "-c") {
                m_minimum_common_element = atoi(args[i + 1].c_str());
                i += 2;
            } else if (args[i] == "--accept-continuous") {
                m_accept_continuous_variables = true;
                i++;
            } else if (args[i] == "--extract-flippable-variable-pairs") {
                m_extract_flippable_variable_pairs = true;
                i++;
            } else if (args[i] == "--include-mps-loading-time") {
                m_include_mps_loading_time = true;
                i++;
            } else if (args[i] == "--export-json-instance") {
                m_export_json_instance = true;
                i++;
            } else {
                m_mps_file_name = args[i];
                i++;
            }
        }
    }

    /*************************************************************************/
    inline void setup(int argc, char *argv[]) {
        if (argv[1] == nullptr) {
            /**
             * If no argument is specified, the usage is displayed and the
             * program exits.
             */
            this->print_usage();
            exit(1);
        }

        m_time_keeper.set_start_time();

        /**
         * Parse the command line arguments.
         */
        this->parse_command_line_arguments(argc, argv);

        /**
         * Only one of Mutable variable file list and fixed variable file list
         * can be specified.
         */
        if (!m_mutable_variable_file_name.empty() &&
            !m_fixed_variable_file_name.empty()) {
            throw std::logic_error(printemps::utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The flags -m and -v cannot be used simultaneously."));
        }

        /**
         * Read the specified MPS file and convert to the model.
         */
        {
            printemps::mps::MPS mps(m_mps_file_name);
            m_model.import_mps(mps, m_accept_continuous_variables);
            m_model.set_name(printemps::utility::base_name(m_mps_file_name));
        }

        /**
         * If the option file is given, the option values specified in the file
         * will be used for the calculation. Otherwise, the default values will
         * be used.
         */
        if (!m_option_file_name.empty()) {
            m_option.setup(m_option_file_name);
        }

        /**
         * If the mutable variable file is given, only the variables listed in
         * the file can be changed.
         */
        if (!m_mutable_variable_file_name.empty()) {
            const auto MUTABLE_VARIABLE_NAMES =
                printemps::helper::read_names(m_mutable_variable_file_name);
            m_model.unfix_variables(MUTABLE_VARIABLE_NAMES);
        }

        /**
         * If the fixed variable file is given, the values of the variables will
         * be fixed at the specified values.
         */
        if (!m_fixed_variable_file_name.empty()) {
            const auto FIXED_VARIABLES_AND_VALUES =
                printemps::helper::read_names_and_values(
                    m_fixed_variable_file_name);
            m_model.fix_variables(FIXED_VARIABLES_AND_VALUES);
        }

        /**
         * If the selection constraint file is given, the constraints listed in
         * the file will be regarded as user-defined selection constraints.
         */
        if (!m_selection_constraint_file_name.empty()) {
            const auto SELECTION_CONSTRAINT_NAMES =
                printemps::helper::read_names(m_selection_constraint_file_name);
            m_model.set_user_defined_selection_constraints(
                SELECTION_CONSTRAINT_NAMES);
        }

        /**
         * If the flippable variable pair file is given, register 2-flip moves
         * and activate two-flip neighborhood moves.
         */
        if (!m_flippable_variable_pair_file_name.empty()) {
            const auto VARIABLE_NAME_PAIRS = printemps::helper::read_name_pairs(
                m_flippable_variable_pair_file_name);
            m_option.neighborhood.is_enabled_two_flip_move = true;
            m_model.setup_flippable_variable_ptr_pairs(VARIABLE_NAME_PAIRS);
        }

        /**
         * If the initial solution file is given, the values of the variables in
         * the file will be used as the initial values. Otherwise, the default
         * values will be used.
         */
        if (!m_initial_solution_file_name.empty()) {
            const auto INITIAL_SOLUTION =
                printemps::helper::read_names_and_values(
                    m_initial_solution_file_name);
            m_model.import_solution(INITIAL_SOLUTION);
        }
    }

    /*************************************************************************/
    inline void solve(void) {
        /**
         * Run the solver.
         */

        printemps::solver::IPSolver solver;

        if (m_include_mps_loading_time) {
            solver.setup(&m_model, m_option, m_time_keeper);
        } else {
            solver.setup(&m_model, m_option);
        }
        auto result = solver.solve();

        /**
         * Print the result summary.
         */
        printemps::utility::print_info(
            "status: " + std::to_string(result.solution.is_feasible()),
            m_option.output.verbose >= printemps::option::verbose::Warning);

        printemps::utility::print_info(
            "objective: " + std::to_string(result.solution.objective()),
            m_option.output.verbose >= printemps::option::verbose::Warning);

        printemps::utility::print_info(
            "total violation: " +
                std::to_string(result.solution.total_violation()),
            m_option.output.verbose >= printemps::option::verbose::Warning);

        result.solution.write_json_by_name("incumbent.json");
        result.solution.write_solution("incumbent.sol");
        result.status.write_json_by_name("status.json");

        if (m_option.output.is_enabled_store_feasible_solutions) {
            result.feasible_solution_archive.write_solutions_json(
                "feasible.json");
        }

        if (m_export_json_instance) {
            m_model.write_json(m_model.name() + ".json");
        }
    }

    /*************************************************************************/
    inline void extract_flippable_variable_pairs(void) {
        /**
         * Extract flippable variable pairs.
         */
        printemps::solver::IPSolver solver(&m_model, m_option);
        solver.preprocess();

        printemps::preprocess::IPFlippableVariablePairExtractor extractor(
            solver.model_ptr());
        extractor.extract_pairs(
            m_minimum_common_element,
            m_option.output.verbose >= printemps::option::verbose::Outer);
        extractor.write_pairs("flip.txt");
    }

    /*************************************************************************/
    inline void run(void) {
        if (!m_extract_flippable_variable_pairs) {
            this->solve();
        } else {
            this->extract_flippable_variable_pairs();
        }
    }
};
}  // namespace printemps::standalone
#endif
/*****************************************************************************/
// END
/*****************************************************************************/