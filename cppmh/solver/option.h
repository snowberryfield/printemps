/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef CPPMH_SOLVER_OPTION_H__
#define CPPMH_SOLVER_OPTION_H__

#include "tabu_search/tabu_search_option.h"
#include "local_search/local_search_option.h"

namespace cppmh {
namespace solver {

/*****************************************************************************/
enum Verbose : int { None, Warning, Outer, Full, Debug };

/*****************************************************************************/
struct OptionConstant {
    static constexpr int    DEFAULT_ITERATION_MAX                      = 20;
    static constexpr double DEFAULT_TIME_MAX                           = 120.0;
    static constexpr double DEFAULT_TIME_OFFSET                        = 0.0;
    static constexpr double DEFAULT_PENALTY_COEFFICIENT_RELAXING_RATIO = 0.5;
    static constexpr double DEFAULT_PENALTY_COEFFICIENT_TIGHTENING_RATIO = 1.0;
    static constexpr double DEFAULT_INITIAL_PENALTY_COEFFICIENT          = 1E6;
    static constexpr bool   DEFAULT_IS_ENABLED_LOCAL_SEARCH = false;
    static constexpr bool   DEFAULT_IS_ENABLED_GROUPING_PENALTY_COEFFICIENT =
        false;
    static constexpr bool DEFAULT_IS_ENABLED_INITIAL_VALUE_CORRECTION = true;
    static constexpr bool DEFAULT_IS_ENABLED_PARALLEL_EVALUATION      = true;
    static constexpr bool DEFAULT_IS_ENABLED_PARALLEL_NEIGHBORHOOD_UPDATE =
        true;
    static constexpr double DEFAULT_TARGET_OBJECTIVE = -1E100;
    static constexpr bool   DEFAULT_VERBOSE          = Verbose::None;
};

/*****************************************************************************/
struct Option {
    int    iteration_max;
    double time_offset;
    double time_max;
    double penalty_coefficient_relaxing_ratio;
    double penalty_coefficient_tightening_ratio;
    double initial_penalty_coefficient;
    bool   is_enabled_local_search;
    bool   is_enabled_grouping_penalty_coefficient;
    bool   is_enabled_initial_value_correction;
    bool   is_enabled_parallel_evaluation;
    bool   is_enabled_parallel_neighborhood_update;
    double target_objective_value;
    int    verbose;

    tabu_search::TabuSearchOption   tabu_search;
    local_search::LocalSearchOption local_search;

    /*************************************************************************/
    Option(void) {
        this->initialize();
    }

    /*************************************************************************/
    virtual ~Option(void) {
        /// nothing to do
    }

    /*************************************************************************/
    inline constexpr void initialize(void) {
        this->iteration_max = OptionConstant::DEFAULT_ITERATION_MAX;
        this->time_max      = OptionConstant::DEFAULT_TIME_MAX;
        this->time_offset   = OptionConstant::DEFAULT_TIME_OFFSET;
        this->penalty_coefficient_relaxing_ratio =
            OptionConstant::DEFAULT_PENALTY_COEFFICIENT_RELAXING_RATIO;
        this->penalty_coefficient_tightening_ratio =
            OptionConstant::DEFAULT_PENALTY_COEFFICIENT_TIGHTENING_RATIO;
        this->initial_penalty_coefficient =
            OptionConstant::DEFAULT_INITIAL_PENALTY_COEFFICIENT;
        this->is_enabled_local_search =
            OptionConstant::DEFAULT_IS_ENABLED_LOCAL_SEARCH;
        this->is_enabled_grouping_penalty_coefficient =
            OptionConstant::DEFAULT_IS_ENABLED_GROUPING_PENALTY_COEFFICIENT;
        this->is_enabled_initial_value_correction =
            OptionConstant::DEFAULT_IS_ENABLED_INITIAL_VALUE_CORRECTION;
        this->is_enabled_parallel_evaluation =
            OptionConstant::DEFAULT_IS_ENABLED_PARALLEL_EVALUATION;
        this->is_enabled_parallel_neighborhood_update =
            OptionConstant::DEFAULT_IS_ENABLED_PARALLEL_NEIGHBORHOOD_UPDATE;
        this->target_objective_value = OptionConstant::DEFAULT_TARGET_OBJECTIVE;
        this->verbose                = OptionConstant::DEFAULT_VERBOSE;

        this->tabu_search.initialize();
        this->local_search.initialize();
    }

    /*************************************************************************/
    inline void print(void) const {
        utility::print_single_line(true);
        utility::print_info(
            "The values for each option are specified as follows:", true);

        utility::print(             //
            " - iteration_max: " +  //
            utility::to_string(this->iteration_max, "%d"));

        utility::print(           //
            " - time_offset: " +  //
            utility::to_string(this->time_offset, "%f"));

        utility::print(        //
            " - time_max: " +  //
            utility::to_string(this->time_max, "%f"));

        utility::print(                                  //
            " - penalty_coefficient_relaxing_ratio: " +  //
            utility::to_string(this->penalty_coefficient_relaxing_ratio, "%f"));

        utility::print(                                    //
            " - penalty_coefficient_tightening_ratio: " +  //
            utility::to_string(this->penalty_coefficient_tightening_ratio,
                               "%f"));

        utility::print(                           //
            " - initial_penalty_coefficient: " +  //
            utility::to_string(this->initial_penalty_coefficient, "%f"));

        utility::print(                       //
            " - is_enabled_local_search: " +  //
            utility::to_string(this->is_enabled_local_search, "%d"));

        utility::print(                                       //
            " - is_enabled_grouping_penalty_coefficient: " +  //
            utility::to_string(this->is_enabled_grouping_penalty_coefficient,
                               "%d"));

        utility::print(                                   //
            " - is_enabled_initial_value_correction: " +  //
            utility::to_string(this->is_enabled_initial_value_correction,
                               "%d"));

        utility::print(                              //
            " - is_enabled_parallel_evaluation: " +  //
            utility::to_string(this->is_enabled_parallel_evaluation, "%d"));

        utility::print(                                       //
            " - is_enabled_parallel_neighborhood_update: " +  //
            utility::to_string(this->is_enabled_parallel_neighborhood_update,
                               "%d"));

        utility::print(                      //
            " - target_objective_value: " +  //
            utility::to_string(this->target_objective_value, "%.3e"));

        utility::print(       //
            " - verbose: " +  //
            utility::to_string(this->verbose, "%d"));

        utility::print(                          //
            " - local_search.iteration_max: " +  //
            utility::to_string(this->local_search.iteration_max, "%d"));

        utility::print(  //
            " - local_search.time_max: " +
            utility::to_string(this->local_search.time_max, "%f"));

        utility::print(                        //
            " - local_search.time_offset: " +  //
            utility::to_string(this->local_search.time_offset, "%f"));

        utility::print(                         //
            " - local_search.log_interval: " +  //
            utility::to_string(this->local_search.log_interval, "%d"));

        utility::print(                 //
            " - local_search.seed: " +  //
            utility::to_string(this->local_search.seed, "%d"));

        utility::print(                         //
            " - tabu_search.iteration_max: " +  //
            utility::to_string(this->tabu_search.iteration_max, "%d"));

        utility::print(                    //
            " - tabu_search.time_max: " +  //
            utility::to_string(this->tabu_search.time_max, "%f"));

        utility::print(                       //
            " - tabu_search.time_offset: " +  //
            utility::to_string(this->tabu_search.time_offset, "%f"));

        utility::print(                        //
            " - tabu_search.log_interval: " +  //
            utility::to_string(this->tabu_search.log_interval, "%d"));

        utility::print(  //
            " - tabu_search.initial_tabu_tenure: " +
            utility::to_string(this->tabu_search.initial_tabu_tenure, "%d"));

        utility::print(                     //
            " - tabu_search.tabu_mode: " +  //
            utility::to_string(this->tabu_search.tabu_mode, "%d"));

        utility::print(                              //
            " - tabu_search.move_preserve_rate: " +  //
            utility::to_string(this->tabu_search.move_preserve_rate, "%f"));

        utility::print(                                         //
            " - tabu_search.frequency_penalty_coefficient: " +  //
            utility::to_string(this->tabu_search.frequency_penalty_coefficient,
                               "%f"));

        utility::print(                                              //
            " - tabu_search.is_enabled_improvability_screening: " +  //
            utility::to_string(
                this->tabu_search.is_enabled_improvability_screening, "%d"));

        utility::print(                              //
            " - tabu_search.is_enabled_shuffle: " +  //
            utility::to_string(this->tabu_search.is_enabled_shuffle, "%d"));

        utility::print(                                   //
            " - tabu_search.is_enabled_move_curtail: " +  //
            utility::to_string(this->tabu_search.is_enabled_move_curtail,
                               "%d"));

        utility::print(                                      //
            " - tabu_search.is_enabled_automatic_break: " +  //
            utility::to_string(this->tabu_search.is_enabled_automatic_break,
                               "%d"));

        utility::print(                                                      //
            " - tabu_search.is_enabled_automatic_tabu_tenure_adjustment:" +  //
            utility::to_string(
                this->tabu_search.is_enabled_automatic_tabu_tenure_adjustment,
                "%d"));

        utility::print(                                              //
            " - tabu_search.ignore_tabu_if_augmented_incumbent: " +  //
            utility::to_string(
                this->tabu_search.ignore_tabu_if_augmented_incumbent, "%d"));

        utility::print(                                             //
            " - tabu_search.ignore_tabu_if_feasible_incumbent: " +  //
            utility::to_string(
                this->tabu_search.ignore_tabu_if_feasible_incumbent, "%d"));

        utility::print(                                          //
            " - tabu_search.number_of_initial_modification: " +  //
            utility::to_string(this->tabu_search.number_of_initial_modification,
                               "%d"));

        utility::print(              //
            " - tabu_search.seed: "  //
            + utility::to_string(this->tabu_search.seed, "%d"));
    }
};
}  // namespace solver
}  // namespace cppmh

#endif
/*****************************************************************************/
// END
/*****************************************************************************/