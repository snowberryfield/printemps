/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef CPPMH_SOLVER_LOCAL_SEARCH_LOCAL_SEARCH_RESULT_H__
#define CPPMH_SOLVER_LOCAL_SEARCH_LOCAL_SEARCH_RESULT_H__

namespace cppmh {
namespace solver {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class IncumbentHolder;

/*****************************************************************************/
namespace local_search {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
struct LocalSearchResult {
    IncumbentHolder<T_Variable, T_Expression> incumbent_holder;
    int                                       total_update_status;

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
        this->total_update_status = 0;
    }
};
}  // namespace local_search
}  // namespace solver
}  // namespace cppmh

#endif
/*****************************************************************************/
// END
/*****************************************************************************/