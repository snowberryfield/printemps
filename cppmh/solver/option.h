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
    static constexpr int    DEFAULT_ITERATION_MAX                      = 100;
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
};
}  // namespace solver
}  // namespace cppmh

#endif
/*****************************************************************************/
// END
/*****************************************************************************/