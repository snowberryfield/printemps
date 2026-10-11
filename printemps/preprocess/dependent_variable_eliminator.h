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
    inline void flatten_dependent_expressions(
        const std::vector<model_component::Variable<T_Variable, T_Expression> *>
                  &a_DEPENDENT_VARIABLE_PTRS,
        const bool a_IS_ENABLED_PRINT) {
        if (a_DEPENDENT_VARIABLE_PTRS.empty()) {
            return;
        }

        utility::graph::DirectedGraph<
            model_component::Variable<T_Variable, T_Expression> *>
            graph(a_DEPENDENT_VARIABLE_PTRS);

        for (auto *variable_ptr_target : a_DEPENDENT_VARIABLE_PTRS) {
            auto *expression_ptr =
                variable_ptr_target->dependent_expression_ptr();
            if (expression_ptr == nullptr) {
                continue;
            }
            for (auto &&sensitivity : expression_ptr->sensitivities()) {
                auto *variable_ptr_source = sensitivity.first;
                if (graph.has_node(variable_ptr_source)) {
                    graph.add_edge(variable_ptr_source, variable_ptr_target);
                }
            }
        }
        const auto SORTED_VARIABLE_PTRS = graph.topological_sort();
        for (auto *variable_ptr_source : SORTED_VARIABLE_PTRS) {
            auto *expression_ptr =
                variable_ptr_source->dependent_expression_ptr();
            if (expression_ptr == nullptr) {
                continue;
            }

            for (auto *variable_ptr_target :
                 graph.successors(variable_ptr_source)) {
                auto *expression_ptr_target =
                    variable_ptr_target->dependent_expression_ptr();
                if (expression_ptr_target == nullptr) {
                    continue;
                }
                if (expression_ptr_target->sensitivities().find(
                        variable_ptr_source) !=
                    expression_ptr_target->sensitivities().end()) {
                    expression_ptr_target->substitute(variable_ptr_source,
                                                      *expression_ptr);
                    utility::print_message(
                        "The dependent variable " +
                            variable_ptr_source->name() +
                            " in the dependent expression of " +
                            variable_ptr_target->name() + " was eliminated. ",
                        a_IS_ENABLED_PRINT);
                }
            }
        }
    }

    /*************************************************************************/
    inline int eliminate_dependent_variables_from_objective(
        const std::vector<model_component::Variable<T_Variable, T_Expression> *>
                  &a_DEPENDENT_VARIABLE_PTRS,
        const bool a_IS_ENABLED_PRINT) {
        if (a_DEPENDENT_VARIABLE_PTRS.empty()) {
            return 0;
        }

        int   number_of_eliminated_dependent_variables = 0;
        auto &objective_expression = m_model_ptr->objective().expression();

        for (const auto &variable_ptr : a_DEPENDENT_VARIABLE_PTRS) {
            if (objective_expression.sensitivities().find(variable_ptr) !=
                objective_expression.sensitivities().end()) {
                objective_expression.substitute(
                    variable_ptr, *(variable_ptr->dependent_expression_ptr()));
                number_of_eliminated_dependent_variables++;

                utility::print_message(
                    "The dependent variable " + variable_ptr->name() +
                        " in the objective function was eliminated. ",
                    a_IS_ENABLED_PRINT);
            }
        }
        return number_of_eliminated_dependent_variables;
    }

    /*************************************************************************/
    inline int eliminate_dependent_variables_from_constraints(
        const std::vector<model_component::Variable<T_Variable, T_Expression> *>
                  &a_DEPENDENT_VARIABLE_PTRS,
        const bool a_IS_ENABLED_PRINT) {
        if (a_DEPENDENT_VARIABLE_PTRS.empty()) {
            return 0;
        }

        int number_of_eliminated_dependent_variables = 0;

        for (const auto &variable_ptr : a_DEPENDENT_VARIABLE_PTRS) {
            for (auto &&constraint_ptr :
                 variable_ptr->related_constraint_ptrs()) {
                auto &expression = constraint_ptr->expression();
                if (expression.sensitivities().find(variable_ptr) !=
                    expression.sensitivities().end()) {
                    expression.substitute(
                        variable_ptr,
                        *(variable_ptr->dependent_expression_ptr()));
                    number_of_eliminated_dependent_variables++;

                    utility::print_message(
                        "The dependent variable " + variable_ptr->name() +
                            " in the constraint " + constraint_ptr->name() +
                            " was eliminated. ",
                        a_IS_ENABLED_PRINT);
                }
            }
        }
        return number_of_eliminated_dependent_variables;
    }

    /*************************************************************************/
    inline int eliminate_dependent_variables_from_expressions(
        const std::vector<model_component::Variable<T_Variable, T_Expression> *>
                  &a_DEPENDENT_VARIABLE_PTRS,
        const bool a_IS_ENABLED_PRINT) {
        if (a_DEPENDENT_VARIABLE_PTRS.empty()) {
            return 0;
        }

        int number_of_eliminated_dependent_variables = 0;
        std::unordered_map<
            model_component::Variable<T_Variable, T_Expression> *,
            std::vector<
                model_component::Expression<T_Variable, T_Expression> *>>
            variable_to_expression_ptrs;

        std::unordered_set<model_component::Variable<T_Variable, T_Expression> *>
            dependent_variable_set(a_DEPENDENT_VARIABLE_PTRS.begin(),
                                   a_DEPENDENT_VARIABLE_PTRS.end());

        for (auto &&proxy : m_model_ptr->expression_proxies()) {
            for (auto &&expression : proxy.flat_indexed_expressions()) {
                for (auto &&sensitivity : expression.sensitivities()) {
                    if (dependent_variable_set.find(sensitivity.first) !=
                        dependent_variable_set.end()) {
                        variable_to_expression_ptrs[sensitivity.first].push_back(
                            &expression);
                    }
                }
            }
        }

        for (const auto &variable_ptr : a_DEPENDENT_VARIABLE_PTRS) {
            auto it = variable_to_expression_ptrs.find(variable_ptr);
            if (it == variable_to_expression_ptrs.end()) {
                continue;
            }

            for (auto *expression_ptr : it->second) {
                if (expression_ptr->sensitivities().find(variable_ptr) !=
                    expression_ptr->sensitivities().end()) {
                    expression_ptr->substitute(
                        variable_ptr,
                        *(variable_ptr->dependent_expression_ptr()));
                    number_of_eliminated_dependent_variables++;

                    utility::print_message(
                        "The dependent variable " + variable_ptr->name() +
                            " in the expression " + expression_ptr->name() +
                            " was eliminated. ",
                        a_IS_ENABLED_PRINT);
                }
            }
        }
        return number_of_eliminated_dependent_variables;
    }

    /*************************************************************************/
    inline int run(const bool a_IS_ENABLED_PRINT) {
        utility::print_single_line(a_IS_ENABLED_PRINT);
        utility::print_message("Eliminating dependent variables...",
                               a_IS_ENABLED_PRINT);

        int  number_of_eliminated_dependent_variables = 0;
        auto dependent_variable_ptrs = this->collect_dependent_variable_ptrs();

        this->flatten_dependent_expressions(dependent_variable_ptrs,
                                            a_IS_ENABLED_PRINT);

        number_of_eliminated_dependent_variables +=
            this->eliminate_dependent_variables_from_objective(
                dependent_variable_ptrs, a_IS_ENABLED_PRINT);

        number_of_eliminated_dependent_variables +=
            this->eliminate_dependent_variables_from_constraints(
                dependent_variable_ptrs, a_IS_ENABLED_PRINT);

        number_of_eliminated_dependent_variables +=
            this->eliminate_dependent_variables_from_expressions(
                dependent_variable_ptrs, a_IS_ENABLED_PRINT);

        utility::print_message("Done.", a_IS_ENABLED_PRINT);
        return number_of_eliminated_dependent_variables;
    }
};
}  // namespace printemps::preprocess
#endif
/*****************************************************************************/
// END
/*****************************************************************************/