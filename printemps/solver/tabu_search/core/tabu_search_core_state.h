/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_TABU_SEARCH_CORE_TABU_SEARCH_CORE_STATE_H__
#define PRINTEMPS_SOLVER_TABU_SEARCH_CORE_TABU_SEARCH_CORE_STATE_H__

namespace printemps {
namespace solver {
namespace tabu_search {
namespace core {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
struct TabuSearchCoreState {
    int update_status;
    int total_update_status;
    int iteration;
    int number_of_moves;

    TabuSearchCoreTerminationStatus termination_status;

    solution::SolutionScore current_solution_score;
    solution::SolutionScore previous_solution_score;

    neighborhood::Move<T_Variable, T_Expression> previous_move;
    neighborhood::Move<T_Variable, T_Expression> current_move;

    int original_tabu_tenure;
    int tabu_tenure;

    double current_primal_intensity;
    double previous_primal_intensity;
    double current_dual_intensity;
    double previous_dual_intensity;

    double intensity_increase_count;
    double intensity_decrease_count;

    int last_tabu_tenure_updated_iteration;

    int number_of_all_neighborhoods;
    int number_of_feasible_neighborhoods;
    int number_of_permissible_neighborhoods;
    int number_of_improvable_neighborhoods;

    int last_local_augmented_incumbent_update_iteration;
    int last_global_augmented_incumbent_update_iteration;
    int last_feasible_incumbent_update_iteration;

    int local_augmented_incumbent_update_count;

    int selected_index;

    bool is_few_permissible_neighborhood;
    bool is_found_new_feasible_solution;
    bool is_aspirated;
    bool is_special_neighborhood_move;

    long number_of_evaluated_moves;

    double min_objective;
    double max_objective;

    double min_local_augmented_objective;
    double max_local_augmented_objective;

    double min_global_augmented_objective;
    double max_global_augmented_objective;
    double min_local_penalty;

    double oscillation;

#if 1
    /*************************************************************************/
    TabuSearchCoreState(void) {
        this->initialize();
    }

    /*************************************************************************/
    inline void initialize(void) {
        this->update_status       = 0;
        this->total_update_status = 0;
        this->iteration           = 0;
        this->number_of_moves     = 0;
        this->termination_status =
            TabuSearchCoreTerminationStatus::ITERATION_OVER;

        this->current_move.initialize();
        this->previous_move.initialize();

        this->original_tabu_tenure = 0;
        this->tabu_tenure          = 0;

        this->current_primal_intensity  = 0.0;
        this->previous_primal_intensity = 0.0;
        this->current_dual_intensity    = 0.0;
        this->previous_dual_intensity   = 0.0;

        this->intensity_increase_count = 0;
        this->intensity_decrease_count = 0;

        this->last_tabu_tenure_updated_iteration = 0;

        this->number_of_all_neighborhoods         = 0;
        this->number_of_feasible_neighborhoods    = 0;
        this->number_of_permissible_neighborhoods = 0;
        this->number_of_improvable_neighborhoods  = 0;

        this->last_local_augmented_incumbent_update_iteration  = 0;
        this->last_global_augmented_incumbent_update_iteration = 0;
        this->last_feasible_incumbent_update_iteration         = 0;

        this->local_augmented_incumbent_update_count = 0;

        this->selected_index = 0;

        this->is_few_permissible_neighborhood = false;
        this->is_found_new_feasible_solution  = false;
        this->is_aspirated                    = false;
        this->is_special_neighborhood_move    = false;

        this->number_of_evaluated_moves = 0;

        this->min_objective = 0.0;
        this->max_objective = 0.0;

        this->min_local_augmented_objective = 0.0;
        this->max_local_augmented_objective = 0.0;

        this->min_global_augmented_objective = 0.0;
        this->max_global_augmented_objective = 0.0;
        this->min_local_penalty              = 0.0;

        this->oscillation = 0.0;
    }
#endif
};
}  // namespace core
}  // namespace tabu_search
}  // namespace solver
}  // namespace printemps

#endif
/*****************************************************************************/
// END
/*****************************************************************************/