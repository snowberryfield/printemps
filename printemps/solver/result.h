/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef printemps_SOLVER_RESULT_H__
#define printemps_SOLVER_RESULT_H__

namespace printemps {
namespace solver {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
struct Result {
    model::NamedSolution<T_Variable, T_Expression> solution;
    Status                                         status;
    SolutionArchive<T_Variable, T_Expression>      solution_archive;

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
        this->status.initialize();
        this->solution_archive.initialize();
    }
};
}  // namespace solver
}  // namespace printemps

#endif
/*****************************************************************************/
// END
/*****************************************************************************/