/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_TABU_SEARCH_CONTROLLER_TABU_SEARCH_CONTROLLER_STATE_H__
#define PRINTEMPS_SOLVER_TABU_SEARCH_CONTROLLER_TABU_SEARCH_CONTROLLER_STATE_H__

namespace printemps::solver::tabu_search::controller {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
struct TabuSearchControllerState {
    /**
     * Pure states
     */
    double total_elapsed_time;
    double tabu_search_start_time;

    /**
     * This member will be calculated by total_elapsed_time -
     * tabu_search_start_time.
     */
    double tabu_search_elapsed_time;

    int iteration;
    int iteration_after_global_augmented_incumbent_update;
    int iteration_after_no_update;
    int iteration_after_relaxation;
    int relaxation_count;

    long total_number_of_inner_iterations;
    long total_number_of_evaluated_moves;

    double averaged_inner_iteration_speed;
    double averaged_move_evaluation_speed;

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

    int total_update_status;

    bool is_global_augmented_incumbent_updated;
    bool previous_is_feasible_incumbent_updated;
    bool current_is_feasible_incumbent_updated;
    bool is_not_updated;
    bool is_infeasible_stagnation;
    bool is_improved;
    bool is_exceeded_initial_penalty_coefficient;

    tabu_search::core::TabuSearchCoreResult<T_Variable, T_Expression>
        tabu_search_result;

    /**
     * Parameters
     */
    int    initial_tabu_tenure;
    double pruning_rate_threshold;
    int    number_of_initial_modification;
    int    iteration_max;
    bool   employing_local_augmented_solution_flag;
    bool   employing_global_augmented_solution_flag;
    bool   employing_previous_solution_flag;
    bool   is_enabled_penalty_coefficient_tightening;
    bool   is_enabled_penalty_coefficient_relaxing;
    bool   is_enabled_forcibly_initial_modification;
    bool   penalty_coefficient_reset_flag;
    double penalty_coefficient_relaxing_rate;
    double penalty_coefficient_tightening_rate;
    bool   is_enabled_special_neighborhood_move;
    bool   is_disabled_special_neighborhood_move;
    option::improvability_screening_mode::ImprovabilityScreeningMode
        improvability_screening_mode;

    bool is_enabled_parallel_neighborhood_update;
    bool is_enabled_parallel_evaluation;

    int neighborhood_update_parallelized_count;
    int evaluation_parallelized_count;

    /**
     * Learners
     */
    utility::ucb1::Learner<bool> neighborhood_update_parallelization_controller;
    utility::ucb1::Learner<bool> evaluation_parallelization_controller;

    /*************************************************************************/
    TabuSearchControllerState(void) {
        this->initialize();
    }

    /*************************************************************************/
    void initialize(void) {
        this->total_elapsed_time       = 0.0;
        this->tabu_search_start_time   = 0.0;
        this->tabu_search_elapsed_time = 0.0;

        this->iteration                                         = 0;
        this->iteration_after_global_augmented_incumbent_update = 0;
        this->iteration_after_no_update                         = 0;
        this->iteration_after_relaxation                        = 0;
        this->relaxation_count                                  = 0;

        this->total_number_of_inner_iterations = 0;
        this->total_number_of_evaluated_moves  = 0;
        this->averaged_inner_iteration_speed   = 0.0;
        this->averaged_move_evaluation_speed   = 0.0;

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

        this->total_update_status = 0;

        this->is_global_augmented_incumbent_updated   = false;
        this->previous_is_feasible_incumbent_updated  = false;
        this->current_is_feasible_incumbent_updated   = false;
        this->is_not_updated                          = false;
        this->is_infeasible_stagnation                = false;
        this->is_improved                             = false;
        this->is_exceeded_initial_penalty_coefficient = false;

        this->tabu_search_result.initialize();

        this->initial_tabu_tenure                       = 0;
        this->pruning_rate_threshold                    = 0.0;
        this->number_of_initial_modification            = 0;
        this->iteration_max                             = 0;
        this->employing_local_augmented_solution_flag   = false;
        this->employing_global_augmented_solution_flag  = false;
        this->employing_previous_solution_flag          = false;
        this->is_enabled_penalty_coefficient_tightening = false;
        this->is_enabled_penalty_coefficient_relaxing   = false;
        this->is_enabled_forcibly_initial_modification  = false;
        this->penalty_coefficient_reset_flag            = false;
        this->penalty_coefficient_relaxing_rate         = 0.0;
        this->penalty_coefficient_tightening_rate       = 0.0;
        this->is_enabled_special_neighborhood_move      = false;
        this->is_disabled_special_neighborhood_move     = false;
        this->improvability_screening_mode =
            option::improvability_screening_mode::Off;

        this->is_enabled_parallel_neighborhood_update = false;
        this->is_enabled_parallel_evaluation          = false;
        this->neighborhood_update_parallelized_count  = 0;
        this->evaluation_parallelized_count           = 0;

        this->neighborhood_update_parallelization_controller.initialize();
        this->evaluation_parallelization_controller.initialize();
    }
};
}  // namespace printemps::solver::tabu_search::controller
#endif
/*****************************************************************************/
// END
/*****************************************************************************/