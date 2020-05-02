/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef CPPMH_SOLVER_TABU_SEARCH_RESULT_H__
#define CPPMH_SOLVER_TABU_SEARCH_RESULT_H__

namespace cppmh {
namespace solver {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class IncumbentHolder;

namespace tabu_search {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
struct TabuSearchResult {
    IncumbentHolder<T_Variable, T_Expression> incumbent_holder;
    int                                       tabu_tenure;
    int                                       total_update_status;

    /*************************************************************************/
    TabuSearchResult(void) {
        this->initialize();
    }

    /*************************************************************************/
    virtual ~TabuSearchResult(void) {
        /// nothing to do
    }

    /*************************************************************************/
    inline void constexpr initialize(void) {
        this->incumbent_holder.initialize();
        this->tabu_tenure         = 0;
        this->total_update_status = 0;
    }
};
}  // namespace tabu_search
}  // namespace solver
}  // namespace cppmh

#endif
/*****************************************************************************/
// END
/*****************************************************************************/