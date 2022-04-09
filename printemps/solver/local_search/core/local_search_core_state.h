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
template <class T_Variable, class T_Expression>
struct LocalSearchCoreState {
    int    update_status;
    int    total_update_status;
    int    iteration;
    double elapsed_time;

    LocalSearchCoreTerminationStatus termination_status;

    neighborhood::Move<T_Variable, T_Expression> previous_move;
    neighborhood::Move<T_Variable, T_Expression> current_move;

    solution::SolutionScore current_solution_score;
    solution::SolutionScore previous_solution_score;

    int  number_of_moves;
    int  number_of_performed_moves;
    bool is_found_improving_solution;

    /*************************************************************************/
    LocalSearchCoreState(void) {
        this->initialize();
    }

    /*************************************************************************/
    void initialize(void) {
        this->update_status       = 0;
        this->total_update_status = 0;
        this->iteration           = 0;
        this->elapsed_time        = 0.0;

        this->termination_status =
            LocalSearchCoreTerminationStatus::ITERATION_OVER;

        this->current_move.initialize();
        this->previous_move.initialize();

        this->current_solution_score  = solution::SolutionScore();
        this->previous_solution_score = solution::SolutionScore();

        this->number_of_moves             = 0;
        this->number_of_performed_moves   = 0;
        this->is_found_improving_solution = false;
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