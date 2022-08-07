/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_OPTION_GENERAL_OPTION_H__
#define PRINTEMPS_OPTION_GENERAL_OPTION_H__

namespace printemps::option {
/*****************************************************************************/
struct GeneralOptionConstant {
    static constexpr int    DEFAULT_ITERATION_MAX          = 100;
    static constexpr double DEFAULT_TIME_MAX               = 120.0;
    static constexpr double DEFAULT_TIME_OFFSET            = 0.0;
    static constexpr double DEFAULT_TARGET_OBJECTIVE_VALUE = -1E100;
    static constexpr bool   DEFAULT_SEED                   = 1;
};

/*****************************************************************************/
struct GeneralOption {
    int    iteration_max;
    double time_offset;
    double time_max;
    double target_objective_value;
    int    seed;

    /*************************************************************************/
    GeneralOption(void) {
        this->initialize();
    }

    /*************************************************************************/
    GeneralOption(const utility::json::JsonObject &a_OBJECT) {
        this->setup(a_OBJECT);
    }

    /*************************************************************************/
    void initialize(void) {
        this->iteration_max = GeneralOptionConstant::DEFAULT_ITERATION_MAX;
        this->time_max      = GeneralOptionConstant::DEFAULT_TIME_MAX;
        this->time_offset   = GeneralOptionConstant::DEFAULT_TIME_OFFSET;
        this->target_objective_value =
            GeneralOptionConstant::DEFAULT_TARGET_OBJECTIVE_VALUE;
        this->seed = GeneralOptionConstant::DEFAULT_SEED;
    }

    /*************************************************************************/
    void print(void) const {
        utility::print(                      //
            " -- general.iteration_max: " +  //
            utility::to_string(              //
                this->iteration_max, "%d"));

        utility::print(                    //
            " -- general.time_offset: " +  //
            utility::to_string(            //
                this->time_offset, "%f"));

        utility::print(                 //
            " -- general.time_max: " +  //
            utility::to_string(         //
                this->time_max, "%f"));

        utility::print(                               //
            " -- general.target_objective_value: " +  //
            utility::to_string(                       //
                this->target_objective_value, "%.3e"));

        utility::print(             //
            " -- general.seed: " +  //
            utility::to_string(     //
                this->seed, "%d"));
    }

    /******************************************************************************/
    inline void setup(const utility::json::JsonObject &a_OBJECT) {
        this->initialize();

        read_json(&this->iteration_max,  //
                  "iteration_max", a_OBJECT);

        read_json(&this->time_offset,  //
                  "time_offset", a_OBJECT);

        read_json(&this->time_max,  //
                  "time_max", a_OBJECT);

        read_json(&this->target_objective_value,  //
                  "target_objective_value", a_OBJECT);

        read_json(&this->seed,  //
                  "seed", a_OBJECT);
    }
};
}  // namespace printemps::option

#endif
/*****************************************************************************/
// END
/*****************************************************************************/