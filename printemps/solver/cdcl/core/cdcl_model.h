/*****************************************************************************/
// Copyright (c) 2020-2026 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_CDCL_CORE_CDCL_MODEL_H__
#define PRINTEMPS_SOLVER_CDCL_CORE_CDCL_MODEL_H__

#include "cdcl_variable.h"
#include "cardinality_constraint.h"
#include "pseudo_boolean_constraint.h"
#include "cdcl_clause.h"

namespace printemps::solver::cdcl::core {

/*****************************************************************************/
class CDCLModel {
   private:
    matrix_model::MatrixModel* m_instance_ptr;

    std::vector<CDCLVariable>     m_variables;
    std::vector<int8_t>           m_values;
    std::vector<std::vector<int>> m_selection_variable_indices;

    std::vector<PseudoBooleanConstraint>  m_pseudo_boolean_constraints;
    std::vector<CardinalityConstraint>    m_cardinality_constraints;
    std::vector<CDCLClause>               m_clauses;
    std::vector<std::vector<CDCLWatcher>> m_watches;

    std::vector<std::vector<std::pair<int, int>>>
        m_variable_to_pseudo_boolean_positive_constraints;
    std::vector<std::vector<std::pair<int, int>>>
        m_variable_to_pseudo_boolean_negative_constraints;

    std::vector<std::vector<int>>
        m_variable_to_cardinality_positive_constraints;
    std::vector<std::vector<int>>
        m_variable_to_cardinality_negative_constraints;

    std::vector<int>     m_pseudo_boolean_propagation_queue;
    std::vector<uint8_t> m_in_pseudo_boolean_propagation_queue;

    std::vector<int>     m_cardinality_propagation_queue;
    std::vector<uint8_t> m_in_cardinality_propagation_queue;

    std::vector<double> m_initial_activities;

    /*************************************************************************/
    inline bool is_literal_satisfied(const int a_LITERAL) const noexcept {
        return m_values[literal_variable(a_LITERAL)] ==
               (literal_sign(a_LITERAL) ^ 1);
    }

    inline bool is_literal_falsified(const int a_LITERAL) const noexcept {
        return m_values[literal_variable(a_LITERAL)] == literal_sign(a_LITERAL);
    }

    /*************************************************************************/
    inline void build_constraint(const std::vector<int>&    a_VARIABLE_INDICES,
                                 const std::vector<double>& a_COEFFICIENTS,
                                 const double               a_RHS) {
        const int NUMBER_OF_NON_ZERO_ELEMENTS =
            static_cast<int>(a_VARIABLE_INDICES.size());

        if (NUMBER_OF_NON_ZERO_ELEMENTS == 0) {
            return;
        }

        // Fast path for pure +/- 1 coefficients and integer RHS:
        bool is_pure_cardinality = utility::is_integer(a_RHS);
        if (is_pure_cardinality) {
            for (int j = 0; j < NUMBER_OF_NON_ZERO_ELEMENTS; j++) {
                const double COEFFICIENT = a_COEFFICIENTS[j];
                if (COEFFICIENT != 1.0 && COEFFICIENT != -1.0) {
                    is_pure_cardinality = false;
                    break;
                }
            }
        }

        if (is_pure_cardinality) {
            std::vector<int> positive_variable_indices;
            std::vector<int> negative_variable_indices;
            positive_variable_indices.reserve(NUMBER_OF_NON_ZERO_ELEMENTS);
            negative_variable_indices.reserve(NUMBER_OF_NON_ZERO_ELEMENTS);
            for (int j = 0; j < NUMBER_OF_NON_ZERO_ELEMENTS; j++) {
                if (a_COEFFICIENTS[j] > 0.0) {
                    positive_variable_indices.push_back(a_VARIABLE_INDICES[j]);
                } else {
                    negative_variable_indices.push_back(a_VARIABLE_INDICES[j]);
                }
            }
            int64_t rhs = static_cast<int64_t>(std::round(a_RHS));
            rhs += static_cast<int64_t>(negative_variable_indices.size());

            const int INDEX =
                static_cast<int>(m_cardinality_constraints.size());
            m_cardinality_constraints.emplace_back(
                INDEX, rhs, std::move(positive_variable_indices),
                std::move(negative_variable_indices));
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

        std::vector<int64_t> integer_coefficients(NUMBER_OF_NON_ZERO_ELEMENTS);
        int64_t              integer_rhs = 0;

        if (max_decimal_places > 0) {
            const double         SCALE = std::pow(10.0, max_decimal_places);
            std::vector<int64_t> scaled_values;
            scaled_values.reserve(NUMBER_OF_NON_ZERO_ELEMENTS + 1);

            for (int j = 0; j < NUMBER_OF_NON_ZERO_ELEMENTS; j++) {
                const int64_t COEFFICIENT =
                    std::llround(a_COEFFICIENTS[j] * SCALE);
                integer_coefficients[j] = COEFFICIENT;
                if (COEFFICIENT != 0) {
                    scaled_values.push_back(std::abs(COEFFICIENT));
                }
            }

            const int64_t RHS = std::llround(a_RHS * SCALE);
            integer_rhs       = RHS;
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
                        integer_coefficients[j] /= gcd;
                    }
                    integer_rhs /= gcd;
                }
            }
        } else {
            for (int j = 0; j < NUMBER_OF_NON_ZERO_ELEMENTS; j++) {
                integer_coefficients[j] =
                    static_cast<int64_t>(std::round(a_COEFFICIENTS[j]));
            }
            integer_rhs = static_cast<int64_t>(std::round(a_RHS));
        }

        // 2. Check if the constraint is Cardinality (all non-zero coefficients
        // are +/- 1)
        bool is_cardinality = true;
        for (int j = 0; j < NUMBER_OF_NON_ZERO_ELEMENTS; j++) {
            if (std::abs(integer_coefficients[j]) != 1) {
                is_cardinality = false;
                break;
            }
        }

        if (is_cardinality) {
            std::vector<int> positive_variable_indices;
            std::vector<int> negative_variable_indices;
            for (int j = 0; j < NUMBER_OF_NON_ZERO_ELEMENTS; j++) {
                const int     VARIABLE_INDEX = a_VARIABLE_INDICES[j];
                const int64_t COEFFICIENT    = integer_coefficients[j];

                if (COEFFICIENT > 0) {
                    positive_variable_indices.push_back(VARIABLE_INDEX);
                } else if (COEFFICIENT < 0) {
                    negative_variable_indices.push_back(VARIABLE_INDEX);
                }
            }

            int64_t rhs = integer_rhs;
            rhs += static_cast<int64_t>(negative_variable_indices.size());

            const int INDEX =
                static_cast<int>(m_cardinality_constraints.size());
            m_cardinality_constraints.emplace_back(
                INDEX, rhs, std::move(positive_variable_indices),
                std::move(negative_variable_indices));
        } else {
            std::vector<std::pair<int, int64_t>> positive_terms;
            std::vector<std::pair<int, int64_t>> negative_terms;

            int64_t total_positive_coefficient = 0;
            int64_t rhs                        = integer_rhs;

            for (int j = 0; j < NUMBER_OF_NON_ZERO_ELEMENTS; j++) {
                const int     VARIABLE_INDEX = a_VARIABLE_INDICES[j];
                const int64_t COEFFICIENT    = integer_coefficients[j];

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
        m_values.clear();
        m_pseudo_boolean_constraints.clear();
        m_cardinality_constraints.clear();
        m_clauses.clear();
        m_watches.clear();

        m_variable_to_pseudo_boolean_positive_constraints.clear();
        m_variable_to_pseudo_boolean_negative_constraints.clear();

        m_variable_to_cardinality_positive_constraints.clear();
        m_variable_to_cardinality_negative_constraints.clear();

        m_pseudo_boolean_propagation_queue.clear();
        m_in_pseudo_boolean_propagation_queue.clear();
        m_cardinality_propagation_queue.clear();
        m_in_cardinality_propagation_queue.clear();

        m_selection_variable_indices.clear();
        m_initial_activities.clear();
    }

    /*************************************************************************/
    inline void setup(matrix_model::MatrixModel* a_instance_ptr) {
        this->initialize();
        m_instance_ptr = a_instance_ptr;

        this->setup_variables();
        this->setup_selections();
        this->setup_constraints();
        this->setup_initial_propagation_queue();
        this->setup_initial_activities();
        this->setup_initial_phases();
    }

    /*************************************************************************/
    inline void setup_variables(void) {
        const int N = m_instance_ptr->number_of_variables;
        m_variables.resize(N);
        m_values.assign(N, -1);
        for (int i = 0; i < N; i++) {
            m_variables[i].index = i;
        }
        m_variable_to_pseudo_boolean_positive_constraints.assign(N, {});
        m_variable_to_pseudo_boolean_negative_constraints.assign(N, {});
        m_variable_to_cardinality_positive_constraints.assign(N, {});
        m_variable_to_cardinality_negative_constraints.assign(N, {});
        m_watches.assign(2 * N, {});
    }

    /*************************************************************************/
    inline void setup_selections(void) {
        m_selection_variable_indices.clear();
        if (m_instance_ptr == nullptr ||
            m_instance_ptr->variable_index_map.empty()) {
            return;
        }
        std::unordered_map<void*, std::vector<int>> selection_map;
        const int N = static_cast<int>(m_variables.size());
        for (int i = 0; i < N; i++) {
            if (!m_instance_ptr->variable_index_map.contains_second(i)) {
                continue;
            }
            auto* variable_ptr =
                m_instance_ptr->variable_index_map.reverse_at(i);
            if (variable_ptr != nullptr &&
                variable_ptr->selection_ptr() != nullptr) {
                selection_map[static_cast<void*>(variable_ptr->selection_ptr())]
                    .push_back(i);
            }
        }
        m_selection_variable_indices.reserve(selection_map.size());
        for (auto& [ptr, indices] : selection_map) {
            m_selection_variable_indices.push_back(std::move(indices));
        }
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

            const bool HAS_LOWER_BOUND =
                std::isfinite(LOWER_BOUND) &&
                (LOWER_BOUND > -constant::LARGE_VALUE_50);
            const bool HAS_UPPER_BOUND =
                std::isfinite(UPPER_BOUND) &&
                (UPPER_BOUND < constant::LARGE_VALUE_50);

            if (HAS_LOWER_BOUND) {
                this->build_constraint(variable_indices, raw_coefficients,
                                       LOWER_BOUND);
            }

            if (HAS_UPPER_BOUND) {
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
                m_variable_to_cardinality_positive_constraints[variable_index]
                    .push_back(constraint_index);
            }
            for (int variable_index :
                 CARDINALITY_CONSTRAINT.negative_variable_indices) {
                m_variable_to_cardinality_negative_constraints[variable_index]
                    .push_back(constraint_index);
            }
        }
    }

    /*************************************************************************/
    inline void setup_initial_propagation_queue(void) {
        m_in_pseudo_boolean_propagation_queue.assign(
            this->number_of_pseudo_boolean_constraints(), 0);
        m_pseudo_boolean_propagation_queue.clear();
        for (int i = 0; i < this->number_of_pseudo_boolean_constraints(); i++) {
            m_pseudo_boolean_propagation_queue.push_back(i);
            m_in_pseudo_boolean_propagation_queue[i] = 1;
        }

        m_in_cardinality_propagation_queue.assign(
            this->number_of_cardinality_constraints(), 0);
        m_cardinality_propagation_queue.clear();
        for (int i = 0; i < this->number_of_cardinality_constraints(); i++) {
            m_cardinality_propagation_queue.push_back(i);
            m_in_cardinality_propagation_queue[i] = 1;
        }
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

            if (!m_instance_ptr->constraint_index_map.contains_second(i)) {
                continue;
            }
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
                m_variables[i].phase = 0;
            }
        }

        // Ensure that each Selection constraint has exactly one variable with
        // phase = 1
        for (const auto& selection_variable_indices :
             m_selection_variable_indices) {
            if (selection_variable_indices.empty()) {
                continue;
            }
            int selected_index = -1;
            // 1. Try to find if one variable was already designated as
            // selected_variable_ptr
            for (int variable_index : selection_variable_indices) {
                if (m_instance_ptr != nullptr &&
                    m_instance_ptr->variable_index_map.contains_second(
                        variable_index)) {
                    auto* variable_ptr =
                        m_instance_ptr->variable_index_map.reverse_at(
                            variable_index);
                    if (variable_ptr != nullptr &&
                        variable_ptr->selection_ptr() != nullptr &&
                        variable_ptr->selection_ptr()->selected_variable_ptr ==
                            variable_ptr) {
                        selected_index = variable_index;
                        break;
                    }
                }
            }
            // 2. If not found, check variable_initial_solution
            if (selected_index == -1 &&
                m_instance_ptr->variable_initial_solution.size() ==
                    static_cast<std::size_t>(NUMBER_OF_VARIABLES)) {
                for (int variable_index : selection_variable_indices) {
                    if (m_instance_ptr
                            ->variable_initial_solution[variable_index] == 1) {
                        selected_index = variable_index;
                        break;
                    }
                }
            }
            // 3. If still not found, pick variable with highest activity
            if (selected_index == -1) {
                selected_index = selection_variable_indices.front();
                for (int variable_index : selection_variable_indices) {
                    if (!m_initial_activities.empty() &&
                        m_initial_activities[variable_index] >
                            m_initial_activities[selected_index]) {
                        selected_index = variable_index;
                    }
                }
            }
            // Set phase = 1 for selected_index, phase = 0 for others
            for (int variable_index : selection_variable_indices) {
                m_variables[variable_index].phase =
                    (variable_index == selected_index) ? 1 : 0;
            }
        }
    }

    /*************************************************************************/
    inline void assign(const int a_VARIABLE_INDEX, const int a_VALUE,
                       const int                      a_DECISION_LEVEL,
                       const CDCLConstraintReference& a_REASON) {
        auto& variable = m_variables[a_VARIABLE_INDEX];
        variable.assign(a_VALUE, a_DECISION_LEVEL, a_REASON);
        m_values[a_VARIABLE_INDEX] = static_cast<int8_t>(a_VALUE);

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
                 m_variable_to_cardinality_positive_constraints
                     [a_VARIABLE_INDEX]) {
                m_cardinality_constraints[constraint_index]
                    .on_variable_assigned(true);
            }

            for (int constraint_index :
                 m_variable_to_cardinality_negative_constraints
                     [a_VARIABLE_INDEX]) {
                auto& cardinality_constraint =
                    m_cardinality_constraints[constraint_index];
                cardinality_constraint.on_variable_assigned(false);
                if (!cardinality_constraint.is_satisfied() &&
                    !m_in_cardinality_propagation_queue[constraint_index]) {
                    m_cardinality_propagation_queue.push_back(constraint_index);
                    m_in_cardinality_propagation_queue[constraint_index] = 1;
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
                 m_variable_to_cardinality_positive_constraints
                     [a_VARIABLE_INDEX]) {
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
                 m_variable_to_cardinality_negative_constraints
                     [a_VARIABLE_INDEX]) {
                m_cardinality_constraints[constraint_index]
                    .on_variable_assigned(true);
            }
        }
    }

    /*************************************************************************/
    inline void rollback(const int a_VARIABLE_INDEX) {
        auto&     variable = m_variables[a_VARIABLE_INDEX];
        const int VALUE    = m_values[a_VARIABLE_INDEX];

        if (VALUE == 1) {
            for (const auto& [constraint_index, coefficient] :
                 m_variable_to_pseudo_boolean_positive_constraints
                     [a_VARIABLE_INDEX]) {
                m_pseudo_boolean_constraints[constraint_index]
                    .on_variable_removed(true, coefficient);
            }

            for (const auto& [constraint_index, coefficient] :
                 m_variable_to_pseudo_boolean_negative_constraints
                     [a_VARIABLE_INDEX]) {
                m_pseudo_boolean_constraints[constraint_index]
                    .on_variable_removed(false, coefficient);
            }

            for (int constraint_index :
                 m_variable_to_cardinality_positive_constraints
                     [a_VARIABLE_INDEX]) {
                m_cardinality_constraints[constraint_index].on_variable_removed(
                    true);
            }

            for (int constraint_index :
                 m_variable_to_cardinality_negative_constraints
                     [a_VARIABLE_INDEX]) {
                m_cardinality_constraints[constraint_index].on_variable_removed(
                    false);
            }
        } else {
            for (const auto& [constraint_index, coefficient] :
                 m_variable_to_pseudo_boolean_positive_constraints
                     [a_VARIABLE_INDEX]) {
                m_pseudo_boolean_constraints[constraint_index]
                    .on_variable_removed(false, coefficient);
            }

            for (const auto& [constraint_index, coefficient] :
                 m_variable_to_pseudo_boolean_negative_constraints
                     [a_VARIABLE_INDEX]) {
                m_pseudo_boolean_constraints[constraint_index]
                    .on_variable_removed(true, coefficient);
            }

            for (int constraint_index :
                 m_variable_to_cardinality_positive_constraints
                     [a_VARIABLE_INDEX]) {
                m_cardinality_constraints[constraint_index].on_variable_removed(
                    false);
            }

            for (int constraint_index :
                 m_variable_to_cardinality_negative_constraints
                     [a_VARIABLE_INDEX]) {
                m_cardinality_constraints[constraint_index].on_variable_removed(
                    true);
            }
        }

        m_values[a_VARIABLE_INDEX] = -1;
        variable.remove();
    }

    /*************************************************************************/
    template <typename T_AssignCallback>
    inline CDCLConstraintReference propagate(
        const std::vector<int>& a_TRAIL, int& a_propagation_queue_head,
        const T_AssignCallback& a_ASSIGN_CALLBACK) {
        CDCLConstraintReference null_reference{CDCLConstraintType::NONE,
                                               CDCLCoreConstant::NO_CONSTRAINT};

        while (a_propagation_queue_head < static_cast<int>(a_TRAIL.size()) ||
               !m_cardinality_propagation_queue.empty() ||
               !m_pseudo_boolean_propagation_queue.empty()) {
            if (a_propagation_queue_head < static_cast<int>(a_TRAIL.size())) {
                const int VARIABLE_INDEX = a_TRAIL[a_propagation_queue_head++];
                const int VARIABLE_VALUE = m_values[VARIABLE_INDEX];

                // Two-Watched Literals: literal falsified by this assignment
                const int FALSIFIED_LITERAL =
                    make_literal(VARIABLE_INDEX, VARIABLE_VALUE);
                auto&        watch_list = m_watches[FALSIFIED_LITERAL];
                size_t       i          = 0;
                size_t       j          = 0;
                const size_t WATCH_SIZE = watch_list.size();

                while (i < WATCH_SIZE) {
                    const auto WATCHER          = watch_list[i];
                    const int  CONSTRAINT_INDEX = WATCHER.constraint_index;
                    auto&      clause           = m_clauses[CONSTRAINT_INDEX];
                    if (clause.is_deleted) {
                        i++;
                        continue;
                    }

                    const int BLOCKER = WATCHER.blocker_literal;
                    if (this->is_literal_satisfied(BLOCKER)) {
                        watch_list[j++] = WATCHER;
                        i++;
                        continue;
                    }

                    if (clause.literals[0] == FALSIFIED_LITERAL) {
                        std::swap(clause.literals[0], clause.literals[1]);
                    }
                    const int LITERAL_0 = clause.literals[0];
                    const int LITERAL_1 = clause.literals[1];

                    if (this->is_literal_satisfied(LITERAL_0)) {
                        watch_list[j++] =
                            CDCLWatcher{CONSTRAINT_INDEX, LITERAL_0};
                        i++;
                        continue;
                    }

                    bool      found_new_watch = false;
                    const int CLAUSE_SIZE =
                        static_cast<int>(clause.literals.size());
                    for (int k = 2; k < CLAUSE_SIZE; k++) {
                        const int CANDIDATE_LITERAL = clause.literals[k];
                        if (!this->is_literal_falsified(CANDIDATE_LITERAL)) {
                            clause.literals[1] = CANDIDATE_LITERAL;
                            clause.literals[k] = LITERAL_1;
                            m_watches[CANDIDATE_LITERAL].push_back(
                                CDCLWatcher{CONSTRAINT_INDEX, LITERAL_0});
                            found_new_watch = true;
                            break;
                        }
                    }

                    if (found_new_watch) {
                        i++;
                        continue;
                    }

                    watch_list[j++] = CDCLWatcher{CONSTRAINT_INDEX, LITERAL_0};
                    i++;

                    if (this->is_literal_falsified(LITERAL_0)) {
                        while (i < WATCH_SIZE) {
                            watch_list[j++] = watch_list[i++];
                        }
                        watch_list.resize(j);
                        return CDCLConstraintReference{
                            CDCLConstraintType::LEARNED, CONSTRAINT_INDEX};
                    } else {
                        const int FORCED_VALUE =
                            (literal_sign(LITERAL_0) == 0 ? 1 : 0);
                        a_ASSIGN_CALLBACK(
                            literal_variable(LITERAL_0), FORCED_VALUE,
                            CDCLConstraintReference{CDCLConstraintType::LEARNED,
                                                    CONSTRAINT_INDEX});
                    }
                }
                watch_list.resize(j);
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

                for (const auto& [variable_index, coefficient] :
                     PSEUDO_BOOLEAN_CONSTRAINT.positive_terms) {
                    if (!m_variables[variable_index].is_assigned() &&
                        SLACK < coefficient) {
                        a_ASSIGN_CALLBACK(variable_index, 1, reference);
                    }
                }
                for (const auto& [variable_index, coefficient] :
                     PSEUDO_BOOLEAN_CONSTRAINT.negative_terms) {
                    if (!m_variables[variable_index].is_assigned() &&
                        SLACK < coefficient) {
                        a_ASSIGN_CALLBACK(variable_index, 0, reference);
                    }
                }
            }
        }

        return null_reference;
    }

    /*************************************************************************/
    template <typename T_AssignCallback>
    inline CDCLConstraintReference propagate(
        const T_AssignCallback& a_ASSIGN_CALLBACK) {
        static const std::vector<int> EMPTY_TRAIL;
        int                           dummy_propagation_queue_head = 0;
        return this->propagate(EMPTY_TRAIL, dummy_propagation_queue_head,
                               a_ASSIGN_CALLBACK);
    }

    /*************************************************************************/
    inline CDCLConstraintReference add_learned_constraint(
        CardinalityConstraint&& a_CONSTRAINT) {
        const int LEARNED_CONSTRAINT_INDEX = static_cast<int>(m_clauses.size());

        std::vector<int> literals;
        literals.reserve(a_CONSTRAINT.positive_variable_indices.size() +
                         a_CONSTRAINT.negative_variable_indices.size());

        for (const int VARIABLE_INDEX :
             a_CONSTRAINT.positive_variable_indices) {
            literals.push_back(make_literal(VARIABLE_INDEX, 0));
        }
        for (const int VARIABLE_INDEX :
             a_CONSTRAINT.negative_variable_indices) {
            literals.push_back(make_literal(VARIABLE_INDEX, 1));
        }

        const int CLAUSE_LENGTH = static_cast<int>(literals.size());
        if (CLAUSE_LENGTH >= 2) {
            int max_index_0 = 0;
            int max_level_0 = -1;
            for (int k = 0; k < CLAUSE_LENGTH; k++) {
                const auto& variable =
                    m_variables[literal_variable(literals[k])];
                const int LEVEL = variable.is_assigned()
                                      ? variable.decision_level
                                      : 1000000000;
                if (LEVEL > max_level_0) {
                    max_level_0 = LEVEL;
                    max_index_0 = k;
                }
            }
            std::swap(literals[0], literals[max_index_0]);

            int max_index_1 = 1;
            int max_level_1 = -1;
            for (int k = 1; k < CLAUSE_LENGTH; k++) {
                const auto& variable =
                    m_variables[literal_variable(literals[k])];
                const int LEVEL = variable.is_assigned()
                                      ? variable.decision_level
                                      : 1000000000;
                if (LEVEL > max_level_1) {
                    max_level_1 = LEVEL;
                    max_index_1 = k;
                }
            }
            std::swap(literals[1], literals[max_index_1]);

            m_watches[literals[0]].push_back(
                CDCLWatcher{LEARNED_CONSTRAINT_INDEX, literals[1]});
            m_watches[literals[1]].push_back(
                CDCLWatcher{LEARNED_CONSTRAINT_INDEX, literals[0]});
        }

        m_clauses.emplace_back(
            LEARNED_CONSTRAINT_INDEX, a_CONSTRAINT.literal_block_distance, true,
            std::move(literals),
            std::move(a_CONSTRAINT.positive_variable_indices),
            std::move(a_CONSTRAINT.negative_variable_indices));

        return CDCLConstraintReference{CDCLConstraintType::LEARNED,
                                       LEARNED_CONSTRAINT_INDEX};
    }

    /*************************************************************************/
    inline int reduce_learned_constraints(
        const double a_REDUCTION_RATE                              = 0.5,
        const int    a_LITERAL_BLOCK_DISTANCE_PROTECTION_THRESHOLD = 2) {
        const int NUMBER_OF_LEARNED_CONSTRAINTS =
            static_cast<int>(m_clauses.size());
        if (NUMBER_OF_LEARNED_CONSTRAINTS == 0) {
            return 0;
        }

        const int TOTAL_CLAUSES = NUMBER_OF_LEARNED_CONSTRAINTS;

        std::vector<uint8_t> is_locked(TOTAL_CLAUSES, 0);

        for (const auto& variable : m_variables) {
            if (variable.is_assigned() && variable.reason_constraint.type ==
                                              CDCLConstraintType::LEARNED) {
                const int INDEX = variable.reason_constraint.index;
                if (INDEX >= 0 && INDEX < TOTAL_CLAUSES) {
                    is_locked[INDEX] = 1;
                }
            }
        }

        std::vector<std::pair<int, int>> candidates;
        for (int i = 0; i < TOTAL_CLAUSES; i++) {
            if (!is_locked[i] && !m_clauses[i].is_deleted &&
                m_clauses[i].literal_block_distance >
                    a_LITERAL_BLOCK_DISTANCE_PROTECTION_THRESHOLD) {
                candidates.push_back({m_clauses[i].literal_block_distance, i});
            }
        }
        std::sort(candidates.rbegin(), candidates.rend());
        const int NUMBER_OF_CONSTRAINTS_TO_REMOVE =
            static_cast<int>(candidates.size() * a_REDUCTION_RATE);

        for (int i = 0; i < NUMBER_OF_CONSTRAINTS_TO_REMOVE; i++) {
            m_clauses[candidates[i].second].is_deleted = true;
        }

        std::vector<CDCLClause> new_clauses;
        std::vector<int>        old_to_new(TOTAL_CLAUSES, -1);
        new_clauses.reserve(TOTAL_CLAUSES - NUMBER_OF_CONSTRAINTS_TO_REMOVE);

        for (int i = 0; i < TOTAL_CLAUSES; i++) {
            if (!m_clauses[i].is_deleted) {
                old_to_new[i] = static_cast<int>(new_clauses.size());
                new_clauses.push_back(std::move(m_clauses[i]));
                new_clauses.back().index = old_to_new[i];
            }
        }
        m_clauses = std::move(new_clauses);

        for (auto& variable : m_variables) {
            if (variable.reason_constraint.type ==
                CDCLConstraintType::LEARNED) {
                const int OLD_INDEX = variable.reason_constraint.index;
                if (OLD_INDEX >= 0 && OLD_INDEX < TOTAL_CLAUSES) {
                    const int NEW_INDEX = old_to_new[OLD_INDEX];
                    if (NEW_INDEX >= 0) {
                        variable.reason_constraint.index = NEW_INDEX;
                    } else {
                        variable.reason_constraint.type =
                            CDCLConstraintType::NONE;
                        variable.reason_constraint.index =
                            CDCLCoreConstant::NO_CONSTRAINT;
                    }
                }
            }
        }

        for (auto& watch_list : m_watches) {
            watch_list.clear();
        }
        const int NEW_COUNT = static_cast<int>(m_clauses.size());
        for (int i = 0; i < NEW_COUNT; i++) {
            auto&     c           = m_clauses[i];
            const int CLAUSE_SIZE = static_cast<int>(c.literals.size());
            if (CLAUSE_SIZE >= 2) {
                if (this->is_literal_falsified(c.literals[0])) {
                    for (int k = 1; k < CLAUSE_SIZE; k++) {
                        if (!this->is_literal_falsified(c.literals[k])) {
                            std::swap(c.literals[0], c.literals[k]);
                            break;
                        }
                    }
                }
                if (this->is_literal_falsified(c.literals[1])) {
                    for (int k = 2; k < CLAUSE_SIZE; k++) {
                        if (!this->is_literal_falsified(c.literals[k])) {
                            std::swap(c.literals[1], c.literals[k]);
                            break;
                        }
                    }
                }
                m_watches[c.literals[0]].push_back(
                    CDCLWatcher{i, c.literals[1]});
                m_watches[c.literals[1]].push_back(
                    CDCLWatcher{i, c.literals[0]});
            }
        }

        return NUMBER_OF_CONSTRAINTS_TO_REMOVE;
    }

    /*************************************************************************/
    inline void clear_propagation_queue(void) {
        m_pseudo_boolean_propagation_queue.clear();
        m_in_pseudo_boolean_propagation_queue.assign(
            this->number_of_pseudo_boolean_constraints(), false);
        m_cardinality_propagation_queue.clear();
        m_in_cardinality_propagation_queue.assign(
            this->number_of_cardinality_constraints(), false);
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
            return m_clauses[a_REFERENCE.index].positive_variable_indices;
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
            return m_clauses[a_REFERENCE.index].negative_variable_indices;
        }
        static const std::vector<int> EMPTY_INDICES;
        return EMPTY_INDICES;
    }

    /*************************************************************************/
    inline const CDCLClause& clause(const int a_INDEX) const noexcept {
        return m_clauses[a_INDEX];
    }

    /*****************************************************************************/
    inline const CDCLLearnedClause& learned_clause(
        const int a_INDEX) const noexcept {
        return m_clauses[a_INDEX];
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
    inline CDCLVariable& mutable_variable(const int a_INDEX) noexcept {
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
    inline int number_of_clauses(void) const noexcept {
        return static_cast<int>(m_clauses.size());
    }

    /*****************************************************************************/
    inline int number_of_initial_clauses(void) const noexcept {
        return 0;
    }

    /*****************************************************************************/
    inline int number_of_learned_clauses(void) const noexcept {
        return static_cast<int>(m_clauses.size());
    }

    /*****************************************************************************/
    inline int number_of_pseudo_boolean_constraints(void) const noexcept {
        return static_cast<int>(m_pseudo_boolean_constraints.size());
    }

    /*************************************************************************/
    inline int number_of_cardinality_constraints(void) const noexcept {
        return static_cast<int>(m_cardinality_constraints.size());
    }

    /*************************************************************************/
    inline int number_of_learned_constraints(void) const noexcept {
        return this->number_of_learned_clauses();
    }

    /*************************************************************************/
    inline int number_of_constraints(void) const noexcept {
        return this->number_of_pseudo_boolean_constraints() +
               this->number_of_cardinality_constraints() +
               this->number_of_clauses();
    }

    /*************************************************************************/
    inline bool is_selection_variable(const int a_INDEX) const noexcept {
        if (m_instance_ptr == nullptr ||
            !m_instance_ptr->variable_index_map.contains_second(a_INDEX)) {
            return false;
        }
        return (m_instance_ptr->variable_index_map.reverse_at(a_INDEX)
                    ->selection_ptr() != nullptr);
    }

    /*************************************************************************/
    inline const std::vector<std::vector<int>>& selection_variable_indices(
        void) const noexcept {
        return m_selection_variable_indices;
    }

    /*************************************************************************/
    inline std::vector<int> extract_current_solution(void) const {
        const int NUMBER_OF_VARIABLES = static_cast<int>(m_variables.size());
        std::vector<int> solution(NUMBER_OF_VARIABLES);
        for (int i = 0; i < NUMBER_OF_VARIABLES; i++) {
            solution[i] = m_variables[i].is_assigned() ? m_variables[i].value
                                                       : m_variables[i].phase;
        }

        // Strictly enforce that each Selection constraint has exactly one
        // variable = 1
        const bool HAS_INITIAL_ACTIVITIES = !m_initial_activities.empty();
        for (const auto& selection_variable_indices :
             m_selection_variable_indices) {
            if (selection_variable_indices.empty()) {
                continue;
            }
            int count = 0;
            for (int variable_index : selection_variable_indices) {
                if (solution[variable_index] == 1) {
                    count++;
                }
            }
            if (count == 1) {
                // Exactly one variable is 1, perfect!
                continue;
            }

            int best_variable_index = -1;
            if (count > 1) {
                // Multiple variables are 1: pick the best one among those
                // currently 1
                for (int variable_index : selection_variable_indices) {
                    if (solution[variable_index] != 1) {
                        continue;
                    }
                    if (best_variable_index == -1) {
                        best_variable_index = variable_index;
                        continue;
                    }
                    const auto& current_variable = m_variables[variable_index];
                    const auto& best_variable =
                        m_variables[best_variable_index];
                    if (current_variable.is_assigned() &&
                        !best_variable.is_assigned()) {
                        best_variable_index = variable_index;
                    } else if (current_variable.is_assigned() &&
                               best_variable.is_assigned()) {
                        if (current_variable.decision_level == 0 &&
                            best_variable.decision_level > 0) {
                            best_variable_index = variable_index;
                        } else if (best_variable.decision_level != 0 &&
                                   current_variable.decision_level >
                                       best_variable.decision_level) {
                            best_variable_index = variable_index;
                        } else if (current_variable.decision_level ==
                                       best_variable.decision_level &&
                                   HAS_INITIAL_ACTIVITIES &&
                                   m_initial_activities[variable_index] >
                                       m_initial_activities
                                           [best_variable_index]) {
                            best_variable_index = variable_index;
                        }
                    } else if (!current_variable.is_assigned() &&
                               !best_variable.is_assigned()) {
                        if (HAS_INITIAL_ACTIVITIES &&
                            m_initial_activities[variable_index] >
                                m_initial_activities[best_variable_index]) {
                            best_variable_index = variable_index;
                        }
                    }
                }
            } else {
                // count == 0: no variable is 1 in this selection!
                // Pick the best variable in selection_variable_indices to set
                // to 1
                for (int variable_index : selection_variable_indices) {
                    if (best_variable_index == -1) {
                        best_variable_index = variable_index;
                        continue;
                    }
                    if (HAS_INITIAL_ACTIVITIES &&
                        m_initial_activities[variable_index] >
                            m_initial_activities[best_variable_index]) {
                        best_variable_index = variable_index;
                    }
                }
            }

            for (int variable_index : selection_variable_indices) {
                solution[variable_index] =
                    (variable_index == best_variable_index) ? 1 : 0;
            }
        }

        return solution;
    }

    /*************************************************************************/
    inline double compute_total_violation(
        const std::vector<int>& a_SOLUTION) const {
        if (m_instance_ptr == nullptr) {
            return 0.0;
        }
        const auto& MATRIX = m_instance_ptr->constraint_coefficients;
        const auto& LOWER_BOUNDS =
            m_instance_ptr->constraint_lower_bounds.values;
        const auto& UPPER_BOUNDS =
            m_instance_ptr->constraint_upper_bounds.values;
        const int NUMBER_OF_CONSTRAINTS = m_instance_ptr->number_of_constraints;

        double total_violation = 0.0;
        for (int i = 0; i < NUMBER_OF_CONSTRAINTS; i++) {
            const int PTR_BEGIN = MATRIX.row_ptrs[i];
            const int PTR_END   = MATRIX.row_ptrs[i + 1];

            double lhs = 0.0;
            for (int j = PTR_BEGIN; j < PTR_END; j++) {
                if (a_SOLUTION[MATRIX.column_indices[j]]) {
                    lhs += MATRIX.values[j];
                }
            }

            const double LOWER_BOUND = LOWER_BOUNDS[i];
            const double UPPER_BOUND = UPPER_BOUNDS[i];

            if (std::isfinite(LOWER_BOUND) && lhs < LOWER_BOUND) {
                total_violation += (LOWER_BOUND - lhs);
            }
            if (std::isfinite(UPPER_BOUND) && lhs > UPPER_BOUND) {
                total_violation += (lhs - UPPER_BOUND);
            }
        }
        return total_violation;
    }
};

}  // namespace printemps::solver::cdcl::core

#endif
/*****************************************************************************/
// END
/*****************************************************************************/
