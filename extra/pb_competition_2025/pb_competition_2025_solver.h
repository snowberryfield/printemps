/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_EXTRA_PB_COMPETITION_2025_PB_COMPETITION_2025_SOLVER_H__
#define PRINTEMPS_EXTRA_PB_COMPETITION_2025_PB_COMPETITION_2025_SOLVER_H__

#include "pb_competition_2025_solver_argparser.h"

namespace printemps::extra::pb_competition_2025 {
inline bool interrupted = false;
inline void interrupt_handler([[maybe_unused]] int signum) {
    interrupted = true;
}

/*****************************************************************************/
class PBCompetition2025Solver {
   private:
    PBCompetition2025SolverArgparser m_argparser;
    opb::OPB                         m_opb;
    model::IPModel                   m_model;
    option::Option                   m_option;
    utility::TimeKeeper              m_time_keeper;

    /*************************************************************************/
    inline void print_program_name(void) const {
        std::cout << "c PRINTEMPS " + constant::VERSION + " (" +
                         constant::PROJECT_URL + ")"
                  << std::endl;
    }

    /*************************************************************************/
    inline void print_metadata(void) const {
        std::cout << "c #variable: " << m_opb.metadata.number_of_variables
                  << std::endl;
        std::cout << "c #constraint: " << m_opb.metadata.number_of_constraints
                  << std::endl;
        std::cout << "c #equal: " << m_opb.metadata.number_of_equals
                  << std::endl;
        std::cout << "c intsize: " << m_opb.metadata.intsize << std::endl;
        std::cout << "c sizeproduct: " << m_opb.metadata.sizeproduct
                  << std::endl;
        std::cout << "c #soft: " << m_opb.metadata.number_of_soft_constraints
                  << std::endl;
        std::cout << "c mincost: " << m_opb.metadata.mincost << std::endl;
        std::cout << "c maxcost: " << m_opb.metadata.maxcost << std::endl;
        std::cout << "c sumcost: " << m_opb.metadata.sumcost << std::endl;
    }

    /*************************************************************************/
    inline void print_option(void) const {
        std::cout << "c iteration_max: " << m_option.general.iteration_max
                  << std::endl;
        std::cout << "c time_max: " << m_option.general.time_max << std::endl;
        std::cout << "c number_of_threads(move update): "
                  << m_option.parallel.number_of_threads_move_update
                  << std::endl;
        std::cout << "c number_of_threads(move evaluation): "
                  << m_option.parallel.number_of_threads_move_evaluation
                  << std::endl;
        std::cout << "c seed: " << m_option.general.seed << std::endl;
    }

    /*************************************************************************/
    inline void print_result(
        const printemps::solver::IPSolver &a_SOLVER) const {
        if (a_SOLVER.global_state()
                .incumbent_holder.is_found_feasible_solution()) {
            std::cout << "s SATISFIABLE" << std::endl;
            std::cout << "v ";

            const auto &SOLUTION =
                a_SOLVER.global_state()
                    .incumbent_holder.feasible_incumbent_solution();
            const auto &VALUE_PRROXY = SOLUTION.variable_value_proxies.front();
            const int   NUMBER_OF_VARIABLES = VALUE_PRROXY.number_of_elements();
            for (auto i = 0; i < NUMBER_OF_VARIABLES; i++) {
                if (VALUE_PRROXY.flat_indexed_values(i) == 1) {
                    std::cout << VALUE_PRROXY.flat_indexed_names(i) << " ";
                } else {
                    std::cout << "-" << VALUE_PRROXY.flat_indexed_names(i)
                              << " ";
                }
            }
            std::cout << std::endl;
        } else {
            std::cout << "s UNKNOWN" << std::endl;
        }
    }

   public:
    /*************************************************************************/
    PBCompetition2025Solver(void) {
        this->initialize();
    }

    /*************************************************************************/
    PBCompetition2025Solver(const int argc, const char *argv[]) {
        this->initialize();
        this->setup(argc, argv);
    }

    /*************************************************************************/
    inline void initialize(void) {
        m_argparser.initialize();
        m_opb.initialize();
        m_model.initialize();
        m_option.initialize();
        m_time_keeper.initialize();

        m_option.general.iteration_max = -1;
        m_option.general.time_max      = -1.0;
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

        this->print_program_name();

        /**
         * Check the metadata of the specified OPB file.
         */
        auto metadata = opb::OPB::check_metadata(m_argparser.pb_file_name);
        if (metadata.intsize >= 32) {
            std::cout << "s UNSUPPORTED" << std::endl;
            exit(0);
        }

        /**
         * Read the specified OPB file and convert to the model.
         */
        m_opb.read_opb(m_argparser.pb_file_name);

        m_model.import_opb(m_opb);
        m_model.set_name(
            printemps::utility::base_name(m_argparser.pb_file_name));

        if (m_argparser.is_specified_iteration_max) {
            m_option.general.iteration_max = m_argparser.iteration_max;
        }
        if (m_argparser.is_specified_time_max) {
            m_option.general.time_max = m_argparser.time_max;
        }
        if (m_argparser.is_specified_number_of_threads) {
            m_option.parallel.number_of_threads_move_evaluation =
                m_argparser.number_of_threads;
            m_option.parallel.number_of_threads_move_update =
                m_argparser.number_of_threads;
#ifdef _OPENMP
            omp_set_num_threads(m_argparser.number_of_threads);
#endif
        }
        if (m_argparser.is_specified_seed) {
            m_option.general.seed = m_argparser.seed;
        }

        this->print_metadata();
        this->print_option();

        signal(SIGINT, interrupt_handler);
        signal(SIGTERM, interrupt_handler);
#ifndef _WIN32
        signal(SIGXCPU, interrupt_handler);
#endif
    }

    /*************************************************************************/
    inline void solve() {
        /**
         * Run the solver with callback function
         */
        printemps::solver::IPSolver solver;

        solver.setup(&m_model, m_option, m_time_keeper);

        double feasible_incumbent_objective =
            std::numeric_limits<double>::max();

        auto callback =
            [&feasible_incumbent_objective](
                printemps::solver::IPGlobalState *global_state_ptr) {
                const double current_feasible_incumbent_objective =
                    global_state_ptr->incumbent_holder
                        .feasible_incumbent_objective();

                if (current_feasible_incumbent_objective <
                    feasible_incumbent_objective) {
                    feasible_incumbent_objective =
                        current_feasible_incumbent_objective;
                    std::cout << "o " << std::fixed << std::setprecision(0)
                              << current_feasible_incumbent_objective
                              << std::endl
                              << std::flush;
                }
            };

        solver.set_check_interrupt([]() { return interrupted; });
        solver.set_callback(callback);
        solver.solve_without_postprocess();
        this->print_result(solver);
    }

    /*************************************************************************/
    inline void run(void) {
        this->solve();
    }
};
}  // namespace printemps::extra::pb_competition_2025
#endif
/*****************************************************************************/
// END
/*****************************************************************************/