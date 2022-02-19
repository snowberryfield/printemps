/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_LOCAL_SEARCH_CORE_LOCAL_SEARCH_H__
#define PRINTEMPS_SOLVER_LOCAL_SEARCH_CORE_LOCAL_SEARCH_H__

#include "local_search_print.h"
#include "local_search_termination_status.h"
#include "local_search_result.h"

namespace printemps {
namespace solver {
namespace local_search {
namespace core {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
LocalSearchResult solve(
    model::Model<T_Variable, T_Expression>*              a_model_ptr,         //
    solution::IncumbentHolder<T_Variable, T_Expression>* a_incumbent_holder,  //
    Memory*                                              a_memory_ptr,        //
    std::vector<solution::SparseSolution<T_Variable, T_Expression>>*
                          a_feasible_solutions_ptr,          //
    const option::Option& a_OPTION,                          //
    const std::vector<multi_array::ValueProxy<T_Variable>>&  //
        a_INITIAL_VARIABLE_VALUE_PROXIES) {
    /**
     * Define type aliases.
     */
    using Model_T = model::Model<T_Variable, T_Expression>;
    using IncumbentHolder_T =
        solution::IncumbentHolder<T_Variable, T_Expression>;
    using Move_T = neighborhood::Move<T_Variable, T_Expression>;

    /**
     * Start to measure computational time.
     */
    utility::TimeKeeper time_keeper;
    time_keeper.set_start_time();

    /**
     * Copy arguments as local variables.
     */
    Model_T*           model_ptr            = a_model_ptr;
    Memory*            memory_ptr           = a_memory_ptr;
    IncumbentHolder_T* incumbent_holder_ptr = a_incumbent_holder;
    option::Option     option               = a_OPTION;

    /**
     * Reset the local augmented incumbent.
     */
    incumbent_holder_ptr->reset_local_augmented_incumbent();

    /**
     * Reset the feasible solutions storage.
     */
    a_feasible_solutions_ptr->clear();

    /**
     * Prepare a random generator, which is used for shuffling moves.
     */
    std::mt19937 get_rand_mt(option.local_search.seed);

    /**
     * Initialize the solution and update the model.
     */
    model_ptr->import_variable_values(a_INITIAL_VARIABLE_VALUE_PROXIES);
    model_ptr->update();

    solution::SolutionScore solution_score = model_ptr->evaluate({});

    int update_status =
        incumbent_holder_ptr->try_update_incumbent(model_ptr, solution_score);
    int total_update_status =
        solution::IncumbentHolderConstant::STATUS_NO_UPDATED;

    /**
     * Reset the last update iterations.
     */
    memory_ptr->reset_last_update_iterations();

    /**
     * Reset the variable improvability.
     */
    model_ptr->reset_variable_objective_improvabilities();
    model_ptr->reset_variable_feasibility_improvabilities();

    /**
     * Prepare other local variables.
     */

    auto termination_status = LocalSearchTerminationStatus::ITERATION_OVER;

    neighborhood::Move<T_Variable, T_Expression> previous_move;
    neighborhood::Move<T_Variable, T_Expression> current_move;

    /**
     * Print the header of optimization progress table and print the initial
     * solution status.
     */
    utility::print_single_line(option.verbose >= option::verbose::Full);
    utility::print_message("Local search starts.",
                           option.verbose >= option::verbose::Full);
    print_table_header(option.verbose >= option::verbose::Full);
    print_table_initial(model_ptr,             //
                        solution_score,        //
                        incumbent_holder_ptr,  //
                        option.verbose >= option::verbose::Full);

    /**
     * Iterations start.
     */
    int iteration = 0;

    while (true) {
        /**
         *  Check the terminating condition.
         */
        double elapsed_time = time_keeper.clock();
        if (elapsed_time > option.local_search.time_max) {
            termination_status = LocalSearchTerminationStatus::TIME_OVER;
            break;
        }
        if (elapsed_time + option.local_search.time_offset > option.time_max) {
            termination_status = LocalSearchTerminationStatus::TIME_OVER;
            break;
        }
        if (iteration >= option.local_search.iteration_max) {
            termination_status = LocalSearchTerminationStatus::ITERATION_OVER;
            break;
        }
        if (incumbent_holder_ptr->feasible_incumbent_objective() <=
            option.target_objective_value) {
            termination_status = LocalSearchTerminationStatus::REACH_TARGET;
            break;
        }

        /**
         * Update the moves.
         */
        bool accept_all                    = true;
        bool accept_objective_improvable   = true;
        bool accept_feasibility_improvable = true;

        if (model_ptr->is_linear()) {
            auto changed_variable_ptrs = utility::to_vector(
                neighborhood::related_variable_ptrs(current_move));
            auto changed_constraint_ptrs =
                utility::to_vector(current_move.related_constraint_ptrs);

            if (iteration == 0) {
                model_ptr->update_variable_objective_improvabilities();
            } else {
                model_ptr->update_variable_objective_improvabilities(
                    changed_variable_ptrs);
            }

            if (model_ptr->is_feasible()) {
                accept_all                    = false;
                accept_objective_improvable   = true;
                accept_feasibility_improvable = false;
            } else {
                model_ptr->reset_variable_feasibility_improvabilities();
                model_ptr->update_variable_feasibility_improvabilities();

                accept_all                    = false;
                accept_objective_improvable   = false;
                accept_feasibility_improvable = true;
            }
        }
        model_ptr->neighborhood().update_moves(
            accept_all,                     //
            accept_objective_improvable,    //
            accept_feasibility_improvable,  //
            option.is_enabled_parallel_neighborhood_update);

        model_ptr->neighborhood().shuffle_moves(&get_rand_mt);

        bool is_found_improving_solution = false;

        const auto& move_ptrs = model_ptr->neighborhood().move_ptrs();

        int number_of_moves        = move_ptrs.size();
        int number_of_checked_move = 0;

        /**
         * If the number of the moves is zero, the tabu search iterations will
         * be terminated.
         */
        if (number_of_moves == 0) {
            termination_status = LocalSearchTerminationStatus::NO_MOVE;
            break;
        }

        for (const auto& move_ptr : move_ptrs) {
            solution::SolutionScore trial_solution_score;
            /**
             * The neighborhood solutions are evaluated in sequential by fast or
             * ordinary(slow) evaluation methods.
             */
#ifndef _MPS_SOLVER
            if (model_ptr->is_enabled_fast_evaluation()) {
#endif
                if (move_ptr->is_univariable_move) {
                    model_ptr->evaluate_single(&trial_solution_score,  //
                                               *move_ptr,              //
                                               solution_score);
                } else {
                    model_ptr->evaluate_multi(&trial_solution_score,  //
                                              *move_ptr,              //
                                              solution_score);
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
                incumbent_holder_ptr->local_augmented_incumbent_objective()) {
                solution_score              = trial_solution_score;
                is_found_improving_solution = true;

                previous_move = current_move;
                current_move  = *move_ptr;
                break;
            }

            number_of_checked_move++;
        }

        /**
         * The local search will be terminated if there is no improving solution
         * in the checked neighborhood.
         */
        if (!is_found_improving_solution) {
            termination_status = LocalSearchTerminationStatus::LOCAL_OPTIMAL;
            break;
        }

        /**
         * Update the model by the selected move.
         */
        Move_T* move_ptr = move_ptrs[number_of_checked_move];

        model_ptr->update(*move_ptr);
        update_status = incumbent_holder_ptr->try_update_incumbent(
            model_ptr, solution_score);
        total_update_status = update_status || total_update_status;

        /**
         * Store the current feasible solution.
         */
        if (solution_score.is_feasible) {
            a_feasible_solutions_ptr->push_back(
                model_ptr->export_plain_solution());
        }

        /**
         * Update the memory_ptr.
         */
        memory_ptr->update(*move_ptr, iteration);

        /**
         * Print the optimization progress.
         */
        if (iteration % std::max(option.local_search.log_interval, 1) == 0 ||
            update_status > 1) {
            print_table_body(model_ptr,               //
                             iteration,               //
                             number_of_moves,         //
                             number_of_checked_move,  //
                             solution_score,          //
                             update_status,           //
                             incumbent_holder_ptr,    //
                             option.verbose >= option::verbose::Full);
        }
        iteration++;
    }

    /**
     * Print the footer of the optimization progress table.
     */
    print_table_footer(option.verbose >= option::verbose::Full);

    /**
     * Prepare the result.
     */
    LocalSearchResult result(total_update_status,  //
                             iteration,            //
                             termination_status);

    return result;
}
}  // namespace core
}  // namespace local_search
}  // namespace solver
}  // namespace printemps

#endif
/*****************************************************************************/
// END
/*****************************************************************************/