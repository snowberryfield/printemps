/*****************************************************************************/
// Copyright (c) 2020-2023 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_TABU_SEARCH_CORE_TABU_SEARCH_CORE_STATE_H__
#define PRINTEMPS_SOLVER_TABU_SEARCH_CORE_TABU_SEARCH_CORE_STATE_H__

namespace printemps::solver::tabu_search::core {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
struct TabuSearchCoreState {
    /**
     * Pure states.
     */
    int    update_status;
    int    total_update_status;
    int    iteration;
    double elapsed_time;

    TabuSearchCoreTerminationStatus termination_status;

    neighborhood::Move<T_Variable, T_Expression> previous_move;
    neighborhood::Move<T_Variable, T_Expression> current_move;

    solution::SolutionScore current_solution_score;
    solution::SolutionScore previous_solution_score;

    utility::Range<double> objective_range;
    utility::Range<double> local_augmented_objective_range;
    utility::Range<double> global_augmented_objective_range;
    utility::Range<double> local_penalty_range;

    int number_of_effective_updates;

    int    number_of_moves;
    long   number_of_evaluated_moves;
    double elapsed_time_for_evaluating_moves;

    long   number_of_updated_moves;
    double elapsed_time_for_updating_moves;

    bool is_few_permissible_neighborhood;
    bool is_found_new_feasible_solution;
    bool is_aspirated;
    bool is_improved;

    int last_local_augmented_incumbent_update_iteration;
    int last_global_augmented_incumbent_update_iteration;
    int last_feasible_incumbent_update_iteration;
    int local_augmented_incumbent_update_count;

    int number_of_all_neighborhoods;
    int number_of_feasible_neighborhoods;
    int number_of_permissible_neighborhoods;
    int number_of_improvable_neighborhoods;

    /**
     * Parameters and related states.
     */
    int last_tabu_tenure_updated_iteration;

    double current_primal_intensity;
    double previous_primal_intensity;
    double current_dual_intensity;
    double previous_dual_intensity;

    double intensity_increase_count;
    double intensity_decrease_count;

    int original_tabu_tenure;
    int tabu_tenure;

    /*************************************************************************/
    TabuSearchCoreState(void) {
        this->initialize();
    }

    /*************************************************************************/
    inline void initialize(void) {
        /**
         * Pure states.
         */
        this->update_status       = 0;
        this->total_update_status = 0;
        this->iteration           = 0;
        this->elapsed_time        = 0.0;

        this->termination_status =
            TabuSearchCoreTerminationStatus::ITERATION_OVER;

        this->current_move.initialize();
        this->previous_move.initialize();

        this->current_solution_score  = solution::SolutionScore();
        this->previous_solution_score = solution::SolutionScore();

        this->objective_range.initialize();
        this->local_augmented_objective_range.initialize();
        this->global_augmented_objective_range.initialize();
        this->local_penalty_range.initialize();

        this->number_of_effective_updates = 0;

        this->number_of_moves                   = 0;
        this->number_of_evaluated_moves         = 0;
        this->elapsed_time_for_evaluating_moves = 0.0;

        this->number_of_updated_moves         = 0;
        this->elapsed_time_for_updating_moves = 0.0;

        this->is_few_permissible_neighborhood = false;
        this->is_found_new_feasible_solution  = false;
        this->is_aspirated                    = false;
        this->is_improved                     = false;

        this->last_local_augmented_incumbent_update_iteration  = 0;
        this->last_global_augmented_incumbent_update_iteration = 0;
        this->last_feasible_incumbent_update_iteration         = 0;
        this->local_augmented_incumbent_update_count           = 0;

        this->number_of_all_neighborhoods         = 0;
        this->number_of_feasible_neighborhoods    = 0;
        this->number_of_permissible_neighborhoods = 0;
        this->number_of_improvable_neighborhoods  = 0;

        /**
         * Parameters and related states.
         */
        this->last_tabu_tenure_updated_iteration = 0;

        this->current_primal_intensity  = 0.0;
        this->previous_primal_intensity = 0.0;
        this->current_dual_intensity    = 0.0;
        this->previous_dual_intensity   = 0.0;

        this->intensity_increase_count = 0;
        this->intensity_decrease_count = 0;

        this->original_tabu_tenure = 0;
        this->tabu_tenure          = 0;
    }
};
}  // namespace printemps::solver::tabu_search::core
#endif
/*****************************************************************************/
// END
/*****************************************************************************/