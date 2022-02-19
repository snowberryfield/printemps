/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_TABU_SEARCH_CONTROLLER_TABU_SEARCH_CONTROLLER_PARAMETER_H__
#define PRINTEMPS_SOLVER_TABU_SEARCH_CONTROLLER_TABU_SEARCH_CONTROLLER_PARAMETER_H__

namespace printemps {
namespace solver {
namespace tabu_search {
namespace controller {
/*****************************************************************************/
struct TabuSearchControllerParameter {
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

    /*************************************************************************/
    TabuSearchControllerParameter(void) {
        this->initialize();
    }

    /*************************************************************************/
    void initialize(void) {
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