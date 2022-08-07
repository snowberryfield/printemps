/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_OPTION_PREPROCESS_OPTION_H__
#define PRINTEMPS_OPTION_PREPROCESS_OPTION_H__

namespace printemps::option {
/*****************************************************************************/
struct PreprocessOptionConstant {
    static constexpr bool  //
        DEFAULT_IS_ENABLED_PRESOLVE = true;
    static constexpr bool  //
        DEFAULT_IS_ENABLED_REMOVE_DUPLICATED_CONSTRAINTS = true;
    static constexpr bool  //
        DEFAULT_IS_ENABLED_REMOVE_REDUNDANT_SET_VARIABLES = true;
    static constexpr bool  //
        DEFAULT_IS_ENABLED_REMOVE_REDUNDANT_SET_CONSTRAINTS = true;
    static constexpr bool  //
        DEFAULT_IS_ENABLED_EXTRACT_IMPLICIT_EQUALITY_CONSTRAINTS = true;
    static constexpr bool  //
        DEFAULT_IS_ENABLED_ONLINE_BOUNDING = true;
    static constexpr bool  //
        DEFAULT_IS_ENABLED_INITIAL_VALUE_CORRECTION = true;
    static constexpr bool  //
        DEFAULT_IS_ENABLED_EXTRACT_DEPENDENT_EXCLUSIVE_OR = true;
    static constexpr bool  //
        DEFAULT_IS_ENABLED_EXTRACT_DEPENDENT_EXCLUSIVE_NOR = false;
    static constexpr bool  //
        DEFAULT_IS_ENABLED_EXTRACT_DEPENDENT_INVERTED_INTEGERS = true;
    static constexpr bool  //
        DEFAULT_IS_ENABLED_EXTRACT_DEPENDENT_BALANCED_INTEGERS = false;
    static constexpr bool  //
        DEFAULT_IS_ENABLED_EXTRACT_DEPENDENT_CONSTANT_SUM_INTEGERS = true;
    static constexpr bool  //
        DEFAULT_IS_ENABLED_EXTRACT_DEPENDENT_CONSTANT_DIFFERENCE_INTEGERS =
            true;
    static constexpr bool  //
        DEFAULT_IS_ENABLED_EXTRACT_DEPENDENT_CONSTANT_RATIO_INTEGERS = true;
    static constexpr bool  //
        DEFAULT_IS_ENABLED_EXTRACT_DEPENDENT_INTERMEDIATE = true;
};

/*****************************************************************************/
struct PreprocessOption {
    bool is_enabled_presolve;
    bool is_enabled_remove_duplicated_constraints;
    bool is_enabled_remove_redundant_set_variables;
    bool is_enabled_remove_redundant_set_constraints;
    bool is_enabled_extract_implicit_equality_constraints;
    bool is_enabled_online_bounding;
    bool is_enabled_initial_value_correction;
    bool is_enabled_extract_dependent_exclusive_or;
    bool is_enabled_extract_dependent_exclusive_nor;
    bool is_enabled_extract_dependent_inverted_integers;
    bool is_enabled_extract_dependent_balanced_integers;
    bool is_enabled_extract_dependent_constant_sum_integers;
    bool is_enabled_extract_dependent_constant_difference_integers;
    bool is_enabled_extract_dependent_constant_ratio_integers;
    bool is_enabled_extract_dependent_intermediate;

    /*************************************************************************/
    PreprocessOption(void) {
        this->initialize();
    }

    /*************************************************************************/
    PreprocessOption(const utility::json::JsonObject &a_OBJECT) {
        this->setup(a_OBJECT);
    }

    /*************************************************************************/
    void initialize(void) {
        this->is_enabled_presolve =
            PreprocessOptionConstant::DEFAULT_IS_ENABLED_PRESOLVE;

        this->is_enabled_remove_duplicated_constraints =
            PreprocessOptionConstant::
                DEFAULT_IS_ENABLED_REMOVE_DUPLICATED_CONSTRAINTS;

        this->is_enabled_remove_redundant_set_variables =
            PreprocessOptionConstant::
                DEFAULT_IS_ENABLED_REMOVE_REDUNDANT_SET_VARIABLES;

        this->is_enabled_remove_redundant_set_constraints =
            PreprocessOptionConstant::
                DEFAULT_IS_ENABLED_REMOVE_REDUNDANT_SET_CONSTRAINTS;

        this->is_enabled_extract_implicit_equality_constraints =
            PreprocessOptionConstant::
                DEFAULT_IS_ENABLED_EXTRACT_IMPLICIT_EQUALITY_CONSTRAINTS;

        this->is_enabled_online_bounding =
            PreprocessOptionConstant::DEFAULT_IS_ENABLED_ONLINE_BOUNDING;

        this->is_enabled_initial_value_correction = PreprocessOptionConstant::
            DEFAULT_IS_ENABLED_INITIAL_VALUE_CORRECTION;

        this->is_enabled_extract_dependent_exclusive_or =
            PreprocessOptionConstant::
                DEFAULT_IS_ENABLED_EXTRACT_DEPENDENT_EXCLUSIVE_OR;

        this->is_enabled_extract_dependent_exclusive_nor =
            PreprocessOptionConstant::
                DEFAULT_IS_ENABLED_EXTRACT_DEPENDENT_EXCLUSIVE_NOR;

        this->is_enabled_extract_dependent_inverted_integers =
            PreprocessOptionConstant::
                DEFAULT_IS_ENABLED_EXTRACT_DEPENDENT_INVERTED_INTEGERS;

        this->is_enabled_extract_dependent_balanced_integers =
            PreprocessOptionConstant::
                DEFAULT_IS_ENABLED_EXTRACT_DEPENDENT_BALANCED_INTEGERS;

        this->is_enabled_extract_dependent_constant_sum_integers =
            PreprocessOptionConstant::
                DEFAULT_IS_ENABLED_EXTRACT_DEPENDENT_CONSTANT_SUM_INTEGERS;

        this->is_enabled_extract_dependent_constant_difference_integers =
            PreprocessOptionConstant::
                DEFAULT_IS_ENABLED_EXTRACT_DEPENDENT_CONSTANT_DIFFERENCE_INTEGERS;

        this->is_enabled_extract_dependent_constant_ratio_integers =
            PreprocessOptionConstant::
                DEFAULT_IS_ENABLED_EXTRACT_DEPENDENT_CONSTANT_RATIO_INTEGERS;

        this->is_enabled_extract_dependent_intermediate =
            PreprocessOptionConstant::
                DEFAULT_IS_ENABLED_EXTRACT_DEPENDENT_INTERMEDIATE;
    }

    /*************************************************************************/
    void print(void) const {
        utility::print(                               //
            " -- preprocess.is_enabled_presolve: " +  //
            utility::to_true_or_false(                //
                this->is_enabled_presolve));

        utility::print(                                                    //
            " -- preprocess.is_enabled_remove_duplicated_constraints: " +  //
            utility::to_true_or_false(                                     //
                this->is_enabled_remove_duplicated_constraints));

        utility::print(                                                     //
            " -- preprocess.is_enabled_remove_redundant_set_variables: " +  //
            utility::to_true_or_false(                                      //
                this->is_enabled_remove_redundant_set_variables));

        utility::print(                                                       //
            " -- preprocess.is_enabled_remove_redundant_set_constraints: " +  //
            utility::to_true_or_false(                                        //
                this->is_enabled_remove_redundant_set_constraints));

        utility::print(
            " -- "
            "preprocess.is_enabled_extract_implicit_equality_constraints:"
            " " +                       //
            utility::to_true_or_false(  //
                this->is_enabled_extract_implicit_equality_constraints));

        utility::print(                                      //
            " -- preprocess.is_enabled_online_bounding: " +  //
            utility::to_true_or_false(                       //
                this->is_enabled_online_bounding));

        utility::print(                                               //
            " -- preprocess.is_enabled_initial_value_correction: " +  //
            utility::to_true_or_false(                                //
                this->is_enabled_initial_value_correction));

        utility::print(                                                     //
            " -- preprocess.is_enabled_extract_dependent_exclusive_or: " +  //
            utility::to_true_or_false(                                      //
                this->is_enabled_extract_dependent_exclusive_or));

        utility::print(                                                      //
            " -- preprocess.is_enabled_extract_dependent_exclusive_nor: " +  //
            utility::to_true_or_false(                                       //
                this->is_enabled_extract_dependent_exclusive_nor));

        utility::print(  //
            " -- preprocess.is_enabled_extract_dependent_inverted_integers: " +  //
            utility::to_true_or_false(  //
                this->is_enabled_extract_dependent_inverted_integers));

        utility::print(  //
            " -- preprocess.is_enabled_extract_dependent_balanced_integers: " +  //
            utility::to_true_or_false(  //
                this->is_enabled_extract_dependent_balanced_integers));

        utility::print(  //
            " -- "
            "preprocess.is_enabled_extract_dependent_constant_sum_integers:"
            " " +                       //
            utility::to_true_or_false(  //
                this->is_enabled_extract_dependent_constant_sum_integers));

        utility::print(  //
            " -- "
            "preprocess.is_enabled_extract_dependent_constant_difference_"
            "integers: " +              //
            utility::to_true_or_false(  //
                this->is_enabled_extract_dependent_constant_difference_integers));

        utility::print(  //
            " -- "
            "preprocess.is_enabled_extract_dependent_constant_ratio_integers:"
            " " +                       //
            utility::to_true_or_false(  //
                this->is_enabled_extract_dependent_constant_ratio_integers));

        utility::print(                                                     //
            " -- preprocess.is_enabled_extract_dependent_intermediate: " +  //
            utility::to_true_or_false(                                      //
                this->is_enabled_extract_dependent_intermediate));
    }

    /**************************************************************************/
    inline void setup(const utility::json::JsonObject &a_OBJECT) {
        this->initialize();

        read_json(&this->is_enabled_presolve,  //
                  "is_enabled_presolve", a_OBJECT);

        read_json(&this->is_enabled_remove_duplicated_constraints,  //
                  "is_enabled_remove_duplicated_constraints", a_OBJECT);

        read_json(&this->is_enabled_remove_redundant_set_variables,  //
                  "is_enabled_remove_redundant_set_variables", a_OBJECT);

        read_json(&this->is_enabled_remove_redundant_set_constraints,  //
                  "is_enabled_remove_redundant_set_constraints", a_OBJECT);

        read_json(&this->is_enabled_extract_implicit_equality_constraints,  //
                  "is_enabled_extract_implicit_equality_constraint", a_OBJECT);

        read_json(&this->is_enabled_online_bounding,  //
                  "is_enabled_online_bounding", a_OBJECT);

        read_json(&this->is_enabled_initial_value_correction,  //
                  "is_enabled_initial_value_correction", a_OBJECT);

        read_json(&this->is_enabled_extract_dependent_exclusive_or,  //
                  "is_enabled_extract_dependent_exclusive_or", a_OBJECT);

        read_json(&this->is_enabled_extract_dependent_exclusive_nor,  //
                  "is_enabled_extract_dependent_exclusive_nor", a_OBJECT);

        read_json(&this->is_enabled_extract_dependent_inverted_integers,  //
                  "is_enabled_extract_dependent_inverted_integers", a_OBJECT);

        read_json(&this->is_enabled_extract_dependent_balanced_integers,  //
                  "is_enabled_extract_dependent_balanced_integers", a_OBJECT);

        read_json(&this->is_enabled_extract_dependent_constant_sum_integers,  //
                  "is_enabled_extract_dependent_constant_sum_integers",
                  a_OBJECT);

        read_json(
            &this->is_enabled_extract_dependent_constant_difference_integers,  //
            "is_enabled_extract_dependent_constant_difference_integers",
            a_OBJECT);

        read_json(
            &this->is_enabled_extract_dependent_constant_ratio_integers,  //
            "is_enabled_extract_dependent_constant_ratio_integers", a_OBJECT);

        read_json(&this->is_enabled_extract_dependent_intermediate,  //
                  "is_enabled_extract_dependent_intermediate", a_OBJECT);
    }

    /**************************************************************************/
    inline constexpr bool is_enabled_extract_dependent(void) const {
        if (is_enabled_extract_dependent_exclusive_or) {
            return true;
        }
        if (is_enabled_extract_dependent_exclusive_nor) {
            return true;
        }
        if (is_enabled_extract_dependent_inverted_integers) {
            return true;
        }
        if (is_enabled_extract_dependent_balanced_integers) {
            return true;
        }
        if (is_enabled_extract_dependent_constant_sum_integers) {
            return true;
        }
        if (is_enabled_extract_dependent_constant_difference_integers) {
            return true;
        }
        if (is_enabled_extract_dependent_constant_ratio_integers) {
            return true;
        }
        if (is_enabled_extract_dependent_intermediate) {
            return true;
        }
        return false;
    }
};
}  // namespace printemps::option

#endif
/*****************************************************************************/
// END
/*****************************************************************************/