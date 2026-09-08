/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_MODEL_HANDLER_MATRIX_MODEL_HANDLER_H__
#define PRINTEMPS_MODEL_HANDLER_MATRIX_MODEL_HANDLER_H__

#include <map>
#include <cmath>
#include <stdexcept>
#include "matrix_model/matrix_model.h"
#include "utility/string_utility.h"

namespace printemps::model_handler {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class MatrixModelHandler {
   private:
    const model::Model<T_Variable, T_Expression> *m_model_ptr;

    utility::BidirectionalMap<
        model_component::Variable<T_Variable, T_Expression> *, int>
        m_variable_index_map;
    utility::BidirectionalMap<
        model_component::Constraint<T_Variable, T_Expression> *, int>
        m_constraint_index_map;

    /*************************************************************************/
    inline void setup_variable_index_map(void) {
        /**
         * Maps all active non-fixed variables to a flat index space [0, N-1].
         */
        const auto &REFERENCE = m_model_ptr->reference();

        m_variable_index_map.initialize();
        int variable_index = 0;
        for (auto &&variable_ptr :
             REFERENCE.variable_type.binary_variable_ptrs) {
            if (!variable_ptr->is_fixed()) {
                m_variable_index_map.insert(variable_ptr, variable_index++);
            }
        }
        for (auto &&variable_ptr :
             REFERENCE.variable_type.integer_variable_ptrs) {
            if (!variable_ptr->is_fixed()) {
                m_variable_index_map.insert(variable_ptr, variable_index++);
            }
        }
        for (auto &&variable_ptr :
             REFERENCE.variable_type.selection_variable_ptrs) {
            if (!variable_ptr->is_fixed()) {
                m_variable_index_map.insert(variable_ptr, variable_index++);
            }
        }
    }

    /*************************************************************************/
    inline void setup_constraint_index_map(void) {
        /**
         * Maps active constraints (Less, Equal, Greater, Selections)
         * to a flat index space [0, M-1].
         */
        const auto &REFERENCE = m_model_ptr->reference();
        m_constraint_index_map.initialize();
        int constraint_index = 0;

        for (auto &&constraint_ptr : REFERENCE.constraint.less_ptrs) {
            if (constraint_ptr->is_enabled()) {
                m_constraint_index_map.insert(constraint_ptr,
                                              constraint_index++);
            }
        }
        for (auto &&selection : m_model_ptr->selections()) {
            if (!selection.constraint_ptr->is_enabled()) {
                m_constraint_index_map.insert(selection.constraint_ptr,
                                              constraint_index++);
            }
        }
        for (auto &&constraint_ptr : REFERENCE.constraint.equal_ptrs) {
            if (constraint_ptr->is_enabled()) {
                m_constraint_index_map.insert(constraint_ptr,
                                              constraint_index++);
            }
        }
        for (auto &&constraint_ptr : REFERENCE.constraint.greater_ptrs) {
            if (constraint_ptr->is_enabled()) {
                m_constraint_index_map.insert(constraint_ptr,
                                              constraint_index++);
            }
        }
    }

    /*************************************************************************/
    inline void compute_constraint_ranges(
        std::pair<int, int> *a_less_range_ptr,
        std::pair<int, int> *a_equal_range_ptr,
        std::pair<int, int> *a_greater_range_ptr) const {
        /**
         * Determines the index ranges for different constraint senses in the
         * flat constraint index space. This assists block operations in LP
         * solvers.
         */
        const auto &REFERENCE = m_model_ptr->reference();

        int constraint_index = 0;
        int offset           = 0;

        for (auto &&constraint_ptr : REFERENCE.constraint.less_ptrs) {
            if (constraint_ptr->is_enabled()) {
                constraint_index++;
            }
        }
        *a_less_range_ptr = {offset, constraint_index};
        offset            = constraint_index;

        for (auto &&selection : m_model_ptr->selections()) {
            if (!selection.constraint_ptr->is_enabled()) {
                constraint_index++;
            }
        }
        for (auto &&constraint_ptr : REFERENCE.constraint.equal_ptrs) {
            if (constraint_ptr->is_enabled()) {
                constraint_index++;
            }
        }
        *a_equal_range_ptr = {offset, constraint_index};
        offset             = constraint_index;

        for (auto &&constraint_ptr : REFERENCE.constraint.greater_ptrs) {
            if (constraint_ptr->is_enabled()) {
                constraint_index++;
            }
        }
        *a_greater_range_ptr = {offset, constraint_index};
    }

    /*************************************************************************/
    inline double compute_objective_offset(void) const {
        /**
         * Computes the objective constant offset resulting from variables
         * that are fixed to specific values.
         */
        const auto &REFERENCE        = m_model_ptr->reference();
        double      objective_offset = 0.0;
        for (const auto &variable_ptr :
             REFERENCE.variable.fixed_variable_ptrs) {
            objective_offset +=
                variable_ptr->value() * variable_ptr->objective_sensitivity();
        }
        return objective_offset;
    }

    /*************************************************************************/
    inline void extract_variable_initial_solution(
        utility::sparse::Vector *a_variable_initial_solution_ptr) const {
        /**
         * Fills the initial solution vector using current values of
         * the mapped mutable variables.
         */
        const int MUTABLE_VARIABLES_SIZE = m_variable_index_map.size();
        for (auto i = 0; i < MUTABLE_VARIABLES_SIZE; i++) {
            (*a_variable_initial_solution_ptr)[i] =
                m_variable_index_map.reverse_at(i)->value();
        }
    }

    /*************************************************************************/
    inline void extract_variable_bounds(
        utility::sparse::Vector *a_variable_lower_bounds_ptr,
        utility::sparse::Vector *a_variable_upper_bounds_ptr,
        std::vector<int>        *a_is_variable_lower_unbounded_ptr,
        std::vector<int>        *a_is_variable_upper_unbounded_ptr) const {
        /**
         * Extracts lower and upper bounds for mutable variables, and
         * detects if the bounds correspond to unbounded conditions.
         */
        const int MUTABLE_VARIABLES_SIZE = m_variable_index_map.size();
        for (auto i = 0; i < MUTABLE_VARIABLES_SIZE; i++) {
            const auto &VARIABLE_PTR = m_variable_index_map.reverse_at(i);
            (*a_variable_lower_bounds_ptr)[i] = VARIABLE_PTR->lower_bound();
            (*a_variable_upper_bounds_ptr)[i] = VARIABLE_PTR->upper_bound();
            if (VARIABLE_PTR->lower_bound() < (constant::INT_HALF_MIN >> 1)) {
                (*a_is_variable_lower_unbounded_ptr)[i] = 1;
            }
            if (VARIABLE_PTR->upper_bound() > (constant::INT_HALF_MAX >> 1)) {
                (*a_is_variable_upper_unbounded_ptr)[i] = 1;
            }
        }
    }

    /*************************************************************************/
    inline void extract_dual_objective_coefficients(
        utility::sparse::Vector *a_dual_objective_coefficients_ptr) const {
        /**
         * Calculates objective coefficients for the dual problem.
         * Due to the coefficient sign flipping of Less (<=) constraints in
         * primal normalization, we keep the coefficient positive for Less
         * constraints, and flip the sign to negative for Greater (>=) and Equal
         * (=) constraints.
         */
        const int ENABLED_CONSTRAINTS_SIZE = m_constraint_index_map.size();
        for (auto i = 0; i < ENABLED_CONSTRAINTS_SIZE; i++) {
            const auto &CONSTRAINT_PTR = m_constraint_index_map.reverse_at(i);
            const auto &EXPRESSION     = CONSTRAINT_PTR->expression();

            double dual_objective_coefficient = EXPRESSION.constant_value();
            for (const auto &sensitivity : EXPRESSION.sensitivities()) {
                if (sensitivity.first->is_fixed()) {
                    dual_objective_coefficient +=
                        sensitivity.first->value() * sensitivity.second;
                }
            }

            if (CONSTRAINT_PTR->sense() ==
                model_component::ConstraintSense::Less) {
                (*a_dual_objective_coefficients_ptr)[i] =
                    dual_objective_coefficient;
            } else {
                (*a_dual_objective_coefficients_ptr)[i] =
                    -dual_objective_coefficient;
            }
        }
    }

    /*************************************************************************/
    inline void extract_dual_bounds(
        const std::pair<int, int> &a_LESS_RANGE,
        const std::pair<int, int> &a_GREATER_RANGE,
        utility::sparse::Vector   *a_dual_lower_bounds_ptr,
        utility::sparse::Vector   *a_dual_upper_bounds_ptr) const {
        /**
         * Sets bounds for dual variables.
         * Since all primal inequality constraints (Less/Greater) are normalized
         * into '>=' constraints, their corresponding dual variables are
         * constrained to be non-negative (lower bound is set to 0.0). Dual
         * variables for Equal constraints remain free (-infinity to +infinity).
         */
        const int ENABLED_CONSTRAINTS_SIZE = m_constraint_index_map.size();
        a_dual_lower_bounds_ptr->setup(ENABLED_CONSTRAINTS_SIZE,
                                       std::numeric_limits<double>::lowest());
        a_dual_upper_bounds_ptr->setup(ENABLED_CONSTRAINTS_SIZE,
                                       std::numeric_limits<double>::max());

        for (auto i = a_LESS_RANGE.first; i < a_LESS_RANGE.second; i++) {
            (*a_dual_lower_bounds_ptr)[i] = 0.0;
        }

        for (auto i = a_GREATER_RANGE.first; i < a_GREATER_RANGE.second; i++) {
            (*a_dual_lower_bounds_ptr)[i] = 0.0;
        }
    }

    /*************************************************************************/
    inline void extract_constraint_coefficients(
        std::vector<int>          *a_row_indices_ptr,
        std::vector<int>          *a_column_indices_ptr,
        std::vector<T_Expression> *a_values_ptr) const {
        /**
         * Extracts constraint coefficients.
         * For Less (<=) constraints, coefficients are multiplied by -1 to
         * format the constraint as a '>=' condition. Coordinates are stable
         * sorted by column index for memory efficiency.
         */
        const int ENABLED_CONSTRAINTS_SIZE = m_constraint_index_map.size();
        std::vector<std::tuple<int, int, double>> row_records;

        for (auto i = 0; i < ENABLED_CONSTRAINTS_SIZE; i++) {
            const auto &CONSTRAINT_PTR = m_constraint_index_map.reverse_at(i);
            const auto &EXPRESSION     = CONSTRAINT_PTR->expression();

            row_records.clear();

            for (const auto &sensitivity : EXPRESSION.sensitivities()) {
                if (sensitivity.first->is_fixed()) {
                    continue;
                }
                const auto COLUMN_INDEX =
                    m_variable_index_map.forward_at(sensitivity.first);
                const auto VALUE = sensitivity.second;

                if (CONSTRAINT_PTR->sense() ==
                    model_component::ConstraintSense::Less) {
                    row_records.emplace_back(i, COLUMN_INDEX, -VALUE);
                } else {
                    row_records.emplace_back(i, COLUMN_INDEX, VALUE);
                }
            }
            std::stable_sort(row_records.begin(), row_records.end(),
                             [](const auto &a_FIRST, const auto &a_SECOND) {
                                 return std::get<1>(a_FIRST) <
                                        std::get<1>(a_SECOND);
                             });
            for (const auto &record : row_records) {
                a_row_indices_ptr->push_back(std::get<0>(record));
                a_column_indices_ptr->push_back(std::get<1>(record));
                a_values_ptr->push_back(std::get<2>(record));
            }
        }
    }

    /*************************************************************************/
    inline void extract_constraint_bounds(
        utility::sparse::Vector *a_constraint_lower_bounds_ptr,
        utility::sparse::Vector *a_constraint_upper_bounds_ptr) const {
        /**
         * Sets constraint bounds.
         * Translates constraints from high-level 'Sense' to upper/lower bounds:
         *  - Less (<=):     constant_value <= -sum(a_j x_j) <= infinity
         *  - Greater (>=): -constant_value <=  sum(a_j x_j) <= infinity
         *  - Equal (=):    -constant_value <=  sum(a_j x_j) <= -constant_value
         */
        const int ENABLED_CONSTRAINTS_SIZE = m_constraint_index_map.size();
        for (auto i = 0; i < ENABLED_CONSTRAINTS_SIZE; i++) {
            const auto &CONSTRAINT_PTR = m_constraint_index_map.reverse_at(i);
            const auto &EXPRESSION     = CONSTRAINT_PTR->expression();

            double constant_value = EXPRESSION.constant_value();
            for (const auto &sensitivity : EXPRESSION.sensitivities()) {
                if (sensitivity.first->is_fixed()) {
                    constant_value +=
                        sensitivity.first->value() * sensitivity.second;
                }
            }

            if (CONSTRAINT_PTR->sense() ==
                model_component::ConstraintSense::Less) {
                (*a_constraint_lower_bounds_ptr)[i] = constant_value;
                (*a_constraint_upper_bounds_ptr)[i] = HUGE_VALF;
            } else if (CONSTRAINT_PTR->sense() ==
                       model_component::ConstraintSense::Equal) {
                (*a_constraint_lower_bounds_ptr)[i] = -constant_value;
                (*a_constraint_upper_bounds_ptr)[i] = -constant_value;
            } else if (CONSTRAINT_PTR->sense() ==
                       model_component::ConstraintSense::Greater) {
                (*a_constraint_lower_bounds_ptr)[i] = -constant_value;
                (*a_constraint_upper_bounds_ptr)[i] = HUGE_VALF;
            }
        }
    }

   public:
    /*************************************************************************/
    MatrixModelHandler(void) {
        this->initialize();
    }

    /*************************************************************************/
    MatrixModelHandler(
        const model::Model<T_Variable, T_Expression> *a_model_ptr) {
        this->setup(a_model_ptr);
    }

    /*************************************************************************/
    inline void initialize(void) noexcept {
        m_model_ptr = nullptr;
        m_variable_index_map.initialize();
        m_constraint_index_map.initialize();
    }

    /*************************************************************************/
    inline void setup(
        const model::Model<T_Variable, T_Expression> *a_model_ptr) noexcept {
        this->initialize();
        m_model_ptr = a_model_ptr;
    }

    /*************************************************************************/
    inline matrix_model::MatrixModel export_matrix_model(void) {
        /**
         * Builds and exports the flat MatrixModel from m_model_ptr.
         */
        const auto               &model = *m_model_ptr;
        matrix_model::MatrixModel mm;

        // 1. Variable Mapping & General Integer Validation
        this->setup_variable_index_map();

        // 2. Constraint Mapping
        this->setup_constraint_index_map();

        std::pair<int, int> less_enabled_constraint_index_range    = {0, 0};
        std::pair<int, int> equal_enabled_constraint_index_range   = {0, 0};
        std::pair<int, int> greater_enabled_constraint_index_range = {0, 0};
        this->compute_constraint_ranges(
            &less_enabled_constraint_index_range,
            &equal_enabled_constraint_index_range,
            &greater_enabled_constraint_index_range);

        using utility::sparse::SparseMatrix;
        using utility::sparse::Vector;

        // 3. Objective Offset
        mm.objective_offset = this->compute_objective_offset();

        const int MUTABLE_VARIABLES_SIZE   = m_variable_index_map.size();
        const int ENABLED_CONSTRAINTS_SIZE = m_constraint_index_map.size();
        const int NONZERO_RESERVATION      = MUTABLE_VARIABLES_SIZE * 5;

        // 4. Objective Coefficients & Variable Bounds
        Vector           objective_coefficients(MUTABLE_VARIABLES_SIZE, 0.0);
        Vector           variable_lower_bounds(MUTABLE_VARIABLES_SIZE, 0.0);
        Vector           variable_upper_bounds(MUTABLE_VARIABLES_SIZE, 0.0);
        Vector           variable_initial_solution(MUTABLE_VARIABLES_SIZE, 0.0);
        std::vector<int> is_variable_lower_unbounded(MUTABLE_VARIABLES_SIZE, 0);
        std::vector<int> is_variable_upper_unbounded(MUTABLE_VARIABLES_SIZE, 0);

        this->extract_variable_initial_solution(&variable_initial_solution);
        this->extract_variable_bounds(
            &variable_lower_bounds, &variable_upper_bounds,
            &is_variable_lower_unbounded, &is_variable_upper_unbounded);

        for (auto i = 0; i < MUTABLE_VARIABLES_SIZE; i++) {
            const auto &VARIABLE_PTR  = m_variable_index_map.reverse_at(i);
            objective_coefficients[i] = VARIABLE_PTR->objective_sensitivity();
        }
        variable_initial_solution.clamp(variable_lower_bounds,
                                        variable_upper_bounds);

        // 5. Dual Vectors Setup
        Vector dual_objective_coefficients(ENABLED_CONSTRAINTS_SIZE, 0.0);
        Vector dual_lower_bounds;
        Vector dual_upper_bounds;
        Vector dual_initial_solution(ENABLED_CONSTRAINTS_SIZE, 0.0);

        this->extract_dual_objective_coefficients(&dual_objective_coefficients);
        this->extract_dual_bounds(less_enabled_constraint_index_range,
                                  greater_enabled_constraint_index_range,
                                  &dual_lower_bounds, &dual_upper_bounds);

        // 6. Constraint Bounds
        Vector constraint_lower_bounds(ENABLED_CONSTRAINTS_SIZE, 0.0);
        Vector constraint_upper_bounds(ENABLED_CONSTRAINTS_SIZE, 0.0);

        this->extract_constraint_bounds(&constraint_lower_bounds,
                                        &constraint_upper_bounds);

        std::vector<int>          row_indices;
        std::vector<int>          column_indices;
        std::vector<T_Expression> values;

        row_indices.reserve(MUTABLE_VARIABLES_SIZE);
        column_indices.reserve(MUTABLE_VARIABLES_SIZE);
        values.reserve(NONZERO_RESERVATION);

        this->extract_constraint_coefficients(&row_indices, &column_indices,
                                              &values);

        SparseMatrix constraint_coefficients(
            values, row_indices, column_indices, ENABLED_CONSTRAINTS_SIZE,
            MUTABLE_VARIABLES_SIZE);

        mm.number_of_constraints = ENABLED_CONSTRAINTS_SIZE;
        mm.number_of_variables   = MUTABLE_VARIABLES_SIZE;

        mm.constraint_coefficients = constraint_coefficients;
        mm.dual_constraint_coefficients =
            utility::sparse::SparseMatrix(constraint_coefficients.transpose());

        mm.objective_coefficients    = objective_coefficients;
        mm.variable_lower_bounds     = variable_lower_bounds;
        mm.variable_upper_bounds     = variable_upper_bounds;
        mm.variable_initial_solution = variable_initial_solution;

        mm.is_variable_lower_unbounded = is_variable_lower_unbounded;
        mm.is_variable_upper_unbounded = is_variable_upper_unbounded;

        mm.dual_objective_coefficients = dual_objective_coefficients;
        mm.dual_lower_bounds           = dual_lower_bounds;
        mm.dual_upper_bounds           = dual_upper_bounds;
        mm.dual_initial_solution       = dual_initial_solution;

        mm.constraint_lower_bounds = constraint_lower_bounds;
        mm.constraint_upper_bounds = constraint_upper_bounds;

        mm.less_constraint_index_range  = less_enabled_constraint_index_range;
        mm.equal_constraint_index_range = equal_enabled_constraint_index_range;
        mm.greater_constraint_index_range =
            greater_enabled_constraint_index_range;

        mm.is_minimization = model.is_minimization();
        if (!mm.is_minimization) {
            mm.objective_offset *= -1.0;
            mm.objective_coefficients *= -1.0;
        }

        /**
         * Set reference pointer and bidirectional index maps.
         */
        mm.reference_ptr        = &m_model_ptr->reference();
        mm.variable_index_map   = m_variable_index_map;
        mm.constraint_index_map = m_constraint_index_map;

        return mm;
    }

    /*************************************************************************/
    inline void import_solution(const std::vector<int> &a_SOLUTION) {
        /*
         * Imports flat solver solution back to high-level model variables.
         * Throws an exception if called before export or with a mismatched
         * size.
         */
        if (m_variable_index_map.size() == 0) {
            throw std::runtime_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The matrix model has not been exported yet. Call "
                "export_matrix_model() before calling import_solution()."));
        }
        if (static_cast<int>(a_SOLUTION.size()) !=
            m_variable_index_map.size()) {
            throw std::runtime_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The size of the solution vector does not match the number of "
                "variables in the matrix model. Expected: " +
                    std::to_string(m_variable_index_map.size()) +
                    ", Actual: " + std::to_string(a_SOLUTION.size())));
        }
        for (int i = 0; i < static_cast<int>(a_SOLUTION.size()); i++) {
            auto variable_ptr = m_variable_index_map.reverse_at(i);
            variable_ptr->set_value_if_mutable(a_SOLUTION[i]);
        }
    }
};
}  // namespace printemps::model_handler
#endif
/*****************************************************************************/
// END
/*****************************************************************************/
