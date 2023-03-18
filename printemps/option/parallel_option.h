/*****************************************************************************/
// Copyright (c) 2020-2023 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_OPTION_PARALLEL_OPTION_H__
#define PRINTEMPS_OPTION_PARALLEL_OPTION_H__

namespace printemps::option {
/*****************************************************************************/
struct ParallelOptionConstant {
    static constexpr bool  //
        DEFAULT_IS_ENABLED_PARALLEL_EVALUATION = true;
    static constexpr bool  //
        DEFAULT_IS_ENABLED_PARALLEL_NEIGHBORHOOD_UPDATE = true;
    static constexpr bool  //
        DEFAULT_IS_ENABLED_AUTOMATIC_EVALUATION_PARALLELIZATION = true;
    static constexpr bool  //
        DEFAULT_IS_ENABLED_AUTOMATIC_NEIGHBORHOOD_UPDATE_PARALLELIZATION = true;
    static constexpr double  //
        DEFAULT_EVALUATION_PARALLELIZATION_DECAY_FACTOR = 0.5;
    static constexpr double  //
        DEFAULT_NEIGHBORHOOD_UPDATE_PARALLELIZATION_DECAY_FACTOR = 0.5;
    static constexpr int                                     //
        DEFAULT_NUMBER_OF_THREADS_EVALUATION = -1;           // Adjust to max
    static constexpr int                                     //
        DEFAULT_NUMBER_OF_THREADS_NEIGHBORHOOD_UPDATE = -1;  // Adjust to max
};

/*****************************************************************************/
struct ParallelOption {
    bool   is_enabled_parallel_evaluation;
    bool   is_enabled_parallel_neighborhood_update;
    bool   is_enabled_automatic_evaluation_parallelization;
    bool   is_enabled_automatic_neighborhood_update_parallelization;
    double evaluation_parallelization_decay_factor;
    double neighborhood_update_parallelization_decay_factor;
    int    number_of_threads_evaluation;
    int    number_of_threads_neighborhood_update;

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
        this->is_enabled_parallel_evaluation  //
            = ParallelOptionConstant::DEFAULT_IS_ENABLED_PARALLEL_EVALUATION;

        this->is_enabled_parallel_neighborhood_update  //
            = ParallelOptionConstant::
                DEFAULT_IS_ENABLED_PARALLEL_NEIGHBORHOOD_UPDATE;

        this->is_enabled_automatic_evaluation_parallelization  //
            = ParallelOptionConstant::
                DEFAULT_IS_ENABLED_AUTOMATIC_EVALUATION_PARALLELIZATION;

        this->is_enabled_automatic_neighborhood_update_parallelization  //
            = ParallelOptionConstant::
                DEFAULT_IS_ENABLED_AUTOMATIC_NEIGHBORHOOD_UPDATE_PARALLELIZATION;

        this->evaluation_parallelization_decay_factor  //
            = ParallelOptionConstant::
                DEFAULT_EVALUATION_PARALLELIZATION_DECAY_FACTOR;

        this->neighborhood_update_parallelization_decay_factor  //
            = ParallelOptionConstant::
                DEFAULT_NEIGHBORHOOD_UPDATE_PARALLELIZATION_DECAY_FACTOR;

        this->number_of_threads_evaluation  //
            = ParallelOptionConstant::DEFAULT_NUMBER_OF_THREADS_EVALUATION;

        this->number_of_threads_neighborhood_update  //
            = ParallelOptionConstant::
                DEFAULT_NUMBER_OF_THREADS_NEIGHBORHOOD_UPDATE;
    }

    /*************************************************************************/
    void print(void) const {
        utility::print(                                        //
            " -- parallel.is_enabled_parallel_evaluation: " +  //
            utility::to_true_or_false(                         //
                this->is_enabled_parallel_evaluation));

        utility::print(                                                 //
            " -- parallel.is_enabled_parallel_neighborhood_update: " +  //
            utility::to_true_or_false(                                  //
                this->is_enabled_parallel_neighborhood_update));

        utility::print(  //
            " -- parallel.is_enabled_automatic_evaluation_parallelization: " +  //
            utility::to_true_or_false(  //
                this->is_enabled_automatic_evaluation_parallelization));

        utility::print(  //
            " -- "
            "parallel.is_enabled_automatic_neighborhood_update_parallelization:"
            " " +                       //
            utility::to_true_or_false(  //
                this->is_enabled_automatic_neighborhood_update_parallelization));

        utility::print(  //
            " -- "
            "parallel.evaluation_parallelization_decay_factor:"
            " " +                //
            utility::to_string(  //
                this->evaluation_parallelization_decay_factor, "%f"));

        utility::print(  //
            " -- "
            "parallel.neighborhood_update_parallelization_decay_factor:"
            " " +                //
            utility::to_string(  //
                this->neighborhood_update_parallelization_decay_factor, "%f"));

        utility::print(  //
            " -- "
            "parallel.number_of_threads_evaluation:"
            " " +                //
            utility::to_string(  //
                this->number_of_threads_evaluation, "%d"));

        utility::print(  //
            " -- "
            "parallel.number_of_threads_neighborhood_update:"
            " " +                //
            utility::to_string(  //
                this->number_of_threads_neighborhood_update, "%d"));
    }

    /**************************************************************************/
    inline void setup(const utility::json::JsonObject &a_OBJECT) {
        this->initialize();

        read_json(                                  //
            &this->is_enabled_parallel_evaluation,  //
            "is_enabled_parallel_evaluation",       //
            a_OBJECT);

        read_json(                                           //
            &this->is_enabled_parallel_neighborhood_update,  //
            "is_enabled_parallel_neighborhood_update",       //
            a_OBJECT);

        read_json(                                                   //
            &this->is_enabled_automatic_evaluation_parallelization,  //
            "is_enabled_automatic_evaluation_parallelization",       //
            a_OBJECT);

        read_json(                                                            //
            &this->is_enabled_automatic_neighborhood_update_parallelization,  //
            "is_enabled_automatic_neighborhood_update_parallelization",       //
            a_OBJECT);

        read_json(                                           //
            &this->evaluation_parallelization_decay_factor,  //
            "evaluation_parallelization_decay_factor",       //
            a_OBJECT);

        read_json(                                                    //
            &this->neighborhood_update_parallelization_decay_factor,  //
            "neighborhood_update_parallelization_decay_factor",       //
            a_OBJECT);

        read_json(                                //
            &this->number_of_threads_evaluation,  //
            "number_of_threads_evaluation",       //
            a_OBJECT);

        read_json(                                         //
            &this->number_of_threads_neighborhood_update,  //
            "number_of_threads_neighborhood_update",       //
            a_OBJECT);
    }

    /**************************************************************************/
    inline utility::json::JsonObject to_json(void) const {
        utility::json::JsonObject obj;
        obj.emplace_back(                      //
            "is_enabled_parallel_evaluation",  //
            this->is_enabled_parallel_evaluation);

        obj.emplace_back(                               //
            "is_enabled_parallel_neighborhood_update",  //
            this->is_enabled_parallel_neighborhood_update);

        obj.emplace_back(                                       //
            "is_enabled_automatic_evaluation_parallelization",  //
            this->is_enabled_automatic_evaluation_parallelization);

        obj.emplace_back(                                                //
            "is_enabled_automatic_neighborhood_update_parallelization",  //
            this->is_enabled_automatic_neighborhood_update_parallelization);

        obj.emplace_back(                               //
            "evaluation_parallelization_decay_factor",  //
            this->evaluation_parallelization_decay_factor);

        obj.emplace_back(                                        //
            "neighborhood_update_parallelization_decay_factor",  //
            this->neighborhood_update_parallelization_decay_factor);

        obj.emplace_back(                    //
            "number_of_threads_evaluation",  //
            this->number_of_threads_evaluation);

        obj.emplace_back(                             //
            "number_of_threads_neighborhood_update",  //
            this->number_of_threads_neighborhood_update);

        return obj;
    }
};
}  // namespace printemps::option

#endif
/*****************************************************************************/
// END
/*****************************************************************************/