/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_LAGRANGE_DUAL_CORE_LAGRANGE_DUAL_CORE_STATE_H__
#define PRINTEMPS_SOLVER_LAGRANGE_DUAL_CORE_LAGRANGE_DUAL_CORE_STATE_H__

namespace printemps::solver::lagrange_dual::core {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
struct LagrangeDualCoreState {
    int    update_status;
    int    total_update_status;
    int    iteration;
    double elapsed_time;

    LagrangeDualCoreTerminationStatus termination_status;

    double lagrangian;
    double lagrangian_incumbent;
    double step_size;

    solution::SolutionScore current_solution_score;
    solution::SolutionScore previous_solution_score;

    solution::DenseSolution<T_Variable, T_Expression> primal;
    solution::DenseSolution<T_Variable, T_Expression> primal_incumbent;

    std::vector<multi_array::ValueProxy<double>> dual;
    std::vector<multi_array::ValueProxy<double>> dual_incumbent;

    utility::FixedSizeQueue<double> queue;

    /*************************************************************************/
    LagrangeDualCoreState(void) {
        this->initialize();
    }

    /*************************************************************************/
    void initialize(void) {
        this->update_status       = 0;
        this->total_update_status = 0;
        this->iteration           = 0;
        this->elapsed_time        = 0.0;

        this->termination_status =
            LagrangeDualCoreTerminationStatus::ITERATION_OVER;

        this->lagrangian           = 0.0;
        this->lagrangian_incumbent = 0.0;
        this->step_size            = 0.0;
        this->primal.initialize();
        this->primal_incumbent.initialize();
        this->dual.clear();
        this->dual_incumbent.clear();
        this->queue.initialize();
    }
};
}  // namespace printemps::solver::lagrange_dual::core

#endif
/*****************************************************************************/
// END
/*****************************************************************************/