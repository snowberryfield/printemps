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
    static constexpr bool DEFAULT_IS_ENABLED_PRESOLVE                 = true;
    static constexpr bool DEFAULT_IS_ENABLED_INITIAL_VALUE_CORRECTION = true;
};

/*****************************************************************************/
struct PreprocessOption {
    bool is_enabled_presolve;
    bool is_enabled_initial_value_correction;

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
        this->is_enabled_initial_value_correction = PreprocessOptionConstant::
            DEFAULT_IS_ENABLED_INITIAL_VALUE_CORRECTION;
    }

    /*************************************************************************/
    void print(void) const {
        utility::print(                               //
            " -- preprocess.is_enabled_presolve: " +  //
            utility::to_true_or_false(                //
                this->is_enabled_presolve));

        utility::print(                                               //
            " -- preprocess.is_enabled_initial_value_correction: " +  //
            utility::to_true_or_false(                                //
                this->is_enabled_initial_value_correction));
    }

    /******************************************************************************/
    inline void setup(const utility::json::JsonObject &a_OBJECT) {
        this->initialize();

        read_json(&this->is_enabled_presolve,  //
                  "is_enabled_presolve", a_OBJECT);

        read_json(&this->is_enabled_initial_value_correction,  //
                  "is_enabled_initial_value_correction", a_OBJECT);
    }
};
}  // namespace printemps::option

#endif
/*****************************************************************************/
// END
/*****************************************************************************/