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
        const auto &PRESOLVED_TYPE =  //
            MODEL.reference().variable_type;

        auto compute_number_of_variables = [](const auto &a_VARIABLE_PTRS) {
            return a_VARIABLE_PTRS.size();
        };

        auto compute_number_of_mutable_variables =
            [](const auto &a_VARIABLE_PTRS) {
                return std::count_if(a_VARIABLE_PTRS.begin(),
                                     a_VARIABLE_PTRS.end(),
                                     [](const auto *a_VARIABLE_PTR) {
                                         return !a_VARIABLE_PTR->is_fixed();
                                     });
            };

        utility::print_info(  //
            "The number of variables: " +
                utility::to_string(               //
                    compute_number_of_variables(  //
                        ORIGINAL.variable_ptrs),
                    "%d") +
                " (" +
                utility::to_string(                       //
                    compute_number_of_mutable_variables(  //
                        PRESOLVED.variable_ptrs),
                    "%d") +
                ")",
            true);

        utility::print_info(  //
            " -- Binary: " +
                utility::to_string(               //
                    compute_number_of_variables(  //
                        ORIGINAL_TYPE.binary_variable_ptrs),
                    "%d") +
                " (" +
                utility::to_string(                       //
                    compute_number_of_mutable_variables(  //
                        PRESOLVED_TYPE.binary_variable_ptrs),
                    "%d") +
                ")",
            true);

        utility::print_info(  //
            " -- Integer: " +
                utility::to_string(               //
                    compute_number_of_variables(  //
                        ORIGINAL_TYPE.integer_variable_ptrs),
                    "%d") +
                " (" +
                utility::to_string(  //
                    compute_number_of_mutable_variables(
                        PRESOLVED_TYPE.integer_variable_ptrs),
                    "%d") +
                ")",
            true);

        utility::print_info(  //
            " -- Selection: " +
                utility::to_string(  //
                    compute_number_of_variables(
                        ORIGINAL_TYPE.selection_variable_ptrs),
                    "%d") +
                " (" +
                utility::to_string(  //
                    compute_number_of_mutable_variables(
                        PRESOLVED_TYPE.selection_variable_ptrs),
                    "%d") +
                ")",
            true);

        utility::print_info(  //
            " -- Dependent Binary: " +
                utility::to_string(  //
                    compute_number_of_variables(
                        ORIGINAL_TYPE.dependent_binary_variable_ptrs),
                    "%d") +
                " (" +
                utility::to_string(  //
                    compute_number_of_mutable_variables(
                        PRESOLVED_TYPE.dependent_binary_variable_ptrs),
                    "%d") +
                ")",
            true);

        utility::print_info(  //
            " -- Dependent Integer: " +
                utility::to_string(  //
                    compute_number_of_variables(
                        ORIGINAL_TYPE.dependent_integer_variable_ptrs),
                    "%d") +
                " (" +
                utility::to_string(  //
                    compute_number_of_mutable_variables(
                        PRESOLVED_TYPE.dependent_integer_variable_ptrs),
                    "%d") +
                ")",
            true);

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
        const auto &PRESOLVED_TYPE =  //
            MODEL.reference().constraint_type;

        auto compute_number_of_constraints = [](const auto &a_CONSTRAINT_PTRS) {
            return a_CONSTRAINT_PTRS.size();
        };

        auto compute_number_of_enabled_constraints =
            [](const auto &a_CONSTRAINT_PTRS) {
                return std::count_if(a_CONSTRAINT_PTRS.begin(),
                                     a_CONSTRAINT_PTRS.end(),
                                     [](const auto *a_CONSTRAINT_PTR) {
                                         return a_CONSTRAINT_PTR->is_enabled();
                                     });
            };

        utility::print_info(  //
            "The number of constraints: " +
                utility::to_string(                 //
                    compute_number_of_constraints(  //
                        ORIGINAL.constraint_ptrs),
                    "%d") +
                " (" +
                utility::to_string(                         //
                    compute_number_of_enabled_constraints(  //
                        PRESOLVED.constraint_ptrs),
                    "%d") +
                ")",
            true);

        utility::print_info(                        //
            "[<= : " +                              //
                utility::to_string(                 //
                    compute_number_of_constraints(  //
                        ORIGINAL.less_ptrs),
                    "%d") +
                " (" +
                utility::to_string(                         //
                    compute_number_of_enabled_constraints(  //
                        PRESOLVED.less_ptrs),
                    "%d") +
                "), " + "== : " +                   //
                utility::to_string(                 //
                    compute_number_of_constraints(  //
                        ORIGINAL.equal_ptrs),
                    "%d") +
                " (" +
                utility::to_string(                         //
                    compute_number_of_enabled_constraints(  //
                        PRESOLVED.equal_ptrs),
                    "%d") +
                "), " + ">= : " +                   //
                utility::to_string(                 //
                    compute_number_of_constraints(  //
                        ORIGINAL.greater_ptrs),
                    "%d") +
                " (" +
                utility::to_string(                         //
                    compute_number_of_enabled_constraints(  //
                        PRESOLVED.greater_ptrs),
                    "%d") +
                ")]",
            true);

        utility::print_info(                        //
            " -- Singleton: " +                     //
                utility::to_string(                 //
                    compute_number_of_constraints(  //
                        ORIGINAL_TYPE.singleton_ptrs),
                    "%d") +
                " (" +
                utility::to_string(                         //
                    compute_number_of_enabled_constraints(  //
                        PRESOLVED_TYPE.singleton_ptrs),
                    "%d") +
                ")",
            true);

        utility::print_info(                        //
            " -- Exclusive OR: " +                  //
                utility::to_string(                 //
                    compute_number_of_constraints(  //
                        ORIGINAL_TYPE.exclusive_or_ptrs),
                    "%d") +
                " (" +
                utility::to_string(                         //
                    compute_number_of_enabled_constraints(  //
                        PRESOLVED_TYPE.exclusive_or_ptrs),
                    "%d") +
                ")",
            true);

        utility::print_info(                        //
            " -- Exclusive NOR: " +                 //
                utility::to_string(                 //
                    compute_number_of_constraints(  //
                        ORIGINAL_TYPE.exclusive_nor_ptrs),
                    "%d") +
                " (" +
                utility::to_string(                         //
                    compute_number_of_enabled_constraints(  //
                        PRESOLVED_TYPE.exclusive_nor_ptrs),
                    "%d") +
                ")",
            true);

        utility::print_info(                        //
            " -- Inverted Integers: " +             //
                utility::to_string(                 //
                    compute_number_of_constraints(  //
                        ORIGINAL_TYPE.inverted_integers_ptrs),
                    "%d") +
                " (" +
                utility::to_string(                         //
                    compute_number_of_enabled_constraints(  //
                        PRESOLVED_TYPE.inverted_integers_ptrs),
                    "%d") +
                ")",
            true);

        utility::print_info(                        //
            " -- Balanced Integers: " +             //
                utility::to_string(                 //
                    compute_number_of_constraints(  //
                        ORIGINAL_TYPE.balanced_integers_ptrs),
                    "%d") +
                " (" +
                utility::to_string(                         //
                    compute_number_of_enabled_constraints(  //
                        PRESOLVED_TYPE.balanced_integers_ptrs),
                    "%d") +
                ")",
            true);

        utility::print_info(                        //
            " -- Constant Sum Integers: " +         //
                utility::to_string(                 //
                    compute_number_of_constraints(  //
                        ORIGINAL_TYPE.constant_sum_integers_ptrs),
                    "%d") +
                " (" +
                utility::to_string(                         //
                    compute_number_of_enabled_constraints(  //
                        PRESOLVED_TYPE.constant_sum_integers_ptrs),
                    "%d") +
                ")",
            true);

        utility::print_info(                        //
            " -- Constant Difference Integers: " +  //
                utility::to_string(                 //
                    compute_number_of_constraints(  //
                        ORIGINAL_TYPE.constant_difference_integers_ptrs),
                    "%d") +
                " (" +
                utility::to_string(                         //
                    compute_number_of_enabled_constraints(  //
                        PRESOLVED_TYPE.constant_difference_integers_ptrs),
                    "%d") +
                ")",
            true);

        utility::print_info(                        //
            " -- Constant Ratio Integers: " +       //
                utility::to_string(                 //
                    compute_number_of_constraints(  //
                        ORIGINAL_TYPE.constant_ratio_integers_ptrs),
                    "%d") +
                " (" +
                utility::to_string(                         //
                    compute_number_of_enabled_constraints(  //
                        PRESOLVED_TYPE.constant_ratio_integers_ptrs),
                    "%d") +
                ")",
            true);

        utility::print_info(                        //
            " -- Aggregation: " +                   //
                utility::to_string(                 //
                    compute_number_of_constraints(  //
                        ORIGINAL_TYPE.aggregation_ptrs),
                    "%d") +
                " (" +
                utility::to_string(                         //
                    compute_number_of_enabled_constraints(  //
                        PRESOLVED_TYPE.aggregation_ptrs),
                    "%d") +
                ")",
            true);

        utility::print_info(                        //
            " -- Precedence: " +                    //
                utility::to_string(                 //
                    compute_number_of_constraints(  //
                        ORIGINAL_TYPE.precedence_ptrs),
                    "%d") +
                " (" +
                utility::to_string(                         //
                    compute_number_of_enabled_constraints(  //
                        PRESOLVED_TYPE.precedence_ptrs),
                    "%d") +
                ")",
            true);

        utility::print_info(                        //
            " -- Variable Bound: " +                //
                utility::to_string(                 //
                    compute_number_of_constraints(  //
                        ORIGINAL_TYPE.variable_bound_ptrs),
                    "%d") +
                " (" +
                utility::to_string(                         //
                    compute_number_of_enabled_constraints(  //
                        PRESOLVED_TYPE.variable_bound_ptrs),
                    "%d") +
                ")",
            true);

        utility::print_info(                        //
            " -- Trinomial Exclusive NOR: " +       //
                utility::to_string(                 //
                    compute_number_of_constraints(  //
                        ORIGINAL_TYPE.trinomial_exclusive_nor_ptrs),
                    "%d") +
                " (" +
                utility::to_string(                         //
                    compute_number_of_enabled_constraints(  //
                        PRESOLVED_TYPE.trinomial_exclusive_nor_ptrs),
                    "%d") +
                ")",
            true);

        utility::print_info(                        //
            " -- Set Partitioning: " +              //
                utility::to_string(                 //
                    compute_number_of_constraints(  //
                        ORIGINAL_TYPE.set_partitioning_ptrs),
                    "%d") +
                " (" +
                utility::to_string(                         //
                    compute_number_of_enabled_constraints(  //
                        PRESOLVED_TYPE.set_partitioning_ptrs),
                    "%d") +
                ")",
            true);

        utility::print_info(                        //
            " -- Set Packing: " +                   //
                utility::to_string(                 //
                    compute_number_of_constraints(  //
                        ORIGINAL_TYPE.set_packing_ptrs),
                    "%d") +
                " (" +
                utility::to_string(                         //
                    compute_number_of_enabled_constraints(  //
                        PRESOLVED_TYPE.set_packing_ptrs),
                    "%d") +
                ")",
            true);

        utility::print_info(                        //
            " -- Set Covering: " +                  //
                utility::to_string(                 //
                    compute_number_of_constraints(  //
                        ORIGINAL_TYPE.set_covering_ptrs),
                    "%d") +
                " (" +
                utility::to_string(                         //
                    compute_number_of_enabled_constraints(  //
                        PRESOLVED_TYPE.set_covering_ptrs),
                    "%d") +
                ")",
            true);

        utility::print_info(                        //
            " -- Cardinality: " +                   //
                utility::to_string(                 //
                    compute_number_of_constraints(  //
                        ORIGINAL_TYPE.cardinality_ptrs),
                    "%d") +
                " (" +
                utility::to_string(                         //
                    compute_number_of_enabled_constraints(  //
                        PRESOLVED_TYPE.cardinality_ptrs),
                    "%d") +
                ")",
            true);

        utility::print_info(                        //
            " -- Invariant Knapsack: " +            //
                utility::to_string(                 //
                    compute_number_of_constraints(  //
                        ORIGINAL_TYPE.invariant_knapsack_ptrs),
                    "%d") +
                " (" +
                utility::to_string(                         //
                    compute_number_of_enabled_constraints(  //
                        PRESOLVED_TYPE.invariant_knapsack_ptrs),
                    "%d") +
                ")",
            true);

        utility::print_info(                        //
            " -- Multiple Covering: " +             //
                utility::to_string(                 //
                    compute_number_of_constraints(  //
                        ORIGINAL_TYPE.multiple_covering_ptrs),
                    "%d") +
                " (" +
                utility::to_string(                         //
                    compute_number_of_enabled_constraints(  //
                        PRESOLVED_TYPE.multiple_covering_ptrs),
                    "%d") +
                ")",
            true);

        utility::print_info(                        //
            " -- Binary Flow: " +                   //
                utility::to_string(                 //
                    compute_number_of_constraints(  //
                        ORIGINAL_TYPE.binary_flow_ptrs),
                    "%d") +
                " (" +
                utility::to_string(                         //
                    compute_number_of_enabled_constraints(  //
                        PRESOLVED_TYPE.binary_flow_ptrs),
                    "%d") +
                ")",
            true);

        utility::print_info(                        //
            " -- Integer Flow: " +                  //
                utility::to_string(                 //
                    compute_number_of_constraints(  //
                        ORIGINAL_TYPE.integer_flow_ptrs),
                    "%d") +
                " (" +
                utility::to_string(                         //
                    compute_number_of_enabled_constraints(  //
                        PRESOLVED_TYPE.integer_flow_ptrs),
                    "%d") +
                ")",
            true);

        utility::print_info(                        //
            " -- Soft Selection: " +                //
                utility::to_string(                 //
                    compute_number_of_constraints(  //
                        ORIGINAL_TYPE.soft_selection_ptrs),
                    "%d") +
                " (" +
                utility::to_string(                         //
                    compute_number_of_enabled_constraints(  //
                        PRESOLVED_TYPE.soft_selection_ptrs),
                    "%d") +
                ")",
            true);

        utility::print_info(                        //
            " -- Min-Max: " +                       //
                utility::to_string(                 //
                    compute_number_of_constraints(  //
                        ORIGINAL_TYPE.min_max_ptrs),
                    "%d") +
                " (" +
                utility::to_string(                         //
                    compute_number_of_enabled_constraints(  //
                        PRESOLVED_TYPE.min_max_ptrs),
                    "%d") +
                ")",
            true);

        utility::print_info(                        //
            " -- Max-Min: " +                       //
                utility::to_string(                 //
                    compute_number_of_constraints(  //
                        ORIGINAL_TYPE.max_min_ptrs),
                    "%d") +
                " (" +
                utility::to_string(                         //
                    compute_number_of_enabled_constraints(  //
                        PRESOLVED_TYPE.max_min_ptrs),
                    "%d") +
                ")",
            true);

        utility::print_info(                        //
            " -- Intermediate: " +                  //
                utility::to_string(                 //
                    compute_number_of_constraints(  //
                        ORIGINAL_TYPE.intermediate_ptrs),
                    "%d") +
                " (" +
                utility::to_string(                         //
                    compute_number_of_enabled_constraints(  //
                        PRESOLVED_TYPE.intermediate_ptrs),
                    "%d") +
                ")",
            true);

        utility::print_info(                        //
            " -- Equation Knapsack: " +             //
                utility::to_string(                 //
                    compute_number_of_constraints(  //
                        ORIGINAL_TYPE.equation_knapsack_ptrs),
                    "%d") +
                " (" +
                utility::to_string(                         //
                    compute_number_of_enabled_constraints(  //
                        PRESOLVED_TYPE.equation_knapsack_ptrs),
                    "%d") +
                ")",
            true);

        utility::print_info(                        //
            " -- Bin Packing: " +                   //
                utility::to_string(                 //
                    compute_number_of_constraints(  //
                        ORIGINAL_TYPE.bin_packing_ptrs),
                    "%d") +
                " (" +
                utility::to_string(                         //
                    compute_number_of_enabled_constraints(  //
                        PRESOLVED_TYPE.bin_packing_ptrs),
                    "%d") +
                ")",
            true);

        utility::print_info(                        //
            " -- Knapsack: " +                      //
                utility::to_string(                 //
                    compute_number_of_constraints(  //
                        ORIGINAL_TYPE.knapsack_ptrs),
                    "%d") +
                " (" +
                utility::to_string(                         //
                    compute_number_of_enabled_constraints(  //
                        PRESOLVED_TYPE.knapsack_ptrs),
                    "%d") +
                ")",
            true);

        utility::print_info(                        //
            " -- Integer Knapsack: " +              //
                utility::to_string(                 //
                    compute_number_of_constraints(  //
                        ORIGINAL_TYPE.integer_knapsack_ptrs),
                    "%d") +
                " (" +
                utility::to_string(                         //
                    compute_number_of_enabled_constraints(  //
                        PRESOLVED_TYPE.integer_knapsack_ptrs),
                    "%d") +
                ")",
            true);

        utility::print_info(                        //
            " -- GF(2): " +                         //
                utility::to_string(                 //
                    compute_number_of_constraints(  //
                        ORIGINAL_TYPE.gf2_ptrs),
                    "%d") +
                " (" +
                utility::to_string(                         //
                    compute_number_of_enabled_constraints(  //
                        PRESOLVED_TYPE.gf2_ptrs),
                    "%d") +
                ")",
            true);

        utility::print_info(                        //
            " -- General Linear: " +                //
                utility::to_string(                 //
                    compute_number_of_constraints(  //
                        ORIGINAL_TYPE.general_linear_ptrs),
                    "%d") +
                " (" +
                utility::to_string(                         //
                    compute_number_of_enabled_constraints(  //
                        PRESOLVED_TYPE.general_linear_ptrs),
                    "%d") +
                ")",
            true);

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