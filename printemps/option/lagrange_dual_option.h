/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_LAGRANGE_DUAL_LAGRANGE_DUAL_OPTION_H__
#define PRINTEMPS_SOLVER_LAGRANGE_DUAL_LAGRANGE_DUAL_OPTION_H__

namespace printemps::option {
/*****************************************************************************/
struct LagrangeDualOptionConstant {
    static constexpr bool   DEFAULT_IS_ENABLED            = false;
    static constexpr int    DEFAULT_ITERATION_MAX         = 10000;
    static constexpr double DEFAULT_TIME_MAX              = 120.0;
    static constexpr double DEFAULT_TIME_OFFSET           = 0.0;
    static constexpr double DEFAULT_STEP_SIZE_EXTEND_RATE = 1.05;
    static constexpr double DEFAULT_STEP_SIZE_REDUCE_RATE = 0.95;
    static constexpr double DEFAULT_TOLERANCE             = 1E-5;
    static constexpr int    DEFAULT_QUEUE_SIZE            = 100;
    static constexpr int    DEFAULT_LOG_INTERVAL          = 10;
};

/*****************************************************************************/
struct LagrangeDualOption {
    bool   is_enabled;
    int    iteration_max;
    double time_max;
    double time_offset;
    double step_size_extend_rate;
    double step_size_reduce_rate;
    double tolerance;
    int    queue_size;
    int    log_interval;

    /*************************************************************************/
    LagrangeDualOption(void) {
        this->initialize();
    }

    /*************************************************************************/
    LagrangeDualOption(const utility::json::JsonObject &a_OBJECT) {
        this->setup(a_OBJECT);
    }

    /*************************************************************************/
    void initialize(void) {
        this->is_enabled    = LagrangeDualOptionConstant::DEFAULT_IS_ENABLED;
        this->iteration_max = LagrangeDualOptionConstant::DEFAULT_ITERATION_MAX;
        this->time_max      = LagrangeDualOptionConstant::DEFAULT_TIME_MAX;
        this->time_offset   = LagrangeDualOptionConstant::DEFAULT_TIME_OFFSET;
        this->step_size_extend_rate =
            LagrangeDualOptionConstant::DEFAULT_STEP_SIZE_EXTEND_RATE;
        this->step_size_reduce_rate =
            LagrangeDualOptionConstant::DEFAULT_STEP_SIZE_REDUCE_RATE;
        this->tolerance    = LagrangeDualOptionConstant::DEFAULT_TOLERANCE;
        this->queue_size   = LagrangeDualOptionConstant::DEFAULT_QUEUE_SIZE;
        this->log_interval = LagrangeDualOptionConstant::DEFAULT_LOG_INTERVAL;
    }

    /*************************************************************************/
    void print(void) const {
        utility::print(                         //
            " -- lagrange_dual.is_enabled: " +  //
            utility::to_true_or_false(          //
                this->is_enabled));

        utility::print(                            //
            " -- lagrange_dual.iteration_max: " +  //
            utility::to_string(                    //
                this->iteration_max, "%d"));

        utility::print(                       //
            " -- lagrange_dual.time_max: " +  //
            utility::to_string(               //
                this->time_max, "%f"));

        utility::print(                          //
            " -- lagrange_dual.time_offset: " +  //
            utility::to_string(                  //
                this->time_offset, "%f"));

        utility::print(                                    //
            " -- lagrange_dual.step_size_extend_rate: " +  //
            utility::to_string(                            //
                this->step_size_extend_rate, "%f"));

        utility::print(                                    //
            " -- lagrange_dual.step_size_reduce_rate: " +  //
            utility::to_string(                            //
                this->step_size_reduce_rate, "%f"));

        utility::print(                        //
            " -- lagrange_dual.tolerance: " +  //
            utility::to_string(                //
                this->tolerance, "%f"));

        utility::print(                         //
            " -- lagrange_dual.queue_size: " +  //
            utility::to_string(                 //
                this->queue_size, "%d"));

        utility::print(                           //
            " -- lagrange_dual.log_interval: " +  //
            utility::to_string(                   //
                this->log_interval, "%d"));
    }

    /**************************************************************************/
    inline void setup(const utility::json::JsonObject &a_OBJECT) {
        this->initialize();

        read_json(              //
            &this->is_enabled,  //
            "is_enabled", a_OBJECT);

        read_json(                 //
            &this->iteration_max,  //
            "iteration_max", a_OBJECT);

        read_json(            //
            &this->time_max,  //
            "time_max", a_OBJECT);

        read_json(               //
            &this->time_offset,  //
            "time_offset", a_OBJECT);

        read_json(                         //
            &this->step_size_extend_rate,  //
            "step_size_extend_rate", a_OBJECT);

        read_json(                         //
            &this->step_size_reduce_rate,  //
            "step_size_reduce_rate", a_OBJECT);

        read_json(             //
            &this->tolerance,  //
            "tolerance", a_OBJECT);

        read_json(              //
            &this->queue_size,  //
            "queue_size", a_OBJECT);

        read_json(                //
            &this->log_interval,  //
            "log_interval", a_OBJECT);
    }

    /**************************************************************************/
    inline utility::json::JsonObject to_json(void) const {
        utility::json::JsonObject obj;
        obj.emplace_back(  //
            "is_enabled",  //
            this->is_enabled);

        obj.emplace_back(     //
            "iteration_max",  //
            this->iteration_max);

        obj.emplace_back(  //
            "time_max",    //
            this->time_max);

        obj.emplace_back(   //
            "time_offset",  //
            this->time_offset);

        obj.emplace_back(             //
            "step_size_extend_rate",  //
            this->step_size_extend_rate);

        obj.emplace_back(             //
            "step_size_reduce_rate",  //
            this->step_size_reduce_rate);

        obj.emplace_back(  //
            "tolerance",   //
            this->tolerance);

        obj.emplace_back(  //
            "queue_size",  //
            this->queue_size);

        obj.emplace_back(    //
            "log_interval",  //
            this->log_interval);

        return obj;
    }
};
}  // namespace printemps::option

#endif
/*****************************************************************************/
// END
/*****************************************************************************/