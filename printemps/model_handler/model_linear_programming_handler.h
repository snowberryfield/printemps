/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_MODEL_HANDLER_MODEL_LINEAR_PROGRAMMING_HANDLER_H__
#define PRINTEMPS_MODEL_HANDLER_MODEL_LINEAR_PROGRAMMING_HANDLER_H__

namespace printemps::model_handler {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class ModelLinearProgrammingHandler {
   private:
    const model::Model<T_Variable, T_Expression> *m_model_ptr;

   public:
    /*************************************************************************/
    ModelLinearProgrammingHandler(void) {
        m_model_ptr = nullptr;
    }

    /*************************************************************************/
    ModelLinearProgrammingHandler(
        const model::Model<T_Variable, T_Expression> *a_model_ptr) {
        this->initialize();
        this->setup(a_model_ptr);
    }

    /*************************************************************************/
    inline void initialize(void) noexcept {
        m_model_ptr = nullptr;
    }

    /*************************************************************************/
    inline void setup(
        const model::Model<T_Variable, T_Expression> *a_model_ptr) noexcept {
        m_model_ptr = a_model_ptr;
    }

    /*************************************************************************/
    inline linear_programming::LinearProgramming export_lp_instance(
        void) const {
        const auto &model = *m_model_ptr;

        utility::BidirectionalMap<  //
            model_component::Variable<T_Variable, T_Expression> *, int>
            mutable_variable_index_map;
        utility::BidirectionalMap<  //
            model_component::Constraint<T_Variable, T_Expression> *, int>
            enabled_constraint_index_map;

        std::pair<int, int> less_enabled_constraint_index_range    = {0, 0};
        std::pair<int, int> equal_enabled_constraint_index_range   = {0, 0};
        std::pair<int, int> greater_enabled_constraint_index_range = {0, 0};

        linear_programming::LinearProgramming lp;

        const auto &REFERENCE = model.reference();

        int variable_index = 0;
        for (auto &&variable_ptr :
             REFERENCE.variable_type.binary_variable_ptrs) {
            if (!variable_ptr->is_fixed()) {
                mutable_variable_index_map.insert(variable_ptr,
                                                  variable_index++);
            }
        }
        for (auto &&variable_ptr :
             REFERENCE.variable_type.integer_variable_ptrs) {
            if (!variable_ptr->is_fixed()) {
                mutable_variable_index_map.insert(variable_ptr,
                                                  variable_index++);
            }
        }

        for (auto &&variable_ptr :
             REFERENCE.variable_type.selection_variable_ptrs) {
            if (!variable_ptr->is_fixed()) {
                mutable_variable_index_map.insert(variable_ptr,
                                                  variable_index++);
            }
        }

        int constraint_index = 0;
        int offset           = 0;
        for (auto &&constraint_ptr : REFERENCE.constraint.less_ptrs) {
            if (constraint_ptr->is_enabled()) {
                enabled_constraint_index_map.insert(constraint_ptr,
                                                    constraint_index++);
            }
        }
        less_enabled_constraint_index_range = {offset, constraint_index};
        offset                              = constraint_index;

        for (auto &&selection : model.selections()) {
            if (!selection.constraint_ptr->is_enabled()) {
                enabled_constraint_index_map.insert(selection.constraint_ptr,
                                                    constraint_index++);
            }
        }

        for (auto &&constraint_ptr : REFERENCE.constraint.equal_ptrs) {
            if (constraint_ptr->is_enabled()) {
                enabled_constraint_index_map.insert(constraint_ptr,
                                                    constraint_index++);
            }
        }

        equal_enabled_constraint_index_range = {offset, constraint_index};
        offset                               = constraint_index;

        for (auto &&constraint_ptr : REFERENCE.constraint.greater_ptrs) {
            if (constraint_ptr->is_enabled()) {
                enabled_constraint_index_map.insert(constraint_ptr,
                                                    constraint_index++);
            }
        }
        greater_enabled_constraint_index_range = {offset, constraint_index};
        offset += constraint_index;

        using utility::sparse::SparseMatrix;
        using utility::sparse::Vector;

        double objective_offset = 0.0;
        for (const auto &variable_ptr :
             REFERENCE.variable.fixed_variable_ptrs) {
            objective_offset +=
                variable_ptr->value() * variable_ptr->objective_sensitivity();
        }

        lp.objective_offset = objective_offset;

        const int MUTABLE_VARIABLES_SIZE = mutable_variable_index_map.size();
        const int ENABLED_CONSTRAINTS_SIZE =
            enabled_constraint_index_map.size();
        const int NONZERO_RESERVATION = MUTABLE_VARIABLES_SIZE * 5;

        std::vector<int> is_primal_lower_unbounded(MUTABLE_VARIABLES_SIZE, 0);
        std::vector<int> is_primal_upper_unbounded(MUTABLE_VARIABLES_SIZE, 0);

        Vector primal_objective_coefficients(MUTABLE_VARIABLES_SIZE, 0.0);
        Vector primal_lower_bounds(MUTABLE_VARIABLES_SIZE, 0.0);
        Vector primal_upper_bounds(MUTABLE_VARIABLES_SIZE, 0.0);
        Vector primal_initial_solution(MUTABLE_VARIABLES_SIZE, 0.0);

        for (auto i = 0; i < MUTABLE_VARIABLES_SIZE; i++) {
            primal_initial_solution[i] =
                mutable_variable_index_map.reverse_at(i)->value();
        }

        for (auto i = 0; i < MUTABLE_VARIABLES_SIZE; i++) {
            const auto &VARIABLE_PTR = mutable_variable_index_map.reverse_at(i);

            primal_objective_coefficients[i] =
                VARIABLE_PTR->objective_sensitivity();
            primal_lower_bounds[i] = VARIABLE_PTR->lower_bound();
            primal_upper_bounds[i] = VARIABLE_PTR->upper_bound();
            if (VARIABLE_PTR->lower_bound() < (constant::INT_HALF_MIN >> 1)) {
                is_primal_lower_unbounded[i] = 1;
            }
            if (VARIABLE_PTR->upper_bound() > (constant::INT_HALF_MAX >> 1)) {
                is_primal_upper_unbounded[i] = true;
            }
        }

        primal_initial_solution.clamp(primal_lower_bounds, primal_upper_bounds);

        Vector dual_objective_coefficients(ENABLED_CONSTRAINTS_SIZE, 0.0);
        Vector dual_lower_bounds(ENABLED_CONSTRAINTS_SIZE,
                                 std::numeric_limits<double>::lowest());
        Vector dual_upper_bounds(ENABLED_CONSTRAINTS_SIZE,
                                 std::numeric_limits<double>::max());
        Vector dual_initial_solution(ENABLED_CONSTRAINTS_SIZE, 0.0);

        for (auto i = 0; i < ENABLED_CONSTRAINTS_SIZE; i++) {
            const auto &CONSTRAINT_PTR =
                enabled_constraint_index_map.reverse_at(i);
            const auto &EXPRESSION = CONSTRAINT_PTR->expression();

            double dual_objective_coefficient = EXPRESSION.constant_value();
            for (const auto &sensitivity : EXPRESSION.sensitivities()) {
                if (sensitivity.first->is_fixed()) {
                    dual_objective_coefficient +=
                        sensitivity.first->value() * sensitivity.second;
                }
            }

            if (CONSTRAINT_PTR->sense() ==
                model_component::ConstraintSense::Less) {
                dual_objective_coefficients[i] = dual_objective_coefficient;
            } else {
                dual_objective_coefficients[i] = -dual_objective_coefficient;
            }
        }

        for (auto i = less_enabled_constraint_index_range.first;
             i < less_enabled_constraint_index_range.second; i++) {
            dual_lower_bounds[i] = 0.0;
        }

        for (auto i = greater_enabled_constraint_index_range.first;
             i < greater_enabled_constraint_index_range.second; i++) {
            dual_lower_bounds[i] = 0.0;
        }

        std::vector<int>          row_indices;
        std::vector<int>          column_indices;
        std::vector<T_Expression> values;

        row_indices.reserve(MUTABLE_VARIABLES_SIZE);
        column_indices.reserve(MUTABLE_VARIABLES_SIZE);
        values.reserve(NONZERO_RESERVATION);

        std::vector<std::tuple<int, int, double>> row_records;

        for (auto i = 0; i < ENABLED_CONSTRAINTS_SIZE; i++) {
            const auto &CONSTRAINT_PTR =
                enabled_constraint_index_map.reverse_at(i);
            const auto &EXPRESSION = CONSTRAINT_PTR->expression();

            row_records.clear();

            for (const auto &sensitivity : EXPRESSION.sensitivities()) {
                if (sensitivity.first->is_fixed()) {
                    continue;
                }
                const auto COLUMN_INDEX =
                    mutable_variable_index_map.forward_at(sensitivity.first);
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
                row_indices.push_back(std::get<0>(record));
                column_indices.push_back(std::get<1>(record));
                values.push_back(std::get<2>(record));
            }
        }

        SparseMatrix primal_constraint_coefficients(
            values, row_indices, column_indices, ENABLED_CONSTRAINTS_SIZE,
            MUTABLE_VARIABLES_SIZE);

        lp.number_of_rows    = ENABLED_CONSTRAINTS_SIZE;
        lp.number_of_columns = MUTABLE_VARIABLES_SIZE;

        lp.objective_offset = objective_offset;

        lp.primal_constraint_coefficients = primal_constraint_coefficients;
        lp.dual_constraint_coefficients   = utility::sparse::SparseMatrix(
            primal_constraint_coefficients.transpose());

        lp.primal_objective_coefficients = primal_objective_coefficients;
        lp.primal_lower_bounds           = primal_lower_bounds;
        lp.primal_upper_bounds           = primal_upper_bounds;
        lp.primal_initial_solution       = primal_initial_solution;

        lp.is_primal_lower_unbounded = is_primal_lower_unbounded;
        lp.is_primal_upper_unbounded = is_primal_upper_unbounded;

        lp.dual_objective_coefficients = dual_objective_coefficients;
        lp.dual_lower_bounds           = dual_lower_bounds;
        lp.dual_upper_bounds           = dual_upper_bounds;
        lp.dual_initial_solution       = dual_initial_solution;

        lp.less_constraint_index_range  = less_enabled_constraint_index_range;
        lp.equal_constraint_index_range = equal_enabled_constraint_index_range;
        lp.greater_constraint_index_range =
            greater_enabled_constraint_index_range;

        lp.is_minimization = model.is_minimization();
        if (!lp.is_minimization) {
            lp.objective_offset *= -1.0;
            lp.primal_objective_coefficients *= -1.0;
        }
        return lp;
    }
};
}  // namespace printemps::model_handler
#endif
/*****************************************************************************/
// END
/*****************************************************************************/