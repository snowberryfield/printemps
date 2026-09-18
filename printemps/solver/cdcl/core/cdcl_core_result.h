/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_CDCL_CORE_CDCL_CORE_RESULT_H__
#define PRINTEMPS_SOLVER_CDCL_CORE_CDCL_CORE_RESULT_H__

#include "cdcl_core_termination_status.h"
#include "cdcl_core_state.h"
#include "cdcl_model.h"

namespace printemps::solver::cdcl::core {
/*****************************************************************************/
struct CDCLCoreResult {
    CDCLCoreTerminationStatus termination_status;
    std::vector<int>          solution;
    std::vector<int>          best_solution;
    double                    best_total_violation;

    int    number_of_conflicts;
    int    number_of_restarts;
    int    number_of_decisions;
    int    number_of_propagations;
    int    total_learned_count;
    double average_learned_literal_block_distance;
    double average_learned_length;

    /*************************************************************************/
    CDCLCoreResult(void) {
        this->initialize();
    }

    /*************************************************************************/
    CDCLCoreResult(const CDCLCoreState    &a_STATE,  //
                   const CDCLModel        &a_MODEL,  //
                   const std::vector<int> &a_BEST_SOLUTION = {},
                   const double            a_BEST_TOTAL_VIOLATION =
                       std::numeric_limits<double>::infinity()) {
        this->setup(a_STATE, a_MODEL, a_BEST_SOLUTION, a_BEST_TOTAL_VIOLATION);
    }

    /*************************************************************************/
    inline void initialize(void) {
        this->termination_status = CDCLCoreTerminationStatus::NOT_SOLVED;
        this->solution.clear();
        this->best_solution.clear();
        this->best_total_violation   = std::numeric_limits<double>::infinity();
        this->number_of_conflicts    = 0;
        this->number_of_restarts     = 0;
        this->number_of_decisions    = 0;
        this->number_of_propagations = 0;
        this->total_learned_count    = 0;
        this->average_learned_literal_block_distance = 0.0;
        this->average_learned_length                 = 0.0;
    }

    /*************************************************************************/
    inline void setup(const CDCLCoreState    &a_STATE,  //
                      const CDCLModel        &a_MODEL,  //
                      const std::vector<int> &a_BEST_SOLUTION = {},
                      const double            a_BEST_TOTAL_VIOLATION =
                          std::numeric_limits<double>::infinity()) {
        this->initialize();
        this->termination_status     = a_STATE.termination_status;
        this->number_of_conflicts    = a_STATE.number_of_conflicts;
        this->number_of_restarts     = a_STATE.number_of_restarts;
        this->number_of_decisions    = a_STATE.number_of_decisions;
        this->number_of_propagations = a_STATE.number_of_propagations;
        this->total_learned_count    = a_STATE.total_learned_count;

        this->average_learned_literal_block_distance =
            (a_STATE.total_learned_count > 0)
                ? (static_cast<double>(
                       a_STATE.total_learned_literal_block_distance) /
                   a_STATE.total_learned_count)
                : 0.0;
        this->average_learned_length =
            (a_STATE.total_learned_count > 0)
                ? (static_cast<double>(a_STATE.total_learned_length) /
                   a_STATE.total_learned_count)
                : 0.0;

        if (this->termination_status == CDCLCoreTerminationStatus::FEASIBLE) {
            const int VARIABLES_SIZE = a_MODEL.number_of_variables();
            this->solution.resize(VARIABLES_SIZE);
            for (int i = 0; i < VARIABLES_SIZE; i++) {
                this->solution[i] = a_MODEL.variable(i).value;
            }
            this->best_solution        = this->solution;
            this->best_total_violation = 0.0;
        } else {
            this->best_solution        = a_BEST_SOLUTION;
            this->best_total_violation = a_BEST_TOTAL_VIOLATION;
        }
    }
};
}  // namespace printemps::solver::cdcl::core

#endif
/*****************************************************************************/
// END
/*****************************************************************************/
