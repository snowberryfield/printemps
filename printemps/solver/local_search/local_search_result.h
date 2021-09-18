/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_LOCAL_SEARCH_LOCAL_SEARCH_RESULT_H__
#define PRINTEMPS_SOLVER_LOCAL_SEARCH_LOCAL_SEARCH_RESULT_H__

namespace printemps {
namespace solver {
namespace local_search {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
struct LocalSearchResult {
    solution::IncumbentHolder<T_Variable, T_Expression> incumbent_holder;
    Memory                                              memory;

    int total_update_status;
    int number_of_iterations;

    LocalSearchTerminationStatus termination_status;

    std::vector<solution::SparseSolution<T_Variable, T_Expression>>
        feasible_solutions;

    /*************************************************************************/
    LocalSearchResult(void) {
        this->initialize();
    }

    /*************************************************************************/
    void initialize(void) {
        this->incumbent_holder.initialize();
        this->memory.initialize();
        this->total_update_status  = 0;
        this->number_of_iterations = 0;
        this->termination_status = LocalSearchTerminationStatus::ITERATION_OVER;
        this->feasible_solutions.clear();
    }
};
}  // namespace local_search
}  // namespace solver
}  // namespace printemps

#endif
/*****************************************************************************/
// END
/*****************************************************************************/