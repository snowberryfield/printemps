/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef printemps_SOLVER_LOCAL_SEARCH_LOCAL_SEARCH_RESULT_H__
#define printemps_SOLVER_LOCAL_SEARCH_LOCAL_SEARCH_RESULT_H__

namespace printemps {
namespace solver {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class IncumbentHolder;

namespace local_search {
/*****************************************************************************/
enum class LocalSearchTerminationStatus {
    TIME_OVER,
    ITERATION_OVER,
    NO_MOVE,
    REACH_TARGET,
    LOCAL_OPTIMAL
};

/*****************************************************************************/
template <class T_Variable, class T_Expression>
struct LocalSearchResult {
    IncumbentHolder<T_Variable, T_Expression> incumbent_holder;
    Memory                                    memory;

    int total_update_status;
    int number_of_iterations;

    LocalSearchTerminationStatus termination_status;

    std::vector<model::PlainSolution<T_Variable, T_Expression>>
        historical_feasible_solutions;

    /*************************************************************************/
    LocalSearchResult(void) {
        this->initialize();
    }

    /*************************************************************************/
    virtual ~LocalSearchResult(void) {
        /// nothing to do
    }

    /*************************************************************************/
    inline void constexpr initialize(void) {
        this->incumbent_holder.initialize();
        this->memory.initialize();
        this->total_update_status  = 0;
        this->number_of_iterations = 0;
        this->termination_status = LocalSearchTerminationStatus::ITERATION_OVER;
        this->historical_feasible_solutions.clear();
    }
};
}  // namespace local_search
}  // namespace solver
}  // namespace printemps

#endif
/*****************************************************************************/
// END
/*****************************************************************************/