/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef CPPMH_SOLVER_TABU_SEARCH_TABU_SEARCH_OPTION_H__
#define CPPMH_SOLVER_TABU_SEARCH_TABU_SEARCH_OPTION_H__

namespace cppmh {
namespace solver {
namespace tabu_search {
/*****************************************************************************/
struct TabuSearchOptionConstant {
    static constexpr int    DEFAULT_ITERATION_MAX                 = 200;
    static constexpr double DEFAULT_TIME_MAX                      = 120.0;
    static constexpr double DEFAULT_TIME_OFFSET                   = 0.0;
    static constexpr int    DEFAULT_LOG_INTERVAL                  = 10;
    static constexpr int    DEFAULT_INITIAL_TABU_TENURE           = 10;
    static constexpr double DEFAULT_MOVE_PRESERVE_RATE            = 1.0;
    static constexpr double DEFAULT_FREQUENCY_PENALTY_COEFFICIENT = 1E-5;

    static constexpr bool   DEFAULT_IS_ENABLED_IMPROVABILITY_SCREENING = true;
    static constexpr double DEFAULT_IS_ENABLED_SHUFFLE                 = false;
    static constexpr double DEFAULT_IS_ENABLED_MOVE_CURTAIL            = false;
    static constexpr bool   DEFAULT_IS_ENABLED_AUTOMATIC_BREAK         = true;
    static constexpr bool DEFAULT_IS_ENABLED_AUTOMATIC_TABU_TENURE_ADJUSTMENT =
        true;
    static constexpr bool DEFAULT_IS_ENABLED_AUTOMATIC_ITERATION_ADJUSTMENT =
        true;
    static constexpr bool   DEFAULT_IS_ENABLED_INITIAL_MODIFICATION    = true;
    static constexpr int    DEFAULT_BIAS_INCREASE_COUNT_THRESHOLD      = 5;
    static constexpr int    DEFAULT_BIAS_DECREASE_COUNT_THRESHOLD      = 10;
    static constexpr double DEFAULT_ITERATION_INCREASE_RATE            = 1.5;
    static constexpr double DEFAULT_ITERATION_DEREASE_RATE             = 0.9;
    static constexpr double DEFAULT_IGNORE_TABU_IF_AUGMENTED_INCUMBENT = false;
    static constexpr double DEFAULT_IGNORE_TABU_IF_FEASIBLE_INCUMBENT  = true;
    static constexpr bool   DEFAULT_NUMBER_OF_INITIAL_MODIFICATION     = 0;
    static constexpr int    DEFAULT_SEED                               = 1;
};

/*****************************************************************************/
enum TabuMode : int {
    Any,
    All,
};

/*****************************************************************************/
struct TabuSearchOption {
    int      iteration_max;
    double   time_max;
    double   time_offset;
    int      log_interval;
    int      initial_tabu_tenure;
    TabuMode tabu_mode;
    double   move_preserve_rate;
    double   frequency_penalty_coefficient;
    bool     is_enabled_improvability_screening;
    bool     is_enabled_shuffle;
    bool     is_enabled_move_curtail;
    bool     is_enabled_automatic_break;
    bool     is_enabled_automatic_tabu_tenure_adjustment;
    bool     is_enabled_automatic_iteration_adjustment;
    bool     is_enabled_initial_modification;
    int      bias_increase_count_threshold;
    int      bias_decrease_count_threshold;
    double   iteration_increase_rate;
    double   iteration_decrease_rate;
    bool     ignore_tabu_if_augmented_incumbent;
    bool     ignore_tabu_if_feasible_incumbent;
    int      number_of_initial_modification;
    int      seed;

    /*************************************************************************/
    TabuSearchOption(void) {
        this->initialize();
    }

    /*************************************************************************/
    virtual ~TabuSearchOption(void) {
        /// nothing to do
    }

    /*************************************************************************/
    inline constexpr void initialize(void) {
        this->iteration_max = TabuSearchOptionConstant::DEFAULT_ITERATION_MAX;
        this->time_max      = TabuSearchOptionConstant::DEFAULT_TIME_MAX;
        this->time_offset   = TabuSearchOptionConstant::DEFAULT_TIME_OFFSET;
        this->log_interval  = TabuSearchOptionConstant::DEFAULT_LOG_INTERVAL;
        this->initial_tabu_tenure =
            TabuSearchOptionConstant::DEFAULT_INITIAL_TABU_TENURE;
        this->tabu_mode = TabuMode::All;
        this->move_preserve_rate =
            TabuSearchOptionConstant::DEFAULT_MOVE_PRESERVE_RATE;
        this->frequency_penalty_coefficient =
            TabuSearchOptionConstant::DEFAULT_FREQUENCY_PENALTY_COEFFICIENT;
        this->is_enabled_improvability_screening = TabuSearchOptionConstant::
            DEFAULT_IS_ENABLED_IMPROVABILITY_SCREENING;
        this->is_enabled_shuffle =
            TabuSearchOptionConstant::DEFAULT_IS_ENABLED_SHUFFLE;
        this->is_enabled_move_curtail =
            TabuSearchOptionConstant::DEFAULT_IS_ENABLED_MOVE_CURTAIL;
        this->is_enabled_automatic_break =
            TabuSearchOptionConstant::DEFAULT_IS_ENABLED_AUTOMATIC_BREAK;
        this->is_enabled_automatic_tabu_tenure_adjustment =
            TabuSearchOptionConstant::
                DEFAULT_IS_ENABLED_AUTOMATIC_TABU_TENURE_ADJUSTMENT;
        this->is_enabled_automatic_iteration_adjustment =
            TabuSearchOptionConstant::
                DEFAULT_IS_ENABLED_AUTOMATIC_ITERATION_ADJUSTMENT;
        this->is_enabled_initial_modification =
            TabuSearchOptionConstant::DEFAULT_IS_ENABLED_INITIAL_MODIFICATION;
        this->bias_increase_count_threshold =
            TabuSearchOptionConstant::DEFAULT_BIAS_INCREASE_COUNT_THRESHOLD;
        this->bias_decrease_count_threshold =
            TabuSearchOptionConstant::DEFAULT_BIAS_DECREASE_COUNT_THRESHOLD;
        this->iteration_increase_rate =
            TabuSearchOptionConstant::DEFAULT_ITERATION_INCREASE_RATE;
        this->iteration_decrease_rate =
            TabuSearchOptionConstant::DEFAULT_ITERATION_DEREASE_RATE;
        this->ignore_tabu_if_augmented_incumbent = TabuSearchOptionConstant::
            DEFAULT_IGNORE_TABU_IF_AUGMENTED_INCUMBENT;
        this->ignore_tabu_if_feasible_incumbent =
            TabuSearchOptionConstant::DEFAULT_IGNORE_TABU_IF_FEASIBLE_INCUMBENT;
        this->number_of_initial_modification =
            TabuSearchOptionConstant::DEFAULT_NUMBER_OF_INITIAL_MODIFICATION;
        this->seed = TabuSearchOptionConstant::DEFAULT_SEED;
    }
};
}  // namespace tabu_search
}  // namespace solver
}  // namespace cppmh

#endif
/*****************************************************************************/
// END
/*****************************************************************************/