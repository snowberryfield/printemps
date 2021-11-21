/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_STATE_H__
#define PRINTEMPS_SOLVER_STATE_H__

namespace printemps {
namespace solver {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
struct State {
    solution::DenseSolution<T_Variable, T_Expression> current_solution;
    solution::DenseSolution<T_Variable, T_Expression> previous_solution;

    solution::SolutionScore current_solution_score;
    solution::SolutionScore previous_solution_score;

    int update_status;

    int number_of_lagrange_dual_iterations;
    int number_of_local_search_iterations;
    int number_of_tabu_search_iterations;
    int number_of_tabu_search_loops;

    bool has_special_neighborhood_moves;
    bool is_terminated;

    /*************************************************************************/
    State(void) {
        this->initialize();
    }

    /*************************************************************************/
    inline void initialize(void) {
        this->current_solution.initialize();
        this->previous_solution.initialize();

        this->update_status                      = 0;
        this->number_of_lagrange_dual_iterations = 0;
        this->number_of_local_search_iterations  = 0;
        this->number_of_tabu_search_iterations   = 0;
        this->number_of_tabu_search_loops        = 0;

        this->has_special_neighborhood_moves = 0;
        this->is_terminated                  = 0;
    }
};
}  // namespace solver
}  // namespace printemps

#endif
/*****************************************************************************/
// END
/*****************************************************************************/