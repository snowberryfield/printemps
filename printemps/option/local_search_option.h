/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_OPTION_LOCAL_SEARCH_OPTION_H__
#define PRINTEMPS_OPTION_LOCAL_SEARCH_OPTION_H__

namespace printemps::option {
/*****************************************************************************/
struct LocalSearchOptionConstant {
    static constexpr bool   DEFAULT_IS_ENABLED    = true;
    static constexpr int    DEFAULT_ITERATION_MAX = 10000;
    static constexpr double DEFAULT_TIME_MAX      = 120.0;
    static constexpr double DEFAULT_TIME_OFFSET   = 0.0;
    static constexpr int    DEFAULT_LOG_INTERVAL  = 10;
    static constexpr int    DEFAULT_SEED          = 1;
};

/*****************************************************************************/
struct LocalSearchOption {
    bool   is_enabled;
    int    iteration_max;
    double time_max;
    double time_offset;
    int    log_interval;
    int    seed;

    /*************************************************************************/
    LocalSearchOption(void) {
        this->initialize();
    }

    /*************************************************************************/
    LocalSearchOption(const utility::json::JsonObject &a_OBJECT) {
        this->setup(a_OBJECT);
    }

    /*************************************************************************/
    void initialize(void) {
        this->is_enabled    = LocalSearchOptionConstant::DEFAULT_IS_ENABLED;
        this->iteration_max = LocalSearchOptionConstant::DEFAULT_ITERATION_MAX;
        this->time_max      = LocalSearchOptionConstant::DEFAULT_TIME_MAX;
        this->time_offset   = LocalSearchOptionConstant::DEFAULT_TIME_OFFSET;
        this->log_interval  = LocalSearchOptionConstant::DEFAULT_LOG_INTERVAL;
        this->seed          = LocalSearchOptionConstant::DEFAULT_SEED;
    }

    /*************************************************************************/
    void print(void) const {
        utility::print(                        //
            " -- local_search.is_enabled: " +  //
            utility::to_true_or_false(         //
                this->is_enabled));

        utility::print(                           //
            " -- local_search.iteration_max: " +  //
            utility::to_string(                   //
                this->iteration_max, "%d"));

        utility::print(                      //
            " -- local_search.time_max: " +  //
            utility::to_string(              //
                this->time_max, "%f"));

        utility::print(                         //
            " -- local_search.time_offset: " +  //
            utility::to_string(                 //
                this->time_offset, "%f"));

        utility::print(                          //
            " -- local_search.log_interval: " +  //
            utility::to_string(                  //
                this->log_interval, "%d"));

        utility::print(                  //
            " -- local_search.seed: " +  //
            utility::to_string(          //
                this->seed, "%d"));
    }

    /**************************************************************************/
    inline void setup(const utility::json::JsonObject &a_OBJECT) {
        this->initialize();

        read_json(&this->is_enabled,  //
                  "is_enabled", a_OBJECT);

        read_json(&this->iteration_max,  //
                  "iteration_max", a_OBJECT);

        read_json(&this->time_max,  //
                  "time_max", a_OBJECT);

        read_json(&this->time_offset,  //
                  "time_offset", a_OBJECT);

        read_json(&this->log_interval,  //
                  "log_interval", a_OBJECT);

        read_json(&this->seed,  //
                  "seed", a_OBJECT);
    }
};
}  // namespace printemps::option

#endif
/*****************************************************************************/
// END
/*****************************************************************************/