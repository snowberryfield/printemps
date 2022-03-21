/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_LOCAL_SEARCH_CORE_LOCAL_SEARCH_CORE_H__
#define PRINTEMPS_SOLVER_LOCAL_SEARCH_CORE_LOCAL_SEARCH_CORE_H__

#include "../../abstract_solver_core.h"
#include "local_search_core_termination_status.h"
#include "local_search_core_state.h"
#include "local_search_core_result.h"

namespace printemps {
namespace solver {
namespace local_search {
namespace core {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class LocalSearchCore : public AbstractSolverCore<T_Variable, T_Expression> {
   private:
    LocalSearchCoreState<T_Variable, T_Expression>  m_state;
    LocalSearchCoreResult<T_Variable, T_Expression> m_result;
    std::mt19937                                    m_get_rand_mt;

    /*************************************************************************/
    inline void preprocess(void) {
        /**
         * Reset the local augmented incumbent.
         */
        this->m_incumbent_holder_ptr->reset_local_augmented_incumbent();

        /**
         * Reset the feasible solutions storage.
         */
        this->m_feasible_solutions.clear();

        /**
         * Reset the last update iterations.
         */
        this->m_memory_ptr->reset_last_update_iterations();

        /**
         * Reset the variable improvability.
         */
        this->m_model_ptr->reset_variable_objective_improvabilities();
        this->m_model_ptr->reset_variable_feasibility_improvabilities();

        /**
         * Initialize the solution and update the model.
         */
        this->m_model_ptr->import_variable_values(
            this->m_initial_variable_value_proxies);
        this->m_model_ptr->update();

        m_state.solution_score = this->m_model_ptr->evaluate({});
        m_state.update_status =
            this->m_incumbent_holder_ptr->try_update_incumbent(
                this->m_model_ptr, m_state.solution_score);

        /**
         * Initialize the update status.
         */
        m_state.total_update_status =
            solution::IncumbentHolderConstant::STATUS_NOT_UPDATED;
        m_state.termination_status =
            LocalSearchCoreTerminationStatus::ITERATION_OVER;

        /**
         * Initialize the random generator, which is used for shuffling moves.
         */
        m_get_rand_mt.seed(this->m_option.local_search.seed);
    }

    /*************************************************************************/
    inline void postprocess(void) {
        /**
         * Prepare the result.
         */
        m_result = LocalSearchCoreResult<T_Variable, T_Expression>(m_state);
    }

    /*************************************************************************/
    inline bool satisfy_terminate_condition(const double a_ELAPSED_TIME) {
        if (a_ELAPSED_TIME > this->m_option.local_search.time_max) {
            m_state.termination_status =
                LocalSearchCoreTerminationStatus::TIME_OVER;
            return true;
        }

        if (a_ELAPSED_TIME + this->m_option.local_search.time_offset >
            this->m_option.time_max) {
            m_state.termination_status =
                LocalSearchCoreTerminationStatus::TIME_OVER;
            return true;
        }

        if (m_state.iteration >= this->m_option.local_search.iteration_max) {
            m_state.termination_status =
                LocalSearchCoreTerminationStatus::ITERATION_OVER;
            return true;
        }

        if (this->m_incumbent_holder_ptr->feasible_incumbent_objective() <=
            this->m_option.target_objective_value) {
            m_state.termination_status =
                LocalSearchCoreTerminationStatus::REACH_TARGET;
            return true;
        }
        return false;
    }

    /*************************************************************************/
    inline void update_moves(
        const neighborhood::Move<T_Variable, T_Expression>& a_CURRENT_MOVE) {
        bool accept_all                    = true;
        bool accept_objective_improvable   = true;
        bool accept_feasibility_improvable = true;

        if (this->m_model_ptr->is_linear()) {
            if (m_state.iteration == 0) {
                this->m_model_ptr->update_variable_objective_improvabilities();
            } else {
                const auto CHANGED_VARIABLE_PTRS = utility::to_vector(
                    neighborhood::related_variable_ptrs(a_CURRENT_MOVE));
                this->m_model_ptr->update_variable_objective_improvabilities(
                    CHANGED_VARIABLE_PTRS);
            }

            if (this->m_model_ptr->is_feasible()) {
                accept_all                    = false;
                accept_objective_improvable   = true;
                accept_feasibility_improvable = false;
            } else {
                this->m_model_ptr->reset_variable_feasibility_improvabilities();
                this->m_model_ptr
                    ->update_variable_feasibility_improvabilities();

                accept_all                    = false;
                accept_objective_improvable   = false;
                accept_feasibility_improvable = true;
            }
        }
        this->m_model_ptr->neighborhood().update_moves(
            accept_all,                     //
            accept_objective_improvable,    //
            accept_feasibility_improvable,  //
            this->m_option.is_enabled_parallel_neighborhood_update);

        this->m_model_ptr->neighborhood().shuffle_moves(&m_get_rand_mt);
    }

    /*************************************************************************/
    inline void print_table_header(const bool a_IS_ENABLED_PRINT) {
        if (!a_IS_ENABLED_PRINT) {
            return;
        }

        utility::print(
            "---------+------------------------+----------------------+--------"
            "--------------",
            true);
        utility::print(
            "Iteration| Number of Neighborhoods|   Current Solution   |"
            "  Incumbent Solution ",
            true);
        utility::print(
            "         |      All       checked |   Aug.Obj.(Penalty)  | "
            "  Aug.Obj.  Feas.Obj ",
            true);
        utility::print(
            "---------+------------------------+----------------------+--------"
            "--------------",
            true);
    }

    /*************************************************************************/
    inline void print_table_initial(const bool a_IS_ENABLED_PRINT) {
        if (!a_IS_ENABLED_PRINT) {
            return;
        }

        std::printf(
            " INITIAL |          -           - | %9.2e(%9.2e) | %9.2e  %9.2e\n",
            m_state.solution_score.local_augmented_objective *
                this->m_model_ptr->sign(),
            m_state.solution_score.is_feasible
                ? 0.0
                : m_state.solution_score.local_penalty,  //
            this->m_incumbent_holder_ptr
                    ->global_augmented_incumbent_objective() *
                this->m_model_ptr->sign(),
            this->m_incumbent_holder_ptr->feasible_incumbent_objective() *
                this->m_model_ptr->sign());
    }

    /*************************************************************************/
    inline void print_table_body(const bool a_IS_ENABLED_PRINT) {
        if (!a_IS_ENABLED_PRINT) {
            return;
        }

        char mark_current                    = ' ';
        char mark_global_augmented_incumbent = ' ';
        char mark_feasible_incumbent         = ' ';

        if (m_state.update_status &
            solution::IncumbentHolderConstant::
                STATUS_LOCAL_AUGMENTED_INCUMBENT_UPDATE) {
            mark_current = '!';
        }

        if (m_state.update_status &
            solution::IncumbentHolderConstant::
                STATUS_GLOBAL_AUGMENTED_INCUMBENT_UPDATE) {
            mark_current                    = '#';
            mark_global_augmented_incumbent = '#';
        }

        if (m_state.update_status &  //
            solution::IncumbentHolderConstant::
                STATUS_FEASIBLE_INCUMBENT_UPDATE) {
            mark_current                    = '*';
            mark_global_augmented_incumbent = '*';
            mark_feasible_incumbent         = '*';
        }

        std::printf(  //
            "%8d |      %5d       %5d |%c%9.2e(%9.2e) |%c%9.2e %c%9.2e\n",
            m_state.iteration,               //
            m_state.number_of_moves,         //
            m_state.number_of_checked_move,  //
            mark_current,                    //
            m_state.solution_score.local_augmented_objective *
                this->m_model_ptr->sign(),  //
            m_state.solution_score.is_feasible
                ? 0.0
                : m_state.solution_score.local_penalty,  //
            mark_global_augmented_incumbent,             //
            this->m_incumbent_holder_ptr
                    ->global_augmented_incumbent_objective() *
                this->m_model_ptr->sign(),  //
            mark_feasible_incumbent,        //
            this->m_incumbent_holder_ptr->feasible_incumbent_objective() *
                this->m_model_ptr->sign());
    }

    /*************************************************************************/
    inline void print_table_footer(const bool a_IS_ENABLED_PRINT) {
        if (!a_IS_ENABLED_PRINT) {
            return;
        }

        utility::print(
            "---------+------------------------+----------------------+--------"
            "--------------",
            true);
    }

   public:
    /*************************************************************************/
    LocalSearchCore(void) {
        this->initialize();
    }

    /*************************************************************************/
    LocalSearchCore(model::Model<T_Variable, T_Expression>* a_model_ptr,     //
                    const std::vector<multi_array::ValueProxy<T_Variable>>&  //
                        a_INITIAL_VARIABLE_VALUE_PROXIES,                    //
                    solution::IncumbentHolder<T_Variable, T_Expression>*     //
                                                      a_incumbent_holder_ptr,  //
                    Memory<T_Variable, T_Expression>* a_memory_ptr,  //
                    const option::Option&             a_OPTION) {
        this->initialize();
        this->setup(a_model_ptr,                       //
                    a_INITIAL_VARIABLE_VALUE_PROXIES,  //
                    a_incumbent_holder_ptr,            //
                    a_memory_ptr,                      //
                    a_OPTION);
    }

    /*************************************************************************/
    virtual ~LocalSearchCore(void) {
        /// nothing to do
    }

    /*************************************************************************/
    inline void initialize(void) {
        AbstractSolverCore<T_Variable, T_Expression>::initialize();
        m_state.initialize();
        m_result.initialize();
        m_get_rand_mt.seed(0);
    }

    /*************************************************************************/
    inline void run(void) {
        /**
         * Start to measure computational time.
         */
        utility::TimeKeeper time_keeper;
        time_keeper.set_start_time();

        /**
         * Copy arguments as local variables.
         */
        auto  model_ptr            = this->m_model_ptr;
        auto  memory_ptr           = this->m_memory_ptr;
        auto  incumbent_holder_ptr = this->m_incumbent_holder_ptr;
        auto  option               = this->m_option;
        auto& feasible_solutions   = this->m_feasible_solutions;
        auto& state                = m_state;

        /**
         * Preprocess.
         */
        this->preprocess();

        /**
         * Print the header of optimization progress table and print the initial
         * solution status.
         */
        utility::print_single_line(option.verbose >= option::verbose::Full);
        utility::print_message("Local search starts.",
                               option.verbose >= option::verbose::Full);
        this->print_table_header(option.verbose >= option::verbose::Full);
        this->print_table_initial(option.verbose >= option::verbose::Full);

        /**
         * Iterations start.
         */
        state.iteration = 0;
        solution::SolutionScore trial_solution_score;
        while (true) {
            /**
             *  Check the terminating condition.
             */
            const double ELAPSED_TIME = time_keeper.clock();
            if (this->satisfy_terminate_condition(ELAPSED_TIME)) {
                break;
            }

            /**
             * Update the moves.
             */
            this->update_moves(state.current_move);

            const auto& MOVE_PTRS = model_ptr->neighborhood().move_ptrs();
            state.number_of_moves = MOVE_PTRS.size();
            state.number_of_checked_move      = 0;
            state.is_found_improving_solution = false;

            /**
             * If the number of the moves is zero, the tabu search iterations
             * will be terminated.
             */
            if (state.number_of_moves == 0) {
                state.termination_status =
                    LocalSearchCoreTerminationStatus::NO_MOVE;
                break;
            }

            for (const auto& move_ptr : MOVE_PTRS) {
                /**
                 * The neighborhood solutions are evaluated in sequential by
                 * fast or ordinary(slow) evaluation methods.
                 */
#ifndef _MPS_SOLVER
                if (model_ptr->is_enabled_fast_evaluation()) {
#endif
                    if (move_ptr->is_univariable_move) {
                        model_ptr->evaluate_single(&trial_solution_score,  //
                                                   *move_ptr,              //
                                                   state.solution_score);
                    } else {
                        model_ptr->evaluate_multi(&trial_solution_score,  //
                                                  *move_ptr,              //
                                                  state.solution_score);
                    }
#ifndef _MPS_SOLVER
                } else {
                    model_ptr->evaluate(&trial_solution_score, *move_ptr);
                }
#endif
                /**
                 * Update the incumbent if the evaluated solution improves it.
                 */
                if (trial_solution_score.local_augmented_objective +
                        constant::EPSILON <
                    incumbent_holder_ptr
                        ->local_augmented_incumbent_objective()) {
                    state.solution_score              = trial_solution_score;
                    state.is_found_improving_solution = true;

                    state.previous_move = state.current_move;
                    state.current_move  = *move_ptr;
                    break;
                }

                state.number_of_checked_move++;
            }

            /**
             * The local search will be terminated if there is no improving
             * solution in the checked neighborhood.
             */
            if (!state.is_found_improving_solution) {
                state.termination_status =
                    LocalSearchCoreTerminationStatus::LOCAL_OPTIMAL;
                break;
            }

            /**
             * Update the model by the selected move.
             */
            auto move_ptr = MOVE_PTRS[state.number_of_checked_move];

            model_ptr->update(*move_ptr);
            state.update_status = incumbent_holder_ptr->try_update_incumbent(
                model_ptr, state.solution_score);
            state.total_update_status |= state.update_status;

            /**
             * Store the current feasible solution.
             */
            if (state.solution_score.is_feasible) {
                feasible_solutions.push_back(
                    model_ptr->export_plain_solution());
            }

            /**
             * Update the memory_ptr.
             */
            memory_ptr->update(*move_ptr, state.iteration);

            /**
             * Print the optimization progress.
             */
            if ((state.iteration %
                 std::max(option.local_search.log_interval, 1)) == 0 ||
                state.update_status > 1) {
                this->print_table_body(option.verbose >= option::verbose::Full);
            }
            state.iteration++;
        }

        /**
         * Print the footer of the optimization progress table.
         */
        this->print_table_footer(option.verbose >= option::verbose::Full);

        /**
         * Postprocess.
         */
        this->postprocess();
    }

    /*************************************************************************/
    inline constexpr const LocalSearchCoreResult<T_Variable, T_Expression>&
    result(void) const {
        return m_result;
    }
};
}  // namespace core
}  // namespace local_search
}  // namespace solver
}  // namespace printemps

#endif
/*****************************************************************************/
// END
/*****************************************************************************/