/*****************************************************************************/
// Copyright (c) 2020-2024 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_OPTION_PARALLEL_OPTION_H__
#define PRINTEMPS_OPTION_PARALLEL_OPTION_H__

namespace printemps::option {
/*****************************************************************************/
struct ParallelOptionConstant {
    static constexpr bool  //
        DEFAULT_IS_ENABLED_MOVE_UPDATE_PARALLELIZATION = true;
    static constexpr bool  //
        DEFAULT_IS_ENABLED_MOVE_EVALUATION_PARALLELIZATION = true;

    static constexpr bool  //
        DEFAULT_IS_ENABLED_AUTOMATIC_MOVE_UPDATE_PARALLELIZATION = true;
    static constexpr bool  //
        DEFAULT_IS_ENABLED_AUTOMATIC_MOVE_EVALUATION_PARALLELIZATION = true;

    static constexpr double  //
        DEFAULT_DECAY_FACTOR_MOVE_UPDATE = 0.5;
    static constexpr double  //
        DEFAULT_DECAY_FACTOR_MOVE_EVALUATION = 0.5;

    static constexpr int                                 //
        DEFAULT_NUMBER_OF_THREADS_MOVE_EVALUATION = -1;  // Adjust to max
    static constexpr int                                 //
        DEFAULT_NUMBER_OF_THREADS_MOVE_UPDATE = -1;      // Adjust to max
};

/*****************************************************************************/
struct ParallelOption {
    bool is_enabled_move_update_parallelization;
    bool is_enabled_move_evaluation_parallelization;

    bool is_enabled_automatic_move_update_parallelization;
    bool is_enabled_automatic_move_evaluation_parallelization;

    double decay_factor_move_update;
    double decay_factor_move_evaluation;

    int number_of_threads_move_update;
    int number_of_threads_move_evaluation;

    /*************************************************************************/
    ParallelOption(void) {
        this->initialize();
    }

    /*************************************************************************/
    ParallelOption(const utility::json::JsonObject &a_OBJECT) {
        this->setup(a_OBJECT);
    }

    /*************************************************************************/
    void initialize(void) {
        this->is_enabled_move_update_parallelization  //
            = ParallelOptionConstant::
                DEFAULT_IS_ENABLED_MOVE_UPDATE_PARALLELIZATION;

        this->is_enabled_move_evaluation_parallelization  //
            = ParallelOptionConstant::
                DEFAULT_IS_ENABLED_MOVE_EVALUATION_PARALLELIZATION;

        this->is_enabled_automatic_move_update_parallelization  //
            = ParallelOptionConstant::
                DEFAULT_IS_ENABLED_AUTOMATIC_MOVE_UPDATE_PARALLELIZATION;

        this->is_enabled_automatic_move_evaluation_parallelization  //
            = ParallelOptionConstant::
                DEFAULT_IS_ENABLED_AUTOMATIC_MOVE_EVALUATION_PARALLELIZATION;

        this->decay_factor_move_update  //
            = ParallelOptionConstant::DEFAULT_DECAY_FACTOR_MOVE_UPDATE;

        this->decay_factor_move_evaluation  //
            = ParallelOptionConstant::DEFAULT_DECAY_FACTOR_MOVE_EVALUATION;

        this->number_of_threads_move_update  //
            = ParallelOptionConstant::DEFAULT_NUMBER_OF_THREADS_MOVE_UPDATE;

        this->number_of_threads_move_evaluation  //
            = ParallelOptionConstant::DEFAULT_NUMBER_OF_THREADS_MOVE_EVALUATION;
    }

    /*************************************************************************/
    void print(void) const {
        utility::print(                                                //
            " -- parallel.is_enabled_move_update_parallelization: " +  //
            utility::to_true_or_false(                                 //
                this->is_enabled_move_update_parallelization));

        utility::print(                                                    //
            " -- parallel.is_enabled_move_evaluation_parallelization: " +  //
            utility::to_true_or_false(                                     //
                this->is_enabled_move_evaluation_parallelization));

        utility::print(  //
            " -- parallel.is_enabled_automatic_move_update_parallelization:"
            " " +                       //
            utility::to_true_or_false(  //
                this->is_enabled_automatic_move_update_parallelization));

        utility::print(  //
            " -- parallel.is_enabled_automatic_move_evaluation_parallelization:"
            " " +                       //
            utility::to_true_or_false(  //
                this->is_enabled_automatic_move_evaluation_parallelization));

        utility::print(                                  //
            " -- parallel.decay_factor_move_update: " +  //
            utility::to_string(                          //
                this->decay_factor_move_update, "%f"));

        utility::print(                                      //
            " -- parallel.decay_factor_move_evaluation: " +  //
            utility::to_string(                              //
                this->decay_factor_move_evaluation, "%f"));

        utility::print(                                       //
            " -- parallel.number_of_threads_move_update: " +  //
            utility::to_string(                               //
                this->number_of_threads_move_update, "%d"));

        utility::print(                                           //
            " -- parallel.number_of_threads_move_evaluation: " +  //
            utility::to_string(                                   //
                this->number_of_threads_move_evaluation, "%d"));
    }

    /**************************************************************************/
    inline void setup(const utility::json::JsonObject &a_OBJECT) {
        this->initialize();

        read_json(                                          //
            &this->is_enabled_move_update_parallelization,  //
            "is_enabled_move_update_parallelization",       //
            a_OBJECT);

        read_json(                                              //
            &this->is_enabled_move_evaluation_parallelization,  //
            "is_enabled_move_evaluation_parallelization",       //
            a_OBJECT);

        read_json(                                                    //
            &this->is_enabled_automatic_move_update_parallelization,  //
            "is_enabled_automatic_move_update_parallelization",       //
            a_OBJECT);

        read_json(                                                        //
            &this->is_enabled_automatic_move_evaluation_parallelization,  //
            "is_enabled_automatic_move_evaluation_parallelization",       //
            a_OBJECT);

        read_json(                            //
            &this->decay_factor_move_update,  //
            "decay_factor_move_update",       //
            a_OBJECT);

        read_json(                                //
            &this->decay_factor_move_evaluation,  //
            "decay_factor_move_evaluation",       //
            a_OBJECT);

        read_json(                                 //
            &this->number_of_threads_move_update,  //
            "number_of_threads_move_update;",      //
            a_OBJECT);

        read_json(                                     //
            &this->number_of_threads_move_evaluation,  //
            "number_of_threads_move_evaluation",       //
            a_OBJECT);
    }

    /**************************************************************************/
    inline utility::json::JsonObject to_json(void) const {
        utility::json::JsonObject obj;
        obj.emplace_back(                              //
            "is_enabled_move_update_parallelization",  //
            this->is_enabled_move_update_parallelization);

        obj.emplace_back(                                  //
            "is_enabled_move_evaluation_parallelization",  //
            this->is_enabled_move_evaluation_parallelization);

        obj.emplace_back(                                        //
            "is_enabled_automatic_move_update_parallelization",  //
            this->is_enabled_automatic_move_update_parallelization);

        obj.emplace_back(                                            //
            "is_enabled_automatic_move_evaluation_parallelization",  //
            this->is_enabled_automatic_move_evaluation_parallelization);

        obj.emplace_back(                //
            "decay_factor_move_update",  //
            this->decay_factor_move_update);

        obj.emplace_back(                    //
            "decay_factor_move_evaluation",  //
            this->decay_factor_move_evaluation);

        obj.emplace_back(                     //
            "number_of_threads_move_update",  //
            this->number_of_threads_move_update);

        obj.emplace_back(                         //
            "number_of_threads_move_evaluation",  //
            this->number_of_threads_move_evaluation);

        return obj;
    }
};
}  // namespace printemps::option

#endif
/*****************************************************************************/
// END
/*****************************************************************************/