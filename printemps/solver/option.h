/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_OPTION_H__
#define PRINTEMPS_SOLVER_OPTION_H__

#include "lagrange_dual/lagrange_dual_option.h"
#include "local_search/local_search_option.h"
#include "tabu_search/tabu_search_option.h"

namespace printemps {
namespace solver {
/*****************************************************************************/
enum Verbose : int { None, Warning, Outer, Full, Debug };

/*****************************************************************************/
enum ImprovabilityScreeningMode : int {
    Off,
    Soft,
    Aggressive,
    Automatic,
    Intensive
};

/*****************************************************************************/
struct OptionConstant {
    static constexpr int    DEFAULT_ITERATION_MAX                       = 100;
    static constexpr double DEFAULT_TIME_MAX                            = 120.0;
    static constexpr double DEFAULT_TIME_OFFSET                         = 0.0;
    static constexpr double DEFAULT_PENALTY_COEFFICIENT_RELAXING_RATE   = 0.9;
    static constexpr double DEFAULT_PENALTY_COEFFICIENT_TIGHTENING_RATE = 1.0;
    static constexpr double DEFAULT_PENALTY_COEFFICIENT_UPDATING_BALANCE = 0.0;
    static constexpr int DEFAULT_PENALTY_COEFFICIENT_RESET_COUNT_THRESHOLD = -1;
    static constexpr double DEFAULT_INITIAL_PENALTY_COEFFICIENT = 1E7;
    static constexpr bool   DEFAULT_IS_ENABLED_LAGRANGE_DUAL    = false;
    static constexpr bool   DEFAULT_IS_ENABLED_LOCAL_SEARCH     = false;
    static constexpr bool   DEFAULT_IS_ENABLED_GROUPING_PENALTY_COEFFICIENT =
        false;
    static constexpr bool DEFAULT_IS_ENABLED_PRESOLVE                 = true;
    static constexpr bool DEFAULT_IS_ENABLED_INITIAL_VALUE_CORRECTION = true;
    static constexpr bool DEFAULT_IS_ENABLED_PARALLEL_EVALUATION      = true;
    static constexpr bool DEFAULT_IS_ENABLED_PARALLEL_NEIGHBORHOOD_UPDATE =
        true;
    static constexpr bool DEFAULT_IS_ENABLED_BINARY_MOVE         = true;
    static constexpr bool DEFAULT_IS_ENABLED_INTEGER_MOVE        = true;
    static constexpr bool DEFAULT_IS_ENABLED_AGGREGATION_MOVE    = false;
    static constexpr bool DEFAULT_IS_ENABLED_PRECEDENCE_MOVE     = false;
    static constexpr bool DEFAULT_IS_ENABLED_VARIABLE_BOUND_MOVE = false;
    static constexpr bool DEFAULT_IS_ENABLED_EXCLUSIVE_MOVE      = false;
    static constexpr bool DEFAULT_IS_ENABLED_CHAIN_MOVE          = false;
    static constexpr bool DEFAULT_IS_ENABLED_USER_DEFINED_MOVE   = false;

    static constexpr model::SelectionMode DEFAULT_SELECTION_MODE =
        model::SelectionMode::None;
    static constexpr ImprovabilityScreeningMode
        DEFAULT_IMPROVABILITY_SCREENING_MODE =
            ImprovabilityScreeningMode::Automatic;

    static constexpr double DEFAULT_TARGET_OBJECTIVE = -1E100;
    static constexpr bool   DEFAULT_SEED             = 1;
    static constexpr bool   DEFAULT_VERBOSE          = Verbose::None;
    static constexpr bool   DEFAULT_IS_ENABLED_COLLECT_HISTORICAL_DATA = false;
    static constexpr int    DEFAULT_HISTORICAL_DATA_CAPACITY           = 1000;
};

/*****************************************************************************/
struct Option {
    int    iteration_max;
    double time_offset;  // hidden
    double time_max;
    double penalty_coefficient_relaxing_rate;
    double penalty_coefficient_tightening_rate;
    double penalty_coefficient_updating_balance;       // hidden
    int    penalty_coefficient_reset_count_threshold;  // hidden
    double initial_penalty_coefficient;
    bool   is_enabled_lagrange_dual;
    bool   is_enabled_local_search;
    bool   is_enabled_grouping_penalty_coefficient;
    bool   is_enabled_presolve;
    bool   is_enabled_initial_value_correction;
    bool   is_enabled_parallel_evaluation;
    bool   is_enabled_parallel_neighborhood_update;

    bool is_enabled_binary_move;
    bool is_enabled_integer_move;
    bool is_enabled_aggregation_move;
    bool is_enabled_precedence_move;
    bool is_enabled_variable_bound_move;
    bool is_enabled_exclusive_move;
    bool is_enabled_chain_move;
    bool is_enabled_user_defined_move;

    model::SelectionMode       selection_mode;
    ImprovabilityScreeningMode improvability_screening_mode;

    double target_objective_value;
    int    seed;  // hidden
    int    verbose;
    bool   is_enabled_collect_historical_data;  // hidden
    int    historical_data_capacity;            // hidden

    tabu_search::TabuSearchOption     tabu_search;
    local_search::LocalSearchOption   local_search;
    lagrange_dual::LagrangeDualOption lagrange_dual;

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
        this->penalty_coefficient_relaxing_rate =
            OptionConstant::DEFAULT_PENALTY_COEFFICIENT_RELAXING_RATE;
        this->penalty_coefficient_tightening_rate =
            OptionConstant::DEFAULT_PENALTY_COEFFICIENT_TIGHTENING_RATE;
        this->penalty_coefficient_updating_balance =
            OptionConstant::DEFAULT_PENALTY_COEFFICIENT_UPDATING_BALANCE;
        this->penalty_coefficient_reset_count_threshold =
            OptionConstant::DEFAULT_PENALTY_COEFFICIENT_RESET_COUNT_THRESHOLD;
        this->initial_penalty_coefficient =
            OptionConstant::DEFAULT_INITIAL_PENALTY_COEFFICIENT;
        this->is_enabled_lagrange_dual =
            OptionConstant::DEFAULT_IS_ENABLED_LAGRANGE_DUAL;
        this->is_enabled_local_search =
            OptionConstant::DEFAULT_IS_ENABLED_LOCAL_SEARCH;
        this->is_enabled_grouping_penalty_coefficient =
            OptionConstant::DEFAULT_IS_ENABLED_GROUPING_PENALTY_COEFFICIENT;
        this->is_enabled_presolve = OptionConstant::DEFAULT_IS_ENABLED_PRESOLVE;
        this->is_enabled_initial_value_correction =
            OptionConstant::DEFAULT_IS_ENABLED_INITIAL_VALUE_CORRECTION;
        this->is_enabled_parallel_evaluation =
            OptionConstant::DEFAULT_IS_ENABLED_PARALLEL_EVALUATION;
        this->is_enabled_parallel_neighborhood_update =
            OptionConstant::DEFAULT_IS_ENABLED_PARALLEL_NEIGHBORHOOD_UPDATE;
        this->is_enabled_binary_move =
            OptionConstant::DEFAULT_IS_ENABLED_BINARY_MOVE;
        this->is_enabled_integer_move =
            OptionConstant::DEFAULT_IS_ENABLED_INTEGER_MOVE;
        this->is_enabled_aggregation_move =
            OptionConstant::DEFAULT_IS_ENABLED_AGGREGATION_MOVE;
        this->is_enabled_precedence_move =
            OptionConstant::DEFAULT_IS_ENABLED_PRECEDENCE_MOVE;
        this->is_enabled_variable_bound_move =
            OptionConstant::DEFAULT_IS_ENABLED_VARIABLE_BOUND_MOVE;
        this->is_enabled_exclusive_move =
            OptionConstant::DEFAULT_IS_ENABLED_EXCLUSIVE_MOVE;
        this->is_enabled_chain_move =
            OptionConstant::DEFAULT_IS_ENABLED_CHAIN_MOVE;
        this->is_enabled_user_defined_move =
            OptionConstant::DEFAULT_IS_ENABLED_USER_DEFINED_MOVE;

        this->selection_mode = OptionConstant::DEFAULT_SELECTION_MODE;
        this->improvability_screening_mode =
            OptionConstant::DEFAULT_IMPROVABILITY_SCREENING_MODE;

        this->target_objective_value = OptionConstant::DEFAULT_TARGET_OBJECTIVE;
        this->seed                   = OptionConstant::DEFAULT_SEED;
        this->verbose                = OptionConstant::DEFAULT_VERBOSE;
        this->is_enabled_collect_historical_data =
            OptionConstant::DEFAULT_IS_ENABLED_COLLECT_HISTORICAL_DATA;
        this->historical_data_capacity =
            OptionConstant::DEFAULT_HISTORICAL_DATA_CAPACITY;

        this->lagrange_dual.initialize();
        this->local_search.initialize();
        this->tabu_search.initialize();
    }

    /*************************************************************************/
    inline void print(void) const {
        utility::print_single_line(true);
        utility::print_info("The values for options are specified as follows:",
                            true);

        utility::print(              //
            " -- iteration_max: " +  //
            utility::to_string(this->iteration_max, "%d"));

        utility::print(            //
            " -- time_offset: " +  //
            utility::to_string(this->time_offset, "%f"));

        utility::print(         //
            " -- time_max: " +  //
            utility::to_string(this->time_max, "%f"));

        utility::print(                                  //
            " -- penalty_coefficient_relaxing_rate: " +  //
            utility::to_string(this->penalty_coefficient_relaxing_rate, "%f"));

        utility::print(                                    //
            " -- penalty_coefficient_tightening_rate: " +  //
            utility::to_string(this->penalty_coefficient_tightening_rate,
                               "%f"));

        utility::print(                                     //
            " -- penalty_coefficient_updating_balance: " +  //
            utility::to_string(this->penalty_coefficient_updating_balance,
                               "%f"));

        utility::print(                                          //
            " -- penalty_coefficient_reset_count_threshold: " +  //
            utility::to_string(this->penalty_coefficient_reset_count_threshold,
                               "%d"));

        utility::print(                            //
            " -- initial_penalty_coefficient: " +  //
            utility::to_string(this->initial_penalty_coefficient, "%f"));

        utility::print(                    //
            " -- is_enabled_presolve: " +  //
            utility::to_string(this->is_enabled_presolve, "%d"));

        utility::print(                         //
            " -- is_enabled_lagrange_dual: " +  //
            utility::to_string(this->is_enabled_lagrange_dual, "%d"));

        utility::print(                        //
            " -- is_enabled_local_search: " +  //
            utility::to_string(this->is_enabled_local_search, "%d"));

        utility::print(                                        //
            " -- is_enabled_grouping_penalty_coefficient: " +  //
            utility::to_string(this->is_enabled_grouping_penalty_coefficient,
                               "%d"));

        utility::print(                                    //
            " -- is_enabled_initial_value_correction: " +  //
            utility::to_string(this->is_enabled_initial_value_correction,
                               "%d"));

        utility::print(                               //
            " -- is_enabled_parallel_evaluation: " +  //
            utility::to_string(this->is_enabled_parallel_evaluation, "%d"));

        utility::print(                                        //
            " -- is_enabled_parallel_neighborhood_update: " +  //
            utility::to_string(this->is_enabled_parallel_neighborhood_update,
                               "%d"));

        utility::print(                       //
            " -- is_enabled_binary_move: " +  //
            utility::to_string(this->is_enabled_binary_move, "%d"));

        utility::print(                        //
            " -- is_enabled_integer_move: " +  //
            utility::to_string(this->is_enabled_integer_move, "%d"));

        utility::print(                            //
            " -- is_enabled_aggregation_move: " +  //
            utility::to_string(this->is_enabled_aggregation_move, "%d"));

        utility::print(                           //
            " -- is_enabled_precedence_move: " +  //
            utility::to_string(this->is_enabled_precedence_move, "%d"));

        utility::print(                               //
            " -- is_enabled_variable_bound_move: " +  //
            utility::to_string(this->is_enabled_variable_bound_move, "%d"));

        utility::print(                          //
            " -- is_enabled_exclusive_move: " +  //
            utility::to_string(this->is_enabled_exclusive_move, "%d"));

        utility::print(                      //
            " -- is_enabled_chain_move: " +  //
            utility::to_string(this->is_enabled_chain_move, "%d"));

        utility::print(                             //
            " -- is_enabled_user_defined_move: " +  //
            utility::to_string(this->is_enabled_user_defined_move, "%d"));

        utility::print(               //
            " -- selection_mode: " +  //
            utility::to_string(this->selection_mode, "%d"));

        utility::print(                             //
            " -- improvability_screening_mode: " +  //
            utility::to_string(this->improvability_screening_mode, "%d"));

        utility::print(                       //
            " -- target_objective_value: " +  //
            utility::to_string(this->target_objective_value, "%.3e"));

        utility::print(     //
            " -- seed: " +  //
            utility::to_string(this->seed, "%d"));

        utility::print(        //
            " -- verbose: " +  //
            utility::to_string(this->verbose, "%d"));

        utility::print(                                   //
            " -- is_enabled_collect_historical_data: " +  //
            utility::to_string(this->is_enabled_collect_historical_data, "%d"));

        utility::print(                         //
            " -- historical_data_capacity: " +  //
            utility::to_string(this->historical_data_capacity, "%d"));

        utility::print(                            //
            " -- lagrange_dual.iteration_max: " +  //
            utility::to_string(this->lagrange_dual.iteration_max, "%d"));

        utility::print(  //
            " -- lagrange_dual.time_max: " +
            utility::to_string(this->lagrange_dual.time_max, "%f"));

        utility::print(                          //
            " -- lagrange_dual.time_offset: " +  //
            utility::to_string(this->lagrange_dual.time_offset, "%f"));

        utility::print(                                    //
            " -- lagrange_dual.step_size_extend_rate: " +  //
            utility::to_string(this->lagrange_dual.step_size_extend_rate,
                               "%f"));

        utility::print(                                    //
            " -- lagrange_dual.step_size_reduce_rate: " +  //
            utility::to_string(this->lagrange_dual.step_size_reduce_rate,
                               "%f"));

        utility::print(                        //
            " -- lagrange_dual.tolerance: " +  //
            utility::to_string(this->lagrange_dual.tolerance, "%f"));

        utility::print(                         //
            " -- lagrange_dual.queue_size: " +  //
            utility::to_string(this->lagrange_dual.queue_size, "%d"));

        utility::print(                           //
            " -- lagrange_dual.log_interval: " +  //
            utility::to_string(this->lagrange_dual.log_interval, "%d"));

        utility::print(                           //
            " -- local_search.iteration_max: " +  //
            utility::to_string(this->local_search.iteration_max, "%d"));

        utility::print(  //
            " -- local_search.time_max: " +
            utility::to_string(this->local_search.time_max, "%f"));

        utility::print(                         //
            " -- local_search.time_offset: " +  //
            utility::to_string(this->local_search.time_offset, "%f"));

        utility::print(                          //
            " -- local_search.log_interval: " +  //
            utility::to_string(this->local_search.log_interval, "%d"));

        utility::print(                  //
            " -- local_search.seed: " +  //
            utility::to_string(this->local_search.seed, "%d"));

        utility::print(                          //
            " -- tabu_search.iteration_max: " +  //
            utility::to_string(this->tabu_search.iteration_max, "%d"));

        utility::print(                     //
            " -- tabu_search.time_max: " +  //
            utility::to_string(this->tabu_search.time_max, "%f"));

        utility::print(                        //
            " -- tabu_search.time_offset: " +  //
            utility::to_string(this->tabu_search.time_offset, "%f"));

        utility::print(                         //
            " -- tabu_search.log_interval: " +  //
            utility::to_string(this->tabu_search.log_interval, "%d"));

        utility::print(  //
            " -- tabu_search.initial_tabu_tenure: " +
            utility::to_string(this->tabu_search.initial_tabu_tenure, "%d"));

        utility::print(  //
            " -- tabu_search.tabu_tenure_randomize_rate: " +
            utility::to_string(this->tabu_search.tabu_tenure_randomize_rate,
                               "%f"));

        utility::print(  //
            " -- tabu_search.initial_modification_fixed_rate: " +
            utility::to_string(
                this->tabu_search.initial_modification_fixed_rate, "%f"));

        utility::print(  //
            " -- tabu_search.initial_modification_randomize_rate: " +
            utility::to_string(
                this->tabu_search.initial_modification_randomize_rate, "%f"));

        utility::print(                      //
            " -- tabu_search.tabu_mode: " +  //
            utility::to_string(this->tabu_search.tabu_mode, "%d"));

        utility::print(                               //
            " -- tabu_search.move_preserve_rate: " +  //
            utility::to_string(this->tabu_search.move_preserve_rate, "%f"));

        utility::print(                                          //
            " -- tabu_search.frequency_penalty_coefficient: " +  //
            utility::to_string(this->tabu_search.frequency_penalty_coefficient,
                               "%f"));

        utility::print(                               //
            " -- tabu_search.is_enabled_shuffle: " +  //
            utility::to_string(this->tabu_search.is_enabled_shuffle, "%d"));

        utility::print(                                    //
            " -- tabu_search.is_enabled_move_curtail: " +  //
            utility::to_string(this->tabu_search.is_enabled_move_curtail,
                               "%d"));

        utility::print(                                       //
            " -- tabu_search.is_enabled_automatic_break: " +  //
            utility::to_string(this->tabu_search.is_enabled_automatic_break,
                               "%d"));

        utility::print(  //
            " -- tabu_search.is_enabled_automatic_tabu_tenure_adjustment: " +  //
            utility::to_string(
                this->tabu_search.is_enabled_automatic_tabu_tenure_adjustment,
                "%d"));

        utility::print(                                                      //
            " -- tabu_search.is_enabled_automatic_iteration_adjustment: " +  //
            utility::to_string(
                this->tabu_search.is_enabled_automatic_iteration_adjustment,
                "%d"));

        utility::print(                                            //
            " -- tabu_search.is_enabled_initial_modification: " +  //
            utility::to_string(
                this->tabu_search.is_enabled_initial_modification, "%d"));

        utility::print(                                          //
            " -- tabu_search.bias_increase_count_threshold: " +  //
            utility::to_string(this->tabu_search.bias_increase_count_threshold,
                               "%d"));

        utility::print(                                          //
            " -- tabu_search.bias_decrease_count_threshold: " +  //
            utility::to_string(this->tabu_search.bias_decrease_count_threshold,
                               "%d"));

        utility::print(                                    //
            " -- tabu_search.iteration_increase_rate: " +  //
            utility::to_string(this->tabu_search.iteration_increase_rate,
                               "%f"));

        utility::print(                                    //
            " -- tabu_search.iteration_decrease_rate: " +  //
            utility::to_string(this->tabu_search.iteration_decrease_rate,
                               "%f"));

        utility::print(                                               //
            " -- tabu_search.ignore_tabu_if_augmented_incumbent: " +  //
            utility::to_string(
                this->tabu_search.ignore_tabu_if_augmented_incumbent, "%d"));

        utility::print(                                              //
            " -- tabu_search.ignore_tabu_if_feasible_incumbent: " +  //
            utility::to_string(
                this->tabu_search.ignore_tabu_if_feasible_incumbent, "%d"));

        utility::print(                                           //
            " -- tabu_search.number_of_initial_modification: " +  //
            utility::to_string(this->tabu_search.number_of_initial_modification,
                               "%d"));

        utility::print(               //
            " -- tabu_search.seed: "  //
            + utility::to_string(this->tabu_search.seed, "%d"));
    }
};
}  // namespace solver
}  // namespace printemps

#endif
/*****************************************************************************/
// END
/*****************************************************************************/