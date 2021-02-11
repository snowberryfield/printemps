/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/

#ifndef PRINTEMPS_UTILITY_OPTION_UTILITY_H__
#define PRINTEMPS_UTILITY_OPTION_UTILITY_H__

#include <printemps.h>
#include <nlohmann/json.hpp>

namespace printemps {
namespace utility {
/******************************************************************************/
template <class T>
bool read_json(T *a_parameter, const std::string &a_NAME,
               const nlohmann::json &a_JSON) {
    if (a_JSON.find(a_NAME) != a_JSON.end()) {
        *a_parameter = a_JSON.at(a_NAME).get<T>();
        return true;
    }
    return false;
}

/******************************************************************************/
printemps::solver::Option read_option(const std::string &a_FILE_NAME) {
    std::fstream   option_file(a_FILE_NAME);
    nlohmann::json option_object;
    option_file >> option_object;
    option_file.close();

    printemps::solver::Option option;

    /**************************************************************************/
    /// global
    /**************************************************************************/
    /// iteration_max
    read_json(&option.iteration_max,  //
              "iteration_max",        //
              option_object);

    /**************************************************************************/
    /// time_offset
    read_json(&option.time_offset,  //
              "time_offset",        //
              option_object);

    /**************************************************************************/
    /// time_max
    read_json(&option.time_max,  //
              "time_max",        //
              option_object);

    /**************************************************************************/
    /// penalty_coefficient_relaxing_rate
    read_json(&option.penalty_coefficient_relaxing_rate,  //
              "penalty_coefficient_relaxing_rate",        //
              option_object);

    /**************************************************************************/
    /// penalty_coefficient_tightening_rate
    read_json(&option.penalty_coefficient_tightening_rate,  //
              "penalty_coefficient_tightening_rate",        //
              option_object);

    /**************************************************************************/
    /// penalty_coefficient_updating_balance
    read_json(&option.penalty_coefficient_updating_balance,  //
              "penalty_coefficient_updating_balance",        //
              option_object);

    /**************************************************************************/
    /// penalty_coefficient_reset_count_threshold
    read_json(&option.penalty_coefficient_reset_count_threshold,  //
              "penalty_coefficient_reset_count_threshold",        //
              option_object);

    /**************************************************************************/
    /// initial_penalty_coefficient
    read_json(&option.initial_penalty_coefficient,  //
              "initial_penalty_coefficient",        //
              option_object);

    /**************************************************************************/
    /// is_enabled_lagrange_dual
    read_json(&option.is_enabled_lagrange_dual,  //
              "is_enabled_lagrange_dual",        //
              option_object);

    /**************************************************************************/
    /// is_enabled_local_search
    read_json(&option.is_enabled_local_search,  //
              "is_enabled_local_search",        //
              option_object);

    /**************************************************************************/
    /// is_enabled_grouping_penalty_coefficient
    read_json(&option.is_enabled_grouping_penalty_coefficient,  //
              "is_enabled_grouping_penalty_coefficient",        //
              option_object);

    /**************************************************************************/
    /// is_enabled_presolve
    read_json(&option.is_enabled_presolve,  //
              "is_enabled_presolve",        //
              option_object);

    /**************************************************************************/
    /// is_enabled_initial_value_correction
    read_json(&option.is_enabled_initial_value_correction,  //
              "is_enabled_initial_value_correction",        //
              option_object);

    /**********************************************************************/
    /// is_enabled_parallel_evaluation
    read_json(&option.is_enabled_parallel_evaluation,  //
              "is_enabled_parallel_evaluation",        //
              option_object);

    /**********************************************************************/
    /// is_enabled_parallel_neighborhood_update
    read_json(&option.is_enabled_parallel_neighborhood_update,  //
              "is_enabled_parallel_neighborhood_update",        //
              option_object);

    /**********************************************************************/
    /// is_enabled_binary_move
    read_json(&option.is_enabled_binary_move,  //
              "is_enabled_binary_move",        //
              option_object);

    /**********************************************************************/
    /// is_enabled_integer_move
    read_json(&option.is_enabled_integer_move,  //
              "is_enabled_integer_move",        //
              option_object);

    /**********************************************************************/
    /// is_enabled_aggregation_move
    read_json(&option.is_enabled_aggregation_move,  //
              "is_enabled_aggregation_move",        //
              option_object);

    /**********************************************************************/
    /// is_enabled_precedence_move
    read_json(&option.is_enabled_precedence_move,  //
              "is_enabled_precedence_move",        //
              option_object);

    /**********************************************************************/
    /// is_enabled_variable_bound_move
    read_json(&option.is_enabled_variable_bound_move,  //
              "is_enabled_variable_bound_move",        //
              option_object);

    /**********************************************************************/
    /// is_enabled_exclusive_move
    read_json(&option.is_enabled_exclusive_move,  //
              "is_enabled_exclusive_move",        //
              option_object);

    /**********************************************************************/
    /// is_enabled_chain_move
    read_json(&option.is_enabled_chain_move,  //
              "is_enabled_chain_move",        //
              option_object);

    /**********************************************************************/
    /// is_enabled_user_defined_move
    read_json(&option.is_enabled_user_defined_move,  //
              "is_enabled_user_defined_move",        //
              option_object);

    /**********************************************************************/
    /// chain_move_capacity
    read_json(&option.chain_move_capacity,  //
              "chain_move_capacity",        //
              option_object);

    /**************************************************************************/
    /// selection_mode
    read_json(&option.selection_mode,  //
              "selection_mode",        //
              option_object);

    /**************************************************************************/
    /// improvability_screening_mode
    read_json(&option.improvability_screening_mode,  //
              "improvability_screening_mode",        //
              option_object);

    /**************************************************************************/
    /// target_objective_value
    read_json(&option.target_objective_value,  //
              "target_objective_value",        //
              option_object);

    /**************************************************************************/
    /// seed
    read_json(&option.seed,  //
              "seed",        //
              option_object);

    /**************************************************************************/
    /// verbose
    read_json(&option.verbose,  //
              "verbose",        //
              option_object);

    /**************************************************************************/
    /// is_enabled_collect_historical_data
    read_json(&option.is_enabled_collect_historical_data,  //
              "is_enabled_collect_historical_data",        //
              option_object);

    /**************************************************************************/
    /// historical_data_capacity
    read_json(&option.historical_data_capacity,  //
              "historical_data_capacity",        //
              option_object);

    /**************************************************************************/
    /// lagrange dual
    /**************************************************************************/
    nlohmann::json option_object_lagrange_dual;

    bool has_lagrange_dual_option = read_json(&option_object_lagrange_dual,  //
                                              "lagrange_dual",               //
                                              option_object);

    if (has_lagrange_dual_option) {
        /**********************************************************************/
        /// lagrange_dual.iteration_max
        read_json(&option.lagrange_dual.iteration_max,  //
                  "iteration_max",                      //
                  option_object_lagrange_dual);

        /**********************************************************************/
        /// lagrange_dual.time_max
        read_json(&option.lagrange_dual.time_max,  //
                  "time_max",                      //
                  option_object_lagrange_dual);

        /**********************************************************************/
        /// lagrange_dual.time_offset
        read_json(&option.lagrange_dual.time_offset,  //
                  "time_offset",                      //
                  option_object_lagrange_dual);

        /**********************************************************************/
        /// lagrange_dual.step_size_extend_rate
        read_json(&option.lagrange_dual.step_size_extend_rate,  //
                  "step_size_extend_rate",                      //
                  option_object_lagrange_dual);

        /**********************************************************************/
        /// lagrange_dual.step_size_reduce_rate
        read_json(&option.lagrange_dual.step_size_reduce_rate,  //
                  "step_size_reduce_rate",                      //
                  option_object_lagrange_dual);

        /**********************************************************************/
        /// lagrange_dual.tolerance
        read_json(&option.lagrange_dual.tolerance,  //
                  "tolerance",                      //
                  option_object_lagrange_dual);

        /**********************************************************************/
        /// lagrange_dual.queue_size
        read_json(&option.lagrange_dual.queue_size,  //
                  "queue_size",                      //
                  option_object_lagrange_dual);

        /**********************************************************************/
        /// lagrange_dual.log_interval
        read_json(&option.lagrange_dual.log_interval,  //
                  "log_interval",                      //
                  option_object_lagrange_dual);
    }

    /**************************************************************************/
    /// local search
    /**************************************************************************/
    nlohmann::json option_object_local_search;

    bool has_local_search_option = read_json(&option_object_local_search,  //
                                             "local_search",               //
                                             option_object);

    if (has_local_search_option) {
        /**********************************************************************/
        /// local_search.iteration_max
        read_json(&option.local_search.iteration_max,  //
                  "iteration_max",                     //
                  option_object_local_search);

        /**********************************************************************/
        /// local_search.time_max
        read_json(&option.local_search.time_max,  //
                  "time_max",                     //
                  option_object_local_search);

        /**********************************************************************/
        /// local_search.time_offset
        read_json(&option.local_search.time_offset,  //
                  "time_offset",                     //
                  option_object_local_search);

        /**********************************************************************/
        /// local_search.log_interval
        read_json(&option.local_search.log_interval,  //
                  "log_interval",                     //
                  option_object_local_search);

        /**********************************************************************/
        /// local_search.seed
        read_json(&option.local_search.seed,  //
                  "seed",                     //
                  option_object_local_search);
    }

    /**************************************************************************/
    /// tabu search
    /**************************************************************************/
    nlohmann::json option_object_tabu_search;

    bool has_tabu_search_option = read_json(&option_object_tabu_search,  //
                                            "tabu_search",               //
                                            option_object);

    if (has_tabu_search_option) {
        /**********************************************************************/
        /// tabu_search.iteration_max
        read_json(&option.tabu_search.iteration_max,  //
                  "iteration_max",                    //
                  option_object_tabu_search);

        /**********************************************************************/
        /// tabu_search.time_max
        read_json(&option.tabu_search.time_max,  //
                  "time_max",                    //
                  option_object_tabu_search);

        /**********************************************************************/
        /// tabu_search.time_offset
        read_json(&option.tabu_search.time_offset,  //
                  "time_offset",                    //
                  option_object_tabu_search);

        /**********************************************************************/
        /// tabu_search.log_interval
        read_json(&option.tabu_search.log_interval,  //
                  "log_interval",                    //
                  option_object_tabu_search);

        /**********************************************************************/
        /// tabu_search.initial_tabu_tenure
        read_json(&option.tabu_search.initial_tabu_tenure,  //
                  "initial_tabu_tenure",                    //
                  option_object_tabu_search);

        /**********************************************************************/
        /// tabu_search.tabu_tenure_randomize_rate
        read_json(&option.tabu_search.tabu_tenure_randomize_rate,  //
                  "tabu_tenure_randomize_rate",                    //
                  option_object_tabu_search);

        /**********************************************************************/
        /// tabu_search.initial_modification_fixed_rate
        read_json(&option.tabu_search.initial_modification_fixed_rate,  //
                  "initial_modification_fixed_rate",                    //
                  option_object_tabu_search);

        /**********************************************************************/
        /// tabu_search.initial_modification_randomize_rate
        read_json(&option.tabu_search.initial_modification_randomize_rate,  //
                  "initial_modification_randomize_rate",                    //
                  option_object_tabu_search);

        /**********************************************************************/
        /// tabu_search.tabu_mode
        read_json(&option.tabu_search.tabu_mode,  //
                  "tabu_mode",                    //
                  option_object_tabu_search);

        /**********************************************************************/
        /// tabu_search.move_preserve_rate
        read_json(&option.tabu_search.move_preserve_rate,  //
                  "move_preserve_rate",                    //
                  option_object_tabu_search);

        /**********************************************************************/
        /// tabu_search.frequency_penalty_coefficient
        read_json(&option.tabu_search.frequency_penalty_coefficient,  //
                  "frequency_penalty_coefficient",                    //
                  option_object_tabu_search);

        /**********************************************************************/
        /// tabu_search.pruning_rate_threshold
        read_json(&option.tabu_search.pruning_rate_threshold,  //
                  "pruning_rate_threshold",                    //
                  option_object_tabu_search);

        /**********************************************************************/
        /// tabu_search.is_enabled_shuffle
        read_json(&option.tabu_search.is_enabled_shuffle,  //
                  "is_enabled_shuffle",                    //
                  option_object_tabu_search);

        /**********************************************************************/
        /// tabu_search.is_enabled_move_curtail
        read_json(&option.tabu_search.is_enabled_move_curtail,  //
                  "is_enabled_move_curtail",                    //
                  option_object_tabu_search);

        /**********************************************************************/
        /// tabu_search.is_enabled_automatic_break
        read_json(&option.tabu_search.is_enabled_automatic_break,  //
                  "is_enabled_automatic_break",                    //
                  option_object_tabu_search);

        /**********************************************************************/
        /// tabu_search.is_enabled_automatic_tabu_tenure_adjustment
        read_json(
            &option.tabu_search.is_enabled_automatic_tabu_tenure_adjustment,  //
            "is_enabled_automatic_tabu_tenure_adjustment",                    //
            option_object_tabu_search);

        /**********************************************************************/
        /// tabu_search.is_enabled_automatic_iteration_adjustment
        read_json(
            &option.tabu_search.is_enabled_automatic_iteration_adjustment,  //
            "is_enabled_automatic_iteration_adjustment",                    //
            option_object_tabu_search);

        /**********************************************************************/
        /// tabu_search.is_enabled_initial_modification
        read_json(&option.tabu_search.is_enabled_initial_modification,  //
                  "is_enabled_initial_modification",                    //
                  option_object_tabu_search);

        /**********************************************************************/
        /// tabu_search.bias_increase_count_threshold
        read_json(&option.tabu_search.bias_increase_count_threshold,  //
                  "bias_increase_count_threshold",                    //
                  option_object_tabu_search);

        /**********************************************************************/
        /// tabu_search.bias_decrease_count_threshold
        read_json(&option.tabu_search.bias_decrease_count_threshold,  //
                  "bias_decrease_count_threshold",                    //
                  option_object_tabu_search);

        /**********************************************************************/
        /// tabu_search.iteration_increase_rate
        read_json(&option.tabu_search.iteration_increase_rate,  //
                  "iteration_increase_rate",                    //
                  option_object_tabu_search);

        /**********************************************************************/
        /// tabu_search.iteration_decrease_rate
        read_json(&option.tabu_search.iteration_decrease_rate,  //
                  "iteration_decrease_rate",                    //
                  option_object_tabu_search);

        /**********************************************************************/
        /// tabu_search.ignore_tabu_if_global_incumbent
        read_json(&option.tabu_search.ignore_tabu_if_global_incumbent,  //
                  "ignore_tabu_if_global_incumbent",                    //
                  option_object_tabu_search);

        /**********************************************************************/
        /// tabu_search.number_of_initial_modification
        read_json(&option.tabu_search.number_of_initial_modification,  //
                  "number_of_initial_modification",                    //
                  option_object_tabu_search);

        /**********************************************************************/
        /// tabu_search.seed
        read_json(&option.tabu_search.seed,  //
                  "seed",                    //
                  option_object_tabu_search);
    }

    return option;
}
}  // namespace utility
}  // namespace printemps

/******************************************************************************/
// END
/******************************************************************************/
#endif