/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_OPTION_OUTPUT_OPTION_H__
#define PRINTEMPS_OPTION_OUTPUT_OPTION_H__

namespace printemps::option {
/*****************************************************************************/
struct OutputOptionConstant {
    static constexpr verbose::Verbose DEFAULT_VERBOSE = verbose::Off;
    static constexpr bool             DEFAULT_IS_ENABLED_WRITE_TREND  = false;
    static constexpr bool DEFAULT_IS_ENABLED_STORE_FEASIBLE_SOLUTIONS = false;
    static constexpr int  DEFAULT_FEASIBLE_SOLUTIONS_CAPACITY         = 1000;
};

/*****************************************************************************/
struct OutputOption {
    verbose::Verbose verbose;
    bool             is_enabled_write_trend;
    bool             is_enabled_store_feasible_solutions;
    int              feasible_solutions_capacity;

    /*************************************************************************/
    OutputOption(void) {
        this->initialize();
    }

    /*************************************************************************/
    OutputOption(const utility::json::JsonObject &a_OBJECT) {
        this->setup(a_OBJECT);
    }

    /*************************************************************************/
    void initialize(void) {
        this->verbose = OutputOptionConstant::DEFAULT_VERBOSE;
        this->is_enabled_write_trend =
            OutputOptionConstant::DEFAULT_IS_ENABLED_WRITE_TREND;
        this->is_enabled_store_feasible_solutions =
            OutputOptionConstant::DEFAULT_IS_ENABLED_STORE_FEASIBLE_SOLUTIONS;
        this->feasible_solutions_capacity =
            OutputOptionConstant::DEFAULT_FEASIBLE_SOLUTIONS_CAPACITY;
    }

    /*************************************************************************/
    void print(void) const {
        utility::print(               //
            " -- output.verbose: " +  //
            verbose::VerboseInverseMap.at(this->verbose));

        utility::print(                              //
            " -- output.is_enabled_write_trend: " +  //
            utility::to_true_or_false(               //
                this->is_enabled_write_trend));

        utility::print(                                           //
            " -- output.is_enabled_store_feasible_solutions: " +  //
            utility::to_true_or_false(                            //
                this->is_enabled_store_feasible_solutions));

        utility::print(                                   //
            " -- output.feasible_solutions_capacity: " +  //
            utility::to_string(                           //
                this->feasible_solutions_capacity, "%d"));
    }

    /**************************************************************************/
    inline void setup(const utility::json::JsonObject &a_OBJECT) {
        this->initialize();

        read_json(&this->verbose,  //
                  "verbose", a_OBJECT);

        read_json(&this->is_enabled_write_trend,  //
                  "is_enabled_write_trend", a_OBJECT);

        read_json(&this->is_enabled_store_feasible_solutions,  //
                  "is_enabled_store_feasible_solutions", a_OBJECT);

        read_json(&this->feasible_solutions_capacity,  //
                  "feasible_solutions_capacity", a_OBJECT);
    }
};
}  // namespace printemps::option

#endif
/*****************************************************************************/
// END
/*****************************************************************************/