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
     * Best model objective processed so far, used only as a cheap pre-gate:
     * the incumbent holder replaces its feasible incumbent solely on a strict
     * model-objective improvement, so when this value has not decreased the
     * held solution (and therefore its true cost) is unchanged and we can skip
     * recomputation. Initialized to +infinity so the first feasible incumbent
     * always passes. The callback runs serially from the solver thread, so no
     * mutex is required.
     */
    double m_best_model_objective;

    /**
     * Best *true* soft-clause cost emitted so far, recomputed directly from the
     * printed variable assignment (see exact_cost). Emission is gated on this
     * value so the streamed o-lines are strictly decreasing and each o-line
     * matches the cost of its own v-line. Initialized to the maximum uint64_t.
     */
    uint64_t m_best_cost;

    bool m_have_emitted_solution;

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

        m_best_model_objective  = std::numeric_limits<double>::infinity();
        m_best_cost             = std::numeric_limits<uint64_t>::max();
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
        m_model.wcnf_handler().import(m_wcnf);

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

        /**
         * Cheap pre-gate: skip unless the model objective strictly improved,
         * because the incumbent holder only swaps in a new feasible solution on
         * a strict improvement, so otherwise the assignment is unchanged.
         */
        const double MODEL_OBJECTIVE = HOLDER.feasible_incumbent_objective();
        if (MODEL_OBJECTIVE + constant::EPSILON >= m_best_model_objective) {
            return;
        }
        m_best_model_objective = MODEL_OBJECTIVE;

        /**
         * Recompute the true cost from the assignment itself. The model's
         * objective is built on the soft-clause slack variables, which the
         * metaheuristic may leave at 1 even when the clause is actually
         * satisfied, so the model objective can overestimate the true cost.
         * Emit only when the true cost strictly improves so that the streamed
         * o-lines stay strictly decreasing.
         */
        const auto    &SOLUTION = HOLDER.feasible_incumbent_solution();
        const uint64_t COST     = exact_cost(SOLUTION);
        if (COST >= m_best_cost) {
            return;
        }
        m_best_cost = COST;
        emit_solution(SOLUTION, COST);
    }

    /*************************************************************************/
    /**
     * Recompute the soft-clause cost exactly in uint64_t directly from the
     * variable assignment: sum the weights of the soft clauses that the given
     * assignment fails to satisfy.
     *
     * This must not be derived from the "soft_slacks" proxy. Each soft clause
     * is modelled as `... + s >= 1 - |N|`, so the slack s is free to be 1 even
     * when the clause is already satisfied by the x-variables; the
     * metaheuristic frequently leaves such slacks at 1, which made the old
     * slack-based `o`-line overestimate the true cost and disagree with the
     * printed `v`-line (MSE's verifier reports "VERIFIED NOT_MATCHED").
     * Evaluating the clauses themselves yields exactly what the verifier
     * computes. Accumulating in uint64_t also avoids the double-precision loss
     * that would occur for weight sums exceeding 2^53.
     *
     * @param a_VALUES 0-based variable assignment; index i holds x_{i+1}.
     */
    inline uint64_t exact_cost(const std::vector<int> &a_VALUES) const {
        const int N    = static_cast<int>(a_VALUES.size());
        uint64_t  cost = 0;
        for (const auto &CLAUSE : m_wcnf.soft_clauses) {
            bool satisfied = false;
            for (const auto literal : CLAUSE.literals) {
                const int  VAR_INDEX = std::abs(literal);  // 1-based
                const bool VALUE     = (VAR_INDEX >= 1 && VAR_INDEX <= N) &&
                                   (a_VALUES[VAR_INDEX - 1] != 0);
                if ((literal > 0 && VALUE) || (literal < 0 && !VALUE)) {
                    satisfied = true;
                    break;
                }
            }
            if (!satisfied) {
                cost += CLAUSE.weight;
            }
        }
        return cost;
    }

    /*************************************************************************/
    /**
     * Convenience overload that reads the x-variable assignment from the first
     * ("variables") proxy of a dense incumbent solution. The soft-clause slack
     * proxies follow it and are deliberately ignored (see exact_cost above).
     */
    inline uint64_t exact_cost(
        const solution::IPDenseSolution &a_SOLUTION) const {
        static const std::vector<int> EMPTY;
        if (a_SOLUTION.variable_value_proxies.empty()) {
            return exact_cost(EMPTY);
        }
        return exact_cost(
            a_SOLUTION.variable_value_proxies.front().flat_indexed_values());
    }

    /*************************************************************************/
    inline void emit_solution(const solution::IPDenseSolution &a_SOLUTION,
                              const uint64_t                   a_COST) {
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

        std::cout << "o " << a_COST << "\n";
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
            const auto &VALUES =
                SOLUTION.variables("variables").flat_indexed_values();
            const uint64_t COST = exact_cost(VALUES);
            if (COST < m_best_cost) {
                m_best_cost = COST;
                std::string v_line;
                const int   N = static_cast<int>(VALUES.size());
                v_line.reserve(N);
                for (auto i = 0; i < N; i++) {
                    v_line.push_back(VALUES[i] != 0 ? '1' : '0');
                }
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
