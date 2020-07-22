/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef CPPMH_SOLVER_LAGRANGE_DUAL_LAGRANGE_DUAL_OPTION_H__
#define CPPMH_SOLVER_LAGRANGE_DUAL_LAGRANGE_DUAL_OPTION_H__

namespace cppmh {
namespace solver {
namespace lagrange_dual {
/*****************************************************************************/
struct LagrangeDualOptionConstant {
    static constexpr int    DEFAULT_ITERATION_MAX         = 10000;
    static constexpr double DEFAULT_TIME_MAX              = 120.0;
    static constexpr double DEFAULT_TIME_OFFSET           = 0.0;
    static constexpr double DEFAULT_STEP_SIZE_EXTEND_RATE = 1.05;
    static constexpr double DEFAULT_STEP_SIZE_REDUCE_RATE = 0.95;
    static constexpr double DEFAULT_TOLERANCE             = 0.001;
    static constexpr int    DEFAULT_QUEUE_SIZE            = 100;
    static constexpr int    DEFAULT_LOG_INTERVAL          = 10;
};

/*****************************************************************************/
struct LagrangeDualOption {
    int    iteration_max;
    double time_max;
    double time_offset;  // hidden
    double step_size_extend_rate;
    double step_size_reduce_rate;
    double tolerance;
    double queue_size;
    int    log_interval;

    /*************************************************************************/
    LagrangeDualOption(void) {
        this->initialize();
    }

    /*************************************************************************/
    virtual ~LagrangeDualOption(void) {
        /// nothing to do
    }

    /*************************************************************************/
    inline constexpr void initialize(void) {
        this->iteration_max = LagrangeDualOptionConstant::DEFAULT_ITERATION_MAX;
        this->time_max      = LagrangeDualOptionConstant::DEFAULT_TIME_MAX;
        this->time_offset   = LagrangeDualOptionConstant::DEFAULT_TIME_OFFSET;
        this->step_size_extend_rate =
            LagrangeDualOptionConstant::DEFAULT_STEP_SIZE_EXTEND_RATE;
        this->step_size_reduce_rate =
            LagrangeDualOptionConstant::DEFAULT_STEP_SIZE_REDUCE_RATE;
        this->tolerance    = LagrangeDualOptionConstant::DEFAULT_TOLERANCE;
        this->queue_size   = LagrangeDualOptionConstant::DEFAULT_QUEUE_SIZE;
        this->log_interval = LagrangeDualOptionConstant::DEFAULT_LOG_INTERVAL;
    }
};
}  // namespace lagrange_dual
}  // namespace solver
}  // namespace cppmh

#endif
/*****************************************************************************/
// END
/*****************************************************************************/