/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef CPPMH_SOLVER_RESULT_H__
#define CPPMH_SOLVER_RESULT_H__

namespace cppmh {
namespace solver {

/*****************************************************************************/
template <class T_Variable, class T_Expression>
struct Result {
    model::NamedSolution<T_Variable, T_Expression> solution;
    // Status               status;

    /*************************************************************************/
    Result(void) {
        this->initialize();
    }

    /*************************************************************************/
    virtual ~Result(void) {
        /// nothing to do
    }

    /*************************************************************************/
    inline void constexpr initialize(void) {
        this->solution.initialize();
        // this->status.initialize();
    }
};
}  // namespace solver
}  // namespace cppmh

#endif
/*****************************************************************************/
// END
/*****************************************************************************/