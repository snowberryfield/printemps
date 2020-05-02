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
    if (read_json(&option.iteration_max,  //
                  "iteration_max",        //
                  option_object)) {
        print_message(  //
            "Option iteration_max is set to " +
                to_string(option.iteration_max, "%d."),
            true);
    }

    /**************************************************************************/
    /// time_offset
    if (read_json(&option.time_offset,  //
                  "time_offset",        //
                  option_object)) {
        print_message(  //
            "Option time_offset is set to " +
                to_string(option.time_offset, "%f."),
            true);
    }

    /**************************************************************************/
    /// time_max
    if (read_json(&option.time_max,  //
                  "time_max",        //
                  option_object)) {
        print_message(  //
            "Option time_max is set to " + to_string(option.time_max, "%f."),
            true);
    }

    /**************************************************************************/
    /// penalty_coefficient_relaxing_ratio
    if (read_json(&option.penalty_coefficient_relaxing_ratio,  //
                  "penalty_coefficient_relaxing_ratio",        //
                  option_object)) {
        print_message(  //
            "Option penalty_coefficient_relaxing_ratio is set to " +
                to_string(option.penalty_coefficient_relaxing_ratio, "%f."),
            true);
    }

    /**************************************************************************/
    /// penalty_coefficient_tightening_ratio
    if (read_json(&option.penalty_coefficient_tightening_ratio,  //
                  "penalty_coefficient_tightening_ratio",        //
                  option_object)) {
        print_message(  //
            "Option penalty_coefficient_tightening_ratio is set to " +
                to_string(option.penalty_coefficient_tightening_ratio, "%f."),
            true);
    }

    /**************************************************************************/
    /// initial_penalty_coefficient
    if (read_json(&option.initial_penalty_coefficient,  //
                  "initial_penalty_coefficient",        //
                  option_object)) {
        print_message(  //
            "Option initial_penalty_coefficient is set to " +
                to_string(option.initial_penalty_coefficient, "%f."),
            true);
    }

    /**************************************************************************/
    /// is_enabled_local_search
    if (read_json(&option.is_enabled_local_search,  //
                  "is_enabled_local_search",        //
                  option_object)) {
        print_message(  //
            "Option is_enabled_local_search is set to " +
                to_string(option.is_enabled_local_search, "%d."),
            true);
    }

    /**************************************************************************/
    /// is_enabled_grouping_penalty_coefficient
    if (read_json(&option.is_enabled_grouping_penalty_coefficient,  //
                  "is_enabled_grouping_penalty_coefficient",        //
                  option_object)) {
        print_message(  //
            "Option is_enabled_grouping_penalty_coefficient is set to " +
                to_string(option.is_enabled_grouping_penalty_coefficient,
                          "%d."),
            true);
    }

    /**************************************************************************/
    /// is_enabled_initial_value_correction
    if (read_json(&option.is_enabled_initial_value_correction,  //
                  "is_enabled_initial_value_correction",        //
                  option_object)) {
        print_message(  //
            "Option is_enabled_initial_value_correction is set to " +
                to_string(option.is_enabled_initial_value_correction, "%d."),
            true);
    }

    /**********************************************************************/
    /// is_enabled_parallel_evaluation
    if (read_json(&option.is_enabled_parallel_evaluation,  //
                  "is_enabled_parallel_evaluation",        //
                  option_object)) {
        print_message(  //
            "Option is_enabled_parallel_evaluation is set to " +
                to_string(option.is_enabled_parallel_evaluation, "%d."),
            true);
    }

    /**********************************************************************/
    /// is_enabled_parallel_neighborhood_update
    if (read_json(&option.is_enabled_parallel_neighborhood_update,  //
                  "is_enabled_parallel_neighborhood_update",        //
                  option_object)) {
        print_message(  //
            "Option is_enabled_parallel_neighborhood_update is set to " +
                to_string(option.is_enabled_parallel_neighborhood_update,
                          "%d."),
            true);
    }

    /**************************************************************************/
    /// target_objective_value
    if (read_json(&option.target_objective_value,  //
                  "target_objective_value",        //
                  option_object)) {
        print_message(  //
            "Option target_objective_value is set to " +
                to_string(option.target_objective_value, "%.3e."),
            true);
    }

    /**************************************************************************/
    /// verbose
    if (read_json(&option.verbose,  //
                  "verbose",        //
                  option_object)) {
        print_message(  //
            "Option verbose is set to " + to_string(option.verbose, "d."),
            true);
    }

    /**************************************************************************/
    /// local search
    /**************************************************************************/
    nlohmann::json option_object_local_search;

    bool has_local_search_option =
        read_json(&option_object_local_search, "local_search", option_object);

    if (has_local_search_option) {
        /**********************************************************************/
        /// local_search.iteration_max
        if (read_json(&option.local_search.iteration_max,  //
                      "iteration_max",                     //
                      option_object_local_search)) {
            print_message(  //
                "Option local_search.iteration_max is set to " +
                    to_string(option.local_search.iteration_max, "%d."),
                true);
        }

        /**********************************************************************/
        /// local_search.time_max
        if (read_json(&option.local_search.time_max,  //
                      "time_max",                     //
                      option_object_local_search)) {
            print_message(  //
                "Option local_search.time_max is set to " +
                    to_string(option.local_search.time_max, "%f."),
                true);
        }

        /**********************************************************************/
        /// local_search.time_offset
        if (read_json(&option.local_search.time_offset,  //
                      "time_offset",                     //
                      option_object_local_search)) {
            print_message(  //
                "Option local_search.time_offset is set to " +
                    to_string(option.local_search.time_offset, "%f."),
                true);
        }

        /**********************************************************************/
        /// local_search.log_interval
        if (read_json(&option.local_search.log_interval,  //
                      "log_interval",                     //
                      option_object_local_search)) {
            print_message(  //
                "Option local_search.log_interval is set to " +
                    to_string(option.local_search.log_interval, "%d."),
                true);
        }

        /**********************************************************************/
        /// local_search.seed
        if (read_json(&option.local_search.seed,  //
                      "seed",                     //
                      option_object)) {
            print_message(  //
                "Option local_search.seed is set to " +
                    to_string(option.local_search.seed, "%d."),
                true);
        }
    }

    /**************************************************************************/
    /// tabu search
    /**************************************************************************/
    nlohmann::json option_object_tabu_search;

    bool has_tabu_search_option =
        read_json(&option_object_tabu_search, "tabu_search", option_object);

    if (has_tabu_search_option) {
        /**********************************************************************/
        /// tabu_search.iteration_max
        if (read_json(&option.tabu_search.iteration_max,  //
                      "iteration_max",                    //
                      option_object_tabu_search)) {
            print_message(  //
                "Option tabu_search.iteration_max is set to " +
                    to_string(option.tabu_search.iteration_max, "%d."),
                true);
        }

        /**********************************************************************/
        /// tabu_search.time_max
        if (read_json(&option.tabu_search.time_max,  //
                      "time_max",                    //
                      option_object_tabu_search)) {
            print_message(  //
                "Option tabu_search.time_max is set to " +
                    to_string(option.tabu_search.time_max, "%f."),
                true);
        }

        /**********************************************************************/
        /// tabu_search.time_offset
        if (read_json(&option.tabu_search.time_offset,  //
                      "time_offset",                    //
                      option_object_tabu_search)) {
            print_message(  //
                "Option tabu_search.time_offset is set to " +
                    to_string(option.tabu_search.time_offset, "%f."),
                true);
        }

        /**********************************************************************/
        /// tabu_search.log_interval
        if (read_json(&option.tabu_search.log_interval,  //
                      "log_interval",                    //
                      option_object_tabu_search)) {
            print_message(  //
                "Option tabu_search.log_interval is set to " +
                    to_string(option.tabu_search.log_interval, "%d."),
                true);
        }

        /**********************************************************************/
        /// tabu_search.tabu_tenure
        if (read_json(&option.tabu_search.initial_tabu_tenure,  //
                      "initial_tabu_tenure",                    //
                      option_object_tabu_search)) {
            print_message(  //
                "Option tabu_search.initial_tabu_tenure is set to " +
                    to_string(option.tabu_search.initial_tabu_tenure, "%d."),
                true);
        }

        /**********************************************************************/
        /// tabu_search.tabu_mode
        if (read_json(&option.tabu_search.tabu_mode,  //
                      "tabu_tenure",                  //
                      option_object_tabu_search)) {
            print_message(  //
                "Option tabu_search.tabu_mode is set to " +
                    to_string(option.tabu_search.tabu_mode, "%d."),
                true);
        }

        /**********************************************************************/
        /// tabu_search.move_preserve_rate
        if (read_json(&option.tabu_search.move_preserve_rate,  //
                      "move_preserve_rate",                    //
                      option_object_tabu_search)) {
            print_message(  //
                "Option tabu_search.move_preserve_rate is set to " +
                    to_string(option.tabu_search.move_preserve_rate, "%f."),
                true);
        }

        /**********************************************************************/
        /// tabu_search.frequency_penalty_coefficient
        if (read_json(&option.tabu_search.frequency_penalty_coefficient,  //
                      "frequency_penalty_coefficient",                    //
                      option_object_tabu_search)) {
            print_message(  //
                "Option tabu_search.frequency_penalty_coefficient is set to " +
                    to_string(option.tabu_search.frequency_penalty_coefficient,
                              "%f."),
                true);
        }

        /**********************************************************************/
        /// tabu_search.is_enabled_improvability_screening
        if (read_json(
                &option.tabu_search.is_enabled_improvability_screening,  //
                "is_enabled_improvability_screening",                    //
                option_object_tabu_search)) {
            print_message(  //
                "Option tabu_search.is_enabled_improvability_screening is set "
                "to " +
                    to_string(
                        option.tabu_search.is_enabled_improvability_screening,
                        "%d."),
                true);
        }

        /**********************************************************************/
        /// tabu_search.is_enabled_shuffle
        if (read_json(&option.tabu_search.is_enabled_shuffle,  //
                      "is_enabled_shuffle",                    //
                      option_object_tabu_search)) {
            print_message(  //
                "Option tabu_search.is_enabled_shuffle is set to " +
                    to_string(option.tabu_search.is_enabled_shuffle, "%d."),
                true);
        }

        /**********************************************************************/
        /// tabu_search.is_enabled_move_curtail
        if (read_json(&option.tabu_search.is_enabled_move_curtail,  //
                      "is_enabled_move_curtail",                    //
                      option_object_tabu_search)) {
            print_message(  //
                "Option tabu_search.is_enabled_move_curtail is set to " +
                    to_string(option.tabu_search.is_enabled_move_curtail,
                              "%d."),
                true);
        }

        /**********************************************************************/
        /// tabu_search.is_enabled_automatic_break
        if (read_json(&option.tabu_search.is_enabled_automatic_break,  //
                      "is_enabled_automatic_break",                    //
                      option_object_tabu_search)) {
            print_message(  //
                "Option tabu_search.is_enabled_automatic_break is set "
                "to " +
                    to_string(option.tabu_search.is_enabled_automatic_break,
                              "%d."),
                true);
        }

        /**********************************************************************/
        /// tabu_search.is_enabled_automatic_tabu_tenure_adjustment
        if (read_json(&option.tabu_search
                           .is_enabled_automatic_tabu_tenure_adjustment,  //
                      "is_enabled_automatic_tabu_tenure_adjustment",      //
                      option_object_tabu_search)) {
            print_message(  //
                "Option "
                "tabu_search.is_enabled_automatic_tabu_tenure_adjustment is "
                "set to " +
                    to_string(option.tabu_search
                                  .is_enabled_automatic_tabu_tenure_adjustment,
                              "%d."),
                true);
        }

        /**********************************************************************/
        /// tabu_search.ignore_tabu_if_augmented_incumbent
        if (read_json(
                &option.tabu_search.ignore_tabu_if_augmented_incumbent,  //
                "ignore_tabu_if_augmented_incumbent",                    //
                option_object_tabu_search)) {
            print_message(  //
                "Option tabu_search.ignore_tabu_if_augmented_incumbent is set "
                "to " +
                    to_string(
                        option.tabu_search.ignore_tabu_if_augmented_incumbent,
                        "%d."),
                true);
        }

        /**********************************************************************/
        /// tabu_search.ignore_tabu_if_feasible_incumbent
        if (read_json(&option.tabu_search.ignore_tabu_if_feasible_incumbent,  //
                      "ignore_tabu_if_feasible_incumbent",                    //
                      option_object_tabu_search)) {
            print_message(  //
                "Option tabu_search.ignore_tabu_if_feasible_incumbent is set "
                "to " +
                    to_string(
                        option.tabu_search.ignore_tabu_if_feasible_incumbent,
                        "%d."),
                true);
        }

        /**********************************************************************/
        /// tabu_search.number_of_initial_modification
        if (read_json(&option.tabu_search.number_of_initial_modification,  //
                      "number_of_initial_modification",                    //
                      option_object_tabu_search)) {
            print_message(  //
                "Option tabu_search.number_of_initial_modification is set to " +
                    to_string(option.tabu_search.number_of_initial_modification,
                              "%d."),
                true);
        }

        /**********************************************************************/
        /// tabu_search.seed
        if (read_json(&option.tabu_search.seed,  //
                      "seed",                    //
                      option_object)) {
            print_message(  //
                "Option tabu_search.seed is set to" +
                    to_string(option.tabu_search.seed, "%d."),
                true);
        }
    }

    return option;
}
}  // namespace utility
}  // namespace cppmh

/******************************************************************************/
// END
/******************************************************************************/
#endif