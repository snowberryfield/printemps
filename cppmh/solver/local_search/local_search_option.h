/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef CPPMH_SOLVER_LOCAL_SEARCH_LOCAL_SEARCH_OPTION_H__
#define CPPMH_SOLVER_LOCAL_SEARCH_LOCAL_SEARCH_OPTION_H__

namespace cppmh {
namespace solver {
namespace local_search {
/*****************************************************************************/
struct LocalSearchOptionConstant {
    static constexpr int    DEFAULT_ITERATION_MAX = 10000;
    static constexpr double DEFAULT_TIME_MAX      = 120.0;
    static constexpr double DEFAULT_TIME_OFFSET   = 0.0;
    static constexpr int    DEFAULT_LOG_INTERVAL  = 10;
    static constexpr int    DEFAULT_SEED          = 1;
};

/*****************************************************************************/
struct LocalSearchOption {
    int    iteration_max;
    double time_max;
    double time_offset;
    int    log_interval;
    int    seed;

    /*************************************************************************/
    LocalSearchOption(void) {
        this->initialize();
    }

    /*************************************************************************/
    virtual ~LocalSearchOption(void) {
        /// nothing to do
    }

    /*************************************************************************/
    inline constexpr void initialize(void) {
        this->iteration_max = LocalSearchOptionConstant::DEFAULT_ITERATION_MAX;
        this->time_max      = LocalSearchOptionConstant::DEFAULT_TIME_MAX;
        this->time_offset   = LocalSearchOptionConstant::DEFAULT_TIME_OFFSET;
        this->log_interval  = LocalSearchOptionConstant::DEFAULT_LOG_INTERVAL;
        this->seed          = LocalSearchOptionConstant::DEFAULT_SEED;
    }
};
}  // namespace local_search
}  // namespace solver
}  // namespace cppmh

#endif
/*****************************************************************************/
// END
/*****************************************************************************/