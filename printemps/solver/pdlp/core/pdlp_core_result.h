/*****************************************************************************/
// Copyright (c) 2020-2023 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_PDLP_CORE_PDLP_CORE_RESULT_H__
#define PRINTEMPS_SOLVER_PDLP_CORE_PDLP_CORE_RESULT_H__

namespace printemps::solver::pdlp::core {
/*****************************************************************************/
struct PDLPCoreResult {
    int    number_of_iterations;
    double elapsed_time;

    PrimalDualState primal;
    PrimalDualState dual;

    PDLPCoreTerminationStatus termination_status;

    option::Option option;

    /*************************************************************************/
    PDLPCoreResult(void) {
        this->initialize();
    }

    /*************************************************************************/
    PDLPCoreResult(const PDLPCoreState &a_STATE, const option::Option &a_OPTION)
        : number_of_iterations(a_STATE.total_iteration),
          elapsed_time(a_STATE.elapsed_time),
          primal(a_STATE.primal),
          dual(a_STATE.dual),
          termination_status(a_STATE.termination_status),
          option(a_OPTION) {
        /// nothing to do
    }

    /*************************************************************************/
    void initialize(void) {
        this->number_of_iterations = 0;
        this->elapsed_time         = 0.0;
        this->primal.initialize();
        this->dual.initialize();
        this->termination_status = PDLPCoreTerminationStatus::ITERATION_OVER;
        this->option.initialize();
    }

    /*************************************************************************/
    inline void scaling(const utility::sparse::Diagonal &a_ROW_SCALER,
                        const utility::sparse::Diagonal &a_COLUMN_SCALER) {
        const auto ROW_SCALER_INVERSE    = a_ROW_SCALER.inverse();
        const auto COLUMN_SCALER_INVERSE = a_COLUMN_SCALER.inverse();

        COLUMN_SCALER_INVERSE.dot(&this->primal.solution);
        COLUMN_SCALER_INVERSE.dot(&this->primal.lhs);
        COLUMN_SCALER_INVERSE.dot(&this->primal.direction);
        COLUMN_SCALER_INVERSE.dot(&this->primal.solution_trial);
        COLUMN_SCALER_INVERSE.dot(&this->primal.move);
        COLUMN_SCALER_INVERSE.dot(&this->primal.lagrangian_coefficients);
        COLUMN_SCALER_INVERSE.dot(&this->primal.reduced_cost_coefficients);
        COLUMN_SCALER_INVERSE.dot(&this->primal.solution_baseline);
        COLUMN_SCALER_INVERSE.dot(&this->primal.solution_average);
        COLUMN_SCALER_INVERSE.dot(&this->primal.buffer);

        ROW_SCALER_INVERSE.dot(&this->dual.solution);
        ROW_SCALER_INVERSE.dot(&this->dual.lhs);
        ROW_SCALER_INVERSE.dot(&this->dual.direction);
        ROW_SCALER_INVERSE.dot(&this->dual.solution_trial);
        ROW_SCALER_INVERSE.dot(&this->dual.move);
        ROW_SCALER_INVERSE.dot(&this->dual.lagrangian_coefficients);
        ROW_SCALER_INVERSE.dot(&this->dual.reduced_cost_coefficients);
        ROW_SCALER_INVERSE.dot(&this->dual.solution_baseline);
        ROW_SCALER_INVERSE.dot(&this->dual.solution_average);
        ROW_SCALER_INVERSE.dot(&this->dual.buffer);
    }
};
}  // namespace printemps::solver::pdlp::core

#endif
/*****************************************************************************/
// END
/*****************************************************************************/