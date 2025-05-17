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
    pb::PB                           m_pb;
    model::IPModel                   m_model;
    option::Option                   m_option;
    utility::TimeKeeper              m_time_keeper;

    /*************************************************************************/
    inline void print_metadata(void) const {
        std::cout << "c #variable: " << m_pb.metadata.number_of_variables
                  << std::endl;
        std::cout << "c #constraint: " << m_pb.metadata.number_of_constraints
                  << std::endl;
        std::cout << "c #equal: " << m_pb.metadata.number_of_equals
                  << std::endl;
        std::cout << "c intsize: " << m_pb.metadata.intsize << std::endl;
        std::cout << "c sizeproduct: " << m_pb.metadata.sizeproduct
                  << std::endl;
        std::cout << "c #soft: " << m_pb.metadata.number_of_soft_constraints
                  << std::endl;
        std::cout << "c mincost: " << m_pb.metadata.mincost << std::endl;
        std::cout << "c maxcost: " << m_pb.metadata.maxcost << std::endl;
        std::cout << "c sumcost: " << m_pb.metadata.sumcost << std::endl;
    }

    /*************************************************************************/
    inline void print_result(
        const printemps::solver::IPResult &a_RESULT) const {
        if (a_RESULT.status.is_found_feasible_solution) {
            std::cout << "s SATISFIABLE" << std::endl;
            std::cout << "v ";

            const auto &VALUE_PRROXY = a_RESULT.solution.variables("variables");
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
         * Read the specified PB file and convert to the model.
         */
        m_pb.read_pb(m_argparser.pb_file_name);

        if (m_pb.metadata.intsize >= 53) {
            std::cout << "s UNSUPPORTED" << std::endl;
            exit(0);
        }
        this->print_metadata();

        m_model.import_pb(m_pb);
        m_model.set_name(
            printemps::utility::base_name(m_argparser.pb_file_name));

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

        if (m_argparser.include_pb_loading_time) {
            solver.setup(&m_model, m_option, m_time_keeper);
        } else {
            solver.setup(&m_model, m_option);
        }

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
                    std::cout << "o " << current_feasible_incumbent_objective
                              << std::endl;
                }
            };

        solver.set_check_interrupt([]() { return interrupted; });
        solver.set_callback(callback);
        const auto RESULT = solver.solve();
        this->print_result(RESULT);
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