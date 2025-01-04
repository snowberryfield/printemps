/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
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

    static constexpr int                                 //
        DEFAULT_NUMBER_OF_THREADS_MOVE_EVALUATION = -1;  // Adjust to max
    static constexpr int                                 //
        DEFAULT_NUMBER_OF_THREADS_MOVE_UPDATE = -1;      // Adjust to max

    static constexpr bool  //
        DEFAULT_IS_ENABLED_THREAD_COUNT_OPTIMIZATION = true;
    static constexpr double  //
        DEFAULT_THREAD_COUNT_OPTIMIZATION_DECAY_FACTOR = 0.5;
};

/*****************************************************************************/
struct ParallelOption {
    bool is_enabled_move_update_parallelization;
    bool is_enabled_move_evaluation_parallelization;

    int number_of_threads_move_update;
    int number_of_threads_move_evaluation;

    bool   is_enabled_thread_count_optimization;
    double thread_count_optimization_decay_factor;

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

        this->number_of_threads_move_update  //
            = ParallelOptionConstant::DEFAULT_NUMBER_OF_THREADS_MOVE_UPDATE;

        this->number_of_threads_move_evaluation  //
            = ParallelOptionConstant::DEFAULT_NUMBER_OF_THREADS_MOVE_EVALUATION;

        this->is_enabled_thread_count_optimization  //
            = ParallelOptionConstant::
                DEFAULT_IS_ENABLED_THREAD_COUNT_OPTIMIZATION;

        this->thread_count_optimization_decay_factor  //
            = ParallelOptionConstant::
                DEFAULT_THREAD_COUNT_OPTIMIZATION_DECAY_FACTOR;
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

        utility::print(                                       //
            " -- parallel.number_of_threads_move_update: " +  //
            utility::to_string(                               //
                this->number_of_threads_move_update, "%d"));

        utility::print(                                           //
            " -- parallel.number_of_threads_move_evaluation: " +  //
            utility::to_string(                                   //
                this->number_of_threads_move_evaluation, "%d"));

        utility::print(  //
            " -- parallel.is_enabled_thread_count_optimization:"
            " " +                       //
            utility::to_true_or_false(  //
                this->is_enabled_thread_count_optimization));

        utility::print(                                                //
            " -- parallel.thread_count_optimization_decay_factor: " +  //
            utility::to_string(                                        //
                this->thread_count_optimization_decay_factor, "%f"));
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

        read_json(                                 //
            &this->number_of_threads_move_update,  //
            "number_of_threads_move_update;",      //
            a_OBJECT);

        read_json(                                     //
            &this->number_of_threads_move_evaluation,  //
            "number_of_threads_move_evaluation",       //
            a_OBJECT);

        read_json(                                        //
            &this->is_enabled_thread_count_optimization,  //
            "is_enabled_thread_count_optimization",       //
            a_OBJECT);

        read_json(                                          //
            &this->thread_count_optimization_decay_factor,  //
            "thread_count_optimization_decay_factor",       //
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

        obj.emplace_back(                     //
            "number_of_threads_move_update",  //
            this->number_of_threads_move_update);

        obj.emplace_back(                         //
            "number_of_threads_move_evaluation",  //
            this->number_of_threads_move_evaluation);

        obj.emplace_back(                            //
            "is_enabled_thread_count_optimization",  //
            this->is_enabled_thread_count_optimization);

        obj.emplace_back(                              //
            "thread_count_optimization_decay_factor",  //
            this->thread_count_optimization_decay_factor);

        return obj;
    }
};
}  // namespace printemps::option

#endif
/*****************************************************************************/
// END
/*****************************************************************************/