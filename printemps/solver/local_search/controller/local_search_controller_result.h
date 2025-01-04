/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_LOCAL_SEARCH_CONTROLLER_LOCAL_SEARCH_CONTROLLER_RESULT_H__
#define PRINTEMPS_SOLVER_LOCAL_SEARCH_CONTROLLER_LOCAL_SEARCH_CONTROLLER_RESULT_H__

#include "../core/local_search_core_result.h"

namespace printemps::solver::local_search::controller {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
struct LocalSearchControllerResult {
    core::LocalSearchCoreResult<T_Variable, T_Expression> core;

    /*************************************************************************/
    LocalSearchControllerResult(void) {
        this->initialize();
    }

    /*************************************************************************/
    LocalSearchControllerResult(
        const core::LocalSearchCoreResult<T_Variable, T_Expression>
            &a_CORE_RESULT)
        : core(a_CORE_RESULT) {
        /// nothing to do
    }

    /*************************************************************************/
    inline void initialize(void) {
        this->core.initialize();
    }
};
}  // namespace printemps::solver::local_search::controller

#endif
/*****************************************************************************/
// END
/*****************************************************************************/