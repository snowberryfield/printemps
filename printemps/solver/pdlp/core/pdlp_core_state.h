/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_PDLP_CORE_PROGRAMMING_CORE_STATE_H__
#define PRINTEMPS_SOLVER_PDLP_CORE_PROGRAMMING_CORE_STATE_H__

#include "restart_mode.h"
#include "primal_dual_state.h"

namespace printemps::solver::pdlp::core {
/*****************************************************************************/
struct PDLPCoreState {
    int  inner_iteration;
    int  outer_iteration;
    int  total_iteration;
    long number_of_solution_update_attempt;

    double elapsed_time;

    PrimalDualState primal;
    PrimalDualState dual;

    double absolute_gap;
    double relative_gap;

    double step_size_current;
    double step_size_previous;
    double step_size_cumulative_sum;

    double primal_weight;

    double current_outer_loop_normalized_gap;
    double previous_outer_loop_normalized_gap;

    double current_inner_loop_normalized_gap;
    double previous_inner_loop_normalized_gap;

    bool        is_enabled_restart;
    RestartMode restart_mode;

    PDLPCoreTerminationStatus termination_status;

    /*************************************************************************/
    PDLPCoreState(void) {
        this->initialize();
    }

    /*************************************************************************/
    void initialize(void) {
        this->inner_iteration                   = 0;
        this->outer_iteration                   = 0;
        this->total_iteration                   = 0;
        this->number_of_solution_update_attempt = 0;
        this->elapsed_time                      = 0.0;

        this->primal.initialize();
        this->dual.initialize();

        this->absolute_gap = 0.0;
        this->relative_gap = 0.0;

        this->step_size_current        = 0.0;
        this->step_size_previous       = 0.0;
        this->step_size_cumulative_sum = 0.0;

        this->primal_weight = 0.0;

        this->current_outer_loop_normalized_gap  = 0.0;
        this->previous_outer_loop_normalized_gap = 0.0;

        this->current_inner_loop_normalized_gap  = 0.0;
        this->previous_inner_loop_normalized_gap = 0.0;

        this->is_enabled_restart = false;
        this->restart_mode       = RestartMode::Current;

        this->termination_status = PDLPCoreTerminationStatus::ITERATION_OVER;
    }
};
}  // namespace printemps::solver::pdlp::core

#endif
/*****************************************************************************/
// END
/*****************************************************************************/