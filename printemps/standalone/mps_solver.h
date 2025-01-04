/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_STANDALONE_MPS_SOLVER_H__
#define PRINTEMPS_STANDALONE_MPS_SOLVER_H__

#include "argparser.h"

namespace printemps::standalone {
/*****************************************************************************/
class MPSSolver {
   private:
    Argparser                 m_argparser;
    model::IPModel            m_model;
    printemps::option::Option m_option;
    utility::TimeKeeper       m_time_keeper;

   public:
    /*************************************************************************/
    MPSSolver(void) {
        this->initialize();
    }

    /*************************************************************************/
    MPSSolver(const int argc, const char *argv[]) {
        this->initialize();
        this->setup(argc, argv);
    }

    /*************************************************************************/
    inline void initialize(void) {
        m_argparser.initialize();
        m_model.initialize();
        m_option.initialize();
    }

    /*************************************************************************/
    inline void setup(const int argc, const char *argv[]) {
        if (argv[1] == nullptr) {
            /**
             * If no argument is specified, the usage is displayed and the
             * program exits.
             */
            m_argparser.print_usage();
            exit(1);
        }

        m_time_keeper.set_start_time();

        /**
         * Parse the command line arguments.
         */
        m_argparser.parse(argc, argv);

        /**
         * Only one of Mutable variable file list and fixed variable file list
         * can be specified.
         */
        if (!m_argparser.mutable_variable_file_name.empty() &&
            !m_argparser.fixed_variable_file_name.empty()) {
            throw std::runtime_error(printemps::utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The flags -m and -v cannot be used simultaneously."));
        }

        /**
         * Minimization and maximization cannot be specified at the same time.
         */
        if (m_argparser.is_minimization_explicit &&
            m_argparser.is_maximization_explicit) {
            throw std::runtime_error(printemps::utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The flags --minimization and --maximization cannot be used "
                "simultaneously."));
        }

        /**
         * Read the specified MPS file and convert to the model.
         */
        {
            printemps::mps::MPS mps(m_argparser.mps_file_name);
            m_model.import_mps(mps, m_argparser.accept_continuous_variables);
            m_model.set_name(
                printemps::utility::base_name(m_argparser.mps_file_name));

            if (m_argparser.is_minimization_explicit) {
                m_model.set_is_minimization(true);
            } else if (m_argparser.is_maximization_explicit) {
                m_model.set_is_minimization(false);
            }
        }

        /**
         * If the option file is given, the option values specified in the file
         * will be used for the calculation. Otherwise, the default values will
         * be used.
         */
        if (!m_argparser.option_file_name.empty()) {
            m_option.setup(m_argparser.option_file_name);
        }

        /**
         * If the mutable variable file is given, only the variables listed in
         * the file can be changed.
         */
        if (!m_argparser.mutable_variable_file_name.empty()) {
            const auto MUTABLE_VARIABLE_NAMES = printemps::helper::read_names(
                m_argparser.mutable_variable_file_name);
            m_model.unfix_variables(MUTABLE_VARIABLE_NAMES);
        }

        /**
         * If the fixed variable file is given, the values of the variables will
         * be fixed at the specified values.
         */
        if (!m_argparser.fixed_variable_file_name.empty()) {
            const auto FIXED_VARIABLES_AND_VALUES =
                printemps::helper::read_names_and_values(
                    m_argparser.fixed_variable_file_name);
            m_model.fix_variables(FIXED_VARIABLES_AND_VALUES);
        }

        /**
         * If the selection constraint file is given, the constraints listed in
         * the file will be regarded as user-defined selection constraints.
         */
        if (!m_argparser.selection_constraint_file_name.empty()) {
            const auto SELECTION_CONSTRAINT_NAMES =
                printemps::helper::read_names(
                    m_argparser.selection_constraint_file_name);
            m_model.set_user_defined_selection_constraints(
                SELECTION_CONSTRAINT_NAMES);
        }

        /**
         * If the flippable variable pair file is given, register 2-flip moves
         * and activate two-flip neighborhood moves.
         */
        if (!m_argparser.flippable_variable_pair_file_name.empty()) {
            const auto VARIABLE_NAME_PAIRS = printemps::helper::read_name_pairs(
                m_argparser.flippable_variable_pair_file_name);
            m_option.neighborhood.is_enabled_two_flip_move = true;
            m_model.setup_flippable_variable_ptr_pairs(VARIABLE_NAME_PAIRS);
        }

        /**
         * If the initial solution file is given, the values of the variables in
         * the file will be used as the initial values. Otherwise, the default
         * values will be used.
         */
        if (!m_argparser.initial_solution_file_name.empty()) {
            const auto INITIAL_SOLUTION =
                printemps::helper::read_names_and_values(
                    m_argparser.initial_solution_file_name);
            m_model.import_solution(INITIAL_SOLUTION);
        }
    }

    /*************************************************************************/
    inline void solve(void) {
        /**
         * Run the solver.
         */
        printemps::solver::IPSolver solver;

        if (m_argparser.include_mps_loading_time) {
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

        if (m_argparser.export_json_instance) {
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
            m_argparser.minimum_common_element,
            m_option.output.verbose >= printemps::option::verbose::Outer);
        extractor.write_pairs("flip.txt");
    }

    /*************************************************************************/
    inline void run(void) {
        if (!m_argparser.extract_flippable_variable_pairs) {
            this->solve();
        } else {
            this->extract_flippable_variable_pairs();
        }
    }

    /*************************************************************************/
    inline const Argparser &argparser(void) const {
        return m_argparser;
    }
};
}  // namespace printemps::standalone
#endif
/*****************************************************************************/
// END
/*****************************************************************************/