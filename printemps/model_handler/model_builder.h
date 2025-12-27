/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_MODEL_HANDLER_MODEL_BUILDER_H__
#define PRINTEMPS_MODEL_HANDLER_MODEL_BUILDER_H__

namespace printemps::model_handler {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class ModelBuilder {
   private:
    model::Model<T_Variable, T_Expression> *m_model_ptr;

   public:
    /*************************************************************************/
    ModelBuilder(void) {
        m_model_ptr = nullptr;
    }

    /*************************************************************************/
    ModelBuilder(model::Model<T_Variable, T_Expression> *a_model_ptr) {
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

    /*************************************************************************/
    inline void build(const option::Option &a_OPTION,
                      const bool            a_IS_ENABLED_PRINT) {
        auto &model = *m_model_ptr;

        /**
         * Verify the problem.
         */
        model.verifier().verify_problem(a_IS_ENABLED_PRINT);

        /**
         * Determine unique name of variables and constraints.
         */
        this->setup_unique_names();

        /**
         * Initial structure analysis.
         */
        this->setup_structure();

        /**
         * Store original categorization results. The final categorization would
         * be changed by presolving, extracting/eliminating intermediate
         * variables, and extracting selection constraints.
         */
        model.m_reference_original = model.m_reference;

        /**
         * Presolve the problem by removing redundant constraints and fixing
         * variables implicitly fixed.
         */
        if (a_OPTION.preprocess.is_enabled_presolve) {
            if (a_OPTION.preprocess
                    .is_enabled_extract_implicit_equality_constraints) {
                model.problem_size_reducer()
                    .extract_implicit_equality_constraints(a_IS_ENABLED_PRINT);
            }

            if (a_OPTION.preprocess
                    .is_enabled_remove_redundant_set_constraints) {
                model.problem_size_reducer().remove_redundant_set_constraints(
                    a_IS_ENABLED_PRINT);
            }

            model.problem_size_reducer().reduce_problem_size(
                a_IS_ENABLED_PRINT);
        }

        /**
         * Extract and eliminate the intermediate variables.
         */
        this->setup_structure();

        if (a_OPTION.preprocess.is_enabled_presolve &&
            a_OPTION.preprocess.is_enabled_extract_dependent()) {
            preprocess::DependentVariableExtractor<T_Variable, T_Expression>
                dependent_variable_extractor(m_model_ptr);
            preprocess::DependentVariableEliminator<T_Variable, T_Expression>
                dependent_variable_eliminator(m_model_ptr);
            while (true) {
                if (dependent_variable_extractor.extract(
                        a_OPTION, a_IS_ENABLED_PRINT) == 0) {
                    break;
                }

                while (true) {
                    this->setup_structure();
                    if (dependent_variable_eliminator.eliminate(
                            a_IS_ENABLED_PRINT) == 0) {
                        break;
                    }
                }

                model.problem_size_reducer().reduce_problem_size(
                    a_IS_ENABLED_PRINT);
                this->setup_structure();
            }
        }

        /**
         * Remove redundant set variables.
         */
        int number_of_fixed_variables = 0;
        if (a_OPTION.preprocess.is_enabled_presolve &&
            a_OPTION.preprocess.is_enabled_remove_redundant_set_variables) {
            number_of_fixed_variables =
                model.problem_size_reducer().remove_redundant_set_variables(
                    a_IS_ENABLED_PRINT);
        }

        /**
         * Remove duplicated constraints.
         */
        int number_of_removed_constraints = 0;
        if (a_OPTION.preprocess.is_enabled_presolve &&
            a_OPTION.preprocess.is_enabled_remove_duplicated_constraints) {
            number_of_removed_constraints =
                model.problem_size_reducer().remove_duplicated_constraints(
                    a_IS_ENABLED_PRINT);
        }

        /**
         * Perform setup_structure again if there are new removed(disabled)
         * variables or constraints.
         */
        if (number_of_fixed_variables > 0 ||
            number_of_removed_constraints > 0) {
            this->setup_structure();
        }

        /**
         * Extract selection constraints. If the number of constraints is bigger
         * than that of variables, this process will be skipped because it would
         * affect computational efficiency.
         */
        if (a_OPTION.neighborhood.selection_mode !=
                option::selection_mode::Off &&
            model.reference().number_of_variables() >
                model.reference().number_of_constraints()) {
            preprocess::SelectionExtractor<T_Variable, T_Expression>
                selection_extractor(m_model_ptr);
            selection_extractor.extract(a_OPTION.neighborhood.selection_mode,
                                        a_IS_ENABLED_PRINT);
        }

        /**
         * Final structure analysis.
         */
        this->setup_structure();

        /**
         * Setup the neighborhood generators.
         */
        this->setup_neighborhood(a_OPTION, a_IS_ENABLED_PRINT);

        /**
         * Verify and correct the initial values.
         */
        model.verifier().verify_and_correct(
            a_OPTION.preprocess.is_enabled_initial_value_correction,
            a_IS_ENABLED_PRINT);

        /**
         * Solve GF(2) equations if needed.
         */
        if (a_OPTION.preprocess.is_enabled_presolve &&
            model.reference().constraint_type.gf2_ptrs.size() > 0) {
            preprocess::GF2Solver<T_Variable, T_Expression> gf2_solver(
                m_model_ptr);
            const auto IS_SOLVED = gf2_solver.solve(a_IS_ENABLED_PRINT);

            /**
             * Update fixed variables.
             */
            if (IS_SOLVED) {
                model.reference().update_variable_reference();
            }
        }

        /**
         * Set up the fixed sensitivities.
         */
        this->setup_fixed_sensitivities(a_IS_ENABLED_PRINT);

        /**
         * Set up the pointers to mutable variable with positive and negative
         * coefficients for efficient improvability screening.
         */
        this->setup_positive_and_negative_coefficient_mutable_variable_ptrs();

        /**
         * Set up the constraint sensitivities of variables.
         */
        this->setup_variable_constraint_sensitivities();

        /**
         * Set up the indices of related constraints for selection variables.
         */
        this->setup_variable_related_selection_constraint_ptr_index();

        /**
         * Set up the integrity of constraints.
         */
        this->setup_is_integer();

        /**
         * Store the global penalty coefficient for evaluation.
         */
        model.set_global_penalty_coefficient(
            a_OPTION.penalty.initial_penalty_coefficient);

        /**
         * Modify the global penalty coefficient.
         */
        if (model.m_is_integer &&
            a_OPTION.penalty.is_enabled_shrink_penalty_coefficient) {
            this->shrink_global_penalty_coefficient(a_IS_ENABLED_PRINT);
        }

        for (auto &&proxy : model.constraint_proxies()) {
            for (auto &&constraint : proxy.flat_indexed_constraints()) {
                constraint.global_penalty_coefficient() =
                    model.global_penalty_coefficient();
            }
        }

        model.problem_size_reducer().set_is_preprocess(false);
    }

    /*************************************************************************/
    inline void setup_structure(void) {
        auto &model = *m_model_ptr;

        for (auto &&proxy : model.constraint_proxies()) {
            for (auto &&constraint : proxy.flat_indexed_constraints()) {
                constraint.update_structure();
                constraint.update_constraint_type();
            }
        }

        model.reference().update_variable_reference();
        model.reference().update_constraint_reference();
        model.reference().update_variable_name_map();
        model.reference().update_contraint_name_map();

        this->setup_variable_related_constraints();
        this->setup_variable_related_binary_coefficient_constraints();
        this->setup_variable_objective_sensitivities();
        this->setup_variable_constraint_sensitivities();
    }

    /*************************************************************************/
    inline void setup_unique_names(void) {
        auto &model = *m_model_ptr;

        const int VARIABLE_PROXIES_SIZE   = model.variable_proxies().size();
        const int EXPRESSION_PROXIES_SIZE = model.expression_proxies().size();
        const int CONSTRAINT_PROXIES_SIZE = model.constraint_proxies().size();

        for (auto i = 0; i < VARIABLE_PROXIES_SIZE; i++) {
            const int NUMBER_OF_ELEMENTS =
                model.variable_proxies()[i].number_of_elements();
            for (auto j = 0; j < NUMBER_OF_ELEMENTS; j++) {
                auto &variable =
                    model.variable_proxies()[i].flat_indexed_variables(j);

                if (variable.name() == "") {
                    variable.set_name(
                        model.variable_names()[i] +
                        model.variable_proxies()[i].indices_label(j));
                }
            }
        }

        /// Expression
        for (auto i = 0; i < EXPRESSION_PROXIES_SIZE; i++) {
            const int NUMBER_OF_ELEMENTS =
                model.expression_proxies()[i].number_of_elements();
            for (auto j = 0; j < NUMBER_OF_ELEMENTS; j++) {
                auto &expression =
                    model.expression_proxies()[i].flat_indexed_expressions(j);

                if (expression.name() == "") {
                    expression.set_name(
                        model.expression_names()[i] +
                        model.expression_proxies()[i].indices_label(j));
                }
            }
        }

        /// Constraint
        for (auto i = 0; i < CONSTRAINT_PROXIES_SIZE; i++) {
            const int NUMBER_OF_ELEMENTS =
                model.constraint_proxies()[i].number_of_elements();
            for (auto j = 0; j < NUMBER_OF_ELEMENTS; j++) {
                auto &constraint =
                    model.constraint_proxies()[i].flat_indexed_constraints(j);

                if (constraint.name() == "") {
                    constraint.set_name(
                        model.constraint_names()[i] +
                        model.constraint_proxies()[i].indices_label(j));
                }
            }
        }
    }

    /*************************************************************************/
    inline void setup_is_integer(void) {
        /**
         * NOTE: In this method, m_reference is not referred because
         * the object may not have been set up at the stage this method is
         * called.
         */
        auto &model = *m_model_ptr;

        model.m_is_integer = true;
        for (auto &&proxy : model.constraint_proxies()) {
            for (auto &&constraint : proxy.flat_indexed_constraints()) {
                if (!constraint.structure().is_integer) {
                    model.m_is_integer = false;
                    return;
                }
            }
        }
    }

    /*************************************************************************/
    inline void setup_variable_related_constraints(void) {
        /**
         * NOTE: In this method, m_reference is not referred because
         * the object would not be set up at the stage when this method is
         * called.
         */
        auto &model = *m_model_ptr;

        for (auto &&proxy : model.variable_proxies()) {
            for (auto &&variable : proxy.flat_indexed_variables()) {
                variable.reset_related_constraint_ptrs();
            }
        }

        for (auto &&proxy : model.constraint_proxies()) {
            for (auto &&constraint : proxy.flat_indexed_constraints()) {
                for (auto &&sensitivity :
                     constraint.expression().sensitivities()) {
                    sensitivity.first->register_related_constraint_ptr(
                        &constraint);
                }
            }
        }

        for (auto &&proxy : model.variable_proxies()) {
            for (auto &&variable : proxy.flat_indexed_variables()) {
                variable.sort_and_unique_related_constraint_ptrs();
            }
        }
    }

    /*************************************************************************/
    inline void setup_variable_objective_sensitivities(void) {
        auto &model = *m_model_ptr;

        for (auto &&sensitivity :
             model.objective().expression().sensitivities()) {
            sensitivity.first->set_objective_sensitivity(sensitivity.second);
        }
    }

    /*************************************************************************/
    inline void setup_variable_constraint_sensitivities(void) {
        /**
         * NOTE: In this method, m_reference is not referred because
         * the object may not have been set up at the stage this method is
         * called.
         */
        auto &model = *m_model_ptr;

        for (auto &&proxy : model.variable_proxies()) {
            for (auto &&variable : proxy.flat_indexed_variables()) {
                variable.reset_constraint_sensitivities();
            }
        }
        for (auto &&proxy : model.constraint_proxies()) {
            for (auto &&constraint : proxy.flat_indexed_constraints()) {
                for (auto &&sensitivity :
                     constraint.expression().sensitivities()) {
                    sensitivity.first->register_constraint_sensitivity(
                        &constraint, sensitivity.second);
                }
            }
        }
        for (auto &&proxy : model.variable_proxies()) {
            for (auto &&variable : proxy.flat_indexed_variables()) {
                variable.sort_constraint_sensitivities();
            }
        }
    }

    /*************************************************************************/
    inline void setup_variable_related_selection_constraint_ptr_index(void) {
        auto &model = *m_model_ptr;

        const auto &REFERENCE     = model.reference();
        const auto &VARIABLE_TYPE = REFERENCE.variable_type;

        for (auto &&variable_ptr : VARIABLE_TYPE.selection_variable_ptrs) {
            variable_ptr->setup_related_selection_constraint_ptr_index();
        }
    }

    /*************************************************************************/
    inline void setup_variable_related_binary_coefficient_constraints(void) {
        /**
         * NOTE: In this method, m_reference is not referred because
         * the object may not have been set up at the stage this method is
         * called.
         */
        auto &model = *m_model_ptr;

        for (auto &&proxy : model.variable_proxies()) {
            for (auto &&variable : proxy.flat_indexed_variables()) {
                variable.reset_related_binary_coefficient_constraint_ptrs();
                variable.setup_related_binary_coefficient_constraint_ptrs();
            }
        }
    }

    /*************************************************************************/
    inline void setup_neighborhood(const option::Option &a_OPTION,
                                   const bool            a_IS_ENABLED_PRINT) {
        utility::print_single_line(a_IS_ENABLED_PRINT);
        utility::print_message("Detecting the neighborhood structure...",
                               a_IS_ENABLED_PRINT);
        auto &model = *m_model_ptr;
        model.neighborhood().setup(m_model_ptr, a_OPTION);

        utility::print_message("Done.", a_IS_ENABLED_PRINT);
    }

    /*************************************************************************/
    inline void setup_fixed_sensitivities(const bool a_IS_ENABLED_PRINT) {
        utility::print_single_line(a_IS_ENABLED_PRINT);
        utility::print_message("Creating the sensitivity matrix...",
                               a_IS_ENABLED_PRINT);

        auto &model = *m_model_ptr;
        for (auto &&proxy : model.expression_proxies()) {
            for (auto &&expression : proxy.flat_indexed_expressions()) {
                expression.setup_fixed_sensitivities();
            }
        }

        for (auto &&proxy : model.constraint_proxies()) {
            for (auto &&constraint : proxy.flat_indexed_constraints()) {
                constraint.expression().setup_fixed_sensitivities();
            }
        }
        model.objective().expression().setup_fixed_sensitivities();

        /**
         * The fixed sensitivities for the constraints and the objective are
         * build in their own setup() methods.
         */
        utility::print_message("Done.", a_IS_ENABLED_PRINT);
    }

    /*************************************************************************/
    inline void setup_positive_and_negative_coefficient_mutable_variable_ptrs(
        void) {
        auto &model = *m_model_ptr;
        for (auto &&proxy : model.expression_proxies()) {
            for (auto &&expression : proxy.flat_indexed_expressions()) {
                expression
                    .setup_positive_and_negative_coefficient_mutable_variable_ptrs();
            }
        }

        for (auto &&proxy : model.constraint_proxies()) {
            for (auto &&constraint : proxy.flat_indexed_constraints()) {
                constraint.expression()
                    .setup_positive_and_negative_coefficient_mutable_variable_ptrs();
            }
        }
        model.objective()
            .expression()
            .setup_positive_and_negative_coefficient_mutable_variable_ptrs();
    }

    /*************************************************************************/
    inline void shrink_global_penalty_coefficient(
        const bool a_IS_ENABLED_PRINT) {
        utility::print_single_line(a_IS_ENABLED_PRINT);
        utility::print_message("Modifying the global penalty coefficient...",
                               a_IS_ENABLED_PRINT);

        auto      &model    = *m_model_ptr;
        const auto ORIGINAL = model.global_penalty_coefficient();
        const auto MODIFIED =
            std::min(model.global_penalty_coefficient(),
                     model.objective().expression().upper_bound() -
                         model.objective().expression().lower_bound() + 1.0);

        if (MODIFIED < ORIGINAL) {
            model.set_global_penalty_coefficient(MODIFIED);
            utility::print_message(
                "Done (New global penalty coefficient is " +
                    utility::to_string(model.global_penalty_coefficient(),
                                       "%.5e") +
                    ")",
                a_IS_ENABLED_PRINT);
        } else {
            utility::print_message(
                "Done (global penalty coefficient remains at the original "
                "value).",
                a_IS_ENABLED_PRINT);
        }
    }

    /*************************************************************************/
    inline void setup_selections(
        const std::vector<model_component::Selection<T_Variable, T_Expression>>
            &a_SELECTIONS) {
        auto &model        = *m_model_ptr;
        model.m_selections = a_SELECTIONS;

        for (auto &&selection : model.m_selections) {
            for (auto &&variable_ptr : selection.variable_ptrs) {
                /**
                 * Register the selection object to the variables which is
                 * covered by the corresponding selection constraint, and
                 * categorize the variable into "Selection".
                 */
                variable_ptr->set_selection_ptr(&selection);
            }
            selection.constraint_ptr->set_is_selection(true);
        }
    }

    /*************************************************************************/
    inline void setup_user_defined_selection_constraints(
        const std::unordered_set<std::string> &a_CONSTRAINT_NAMES) {
        auto &model = *m_model_ptr;

        for (auto &&proxy : model.constraint_proxies()) {
            for (auto &&constraint : proxy.flat_indexed_constraints()) {
                if (a_CONSTRAINT_NAMES.find(constraint.name()) !=
                    a_CONSTRAINT_NAMES.end()) {
                    constraint.set_is_user_defined_selection(true);
                }
            }
        }
    }

    /*************************************************************************/
    inline void setup_flippable_variable_ptr_pairs(
        const std::vector<std::pair<std::string, std::string>>
            &a_VARIABLE_NAME_PAIRS) {
        auto &model = *m_model_ptr;

        std::unordered_map<
            std::string, model_component::Variable<T_Variable, T_Expression> *>
            variable_ptrs;

        for (auto &&proxy : model.variable_proxies()) {
            for (auto &&variable : proxy.flat_indexed_variables()) {
                variable_ptrs[variable.name()] = &variable;
            }
        }

        std::vector<
            std::pair<model_component::Variable<T_Variable, T_Expression> *,
                      model_component::Variable<T_Variable, T_Expression> *>>
            flippable_variable_ptr_pairs;

        for (const auto &pair : a_VARIABLE_NAME_PAIRS) {
            if (variable_ptrs.find(pair.first) != variable_ptrs.end() &&
                variable_ptrs.find(pair.second) != variable_ptrs.end()) {
                flippable_variable_ptr_pairs.emplace_back(
                    variable_ptrs[pair.first],  //
                    variable_ptrs[pair.second]);
            }
        }

        model.m_flippable_variable_ptr_pairs = flippable_variable_ptr_pairs;
    }
};
}  // namespace printemps::model_handler
#endif
/*****************************************************************************/
// END
/*****************************************************************************/