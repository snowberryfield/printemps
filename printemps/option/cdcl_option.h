/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_OPTION_CDCL_OPTION_H__
#define PRINTEMPS_OPTION_CDCL_OPTION_H__

namespace printemps::option {
/*****************************************************************************/
struct CDCLOptionConstant {
    static constexpr bool   DEFAULT_IS_ENABLED    = true;
    static constexpr int    DEFAULT_ITERATION_MAX = 10000000;
    static constexpr double DEFAULT_TIME_MAX      = 10.0;
    static constexpr double DEFAULT_TIME_OFFSET   = 0.0;
    static constexpr int    DEFAULT_LOG_INTERVAL  = 1000;
    static constexpr int    DEFAULT_CONFLICT_MAX  = -1;
    static constexpr int    DEFAULT_SEED          = 1;

    static constexpr bool DEFAULT_IS_ENABLED_RESTART = true;
    static constexpr cdcl_restart_mode::CDCLRestartMode DEFAULT_RESTART_MODE =
        cdcl_restart_mode::CDCLRestartMode::Luby;
    static constexpr int    DEFAULT_INITIAL_CONFLICT_LIMIT            = 100;
    static constexpr double DEFAULT_RESTART_EXPONENTIAL_INCREASE_RATE = 1.5;
    static constexpr int    DEFAULT_RESTART_ADAPTIVE_WINDOW           = 50;
    static constexpr double
        DEFAULT_RESTART_ADAPTIVE_LITERAL_BLOCK_DISTANCE_RATIO_THRESHOLD = 1.25;

    static constexpr bool DEFAULT_IS_ENABLED_LEARNED_CONSTRAINT_REDUCTION =
        true;
    static constexpr int    DEFAULT_LEARNED_CONSTRAINT_CAPACITY       = 2000;
    static constexpr double DEFAULT_LEARNED_CONSTRAINT_REDUCTION_RATE = 0.5;
    static constexpr int DEFAULT_LITERAL_BLOCK_DISTANCE_PROTECTION_THRESHOLD =
        2;

    static constexpr double DEFAULT_VARIABLE_ACTIVITY_DECAY_RATE = 0.8;
    static constexpr bool   DEFAULT_IS_ENABLED_PHASE_SAVING      = true;
};

/*****************************************************************************/
struct CDCLOption {
    bool   is_enabled;
    int    iteration_max;
    double time_max;
    double time_offset;
    int    log_interval;
    int    conflict_max;
    int    seed;

    bool                               is_enabled_restart;
    cdcl_restart_mode::CDCLRestartMode restart_mode;
    int                                initial_conflict_limit;
    double                             restart_exponential_increase_rate;
    int                                restart_adaptive_window;
    double restart_adaptive_literal_block_distance_ratio_threshold;

    bool   is_enabled_learned_constraint_reduction;
    int    learned_constraint_capacity;
    double learned_constraint_reduction_rate;
    int    literal_block_distance_protection_threshold;

    double variable_activity_decay_rate;
    bool   is_enabled_phase_saving;

    /*************************************************************************/
    CDCLOption(void) {
        this->initialize();
    }

    /*************************************************************************/
    CDCLOption(const utility::json::JsonObject &a_OBJECT) {
        this->setup(a_OBJECT);
    }

    /*************************************************************************/
    void initialize(void) {
        this->is_enabled    = CDCLOptionConstant::DEFAULT_IS_ENABLED;
        this->iteration_max = CDCLOptionConstant::DEFAULT_ITERATION_MAX;
        this->time_max      = CDCLOptionConstant::DEFAULT_TIME_MAX;
        this->time_offset   = CDCLOptionConstant::DEFAULT_TIME_OFFSET;
        this->log_interval  = CDCLOptionConstant::DEFAULT_LOG_INTERVAL;
        this->conflict_max  = CDCLOptionConstant::DEFAULT_CONFLICT_MAX;
        this->seed          = CDCLOptionConstant::DEFAULT_SEED;

        this->is_enabled_restart =
            CDCLOptionConstant::DEFAULT_IS_ENABLED_RESTART;
        this->restart_mode = CDCLOptionConstant::DEFAULT_RESTART_MODE;
        this->initial_conflict_limit =
            CDCLOptionConstant::DEFAULT_INITIAL_CONFLICT_LIMIT;
        this->restart_exponential_increase_rate =
            CDCLOptionConstant::DEFAULT_RESTART_EXPONENTIAL_INCREASE_RATE;
        this->restart_adaptive_window =
            CDCLOptionConstant::DEFAULT_RESTART_ADAPTIVE_WINDOW;
        this->restart_adaptive_literal_block_distance_ratio_threshold =
            CDCLOptionConstant::
                DEFAULT_RESTART_ADAPTIVE_LITERAL_BLOCK_DISTANCE_RATIO_THRESHOLD;

        this->is_enabled_learned_constraint_reduction =
            CDCLOptionConstant::DEFAULT_IS_ENABLED_LEARNED_CONSTRAINT_REDUCTION;
        this->learned_constraint_capacity =
            CDCLOptionConstant::DEFAULT_LEARNED_CONSTRAINT_CAPACITY;
        this->learned_constraint_reduction_rate =
            CDCLOptionConstant::DEFAULT_LEARNED_CONSTRAINT_REDUCTION_RATE;
        this->literal_block_distance_protection_threshold = CDCLOptionConstant::
            DEFAULT_LITERAL_BLOCK_DISTANCE_PROTECTION_THRESHOLD;

        this->variable_activity_decay_rate =
            CDCLOptionConstant::DEFAULT_VARIABLE_ACTIVITY_DECAY_RATE;
        this->is_enabled_phase_saving =
            CDCLOptionConstant::DEFAULT_IS_ENABLED_PHASE_SAVING;
    }

    /*************************************************************************/
    void print(void) const {
        utility::print(                 //
            " -- cdcl.is_enabled: " +   //
            utility::to_true_or_false(  //
                this->is_enabled));

        utility::print(                   //
            " -- cdcl.iteration_max: " +  //
            utility::to_string(           //
                this->iteration_max, "%d"));

        utility::print(              //
            " -- cdcl.time_max: " +  //
            utility::to_string(      //
                this->time_max, "%f"));

        utility::print(                 //
            " -- cdcl.time_offset: " +  //
            utility::to_string(         //
                this->time_offset, "%f"));

        utility::print(                  //
            " -- cdcl.log_interval: " +  //
            utility::to_string(          //
                this->log_interval, "%d"));

        utility::print(                  //
            " -- cdcl.conflict_max: " +  //
            utility::to_string(          //
                this->conflict_max, "%d"));

        utility::print(          //
            " -- cdcl.seed: " +  //
            utility::to_string(  //
                this->seed, "%d"));

        utility::print(                        //
            " -- cdcl.is_enabled_restart: " +  //
            utility::to_true_or_false(         //
                this->is_enabled_restart));

        utility::print(                  //
            " -- cdcl.restart_mode: " +  //
            cdcl_restart_mode::CDCLRestartModeInverseMap.at(
                this->restart_mode));

        utility::print(                            //
            " -- cdcl.initial_conflict_limit: " +  //
            utility::to_string(                    //
                this->initial_conflict_limit, "%d"));

        utility::print(                                       //
            " -- cdcl.restart_exponential_increase_rate: " +  //
            utility::to_string(                               //
                this->restart_exponential_increase_rate, "%f"));

        utility::print(                            //
            " -- cdcl.restart_adaptive_window: " +  //
            utility::to_string(                    //
                this->restart_adaptive_window, "%d"));

        utility::print(  //
            " -- cdcl.restart_adaptive_literal_block_distance_ratio_threshold: "
            "" +                 //
            utility::to_string(  //
                this->restart_adaptive_literal_block_distance_ratio_threshold,
                "%f"));

        utility::print(                                             //
            " -- cdcl.is_enabled_learned_constraint_reduction: " +  //
            utility::to_true_or_false(                              //
                this->is_enabled_learned_constraint_reduction));

        utility::print(                                 //
            " -- cdcl.learned_constraint_capacity: " +  //
            utility::to_string(                         //
                this->learned_constraint_capacity, "%d"));

        utility::print(                                       //
            " -- cdcl.learned_constraint_reduction_rate: " +  //
            utility::to_string(                               //
                this->learned_constraint_reduction_rate, "%f"));

        utility::print(                                                 //
            " -- cdcl.literal_block_distance_protection_threshold: " +  //
            utility::to_string(                                         //
                this->literal_block_distance_protection_threshold, "%d"));

        utility::print(                                  //
            " -- cdcl.variable_activity_decay_rate: " +  //
            utility::to_string(                          //
                this->variable_activity_decay_rate, "%f"));

        utility::print(                             //
            " -- cdcl.is_enabled_phase_saving: " +  //
            utility::to_true_or_false(              //
                this->is_enabled_phase_saving));
    }

    /**************************************************************************/
    inline void setup(const utility::json::JsonObject &a_OBJECT) {
        this->initialize();

        read_json(&this->is_enabled, "is_enabled", a_OBJECT);
        read_json(&this->iteration_max, "iteration_max", a_OBJECT);
        read_json(&this->time_max, "time_max", a_OBJECT);
        read_json(&this->time_offset, "time_offset", a_OBJECT);
        read_json(&this->log_interval, "log_interval", a_OBJECT);
        read_json(&this->conflict_max, "conflict_max", a_OBJECT);
        read_json(&this->seed, "seed", a_OBJECT);

        read_json(&this->is_enabled_restart, "is_enabled_restart", a_OBJECT);
        read_json(&this->restart_mode, "restart_mode", a_OBJECT);
        read_json(&this->initial_conflict_limit, "initial_conflict_limit",
                  a_OBJECT);
        read_json(&this->restart_exponential_increase_rate,
                  "restart_exponential_increase_rate", a_OBJECT);
        read_json(&this->restart_adaptive_window, "restart_adaptive_window",
                  a_OBJECT);
        read_json(
            &this->restart_adaptive_literal_block_distance_ratio_threshold,
            "restart_adaptive_literal_block_distance_ratio_threshold",
            a_OBJECT);

        read_json(&this->is_enabled_learned_constraint_reduction,
                  "is_enabled_learned_constraint_reduction", a_OBJECT);
        read_json(&this->learned_constraint_capacity,
                  "learned_constraint_capacity", a_OBJECT);
        read_json(&this->learned_constraint_reduction_rate,
                  "learned_constraint_reduction_rate", a_OBJECT);
        read_json(&this->literal_block_distance_protection_threshold,
                  "literal_block_distance_protection_threshold", a_OBJECT);

        read_json(&this->variable_activity_decay_rate,
                  "variable_activity_decay_rate", a_OBJECT);
        read_json(&this->is_enabled_phase_saving, "is_enabled_phase_saving",
                  a_OBJECT);
    }

    /**************************************************************************/
    inline utility::json::JsonObject to_json(void) const {
        utility::json::JsonObject obj;
        obj.emplace_back("is_enabled", this->is_enabled);
        obj.emplace_back("iteration_max", this->iteration_max);
        obj.emplace_back("time_max", this->time_max);
        obj.emplace_back("time_offset", this->time_offset);
        obj.emplace_back("log_interval", this->log_interval);
        obj.emplace_back("conflict_max", this->conflict_max);
        obj.emplace_back("seed", this->seed);

        obj.emplace_back("is_enabled_restart", this->is_enabled_restart);
        obj.emplace_back("restart_mode",
                         cdcl_restart_mode::CDCLRestartModeInverseMap.at(
                             this->restart_mode));
        obj.emplace_back("initial_conflict_limit",
                         this->initial_conflict_limit);
        obj.emplace_back("restart_exponential_increase_rate",
                         this->restart_exponential_increase_rate);
        obj.emplace_back("restart_adaptive_window",
                         this->restart_adaptive_window);
        obj.emplace_back(
            "restart_adaptive_literal_block_distance_ratio_threshold",
            this->restart_adaptive_literal_block_distance_ratio_threshold);

        obj.emplace_back("is_enabled_learned_constraint_reduction",
                         this->is_enabled_learned_constraint_reduction);
        obj.emplace_back("learned_constraint_capacity",
                         this->learned_constraint_capacity);
        obj.emplace_back("learned_constraint_reduction_rate",
                         this->learned_constraint_reduction_rate);
        obj.emplace_back("literal_block_distance_protection_threshold",
                         this->literal_block_distance_protection_threshold);

        obj.emplace_back("variable_activity_decay_rate",
                         this->variable_activity_decay_rate);
        obj.emplace_back("is_enabled_phase_saving",
                         this->is_enabled_phase_saving);

        return obj;
    }
};
}  // namespace printemps::option

#endif
/*****************************************************************************/
// END
/*****************************************************************************/
