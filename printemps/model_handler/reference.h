/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_MODEL_HANDLER_REFERENCE_H__
#define PRINTEMPS_MODEL_HANDLER_REFERENCE_H__

#include "variable_reference.h"
#include "variable_type_reference.h"
#include "constraint_reference.h"
#include "constraint_type_reference.h"

namespace printemps::model_handler {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
struct Reference {
    model::Model<T_Variable, T_Expression> *m_model_ptr;

    VariableReference<T_Variable, T_Expression>       variable;
    VariableTypeReference<T_Variable, T_Expression>   variable_type;
    ConstraintReference<T_Variable, T_Expression>     constraint;
    ConstraintTypeReference<T_Variable, T_Expression> constraint_type;

    std::unordered_map<std::string,
                       model_component::Variable<T_Variable, T_Expression> *>
        variable_name_map;
    std::unordered_map<std::string,
                       model_component::Constraint<T_Variable, T_Expression> *>
        constraint_name_map;

    /*************************************************************************/
    Reference(void) {
        this->initialize();
    }

    /*************************************************************************/
    Reference(model::Model<T_Variable, T_Expression> *a_model_ptr) {
        this->initialize();
        this->setup(a_model_ptr);
    }

    /*************************************************************************/
    void initialize(void) {
        m_model_ptr = nullptr;
        this->variable.initialize();
        this->variable_type.initialize();
        this->constraint.initialize();
        this->constraint_type.initialize();
    }

    /*************************************************************************/
    inline void setup(
        model::Model<T_Variable, T_Expression> *a_model_ptr) noexcept {
        m_model_ptr = a_model_ptr;
    }

    /*************************************************************************/
    inline void update_variable_reference() {
        VariableReference<T_Variable, T_Expression>     variable_reference;
        VariableTypeReference<T_Variable, T_Expression> reference_variable_type;

        for (auto &&proxy : m_model_ptr->variable_proxies()) {
            for (auto &&variable : proxy.flat_indexed_variables()) {
                auto variable_ptr = const_cast<
                    model_component::Variable<T_Variable, T_Expression> *>(
                    &variable);
                variable_reference.variable_ptrs.push_back(variable_ptr);
                if (variable.is_fixed()) {
                    variable_reference.fixed_variable_ptrs.push_back(
                        variable_ptr);
                } else {
                    variable_reference.mutable_variable_ptrs.push_back(
                        variable_ptr);
                }
                if (variable.type() == model_component::VariableType::Binary) {
                    reference_variable_type.binary_variable_ptrs.push_back(
                        variable_ptr);
                }
                if (variable.type() == model_component::VariableType::Integer) {
                    reference_variable_type.integer_variable_ptrs.push_back(
                        variable_ptr);
                }
                if (variable.type() ==
                    model_component::VariableType::Selection) {
                    reference_variable_type.selection_variable_ptrs.push_back(
                        variable_ptr);
                }

                if (variable.type() ==
                    model_component::VariableType::DependentBinary) {
                    reference_variable_type.dependent_binary_variable_ptrs
                        .push_back(variable_ptr);
                }

                if (variable.type() ==
                    model_component::VariableType::DependentInteger) {
                    reference_variable_type.dependent_integer_variable_ptrs
                        .push_back(variable_ptr);
                }
            }
        }
        this->variable      = variable_reference;
        this->variable_type = reference_variable_type;
    }

    /*************************************************************************/
    inline void update_constraint_reference() {
        ConstraintReference<T_Variable, T_Expression> constraint_reference;
        ConstraintTypeReference<T_Variable, T_Expression>
            constraint_type_reference;

        for (auto &&proxy : m_model_ptr->constraint_proxies()) {
            for (auto &&constraint : proxy.flat_indexed_constraints()) {
                auto constraint_ptr = const_cast<
                    model_component::Constraint<T_Variable, T_Expression> *>(
                    &constraint);

                constraint_reference.constraint_ptrs.push_back(constraint_ptr);
                if (constraint.is_enabled()) {
                    constraint_reference.enabled_constraint_ptrs.push_back(
                        constraint_ptr);

                    switch (constraint.sense()) {
                        case model_component::ConstraintSense::Less: {
                            constraint_reference.less_ptrs.push_back(
                                constraint_ptr);
                            break;
                        }
                        case model_component::ConstraintSense::Equal: {
                            constraint_reference.equal_ptrs.push_back(
                                constraint_ptr);
                            break;
                        }
                        case model_component::ConstraintSense::Greater: {
                            constraint_reference.greater_ptrs.push_back(
                                constraint_ptr);
                            break;
                        }
                        default: {
                            /** nothing to do*/
                        }
                    }
                } else {
                    constraint_reference.disabled_constraint_ptrs.push_back(
                        constraint_ptr);
                }

                auto type = constraint.type();

                switch (type) {
                    case model_component::ConstraintType::Singleton:
                        constraint_type_reference.singleton_ptrs.push_back(
                            constraint_ptr);
                        break;

                    case model_component::ConstraintType::ExclusiveOR:
                        constraint_type_reference.exclusive_or_ptrs.push_back(
                            constraint_ptr);
                        break;

                    case model_component::ConstraintType::ExclusiveNOR:
                        constraint_type_reference.exclusive_nor_ptrs.push_back(
                            constraint_ptr);
                        break;

                    case model_component::ConstraintType::InvertedIntegers:
                        constraint_type_reference.inverted_integers_ptrs
                            .push_back(constraint_ptr);
                        break;

                    case model_component::ConstraintType::BalancedIntegers:
                        constraint_type_reference.balanced_integers_ptrs
                            .push_back(constraint_ptr);
                        break;

                    case model_component::ConstraintType::ConstantSumIntegers:
                        constraint_type_reference.constant_sum_integers_ptrs
                            .push_back(constraint_ptr);
                        break;

                    case model_component::ConstraintType::
                        ConstantDifferenceIntegers:
                        constraint_type_reference
                            .constant_difference_integers_ptrs.push_back(
                                constraint_ptr);
                        break;

                    case model_component::ConstraintType::ConstantRatioIntegers:
                        constraint_type_reference.constant_ratio_integers_ptrs
                            .push_back(constraint_ptr);
                        break;

                    case model_component::ConstraintType::Aggregation:
                        constraint_type_reference.aggregation_ptrs.push_back(
                            constraint_ptr);
                        break;

                    case model_component::ConstraintType::Precedence:
                        constraint_type_reference.precedence_ptrs.push_back(
                            constraint_ptr);
                        break;

                    case model_component::ConstraintType::VariableBound:
                        constraint_type_reference.variable_bound_ptrs.push_back(
                            constraint_ptr);
                        break;

                    case model_component::ConstraintType::TrinomialExclusiveNOR:
                        constraint_type_reference.trinomial_exclusive_nor_ptrs
                            .push_back(constraint_ptr);
                        break;

                    case model_component::ConstraintType::SetPartitioning:
                        constraint_type_reference.set_partitioning_ptrs
                            .push_back(constraint_ptr);
                        break;

                    case model_component::ConstraintType::SetPacking:
                        constraint_type_reference.set_packing_ptrs.push_back(
                            constraint_ptr);
                        break;

                    case model_component::ConstraintType::SetCovering:
                        constraint_type_reference.set_covering_ptrs.push_back(
                            constraint_ptr);
                        break;

                    case model_component::ConstraintType::Cardinality:
                        constraint_type_reference.cardinality_ptrs.push_back(
                            constraint_ptr);
                        break;

                    case model_component::ConstraintType::InvariantKnapsack:
                        constraint_type_reference.invariant_knapsack_ptrs
                            .push_back(constraint_ptr);
                        break;

                    case model_component::ConstraintType::MultipleCovering:
                        constraint_type_reference.multiple_covering_ptrs
                            .push_back(constraint_ptr);
                        break;

                    case model_component::ConstraintType::BinaryFlow:
                        constraint_type_reference.binary_flow_ptrs.push_back(
                            constraint_ptr);
                        break;

                    case model_component::ConstraintType::IntegerFlow:
                        constraint_type_reference.integer_flow_ptrs.push_back(
                            constraint_ptr);
                        break;

                    case model_component::ConstraintType::SoftSelection:
                        constraint_type_reference.soft_selection_ptrs.push_back(
                            constraint_ptr);
                        break;

                    case model_component::ConstraintType::MinMax:
                        constraint_type_reference.min_max_ptrs.push_back(
                            constraint_ptr);
                        break;

                    case model_component::ConstraintType::MaxMin:
                        constraint_type_reference.max_min_ptrs.push_back(
                            constraint_ptr);
                        break;

                    case model_component::ConstraintType::Intermediate:
                        constraint_type_reference.intermediate_ptrs.push_back(
                            constraint_ptr);
                        break;

                    case model_component::ConstraintType::EquationKnapsack:
                        constraint_type_reference.equation_knapsack_ptrs
                            .push_back(constraint_ptr);
                        break;

                    case model_component::ConstraintType::BinPacking:
                        constraint_type_reference.bin_packing_ptrs.push_back(
                            constraint_ptr);
                        break;

                    case model_component::ConstraintType::Knapsack:
                        constraint_type_reference.knapsack_ptrs.push_back(
                            constraint_ptr);
                        break;

                    case model_component::ConstraintType::IntegerKnapsack:
                        constraint_type_reference.integer_knapsack_ptrs
                            .push_back(constraint_ptr);
                        break;

                    case model_component::ConstraintType::GF2:
                        constraint_type_reference.gf2_ptrs.push_back(
                            constraint_ptr);
                        break;

                    case model_component::ConstraintType::GeneralLinear:
                        constraint_type_reference.general_linear_ptrs.push_back(
                            constraint_ptr);
                        break;
                    case model_component::ConstraintType::Unknown:
                        throw std::runtime_error(utility::format_error_location(
                            __FILE__, __LINE__, __func__,
                            "Unknown-Type constraint was found."));
                }
            }
        }
        this->constraint      = constraint_reference;
        this->constraint_type = constraint_type_reference;
    }

    /*************************************************************************/
    inline void update_variable_name_map() {
        /**
         * NOTE: This method must be called after update_variable_reference().
         */
        this->variable_name_map.clear();
        this->variable_name_map.reserve(this->number_of_variables() * 5);

        for (auto &&proxy : m_model_ptr->variable_proxies()) {
            for (auto &&variable : proxy.flat_indexed_variables()) {
                this->variable_name_map[variable.name()] = &variable;
            }
        }
    }

    /*************************************************************************/
    inline void update_contraint_name_map() {
        /**
         * NOTE: This method must be called after update_constraint_reference().
         */
        this->constraint_name_map.clear();
        this->constraint_name_map.reserve(this->number_of_constraints() * 5);

        for (auto &&proxy : m_model_ptr->constraint_proxies()) {
            for (auto &&constraint : proxy.flat_indexed_constraints()) {
                this->constraint_name_map[constraint.name()] = &constraint;
            }
        }
    }

    /*************************************************************************/
    inline int number_of_variables(void) const noexcept {
        return this->variable.variable_ptrs.size();
    }

    /*************************************************************************/
    inline int number_of_fixed_variables(void) const noexcept {
        return this->variable.fixed_variable_ptrs.size();
    }

    /*************************************************************************/
    inline int number_of_mutable_variables(void) const noexcept {
        return this->variable.mutable_variable_ptrs.size();
    }

    /*************************************************************************/
    inline int number_of_binary_variables(void) const noexcept {
        return this->variable_type.binary_variable_ptrs.size();
    }

    /*************************************************************************/
    inline int number_of_integer_variables(void) const noexcept {
        return this->variable_type.integer_variable_ptrs.size();
    }

    /*************************************************************************/
    inline int number_of_selection_variables(void) const noexcept {
        return this->variable_type.selection_variable_ptrs.size();
    }

    /*************************************************************************/
    inline int number_of_dependent_binary_variables(void) const noexcept {
        return this->variable_type.dependent_binary_variable_ptrs.size();
    }

    /*************************************************************************/
    inline int number_of_dependent_integer_variables(void) const noexcept {
        return this->variable_type.dependent_integer_variable_ptrs.size();
    }

    /*************************************************************************/
    inline int number_of_constraints(void) const noexcept {
        return this->constraint.constraint_ptrs.size();
    }

    /*************************************************************************/
    inline int number_of_enabled_constraints(void) const noexcept {
        return this->constraint.enabled_constraint_ptrs.size();
    }

    /*************************************************************************/
    inline int number_of_disabled_constraints(void) const noexcept {
        return this->constraint.disabled_constraint_ptrs.size();
    }
};
}  // namespace printemps::model_handler
#endif
/*****************************************************************************/
// END
/*****************************************************************************/
/*****************************************************************************/