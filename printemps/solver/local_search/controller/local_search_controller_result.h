/*****************************************************************************/
// Copyright (c) 2020-2023 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_LOCAL_SEARCH_CORE_LOCAL_SEARCH_CORE_CONTROLLER_RESULT_H__
#define PRINTEMPS_SOLVER_LOCAL_SEARCH_CORE_LOCAL_SEARCH_CORE_CONTROLLER_RESULT_H__

namespace printemps::solver::local_search::controller {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
struct LocalSearchControllerResult {
    int update_status;
    int number_of_iterations;

    /*************************************************************************/
    LocalSearchControllerResult(void) {
        this->initialize();
    }

    /*************************************************************************/
    LocalSearchControllerResult(
        const core::LocalSearchCoreResult<T_Variable, T_Expression> &a_RESULT)
        : update_status(a_RESULT.total_update_status),
          number_of_iterations(a_RESULT.number_of_iterations) {
        /// nothing to do
    }

    /*************************************************************************/
    inline void initialize(void) {
        this->update_status =
            solution::IncumbentHolderConstant::STATUS_NOT_UPDATED;
        this->number_of_iterations = 0;
    }
};
}  // namespace printemps::solver::local_search::controller

#endif
/*****************************************************************************/
// END
/*****************************************************************************/