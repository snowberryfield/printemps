/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_EXTRA_MAXSAT_EVALUATION_MAXSAT_EVALUATION_SOLVER_H__
#define PRINTEMPS_EXTRA_MAXSAT_EVALUATION_MAXSAT_EVALUATION_SOLVER_H__

#include "maxsat_evaluation_solver_argparser.h"

namespace printemps::extra::maxsat_evaluation {
inline bool interrupted = false;
inline void interrupt_handler([[maybe_unused]] int signum) {
    interrupted = true;
}

/*****************************************************************************/
class MaxSATEvaluationSolver {
   private:
    MaxSATEvaluationSolverArgparser m_argparser;
    wcnf::WCNF                      m_wcnf;
    model::IPModel                  m_model;
    option::Option                  m_option;
    utility::TimeKeeper             m_time_keeper;

    /**
     * Best feasible objective value emitted so far. Initialized to +infinity
     * so that the first feasible incumbent always passes the improvement
     * check. The callback runs serially from the solver thread, so no mutex
     * is required.
     */
    double m_best_objective;
    bool   m_have_emitted_solution;

   public:
    /*************************************************************************/
    MaxSATEvaluationSolver(void) {
        this->initialize();
    }

    /*************************************************************************/
    MaxSATEvaluationSolver(const int argc, const char *argv[]) {
        this->initialize();
        this->setup(argc, argv);
    }

    /*************************************************************************/
    inline void initialize(void) {
        m_argparser.initialize();
        m_wcnf.initialize();
        m_model.initialize();
        m_option.initialize();
        m_time_keeper.initialize();

        m_best_objective        = std::numeric_limits<double>::infinity();
        m_have_emitted_solution = false;
    }

    /*************************************************************************/
    inline void setup(const int argc, const char *argv[]) {
        if (argc < 2) {
            m_argparser.print_usage();
            std::exit(1);
        }

        m_time_keeper.set_start_time();

        m_argparser.parse(argc, argv);

        /**
         * Force PRINTEMPS to be silent so that only c/o/v/s lines appear on
         * stdout. Force single-threaded execution per MSE rules.
         */
        m_option.output.verbose                             = option::verbose::Off;
        m_option.parallel.number_of_threads_move_evaluation = 1;
        m_option.parallel.number_of_threads_move_update     = 1;
#ifdef _OPENMP
        omp_set_num_threads(1);
#endif

        /**
         * Reserve a small wall-clock margin so that we have time to flush
         * the final s/o/v lines before SIGTERM->SIGKILL. The margin matters
         * less for the long timeouts but is critical at 60 s.
         */
        if (m_argparser.is_specified_timeout) {
            const double SAFETY_MARGIN_SECONDS = 1.0;
            m_option.general.time_max =
                std::max(m_argparser.timeout_seconds - SAFETY_MARGIN_SECONDS,
                         m_argparser.timeout_seconds * 0.95);
        }

        /**
         * Improve PRINTEMPS' behaviour on Boolean disjunctive constraints.
         */
        m_option.neighborhood.is_enabled_two_flip_move = true;

        /**
         * Parse the WCNF instance and import it into the IPModel.
         */
        m_wcnf.read_wcnf(m_argparser.wcnf_file_name);
        m_model.set_name(utility::base_name(m_argparser.wcnf_file_name));
        m_model.import_wcnf(m_wcnf);

        signal(SIGINT, interrupt_handler);
        signal(SIGTERM, interrupt_handler);
#ifndef _WIN32
        signal(SIGXCPU, interrupt_handler);
#endif
    }

    /*************************************************************************/
    inline void on_incumbent(solver::IPGlobalState *a_GLOBAL_STATE) {
        if (a_GLOBAL_STATE == nullptr) {
            return;
        }
        const auto &HOLDER = a_GLOBAL_STATE->incumbent_holder;
        if (!HOLDER.is_found_feasible_solution()) {
            return;
        }
        const double CURRENT = HOLDER.feasible_incumbent_objective();
        if (CURRENT + constant::EPSILON >= m_best_objective) {
            return;
        }
        m_best_objective = CURRENT;
        emit_solution(HOLDER.feasible_incumbent_solution());
    }

    /*************************************************************************/
    /**
     * Recompute the soft-clause cost exactly in uint64_t from the original
     * WCNF weights and the per-clause violation flags carried by the
     * "soft_slacks" proxy. The model's internal objective is accumulated in
     * double, so for weight sums exceeding 2^53 the double value loses lower
     * bits and the printed `o`-line would drift from the true integer cost
     * (and for sums approaching 2^63, the long-long cast itself would be UB).
     * Computing here from the uint64_t weights avoids both problems.
     */
    inline uint64_t exact_cost(const std::vector<int> &a_SLACKS) const {
        const size_t N = m_wcnf.soft_clauses.size();
        uint64_t cost = 0;
        for (size_t i = 0; i < N && i < a_SLACKS.size(); ++i) {
            if (a_SLACKS[i] != 0) {
                cost += m_wcnf.soft_clauses[i].weight;
            }
        }
        return cost;
    }

    /*************************************************************************/
    inline void emit_solution(const solution::IPDenseSolution &a_SOLUTION) {
        /**
         * The first variable proxy named "variables" holds x_1..x_n in
         * 1-indexed order; the soft-clause slacks live in subsequent proxies
         * and are excluded from the v-line.
         */
        std::string v_line;
        if (!a_SOLUTION.variable_value_proxies.empty()) {
            const auto &PROXY  = a_SOLUTION.variable_value_proxies.front();
            const auto &VALUES = PROXY.flat_indexed_values();
            const int   N      = static_cast<int>(VALUES.size());
            v_line.reserve(N);
            for (auto i = 0; i < N; i++) {
                v_line.push_back(VALUES[i] != 0 ? '1' : '0');
            }
        }

        /**
         * In import_wcnf the "variables" proxy is created first and
         * "soft_slacks" immediately after, so the slacks live at index 1.
         */
        const uint64_t COST =
            a_SOLUTION.variable_value_proxies.size() >= 2
                ? exact_cost(a_SOLUTION.variable_value_proxies[1]
                                 .flat_indexed_values())
                : 0;
        std::cout << "o " << COST << "\n";
        std::cout << "v " << v_line << std::endl;

        m_have_emitted_solution = true;
    }

    /*************************************************************************/
    inline int solve(void) {
        solver::IPSolver solver;
        solver.setup(&m_model, m_option, m_time_keeper);
        solver.set_check_interrupt([]() { return interrupted; });
        solver.set_callback(
            [this](solver::IPGlobalState *g) { this->on_incumbent(g); });

        const auto RESULT = solver.solve();

        /**
         * If the final feasible incumbent is strictly better than what we
         * streamed during the search, emit a final pair of o/v lines.
         */
        const auto &SOLUTION = RESULT.solution;
        if (SOLUTION.is_feasible()) {
            const double FINAL_COST = static_cast<double>(SOLUTION.objective());
            if (FINAL_COST + constant::EPSILON < m_best_objective) {
                m_best_objective = FINAL_COST;
                std::string v_line;
                const auto &PROXY  = SOLUTION.variables("variables");
                const auto &VALUES = PROXY.flat_indexed_values();
                const int   N      = static_cast<int>(VALUES.size());
                v_line.reserve(N);
                for (auto i = 0; i < N; i++) {
                    v_line.push_back(VALUES[i] != 0 ? '1' : '0');
                }
                const uint64_t COST = exact_cost(
                    SOLUTION.variables("soft_slacks").flat_indexed_values());
                std::cout << "o " << COST << "\n";
                std::cout << "v " << v_line << std::endl;
                m_have_emitted_solution = true;
            }
        }

        /**
         * MSE 2026 status / exit code mapping. PRINTEMPS does not prove
         * optimality nor unsatisfiability of hard clauses, so we emit
         * SATISFIABLE on any feasible incumbent and UNKNOWN otherwise.
         * UNSATISFIABLE is handled by the main() exception handler.
         */
        if (m_have_emitted_solution) {
            std::cout << "s SATISFIABLE" << std::endl;
            return 10;
        } else {
            std::cout << "s UNKNOWN" << std::endl;
            return 0;
        }
    }

    /*************************************************************************/
    inline int run(void) {
        return this->solve();
    }
};
}  // namespace printemps::extra::maxsat_evaluation
#endif
/*****************************************************************************/
// END
/*****************************************************************************/
