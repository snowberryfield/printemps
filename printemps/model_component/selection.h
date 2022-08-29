/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_MODEL_COMPONENT_SELECTION_H__
#define PRINTEMPS_MODEL_COMPONENT_SELECTION_H__

namespace printemps::model_component {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class Variable;

/*****************************************************************************/
template <class T_Variable, class T_Expression>
class Constraint;

/*****************************************************************************/
template <class T_Variable, class T_Expression>
struct Selection {
    std::vector<Variable<T_Variable, T_Expression> *> variable_ptrs;
    Variable<T_Variable, T_Expression> *              selected_variable_ptr;
    Constraint<T_Variable, T_Expression> *            constraint_ptr;

    std::unordered_set<Constraint<T_Variable, T_Expression> *>
        related_constraint_ptrs_set;
    std::vector<Constraint<T_Variable, T_Expression> *>
        related_constraint_ptrs_vector;

    /*************************************************************************/
    Selection(void) {
        this->initialize();
    }

    /*************************************************************************/
    Selection(Constraint<T_Variable, T_Expression> *a_constraint_ptr) {
        this->setup(a_constraint_ptr);
    }

    /*************************************************************************/
    void initialize(void) {
        this->variable_ptrs.clear();
        this->selected_variable_ptr = nullptr;
        this->constraint_ptr        = nullptr;
        this->related_constraint_ptrs_set.clear();
        this->related_constraint_ptrs_vector.clear();
    }
    /*************************************************************************/
    void setup(Constraint<T_Variable, T_Expression> *a_constraint_ptr) {
        this->initialize();
        this->constraint_ptr = a_constraint_ptr;

        for (const auto &sensitivity :
             constraint_ptr->expression().sensitivities()) {
            this->variable_ptrs.push_back(sensitivity.first);
        }
    }
    /*************************************************************************/
    void setup_related_constraint_ptrs(void) {
        /**
         * NOTE: The following procedure is intentionally excluded from setup().
         * It is expensive and should be called only when necessary.
         */
        this->related_constraint_ptrs_set.clear();
        this->related_constraint_ptrs_vector.clear();

        /**
         * NOTE: The member related_constraint_ptrs must include pointers to
         * disabled constraints because This information will be used to update
         * the disabled constraint values in model::Model.update().
         */
        for (auto &&variable_ptr : this->variable_ptrs) {
            for (auto &&constraint_ptr :
                 variable_ptr->related_constraint_ptrs()) {
                this->related_constraint_ptrs_set.insert(constraint_ptr);
            }
        }

        /**
         * NOTE: The following process is to improve the efficiency to access
         * moves' related constraint pointers in model::Model.evaluate().Broadly
         * stated, when the pointers of constraints are expressed as arrays,
         * heuristic reordering is performed to reduce the difference between
         * the minimum and maximum indexes of the constraints to which the
         * variables to be written have sensitivity.
         */

        std::sort(this->variable_ptrs.begin(), this->variable_ptrs.end(),
                  [](const auto &a_FIRST, const auto &a_SECOND) {
                      return a_FIRST->name() < a_SECOND->name();
                  });

        std::stable_sort(this->variable_ptrs.begin(), this->variable_ptrs.end(),
                         [](const auto &a_FIRST, const auto &a_SECOND) {
                             return a_FIRST->related_constraint_ptrs().size() >
                                    a_SECOND->related_constraint_ptrs().size();
                         });

        std::unordered_set<Constraint<T_Variable, T_Expression> *>
                  inserted_constraint_ptrs;
        const int VARIABLES_SIZE = this->variable_ptrs.size();

        for (auto i = 0; i < VARIABLES_SIZE; i++) {
            auto &variable_ptr = this->variable_ptrs[i];
            std::vector<Constraint<T_Variable, T_Expression> *> constraint_ptrs(
                variable_ptr->related_constraint_ptrs().begin(),
                variable_ptr->related_constraint_ptrs().end());

            std::sort(constraint_ptrs.begin(), constraint_ptrs.end(),
                      [](const auto &a_FIRST, const auto &a_SECOND) {
                          return a_FIRST->name() < a_SECOND->name();
                      });

            if (this->related_constraint_ptrs_vector.size() <
                this->related_constraint_ptrs_set.size() / 2) {
                std::stable_sort(
                    constraint_ptrs.begin(), constraint_ptrs.end(),
                    [](const auto &a_FIRST, const auto &a_SECOND) {
                        return a_FIRST->expression().sensitivities().size() <
                               a_SECOND->expression().sensitivities().size();
                    });
            } else {
                std::stable_sort(
                    constraint_ptrs.begin(), constraint_ptrs.end(),
                    [](const auto &a_FIRST, const auto &a_SECOND) {
                        return a_FIRST->expression().sensitivities().size() >
                               a_SECOND->expression().sensitivities().size();
                    });
            }
            for (auto &&constraint_ptr : constraint_ptrs) {
                if (constraint_ptr == this->constraint_ptr) {
                    continue;
                }
                if (inserted_constraint_ptrs.find(constraint_ptr) ==
                    inserted_constraint_ptrs.end()) {
                    inserted_constraint_ptrs.insert(constraint_ptr);
                    this->related_constraint_ptrs_vector.push_back(
                        constraint_ptr);
                }
            }
        }
    }
};
}  // namespace printemps::model_component
#endif
/*****************************************************************************/
// END
/*****************************************************************************/