/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_CDCL_CORE_CDCL_CORE_STATE_H__
#define PRINTEMPS_SOLVER_CDCL_CORE_CDCL_CORE_STATE_H__

#include "cdcl_core_constants.h"
#include "cdcl_core_termination_status.h"
#include "cdcl_variable.h"

namespace printemps::solver::cdcl::core {
/*****************************************************************************/
struct CDCLCoreState {
    int    number_of_conflicts;
    int    current_decision_level;
    int    iteration;
    double elapsed_time;

    CDCLCoreTerminationStatus termination_status;

    std::vector<int>        trail;
    std::vector<int>        trail_level_offsets;
    CDCLConstraintReference conflict_constraint_index;

    std::vector<double> activity;
    double              variable_activity_increment;
    int                 conflict_limit;

    std::vector<int> seen_marker;
    int              seen_conflict_id;

    std::vector<int> seen_level_marker;
    int              seen_level_id;

    int     last_literal_block_distance;
    int     last_learned_length;
    int     number_of_assigned_zeros;
    int     number_of_assigned_ones;
    int64_t total_learned_literal_block_distance;
    int64_t total_learned_length;
    int     total_learned_count;
    int     number_of_restarts;
    int     number_of_decisions;
    int     number_of_propagations;

    int                          restart_luby_count;
    utility::FixedSizeQueue<int> literal_block_distance_queue;

    /*********************************************************************/
    CDCLCoreState(void) {
        this->initialize();
    }

    /*********************************************************************/
    inline void initialize(void) {
        this->number_of_conflicts    = 0;
        this->current_decision_level = 0;
        this->iteration              = 0;
        this->elapsed_time           = 0.0;
        this->termination_status     = CDCLCoreTerminationStatus::NOT_SOLVED;

        this->trail.clear();
        this->trail_level_offsets.clear();
        this->conflict_constraint_index = {CDCLConstraintType::NONE,
                                           CDCLCoreConstant::NO_CONFLICT};

        this->activity.clear();
        this->variable_activity_increment = 1.0;
        this->conflict_limit              = 100;

        this->seen_marker.clear();
        this->seen_conflict_id = 0;

        this->seen_level_marker.clear();
        this->seen_level_id = 0;

        this->last_literal_block_distance          = 0;
        this->last_learned_length                  = 0;
        this->number_of_assigned_zeros             = 0;
        this->number_of_assigned_ones              = 0;
        this->total_learned_literal_block_distance = 0;
        this->total_learned_length                 = 0;
        this->total_learned_count                  = 0;
        this->number_of_restarts                   = 0;
        this->number_of_decisions                  = 0;
        this->number_of_propagations               = 0;

        this->restart_luby_count = 0;
        this->literal_block_distance_queue.initialize();
    }
};
}  // namespace printemps::solver::cdcl::core

#endif
/*****************************************************************************/
// END
/*****************************************************************************/
