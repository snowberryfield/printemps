/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_GLOBAL_STATE_H__
#define PRINTEMPS_SOLVER_GLOBAL_STATE_H__

#include "memory.h"
#include "search_tree.h"

namespace printemps::solver {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class Solver;

/*****************************************************************************/
template <class T_Variable, class T_Expression>
struct GlobalState {
    model::Model<T_Variable, T_Expression>*   model_ptr;
    solver::Solver<T_Variable, T_Expression>* solver_ptr;

    solution::IncumbentHolder<T_Variable, T_Expression> incumbent_holder;
    Memory<T_Variable, T_Expression>                    memory;

    solution::SolutionArchive<T_Variable, T_Expression>
        feasible_solution_archive;
    solution::SolutionArchive<T_Variable, T_Expression>
        incumbent_solution_archive;

    SearchTree<T_Variable, T_Expression> search_tree;

    /*************************************************************************/
    GlobalState(void) {
        this->initialize();
    }

    /*************************************************************************/
    inline void initialize(void) {
        this->model_ptr  = nullptr;
        this->solver_ptr = nullptr;

        this->incumbent_holder.initialize();
        this->memory.initialize();
        this->feasible_solution_archive.initialize();
        this->incumbent_solution_archive.initialize();
        this->search_tree.initialize();
    }
};
using IPGlobalState = GlobalState<int, double>;
}  // namespace printemps::solver

#endif
/*****************************************************************************/
// END
/*****************************************************************************/