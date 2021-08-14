/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_MODEL_COMPONENT_CONSTRAINT_TYPE_REFERENCE_H__
#define PRINTEMPS_MODEL_COMPONENT_CONSTRAINT_TYPE_REFERENCE_H__

namespace printemps {
namespace model_component {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class Constraint;

/*****************************************************************************/
template <class T_Variable, class T_Expression>
struct ConstraintTypeReference {
    std::vector<Constraint<T_Variable, T_Expression> *> singleton_ptrs;
    std::vector<Constraint<T_Variable, T_Expression> *> aggregation_ptrs;
    std::vector<Constraint<T_Variable, T_Expression> *> precedence_ptrs;
    std::vector<Constraint<T_Variable, T_Expression> *> variable_bound_ptrs;
    std::vector<Constraint<T_Variable, T_Expression> *> set_partitioning_ptrs;
    std::vector<Constraint<T_Variable, T_Expression> *> set_packing_ptrs;
    std::vector<Constraint<T_Variable, T_Expression> *> set_covering_ptrs;
    std::vector<Constraint<T_Variable, T_Expression> *> cardinality_ptrs;
    std::vector<Constraint<T_Variable, T_Expression> *> invariant_knapsack_ptrs;
    std::vector<Constraint<T_Variable, T_Expression> *> equation_knapsack_ptrs;
    std::vector<Constraint<T_Variable, T_Expression> *> bin_packing_ptrs;
    std::vector<Constraint<T_Variable, T_Expression> *> knapsack_ptrs;
    std::vector<Constraint<T_Variable, T_Expression> *> integer_knapsack_ptrs;
    std::vector<Constraint<T_Variable, T_Expression> *> min_max_ptrs;
    std::vector<Constraint<T_Variable, T_Expression> *> max_min_ptrs;
    std::vector<Constraint<T_Variable, T_Expression> *> intermediate_ptrs;
    std::vector<Constraint<T_Variable, T_Expression> *> gf2_ptrs;
    std::vector<Constraint<T_Variable, T_Expression> *> general_linear_ptrs;
    std::vector<Constraint<T_Variable, T_Expression> *> nonlinear_ptrs;

    /*************************************************************************/
    ConstraintTypeReference(void) {
        this->initialize();
    }

    /*************************************************************************/
    void initialize(void) {
        this->singleton_ptrs.clear();
        this->aggregation_ptrs.clear();
        this->precedence_ptrs.clear();
        this->variable_bound_ptrs.clear();
        this->set_partitioning_ptrs.clear();
        this->set_packing_ptrs.clear();
        this->set_covering_ptrs.clear();
        this->cardinality_ptrs.clear();
        this->invariant_knapsack_ptrs.clear();
        this->equation_knapsack_ptrs.clear();
        this->bin_packing_ptrs.clear();
        this->knapsack_ptrs.clear();
        this->integer_knapsack_ptrs.clear();
        this->min_max_ptrs.clear();
        this->max_min_ptrs.clear();
        this->intermediate_ptrs.clear();
        this->gf2_ptrs.clear();
        this->general_linear_ptrs.clear();
        this->nonlinear_ptrs.clear();
    }
};
}  // namespace model_component
}  // namespace printemps
#endif
/*****************************************************************************/
// END
/*****************************************************************************/