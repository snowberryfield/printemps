/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_MODEL_COMPONENT_CONSTRAINT_TYPE_CLASSIFIER_H__
#define PRINTEMPS_MODEL_COMPONENT_CONSTRAINT_TYPE_CLASSIFIER_H__

namespace printemps::model_component {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class Variable;

/*****************************************************************************/
template <class T_Variable, class T_Expression>
class Constraint;

/*****************************************************************************/
template <class T_Variable, class T_Expression>
class ConstraintTypeClassifier {
   private:
    Constraint<T_Variable, T_Expression>          *m_constraint_ptr;
    ExpressionStructure<T_Variable, T_Expression> *m_structure_ptr;
    ConstraintSense                                m_sense;

    ConstraintType                      m_type;
    Variable<T_Variable, T_Expression> *m_key_variable_ptr;

    using ClassifierMethod = bool (ConstraintTypeClassifier::*)(void);

    /**************************************************************************/
    inline std::pair<ConstraintType, Variable<T_Variable, T_Expression> *>
    return_type(void) const noexcept {
        return {m_type, m_key_variable_ptr};
    }

    /**************************************************************************/
    inline Variable<T_Variable, T_Expression> *determine_key_variable_ptr(
        Variable<T_Variable, T_Expression> *a_FIRST,
        Variable<T_Variable, T_Expression> *a_SECOND) const noexcept {
        if (a_FIRST->name() < a_SECOND->name()) {
            return a_FIRST;
        } else {
            return a_SECOND;
        }
    }

    /**************************************************************************/
    inline bool check_empty(void) {
        if (m_structure_ptr->number_of_variables > 0) {
            return false;
        }

        m_type             = ConstraintType::Empty;
        m_key_variable_ptr = nullptr;

        return true;
    }

    /**************************************************************************/
    inline bool check_singleton(void) {
        if (m_structure_ptr->number_of_variables != 1) {
            return false;
        }

        m_type             = ConstraintType::Singleton;
        m_key_variable_ptr = nullptr;

        return true;
    }

    /**************************************************************************/
    inline bool check_exclusive_or(void) {
        if (m_structure_ptr->number_of_variables != 2 ||
            m_sense != ConstraintSense::Equal) {
            return false;
        }

        auto &variable_ptrs = m_structure_ptr->variable_ptrs;
        auto &coefficients  = m_structure_ptr->coefficients;

        if (!(variable_ptrs[0]->type() == VariableType::Binary &&
              variable_ptrs[1]->type() == VariableType::Binary)) {
            return false;
        }

        if ((coefficients[0] == 1 && coefficients[1] == 1 &&
             m_structure_ptr->constant_value == -1) ||
            (coefficients[0] == -1 && coefficients[1] == -1 &&
             m_structure_ptr->constant_value == 1)) {
            m_type             = ConstraintType::ExclusiveOR;
            m_key_variable_ptr = this->determine_key_variable_ptr(
                variable_ptrs[0], variable_ptrs[1]);
            return true;
        }

        return false;
    }

    /**************************************************************************/
    inline bool check_exclusive_nor(void) {
        if (m_structure_ptr->number_of_variables != 2 ||
            m_sense != ConstraintSense::Equal) {
            return false;
        }

        auto &variable_ptrs = m_structure_ptr->variable_ptrs;
        auto &coefficients  = m_structure_ptr->coefficients;

        if (!(variable_ptrs[0]->type() == VariableType::Binary &&
              variable_ptrs[1]->type() == VariableType::Binary)) {
            return false;
        }

        if ((coefficients[0] == 1 && coefficients[1] == -1 &&
             m_structure_ptr->constant_value == 0) ||
            (coefficients[0] == -1 && coefficients[1] == 1 &&
             m_structure_ptr->constant_value == 0)) {
            m_type             = ConstraintType::ExclusiveNOR;
            m_key_variable_ptr = this->determine_key_variable_ptr(
                variable_ptrs[0], variable_ptrs[1]);
            return true;
        }

        return false;
    }

    /**************************************************************************/
    inline bool check_inverted_integers(void) {
        if (m_structure_ptr->number_of_variables != 2 ||
            m_sense != ConstraintSense::Equal) {
            return false;
        }

        auto &variable_ptrs = m_structure_ptr->variable_ptrs;
        auto &coefficients  = m_structure_ptr->coefficients;

        if (!(variable_ptrs[0]->type() == VariableType::Integer &&
              variable_ptrs[1]->type() == VariableType::Integer)) {
            return false;
        }

        if (((coefficients[0] == 1 && coefficients[1] == 1) ||
             (coefficients[0] == -1 && coefficients[1] == -1)) &&
            m_structure_ptr->constant_value == 0) {
            m_type             = ConstraintType::InvertedIntegers;
            m_key_variable_ptr = this->determine_key_variable_ptr(
                variable_ptrs[0], variable_ptrs[1]);
            return true;
        }

        return false;
    }

    /**************************************************************************/
    inline bool check_balanced_integers(void) {
        if (m_structure_ptr->number_of_variables != 2 ||
            m_sense != ConstraintSense::Equal) {
            return false;
        }

        auto &variable_ptrs = m_structure_ptr->variable_ptrs;
        auto &coefficients  = m_structure_ptr->coefficients;

        if (!(variable_ptrs[0]->type() == VariableType::Integer &&
              variable_ptrs[1]->type() == VariableType::Integer)) {
            return false;
        }

        if (((coefficients[0] == 1 && coefficients[1] == -1) ||
             (coefficients[0] == -1 && coefficients[1] == 1)) &&
            m_structure_ptr->constant_value == 0) {
            m_type             = ConstraintType::BalancedIntegers;
            m_key_variable_ptr = this->determine_key_variable_ptr(
                variable_ptrs[0], variable_ptrs[1]);
            return true;
        }
        return false;
    }

    /**************************************************************************/
    inline bool check_constant_sum_integers(void) {
        if (m_structure_ptr->number_of_variables != 2 ||
            m_sense != ConstraintSense::Equal) {
            return false;
        }

        auto &variable_ptrs = m_structure_ptr->variable_ptrs;
        auto &coefficients  = m_structure_ptr->coefficients;

        if (!(variable_ptrs[0]->type() == VariableType::Integer &&
              variable_ptrs[1]->type() == VariableType::Integer)) {
            return false;
        }

        if (((coefficients[0] == 1 && coefficients[1] == 1) ||
             (coefficients[0] == -1 && coefficients[1] == -1)) &&
            m_structure_ptr->constant_value != 0) {
            m_type             = ConstraintType::ConstantSumIntegers;
            m_key_variable_ptr = this->determine_key_variable_ptr(
                variable_ptrs[0], variable_ptrs[1]);
            return true;
        }

        return false;
    }

    /**************************************************************************/
    inline bool check_constant_difference_integers(void) {
        if (m_structure_ptr->number_of_variables != 2 ||
            m_sense != ConstraintSense::Equal)
            return false;

        auto &variable_ptrs = m_structure_ptr->variable_ptrs;
        auto &coefficients  = m_structure_ptr->coefficients;

        if (!(variable_ptrs[0]->type() == VariableType::Integer &&
              variable_ptrs[1]->type() == VariableType::Integer)) {
            return false;
        }

        if (((coefficients[0] == 1 && coefficients[1] == -1) ||
             (coefficients[0] == -1 && coefficients[1] == 1)) &&
            m_structure_ptr->constant_value != 0) {
            m_type             = ConstraintType::ConstantDifferenceIntegers;
            m_key_variable_ptr = this->determine_key_variable_ptr(
                variable_ptrs[0], variable_ptrs[1]);
            return true;
        }

        return false;
    }

    /**************************************************************************/
    inline bool check_constant_ratio_integers(void) {
        if (m_structure_ptr->number_of_variables != 2 ||
            m_sense != ConstraintSense::Equal) {
            return false;
        }

        auto &variable_ptrs = m_structure_ptr->variable_ptrs;
        auto &coefficients  = m_structure_ptr->coefficients;

        if (!(variable_ptrs[0]->type() == VariableType::Integer &&
              variable_ptrs[1]->type() == VariableType::Integer)) {
            return false;
        }

        if (m_structure_ptr->constant_value == 0) {
            if (std::abs(coefficients[0]) == 1 &&
                std::abs(coefficients[1]) != 1) {
                m_type             = ConstraintType::ConstantRatioIntegers;
                m_key_variable_ptr = variable_ptrs[0];
                return true;
            }
            if (std::abs(coefficients[0]) != 1 &&
                std::abs(coefficients[1]) == 1) {
                m_type             = ConstraintType::ConstantRatioIntegers;
                m_key_variable_ptr = variable_ptrs[1];
                return true;
            }
        }

        return false;
    }

    /**************************************************************************/
    inline bool check_intermediate_two_term(void) {
        if (m_structure_ptr->number_of_variables != 2 ||
            m_sense != ConstraintSense::Equal) {
            return false;
        }

        if (!m_structure_ptr->is_integer)
            return false;

        auto &variable_ptrs = m_structure_ptr->variable_ptrs;
        auto &coefficients  = m_structure_ptr->coefficients;

        if (!(variable_ptrs[0]->type() == VariableType::Integer &&
              variable_ptrs[1]->type() == VariableType::Integer)) {
            return false;
        }

        if (std::abs(coefficients[0]) == 1 && std::abs(coefficients[1]) != 1) {
            m_type             = ConstraintType::Intermediate;
            m_key_variable_ptr = variable_ptrs[0];
            return true;
        }

        if (std::abs(coefficients[0]) != 1 && std::abs(coefficients[1]) == 1) {
            m_type             = ConstraintType::Intermediate;
            m_key_variable_ptr = variable_ptrs[1];
            return true;
        }

        return false;
    }

    /**************************************************************************/
    inline bool check_aggregation(void) {
        if (m_structure_ptr->number_of_variables != 2 ||
            m_sense != ConstraintSense::Equal) {
            return false;
        }

        m_type             = ConstraintType::Aggregation;
        m_key_variable_ptr = nullptr;

        return true;
    }

    /**************************************************************************/
    inline bool check_precedence(void) {
        if (m_structure_ptr->number_of_variables != 2 ||
            m_sense == ConstraintSense::Equal) {
            return false;
        }

        auto &variable_ptrs = m_structure_ptr->variable_ptrs;
        auto &coefficients  = m_structure_ptr->coefficients;

        if ((variable_ptrs[0]->type() == variable_ptrs[1]->type()) &&
            (coefficients[0] == -coefficients[1])) {
            m_type             = ConstraintType::Precedence;
            m_key_variable_ptr = nullptr;
            return true;
        }

        return false;
    }

    /**************************************************************************/
    inline bool check_variable_bound(void) {
        if (m_structure_ptr->number_of_variables != 2 ||
            m_sense == ConstraintSense::Equal) {
            return false;
        }

        m_type = ConstraintType::VariableBound;
        return true;
    }

    /**************************************************************************/
    inline bool check_trinomial_exclusive_nor(void) {
        if (m_structure_ptr->number_of_variables != 3 ||
            m_sense != ConstraintSense::Equal) {
            return false;
        }

        if (m_structure_ptr->constant_value != 0) {
            return false;
        }

        if (!m_structure_ptr->has_only_binary_or_selection_variable) {
            return false;
        }

        if (static_cast<int>(
                m_structure_ptr->plus_one_coefficient_variable_ptrs.size()) ==
                m_structure_ptr->number_of_variables - 1 &&
            m_structure_ptr->minus_n_minus_one_coefficient_integer_variable_ptrs
                    .size() == 1) {
            m_type = ConstraintType::TrinomialExclusiveNOR;
            m_key_variable_ptr =
                m_structure_ptr
                    ->minus_n_minus_one_coefficient_integer_variable_ptrs[0];
            return true;
        }

        if (static_cast<int>(
                m_structure_ptr->minus_one_coefficient_variable_ptrs.size()) ==
                m_structure_ptr->number_of_variables - 1 &&
            m_structure_ptr->plus_n_minus_one_coefficient_integer_variable_ptrs
                    .size() == 1) {
            m_type = ConstraintType::TrinomialExclusiveNOR;
            m_key_variable_ptr =
                m_structure_ptr
                    ->plus_n_minus_one_coefficient_integer_variable_ptrs[0];
            return true;
        }

        return false;
    }

    /**************************************************************************/
    inline bool check_all_or_nothing(void) {
        if (m_sense != ConstraintSense::Equal) {
            return false;
        }

        if (m_structure_ptr->constant_value != 0) {
            return false;
        }

        if (!m_structure_ptr->has_only_binary_or_selection_variable) {
            return false;
        }

        if (static_cast<int>(
                m_structure_ptr->plus_one_coefficient_variable_ptrs.size()) ==
                m_structure_ptr->number_of_variables - 1 &&
            m_structure_ptr->minus_n_minus_one_coefficient_integer_variable_ptrs
                    .size() == 1) {
            m_type = ConstraintType::AllOrNothing;
            m_key_variable_ptr =
                m_structure_ptr
                    ->minus_n_minus_one_coefficient_integer_variable_ptrs[0];
            return true;
        }

        if (static_cast<int>(
                m_structure_ptr->minus_one_coefficient_variable_ptrs.size()) ==
                m_structure_ptr->number_of_variables - 1 &&
            m_structure_ptr->plus_n_minus_one_coefficient_integer_variable_ptrs
                    .size() == 1) {
            m_type = ConstraintType::AllOrNothing;
            m_key_variable_ptr =
                m_structure_ptr
                    ->plus_n_minus_one_coefficient_integer_variable_ptrs[0];
            return true;
        }

        return false;
    }

    /**************************************************************************/
    inline bool check_set_partitioning(void) {
        if (!m_structure_ptr->has_only_binary_coefficient ||
            !m_structure_ptr->has_only_binary_or_selection_variable) {
            return false;
        }

        if (m_structure_ptr->constant_value == -1 &&
            m_sense == ConstraintSense::Equal) {
            m_type = ConstraintType::SetPartitioning;
            return true;
        }

        return false;
    }

    /**************************************************************************/
    inline bool check_set_packing(void) {
        if (!m_structure_ptr->has_only_binary_coefficient ||
            !m_structure_ptr->has_only_binary_or_selection_variable) {
            return false;
        }

        if (m_structure_ptr->constant_value == -1 &&
            m_sense == ConstraintSense::Less) {
            m_type = ConstraintType::SetPacking;
            return true;
        }

        return false;
    }

    /**************************************************************************/
    inline bool check_set_covering(void) {
        if (!m_structure_ptr->has_only_binary_coefficient ||
            !m_structure_ptr->has_only_binary_or_selection_variable) {
            return false;
        }

        if (m_structure_ptr->constant_value == -1 &&
            m_sense == ConstraintSense::Greater) {
            m_type = ConstraintType::SetCovering;
            return true;
        }

        return false;
    }

    /**************************************************************************/
    inline bool check_cardinality(void) {
        if (!m_structure_ptr->has_only_binary_coefficient ||
            !m_structure_ptr->has_only_binary_or_selection_variable) {
            return false;
        }

        if (m_structure_ptr->constant_value <= -2 &&
            m_sense == ConstraintSense::Equal) {
            m_type = ConstraintType::Cardinality;
            return true;
        }

        return false;
    }

    /**************************************************************************/
    inline bool check_invariant_knapsack(void) {
        if (!m_structure_ptr->has_only_binary_coefficient ||
            !m_structure_ptr->has_only_binary_or_selection_variable) {
            return false;
        }

        if (m_structure_ptr->constant_value <= -2 &&
            m_sense == ConstraintSense::Less) {
            m_type = ConstraintType::InvariantKnapsack;
            return true;
        }

        return false;
    }

    /**************************************************************************/
    inline bool check_multiple_covering(void) {
        if (!m_structure_ptr->has_only_binary_coefficient ||
            !m_structure_ptr->has_only_binary_or_selection_variable) {
            return false;
        }

        if (m_structure_ptr->constant_value <= -2 &&
            m_sense == ConstraintSense::Greater) {
            m_type = ConstraintType::MultipleCovering;
            return true;
        }

        return false;
    }

    /**************************************************************************/
    inline bool check_soft_selection(void) {
        if (m_sense != ConstraintSense::Equal ||
            m_structure_ptr->constant_value != 0) {
            return false;
        }

        if (!m_structure_ptr->has_only_plus_or_minus_one_coefficient) {
            return false;
        }

        if (!m_structure_ptr->has_only_binary_or_selection_variable) {
            return false;
        }

        const auto PLUS_ONE_SIZE =
            m_structure_ptr->plus_one_coefficient_variable_ptrs.size();
        const auto MINUS_ONE_SIZE =
            m_structure_ptr->minus_one_coefficient_variable_ptrs.size();

        if (PLUS_ONE_SIZE == 1 && MINUS_ONE_SIZE > 0) {
            m_type = ConstraintType::SoftSelection;
            m_key_variable_ptr =
                m_structure_ptr->plus_one_coefficient_variable_ptrs[0];
            return true;
        }
        if (PLUS_ONE_SIZE > 0 && MINUS_ONE_SIZE == 1) {
            m_type = ConstraintType::SoftSelection;
            m_key_variable_ptr =
                m_structure_ptr->minus_one_coefficient_variable_ptrs[0];
            return true;
        }

        return false;
    }

    /**************************************************************************/
    inline bool check_binary_flow(void) {
        if (m_sense != ConstraintSense::Equal) {
            return false;
        }

        if (!m_structure_ptr->has_only_plus_or_minus_one_coefficient) {
            return false;
        }

        if (!m_structure_ptr->has_only_binary_or_selection_variable) {
            return false;
        }

        m_type             = ConstraintType::BinaryFlow;
        m_key_variable_ptr = nullptr;

        return true;
    }

    /**************************************************************************/
    inline bool check_integer_flow(void) {
        if (m_sense != ConstraintSense::Equal) {
            return false;
        }

        if (!m_structure_ptr->has_only_plus_or_minus_one_coefficient) {
            return false;
        }

        if (!m_structure_ptr->has_only_integer_variables) {
            return false;
        }

        const auto PLUS_ONE_SIZE =
            m_structure_ptr->plus_one_coefficient_variable_ptrs.size();
        const auto MINUS_ONE_SIZE =
            m_structure_ptr->minus_one_coefficient_variable_ptrs.size();

        if (PLUS_ONE_SIZE > 1 && MINUS_ONE_SIZE > 1) {
            m_type             = ConstraintType::IntegerFlow;
            m_key_variable_ptr = nullptr;
            return true;
        }

        return false;
    }

    /**************************************************************************/
    inline bool check_min_max(void) {
        if (!m_structure_ptr->is_integer || m_sense == ConstraintSense::Equal) {
            return false;
        }

        const auto MINUS_ONE_SIZE =
            m_structure_ptr->minus_one_coefficient_integer_variable_ptrs.size();
        const auto PLUS_ONE_SIZE =
            m_structure_ptr->plus_one_coefficient_integer_variable_ptrs.size();

        if (m_sense == ConstraintSense::Less && MINUS_ONE_SIZE == 1 &&
            PLUS_ONE_SIZE == 0) {
            m_type = ConstraintType::MinMax;
            m_key_variable_ptr =
                m_structure_ptr->minus_one_coefficient_integer_variable_ptrs[0];
            return true;
        }

        if (m_sense == ConstraintSense::Greater && PLUS_ONE_SIZE == 1 &&
            MINUS_ONE_SIZE == 0) {
            m_type = ConstraintType::MinMax;
            m_key_variable_ptr =
                m_structure_ptr->plus_one_coefficient_integer_variable_ptrs[0];
            return true;
        }

        return false;
    }

    /**************************************************************************/
    inline bool check_max_min(void) {
        if (!m_structure_ptr->is_integer || m_sense == ConstraintSense::Equal) {
            return false;
        }

        const auto MINUS_ONE_SIZE =
            m_structure_ptr->minus_one_coefficient_integer_variable_ptrs.size();
        const auto PLUS_ONE_SIZE =
            m_structure_ptr->plus_one_coefficient_integer_variable_ptrs.size();

        if (m_sense == ConstraintSense::Greater && MINUS_ONE_SIZE == 1 &&
            PLUS_ONE_SIZE == 0) {
            m_type = ConstraintType::MaxMin;
            m_key_variable_ptr =
                m_structure_ptr->minus_one_coefficient_integer_variable_ptrs[0];
            return true;
        }

        if (m_sense == ConstraintSense::Less && PLUS_ONE_SIZE == 1 &&
            MINUS_ONE_SIZE == 0) {
            m_type = ConstraintType::MaxMin;
            m_key_variable_ptr =
                m_structure_ptr->plus_one_coefficient_integer_variable_ptrs[0];
            return true;
        }

        return false;
    }

    /**************************************************************************/
    inline bool check_intermediate_multi_term(void) {
        if (!m_structure_ptr->is_integer || m_sense != ConstraintSense::Equal) {
            return false;
        }

        const auto MINUS_ONE_SIZE =
            m_structure_ptr->minus_one_coefficient_integer_variable_ptrs.size();
        const auto PLUS_ONE_SIZE =
            m_structure_ptr->plus_one_coefficient_integer_variable_ptrs.size();

        if (MINUS_ONE_SIZE == 1 && PLUS_ONE_SIZE != 1) {
            m_type = ConstraintType::Intermediate;
            m_key_variable_ptr =
                m_structure_ptr->minus_one_coefficient_integer_variable_ptrs[0];
            return true;
        }

        if (PLUS_ONE_SIZE == 1 && MINUS_ONE_SIZE != 1) {
            m_type = ConstraintType::Intermediate;
            m_key_variable_ptr =
                m_structure_ptr->plus_one_coefficient_integer_variable_ptrs[0];
            return true;
        }

        return false;
    }

    /**************************************************************************/
    inline bool check_equation_knapsack(void) {
        if (!m_structure_ptr->has_only_binary_or_selection_variable ||
            m_sense != ConstraintSense::Equal) {
            return false;
        }

        const auto POSITIVE_SIZE =
            m_structure_ptr->positive_coefficient_variable_ptrs.size();
        const auto NEGATIVE_SIZE =
            m_structure_ptr->negative_coefficient_variable_ptrs.size();

        if (POSITIVE_SIZE == 0 || NEGATIVE_SIZE == 0) {
            m_type             = ConstraintType::EquationKnapsack;
            m_key_variable_ptr = nullptr;
            return true;
        }

        return false;
    }

    /**************************************************************************/
    inline bool check_bin_packing(void) {
        if (!m_structure_ptr->has_only_binary_or_selection_variable) {
            return false;
        }

        if (!m_structure_ptr->has_bin_packing_variable) {
            return false;
        }

        const auto POSITIVE_SIZE =
            m_structure_ptr->positive_coefficient_variable_ptrs.size();
        const auto NEGATIVE_SIZE =
            m_structure_ptr->negative_coefficient_variable_ptrs.size();

        if ((NEGATIVE_SIZE == 0 && m_sense == ConstraintSense::Less) ||
            (POSITIVE_SIZE == 0 && m_sense == ConstraintSense::Greater)) {
            m_type             = ConstraintType::BinPacking;
            m_key_variable_ptr = nullptr;
            return true;
        }

        return false;
    }

    /**************************************************************************/
    inline bool check_knapsack(void) {
        if (!m_structure_ptr->has_only_binary_or_selection_variable) {
            return false;
        }

        const auto POSITIVE_SIZE =
            m_structure_ptr->positive_coefficient_variable_ptrs.size();
        const auto NEGATIVE_SIZE =
            m_structure_ptr->negative_coefficient_variable_ptrs.size();

        if ((NEGATIVE_SIZE == 0 && m_sense == ConstraintSense::Less) ||
            (POSITIVE_SIZE == 0 && m_sense == ConstraintSense::Greater)) {
            m_type             = ConstraintType::Knapsack;
            m_key_variable_ptr = nullptr;
            return true;
        }
        return false;
    }

    /**************************************************************************/
    inline bool check_integer_knapsack(void) {
        if (m_structure_ptr->has_only_binary_or_selection_variable) {
            return false;
        }

        const auto POSITIVE_SIZE =
            m_structure_ptr->positive_coefficient_variable_ptrs.size();
        const auto NEGATIVE_SIZE =
            m_structure_ptr->negative_coefficient_variable_ptrs.size();

        if ((NEGATIVE_SIZE == 0 && m_sense == ConstraintSense::Less) ||
            (POSITIVE_SIZE == 0 && m_sense == ConstraintSense::Greater)) {
            m_type             = ConstraintType::IntegerKnapsack;
            m_key_variable_ptr = nullptr;
            return true;
        }

        return false;
    }

    /**************************************************************************/
    inline bool check_gf2(void) {
        if (m_sense != ConstraintSense::Equal) {
            return false;
        }

        if (m_structure_ptr->constant_value != 0 &&
            std::abs(m_structure_ptr->constant_value) != 1) {
            return false;
        }

        Variable<T_Variable, T_Expression> *key_variable_ptr         = nullptr;
        T_Expression                        key_variable_coefficient = 0;

        int number_of_integer_variables_coefficient_two = 0;

        auto &variable_ptrs = m_structure_ptr->variable_ptrs;
        auto &coefficients  = m_structure_ptr->coefficients;

        for (auto i = 0; i < m_structure_ptr->number_of_variables; i++) {
            auto variable_ptr = variable_ptrs[i];
            auto coefficient  = coefficients[i];

            if ((variable_ptr->type() == VariableType::Integer ||
                 variable_ptr->type() == VariableType::Binary) &&
                std::abs(coefficient) == 2) {
                key_variable_ptr         = variable_ptr;
                key_variable_coefficient = coefficient;
                number_of_integer_variables_coefficient_two++;
            } else if (variable_ptr->type() != VariableType::Binary ||
                       std::abs(coefficient) != 1) {
                return false;
            }
        }

        if (number_of_integer_variables_coefficient_two != 1) {
            return false;
        }

        auto rest_part_expression = m_constraint_ptr->m_expression.copy();
        rest_part_expression.erase(key_variable_ptr);
        T_Expression rest_part_lower_bound = rest_part_expression.lower_bound();
        T_Expression rest_part_upper_bound = rest_part_expression.upper_bound();

        if (key_variable_coefficient > 0) {
            std::swap(rest_part_lower_bound, rest_part_upper_bound);
            rest_part_lower_bound *= -1.0;
            rest_part_upper_bound *= -1.0;
        }

        if (key_variable_ptr->lower_bound() != constant::INT_HALF_MIN &&
            key_variable_ptr->lower_bound() >
                static_cast<int>(std::ceil(rest_part_lower_bound * 0.5))) {
            return false;
        }

        if (key_variable_ptr->upper_bound() != constant::INT_HALF_MAX &&
            key_variable_ptr->upper_bound() <
                static_cast<int>(std::floor(rest_part_upper_bound * 0.5))) {
            return false;
        }

        m_type             = ConstraintType::GF2;
        m_key_variable_ptr = key_variable_ptr;

        return true;
    }

    /**************************************************************************/
    inline bool check_general_linear(void) {
        m_type             = ConstraintType::GeneralLinear;
        m_key_variable_ptr = nullptr;

        return true;
    }

   public:
    /**************************************************************************/
    ConstraintTypeClassifier(void) {
        this->initialize();
    }

    /**************************************************************************/
    ConstraintTypeClassifier(
        Constraint<T_Variable, T_Expression> *a_constraint_ptr) {
        this->initialize();
        this->setup(a_constraint_ptr);
    }
    /**************************************************************************/
    inline void initialize(void) noexcept {
        m_constraint_ptr = nullptr;
        m_structure_ptr  = nullptr;
        m_sense          = ConstraintSense::Less;

        m_key_variable_ptr = nullptr;
        m_type             = ConstraintType::Unknown;
    }

    /**************************************************************************/
    inline void setup(Constraint<T_Variable, T_Expression> *a_constraint_ptr) {
        m_constraint_ptr = a_constraint_ptr;
        m_structure_ptr  = &(m_constraint_ptr->structure());
        m_sense          = m_constraint_ptr->sense();
    }

    /**************************************************************************/
    inline std::pair<ConstraintType, Variable<T_Variable, T_Expression> *>
    classify_type(void) {
        m_type             = ConstraintType::Unknown;
        m_key_variable_ptr = nullptr;

        if (!m_constraint_ptr) {
            return this->return_type();
        }

        std::vector<ClassifierMethod> classification_order = {
            &ConstraintTypeClassifier::check_empty,
            &ConstraintTypeClassifier::check_singleton,
            &ConstraintTypeClassifier::check_exclusive_or,
            &ConstraintTypeClassifier::check_exclusive_nor,
            &ConstraintTypeClassifier::check_inverted_integers,
            &ConstraintTypeClassifier::check_balanced_integers,
            &ConstraintTypeClassifier::check_constant_sum_integers,
            &ConstraintTypeClassifier::check_constant_difference_integers,
            &ConstraintTypeClassifier::check_constant_ratio_integers,
            &ConstraintTypeClassifier::check_intermediate_two_term,
            &ConstraintTypeClassifier::check_aggregation,
            &ConstraintTypeClassifier::check_precedence,
            &ConstraintTypeClassifier::check_variable_bound,
            &ConstraintTypeClassifier::check_trinomial_exclusive_nor,
            &ConstraintTypeClassifier::check_all_or_nothing,
            &ConstraintTypeClassifier::check_set_partitioning,
            &ConstraintTypeClassifier::check_set_packing,
            &ConstraintTypeClassifier::check_set_covering,
            &ConstraintTypeClassifier::check_cardinality,
            &ConstraintTypeClassifier::check_invariant_knapsack,
            &ConstraintTypeClassifier::check_multiple_covering,
            &ConstraintTypeClassifier::check_soft_selection,
            &ConstraintTypeClassifier::check_binary_flow,
            &ConstraintTypeClassifier::check_integer_flow,
            &ConstraintTypeClassifier::check_min_max,
            &ConstraintTypeClassifier::check_max_min,
            &ConstraintTypeClassifier::check_intermediate_multi_term,
            &ConstraintTypeClassifier::check_equation_knapsack,
            &ConstraintTypeClassifier::check_bin_packing,
            &ConstraintTypeClassifier::check_knapsack,
            &ConstraintTypeClassifier::check_integer_knapsack,
            &ConstraintTypeClassifier::check_gf2,
            &ConstraintTypeClassifier::check_general_linear};

        for (const auto &method_ptr : classification_order) {
            if ((this->*method_ptr)()) {
                return this->return_type();
            }
        }

        return this->return_type();
    }
};
}  // namespace printemps::model_component
#endif
/*****************************************************************************/
// END
/*****************************************************************************/