/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_LOCAL_SEARCH_CORE_LOCAL_SEARCH_CORE_STATE_H__
#define PRINTEMPS_SOLVER_LOCAL_SEARCH_CORE_LOCAL_SEARCH_CORE_STATE_H__

namespace printemps {
namespace solver {
namespace local_search {
namespace core {
/*****************************************************************************/
struct LocalSearchCoreState {
    int  update_status;
    int  total_update_status;
    int  iteration;
    int  number_of_moves;
    int  number_of_checked_move;
    bool is_found_improving_solution;

    LocalSearchCoreTerminationStatus termination_status;

    /*************************************************************************/
    LocalSearchCoreState(void) {
        this->initialize();
    }

    /*************************************************************************/
    void initialize(void) {
        this->update_status               = 0;
        this->total_update_status         = 0;
        this->iteration                   = 0;
        this->number_of_moves             = 0;
        this->number_of_checked_move      = 0;
        this->is_found_improving_solution = false;
        this->termination_status =
            LocalSearchCoreTerminationStatus::ITERATION_OVER;
    }
};
}  // namespace core
}  // namespace local_search
}  // namespace solver
}  // namespace printemps

#endif
/*****************************************************************************/
// END
/*****************************************************************************/