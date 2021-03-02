/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_TABU_SEARCH_RESULT_H__
#define PRINTEMPS_SOLVER_TABU_SEARCH_RESULT_H__

namespace printemps {
namespace solver {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class IncumbentHolder;

namespace tabu_search {
/*****************************************************************************/
enum class TabuSearchTerminationStatus {
    TIME_OVER,
    ITERATION_OVER,
    NO_MOVE,
    REACH_TARGET,
    EARLY_STOP,
    OPTIMAL
};

/*****************************************************************************/
template <class T_Variable, class T_Expression>
struct TabuSearchResult {
    IncumbentHolder<T_Variable, T_Expression> incumbent_holder;
    Memory                                    memory;

    int tabu_tenure;
    int total_update_status;
    int number_of_iterations;

    int last_local_augmented_incumbent_update_iteration;
    int last_global_augmented_incumbent_update_iteration;
    int last_feasible_incumbent_update_iteration;

    bool is_few_permissible_neighborhood;
    bool is_found_new_feasible_solution;

    double objective_constraint_rate;

    TabuSearchTerminationStatus termination_status;

    std::vector<model::PlainSolution<T_Variable, T_Expression>>
        historical_feasible_solutions;

    /*************************************************************************/
    TabuSearchResult(void) {
        this->initialize();
    }

    /*************************************************************************/
    virtual ~TabuSearchResult(void) {
        /// nothing to do
    }

    /*************************************************************************/
    void initialize(void) {
        this->incumbent_holder.initialize();
        this->memory.initialize();

        this->tabu_tenure          = 0;
        this->total_update_status  = 0;
        this->number_of_iterations = 0;

        this->last_local_augmented_incumbent_update_iteration  = -1;
        this->last_global_augmented_incumbent_update_iteration = -1;
        this->last_feasible_incumbent_update_iteration         = -1;

        this->is_few_permissible_neighborhood = false;
        this->is_found_new_feasible_solution  = false;

        this->objective_constraint_rate = 1.0;

        this->termination_status = TabuSearchTerminationStatus::ITERATION_OVER;

        this->historical_feasible_solutions.clear();
    }
};
}  // namespace tabu_search
}  // namespace solver
}  // namespace printemps

#endif
/*****************************************************************************/
// END
/*****************************************************************************/