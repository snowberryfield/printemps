/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/

#ifndef CPPMH_UTILITY_OPTION_UTILITY_H__
#define CPPMH_UTILITY_OPTION_UTILITY_H__

#include <cppmh.h>
#include <nlohmann/json.hpp>

namespace cppmh {
namespace utility {
/******************************************************************************/
template <class T>
inline bool read_json(T *a_parameter, const std::string &a_NAME,
                      const nlohmann::json &a_JSON) {
    if (a_JSON.find(a_NAME) != a_JSON.end()) {
        *a_parameter = a_JSON.at(a_NAME).get<T>();
        return true;
    }
    return false;
}

/******************************************************************************/
inline cppmh::solver::Option read_option(const std::string &a_FILE_NAME) {
    std::fstream   option_file(a_FILE_NAME);
    nlohmann::json option_object;
    option_file >> option_object;
    option_file.close();

    cppmh::solver::Option option;

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
    /// penalty_coefficient_relaxing_ratio
    read_json(&option.penalty_coefficient_relaxing_ratio,  //
              "penalty_coefficient_relaxing_ratio",        //
              option_object);

    /**************************************************************************/
    /// penalty_coefficient_tightening_ratio
    read_json(&option.penalty_coefficient_tightening_ratio,  //
              "penalty_coefficient_tightening_ratio",        //
              option_object);

    /**************************************************************************/
    /// initial_penalty_coefficient
    read_json(&option.initial_penalty_coefficient,  //
              "initial_penalty_coefficient",        //
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

    /**************************************************************************/
    /// target_objective_value
    read_json(&option.target_objective_value,  //
              "target_objective_value",        //
              option_object);

    /**************************************************************************/
    /// verbose
    read_json(&option.verbose,  //
              "verbose",        //
              option_object);

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
        /// tabu_search.tabu_tenure
        read_json(&option.tabu_search.initial_tabu_tenure,  //
                  "initial_tabu_tenure",                    //
                  option_object_tabu_search);

        /**********************************************************************/
        /// tabu_search.tabu_mode
        read_json(&option.tabu_search.tabu_mode,  //
                  "tabu_tenure",                  //
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
        /// tabu_search.is_enabled_improvability_screening
        read_json(&option.tabu_search.is_enabled_improvability_screening,  //
                  "is_enabled_improvability_screening",                    //
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
        /// tabu_search.ignore_tabu_if_augmented_incumbent
        read_json(&option.tabu_search.ignore_tabu_if_augmented_incumbent,  //
                  "ignore_tabu_if_augmented_incumbent",                    //
                  option_object_tabu_search);

        /**********************************************************************/
        /// tabu_search.ignore_tabu_if_feasible_incumbent
        read_json(&option.tabu_search.ignore_tabu_if_feasible_incumbent,  //
                  "ignore_tabu_if_feasible_incumbent",                    //
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
}  // namespace cppmh

/******************************************************************************/
// END
/******************************************************************************/
#endif