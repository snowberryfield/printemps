/*****************************************************************************/
// Copyright (c) 2020-2024 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_PDLP_CORE_PRIMAL_DUAL_STATE_H__
#define PRINTEMPS_SOLVER_PDLP_CORE_PRIMAL_DUAL_STATE_H__

namespace printemps::solver::pdlp::core {
/*****************************************************************************/
struct PrimalDualState {
    double objective;
    double absolute_violation_norm;
    double relative_violation_norm;

    double objective_coefficients_norm;
    double objective_lower_bound;
    double objective_upper_bound;

    utility::sparse::Vector solution;
    utility::sparse::Vector lhs;
    utility::sparse::Vector direction;
    utility::sparse::Vector solution_trial;
    utility::sparse::Vector move;
    utility::sparse::Vector lagrangian_coefficients;
    utility::sparse::Vector reduced_cost_coefficients;
    utility::sparse::Vector solution_baseline;
    utility::sparse::Vector solution_average;

    utility::sparse::Vector buffer;

    utility::sparse::Vector *solution_restart_candidate_ptr;

    /*************************************************************************/
    PrimalDualState(void) {
        this->initialize();
    }

    /*************************************************************************/
    PrimalDualState(const std::size_t a_SIZE) {
        this->setup(a_SIZE);
    }

    /*************************************************************************/
    void initialize(void) {
        this->objective               = 0.0;
        this->absolute_violation_norm = 0.0;
        this->relative_violation_norm = 0.0;

        this->objective_coefficients_norm = 0.0;

        this->objective_lower_bound = std::numeric_limits<double>::max();
        this->objective_upper_bound = std::numeric_limits<double>::lowest();

        this->solution.initialize();
        this->lhs.initialize();
        this->direction.initialize();
        this->solution_trial.initialize();
        this->move.initialize();
        this->lagrangian_coefficients.initialize();
        this->reduced_cost_coefficients.initialize();
        this->solution_baseline.initialize();
        this->solution_average.initialize();
        this->buffer.initialize();

        this->solution_restart_candidate_ptr = nullptr;
    }

    /*************************************************************************/
    inline void setup(const int a_SIZE) {
        this->absolute_violation_norm = 0.0;
        this->relative_violation_norm = 0.0;

        this->objective_coefficients_norm = 0.0;

        this->objective_lower_bound = std::numeric_limits<double>::max();
        this->objective_upper_bound = std::numeric_limits<double>::lowest();

        this->solution                  = utility::sparse::Vector(a_SIZE);
        this->lhs                       = utility::sparse::Vector(a_SIZE);
        this->direction                 = utility::sparse::Vector(a_SIZE);
        this->solution_trial            = utility::sparse::Vector(a_SIZE);
        this->move                      = utility::sparse::Vector(a_SIZE);
        this->lagrangian_coefficients   = utility::sparse::Vector(a_SIZE);
        this->reduced_cost_coefficients = utility::sparse::Vector(a_SIZE);
        this->solution_baseline         = utility::sparse::Vector(a_SIZE);
        this->solution_average          = utility::sparse::Vector(a_SIZE);
        this->buffer                    = utility::sparse::Vector(a_SIZE);

        this->solution_restart_candidate_ptr = nullptr;
    }
};
}  // namespace printemps::solver::pdlp::core

#endif
/*****************************************************************************/
// END
/*****************************************************************************/