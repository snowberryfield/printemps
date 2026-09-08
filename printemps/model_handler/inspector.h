/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_MODEL_HANDLER_INSPECTOR_H__
#define PRINTEMPS_MODEL_HANDLER_INSPECTOR_H__

namespace printemps::model {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class Model;
}  // namespace printemps::model

namespace printemps::model_handler {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class Inspector {
   private:
    model::Model<T_Variable, T_Expression> *m_model_ptr;

   public:
    /*************************************************************************/
    Inspector(void) {
        this->initialize();
    }

    /*************************************************************************/
    Inspector(model::Model<T_Variable, T_Expression> *a_model_ptr) {
        this->setup(a_model_ptr);
    }

    /*************************************************************************/
    inline void setup(model::Model<T_Variable, T_Expression> *a_model_ptr) {
        this->initialize();
        this->m_model_ptr = a_model_ptr;
    }

    /*************************************************************************/
    inline void initialize(void) {
        this->m_model_ptr = nullptr;
    }

    /*************************************************************************/
    inline bool compute_is_all_binary_variables(void) const {
        if (this->m_model_ptr == nullptr) {
            return false;
        }

        auto &model = *this->m_model_ptr;
        for (auto &&proxy : model.variable_proxies()) {
            for (auto &&variable : proxy.flat_indexed_variables()) {
                if (variable.type() != model_component::VariableType::Binary &&
                    variable.type() !=
                        model_component::VariableType::Selection &&
                    variable.type() !=
                        model_component::VariableType::DependentBinary) {
                    return false;
                }
            }
        }
        return true;
    }

    /*************************************************************************/
    inline bool compute_is_all_integer_coefficients(void) const {
        if (this->m_model_ptr == nullptr) {
            return false;
        }

        auto &model = *this->m_model_ptr;
        for (auto &&proxy : model.constraint_proxies()) {
            for (auto &&constraint : proxy.flat_indexed_constraints()) {
                if (!constraint.is_enabled()) {
                    continue;
                }

                if (!constraint.structure().is_integer) {
                    return false;
                }
            }
        }
        return true;
    }

    /*************************************************************************/
    inline bool compute_is_monotone(void) const {
        if (this->m_model_ptr == nullptr) {
            return false;
        }

        auto &model = *this->m_model_ptr;
        if (model.reference().number_of_variables() == 0 ||
            model.reference().number_of_constraints() == 0) {
            return true;
        }

        for (const auto &constraint_ptr :
             model.reference().constraint.enabled_constraint_ptrs) {
            if (constraint_ptr->sense() ==
                model_component::ConstraintSense::Equal) {
                return false;
            }
        }

        for (const auto &variable_ptr :
             model.reference().variable.mutable_independent_variable_ptrs) {
            const auto &CONSTRAINT_SENSITIVITIES =
                variable_ptr->constraint_sensitivities();

            if (CONSTRAINT_SENSITIVITIES.empty()) {
                continue;
            }

            model_component::Constraint<T_Variable, T_Expression>
                *first_constraint_ptr = nullptr;
            model_component::ConstraintSense first_constraint_sense =
                model_component::ConstraintSense::Less;
            T_Expression first_constraint_coeff = 0;
            std::size_t  start_idx              = 0;

            for (std::size_t i = 0; i < CONSTRAINT_SENSITIVITIES.size(); i++) {
                if (CONSTRAINT_SENSITIVITIES[i].first->is_enabled() &&
                    std::abs(CONSTRAINT_SENSITIVITIES[i].second) >
                        constant::EPSILON_10) {
                    first_constraint_ptr   = CONSTRAINT_SENSITIVITIES[i].first;
                    first_constraint_sense = first_constraint_ptr->sense();
                    first_constraint_coeff = CONSTRAINT_SENSITIVITIES[i].second;
                    start_idx              = i + 1;
                    break;
                }
            }

            if (first_constraint_ptr == nullptr) {
                continue;
            }

            const bool IS_FIRST_COEFF_POSITIVE = (first_constraint_coeff > 0);

            for (auto i = start_idx; i < CONSTRAINT_SENSITIVITIES.size(); i++) {
                const auto CONSTRAINT_PTR = CONSTRAINT_SENSITIVITIES[i].first;
                if (!CONSTRAINT_PTR->is_enabled()) {
                    continue;
                }

                const auto CONSTRAINT_COEFFICIENT =
                    CONSTRAINT_SENSITIVITIES[i].second;
                if (std::abs(CONSTRAINT_COEFFICIENT) <= constant::EPSILON_10) {
                    continue;
                }

                const auto CONSTRAINT_SENSE = CONSTRAINT_PTR->sense();
                const bool IS_SENSE_SAME =
                    (CONSTRAINT_SENSE == first_constraint_sense);
                const bool IS_COEFF_POSITIVE = (CONSTRAINT_COEFFICIENT > 0);
                const bool IS_COEFFICIENT_SIGN_SAME =
                    (IS_COEFF_POSITIVE == IS_FIRST_COEFF_POSITIVE);

                if (IS_SENSE_SAME != IS_COEFFICIENT_SIGN_SAME) {
                    return false;
                }
            }
        }

        return true;
    }
};

}  // namespace printemps::model_handler

#endif
/*****************************************************************************/
// END
/*****************************************************************************/
