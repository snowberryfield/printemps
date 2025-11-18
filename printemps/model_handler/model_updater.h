/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_MODEL_HANDLER_MODEL_UPDATER_H__
#define PRINTEMPS_MODEL_HANDLER_MODEL_UPDATER_H__

namespace printemps::model_handler {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class ModelUpdater {
   private:
    model::Model<T_Variable, T_Expression> *m_model_ptr;

   public:
    /*************************************************************************/
    ModelUpdater(void) {
        m_model_ptr = nullptr;
    }

    /*************************************************************************/
    ModelUpdater(model::Model<T_Variable, T_Expression> *a_model_ptr) {
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
    inline void update(void) {
        /**
         * Update in order of expressions -> dependent variables -> objective,
         * constraints.
         */
        auto &model = *m_model_ptr;
        for (auto &&proxy : model.expression_proxies()) {
            for (auto &&expression : proxy.flat_indexed_expressions()) {
                expression.update();
            }
        }

        for (auto &&variable_ptr :
             model.reference().variable_type.dependent_integer_variable_ptrs) {
            variable_ptr->update();
        }

        for (auto &&variable_ptr :
             model.reference().variable_type.dependent_binary_variable_ptrs) {
            variable_ptr->update();
        }

        for (auto &&proxy : model.constraint_proxies()) {
            for (auto &&constraint : proxy.flat_indexed_constraints()) {
                constraint.update();
            }
        }

        if (model.is_defined_objective()) {
            model.objective().update();
        }

        this->update_violative_constraint_ptrs_and_feasibility();
    }

    /*************************************************************************/
    inline void update(
        const neighborhood::Move<T_Variable, T_Expression> &a_MOVE) {
        /**
         * Update in order of objective, constraints -> expressions ->
         * variables. Note that this method DOES NOT update disabled constraints
         * and dependent variables. If the consistent solution is required,
         * perform update() before obtaining solution.
         */
        auto &model = *m_model_ptr;
        if (model.is_defined_objective()) {
            model.objective().update(a_MOVE);
        }

        if (model.neighborhood().user_defined().is_enabled()) {
            for (auto &&proxy : model.constraint_proxies()) {
                for (auto &&constraint : proxy.flat_indexed_constraints()) {
                    if (constraint.is_enabled()) {
                        constraint.update(a_MOVE);
                    }
                }
            }
        } else {
            for (auto &&constraint_ptr : a_MOVE.related_constraint_ptrs) {
                constraint_ptr->update(a_MOVE);
            }
        }

        for (auto &&proxy : model.expression_proxies()) {
            for (auto &&expression : proxy.flat_indexed_expressions()) {
                if (expression.is_enabled()) {
                    expression.update(a_MOVE);
                }
            }
        }

        for (auto &&alteration : a_MOVE.alterations) {
            alteration.first->set_value_if_mutable(alteration.second);
        }

        if (a_MOVE.sense == neighborhood::MoveSense::Selection) {
            a_MOVE.alterations[1].first->select();
        }

        this->update_violative_constraint_ptrs_and_feasibility();
    }

    /*************************************************************************/
    inline void update_dependent_variables_and_disabled_constraints(void) {
        /**
         * Update in order of expressions -> dependent variables, and
         * constraints.
         */
        auto &model = *m_model_ptr;
        for (auto &&proxy : model.expression_proxies()) {
            for (auto &&expression : proxy.flat_indexed_expressions()) {
                expression.update();
            }
        }

        for (auto &&variable_ptr :
             model.reference().variable_type.dependent_integer_variable_ptrs) {
            variable_ptr->update();
        }

        for (auto &&variable_ptr :
             model.reference().variable_type.dependent_binary_variable_ptrs) {
            variable_ptr->update();
        }

        for (auto &&constraint_ptr :
             model.reference().constraint.disabled_constraint_ptrs) {
            constraint_ptr->update();
        }
    }

    /*************************************************************************/
    inline void update_variable_bounds(const double a_OBJECTIVE,
                                       const bool   a_IS_PRIMAL,
                                       const bool   a_IS_ENABLED_PRINT) {
        auto &model = *m_model_ptr;

        model_component::Constraint<T_Variable, T_Expression> constraint;
        if (model.is_minimization() && a_IS_PRIMAL) {
            constraint = model.objective().expression() <= a_OBJECTIVE;
        } else {
            constraint = model.objective().expression() >= a_OBJECTIVE;
        }

        const auto &[is_constraint_disabled, is_variable_bound_updated] =
            model.problem_size_reducer()
                .remove_redundant_constraint_with_tightening_variable_bound(
                    &constraint, a_IS_ENABLED_PRINT);

        if (!is_variable_bound_updated) {
            return;
        }

        const int NUMBER_OF_NEWLY_FIXED_VARIABLES =
            model.problem_size_reducer().remove_implicit_fixed_variables(
                a_IS_ENABLED_PRINT);

        /**
         * If there is new fixed variable, setup the variable category
         * and the binary/integer neighborhood again.
         */
        if (NUMBER_OF_NEWLY_FIXED_VARIABLES > 0) {
            model.reference().update_variable_reference();
            auto       &neighborhood  = model.neighborhood();
            const auto &VARIABLE_TYPE = model.reference().variable_type;

            neighborhood.binary().setup(VARIABLE_TYPE.binary_variable_ptrs);
            neighborhood.integer().setup(VARIABLE_TYPE.integer_variable_ptrs);
            neighborhood.selection().setup(
                VARIABLE_TYPE.selection_variable_ptrs);
            neighborhood.chain().remove_moves_on_fixed_variables();

            model.builder()
                .setup_positive_and_negative_coefficient_mutable_variable_ptrs();
        }
    }

    /*************************************************************************/
    inline void update_violative_constraint_ptrs_and_feasibility(void) {
        auto &model = *m_model_ptr;

        model.m_previous_violative_constraint_ptrs.swap(
            model.m_current_violative_constraint_ptrs);
        model.m_previous_is_feasible = model.m_current_is_feasible;

        model.m_current_violative_constraint_ptrs.clear();
        for (auto &&constraint_ptr :
             model.reference().constraint.enabled_constraint_ptrs) {
            if (!constraint_ptr->is_feasible()) {
                model.m_current_violative_constraint_ptrs.push_back(
                    constraint_ptr);
            }
        }
        model.m_current_is_feasible =
            (model.m_current_violative_constraint_ptrs.size() == 0);
    }

    /*************************************************************************/
    inline void update_variable_objective_improvabilities(void) {
        auto &model = *m_model_ptr;
        this->update_variable_objective_improvabilities(
            model.reference().variable.mutable_variable_ptrs);
    }

    /*************************************************************************/
    inline void update_variable_objective_improvabilities(
        const std::vector<model_component::Variable<T_Variable, T_Expression> *>
            &a_VARIABLE_PTRS) const noexcept {
        auto &model = *m_model_ptr;

        double coefficient             = 0.0;
        bool   is_objective_improvable = false;

        const double SIGN = model.sign();
        for (const auto &variable_ptr : a_VARIABLE_PTRS) {
            coefficient = variable_ptr->objective_sensitivity() * SIGN;
            is_objective_improvable =
                (coefficient > 0 && variable_ptr->has_lower_bound_margin()) ||
                (coefficient < 0 && variable_ptr->has_upper_bound_margin());

            variable_ptr->set_is_objective_improvable(is_objective_improvable);
        }
    }

    /*************************************************************************/
    inline void reset_variable_objective_improvabilities(void) {
        auto &model = *m_model_ptr;
        this->reset_variable_objective_improvabilities(
            model.reference().variable.variable_ptrs);
    }

    /*************************************************************************/
    inline void reset_variable_objective_improvabilities(
        const std::vector<model_component::Variable<T_Variable, T_Expression> *>
            &a_VARIABLE_PTRS) {
        for (auto &&variable_ptr : a_VARIABLE_PTRS) {
            variable_ptr->set_is_objective_improvable(false);
        }
    }

    /*************************************************************************/
    inline void update_variable_feasibility_improvabilities(void) {
        auto &model = *m_model_ptr;
        this->update_variable_feasibility_improvabilities(
            model.reference().constraint.enabled_constraint_ptrs);
    }

    /*************************************************************************/
    inline void update_variable_feasibility_improvabilities(
        const std::vector<model_component::Constraint<T_Variable, T_Expression>
                              *> &a_CONSTRAINT_PTRS) {
        for (const auto &constraint_ptr : a_CONSTRAINT_PTRS) {
            if (constraint_ptr->is_feasible()) {
                continue;
            }

            if (!constraint_ptr->is_enabled()) {
                continue;
            }

            if (constraint_ptr->constraint_value() > 0) {
                for (const auto &variable_ptr :
                     constraint_ptr->expression()
                         .positive_coefficient_mutable_variable_ptrs()) {
                    variable_ptr
                        ->set_is_feasibility_improvable_if_has_lower_bound_margin();
                }

                for (const auto &variable_ptr :
                     constraint_ptr->expression()
                         .negative_coefficient_mutable_variable_ptrs()) {
                    variable_ptr
                        ->set_is_feasibility_improvable_if_has_upper_bound_margin();
                }
            } else {
                for (const auto &variable_ptr :
                     constraint_ptr->expression()
                         .negative_coefficient_mutable_variable_ptrs()) {
                    variable_ptr
                        ->set_is_feasibility_improvable_if_has_lower_bound_margin();
                }

                for (const auto &variable_ptr :
                     constraint_ptr->expression()
                         .positive_coefficient_mutable_variable_ptrs()) {
                    variable_ptr
                        ->set_is_feasibility_improvable_if_has_upper_bound_margin();
                }
            }
        }
    }
    /*************************************************************************/
    inline void reset_variable_feasibility_improvabilities(void) {
        auto &model = *m_model_ptr;
        this->reset_variable_feasibility_improvabilities(
            model.reference().variable.variable_ptrs);
    }

    /*************************************************************************/
    inline void reset_variable_feasibility_improvabilities(
        const std::vector<model_component::Variable<T_Variable, T_Expression> *>
            &a_VARIABLE_PTRS) const noexcept {
        for (auto &&variable_ptr : a_VARIABLE_PTRS) {
            variable_ptr->set_is_feasibility_improvable(false);
        }
    }

    /*************************************************************************/
    inline void reset_variable_feasibility_improvabilities(
        const std::vector<model_component::Constraint<T_Variable, T_Expression>
                              *> &a_CONSTRAINT_PTRS) const noexcept {
        for (const auto &constraint_ptr : a_CONSTRAINT_PTRS) {
            if (!constraint_ptr->is_enabled()) {
                continue;
            }
            auto &sensitivities = constraint_ptr->expression().sensitivities();
            for (const auto &sensitivity : sensitivities) {
                sensitivity.first->set_is_feasibility_improvable(false);
            }
        }
    }
};
}  // namespace printemps::model_handler
#endif
/*****************************************************************************/
// END
/*****************************************************************************/