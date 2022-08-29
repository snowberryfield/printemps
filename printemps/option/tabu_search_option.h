/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_OPTION_TABU_SEARCH_OPTION_H__
#define PRINTEMPS_OPTION_TABU_SEARCH_OPTION_H__

namespace printemps::option {
/*****************************************************************************/
struct TabuSearchOptionConstant {
    static constexpr int    DEFAULT_ITERATION_MAX                       = 200;
    static constexpr double DEFAULT_TIME_MAX                            = 120.0;
    static constexpr double DEFAULT_TIME_OFFSET                         = 0.0;
    static constexpr int    DEFAULT_LOG_INTERVAL                        = 10;
    static constexpr int    DEFAULT_INITIAL_TABU_TENURE                 = 10;
    static constexpr double DEFAULT_TABU_TENURE_RANDOMIZE_RATE          = 0.3;
    static constexpr double DEFAULT_INITIAL_MODIFICATION_FIXED_RATE     = 1.0;
    static constexpr double DEFAULT_INITIAL_MODIFICATION_RANDOMIZE_RATE = 0.5;
    static constexpr tabu_mode::TabuMode DEFAULT_TABU_MODE = tabu_mode::All;
    static constexpr double              DEFAULT_MOVE_PRESERVE_RATE = 1.0;
    static constexpr double DEFAULT_FREQUENCY_PENALTY_COEFFICIENT   = 1E-4;
    static constexpr double DEFAULT_LAGRANGIAN_PENALTY_COEFFICIENT  = 1.0;
    static constexpr double DEFAULT_PRUNING_RATE_THRESHOLD          = 1.0;

    static constexpr bool DEFAULT_IS_ENABLED_SHUFFLE         = true;
    static constexpr bool DEFAULT_IS_ENABLED_MOVE_CURTAIL    = false;
    static constexpr bool DEFAULT_IS_ENABLED_AUTOMATIC_BREAK = true;
    static constexpr bool DEFAULT_IS_ENABLED_AUTOMATIC_TABU_TENURE_ADJUSTMENT =
        true;
    static constexpr bool DEFAULT_IS_ENABLED_AUTOMATIC_ITERATION_ADJUSTMENT =
        true;
    static constexpr bool   DEFAULT_IS_ENABLED_INITIAL_MODIFICATION    = true;
    static constexpr int    DEFAULT_INTENSITY_INCREASE_COUNT_THRESHOLD = 10;
    static constexpr int    DEFAULT_INTENSITY_DECREASE_COUNT_THRESHOLD = 10;
    static constexpr double DEFAULT_ITERATION_INCREASE_RATE            = 1.5;
    static constexpr double DEFAULT_ITERATION_DECREASE_RATE            = 0.9;
    static constexpr double DEFAULT_IGNORE_TABU_IF_GLOBAL_INCUMBENT    = true;
    static constexpr bool   DEFAULT_NUMBER_OF_INITIAL_MODIFICATION     = 0;
    static constexpr int    DEFAULT_SEED                               = 1;
};

/*****************************************************************************/
struct TabuSearchOption {
    int                 iteration_max;
    double              time_max;
    double              time_offset;
    int                 log_interval;
    int                 initial_tabu_tenure;
    double              tabu_tenure_randomize_rate;
    double              initial_modification_fixed_rate;
    double              initial_modification_randomize_rate;
    tabu_mode::TabuMode tabu_mode;
    double              move_preserve_rate;
    double              frequency_penalty_coefficient;
    double              lagrangian_penalty_coefficient;
    double              pruning_rate_threshold;
    bool                is_enabled_shuffle;
    bool                is_enabled_move_curtail;
    bool                is_enabled_automatic_break;
    bool                is_enabled_automatic_tabu_tenure_adjustment;
    bool                is_enabled_automatic_iteration_adjustment;
    bool                is_enabled_initial_modification;
    int                 intensity_increase_count_threshold;
    int                 intensity_decrease_count_threshold;
    double              iteration_increase_rate;
    double              iteration_decrease_rate;
    bool                ignore_tabu_if_global_incumbent;
    int                 number_of_initial_modification;
    int                 seed;

    /*************************************************************************/
    TabuSearchOption(void) {
        this->initialize();
    }

    /*************************************************************************/
    TabuSearchOption(const utility::json::JsonObject &a_OBJECT) {
        this->setup(a_OBJECT);
    }

    /*************************************************************************/
    void initialize(void) {
        this->iteration_max = TabuSearchOptionConstant::DEFAULT_ITERATION_MAX;
        this->time_max      = TabuSearchOptionConstant::DEFAULT_TIME_MAX;
        this->time_offset   = TabuSearchOptionConstant::DEFAULT_TIME_OFFSET;
        this->log_interval  = TabuSearchOptionConstant::DEFAULT_LOG_INTERVAL;
        this->initial_tabu_tenure =
            TabuSearchOptionConstant::DEFAULT_INITIAL_TABU_TENURE;
        this->tabu_tenure_randomize_rate =
            TabuSearchOptionConstant::DEFAULT_TABU_TENURE_RANDOMIZE_RATE;
        this->initial_modification_fixed_rate =
            TabuSearchOptionConstant::DEFAULT_INITIAL_MODIFICATION_FIXED_RATE;
        this->initial_modification_randomize_rate = TabuSearchOptionConstant::
            DEFAULT_INITIAL_MODIFICATION_RANDOMIZE_RATE;
        this->tabu_mode = TabuSearchOptionConstant::DEFAULT_TABU_MODE;
        this->move_preserve_rate =
            TabuSearchOptionConstant::DEFAULT_MOVE_PRESERVE_RATE;
        this->frequency_penalty_coefficient =
            TabuSearchOptionConstant::DEFAULT_FREQUENCY_PENALTY_COEFFICIENT;
        this->lagrangian_penalty_coefficient =
            TabuSearchOptionConstant::DEFAULT_LAGRANGIAN_PENALTY_COEFFICIENT;
        this->pruning_rate_threshold =
            TabuSearchOptionConstant::DEFAULT_PRUNING_RATE_THRESHOLD;
        this->is_enabled_shuffle =
            TabuSearchOptionConstant::DEFAULT_IS_ENABLED_SHUFFLE;
        this->is_enabled_move_curtail =
            TabuSearchOptionConstant::DEFAULT_IS_ENABLED_MOVE_CURTAIL;
        this->is_enabled_automatic_break =
            TabuSearchOptionConstant::DEFAULT_IS_ENABLED_AUTOMATIC_BREAK;
        this->is_enabled_automatic_tabu_tenure_adjustment =
            TabuSearchOptionConstant::
                DEFAULT_IS_ENABLED_AUTOMATIC_TABU_TENURE_ADJUSTMENT;
        this->is_enabled_automatic_iteration_adjustment =
            TabuSearchOptionConstant::
                DEFAULT_IS_ENABLED_AUTOMATIC_ITERATION_ADJUSTMENT;
        this->is_enabled_initial_modification =
            TabuSearchOptionConstant::DEFAULT_IS_ENABLED_INITIAL_MODIFICATION;
        this->intensity_increase_count_threshold = TabuSearchOptionConstant::
            DEFAULT_INTENSITY_INCREASE_COUNT_THRESHOLD;
        this->intensity_decrease_count_threshold = TabuSearchOptionConstant::
            DEFAULT_INTENSITY_DECREASE_COUNT_THRESHOLD;
        this->iteration_increase_rate =
            TabuSearchOptionConstant::DEFAULT_ITERATION_INCREASE_RATE;
        this->iteration_decrease_rate =
            TabuSearchOptionConstant::DEFAULT_ITERATION_DECREASE_RATE;
        this->ignore_tabu_if_global_incumbent =
            TabuSearchOptionConstant::DEFAULT_IGNORE_TABU_IF_GLOBAL_INCUMBENT;
        this->number_of_initial_modification =
            TabuSearchOptionConstant::DEFAULT_NUMBER_OF_INITIAL_MODIFICATION;
        this->seed = TabuSearchOptionConstant::DEFAULT_SEED;
    }

    /*************************************************************************/
    void print(void) const {
        utility::print(                          //
            " -- tabu_search.iteration_max: " +  //
            utility::to_string(                  //
                this->iteration_max, "%d"));

        utility::print(                     //
            " -- tabu_search.time_max: " +  //
            utility::to_string(             //
                this->time_max, "%f"));

        utility::print(                        //
            " -- tabu_search.time_offset: " +  //
            utility::to_string(                //
                this->time_offset, "%f"));

        utility::print(                         //
            " -- tabu_search.log_interval: " +  //
            utility::to_string(                 //
                this->log_interval, "%d"));

        utility::print(                                //
            " -- tabu_search.initial_tabu_tenure: " +  //
            utility::to_string(                        //
                this->initial_tabu_tenure, "%d"));

        utility::print(                                       //
            " -- tabu_search.tabu_tenure_randomize_rate: " +  //
            utility::to_string(                               //
                this->tabu_tenure_randomize_rate, "%f"));

        utility::print(                                            //
            " -- tabu_search.initial_modification_fixed_rate: " +  //
            utility::to_string(                                    //
                this->initial_modification_fixed_rate, "%f"));

        utility::print(                                                //
            " -- tabu_search.initial_modification_randomize_rate: " +  //
            utility::to_string(                                        //
                this->initial_modification_randomize_rate, "%f"));

        utility::print(                      //
            " -- tabu_search.tabu_mode: " +  //
            tabu_mode::TabuModeInverseMap.at(this->tabu_mode));

        utility::print(                               //
            " -- tabu_search.move_preserve_rate: " +  //
            utility::to_string(                       //
                this->move_preserve_rate, "%f"));

        utility::print(                                          //
            " -- tabu_search.frequency_penalty_coefficient: " +  //
            utility::to_string(                                  //
                this->frequency_penalty_coefficient, "%f"));

        utility::print(                                           //
            " -- tabu_search.lagrangian_penalty_coefficient: " +  //
            utility::to_string(                                   //
                this->lagrangian_penalty_coefficient, "%f"));

        utility::print(                                   //
            " -- tabu_search.pruning_rate_threshold: " +  //
            utility::to_string(                           //
                this->pruning_rate_threshold, "%f"));

        utility::print(                               //
            " -- tabu_search.is_enabled_shuffle: " +  //
            utility::to_true_or_false(                //
                this->is_enabled_shuffle));

        utility::print(                                    //
            " -- tabu_search.is_enabled_move_curtail: " +  //
            utility::to_true_or_false(                     //
                this->is_enabled_move_curtail));

        utility::print(                                       //
            " -- tabu_search.is_enabled_automatic_break: " +  //
            utility::to_true_or_false(                        //
                this->is_enabled_automatic_break));

        utility::print(  //
            " -- tabu_search.is_enabled_automatic_tabu_tenure_adjustment: " +  //
            utility::to_true_or_false(  //
                this->is_enabled_automatic_tabu_tenure_adjustment));

        utility::print(                                                      //
            " -- tabu_search.is_enabled_automatic_iteration_adjustment: " +  //
            utility::to_true_or_false(                                       //
                this->is_enabled_automatic_iteration_adjustment));

        utility::print(                                            //
            " -- tabu_search.is_enabled_initial_modification: " +  //
            utility::to_true_or_false(                             //
                this->is_enabled_initial_modification));

        utility::print(                                               //
            " -- tabu_search.intensity_increase_count_threshold: " +  //
            utility::to_string(                                       //
                this->intensity_increase_count_threshold, "%d"));

        utility::print(                                               //
            " -- tabu_search.intensity_decrease_count_threshold: " +  //
            utility::to_string(                                       //
                this->intensity_decrease_count_threshold, "%d"));

        utility::print(                                    //
            " -- tabu_search.iteration_increase_rate: " +  //
            utility::to_string(                            //
                this->iteration_increase_rate, "%f"));

        utility::print(                                    //
            " -- tabu_search.iteration_decrease_rate: " +  //
            utility::to_string(                            //
                this->iteration_decrease_rate, "%f"));

        utility::print(                                            //
            " -- tabu_search.ignore_tabu_if_global_incumbent: " +  //
            utility::to_true_or_false(                             //
                this->ignore_tabu_if_global_incumbent));

        utility::print(                                           //
            " -- tabu_search.number_of_initial_modification: " +  //
            utility::to_string(                                   //
                this->number_of_initial_modification, "%d"));

        utility::print(               //
            " -- tabu_search.seed: "  //
            + utility::to_string(     //
                  this->seed, "%d"));
    }

    /**************************************************************************/
    inline void setup(const utility::json::JsonObject &a_OBJECT) {
        this->initialize();

        read_json(                 //
            &this->iteration_max,  //
            "iteration_max", a_OBJECT);

        read_json(            //
            &this->time_max,  //
            "time_max", a_OBJECT);

        read_json(               //
            &this->time_offset,  //
            "time_offset", a_OBJECT);

        read_json(                //
            &this->log_interval,  //
            "log_interval", a_OBJECT);

        read_json(                       //
            &this->initial_tabu_tenure,  //
            "initial_tabu_tenure", a_OBJECT);

        read_json(                              //
            &this->tabu_tenure_randomize_rate,  //
            "tabu_tenure_randomize_rate", a_OBJECT);

        read_json(                                   //
            &this->initial_modification_fixed_rate,  //
            "initial_modification_fixed_rate", a_OBJECT);

        read_json(                                       //
            &this->initial_modification_randomize_rate,  //
            "initial_modification_randomize_rate", a_OBJECT);

        read_json(             //
            &this->tabu_mode,  //
            "tabu_mode", a_OBJECT);

        read_json(                      //
            &this->move_preserve_rate,  //
            "move_preserve_rate", a_OBJECT);

        read_json(                                 //
            &this->frequency_penalty_coefficient,  //
            "frequency_penalty_coefficient", a_OBJECT);

        read_json(                                  //
            &this->lagrangian_penalty_coefficient,  //
            "lagrangian_penalty_coefficient", a_OBJECT);

        read_json(                          //
            &this->pruning_rate_threshold,  //
            "pruning_rate_threshold", a_OBJECT);

        read_json(                      //
            &this->is_enabled_shuffle,  //
            "is_enabled_shuffle", a_OBJECT);

        read_json(                           //
            &this->is_enabled_move_curtail,  //
            "is_enabled_move_curtail", a_OBJECT);

        read_json(                              //
            &this->is_enabled_automatic_break,  //
            "is_enabled_automatic_break", a_OBJECT);

        read_json(                                               //
            &this->is_enabled_automatic_tabu_tenure_adjustment,  //
            "is_enabled_automatic_tabu_tenure_adjustment", a_OBJECT);

        read_json(                                             //
            &this->is_enabled_automatic_iteration_adjustment,  //
            "is_enabled_automatic_iteration_adjustment", a_OBJECT);

        read_json(                                   //
            &this->is_enabled_initial_modification,  //
            "is_enabled_initial_modification", a_OBJECT);

        read_json(                                      //
            &this->intensity_increase_count_threshold,  //
            "intensity_increase_count_threshold", a_OBJECT);

        read_json(                                      //
            &this->intensity_decrease_count_threshold,  //
            "intensity_decrease_count_threshold", a_OBJECT);

        read_json(                           //
            &this->iteration_increase_rate,  //
            "iteration_increase_rate", a_OBJECT);

        read_json(                           //
            &this->iteration_decrease_rate,  //
            "iteration_decrease_rate", a_OBJECT);

        read_json(                                   //
            &this->ignore_tabu_if_global_incumbent,  //
            "ignore_tabu_if_global_incumbent", a_OBJECT);

        read_json(                                  //
            &this->number_of_initial_modification,  //
            "number_of_initial_modification", a_OBJECT);

        read_json(        //
            &this->seed,  //
            "seed", a_OBJECT);
    }

    /**************************************************************************/
    inline utility::json::JsonObject to_json(void) const {
        utility::json::JsonObject obj;
        obj.emplace_back(     //
            "iteration_max",  //
            this->iteration_max);

        obj.emplace_back(  //
            "time_max",    //
            this->time_max);

        obj.emplace_back(   //
            "time_offset",  //
            this->time_offset);

        obj.emplace_back(    //
            "log_interval",  //
            this->log_interval);

        obj.emplace_back(           //
            "initial_tabu_tenure",  //
            this->initial_tabu_tenure);

        obj.emplace_back(                  //
            "tabu_tenure_randomize_rate",  //
            this->tabu_tenure_randomize_rate);

        obj.emplace_back(  //
            "initial_modification_fixed_rate",
            this->initial_modification_fixed_rate);

        obj.emplace_back(  //
            "initial_modification_randomize_rate",
            this->initial_modification_randomize_rate);

        obj.emplace_back(  //
            "tabu_mode",   //
            tabu_mode::TabuModeInverseMap.at(this->tabu_mode));

        obj.emplace_back(          //
            "move_preserve_rate",  //
            this->move_preserve_rate);

        obj.emplace_back(                     //
            "frequency_penalty_coefficient",  //
            this->frequency_penalty_coefficient);

        obj.emplace_back(                      //
            "lagrangian_penalty_coefficient",  //
            this->lagrangian_penalty_coefficient);

        obj.emplace_back(              //
            "pruning_rate_threshold",  //
            this->pruning_rate_threshold);

        obj.emplace_back(          //
            "is_enabled_shuffle",  //
            this->is_enabled_shuffle);

        obj.emplace_back(               //
            "is_enabled_move_curtail",  //
            this->is_enabled_move_curtail);

        obj.emplace_back(                  //
            "is_enabled_automatic_break",  //
            this->is_enabled_automatic_break);

        obj.emplace_back(                                   //
            "is_enabled_automatic_tabu_tenure_adjustment",  //
            this->is_enabled_automatic_tabu_tenure_adjustment);

        obj.emplace_back(                                 //
            "is_enabled_automatic_iteration_adjustment",  //
            this->is_enabled_automatic_iteration_adjustment);

        obj.emplace_back(                       //
            "is_enabled_initial_modification",  //
            this->is_enabled_initial_modification);

        obj.emplace_back(                          //
            "intensity_increase_count_threshold",  //
            this->intensity_increase_count_threshold);

        obj.emplace_back(                          //
            "intensity_decrease_count_threshold",  //
            this->intensity_decrease_count_threshold);

        obj.emplace_back(               //
            "iteration_increase_rate",  //
            this->iteration_increase_rate);

        obj.emplace_back(               //
            "iteration_decrease_rate",  //
            this->iteration_decrease_rate);

        obj.emplace_back(                       //
            "ignore_tabu_if_global_incumbent",  //
            this->ignore_tabu_if_global_incumbent);

        obj.emplace_back(                      //
            "number_of_initial_modification",  //
            this->number_of_initial_modification);

        obj.emplace_back(  //
            "seed",        //
            this->seed);

        return obj;
    }
};
}  // namespace printemps::option

#endif
/*****************************************************************************/
// END
/*****************************************************************************/