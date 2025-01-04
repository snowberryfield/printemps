/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_LINEAR_PROGRAMMING_LINEAR_PROGRAMMING_H__
#define PRINTEMPS_LINEAR_PROGRAMMING_LINEAR_PROGRAMMING_H__

namespace printemps::linear_programming {
/*****************************************************************************/
struct LinearProgramming {
    int number_of_rows;
    int number_of_columns;

    double objective_offset;

    utility::sparse::Vector       primal_objective_coefficients;
    utility::sparse::Vector       primal_lower_bounds;
    utility::sparse::Vector       primal_upper_bounds;
    utility::sparse::Vector       primal_initial_solution;
    utility::sparse::SparseMatrix primal_constraint_coefficients;

    std::vector<int> is_primal_lower_unbounded;
    std::vector<int> is_primal_upper_unbounded;

    utility::sparse::Vector       dual_objective_coefficients;
    utility::sparse::Vector       dual_lower_bounds;
    utility::sparse::Vector       dual_upper_bounds;
    utility::sparse::Vector       dual_initial_solution;
    utility::sparse::SparseMatrix dual_constraint_coefficients;

    std::pair<int, int> less_constraint_index_range;
    std::pair<int, int> equal_constraint_index_range;
    std::pair<int, int> greater_constraint_index_range;

    bool is_minimization;

    /*************************************************************************/
    LinearProgramming(void) {
        this->initialize();
    }

    /*************************************************************************/
    inline void initialize(void) {
        this->number_of_rows    = 0;
        this->number_of_columns = 0;

        this->objective_offset = 0.0;

        this->primal_objective_coefficients.initialize();
        this->primal_lower_bounds.initialize();
        this->primal_upper_bounds.initialize();
        this->primal_initial_solution.initialize();
        this->primal_constraint_coefficients.initialize();

        this->is_primal_lower_unbounded.clear();
        this->is_primal_upper_unbounded.clear();

        this->dual_objective_coefficients.initialize();
        this->dual_lower_bounds.initialize();
        this->dual_upper_bounds.initialize();
        this->dual_initial_solution.initialize();
        this->dual_constraint_coefficients.initialize();

        this->less_constraint_index_range    = {0, 0};
        this->equal_constraint_index_range   = {0, 0};
        this->greater_constraint_index_range = {0, 0};

        this->is_minimization = true;
    }

    /*************************************************************************/
    inline void scaling(const int  a_COUNTS_OF_RUIZ_SCALING,
                        const bool a_IS_ENABLED_POCK_CHAMBOLLE_SCALING) {
        if (a_COUNTS_OF_RUIZ_SCALING > 0) {
            this->primal_constraint_coefficients.ruiz_scaling(
                a_COUNTS_OF_RUIZ_SCALING);
        }

        if (a_IS_ENABLED_POCK_CHAMBOLLE_SCALING) {
            this->primal_constraint_coefficients.pock_chambolle_scaling();
        }

        this->dual_constraint_coefficients = utility::sparse::SparseMatrix(
            primal_constraint_coefficients.transpose());

        const auto &ROW_SCALER =
            this->primal_constraint_coefficients.row_scaler;
        const auto &COLUMN_SCALER =
            this->primal_constraint_coefficients.column_scaler;

        const auto &ROW_SCALER_INVERSE    = ROW_SCALER.inverse();
        const auto &COLUMN_SCALER_INVERSE = COLUMN_SCALER.inverse();

        COLUMN_SCALER_INVERSE.dot(&this->primal_objective_coefficients);
        COLUMN_SCALER.dot(&this->primal_lower_bounds);
        COLUMN_SCALER.dot(&this->primal_upper_bounds);
        COLUMN_SCALER.dot(&this->primal_initial_solution);

        ROW_SCALER_INVERSE.dot(&this->dual_objective_coefficients);
        ROW_SCALER.dot(&this->dual_lower_bounds);
        ROW_SCALER.dot(&this->dual_upper_bounds);
        ROW_SCALER.dot(&this->dual_initial_solution);
    }
};
}  // namespace printemps::linear_programming
#endif
/*****************************************************************************/
// END
/*****************************************************************************/