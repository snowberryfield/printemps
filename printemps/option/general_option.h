/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_OPTION_GENERAL_OPTION_H__
#define PRINTEMPS_OPTION_GENERAL_OPTION_H__

namespace printemps::option {
/*****************************************************************************/
struct GeneralOptionConstant {
    static constexpr int    DEFAULT_ITERATION_MAX              = 100;
    static constexpr double DEFAULT_TIME_MAX                   = 120.0;
    static constexpr double DEFAULT_TIME_OFFSET                = 0.0;
    static constexpr double DEFAULT_TARGET_OBJECTIVE_VALUE     = -1E100;
    static constexpr int    DEFAULT_SEED                       = 1;
    static constexpr bool   DEFAULT_IS_ENABLED_FAST_EVALUATION = true;
};

/*****************************************************************************/
struct GeneralOption {
    int    iteration_max;
    double time_offset;
    double time_max;
    double target_objective_value;
    int    seed;
    bool   is_enabled_fast_evaluation;

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
        this->is_enabled_fast_evaluation =
            GeneralOptionConstant::DEFAULT_IS_ENABLED_FAST_EVALUATION;
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

        utility::print(                                   //
            " -- general.is_enabled_fast_evaluation: " +  //
            utility::to_true_or_false(                    //
                this->is_enabled_fast_evaluation));
    }

    /**************************************************************************/
    inline void setup(const utility::json::JsonObject &a_OBJECT) {
        this->initialize();

        read_json(                 //
            &this->iteration_max,  //
            "iteration_max", a_OBJECT);

        read_json(               //
            &this->time_offset,  //
            "time_offset", a_OBJECT);

        read_json(            //
            &this->time_max,  //
            "time_max", a_OBJECT);

        read_json(                          //
            &this->target_objective_value,  //
            "target_objective_value", a_OBJECT);

        read_json(        //
            &this->seed,  //
            "seed", a_OBJECT);

        read_json(                              //
            &this->is_enabled_fast_evaluation,  //
            "is_enabled_fast_evaluation", a_OBJECT);
    }

    /**************************************************************************/
    inline utility::json::JsonObject to_json(void) const {
        utility::json::JsonObject obj;
        obj.emplace_back(     //
            "iteration_max",  //
            this->iteration_max);

        obj.emplace_back(   //
            "time_offset",  //
            this->time_offset);

        obj.emplace_back(  //
            "time_max",    //
            this->time_max);

        obj.emplace_back(              //
            "target_objective_value",  //
            this->target_objective_value);

        obj.emplace_back(  //
            "seed",        //
            this->seed);

        obj.emplace_back(                  //
            "is_enabled_fast_evaluation",  //
            this->is_enabled_fast_evaluation);

        return obj;
    }
};
}  // namespace printemps::option

#endif
/*****************************************************************************/
// END
/*****************************************************************************/