/*****************************************************************************/
// Copyright (c) 2020-2023 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_PREPROCESS_DEPENDENT_VARIABLE_EXTRACTOR_H__
#define PRINTEMPS_PREPROCESS_DEPENDENT_VARIABLE_EXTRACTOR_H__

namespace printemps::preprocess {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class DependentVariableExtractor {
   private:
    model::Model<T_Variable, T_Expression> *m_model_ptr;

   public:
    /*************************************************************************/
    DependentVariableExtractor(void) {
        this->initialize();
    }

    /*************************************************************************/
    DependentVariableExtractor(
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
    inline int extract(const option::Option &a_OPTION,
                       const bool            a_IS_ENABLED_PRINT) {
        /**
         * NOTE: This function cannot be constexpr with clang for
         * std::vector<bool>.
         */
        utility::print_single_line(a_IS_ENABLED_PRINT);
        utility::print_message("Extracting dependent variables...",
                               a_IS_ENABLED_PRINT);

        int number_of_newly_extracted_dependent_variables = 0;

        auto &reference = m_model_ptr->constraint_type_reference();

        auto &exclusive_or_ptrs          = reference.exclusive_or_ptrs;
        auto &exclusive_nor_ptrs         = reference.exclusive_nor_ptrs;
        auto &inverted_integers_ptrs     = reference.inverted_integers_ptrs;
        auto &balanced_integers_ptrs     = reference.balanced_integers_ptrs;
        auto &constant_sum_integers_ptrs = reference.constant_sum_integers_ptrs;
        auto &constant_difference_integers_ptrs =
            reference.constant_difference_integers_ptrs;
        auto &constant_ratio_integers_ptrs =
            reference.constant_ratio_integers_ptrs;
        auto &intermediate_ptrs     = reference.intermediate_ptrs;
        auto &set_partitioning_ptrs = reference.set_partitioning_ptrs;

        std::vector<model_component::Constraint<T_Variable, T_Expression> *>
            constraint_ptrs;

        std::unordered_set<
            model_component::Variable<T_Variable, T_Expression> *>
            set_partitioning_variable_ptrs;
        for (auto &&set_partitioning_ptr : set_partitioning_ptrs) {
            for (auto &&sensitivity :
                 set_partitioning_ptr->expression().sensitivities()) {
                set_partitioning_variable_ptrs.insert(sensitivity.first);
            }
        }

        /// Exclusive OR
        if (a_OPTION.preprocess.is_enabled_extract_dependent_exclusive_or) {
            for (auto &&exclusive_or_ptr : exclusive_or_ptrs) {
                bool is_valid = true;
                for (auto &&sensitivity :
                     exclusive_or_ptr->expression().sensitivities()) {
                    if (set_partitioning_variable_ptrs.find(
                            sensitivity.first) !=
                        set_partitioning_variable_ptrs.end()) {
                        is_valid = false;
                        break;
                    }
                }
                if (is_valid) {
                    constraint_ptrs.push_back(exclusive_or_ptr);
                }
            }
        }

        /// Exclusive NOR
        if (a_OPTION.preprocess.is_enabled_extract_dependent_exclusive_nor) {
            for (auto &&exclusive_nor_ptr : exclusive_nor_ptrs) {
                bool is_valid = true;
                for (auto &&sensitivity :
                     exclusive_nor_ptr->expression().sensitivities()) {
                    if (set_partitioning_variable_ptrs.find(
                            sensitivity.first) !=
                        set_partitioning_variable_ptrs.end()) {
                        is_valid = false;
                        break;
                    }
                }
                if (is_valid) {
                    constraint_ptrs.push_back(exclusive_nor_ptr);
                }
            }
        }

        /// Inverted Integers
        if (a_OPTION.preprocess
                .is_enabled_extract_dependent_inverted_integers) {
            constraint_ptrs.insert(constraint_ptrs.end(),
                                   inverted_integers_ptrs.begin(),
                                   inverted_integers_ptrs.end());
        }

        /// Balanced Integers
        if (a_OPTION.preprocess
                .is_enabled_extract_dependent_balanced_integers) {
            constraint_ptrs.insert(constraint_ptrs.end(),
                                   balanced_integers_ptrs.begin(),
                                   balanced_integers_ptrs.end());
        }

        /// Constant Sum Integers
        if (a_OPTION.preprocess
                .is_enabled_extract_dependent_constant_sum_integers) {
            constraint_ptrs.insert(constraint_ptrs.end(),
                                   constant_sum_integers_ptrs.begin(),
                                   constant_sum_integers_ptrs.end());
        }

        /// Constant Difference Integers
        if (a_OPTION.preprocess
                .is_enabled_extract_dependent_constant_difference_integers) {
            constraint_ptrs.insert(constraint_ptrs.end(),
                                   constant_difference_integers_ptrs.begin(),
                                   constant_difference_integers_ptrs.end());
        }

        /// Constant Ratio Integers
        if (a_OPTION.preprocess
                .is_enabled_extract_dependent_constant_ratio_integers) {
            constraint_ptrs.insert(constraint_ptrs.end(),
                                   constant_ratio_integers_ptrs.begin(),
                                   constant_ratio_integers_ptrs.end());
        }

        /// Constant Intermediate Integers
        if (a_OPTION.preprocess.is_enabled_extract_dependent_intermediate) {
            constraint_ptrs.insert(constraint_ptrs.end(),  //
                                   intermediate_ptrs.begin(),
                                   intermediate_ptrs.end());
        }

        const int CONSTRAINTS_SIZE = constraint_ptrs.size();

        if (CONSTRAINTS_SIZE == 0) {
            return number_of_newly_extracted_dependent_variables;
        }

        auto adj = utility::BinaryMatrix::identity(CONSTRAINTS_SIZE);

        for (auto i = 0; i < CONSTRAINTS_SIZE; i++) {
            auto key = constraint_ptrs[i]->key_variable_ptr();
            for (auto j = 0; j < CONSTRAINTS_SIZE; j++) {
                auto &sensitivities =
                    constraint_ptrs[j]->expression().sensitivities();
                if (i != j && sensitivities.find(key) != sensitivities.end()) {
                    adj[i][j] = 1;
                }
            }
        }
        std::vector<bool> flags(CONSTRAINTS_SIZE, true);
        std::unordered_map<
            model_component::Variable<T_Variable, T_Expression> *, int>
            key_variable_ptr_counts;

        for (auto i = 0; i < CONSTRAINTS_SIZE; i++) {
            if (!constraint_ptrs[i]->is_enabled()) {
                flags[i] = false;
                continue;
            }
            if (!flags[i]) {
                continue;
            }
            for (auto j = i + 1; j < CONSTRAINTS_SIZE; j++) {
                if (adj[i][j] > 0 && adj[j][i] > 0) {
                    flags[j] = false;
                }
            }
            auto key_variable_ptr = constraint_ptrs[i]->key_variable_ptr();
            if (key_variable_ptr_counts.find(key_variable_ptr) !=
                key_variable_ptr_counts.end()) {
                key_variable_ptr_counts[key_variable_ptr] = 0;
            } else {
                key_variable_ptr_counts[key_variable_ptr] += 1;
            }
        }

        std::vector<model_component::Variable<T_Variable, T_Expression> *>
            dependent_variable_ptrs;
        std::vector<model_component::Expression<T_Variable, T_Expression>>
            additional_expressions;
        std::vector<model_component::Constraint<T_Variable, T_Expression>>
            additional_constraints;

        for (auto i = 0; i < CONSTRAINTS_SIZE; i++) {
            auto constraint_ptr = constraint_ptrs[i];
            if (!constraint_ptr->is_enabled()) {
                continue;
            }
            auto key_variable_ptr = constraint_ptr->key_variable_ptr();

            if (flags[i] && key_variable_ptr_counts[key_variable_ptr] == 1) {
                utility::print_message(
                    "The variable " + key_variable_ptr->name() +
                        " in the constraint " + constraint_ptr->name() +
                        " was extracted as a dependent variable. ",
                    a_IS_ENABLED_PRINT);

                constraint_ptr->disable();

                dependent_variable_ptrs.push_back(key_variable_ptr);
                additional_expressions.emplace_back(
                    constraint_ptr->expression().solve(key_variable_ptr));

                auto &expression = additional_expressions.back();
                expression.set_name(key_variable_ptr->name() + "_dependent");

                if (key_variable_ptr->lower_bound() != constant::INT_HALF_MIN &&
                    key_variable_ptr->lower_bound() >
                        expression.lower_bound()) {
                    additional_constraints.emplace_back(
                        expression >= key_variable_ptr->lower_bound());
                    additional_constraints.back().set_name(
                        constraint_ptr->name() + "_greater");
                }

                if (key_variable_ptr->upper_bound() != constant::INT_HALF_MAX &&
                    key_variable_ptr->upper_bound() <
                        expression.upper_bound()) {
                    additional_constraints.emplace_back(
                        expression <= key_variable_ptr->upper_bound());
                    additional_constraints.back().set_name(
                        constraint_ptr->name() + "_less");
                }

                number_of_newly_extracted_dependent_variables++;
            }
        }

        if (additional_expressions.size() > 0) {
            int   number_of_new_expressions = 0;
            auto &dependent_expression_map =
                m_model_ptr->dependent_expression_map();
            for (auto &&variable_ptr : dependent_variable_ptrs) {
                if (dependent_expression_map.find(variable_ptr) ==
                    dependent_expression_map.end()) {
                    number_of_new_expressions++;
                }
            }

            auto &additional_expression_proxy = m_model_ptr->create_expressions(
                "additional", number_of_new_expressions);

            const int DEPENDENT_VARIABLES_SIZE = dependent_variable_ptrs.size();
            auto      count                    = 0;

            for (auto i = 0; i < DEPENDENT_VARIABLES_SIZE; i++) {
                auto &variable_ptr = dependent_variable_ptrs[i];
                auto &expression   = additional_expressions[i];

                if (dependent_expression_map.find(variable_ptr) !=
                    dependent_expression_map.end()) {
                    *(variable_ptr->dependent_expression_ptr()) = expression;
                } else {
                    additional_expression_proxy(count) = expression;
                    additional_expression_proxy(count).set_name(
                        expression.name());
                    variable_ptr->set_dependent_expression_ptr(
                        &additional_expression_proxy(count));
                    count++;
                }
            }
        }

        if (additional_constraints.size() > 0) {
            const int ADDITIONAL_CONSTRAINTS_SIZE =
                additional_constraints.size();
            auto &additional_constraint_proxy = m_model_ptr->create_constraints(
                "additional", ADDITIONAL_CONSTRAINTS_SIZE);

            for (auto i = 0; i < ADDITIONAL_CONSTRAINTS_SIZE; i++) {
                auto &constraint               = additional_constraints[i];
                additional_constraint_proxy(i) = constraint;
                additional_constraint_proxy(i).set_name(constraint.name());
                utility::print_message(
                    "An extra constraint " + constraint.name() + " was added.",
                    a_IS_ENABLED_PRINT);
            }
        }

        utility::print_message("Done.", a_IS_ENABLED_PRINT);
        return number_of_newly_extracted_dependent_variables;
    }

    /*************************************************************************/
    inline constexpr int eliminate(const bool a_IS_ENABLED_PRINT) {
        utility::print_single_line(a_IS_ENABLED_PRINT);
        utility::print_message("Eliminating dependent integer variables...",
                               a_IS_ENABLED_PRINT);

        int number_of_newly_eliminated_dependent_integer_variables = 0;

        auto &binary_ptrs = m_model_ptr->variable_type_reference()
                                .dependent_binary_variable_ptrs;
        auto &integer_ptrs = m_model_ptr->variable_type_reference()
                                 .dependent_integer_variable_ptrs;

        std::vector<model_component::Variable<T_Variable, T_Expression> *>
            variable_ptrs;

        variable_ptrs.insert(variable_ptrs.end(), binary_ptrs.begin(),
                             binary_ptrs.end());
        variable_ptrs.insert(variable_ptrs.end(), integer_ptrs.begin(),
                             integer_ptrs.end());

        /// Objective
        {
            auto &objective_sensitivities =
                m_model_ptr->objective().expression().sensitivities();

            for (auto &&variable_ptr : variable_ptrs) {
                if (objective_sensitivities.find(variable_ptr) !=
                    objective_sensitivities.end()) {
                    m_model_ptr->objective().expression().substitute(
                        variable_ptr,  //
                        *(variable_ptr->dependent_expression_ptr()));

                    number_of_newly_eliminated_dependent_integer_variables++;

                    utility::print_message(
                        "The dependent variable " + variable_ptr->name() +
                            " in the objective function was eliminated. ",
                        a_IS_ENABLED_PRINT);
                }
            }
        }

        /// Constraint
        for (auto &&variable_ptr : variable_ptrs) {
            for (auto &&constraint_ptr :
                 variable_ptr->related_constraint_ptrs()) {
                if (!constraint_ptr->is_enabled()) {
                    continue;
                }

                constraint_ptr->expression().substitute(
                    variable_ptr,  //
                    *(variable_ptr->dependent_expression_ptr()));

                number_of_newly_eliminated_dependent_integer_variables++;

                utility::print_message(
                    "The dependent variable " + variable_ptr->name() +
                        " in the constraint " + constraint_ptr->name() +
                        " was eliminated. ",
                    a_IS_ENABLED_PRINT);
            }
        }

        /// Expression
        for (auto &&variable_ptr : variable_ptrs) {
            for (auto &&proxy : m_model_ptr->expression_proxies()) {
                for (auto &&expression : proxy.flat_indexed_expressions()) {
                    if (expression.sensitivities().find(variable_ptr) ==
                        expression.sensitivities().end()) {
                        continue;
                    }
                    expression.substitute(
                        variable_ptr,  //
                        *(variable_ptr->dependent_expression_ptr()));

                    number_of_newly_eliminated_dependent_integer_variables++;

                    utility::print_message(
                        "The dependent variable " + variable_ptr->name() +
                            " in the expression " + expression.name() +
                            " was eliminated. ",
                        a_IS_ENABLED_PRINT);
                }
            }
        }
        utility::print_message("Done.", a_IS_ENABLED_PRINT);
        return number_of_newly_eliminated_dependent_integer_variables;
    }
};
}  // namespace printemps::preprocess
#endif
/*****************************************************************************/
// END
/*****************************************************************************/