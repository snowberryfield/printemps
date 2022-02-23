/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_PREPROCESS_FLIPPABLE_VARIABLE_PAIR_H__
#define PRINTEMPS_PREPROCESS_FLIPPABLE_VARIABLE_PAIR_H__

namespace printemps {
namespace preprocess {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
struct FlippableVariablePair {
    model_component::Variable<T_Variable, T_Expression> *variable_ptr_first;
    model_component::Variable<T_Variable, T_Expression> *variable_ptr_second;
    int    number_of_common_element;
    double overlap_rate;

    /*************************************************************************/
    FlippableVariablePair(void) {
        this->initialize();
        /// nothing to do
    }

    /*************************************************************************/
    FlippableVariablePair(  //
        model_component::Variable<T_Variable, T_Expression>
            *a_variable_ptr_first,
        model_component::Variable<T_Variable, T_Expression>
            *        a_variable_ptr_second,
        const int    a_NUMBER_OF_COMMON_ELEMENTS,  //
        const double a_OVERLAP_RATE)
        : variable_ptr_first(a_variable_ptr_first),
          variable_ptr_second(a_variable_ptr_second),
          number_of_common_element(a_NUMBER_OF_COMMON_ELEMENTS),
          overlap_rate(a_OVERLAP_RATE) {
        /// nothing to do
    }

    /*************************************************************************/
    void initialize(void) {
        this->variable_ptr_first       = nullptr;
        this->variable_ptr_second      = nullptr;
        this->number_of_common_element = 0;
        this->overlap_rate             = 0;
    }
};
}  // namespace preprocess
}  // namespace printemps
#endif
/*****************************************************************************/
// END
/*****************************************************************************/