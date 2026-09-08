/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_MATRIX_MODEL_MATRIX_MODEL_H__
#define PRINTEMPS_MATRIX_MODEL_MATRIX_MODEL_H__

#include <vector>
#include <utility>
#include "utility/sparse/sparse.h"
#include "utility/bidirectional_map.h"
#include "model_handler/reference.h"

namespace printemps::matrix_model {
/*****************************************************************************/
struct MatrixModel {
    int number_of_variables;
    int number_of_constraints;

    double objective_offset;

    utility::sparse::Vector objective_coefficients;

    // Primal Variable Bounds & Initial Solution
    utility::sparse::Vector variable_lower_bounds;
    utility::sparse::Vector variable_upper_bounds;
    utility::sparse::Vector variable_initial_solution;
    std::vector<int>        is_variable_lower_unbounded;
    std::vector<int>        is_variable_upper_unbounded;

    // Constraint Matrix & Bounds
    utility::sparse::SparseMatrix constraint_coefficients;
    utility::sparse::Vector       constraint_lower_bounds;
    utility::sparse::Vector       constraint_upper_bounds;

    // Dual variables (primarily for LP Duality / PDLP)
    utility::sparse::Vector       dual_objective_coefficients;
    utility::sparse::Vector       dual_lower_bounds;
    utility::sparse::Vector       dual_upper_bounds;
    utility::sparse::Vector       dual_initial_solution;
    utility::sparse::SparseMatrix dual_constraint_coefficients;

    // CDCL specific metadata

    // Ranges & Properties
    std::pair<int, int> less_constraint_index_range;
    std::pair<int, int> equal_constraint_index_range;
    std::pair<int, int> greater_constraint_index_range;
    bool                is_minimization;

    // Model reference and bidirectional mapping dictionaries
    const model_handler::Reference<int, double> *reference_ptr;
    utility::BidirectionalMap<model_component::Variable<int, double> *, int>
        variable_index_map;
    utility::BidirectionalMap<model_component::Constraint<int, double> *, int>
        constraint_index_map;

    /*************************************************************************/
    MatrixModel(void) {
        this->initialize();
    }

    /*************************************************************************/
    inline void initialize(void) {
        this->number_of_variables   = 0;
        this->number_of_constraints = 0;

        this->objective_offset = 0.0;

        this->objective_coefficients.initialize();

        this->variable_lower_bounds.initialize();
        this->variable_upper_bounds.initialize();
        this->variable_initial_solution.initialize();
        this->is_variable_lower_unbounded.clear();
        this->is_variable_upper_unbounded.clear();

        this->constraint_coefficients.initialize();
        this->constraint_lower_bounds.initialize();
        this->constraint_upper_bounds.initialize();

        this->dual_objective_coefficients.initialize();
        this->dual_lower_bounds.initialize();
        this->dual_upper_bounds.initialize();
        this->dual_initial_solution.initialize();
        this->dual_constraint_coefficients.initialize();

        this->less_constraint_index_range    = {0, 0};
        this->equal_constraint_index_range   = {0, 0};
        this->greater_constraint_index_range = {0, 0};

        this->is_minimization = true;

        this->reference_ptr = nullptr;
        this->variable_index_map.initialize();
        this->constraint_index_map.initialize();
    }

    /*************************************************************************/
    inline void scaling(const int  a_COUNTS_OF_RUIZ_SCALING,
                        const bool a_IS_ENABLED_POCK_CHAMBOLLE_SCALING) {
        if (a_COUNTS_OF_RUIZ_SCALING > 0) {
            this->constraint_coefficients.ruiz_scaling(
                a_COUNTS_OF_RUIZ_SCALING);
        }

        if (a_IS_ENABLED_POCK_CHAMBOLLE_SCALING) {
            this->constraint_coefficients.pock_chambolle_scaling();
        }

        this->dual_constraint_coefficients =
            utility::sparse::SparseMatrix(constraint_coefficients.transpose());

        const auto &ROW_SCALER    = this->constraint_coefficients.row_scaler;
        const auto &COLUMN_SCALER = this->constraint_coefficients.column_scaler;

        const auto &ROW_SCALER_INVERSE    = ROW_SCALER.inverse();
        const auto &COLUMN_SCALER_INVERSE = COLUMN_SCALER.inverse();

        COLUMN_SCALER_INVERSE.dot(&this->objective_coefficients);
        COLUMN_SCALER.dot(&this->variable_lower_bounds);
        COLUMN_SCALER.dot(&this->variable_upper_bounds);
        COLUMN_SCALER.dot(&this->variable_initial_solution);

        ROW_SCALER_INVERSE.dot(&this->dual_objective_coefficients);
        ROW_SCALER.dot(&this->dual_lower_bounds);
        ROW_SCALER.dot(&this->dual_upper_bounds);
        ROW_SCALER.dot(&this->dual_initial_solution);
    }
};
}  // namespace printemps::matrix_model
#endif
/*****************************************************************************/
// END
/*****************************************************************************/
