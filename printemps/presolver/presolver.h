/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_PRESOLVER_PRESOLVER_H__
#define PRINTEMPS_PRESOLVER_PRESOLVER_H__

namespace printemps {
namespace model {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class Model;
}  // namespace model
}  // namespace printemps

#include "variable_fixer.h"
#include "constraint_remover.h"
#include "selection_extractor.h"
#include "intermediate_variable_extractor.h"

namespace printemps {
namespace presolver {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
constexpr bool presolve(model::Model<T_Variable, T_Expression> *a_model_ptr,  //
                        const bool a_IS_ENABLED_FIX_REDUNDANT_VARIABLES,
                        const bool a_IS_ENABLED_PRINT) {
    utility::print_single_line(a_IS_ENABLED_PRINT);
    utility::print_message("Presolving...", a_IS_ENABLED_PRINT);

    int number_of_disabled_constaints = 0;
    int number_of_fixed_variables     = 0;

    if (a_model_ptr->is_linear()) {
        number_of_fixed_variables +=
            remove_independent_variables(a_model_ptr, a_IS_ENABLED_PRINT);
    }

    while (true) {
        int number_of_newly_disabled_constaints  //
            = remove_redundant_constraints_with_tightening_variable_bounds(
                a_model_ptr, a_IS_ENABLED_PRINT);

        int number_of_newly_fixed_variables  //
            = fix_implicit_fixed_variables(a_model_ptr, a_IS_ENABLED_PRINT);

        number_of_disabled_constaints += number_of_newly_disabled_constaints;
        number_of_fixed_variables += number_of_newly_fixed_variables;

        if (number_of_newly_disabled_constaints == 0 &&
            number_of_newly_fixed_variables == 0) {
            break;
        }
    }

    /**
     * Since fix_redundant_variables() is expensive, it will be enabled if the
     * number of decision variables is equal to or less than the constant
     * MAX_CONSIDERABLE_NUMBER_OF_VARIABLES.
     */
    const int MAX_CONSIDERABLE_NUMBER_OF_VARIABLES = 100000;
    if (a_model_ptr->is_linear() && a_IS_ENABLED_FIX_REDUNDANT_VARIABLES &&
        a_model_ptr->number_of_variables() <=
            MAX_CONSIDERABLE_NUMBER_OF_VARIABLES) {
        number_of_fixed_variables +=
            fix_redundant_variables(a_model_ptr, a_IS_ENABLED_PRINT);
    }
    utility::print_message("Done.", a_IS_ENABLED_PRINT);

    return (number_of_disabled_constaints > 0) ||
           (number_of_fixed_variables > 0);
}

}  // namespace presolver
}  // namespace printemps
#endif
/*****************************************************************************/
// END
/*****************************************************************************/