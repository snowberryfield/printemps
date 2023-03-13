/*****************************************************************************/
// Copyright (c) 2020-2023 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_OPTION_PDLP_OPTION_H__
#define PRINTEMPS_OPTION_PDLP_OPTION_H__

namespace printemps::option {
/*****************************************************************************/
struct PDLPOptionConstant {
    static constexpr bool   DEFAULT_IS_ENABLED                        = false;
    static constexpr int    DEFAULT_ITERATION_MAX                     = 100000;
    static constexpr double DEFAULT_TIME_MAX                          = 120.0;
    static constexpr double DEFAULT_TIME_OFFSET                       = 0.0;
    static constexpr double DEFAULT_TOLERANCE                         = 1E-5;
    static constexpr double DEFAULT_STEP_SIZE_EXTEND_EXPONENT         = -0.6;
    static constexpr double DEFAULT_STEP_SIZE_REDUCE_EXPONENT         = -0.3;
    static constexpr double DEFAULT_RESTART_THRESHOLD_SUFFICIENT      = 0.1;
    static constexpr double DEFAULT_RESTART_THRESHOLD_NECESSARY       = 0.9;
    static constexpr double DEFAULT_RESTART_THRESHOLD_ARTIFICIAL      = 0.5;
    static constexpr int    DEFAULT_RESTART_CHECK_INTERVAL            = 10;
    static constexpr int    DEFAULT_CONVERGENCE_CHECK_INTERVAL        = 10;
    static constexpr int    DEFAULT_COUNTS_OF_RUIZ_SCALING            = 10;
    static constexpr bool   DEFAULT_IS_ENABLED_POCK_CHAMBOLLE_SCALING = true;
    static constexpr int    DEFAULT_LOG_INTERVAL                      = 10;
};

/*****************************************************************************/
struct PDLPOption {
    bool   is_enabled;
    int    iteration_max;
    double time_max;
    double time_offset;
    double tolerance;
    double step_size_extend_exponent;
    double step_size_reduce_exponent;
    double restart_threshold_sufficient;
    double restart_threshold_necessary;
    double restart_threshold_artificial;
    int    restart_check_interval;
    int    convergence_check_interval;
    int    counts_of_ruiz_scaling;
    bool   is_enabled_pock_chambolle_scaling;
    int    log_interval;

    /*************************************************************************/
    PDLPOption(void) {
        this->initialize();
    }

    /*************************************************************************/
    PDLPOption(const utility::json::JsonObject &a_OBJECT) {
        this->setup(a_OBJECT);
    }

    /*************************************************************************/
    void initialize(void) {
        this->is_enabled    = PDLPOptionConstant::DEFAULT_IS_ENABLED;
        this->iteration_max = PDLPOptionConstant::DEFAULT_ITERATION_MAX;
        this->time_max      = PDLPOptionConstant::DEFAULT_TIME_MAX;
        this->time_offset   = PDLPOptionConstant::DEFAULT_TIME_OFFSET;
        this->tolerance     = PDLPOptionConstant::DEFAULT_TOLERANCE;
        this->step_size_extend_exponent =
            PDLPOptionConstant::DEFAULT_STEP_SIZE_EXTEND_EXPONENT;
        this->step_size_reduce_exponent =
            PDLPOptionConstant::DEFAULT_STEP_SIZE_REDUCE_EXPONENT;
        this->restart_threshold_sufficient =
            PDLPOptionConstant::DEFAULT_RESTART_THRESHOLD_SUFFICIENT;
        this->restart_threshold_necessary =
            PDLPOptionConstant::DEFAULT_RESTART_THRESHOLD_NECESSARY;
        this->restart_threshold_artificial =
            PDLPOptionConstant::DEFAULT_RESTART_THRESHOLD_ARTIFICIAL;
        this->restart_check_interval =
            PDLPOptionConstant::DEFAULT_RESTART_CHECK_INTERVAL;
        this->convergence_check_interval =
            PDLPOptionConstant::DEFAULT_CONVERGENCE_CHECK_INTERVAL;
        this->counts_of_ruiz_scaling =
            PDLPOptionConstant::DEFAULT_COUNTS_OF_RUIZ_SCALING;
        this->is_enabled_pock_chambolle_scaling =
            PDLPOptionConstant::DEFAULT_IS_ENABLED_POCK_CHAMBOLLE_SCALING;
        this->log_interval = PDLPOptionConstant::DEFAULT_LOG_INTERVAL;
    }

    /*************************************************************************/
    void print(void) const {
        utility::print(                 //
            " -- pdlp.is_enabled: " +   //
            utility::to_true_or_false(  //
                this->is_enabled));

        utility::print(                   //
            " -- pdlp.iteration_max: " +  //
            utility::to_string(           //
                this->iteration_max, "%d"));

        utility::print(              //
            " -- pdlp.time_max: " +  //
            utility::to_string(      //
                this->time_max, "%f"));

        utility::print(                 //
            " -- pdlp.time_offset: " +  //
            utility::to_string(         //
                this->time_offset, "%f"));

        utility::print(               //
            " -- pdlp.tolerance: " +  //
            utility::to_string(       //
                this->tolerance, "%f"));

        utility::print(                               //
            " -- pdlp.step_size_extend_exponent: " +  //
            utility::to_string(                       //
                this->step_size_extend_exponent, "%f"));

        utility::print(                               //
            " -- pdlp.step_size_reduce_exponent: " +  //
            utility::to_string(                       //
                this->step_size_reduce_exponent, "%f"));

        utility::print(                                  //
            " -- pdlp.restart_threshold_sufficient: " +  //
            utility::to_string(                          //
                this->restart_threshold_sufficient, "%f"));

        utility::print(                                 //
            " -- pdlp.restart_threshold_necessary: " +  //
            utility::to_string(                         //
                this->restart_threshold_necessary, "%f"));

        utility::print(                                  //
            " -- pdlp.restart_threshold_artificial: " +  //
            utility::to_string(                          //
                this->restart_threshold_artificial, "%f"));

        utility::print(                            //
            " -- pdlp.restart_check_interval: " +  //
            utility::to_string(                    //
                this->restart_check_interval, "%d"));

        utility::print(                                //
            " -- pdlp.convergence_check_interval: " +  //
            utility::to_string(                        //
                this->convergence_check_interval, "%d"));

        utility::print(                            //
            " -- pdlp.counts_of_ruiz_scaling: " +  //
            utility::to_string(                    //
                this->counts_of_ruiz_scaling, "%d"));

        utility::print(                                       //
            " -- pdlp.is_enabled_pock_chambolle_scaling: " +  //
            utility::to_string(                               //
                this->is_enabled_pock_chambolle_scaling, "%d"));

        utility::print(                  //
            " -- pdlp.log_interval: " +  //
            utility::to_string(          //
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

        read_json(             //
            &this->tolerance,  //
            "tolerance", a_OBJECT);

        read_json(                             //
            &this->step_size_extend_exponent,  //
            "step_size_extend_exponent", a_OBJECT);

        read_json(                             //
            &this->step_size_reduce_exponent,  //
            "step_size_reduce_exponent", a_OBJECT);

        read_json(                                //
            &this->restart_threshold_sufficient,  //
            "restart_threshold_sufficient", a_OBJECT);

        read_json(                               //
            &this->restart_threshold_necessary,  //
            "restart_threshold_necessary", a_OBJECT);

        read_json(                                //
            &this->restart_threshold_artificial,  //
            "restart_threshold_artificial", a_OBJECT);

        read_json(                          //
            &this->restart_check_interval,  //
            "restart_check_interval", a_OBJECT);

        read_json(                              //
            &this->convergence_check_interval,  //
            "convergence_check_interval", a_OBJECT);

        read_json(                          //
            &this->counts_of_ruiz_scaling,  //
            "counts_of_ruiz_scaling", a_OBJECT);

        read_json(                                     //
            &this->is_enabled_pock_chambolle_scaling,  //
            "is_enabled_pock_chambolle_scaling", a_OBJECT);

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

        obj.emplace_back(  //
            "tolerance",   //
            this->tolerance);

        obj.emplace_back(                 //
            "step_size_extend_exponent",  //
            this->step_size_extend_exponent);

        obj.emplace_back(                 //
            "step_size_reduce_exponent",  //
            this->step_size_reduce_exponent);

        obj.emplace_back(                    //
            "restart_threshold_sufficient",  //
            this->restart_threshold_sufficient);

        obj.emplace_back(                   //
            "restart_threshold_necessary",  //
            this->restart_threshold_necessary);

        obj.emplace_back(                    //
            "restart_threshold_artificial",  //
            this->restart_threshold_artificial);

        obj.emplace_back(              //
            "restart_check_interval",  //
            this->restart_check_interval);

        obj.emplace_back(                  //
            "convergence_check_interval",  //
            this->convergence_check_interval);

        obj.emplace_back(              //
            "counts_of_ruiz_scaling",  //
            this->counts_of_ruiz_scaling);

        obj.emplace_back(                         //
            "is_enabled_pock_chambolle_scaling",  //
            this->is_enabled_pock_chambolle_scaling);

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