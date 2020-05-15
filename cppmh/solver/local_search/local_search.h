/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef CPPMH_SOLVER_LOCAL_SEARCH_LOCAL_SEARCH_H__
#define CPPMH_SOLVER_LOCAL_SEARCH_LOCAL_SEARCH_H__

#include "local_search_option.h"
#include "local_search_print.h"
#include "local_search_result.h"

namespace cppmh {
namespace solver {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class IncumbentHolder;

/*****************************************************************************/
template <class T_Variable, class T_Expression>
class Result;

/*****************************************************************************/
struct Option;

namespace local_search {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
LocalSearchResult<T_Variable, T_Expression> solve(
    model::Model<T_Variable, T_Expression>* a_model,   //
    const Option&                           a_OPTION,  //
    const std::vector<model::ValueProxy<double>>&      //
        a_LOCAL_PENALTY_COEFFICIENT_PROXIES,           //
    const std::vector<model::ValueProxy<double>>&      //
        a_GLOBAL_PENALTY_COEFFICIENT_PROXIES,          //
    const std::vector<model::ValueProxy<T_Variable>>&  //
        a_INITIAL_VARIABLE_VALUE_PROXIES,              //
    const IncumbentHolder<T_Variable, T_Expression>&   //
        a_INCUMBENT_HOLDER) {
    /**
     * Define type aliases.
     */
    using Model_T          = model::Model<T_Variable, T_Expression>;
    using Result_T         = LocalSearchResult<T_Variable, T_Expression>;
    using IncumbetHolder_T = IncumbentHolder<T_Variable, T_Expression>;

    /**
     * Start to measure computational time.
     */
    utility::TimeKeeper time_keeper;

    /**
     * Copy arguments as local variables.
     */
    Model_T* model  = a_model;
    Option   option = a_OPTION;

    std::vector<model::ValueProxy<double>> local_penalty_coefficient_proxies =
        a_LOCAL_PENALTY_COEFFICIENT_PROXIES;
    std::vector<model::ValueProxy<double>> global_penalty_coefficient_proxies =
        a_GLOBAL_PENALTY_COEFFICIENT_PROXIES;

    IncumbetHolder_T incumbent_holder = a_INCUMBENT_HOLDER;

    /**
     * Reset the local augmented incumbent.
     */
    incumbent_holder.reset_local_augmented_incumbent();

    /**
     * Prepare a random generator, which is used for shuffling moves.
     */
    std::mt19937 get_rand_mt(option.local_search.seed);

    model->import_variable_values(a_INITIAL_VARIABLE_VALUE_PROXIES);
    model->update();

    model::SolutionScore solution_score =
        model->evaluate({},                                 //
                        local_penalty_coefficient_proxies,  //
                        global_penalty_coefficient_proxies);

    int update_status =
        incumbent_holder.try_update_incumbent(model, solution_score);
    int total_update_status = 0;

    utility::print_single_line(option.verbose >= Verbose::Full);
    utility::print_message("Local search starts.",
                           option.verbose >= Verbose::Full);
    print_table_header(option.verbose >= Verbose::Full);
    print_table_initial(model, solution_score, incumbent_holder,
                        option.verbose >= Verbose::Full);

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
            break;
        }
        if (elapsed_time + option.local_search.time_offset > option.time_max) {
            break;
        }
        if (iteration >= option.local_search.iteration_max) {
            break;
        }
        if (incumbent_holder.feasible_incumbent_objective() <=
            option.target_objective_value) {
            break;
        }

        model->neighborhood().update_moves();
        model->neighborhood().shuffle_moves(&get_rand_mt);

        bool is_found_improving_solution = false;

        const auto& move_ptrs = model->neighborhood().move_ptrs();

        int number_of_moves         = move_ptrs.size();
        int number_of_checked_moved = 0;

        if (number_of_moves == 0) {
            break;
        }

        for (const auto& move_ptr : move_ptrs) {
            auto trial_solution_score = model->evaluate(
                *move_ptr, solution_score, local_penalty_coefficient_proxies,
                global_penalty_coefficient_proxies);
            number_of_checked_moved++;
            if (trial_solution_score.local_augmented_objective <
                incumbent_holder.local_augmented_incumbent_objective()) {
                solution_score = trial_solution_score;
                model->update(*move_ptr);

                is_found_improving_solution = true;
                break;
            }
        }
        if (!is_found_improving_solution) {
            break;
        }

        update_status =
            incumbent_holder.try_update_incumbent(model, solution_score);
        total_update_status = update_status || total_update_status;

        if (iteration % std::max(option.local_search.log_interval, 1) == 0 ||
            update_status > 1) {
            print_table_body(model,                    //
                             iteration,                //
                             number_of_moves,          //
                             number_of_checked_moved,  //
                             solution_score,           //
                             update_status,            //
                             incumbent_holder,         //
                             option.verbose >= Verbose::Full);
        }
        iteration++;
    }

    print_table_footer(option.verbose >= Verbose::Full);

    Result_T result;
    result.incumbent_holder    = incumbent_holder;
    result.total_update_status = total_update_status;
    return result;
}
}  // namespace local_search
}  // namespace solver
}  // namespace cppmh

#endif
/*****************************************************************************/
// END
/*****************************************************************************/