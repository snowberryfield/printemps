/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_MODEL_COMPONENT_CONSTRAINT_REFERENCE_H__
#define PRINTEMPS_MODEL_COMPONENT_CONSTRAINT_REFERENCE_H__

namespace printemps {
namespace model_component {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class Constraint;

/*****************************************************************************/
template <class T_Variable, class T_Expression>
struct ConstraintReference {
    std::vector<Constraint<T_Variable, T_Expression> *> constraint_ptrs;
    std::vector<Constraint<T_Variable, T_Expression> *> enabled_constraint_ptrs;
    std::vector<Constraint<T_Variable, T_Expression> *>
        disabled_constraint_ptrs;

    /*************************************************************************/
    ConstraintReference(void) {
        this->initialize();
    }

    /*************************************************************************/
    void initialize(void) {
        this->constraint_ptrs.clear();
        this->enabled_constraint_ptrs.clear();
        this->disabled_constraint_ptrs.clear();
    }
};
}  // namespace model_component
}  // namespace printemps
#endif
/*****************************************************************************/
// END
/*****************************************************************************/