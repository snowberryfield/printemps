/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_MODEL_HANDLER_MODEL_PRINTER_H__
#define PRINTEMPS_MODEL_HANDLER_MODEL_PRINTER_H__

namespace printemps::model_handler {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class ModelPrinter {
   private:
    const model::Model<T_Variable, T_Expression> *m_model_ptr;

   public:
    /*************************************************************************/
    ModelPrinter(void) {
        m_model_ptr = nullptr;
    }

    /*************************************************************************/
    ModelPrinter(const model::Model<T_Variable, T_Expression> *a_model_ptr) {
        this->initialize();
        this->setup(a_model_ptr);
    }

    /*************************************************************************/
    inline void initialize(void) noexcept {
        m_model_ptr = nullptr;
    }

    /*************************************************************************/
    inline void setup(
        const model::Model<T_Variable, T_Expression> *a_model_ptr) noexcept {
        m_model_ptr = a_model_ptr;
    }

    /*************************************************************************/
    inline void print_number_of_variables(void) const {
        utility::print_single_line(true);

        const auto &MODEL     = *m_model_ptr;
        const auto &ORIGINAL  = MODEL.reference_original().variable;
        const auto &PRESOLVED = MODEL.reference().variable;

        const auto &ORIGINAL_TYPE  = MODEL.reference_original().variable_type;
        const auto &PRESOLVED_TYPE = MODEL.reference().variable_type;

        std::function<std::size_t(const std::vector<model_component::Variable<
                                      T_Variable, T_Expression> *> &)>
            compute_number_of_all_variables = [](const auto &a_VARIABLE_PTRS) {
                return a_VARIABLE_PTRS.size();
            };

        std::function<std::size_t(const std::vector<model_component::Variable<
                                      T_Variable, T_Expression> *> &)>
            compute_number_of_mutable_variables =
                [](const auto &a_VARIABLE_PTRS) {
                    return std::count_if(a_VARIABLE_PTRS.begin(),
                                         a_VARIABLE_PTRS.end(),
                                         [](const auto *a_VARIABLE_PTR) {
                                             return !a_VARIABLE_PTR->is_fixed();
                                         });
                };

        utility::print_info(  //
            "The number of variables: " +
                utility::to_string(  //
                    compute_number_of_all_variables(
                        ORIGINAL.mutable_variable_ptrs),
                    "%d") +
                utility::to_string(  //
                    compute_number_of_mutable_variables(
                        PRESOLVED.mutable_variable_ptrs),
                    " (%d)"),
            true);

        const struct {
            std::string name;
            const std::vector<
                model_component::Variable<T_Variable, T_Expression> *>
                *original_ptrs_ptr;
            const std::vector<
                model_component::Variable<T_Variable, T_Expression> *>
                *presolved_ptrs_ptr;
        } TABLE[] = {
            {"Binary", &ORIGINAL_TYPE.binary_variable_ptrs,
             &PRESOLVED_TYPE.binary_variable_ptrs},
            {"Integer", &ORIGINAL_TYPE.integer_variable_ptrs,
             &PRESOLVED_TYPE.integer_variable_ptrs},
            {"Selection", &ORIGINAL_TYPE.selection_variable_ptrs,
             &PRESOLVED_TYPE.selection_variable_ptrs},
            {"Dependent Binary", &ORIGINAL_TYPE.dependent_binary_variable_ptrs,
             &PRESOLVED_TYPE.dependent_binary_variable_ptrs},
            {"Dependent Integer",
             &ORIGINAL_TYPE.dependent_integer_variable_ptrs,
             &PRESOLVED_TYPE.dependent_integer_variable_ptrs},

        };

        for (const auto &row : TABLE) {
            utility::print_info(
                " -- " + row.name + ": " +
                    utility::to_string(
                        compute_number_of_all_variables(*row.original_ptrs_ptr),
                        "%d") +
                    utility::to_string(compute_number_of_mutable_variables(
                                           *row.presolved_ptrs_ptr),
                                       " (%d)"),
                true);
        }

        utility::print(  //
            "          ( ) : Number of mutable variables after presolve.",
            true);
    }

    /*************************************************************************/
    inline void print_number_of_constraints(void) const {
        utility::print_single_line(true);

        const auto &MODEL     = *m_model_ptr;
        const auto &ORIGINAL  = MODEL.reference_original().constraint;
        const auto &PRESOLVED = MODEL.reference().constraint;

        const auto &ORIGINAL_TYPE  = MODEL.reference_original().constraint_type;
        const auto &PRESOLVED_TYPE = MODEL.reference().constraint_type;

        std::function<std::size_t(const std::vector<model_component::Constraint<
                                      T_Variable, T_Expression> *> &)>
            compute_number_of_all_constraints =
                [](const auto &a_CONSTRAINT_PTRS) {
                    return a_CONSTRAINT_PTRS.size();
                };

        std::function<std::size_t(const std::vector<model_component::Constraint<
                                      T_Variable, T_Expression> *> &)>
            compute_number_of_enabled_constraints =
                [](const auto &a_CONSTRAINT_PTRS) {
                    return std::count_if(
                        a_CONSTRAINT_PTRS.begin(), a_CONSTRAINT_PTRS.end(),
                        [](const auto *a_CONSTRAINT_PTR) {
                            return a_CONSTRAINT_PTR->is_enabled();
                        });
                };

        utility::print_info(  //
            "The number of constraints: " +
                utility::to_string(  //
                    compute_number_of_all_constraints(
                        ORIGINAL.enabled_constraint_ptrs),
                    "%d") +
                utility::to_string(  //
                    compute_number_of_enabled_constraints(
                        PRESOLVED.enabled_constraint_ptrs),
                    " (%d)"),
            true);

        utility::print_info(  //
            "[<= : " +
                utility::to_string(  //
                    compute_number_of_all_constraints(ORIGINAL.less_ptrs),
                    "%d") +
                utility::to_string(  //
                    compute_number_of_enabled_constraints(PRESOLVED.less_ptrs),
                    " (%d), ") +
                "== : " +            //
                utility::to_string(  //
                    compute_number_of_all_constraints(ORIGINAL.equal_ptrs),
                    "%d") +
                utility::to_string(  //
                    compute_number_of_enabled_constraints(PRESOLVED.equal_ptrs),
                    " (%d), ") +
                ">= : " +            //
                utility::to_string(  //
                    compute_number_of_all_constraints(ORIGINAL.greater_ptrs),
                    "%d") +
                utility::to_string(  //
                    compute_number_of_enabled_constraints(
                        PRESOLVED.greater_ptrs),
                    " (%d)]"),
            true);

        const struct {
            std::string name;
            const std::vector<
                model_component::Constraint<T_Variable, T_Expression> *>
                *original_ptrs_ptr;
            const std::vector<
                model_component::Constraint<T_Variable, T_Expression> *>
                *presolved_ptrs_ptr;
        } TABLE[] = {
            {"Empty", &ORIGINAL_TYPE.empty_ptrs, &PRESOLVED_TYPE.empty_ptrs},
            {"Singleton", &ORIGINAL_TYPE.singleton_ptrs,
             &PRESOLVED_TYPE.singleton_ptrs},
            {"Exclusive OR", &ORIGINAL_TYPE.exclusive_or_ptrs,
             &PRESOLVED_TYPE.exclusive_or_ptrs},
            {"Exclusive NOR", &ORIGINAL_TYPE.exclusive_nor_ptrs,
             &PRESOLVED_TYPE.exclusive_nor_ptrs},
            {"Inverted Integers", &ORIGINAL_TYPE.inverted_integers_ptrs,
             &PRESOLVED_TYPE.inverted_integers_ptrs},
            {"Balanced Integers", &ORIGINAL_TYPE.balanced_integers_ptrs,
             &PRESOLVED_TYPE.balanced_integers_ptrs},
            {"Constant Sum Integers", &ORIGINAL_TYPE.constant_sum_integers_ptrs,
             &PRESOLVED_TYPE.constant_sum_integers_ptrs},
            {"Constant Difference Integers",
             &ORIGINAL_TYPE.constant_difference_integers_ptrs,
             &PRESOLVED_TYPE.constant_difference_integers_ptrs},
            {"Constant Ratio Integers",
             &ORIGINAL_TYPE.constant_ratio_integers_ptrs,
             &PRESOLVED_TYPE.constant_ratio_integers_ptrs},
            {"Aggregation", &ORIGINAL_TYPE.aggregation_ptrs,
             &PRESOLVED_TYPE.aggregation_ptrs},
            {"Precedence", &ORIGINAL_TYPE.precedence_ptrs,
             &PRESOLVED_TYPE.precedence_ptrs},
            {"Variable Bound", &ORIGINAL_TYPE.variable_bound_ptrs,
             &PRESOLVED_TYPE.variable_bound_ptrs},
            {"Trinomial Exclusive NOR",
             &ORIGINAL_TYPE.trinomial_exclusive_nor_ptrs,
             &PRESOLVED_TYPE.trinomial_exclusive_nor_ptrs},
            {"All Or Nothing", &ORIGINAL_TYPE.all_or_nothing_ptrs,
             &PRESOLVED_TYPE.all_or_nothing_ptrs},
            {"Set Partitioning", &ORIGINAL_TYPE.set_partitioning_ptrs,
             &PRESOLVED_TYPE.set_partitioning_ptrs},
            {"Set Packing", &ORIGINAL_TYPE.set_packing_ptrs,
             &PRESOLVED_TYPE.set_packing_ptrs},
            {"Set Covering", &ORIGINAL_TYPE.set_covering_ptrs,
             &PRESOLVED_TYPE.set_covering_ptrs},
            {"Cardinality", &ORIGINAL_TYPE.cardinality_ptrs,
             &PRESOLVED_TYPE.cardinality_ptrs},
            {"Invariant Knapsack", &ORIGINAL_TYPE.invariant_knapsack_ptrs,
             &PRESOLVED_TYPE.invariant_knapsack_ptrs},
            {"Multiple Covering", &ORIGINAL_TYPE.multiple_covering_ptrs,
             &PRESOLVED_TYPE.multiple_covering_ptrs},
            {"Binary Flow", &ORIGINAL_TYPE.binary_flow_ptrs,
             &PRESOLVED_TYPE.binary_flow_ptrs},
            {"Integer Flow", &ORIGINAL_TYPE.integer_flow_ptrs,
             &PRESOLVED_TYPE.integer_flow_ptrs},
            {"Soft Selection", &ORIGINAL_TYPE.soft_selection_ptrs,
             &PRESOLVED_TYPE.soft_selection_ptrs},
            {"Min-Max", &ORIGINAL_TYPE.min_max_ptrs,
             &PRESOLVED_TYPE.min_max_ptrs},
            {"Max-Min", &ORIGINAL_TYPE.max_min_ptrs,
             &PRESOLVED_TYPE.max_min_ptrs},
            {"Intermediate", &ORIGINAL_TYPE.intermediate_ptrs,
             &PRESOLVED_TYPE.intermediate_ptrs},
            {"Equation Knapsack", &ORIGINAL_TYPE.equation_knapsack_ptrs,
             &PRESOLVED_TYPE.equation_knapsack_ptrs},
            {"Bin Packing", &ORIGINAL_TYPE.bin_packing_ptrs,
             &PRESOLVED_TYPE.bin_packing_ptrs},
            {"Knapsack", &ORIGINAL_TYPE.knapsack_ptrs,
             &PRESOLVED_TYPE.knapsack_ptrs},
            {"Integer Knapsack", &ORIGINAL_TYPE.integer_knapsack_ptrs,
             &PRESOLVED_TYPE.integer_knapsack_ptrs},
            {"GF(2)", &ORIGINAL_TYPE.gf2_ptrs, &PRESOLVED_TYPE.gf2_ptrs},
            {"General Linear", &ORIGINAL_TYPE.general_linear_ptrs,
             &PRESOLVED_TYPE.general_linear_ptrs},
        };

        for (const auto &row : TABLE) {
            utility::print_info(
                " -- " + row.name + ": " +
                    utility::to_string(compute_number_of_all_constraints(
                                           *row.original_ptrs_ptr),
                                       "%d") +
                    utility::to_string(compute_number_of_enabled_constraints(
                                           *row.presolved_ptrs_ptr),
                                       " (%d)"),
                true);
        }

        utility::print(  //
            "          ( ) : Number of enabled constraints after presolve.",
            true);
    }
};
}  // namespace printemps::model_handler
#endif
/*****************************************************************************/
// END
/*****************************************************************************/