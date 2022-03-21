/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_TABU_SEARCH_CORE_RESULT_H__
#define PRINTEMPS_SOLVER_TABU_SEARCH_CORE_RESULT_H__

namespace printemps {
namespace solver {
namespace tabu_search {
namespace core {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
struct TabuSearchCoreResult {
    int  total_update_status;
    int  number_of_iterations;
    long number_of_evaluated_moves;

    TabuSearchCoreTerminationStatus termination_status;

    int tabu_tenure;
    int last_local_augmented_incumbent_update_iteration;
    int last_global_augmented_incumbent_update_iteration;
    int last_feasible_incumbent_update_iteration;

    bool is_few_permissible_neighborhood;
    bool is_found_new_feasible_solution;

    double objective_constraint_rate;
    double global_augmented_objective_range;
    double performance;

    /*************************************************************************/
    TabuSearchCoreResult(void) {
        this->initialize();
    }

    /*************************************************************************/
    TabuSearchCoreResult(
        const TabuSearchCoreState<T_Variable, T_Expression> &a_STATE) {
        this->setup(a_STATE);
    }

    /*************************************************************************/
    inline void initialize(void) {
        this->total_update_status       = 0;
        this->number_of_iterations      = 0;
        this->number_of_evaluated_moves = 0;

        this->termination_status =
            TabuSearchCoreTerminationStatus::ITERATION_OVER;

        this->tabu_tenure                                      = 0;
        this->last_local_augmented_incumbent_update_iteration  = -1;
        this->last_global_augmented_incumbent_update_iteration = -1;
        this->last_feasible_incumbent_update_iteration         = -1;

        this->is_few_permissible_neighborhood = false;
        this->is_found_new_feasible_solution  = false;

        this->objective_constraint_rate        = 1.0;
        this->global_augmented_objective_range = 0.0;
        this->performance                      = 0.0;
    }

    /*************************************************************************/
    inline void setup(
        const TabuSearchCoreState<T_Variable, T_Expression> &a_STATE) {
        this->total_update_status       = a_STATE.total_update_status;
        this->number_of_iterations      = a_STATE.iteration;
        this->number_of_evaluated_moves = a_STATE.number_of_evaluated_moves;
        this->termination_status        = a_STATE.termination_status;
        this->tabu_tenure               = a_STATE.tabu_tenure;
        this->last_local_augmented_incumbent_update_iteration =
            a_STATE.last_local_augmented_incumbent_update_iteration;
        this->last_global_augmented_incumbent_update_iteration =
            a_STATE.last_global_augmented_incumbent_update_iteration;
        this->last_feasible_incumbent_update_iteration =
            a_STATE.last_feasible_incumbent_update_iteration;
        this->is_few_permissible_neighborhood =
            a_STATE.is_few_permissible_neighborhood;
        this->is_found_new_feasible_solution =
            a_STATE.is_found_new_feasible_solution;

        auto abs_max_objective =
            std::max(fabs(a_STATE.max_objective), fabs(a_STATE.min_objective));
        auto objective_constraint_rate =
            std::max(1.0,
                     std::max(abs_max_objective,  //
                              a_STATE.max_objective - a_STATE.min_objective)) /
            std::max(1.0, a_STATE.min_local_penalty);
        auto global_augmented_objective_range =
            std::max(0.0, a_STATE.max_global_augmented_objective -
                              a_STATE.min_global_augmented_objective);

        auto performance =
            a_STATE.oscillation / a_STATE.iteration /
            std::max(1.0, a_STATE.max_local_augmented_objective -
                              a_STATE.min_local_augmented_objective);

        this->objective_constraint_rate = objective_constraint_rate;
        this->global_augmented_objective_range =
            global_augmented_objective_range;
        this->performance = performance;

        /// nothing to do
    }
};
}  // namespace core
}  // namespace tabu_search
}  // namespace solver
}  // namespace printemps

#endif
/*****************************************************************************/
// END
/*****************************************************************************/