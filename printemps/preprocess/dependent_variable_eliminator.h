/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_PREPROCESS_DEPENDENT_VARIABLE_ELIMINATOR_H__
#define PRINTEMPS_PREPROCESS_DEPENDENT_VARIABLE_ELIMINATOR_H__

namespace printemps::preprocess {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class DependentVariableEliminator {
   private:
    model::Model<T_Variable, T_Expression> *m_model_ptr;

   public:
    /*************************************************************************/
    DependentVariableEliminator(void) {
        this->initialize();
    }

    /*************************************************************************/
    DependentVariableEliminator(
        model::Model<T_Variable, T_Expression> *a_model_ptr) {
        this->setup(a_model_ptr);
    }

    /*************************************************************************/
    inline void setup(model::Model<T_Variable, T_Expression> *a_model_ptr) {
        this->initialize();
        this->m_model_ptr = a_model_ptr;
    }

    /*************************************************************************/
    inline void initialize(void) {
        m_model_ptr = nullptr;
    }

    /*************************************************************************/
    inline std::vector<model_component::Variable<T_Variable, T_Expression> *>
    collect_dependent_variable_ptrs(void) {
        std::vector<model_component::Variable<T_Variable, T_Expression> *>
            dependent_variable_ptrs;

        auto &reference = m_model_ptr->reference();
        auto &dependent_binary_ptrs =
            reference.variable_type.dependent_binary_variable_ptrs;
        auto &dependent_integer_ptrs =
            reference.variable_type.dependent_integer_variable_ptrs;

        dependent_variable_ptrs.insert(dependent_variable_ptrs.end(),
                                       dependent_binary_ptrs.begin(),
                                       dependent_binary_ptrs.end());

        dependent_variable_ptrs.insert(dependent_variable_ptrs.end(),
                                       dependent_integer_ptrs.begin(),
                                       dependent_integer_ptrs.end());

        return dependent_variable_ptrs;
    }

    /*************************************************************************/
    inline int eliminate_dependent_variables_from_objective(
        const std::vector<model_component::Variable<T_Variable, T_Expression> *>
                  &a_DEPENDENT_VARIABLE_PTRS,
        const bool a_IS_ENABLED_PRINT) {
        int   number_of_newly_eliminated_dependent_variables = 0;
        auto &objective_sensitivities =
            m_model_ptr->objective().expression().sensitivities();

        for (const auto &variable_ptr : a_DEPENDENT_VARIABLE_PTRS) {
            if (objective_sensitivities.find(variable_ptr) !=
                objective_sensitivities.end()) {
                m_model_ptr->objective().expression().substitute(
                    variable_ptr,  //
                    *(variable_ptr->dependent_expression_ptr()));

                number_of_newly_eliminated_dependent_variables++;

                utility::print_message(
                    "The dependent variable " + variable_ptr->name() +
                        " in the objective function was eliminated. ",
                    a_IS_ENABLED_PRINT);
            }
        }
        return number_of_newly_eliminated_dependent_variables;
    }

    /*************************************************************************/
    inline int eliminate_dependent_variables_from_constraints(
        const std::vector<model_component::Variable<T_Variable, T_Expression> *>
                  &a_DEPENDENT_VARIABLE_PTRS,
        const bool a_IS_ENABLED_PRINT) {
        int number_of_newly_eliminated_dependent_variables = 0;

        for (const auto &variable_ptr : a_DEPENDENT_VARIABLE_PTRS) {
            for (auto &&constraint_ptr :
                 variable_ptr->related_constraint_ptrs()) {
                constraint_ptr->expression().substitute(
                    variable_ptr,  //
                    *(variable_ptr->dependent_expression_ptr()));

                number_of_newly_eliminated_dependent_variables++;

                utility::print_message(
                    "The dependent variable " + variable_ptr->name() +
                        " in the constraint " + constraint_ptr->name() +
                        " was eliminated. ",
                    a_IS_ENABLED_PRINT);
            }
        }
        return number_of_newly_eliminated_dependent_variables;
    }

    /*************************************************************************/
    inline int eliminate_dependent_variables_from_expressions(
        const std::vector<model_component::Variable<T_Variable, T_Expression> *>
                  &a_DEPENDENT_VARIABLE_PTRS,
        const bool a_IS_ENABLED_PRINT) {
        int number_of_newly_eliminated_dependent_variables = 0;
        std::unordered_map<
            model_component::Variable<T_Variable, T_Expression> *,
            std::vector<
                model_component::Expression<T_Variable, T_Expression> *>>
            variable_to_expression_ptrs;

        for (auto &&proxy : m_model_ptr->expression_proxies()) {
            for (auto &&expression : proxy.flat_indexed_expressions()) {
                for (auto &&sensitivity : expression.sensitivities()) {
                    variable_to_expression_ptrs[sensitivity.first].push_back(
                        &expression);
                }
            }
        }

        for (auto &&variable_ptr : a_DEPENDENT_VARIABLE_PTRS) {
            auto it = variable_to_expression_ptrs.find(variable_ptr);
            if (it == variable_to_expression_ptrs.end())
                continue;

            for (auto &expression_ptr : it->second) {
                expression_ptr->substitute(
                    variable_ptr, *variable_ptr->dependent_expression_ptr());
                number_of_newly_eliminated_dependent_variables++;

                utility::print_message(
                    "The dependent variable " + variable_ptr->name() +
                        " in the expression " + expression_ptr->name() +
                        " was eliminated. ",
                    a_IS_ENABLED_PRINT);
            }
        }

        return number_of_newly_eliminated_dependent_variables;
    }

    /*************************************************************************/
    inline int eliminate(const bool a_IS_ENABLED_PRINT) {
        utility::print_single_line(a_IS_ENABLED_PRINT);
        utility::print_message("Eliminating dependent variables...",
                               a_IS_ENABLED_PRINT);

        int  number_of_newly_eliminated_dependent_variables = 0;
        auto dependent_variable_ptrs = this->collect_dependent_variable_ptrs();

        number_of_newly_eliminated_dependent_variables +=
            this->eliminate_dependent_variables_from_objective(
                dependent_variable_ptrs, a_IS_ENABLED_PRINT);

        number_of_newly_eliminated_dependent_variables +=
            this->eliminate_dependent_variables_from_constraints(
                dependent_variable_ptrs, a_IS_ENABLED_PRINT);

        number_of_newly_eliminated_dependent_variables +=
            this->eliminate_dependent_variables_from_expressions(
                dependent_variable_ptrs, a_IS_ENABLED_PRINT);

        utility::print_message("Done.", a_IS_ENABLED_PRINT);
        return number_of_newly_eliminated_dependent_variables;
    }
};
}  // namespace printemps::preprocess
#endif
/*****************************************************************************/
// END
/*****************************************************************************/