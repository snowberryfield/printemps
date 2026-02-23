/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_PREPROCESS_PROBLEM_SIZE_REDUCER_RESULT_H__
#define PRINTEMPS_PREPROCESS_PROBLEM_SIZE_REDUCER_RESULT_H__

namespace printemps::preprocess {
/*****************************************************************************/
struct ProblemSizeReducerResult {
    int number_of_fixed_variables;
    int number_of_disabled_constraints;
    int number_of_updated_variable_bounds;

    /*************************************************************************/
    ProblemSizeReducerResult(void) {
        this->initialize();
    }

    /*************************************************************************/
    ProblemSizeReducerResult(const int a_NUMBER_OF_FIXED_VARIABLES,
                             const int a_NUMBER_OF_DISABLED_CONSTRAINTS,
                             const int a_NUMBER_OF_UPDATED_VARIABLE_BOUNDS) {
        this->number_of_fixed_variables      = a_NUMBER_OF_FIXED_VARIABLES;
        this->number_of_disabled_constraints = a_NUMBER_OF_DISABLED_CONSTRAINTS;
        this->number_of_updated_variable_bounds =
            a_NUMBER_OF_UPDATED_VARIABLE_BOUNDS;
    }

    /*************************************************************************/
    inline void initialize(void) {
        this->number_of_fixed_variables         = 0;
        this->number_of_disabled_constraints    = 0;
        this->number_of_updated_variable_bounds = 0;
    }

    /*************************************************************************/
    inline void reset(void) {
        this->initialize();
    }

    /*************************************************************************/
    inline ProblemSizeReducerResult operator+=(
        const ProblemSizeReducerResult &a_RESULT) {
        this->number_of_fixed_variables += a_RESULT.number_of_fixed_variables;
        this->number_of_disabled_constraints +=
            a_RESULT.number_of_disabled_constraints;
        this->number_of_updated_variable_bounds +=
            a_RESULT.number_of_updated_variable_bounds;
        return *this;
    }

    /*************************************************************************/
    inline bool is_reduced(void) const {
        if (this->number_of_fixed_variables > 0 ||
            this->number_of_disabled_constraints > 0 ||
            this->number_of_updated_variable_bounds > 0) {
            return true;
        }
        return false;
    }
};
}  // namespace printemps::preprocess
#endif
/*****************************************************************************/
// END
/*****************************************************************************/
