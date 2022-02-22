/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_TABU_SEARCH_CONTROLLER_TABU_SEARCH_CONTROLLER_STATE_H__
#define PRINTEMPS_SOLVER_TABU_SEARCH_CONTROLLER_TABU_SEARCH_CONTROLLER_STATE_H__

namespace printemps {
namespace solver {
namespace tabu_search {
namespace controller {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
struct TabuSearchControllerState {
    int iteration;
    int iteration_after_global_augmented_incumbent_update;
    int iteration_after_no_update;
    int iteration_after_relaxation;
    int relaxation_count;

    solution::DenseSolution<T_Variable, T_Expression> current_solution;
    solution::DenseSolution<T_Variable, T_Expression> previous_solution;

    solution::SolutionScore current_solution_score;
    solution::SolutionScore previous_solution_score;

    double current_primal_intensity;
    double previous_primal_intensity;
    double current_primal_intensity_before_relaxation;
    double previous_primal_intensity_before_relaxation;

    double current_dual_intensity;
    double previous_dual_intensity;
    double current_dual_intensity_before_relaxation;
    double previous_dual_intensity_before_relaxation;

    int employing_local_augmented_solution_count_after_relaxation;
    int employing_global_augmented_solution_count_after_relaxation;
    int employing_previous_solution_count_after_relaxation;

    bool is_global_augmented_incumbent_updated;
    bool is_feasible_incumbent_updated;
    bool is_not_updated;
    bool is_infeasible_stagnation;
    bool is_improved;

    tabu_search::core::TabuSearchResult tabu_search_result;

    /*************************************************************************/
    TabuSearchControllerState(void) {
        this->initialize();
    }

    /*************************************************************************/
    void initialize(void) {
        this->iteration                                         = 0;
        this->iteration_after_global_augmented_incumbent_update = 0;
        this->iteration_after_no_update                         = 0;
        this->iteration_after_relaxation                        = 0;
        this->relaxation_count                                  = 0;

        this->current_solution.initialize();
        this->previous_solution.initialize();

        this->current_primal_intensity                    = 0.0;
        this->previous_primal_intensity                   = 0.0;
        this->current_primal_intensity_before_relaxation  = 0.0;
        this->previous_primal_intensity_before_relaxation = 0.0;

        this->current_dual_intensity                    = 0.0;
        this->previous_dual_intensity                   = 0.0;
        this->current_dual_intensity_before_relaxation  = 0.0;
        this->previous_dual_intensity_before_relaxation = 0.0;

        this->employing_local_augmented_solution_count_after_relaxation  = 0;
        this->employing_global_augmented_solution_count_after_relaxation = 0;
        this->employing_previous_solution_count_after_relaxation         = 0;

        this->is_global_augmented_incumbent_updated = false;
        this->is_feasible_incumbent_updated         = false;
        this->is_not_updated                        = false;
        this->is_infeasible_stagnation              = false;
        this->is_improved                           = false;

        this->tabu_search_result.initialize();
    }
};
}  // namespace controller
}  // namespace tabu_search
}  // namespace solver
}  // namespace printemps
#endif
/*****************************************************************************/
// END
/*****************************************************************************/