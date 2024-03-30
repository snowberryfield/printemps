/*****************************************************************************/
// Copyright (c) 2020-2023 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_PDLP_LINEAR_CORE_PROGRAMMING_CORE_H_
#define PRINTEMPS_SOLVER_PDLP_LINEAR_CORE_PROGRAMMING_CORE_H_

#include "pdlp_core_termination_status.h"
#include "pdlp_core_state.h"
#include "pdlp_core_state_manager.h"
#include "pdlp_core_result.h"

namespace printemps::solver::pdlp::core {
/*****************************************************************************/
class PDLPCore {
   private:
    linear_programming::LinearProgramming* m_instance_ptr;

    option::Option       m_option;
    PDLPCoreStateManager m_state_manager;
    PDLPCoreResult       m_result;

    /*************************************************************************/
    inline void preprocess(void) {
        m_state_manager.setup(m_instance_ptr, m_option);
    }

    /*************************************************************************/
    inline void postprocess(void) {
        /**
         * Prepare the result.
         */
        m_result = PDLPCoreResult(m_state_manager.state(), m_option);
    }

    /*************************************************************************/
    inline bool satisfy_time_over_terminate_condition(void) {
        const auto& STATE = m_state_manager.state();

        if (STATE.elapsed_time > m_option.pdlp.time_max) {
            m_state_manager.set_termination_status(
                PDLPCoreTerminationStatus::TIME_OVER);
            return true;
        }

        if (STATE.elapsed_time + m_option.pdlp.time_offset >
            m_option.general.time_max) {
            m_state_manager.set_termination_status(
                PDLPCoreTerminationStatus::TIME_OVER);
            return true;
        }
        return false;
    }

    /*************************************************************************/
    inline bool satisfy_iteration_over_terminate_condition(void) {
        const auto& STATE = m_state_manager.state();

        if (STATE.total_iteration >= m_option.pdlp.iteration_max) {
            m_state_manager.set_termination_status(
                PDLPCoreTerminationStatus::ITERATION_OVER);
            return true;
        }
        return false;
    }

    /*************************************************************************/
    inline bool satisfy_optimal_terminate_condition(void) {
        const auto&  STATE     = m_state_manager.state();
        const double TOLERANCE = m_option.pdlp.tolerance;
        if (STATE.primal.relative_violation_norm > TOLERANCE) {
            return false;
        }

        if (STATE.dual.relative_violation_norm > TOLERANCE) {
            return false;
        }

        if (STATE.relative_gap > TOLERANCE) {
            return false;
        }

        m_state_manager.set_termination_status(
            PDLPCoreTerminationStatus::OPTIMAL);
        return true;
    }

    /*************************************************************************/
    inline bool satisfy_infeasible_terminate_condition(void) {
        const auto&  STATE     = m_state_manager.state();
        const double TOLERANCE = m_option.pdlp.tolerance;

        if (STATE.dual.relative_violation_norm > TOLERANCE) {
            return false;
        }

        if (STATE.dual.objective <
            STATE.primal.objective_upper_bound +
                std::max(1.0, fabs(STATE.primal.objective_upper_bound) *
                                  constant::EPSILON)) {
            return false;
        }

        m_state_manager.set_termination_status(
            PDLPCoreTerminationStatus::INFEASIBLE);
        return true;
    }

    /*****************************************************************************/
    inline void print_table_header(const bool a_IS_ENABLED_PRINT) {
        if (!a_IS_ENABLED_PRINT) {
            return;
        }

        utility::print(
            "---------+-----------+-----------------------+-------------------"
            "----+----------",
            true);
        utility::print(
            "Iteration| Step Size |        Primal         |         Dual      "
            "    |   Gap   ",
            true);
        utility::print(
            "         |           | Objective (Violation) | Objective "
            "(Violation) |         ",
            true);
        utility::print(
            "---------+-----------+-----------------------+-------------------"
            "----+----------",
            true);
    }

    /*****************************************************************************/
    inline void print_table_initial(const bool a_IS_ENABLED_PRINT) {
        if (!a_IS_ENABLED_PRINT) {
            return;
        }

        auto&      state = m_state_manager.state();
        const auto SIGN  = m_instance_ptr->is_minimization ? 1.0 : -1.0;

        char mark_primal_feasible = ' ';
        char mark_dual_feasible   = ' ';
        char mark_converge        = ' ';

        double corrected_dual_relative_violation_norm =
            state.dual.relative_violation_norm;

        if (state.primal.relative_violation_norm < m_option.pdlp.tolerance) {
            mark_primal_feasible = '*';
        }

        if (state.dual.relative_violation_norm < m_option.pdlp.tolerance) {
            mark_dual_feasible                     = '*';
            corrected_dual_relative_violation_norm = 0.0;
        }

        if (state.relative_gap < m_option.pdlp.tolerance) {
            mark_converge = '*';
        }

        std::printf(  //
            " INITIAL |  %8.2e |%c%11.4e (%8.2e)|%c%11.4e (%8.2e)|%c%8.3e\n",
            state.step_size_current,                 //
            mark_primal_feasible,                    //
            state.primal.objective * SIGN,           //
            state.primal.relative_violation_norm,    //
            mark_dual_feasible,                      //
            state.dual.objective * SIGN,             //
            corrected_dual_relative_violation_norm,  //
            mark_converge,                           //
            state.relative_gap);
    }

    /*****************************************************************************/
    inline void print_table_body(const bool a_IS_ENABLED_PRINT) {
        if (!a_IS_ENABLED_PRINT) {
            return;
        }

        auto&      state = m_state_manager.state();
        const auto SIGN  = m_instance_ptr->is_minimization ? 1.0 : -1.0;

        char mark_restart         = ' ';
        char mark_primal_feasible = ' ';
        char mark_dual_feasible   = ' ';
        char mark_converge        = ' ';

        if (state.is_enabled_restart) {
            if (state.restart_mode == RestartMode::Current) {
                mark_restart = 'c';
            } else if (state.restart_mode == RestartMode::Average) {
                mark_restart = 'a';
            }
        }

        double corrected_dual_relative_violation_norm =
            state.dual.relative_violation_norm;
        if (state.primal.relative_violation_norm < m_option.pdlp.tolerance) {
            mark_primal_feasible = '*';
        }

        if (state.dual.relative_violation_norm < m_option.pdlp.tolerance) {
            mark_dual_feasible                     = '*';
            corrected_dual_relative_violation_norm = 0.0;
        }

        if (state.relative_gap < m_option.pdlp.tolerance) {
            mark_converge = '*';
        }

        std::printf(  //
            "%8d%c|  %8.2e |%c%11.4e (%8.2e)|%c%11.4e (%8.2e)|%c%8.3e\n",
            state.total_iteration,                   //
            mark_restart,                            //
            state.step_size_current,                 //
            mark_primal_feasible,                    //
            state.primal.objective * SIGN,           //
            state.primal.relative_violation_norm,    //
            mark_dual_feasible,                      //
            state.dual.objective * SIGN,             //
            corrected_dual_relative_violation_norm,  //
            mark_converge,                           //
            state.relative_gap);
    }

    /*****************************************************************************/
    inline void print_table_footer(const bool a_IS_ENABLED_PRINT) {
        if (!a_IS_ENABLED_PRINT) {
            return;
        }

        utility::print(
            "---------+-----------+-----------------------+-------------------"
            "----+----------",
            true);
        utility::print_info(  //
            " -- c: Restarts from the current solution.", true);
        utility::print_info(  //
            " -- a: Restarts from the averaged solution.", true);
        utility::print_info(  //
            " -- *: Meets the specified convergence criteria.", true);
        utility::print_single_line(true);
    }

   public:
    /*************************************************************************/
    PDLPCore(void) {
        this->initialize();
    }

    /*************************************************************************/
    PDLPCore(linear_programming::LinearProgramming* a_instance_ptr,
             const option::Option&                  a_OPTION) {
        this->initialize();
        this->setup(a_instance_ptr, a_OPTION);
    }

    /*************************************************************************/
    inline void initialize(void) {
        m_instance_ptr = nullptr;
        m_option.initialize();
        m_state_manager.initialize();
        m_result.initialize();
    }

    /*************************************************************************/
    inline void setup(linear_programming::LinearProgramming* a_instance_ptr,
                      const option::Option&                  a_OPTION) {
        m_instance_ptr = a_instance_ptr;
        m_option       = a_OPTION;
    }

    /*************************************************************************/
    inline void run(void) {
        /**
         * Start to measure computational time.
         */
        utility::TimeKeeper time_keeper;
        time_keeper.set_start_time();

        auto& state = m_state_manager.state();

        /**
         * Preprocess.
         */
        this->preprocess();

        bool previous_iteration_log_flag = false;

        /**
         * Iterations start.
         */
        m_state_manager.update_convergence_information();

        utility::print_single_line(m_option.output.verbose >=
                                   option::verbose::Outer);
        utility::print_message(
            "PDLP starts.", m_option.output.verbose >= option::verbose::Outer);

        this->print_table_header(m_option.output.verbose >=
                                 option::verbose::Inner);

        this->print_table_initial(m_option.output.verbose >=
                                  option::verbose::Inner);

        const int RESTART_CHECK_INTERVAL = m_option.pdlp.restart_check_interval;
        const int CONVERGENCE_CHECK_INTERVAL =
            m_option.pdlp.convergence_check_interval;
        const int LOG_INTERVAL = m_option.pdlp.log_interval;

        m_state_manager.setup_new_inner_loop();
        m_state_manager.reset_iteration();
        while (true) {
            m_state_manager.set_elapsed_time(time_keeper.clock());

            /**
             * Terminate the loop if the time is over.
             */
            if (this->satisfy_time_over_terminate_condition()) {
                if (!previous_iteration_log_flag) {
                    this->print_table_body(m_option.output.verbose >=
                                           option::verbose::Inner);
                }
                break;
            }

            /**
             * Terminate the loop if the iteration is over.
             */
            if (this->satisfy_iteration_over_terminate_condition()) {
                if (!previous_iteration_log_flag) {
                    this->print_table_body(m_option.output.verbose >=
                                           option::verbose::Inner);
                }
                break;
            }

            /**
             * Terminate the loop if an optimal solution was found.
             */
            if (this->satisfy_optimal_terminate_condition()) {
                if (!previous_iteration_log_flag) {
                    this->print_table_body(m_option.output.verbose >=
                                           option::verbose::Inner);
                }
                break;
            }

            /**
             * Terminate the loop if an optimal solution was found.
             */
            if (this->satisfy_infeasible_terminate_condition()) {
                if (!previous_iteration_log_flag) {
                    this->print_table_body(m_option.output.verbose >=
                                           option::verbose::Inner);
                }
                break;
            }

            m_state_manager.update_solution();
            m_state_manager.update_averaged_solution();

            if (state.total_iteration % RESTART_CHECK_INTERVAL == 0) {
                m_state_manager.update_restart_information();
            } else {
                m_state_manager.skip_restart();
            }

            if (state.total_iteration % CONVERGENCE_CHECK_INTERVAL == 0 ||
                state.total_iteration % LOG_INTERVAL == 0) {
                m_state_manager.update_convergence_information();
            }

            if (state.total_iteration % LOG_INTERVAL == 0) {
                this->print_table_body(m_option.output.verbose >=
                                       option::verbose::Inner);
                previous_iteration_log_flag = true;
            } else {
                previous_iteration_log_flag = false;
            }

            if (state.is_enabled_restart) {
                m_state_manager.update_restart_solution();
                m_state_manager.update_primal_weight();
                m_state_manager.setup_new_inner_loop();
            } else {
                m_state_manager.next_inner_iteration();
            }
            m_state_manager.next_total_iteration();
        }

        this->print_table_footer(m_option.output.verbose >=
                                 option::verbose::Inner);
        this->postprocess();
    }

    /*************************************************************************/
    inline const PDLPCoreResult& result(void) const {
        return m_result;
    }
};  // namespace printemps::solver::pdlp::core
}  // namespace printemps::solver::pdlp::core

#endif
/*****************************************************************************/
// END
/*****************************************************************************/