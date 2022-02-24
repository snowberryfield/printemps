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
struct TabuSearchResult {
    int                         total_update_status;
    int                         number_of_iterations;
    TabuSearchTerminationStatus termination_status;

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
    TabuSearchResult(void) {
        this->initialize();
    }

    /*************************************************************************/
    TabuSearchResult(
        const int                         a_TOTAL_UPDATE_STATUS,          //
        const int                         a_NUMBER_OF_ITERATIONS,         //
        const TabuSearchTerminationStatus a_TERMINATION_STATUS,           //
        const int                         a_TABU_TENURE,                  //
        const int    a_LAST_LOCAL_AUGMENTED_INCUMBENT_UPDATE_ITERATION,   //
        const int    a_LAST_GLOBAL_AUGMENTED_INCUMBENT_UPDATE_ITERATION,  //
        const int    a_LAST_FEASIBLE_INCUMBENT_UPDATE_ITERATION,          //
        const bool   a_IS_FEW_PERMISSIBLE_NEIGHBORHOOD,                   //
        const bool   a_IS_FOUND_NEW_FEASIBLE_SOLUTION,                    //
        const double a_OBJECTIVE_CONSTRAINT_RATE,                         //
        const double a_GLOBAL_AUGMENTED_OBJECTIVE_RANGE,
        const double a_PERFORMANCE)
        : total_update_status(a_TOTAL_UPDATE_STATUS),
          number_of_iterations(a_NUMBER_OF_ITERATIONS),
          termination_status(a_TERMINATION_STATUS),
          tabu_tenure(a_TABU_TENURE),
          last_local_augmented_incumbent_update_iteration(
              a_LAST_LOCAL_AUGMENTED_INCUMBENT_UPDATE_ITERATION),
          last_global_augmented_incumbent_update_iteration(
              a_LAST_GLOBAL_AUGMENTED_INCUMBENT_UPDATE_ITERATION),
          last_feasible_incumbent_update_iteration(
              a_LAST_FEASIBLE_INCUMBENT_UPDATE_ITERATION),
          is_few_permissible_neighborhood(a_IS_FEW_PERMISSIBLE_NEIGHBORHOOD),
          is_found_new_feasible_solution(a_IS_FOUND_NEW_FEASIBLE_SOLUTION),
          objective_constraint_rate(a_OBJECTIVE_CONSTRAINT_RATE),
          global_augmented_objective_range(a_GLOBAL_AUGMENTED_OBJECTIVE_RANGE),
          performance(a_PERFORMANCE) {
        /// nothing to do
    }

    /*************************************************************************/
    void initialize(void) {
        this->total_update_status  = 0;
        this->number_of_iterations = 0;
        this->termination_status = TabuSearchTerminationStatus::ITERATION_OVER;

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
};
}  // namespace core
}  // namespace tabu_search
}  // namespace solver
}  // namespace printemps

#endif
/*****************************************************************************/
// END
/*****************************************************************************/