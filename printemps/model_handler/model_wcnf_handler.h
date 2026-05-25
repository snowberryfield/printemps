/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
// Modified by Antigravity (2026)
/*****************************************************************************/
#ifndef PRINTEMPS_MODEL_HANDLER_MODEL_WCNF_HANDLER_H__
#define PRINTEMPS_MODEL_HANDLER_MODEL_WCNF_HANDLER_H__

#include "../wcnf/wcnf.h"

namespace printemps::model_handler {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class ModelWCNFHandler {
   private:
    model::Model<T_Variable, T_Expression> *m_model_ptr;

   public:
    /*************************************************************************/
    ModelWCNFHandler(void) {
        this->initialize();
    }

    /*************************************************************************/
    ModelWCNFHandler(model::Model<T_Variable, T_Expression> *a_model_ptr) {
        this->initialize();
        this->setup(a_model_ptr);
    }

    /*************************************************************************/
    inline void initialize(void) noexcept {
        m_model_ptr = nullptr;
    }

    /*************************************************************************/
    inline void setup(
        model::Model<T_Variable, T_Expression> *a_model_ptr) noexcept {
        m_model_ptr = a_model_ptr;
    }

    /*********************************************************************/
    void import(const wcnf::WCNF &a_WCNF) {
        auto &model = *m_model_ptr;

        using VariableMap = std::unordered_map<
            int, model_component::Variable<T_Variable, T_Expression> *>;
        using Sensitivities = std::unordered_map<
            model_component::Variable<T_Variable, T_Expression> *,
            T_Expression>;

        model.set_is_minimization(true);

        /**
         * Set up Boolean variables x_1, ..., x_n.
         */
        const int NUMBER_OF_VARIABLES = a_WCNF.number_of_variables;
        auto     &variable_proxy =
            model.component_creator().create_variables("variables", NUMBER_OF_VARIABLES);

        VariableMap variable_ptrs;
        for (auto i = 0; i < NUMBER_OF_VARIABLES; i++) {
            variable_proxy(i).set_bound(0, 1);
            variable_proxy(i).set_name("x_" + std::to_string(i + 1));
            variable_ptrs[i + 1] = &variable_proxy(i);
        }

        /**
         * Set up hard clauses. A clause (l_1 v ... v l_k) becomes a linear
         * inequality
         *
         *     sum_{positive lit} x_i  -  sum_{negative lit} x_j  >=  1 - |N|
         *
         * where |N| is the number of negative literals in the clause.
         */
        const int NUMBER_OF_HARD_CLAUSES = a_WCNF.hard_clauses.size();
        if (NUMBER_OF_HARD_CLAUSES > 0) {
            auto &hard_clause_proxy = model.component_creator().create_constraints(
                "hard_clauses", NUMBER_OF_HARD_CLAUSES);

#ifdef _OPENMP
#pragma omp parallel for schedule(static)
#endif
            for (auto i = 0; i < NUMBER_OF_HARD_CLAUSES; i++) {
                const auto &CLAUSE = a_WCNF.hard_clauses[i];
                auto        expression =
                    model_component::Expression<T_Variable,
                                                T_Expression>::create_instance();

                Sensitivities expression_sensitivities;
                int           number_of_negative_literals = 0;
                for (const auto literal : CLAUSE.literals) {
                    const int VAR_INDEX = std::abs(literal);
                    auto      *VAR_PTR  = variable_ptrs.at(VAR_INDEX);
                    if (literal > 0) {
                        expression_sensitivities[VAR_PTR] +=
                            static_cast<T_Expression>(1);
                    } else {
                        expression_sensitivities[VAR_PTR] +=
                            static_cast<T_Expression>(-1);
                        number_of_negative_literals++;
                    }
                }
                expression.set_sensitivities(expression_sensitivities);
                const T_Expression RHS =
                    static_cast<T_Expression>(1 - number_of_negative_literals);
                hard_clause_proxy(i) = (expression >= RHS);
                hard_clause_proxy(i).set_name(CLAUSE.name);
            }
        }

        /**
         * Set up soft clauses. Each soft clause introduces a binary slack s
         * with relaxed inequality
         *
         *     sum_{positive} x_i - sum_{negative} x_j + s  >=  1 - |N|
         *
         * When all literals are falsified the LHS without slack equals -|N|,
         * forcing s = 1; the cost weight * s is then added to the objective
         * penalty.
         */
        const int NUMBER_OF_SOFT_CLAUSES = a_WCNF.soft_clauses.size();
        auto     &soft_slack_proxy =
            model.component_creator().create_variables("soft_slacks", NUMBER_OF_SOFT_CLAUSES);
        for (auto i = 0; i < NUMBER_OF_SOFT_CLAUSES; i++) {
            soft_slack_proxy(i).set_bound(0, 1);
            soft_slack_proxy(i).set_name("s_" + std::to_string(i + 1));
        }

        auto objective_penalty =
            model_component::Expression<T_Variable,
                                        T_Expression>::create_instance();

        if (NUMBER_OF_SOFT_CLAUSES > 0) {
            auto &soft_clause_proxy = model.component_creator().create_constraints(
                "soft_clauses", NUMBER_OF_SOFT_CLAUSES);

#ifdef _OPENMP
#pragma omp parallel for schedule(static)
#endif
            for (auto i = 0; i < NUMBER_OF_SOFT_CLAUSES; i++) {
                const auto &CLAUSE = a_WCNF.soft_clauses[i];
                auto        expression =
                    model_component::Expression<T_Variable,
                                                T_Expression>::create_instance();

                Sensitivities expression_sensitivities;
                int           number_of_negative_literals = 0;
                for (const auto literal : CLAUSE.literals) {
                    const int VAR_INDEX = std::abs(literal);
                    auto      *VAR_PTR  = variable_ptrs.at(VAR_INDEX);
                    if (literal > 0) {
                        expression_sensitivities[VAR_PTR] +=
                            static_cast<T_Expression>(1);
                    } else {
                        expression_sensitivities[VAR_PTR] +=
                            static_cast<T_Expression>(-1);
                        number_of_negative_literals++;
                    }
                }
                expression_sensitivities[&soft_slack_proxy(i)] =
                    static_cast<T_Expression>(1);
                expression.set_sensitivities(expression_sensitivities);
                const T_Expression RHS =
                    static_cast<T_Expression>(1 - number_of_negative_literals);
                soft_clause_proxy(i) = (expression >= RHS);
                soft_clause_proxy(i).set_name(CLAUSE.name);
            }

            for (auto i = 0; i < NUMBER_OF_SOFT_CLAUSES; i++) {
                const auto &CLAUSE = a_WCNF.soft_clauses[i];
                /**
                 * Cast uint64_t weight to T_Expression (typically double).
                 * Precision degrades when the weight sum exceeds 2^53.
                 */
                objective_penalty +=
                    static_cast<T_Expression>(CLAUSE.weight) *
                    soft_slack_proxy(i);
            }
        }

        /**
         * Pure MaxSAT objective: minimize the soft-clause penalty.
         */
        auto objective =
            model_component::Expression<T_Variable,
                                        T_Expression>::create_instance();
        objective += objective_penalty;
        model.minimize(objective);
    }
};
}  // namespace printemps::model_handler
#endif
/*****************************************************************************/
// END
/*****************************************************************************/
