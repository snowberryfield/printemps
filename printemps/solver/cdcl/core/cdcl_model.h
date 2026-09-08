/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_CDCL_CORE_CDCL_MODEL_H__
#define PRINTEMPS_SOLVER_CDCL_CORE_CDCL_MODEL_H__

#include "cdcl_variable.h"
#include "cardinality_constraint.h"
#include "pseudo_boolean_constraint.h"

namespace printemps::solver::cdcl::core {

/*****************************************************************************/
class CDCLModel {
   private:
    matrix_model::MatrixModel* m_instance_ptr;

    std::vector<CDCLVariable> m_variables;

    std::vector<PseudoBooleanConstraint> m_pseudo_boolean_constraints;
    std::vector<CardinalityConstraint>   m_cardinality_constraints;
    std::vector<CardinalityConstraint>   m_learned_constraints;

    std::vector<std::vector<std::pair<int, int>>>
        m_variable_to_pseudo_boolean_positive_constraints;
    std::vector<std::vector<std::pair<int, int>>>
        m_variable_to_pseudo_boolean_negative_constraints;

    std::vector<int>     m_pseudo_boolean_propagation_queue;
    std::vector<uint8_t> m_in_pseudo_boolean_propagation_queue;

    std::vector<int>     m_cardinality_propagation_queue;
    std::vector<uint8_t> m_in_cardinality_propagation_queue;

    std::vector<int>     m_learned_constraint_propagation_queue;
    std::vector<uint8_t> m_in_learned_constraint_propagation_queue;

    std::vector<double> m_initial_activities;

    /*************************************************************************/
    inline void build_constraint(const std::vector<int>&    a_VARIABLE_INDICES,
                                 const std::vector<double>& a_COEFFICIENTS,
                                 const double               a_RHS) {
        const int NUMBER_OF_NON_ZERO_ELEMENTS =
            static_cast<int>(a_VARIABLE_INDICES.size());

        if (NUMBER_OF_NON_ZERO_ELEMENTS == 0) {
            return;
        }

        // 1. Automatically calculate required decimal places
        // (max_decimal_places)
        int max_decimal_places = 0;
        for (int j = 0; j < NUMBER_OF_NON_ZERO_ELEMENTS; j++) {
            max_decimal_places = std::max(
                max_decimal_places, utility::decimal_places(a_COEFFICIENTS[j]));
        }
        max_decimal_places =
            std::max(max_decimal_places, utility::decimal_places(a_RHS));

        std::vector<int64_t> int_coefficients(NUMBER_OF_NON_ZERO_ELEMENTS);
        int64_t              int_rhs = 0;

        if (max_decimal_places > 0) {
            const double         SCALE = std::pow(10.0, max_decimal_places);
            std::vector<int64_t> scaled_values;
            scaled_values.reserve(NUMBER_OF_NON_ZERO_ELEMENTS + 1);

            for (int j = 0; j < NUMBER_OF_NON_ZERO_ELEMENTS; j++) {
                const int64_t COEFFICIENT =
                    std::llround(a_COEFFICIENTS[j] * SCALE);
                int_coefficients[j] = COEFFICIENT;
                if (COEFFICIENT != 0) {
                    scaled_values.push_back(std::abs(COEFFICIENT));
                }
            }

            const int64_t RHS = std::llround(a_RHS * SCALE);
            int_rhs           = RHS;
            if (RHS != 0) {
                scaled_values.push_back(std::abs(RHS));
            }

            if (!scaled_values.empty()) {
                int64_t   gcd = scaled_values.front();
                const int SCALED_VALUES_SIZE =
                    static_cast<int>(scaled_values.size());
                for (int i = 1; i < SCALED_VALUES_SIZE; i++) {
                    gcd = std::gcd(gcd, scaled_values[i]);
                }
                if (gcd > 1) {
                    for (int j = 0; j < NUMBER_OF_NON_ZERO_ELEMENTS; j++) {
                        int_coefficients[j] /= gcd;
                    }
                    int_rhs /= gcd;
                }
            }
        } else {
            for (int j = 0; j < NUMBER_OF_NON_ZERO_ELEMENTS; j++) {
                int_coefficients[j] =
                    static_cast<int64_t>(std::round(a_COEFFICIENTS[j]));
            }
            int_rhs = static_cast<int64_t>(std::round(a_RHS));
        }

        // 2. Check if the constraint is Cardinality (all non-zero coefficients
        // are +/- 1)
        bool is_cardinality = true;
        for (int j = 0; j < NUMBER_OF_NON_ZERO_ELEMENTS; j++) {
            if (std::abs(int_coefficients[j]) != 1) {
                is_cardinality = false;
                break;
            }
        }

        if (is_cardinality) {
            std::vector<int> positive_variable_indices;
            std::vector<int> negative_variable_indices;
            for (int j = 0; j < NUMBER_OF_NON_ZERO_ELEMENTS; j++) {
                const int     VARIABLE_INDEX = a_VARIABLE_INDICES[j];
                const int64_t COEFFICIENT    = int_coefficients[j];

                if (COEFFICIENT > 0) {
                    positive_variable_indices.push_back(VARIABLE_INDEX);
                } else if (COEFFICIENT < 0) {
                    negative_variable_indices.push_back(VARIABLE_INDEX);
                }
            }

            int64_t rhs = int_rhs;
            rhs += static_cast<int64_t>(negative_variable_indices.size());

            const int INDEX =
                static_cast<int>(m_cardinality_constraints.size());
            m_cardinality_constraints.emplace_back(INDEX, rhs,
                                                   positive_variable_indices,
                                                   negative_variable_indices);
        } else {
            std::vector<std::pair<int, int64_t>> positive_terms;
            std::vector<std::pair<int, int64_t>> negative_terms;

            int64_t total_positive_coefficient = 0;
            int64_t rhs                        = int_rhs;

            for (int j = 0; j < NUMBER_OF_NON_ZERO_ELEMENTS; j++) {
                const int     VARIABLE_INDEX = a_VARIABLE_INDICES[j];
                const int64_t COEFFICIENT    = int_coefficients[j];

                if (COEFFICIENT == 0) {
                    continue;
                }

                if (COEFFICIENT > 0) {
                    positive_terms.push_back({VARIABLE_INDEX, COEFFICIENT});
                    total_positive_coefficient += COEFFICIENT;

                } else {
                    negative_terms.push_back({VARIABLE_INDEX, -COEFFICIENT});
                    total_positive_coefficient -= COEFFICIENT;
                    rhs -= COEFFICIENT;
                }
            }

            const int INDEX =
                static_cast<int>(m_pseudo_boolean_constraints.size());
            m_pseudo_boolean_constraints.emplace_back(
                INDEX, rhs, total_positive_coefficient, positive_terms,
                negative_terms);
        }
    }

   public:
    /*************************************************************************/
    CDCLModel(void) {
        this->initialize();
    }

    /*************************************************************************/
    inline void initialize(void) {
        m_instance_ptr = nullptr;

        m_variables.clear();
        m_pseudo_boolean_constraints.clear();
        m_cardinality_constraints.clear();
        m_learned_constraints.clear();

        m_variable_to_pseudo_boolean_positive_constraints.clear();
        m_variable_to_pseudo_boolean_negative_constraints.clear();

        m_pseudo_boolean_propagation_queue.clear();
        m_in_pseudo_boolean_propagation_queue.clear();
        m_cardinality_propagation_queue.clear();
        m_in_cardinality_propagation_queue.clear();
        m_learned_constraint_propagation_queue.clear();
        m_in_learned_constraint_propagation_queue.clear();

        m_initial_activities.clear();
    }

    /*************************************************************************/
    inline void setup(matrix_model::MatrixModel* a_instance_ptr) {
        this->initialize();
        m_instance_ptr = a_instance_ptr;

        this->setup_variables();
        this->setup_constraints();
        this->setup_initial_propagation_queue();
        this->setup_initial_activities();
        this->setup_initial_phases();
    }

    /*************************************************************************/
    inline void setup_variables(void) {
        const int N = m_instance_ptr->number_of_variables;
        m_variables.resize(N);
        for (int i = 0; i < N; i++) {
            m_variables[i].index = i;
        }
        m_variable_to_pseudo_boolean_positive_constraints.assign(N, {});
        m_variable_to_pseudo_boolean_negative_constraints.assign(N, {});
    }

    /*************************************************************************/
    inline void setup_constraints(void) {
        const int NUMBER_OF_CONSTRAINTS = m_instance_ptr->number_of_constraints;
        const auto& MATRIX = m_instance_ptr->constraint_coefficients;

        for (int i = 0; i < NUMBER_OF_CONSTRAINTS; i++) {
            const int PTR_BEGIN                   = MATRIX.row_ptrs[i];
            const int PTR_END                     = MATRIX.row_ptrs[i + 1];
            const int NUMBER_OF_NON_ZERO_ELEMENTS = PTR_END - PTR_BEGIN;

            std::vector<double> raw_coefficients(NUMBER_OF_NON_ZERO_ELEMENTS);
            std::vector<int>    variable_indices(NUMBER_OF_NON_ZERO_ELEMENTS);
            for (int j = 0; j < NUMBER_OF_NON_ZERO_ELEMENTS; j++) {
                raw_coefficients[j] = MATRIX.values[PTR_BEGIN + j];
                variable_indices[j] = MATRIX.column_indices[PTR_BEGIN + j];
            }

            const double LOWER_BOUND =
                m_instance_ptr->constraint_lower_bounds.values[i];
            const double UPPER_BOUND =
                m_instance_ptr->constraint_upper_bounds.values[i];

            if (std::isfinite(LOWER_BOUND)) {
                this->build_constraint(variable_indices, raw_coefficients,
                                       LOWER_BOUND);
            }

            const bool IS_EQUAL =
                (i >= m_instance_ptr->equal_constraint_index_range.first &&
                 i < m_instance_ptr->equal_constraint_index_range.second);

            if (IS_EQUAL && std::isfinite(UPPER_BOUND)) {
                std::vector<double> negative_coefficients(
                    NUMBER_OF_NON_ZERO_ELEMENTS);
                for (int j = 0; j < NUMBER_OF_NON_ZERO_ELEMENTS; j++) {
                    negative_coefficients[j] = -raw_coefficients[j];
                }
                this->build_constraint(variable_indices, negative_coefficients,
                                       -UPPER_BOUND);
            }
        }

        const int PSEUDO_BOOLEAN_CONSTRAINTS_SIZE =
            static_cast<int>(m_pseudo_boolean_constraints.size());
        const int CARDINALITY_CONSTRAINTS_SIZE =
            static_cast<int>(m_cardinality_constraints.size());
        for (int constraint_index = 0;
             constraint_index < PSEUDO_BOOLEAN_CONSTRAINTS_SIZE;
             constraint_index++) {
            const auto& PSEUDO_BOOLEAN_CONSTRAINT =
                m_pseudo_boolean_constraints[constraint_index];
            for (const auto& [variable_index, coefficient] :
                 PSEUDO_BOOLEAN_CONSTRAINT.positive_terms) {
                m_variable_to_pseudo_boolean_positive_constraints
                    [variable_index]
                        .emplace_back(constraint_index, coefficient);
            }
            for (const auto& [variable_index, coefficient] :
                 PSEUDO_BOOLEAN_CONSTRAINT.negative_terms) {
                m_variable_to_pseudo_boolean_negative_constraints
                    [variable_index]
                        .emplace_back(constraint_index, coefficient);
            }
        }

        for (int constraint_index = 0;
             constraint_index < CARDINALITY_CONSTRAINTS_SIZE;
             constraint_index++) {
            const auto& CARDINALITY_CONSTRAINT =
                m_cardinality_constraints[constraint_index];
            for (int variable_index :
                 CARDINALITY_CONSTRAINT.positive_variable_indices) {
                m_variables[variable_index]
                    .related_cardinality_positive_constraint_indices.push_back(
                        constraint_index);
            }
            for (int variable_index :
                 CARDINALITY_CONSTRAINT.negative_variable_indices) {
                m_variables[variable_index]
                    .related_cardinality_negative_constraint_indices.push_back(
                        constraint_index);
            }
        }
    }

    /*************************************************************************/
    inline void setup_initial_propagation_queue(void) {
        m_in_pseudo_boolean_propagation_queue.assign(
            number_of_pseudo_boolean_constraints(), 0);
        m_pseudo_boolean_propagation_queue.clear();
        for (int i = 0; i < number_of_pseudo_boolean_constraints(); i++) {
            m_pseudo_boolean_propagation_queue.push_back(i);
            m_in_pseudo_boolean_propagation_queue[i] = 1;
        }

        m_in_cardinality_propagation_queue.assign(
            number_of_cardinality_constraints(), 0);
        m_cardinality_propagation_queue.clear();
        for (int i = 0; i < number_of_cardinality_constraints(); i++) {
            m_cardinality_propagation_queue.push_back(i);
            m_in_cardinality_propagation_queue[i] = 1;
        }

        m_learned_constraint_propagation_queue.clear();
        m_in_learned_constraint_propagation_queue.clear();
    }

    /*************************************************************************/
    inline void setup_initial_activities(void) {
        const int NUMBER_OF_CONSTRAINTS = m_instance_ptr->number_of_constraints;
        const auto& MATRIX = m_instance_ptr->constraint_coefficients;

        std::vector<double> activity_scores(this->number_of_variables(), 0.0);

        for (int i = 0; i < NUMBER_OF_CONSTRAINTS; i++) {
            const int PTR_BEGIN                   = MATRIX.row_ptrs[i];
            const int PTR_END                     = MATRIX.row_ptrs[i + 1];
            const int NUMBER_OF_NON_ZERO_ELEMENTS = PTR_END - PTR_BEGIN;
            if (NUMBER_OF_NON_ZERO_ELEMENTS == 0) {
                continue;
            }

            const double WEIGHT =
                1.0 / static_cast<double>(NUMBER_OF_NON_ZERO_ELEMENTS);

            for (int j = PTR_BEGIN; j < PTR_END; j++) {
                const int VARIABLE_INDEX = MATRIX.column_indices[j];
                activity_scores[VARIABLE_INDEX] += WEIGHT;
            }
        }

        m_initial_activities = activity_scores;
    }

    /*************************************************************************/
    inline void setup_initial_phases(void) {
        /**
         * Use the authoritative constraint type from the Model
         * (via MatrixModel::constraint_ptrs) to determine the initial phase
         * for each variable.
         *
         * Classification for phase initialisation:
         *   MUST-COVER  (prefer phase = 1):
         *     SetCovering     — Σxi >= k  (must assign at least k ones)
         *     SetPartitioning — Σxi == 1  (exactly one must be chosen)
         *     Cardinality     — Σxi >= k  (general cardinality covering)
         *     MultipleCovering
         *   SET-PACKING (prefer phase = 0):
         *     SetPacking      — Σxi <= 1  (at most one can be chosen)
         *     InvariantKnapsack
         */
        const int NUMBER_OF_CONSTRAINTS = m_instance_ptr->number_of_constraints;
        const auto& MATRIX = m_instance_ptr->constraint_coefficients;
        const int   NUMBER_OF_VARIABLES = this->number_of_variables();

        std::vector<int> must_cover_count(NUMBER_OF_VARIABLES, 0);
        std::vector<int> packing_count(NUMBER_OF_VARIABLES, 0);

        for (int i = 0; i < NUMBER_OF_CONSTRAINTS; i++) {
            const int PTR_BEGIN = MATRIX.row_ptrs[i];
            const int PTR_END   = MATRIX.row_ptrs[i + 1];
            if (PTR_BEGIN == PTR_END) {
                continue;
            }

            bool IS_MUST_COVER = false;
            bool IS_PACKING    = false;

            auto* constraint_ptr =
                m_instance_ptr->constraint_index_map.reverse_at(i);
            switch (constraint_ptr->type()) {
                case model_component::ConstraintType::SetCovering:
                case model_component::ConstraintType::SetPartitioning:
                case model_component::ConstraintType::Cardinality:
                case model_component::ConstraintType::MultipleCovering:
                    IS_MUST_COVER = true;
                    break;
                case model_component::ConstraintType::SetPacking:
                case model_component::ConstraintType::InvariantKnapsack:
                    IS_PACKING = true;
                    break;
                case model_component::ConstraintType::SoftSelection: {
                    /**
                     * SoftSelection: x_key = sum(x_j).
                     * The key variable represents the selection obligation
                     * (must cover, prefer 1). The candidate variables are
                     * mutually exclusive (packing, prefer 0).
                     */
                    auto* key_variable_ptr = constraint_ptr->key_variable_ptr();
                    int   key_variable_index = -1;
                    if (key_variable_ptr != nullptr &&
                        m_instance_ptr->variable_index_map.contains_first(
                            key_variable_ptr)) {
                        key_variable_index =
                            m_instance_ptr->variable_index_map.forward_at(
                                key_variable_ptr);
                    }

                    for (int j = PTR_BEGIN; j < PTR_END; j++) {
                        const int VARIABLE_INDEX = MATRIX.column_indices[j];
                        if (VARIABLE_INDEX == key_variable_index) {
                            must_cover_count[VARIABLE_INDEX]++;
                        } else {
                            packing_count[VARIABLE_INDEX]++;
                        }
                    }
                    continue;
                }
                default:
                    break;
            }

            for (int j = PTR_BEGIN; j < PTR_END; j++) {
                const int VARIABLE_INDEX = MATRIX.column_indices[j];
                if (IS_MUST_COVER) {
                    must_cover_count[VARIABLE_INDEX]++;
                }
                if (IS_PACKING) {
                    packing_count[VARIABLE_INDEX]++;
                }
            }
        }

        for (int i = 0; i < NUMBER_OF_VARIABLES; i++) {
            const bool ANY_MUST_COVER = (must_cover_count[i] > 0);
            const bool ANY_PACKING    = (packing_count[i] > 0);

            if (ANY_MUST_COVER && !ANY_PACKING) {
                m_variables[i].phase = 1;
            } else {
                // If packing is present (with or without must-cover), prefer
                // phase = 0 to avoid immediate conflict on packing constraints.
                m_variables[i].phase = 0;
            }
        }
    }

    /*************************************************************************/
    inline void assign(const int a_VARIABLE_INDEX, const int a_VALUE,
                       const int                      a_DECISION_LEVEL,
                       const CDCLConstraintReference& a_REASON) {
        auto& variable = m_variables[a_VARIABLE_INDEX];
        variable.assign(a_VALUE, a_DECISION_LEVEL, a_REASON);

        if (a_VALUE == 1) {
            for (const auto& [constraint_index, coefficient] :
                 m_variable_to_pseudo_boolean_positive_constraints
                     [a_VARIABLE_INDEX]) {
                m_pseudo_boolean_constraints[constraint_index]
                    .on_variable_assigned(true, coefficient);
            }
            for (const auto& [constraint_index, coefficient] :
                 m_variable_to_pseudo_boolean_negative_constraints
                     [a_VARIABLE_INDEX]) {
                auto& pseudo_boolean_constraint =
                    m_pseudo_boolean_constraints[constraint_index];
                pseudo_boolean_constraint.on_variable_assigned(false,
                                                               coefficient);
                if (!pseudo_boolean_constraint.is_satisfied() &&
                    !m_in_pseudo_boolean_propagation_queue[constraint_index]) {
                    m_pseudo_boolean_propagation_queue.push_back(
                        constraint_index);
                    m_in_pseudo_boolean_propagation_queue[constraint_index] = 1;
                }
            }

            for (int constraint_index :
                 variable.related_cardinality_positive_constraint_indices) {
                m_cardinality_constraints[constraint_index]
                    .on_variable_assigned(true);
            }
            for (int constraint_index :
                 variable.related_learned_positive_constraint_indices) {
                m_learned_constraints[constraint_index].on_variable_assigned(
                    true);
            }

            for (int constraint_index :
                 variable.related_cardinality_negative_constraint_indices) {
                auto& cardinality_constraint =
                    m_cardinality_constraints[constraint_index];
                cardinality_constraint.on_variable_assigned(false);
                if (!cardinality_constraint.is_satisfied() &&
                    !m_in_cardinality_propagation_queue[constraint_index]) {
                    m_cardinality_propagation_queue.push_back(constraint_index);
                    m_in_cardinality_propagation_queue[constraint_index] = 1;
                }
            }
            for (int constraint_index :
                 variable.related_learned_negative_constraint_indices) {
                auto& learned_constraint =
                    m_learned_constraints[constraint_index];
                learned_constraint.on_variable_assigned(false);
                if (!learned_constraint.is_satisfied() &&
                    !m_in_learned_constraint_propagation_queue
                        [constraint_index]) {
                    m_learned_constraint_propagation_queue.push_back(
                        constraint_index);
                    m_in_learned_constraint_propagation_queue
                        [constraint_index] = 1;
                }
            }
        } else {
            for (const auto& [constraint_index, coefficient] :
                 m_variable_to_pseudo_boolean_positive_constraints
                     [a_VARIABLE_INDEX]) {
                auto& pseudo_boolean_constraint =
                    m_pseudo_boolean_constraints[constraint_index];
                pseudo_boolean_constraint.on_variable_assigned(false,
                                                               coefficient);
                if (!pseudo_boolean_constraint.is_satisfied() &&
                    !m_in_pseudo_boolean_propagation_queue[constraint_index]) {
                    m_pseudo_boolean_propagation_queue.push_back(
                        constraint_index);
                    m_in_pseudo_boolean_propagation_queue[constraint_index] = 1;
                }
            }
            for (const auto& [constraint_index, coefficient] :
                 m_variable_to_pseudo_boolean_negative_constraints
                     [a_VARIABLE_INDEX]) {
                m_pseudo_boolean_constraints[constraint_index]
                    .on_variable_assigned(true, coefficient);
            }

            for (int constraint_index :
                 variable.related_cardinality_positive_constraint_indices) {
                auto& cardinality_constraint =
                    m_cardinality_constraints[constraint_index];
                cardinality_constraint.on_variable_assigned(false);
                if (!cardinality_constraint.is_satisfied() &&
                    !m_in_cardinality_propagation_queue[constraint_index]) {
                    m_cardinality_propagation_queue.push_back(constraint_index);
                    m_in_cardinality_propagation_queue[constraint_index] = 1;
                }
            }
            for (int constraint_index :
                 variable.related_learned_positive_constraint_indices) {
                auto& learned_constraint =
                    m_learned_constraints[constraint_index];
                learned_constraint.on_variable_assigned(false);
                if (!learned_constraint.is_satisfied() &&
                    !m_in_learned_constraint_propagation_queue
                        [constraint_index]) {
                    m_learned_constraint_propagation_queue.push_back(
                        constraint_index);
                    m_in_learned_constraint_propagation_queue
                        [constraint_index] = 1;
                }
            }

            for (int constraint_index :
                 variable.related_cardinality_negative_constraint_indices) {
                m_cardinality_constraints[constraint_index]
                    .on_variable_assigned(true);
            }
            for (int constraint_index :
                 variable.related_learned_negative_constraint_indices) {
                m_learned_constraints[constraint_index].on_variable_assigned(
                    true);
            }
        }
    }

    /*************************************************************************/
    inline void rollback(const int a_VARIABLE_INDEX) {
        auto&     variable = m_variables[a_VARIABLE_INDEX];
        const int VALUE    = variable.value;

        for (const auto& [constraint_index, coefficient] :
             m_variable_to_pseudo_boolean_positive_constraints
                 [a_VARIABLE_INDEX]) {
            m_pseudo_boolean_constraints[constraint_index]
                .on_variable_removed(  //
                    VALUE == 1, coefficient);
        }

        for (const auto& [constraint_index, coefficient] :
             m_variable_to_pseudo_boolean_negative_constraints
                 [a_VARIABLE_INDEX]) {
            m_pseudo_boolean_constraints[constraint_index]
                .on_variable_removed(  //
                    VALUE == 0, coefficient);
        }

        for (int constraint_index :
             variable.related_cardinality_positive_constraint_indices) {
            m_cardinality_constraints[constraint_index].on_variable_removed(  //
                VALUE == 1);
        }

        for (int constraint_index :
             variable.related_learned_positive_constraint_indices) {
            m_learned_constraints[constraint_index].on_variable_removed(  //
                VALUE == 1);
        }

        for (int constraint_index :
             variable.related_cardinality_negative_constraint_indices) {
            m_cardinality_constraints[constraint_index].on_variable_removed(  //
                VALUE == 0);
        }

        for (int constraint_index :
             variable.related_learned_negative_constraint_indices) {
            m_learned_constraints[constraint_index].on_variable_removed(  //
                VALUE == 0);
        }

        variable.remove();
    }

    /*************************************************************************/
    template <typename T_AssignCallback>
    inline CDCLConstraintReference propagate(
        const T_AssignCallback& a_ASSIGN_CALLBACK) {
        CDCLConstraintReference null_reference{CDCLConstraintType::NONE,
                                               CDCLCoreConstant::NO_CONSTRAINT};

        while (!m_learned_constraint_propagation_queue.empty() ||
               !m_cardinality_propagation_queue.empty() ||
               !m_pseudo_boolean_propagation_queue.empty()) {
            while (!m_learned_constraint_propagation_queue.empty()) {
                const int CONSTRAINT_INDEX =
                    m_learned_constraint_propagation_queue.back();
                m_learned_constraint_propagation_queue.pop_back();
                m_in_learned_constraint_propagation_queue[CONSTRAINT_INDEX] = 0;

                const auto& LEARNED_CONSTRAINT =
                    m_learned_constraints[CONSTRAINT_INDEX];
                CDCLConstraintReference reference{CDCLConstraintType::LEARNED,
                                                  CONSTRAINT_INDEX};

                const int MAX_SUM = LEARNED_CONSTRAINT.max_possible_lhs_sum();
                if (MAX_SUM < LEARNED_CONSTRAINT.rhs) {
                    return reference;
                }

                if (MAX_SUM == LEARNED_CONSTRAINT.rhs &&
                    LEARNED_CONSTRAINT.current_lhs_sum <
                        LEARNED_CONSTRAINT.rhs) {
                    for (int variable_index :
                         LEARNED_CONSTRAINT.positive_variable_indices) {
                        if (!m_variables[variable_index].is_assigned()) {
                            a_ASSIGN_CALLBACK(variable_index, 1, reference);
                        }
                    }
                    for (int variable_index :
                         LEARNED_CONSTRAINT.negative_variable_indices) {
                        if (!m_variables[variable_index].is_assigned()) {
                            a_ASSIGN_CALLBACK(variable_index, 0, reference);
                        }
                    }
                }
            }

            if (!m_cardinality_propagation_queue.empty()) {
                const int CONSTRAINT_INDEX =
                    m_cardinality_propagation_queue.back();
                m_cardinality_propagation_queue.pop_back();
                m_in_cardinality_propagation_queue[CONSTRAINT_INDEX] = 0;

                const auto& CARDINALITY_CONSTRAINT =
                    m_cardinality_constraints[CONSTRAINT_INDEX];
                CDCLConstraintReference reference{
                    CDCLConstraintType::CARDINALITY, CONSTRAINT_INDEX};

                const int MAX_SUM =
                    CARDINALITY_CONSTRAINT.max_possible_lhs_sum();
                if (MAX_SUM < CARDINALITY_CONSTRAINT.rhs) {
                    return reference;
                }

                if (MAX_SUM == CARDINALITY_CONSTRAINT.rhs &&
                    CARDINALITY_CONSTRAINT.current_lhs_sum <
                        CARDINALITY_CONSTRAINT.rhs) {
                    for (int variable_index :
                         CARDINALITY_CONSTRAINT.positive_variable_indices) {
                        if (!m_variables[variable_index].is_assigned()) {
                            a_ASSIGN_CALLBACK(variable_index, 1, reference);
                        }
                    }
                    for (int variable_index :
                         CARDINALITY_CONSTRAINT.negative_variable_indices) {
                        if (!m_variables[variable_index].is_assigned()) {
                            a_ASSIGN_CALLBACK(variable_index, 0, reference);
                        }
                    }
                }
                continue;
            }

            if (!m_pseudo_boolean_propagation_queue.empty()) {
                const int CONSTRAINT_INDEX =
                    m_pseudo_boolean_propagation_queue.back();
                m_pseudo_boolean_propagation_queue.pop_back();
                m_in_pseudo_boolean_propagation_queue[CONSTRAINT_INDEX] = 0;

                const auto& PSEUDO_BOOLEAN_CONSTRAINT =
                    m_pseudo_boolean_constraints[CONSTRAINT_INDEX];
                CDCLConstraintReference reference{
                    CDCLConstraintType::PSEUDO_BOOLEAN, CONSTRAINT_INDEX};

                const int64_t MAX_SUM =
                    PSEUDO_BOOLEAN_CONSTRAINT.max_possible_lhs_sum();
                if (MAX_SUM < PSEUDO_BOOLEAN_CONSTRAINT.rhs) {
                    return reference;
                }

                const int64_t SLACK = MAX_SUM - PSEUDO_BOOLEAN_CONSTRAINT.rhs;

                for (const auto& [variable_index, coeff] :
                     PSEUDO_BOOLEAN_CONSTRAINT.positive_terms) {
                    if (!m_variables[variable_index].is_assigned() &&
                        SLACK < coeff) {
                        a_ASSIGN_CALLBACK(variable_index, 1, reference);
                    }
                }
                for (const auto& [variable_index, coeff] :
                     PSEUDO_BOOLEAN_CONSTRAINT.negative_terms) {
                    if (!m_variables[variable_index].is_assigned() &&
                        SLACK < coeff) {
                        a_ASSIGN_CALLBACK(variable_index, 0, reference);
                    }
                }
            }
        }

        return null_reference;
    }

    /*************************************************************************/
    inline CDCLConstraintReference add_learned_constraint(
        CardinalityConstraint&& a_CONSTRAINT) {
        int current_lhs_sum  = 0;
        int unassigned_count = 0;

        for (int variable_index : a_CONSTRAINT.positive_variable_indices) {
            if (m_variables[variable_index].is_assigned()) {
                if (m_variables[variable_index].value == 1) {
                    current_lhs_sum++;
                }
            } else {
                unassigned_count++;
            }
        }
        for (int variable_index : a_CONSTRAINT.negative_variable_indices) {
            if (m_variables[variable_index].is_assigned()) {
                if (m_variables[variable_index].value == 0) {
                    current_lhs_sum++;
                }
            } else {
                unassigned_count++;
            }
        }

        const int LEARNED_CONSTRAINT_INDEX =
            static_cast<int>(m_learned_constraints.size());
        m_learned_constraints.push_back(std::move(a_CONSTRAINT));

        auto& learned_constraint            = m_learned_constraints.back();
        learned_constraint.index            = LEARNED_CONSTRAINT_INDEX;
        learned_constraint.current_lhs_sum  = current_lhs_sum;
        learned_constraint.unassigned_count = unassigned_count;

        m_in_learned_constraint_propagation_queue.push_back(0);

        for (int variable_index :
             learned_constraint.positive_variable_indices) {
            m_variables[variable_index]
                .related_learned_positive_constraint_indices.push_back(
                    LEARNED_CONSTRAINT_INDEX);
        }
        for (int variable_index :
             learned_constraint.negative_variable_indices) {
            m_variables[variable_index]
                .related_learned_negative_constraint_indices.push_back(
                    LEARNED_CONSTRAINT_INDEX);
        }

        return CDCLConstraintReference{CDCLConstraintType::LEARNED,
                                       LEARNED_CONSTRAINT_INDEX};
    }

    /*************************************************************************/
    inline int reduce_learned_constraints(
        const double a_REDUCTION_RATE                              = 0.5,
        const int    a_LITERAL_BLOCK_DISTANCE_PROTECTION_THRESHOLD = 2) {
        const int NUMBER_OF_LEARNED_CONSTRAINTS =
            static_cast<int>(m_learned_constraints.size());
        std::vector<short> is_locked(NUMBER_OF_LEARNED_CONSTRAINTS, false);

        for (const auto& variable : m_variables) {
            if (variable.is_assigned() && variable.reason_constraint.type ==
                                              CDCLConstraintType::LEARNED) {
                const int LEARNED_CONSTRAINT_INDEX =
                    variable.reason_constraint.index;
                /**
                 * Guard: reason_constraint.index can be NO_CONSTRAINT (= -1)
                 * when the variable was assigned without an explicit reason.
                 * Using a negative index as a vector subscript is UB / heap
                 * underflow, so we must check >= 0 before accessing is_locked.
                 */
                if (LEARNED_CONSTRAINT_INDEX >= 0 &&
                    LEARNED_CONSTRAINT_INDEX < NUMBER_OF_LEARNED_CONSTRAINTS) {
                    is_locked[LEARNED_CONSTRAINT_INDEX] = true;
                }
            }
        }

        std::vector<std::pair<int, int>> candidates;
        for (int i = 0; i < NUMBER_OF_LEARNED_CONSTRAINTS; i++) {
            if (!is_locked[i] &&
                m_learned_constraints[i].literal_block_distance >
                    a_LITERAL_BLOCK_DISTANCE_PROTECTION_THRESHOLD) {
                candidates.push_back(
                    {m_learned_constraints[i].literal_block_distance, i});
            }
        }
        std::sort(candidates.rbegin(), candidates.rend());
        const int NUMBER_OF_CONSTRAINTS_TO_REMOVE =
            static_cast<int>(candidates.size() * a_REDUCTION_RATE);
        std::vector<short> to_remove(NUMBER_OF_LEARNED_CONSTRAINTS, false);
        for (int i = 0; i < NUMBER_OF_CONSTRAINTS_TO_REMOVE; i++) {
            to_remove[candidates[i].second] = true;
        }

        std::vector<CardinalityConstraint> new_learned_constraints;
        std::vector<int>     old_to_new(NUMBER_OF_LEARNED_CONSTRAINTS,
                                        CDCLCoreConstant::NO_CONSTRAINT);
        std::vector<uint8_t> new_in_queue;

        for (int i = 0; i < NUMBER_OF_LEARNED_CONSTRAINTS; i++) {
            if (!to_remove[i]) {
                old_to_new[i] =
                    static_cast<int>(new_learned_constraints.size());
                new_learned_constraints.push_back(
                    std::move(m_learned_constraints[i]));
                new_learned_constraints.back().index = old_to_new[i];
                new_in_queue.push_back(
                    m_in_learned_constraint_propagation_queue[i]);
            }
        }

        m_learned_constraints = std::move(new_learned_constraints);
        m_in_learned_constraint_propagation_queue = std::move(new_in_queue);

        for (auto& variable : m_variables) {
            if (variable.reason_constraint.type ==
                CDCLConstraintType::LEARNED) {
                const int LEARNED_CONSTRAINT_INDEX_OLD =
                    variable.reason_constraint.index;
                /**
                 * Guard: reason_constraint.index may be NO_CONSTRAINT (= -1).
                 * Indexing old_to_new with a negative value is UB.
                 */
                if (LEARNED_CONSTRAINT_INDEX_OLD < 0 ||
                    LEARNED_CONSTRAINT_INDEX_OLD >=
                        NUMBER_OF_LEARNED_CONSTRAINTS) {
                    variable.reason_constraint.type = CDCLConstraintType::NONE;
                    variable.reason_constraint.index =
                        CDCLCoreConstant::NO_CONSTRAINT;
                } else {
                    const int LEARNED_CONSTRAINT_INDEX_NEW =
                        old_to_new[LEARNED_CONSTRAINT_INDEX_OLD];
                    if (LEARNED_CONSTRAINT_INDEX_NEW !=
                        CDCLCoreConstant::NO_CONSTRAINT) {
                        variable.reason_constraint.index =
                            LEARNED_CONSTRAINT_INDEX_NEW;
                    } else {
                        variable.reason_constraint.type =
                            CDCLConstraintType::NONE;
                        variable.reason_constraint.index =
                            CDCLCoreConstant::NO_CONSTRAINT;
                    }
                }
            }

            int positive_count = 0;
            int negative_count = 0;

            const int RELATED_LEARNED_POSITIVE_CONSTRAINT_INDICES_SIZE =
                static_cast<int>(
                    variable.related_learned_positive_constraint_indices
                        .size());
            const int RELATED_LEARNED_NEGATIVE_CONSTRAINT_INDICES_SIZE =
                static_cast<int>(
                    variable.related_learned_negative_constraint_indices
                        .size());

            for (int j = 0;
                 j < RELATED_LEARNED_POSITIVE_CONSTRAINT_INDICES_SIZE; j++) {
                const int LEARNED_CONSTRAINT_INDEX_OLD =
                    variable.related_learned_positive_constraint_indices[j];
                if (LEARNED_CONSTRAINT_INDEX_OLD < 0 ||
                    LEARNED_CONSTRAINT_INDEX_OLD >=
                        NUMBER_OF_LEARNED_CONSTRAINTS) {
                    continue;
                }
                const int LEARNED_CONSTRAINT_INDEX_NEW =
                    old_to_new[LEARNED_CONSTRAINT_INDEX_OLD];
                if (LEARNED_CONSTRAINT_INDEX_NEW !=
                    CDCLCoreConstant::NO_CONSTRAINT) {
                    variable.related_learned_positive_constraint_indices
                        [positive_count++] = LEARNED_CONSTRAINT_INDEX_NEW;
                }
            }
            variable.related_learned_positive_constraint_indices.resize(
                positive_count);

            for (int j = 0;
                 j < RELATED_LEARNED_NEGATIVE_CONSTRAINT_INDICES_SIZE; j++) {
                const int LEARNED_CONSTRAINT_INDEX_OLD =
                    variable.related_learned_negative_constraint_indices[j];
                if (LEARNED_CONSTRAINT_INDEX_OLD < 0 ||
                    LEARNED_CONSTRAINT_INDEX_OLD >=
                        NUMBER_OF_LEARNED_CONSTRAINTS) {
                    continue;
                }
                const int LEARNED_CONSTRAINT_INDEX_NEW =
                    old_to_new[LEARNED_CONSTRAINT_INDEX_OLD];

                if (LEARNED_CONSTRAINT_INDEX_NEW !=
                    CDCLCoreConstant::NO_CONSTRAINT) {
                    variable.related_learned_negative_constraint_indices
                        [negative_count++] = LEARNED_CONSTRAINT_INDEX_NEW;
                }
            }
            variable.related_learned_negative_constraint_indices.resize(
                negative_count);
        }

        std::vector<int> new_queue;
        for (int queue_index : m_learned_constraint_propagation_queue) {
            const int LEARNED_CONSTRAINT_INDEX_NEW = old_to_new[queue_index];
            if (LEARNED_CONSTRAINT_INDEX_NEW !=
                CDCLCoreConstant::NO_CONSTRAINT) {
                new_queue.push_back(LEARNED_CONSTRAINT_INDEX_NEW);
            }
        }
        m_learned_constraint_propagation_queue = std::move(new_queue);

        return NUMBER_OF_CONSTRAINTS_TO_REMOVE;
    }

    /*************************************************************************/
    inline void clear_propagation_queue(void) {
        m_pseudo_boolean_propagation_queue.clear();
        m_in_pseudo_boolean_propagation_queue.assign(
            number_of_pseudo_boolean_constraints(), false);
        m_cardinality_propagation_queue.clear();
        m_in_cardinality_propagation_queue.assign(
            number_of_cardinality_constraints(), false);
        m_learned_constraint_propagation_queue.clear();
        m_in_learned_constraint_propagation_queue.assign(
            static_cast<int>(m_learned_constraints.size()), false);
    }

    /*************************************************************************/
    inline const std::vector<int>& positive_variable_indices(
        const CDCLConstraintReference& a_REFERENCE) const noexcept {
        if (a_REFERENCE.type == CDCLConstraintType::PSEUDO_BOOLEAN) {
            return m_pseudo_boolean_constraints[a_REFERENCE.index]
                .positive_variable_indices;
        } else if (a_REFERENCE.type == CDCLConstraintType::CARDINALITY) {
            return m_cardinality_constraints[a_REFERENCE.index]
                .positive_variable_indices;
        } else if (a_REFERENCE.type == CDCLConstraintType::LEARNED) {
            return m_learned_constraints[a_REFERENCE.index]
                .positive_variable_indices;
        }
        static const std::vector<int> EMPTY_INDICES;
        return EMPTY_INDICES;
    }

    /*************************************************************************/
    inline const std::vector<int>& negative_variable_indices(
        const CDCLConstraintReference& a_REFERENCE) const noexcept {
        if (a_REFERENCE.type == CDCLConstraintType::PSEUDO_BOOLEAN) {
            return m_pseudo_boolean_constraints[a_REFERENCE.index]
                .negative_variable_indices;
        } else if (a_REFERENCE.type == CDCLConstraintType::CARDINALITY) {
            return m_cardinality_constraints[a_REFERENCE.index]
                .negative_variable_indices;
        } else if (a_REFERENCE.type == CDCLConstraintType::LEARNED) {
            return m_learned_constraints[a_REFERENCE.index]
                .negative_variable_indices;
        }
        static const std::vector<int> EMPTY_INDICES;
        return EMPTY_INDICES;
    }

    /*************************************************************************/
    inline const std::vector<CDCLVariable>& variables(void) const noexcept {
        return m_variables;
    }

    /*************************************************************************/
    inline const CDCLVariable& variable(const int a_INDEX) const noexcept {
        return m_variables[a_INDEX];
    }

    /*************************************************************************/
    inline const std::vector<double>& initial_activities(void) const noexcept {
        return m_initial_activities;
    }

    /*************************************************************************/
    inline int number_of_variables(void) const noexcept {
        return static_cast<int>(m_variables.size());
    }

    /*************************************************************************/
    inline int number_of_pseudo_boolean_constraints(void) const noexcept {
        return static_cast<int>(m_pseudo_boolean_constraints.size());
    }

    /*************************************************************************/
    inline int number_of_cardinality_constraints(void) const noexcept {
        return static_cast<int>(m_cardinality_constraints.size());
    }

    /*************************************************************************/
    inline int number_of_learned_constraints(void) const noexcept {
        return static_cast<int>(m_learned_constraints.size());
    }

    /*************************************************************************/
    inline int number_of_constraints(void) const noexcept {
        return number_of_pseudo_boolean_constraints() +
               number_of_cardinality_constraints() +
               static_cast<int>(m_learned_constraints.size());
    }

    /*************************************************************************/
    inline bool is_selection_variable(const int a_INDEX) const noexcept {
        return (m_instance_ptr->variable_index_map.reverse_at(a_INDEX)
                    ->selection_ptr() != nullptr);
    }
};

}  // namespace printemps::solver::cdcl::core

#endif
/*****************************************************************************/
// END
/*****************************************************************************/
