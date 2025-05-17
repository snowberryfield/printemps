/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_STANDALONE_PB_SOLVER_PB_SOLVER_H__
#define PRINTEMPS_STANDALONE_PB_SOLVER_PB_SOLVER_H__

#include "pb_solver_argparser.h"

namespace printemps::standalone::pb_solver {
inline bool interrupted = false;
inline void interrupt_handler([[maybe_unused]] int signum) {
    interrupted = true;
}

/*****************************************************************************/
class PBSolver {
   private:
    PBSolverArgparser   m_argparser;
    pb::PB              m_pb;
    model::IPModel      m_model;
    option::Option      m_option;
    utility::TimeKeeper m_time_keeper;

   public:
    /*************************************************************************/
    PBSolver(void) {
        this->initialize();
    }

    /*************************************************************************/
    PBSolver(const int argc, const char *argv[]) {
        this->initialize();
        this->setup(argc, argv);
    }

    /*************************************************************************/
    inline void initialize(void) {
        m_argparser.initialize();
        m_pb.initialize();
        m_model.initialize();
        m_option.initialize();
        m_time_keeper.initialize();
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
         * Read the specified PB file and convert to the model.
         */
        m_pb.read_pb(m_argparser.pb_file_name);
        m_model.import_pb(m_pb);
        m_model.set_name(
            printemps::utility::base_name(m_argparser.pb_file_name));

        if (m_argparser.is_minimization_explicit) {
            m_model.set_is_minimization(true);
        } else if (m_argparser.is_maximization_explicit) {
            m_model.set_is_minimization(false);
        }

        /**
         * If the option file is given, the option values specified in the file
         * will be used for the calculation. Otherwise, the default values will
         * be used.
         */
        if (!m_argparser.option_file_name.empty()) {
            m_option.setup(m_argparser.option_file_name);
        }
        if (m_argparser.is_iteration_max_given) {
            m_option.general.iteration_max = m_argparser.iteration_max;
        }
        if (m_argparser.is_time_max_given) {
            m_option.general.time_max = m_argparser.time_max;
        }
        if (m_argparser.is_verbose_given) {
            m_option.output.verbose = m_argparser.verbose;
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

        signal(SIGINT, interrupt_handler);
        signal(SIGTERM, interrupt_handler);
#ifndef _WIN32
        signal(SIGXCPU, interrupt_handler);
#endif
    }

    /*************************************************************************/
    inline void solve(void) {
        /**
         * Run the solver with callback function
         */
        printemps::solver::IPSolver solver;

        if (m_argparser.include_pb_loading_time) {
            solver.setup(&m_model, m_option, m_time_keeper);
        } else {
            solver.setup(&m_model, m_option);
        }
        solver.set_check_interrupt([]() { return interrupted; });

        const auto RESULT = solver.solve();

        /**
         * Print the result summary.
         */
        printemps::utility::print_info(
            "status: " + std::to_string(RESULT.solution.is_feasible()),
            m_option.output.verbose >= printemps::option::verbose::Warning);

        printemps::utility::print_info(
            "objective: " + std::to_string(RESULT.solution.objective()),
            m_option.output.verbose >= printemps::option::verbose::Warning);

        printemps::utility::print_info(
            "total violation: " +
                std::to_string(RESULT.solution.total_violation()),
            m_option.output.verbose >= printemps::option::verbose::Warning);

        RESULT.solution.write_json_by_name("incumbent.json");
        RESULT.solution.write_solution("incumbent.sol");
        RESULT.status.write_json_by_name("status.json");

        if (m_option.output.is_enabled_store_feasible_solutions) {
            RESULT.feasible_solution_archive.write_solutions_json(
                "feasible.json");
        }

        if (m_argparser.export_json_instance) {
            m_model.write_json(m_model.name() + ".json");
        }
    }

    /*************************************************************************/
    inline void run(void) {
        this->solve();
    }
};
}  // namespace printemps::standalone::pb_solver
#endif
/*****************************************************************************/
// END
/*****************************************************************************/