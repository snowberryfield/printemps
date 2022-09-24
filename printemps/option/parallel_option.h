/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_OPTION_PARALLEL_OPTION_H__
#define PRINTEMPS_OPTION_PARALLEL_OPTION_H__

namespace printemps::option {
/*****************************************************************************/
struct ParallelOptionConstant {
    static constexpr bool DEFAULT_IS_ENABLED_PARALLEL_EVALUATION = true;
    static constexpr bool DEFAULT_IS_ENABLED_PARALLEL_NEIGHBORHOOD_UPDATE =
        true;
    static constexpr bool
        DEFAULT_IS_ENABLED_AUTOMATIC_EVALUATION_PARALLELIZATION = true;
    static constexpr bool
        DEFAULT_IS_ENABLED_AUTOMATIC_NEIGHBORHOOD_UPDATE_PARALLELIZATION = true;
};

/*****************************************************************************/
struct ParallelOption {
    bool is_enabled_parallel_evaluation;
    bool is_enabled_parallel_neighborhood_update;
    bool is_enabled_automatic_evaluation_parallelization;
    bool is_enabled_automatic_neighborhood_update_parallelization;

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
        this->is_enabled_parallel_evaluation =
            ParallelOptionConstant::DEFAULT_IS_ENABLED_PARALLEL_EVALUATION;
        this->is_enabled_parallel_neighborhood_update = ParallelOptionConstant::
            DEFAULT_IS_ENABLED_PARALLEL_NEIGHBORHOOD_UPDATE;
        this->is_enabled_automatic_evaluation_parallelization =
            ParallelOptionConstant::
                DEFAULT_IS_ENABLED_AUTOMATIC_EVALUATION_PARALLELIZATION;
        this->is_enabled_automatic_neighborhood_update_parallelization =
            ParallelOptionConstant::
                DEFAULT_IS_ENABLED_AUTOMATIC_NEIGHBORHOOD_UPDATE_PARALLELIZATION;
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
    }

    /**************************************************************************/
    inline void setup(const utility::json::JsonObject &a_OBJECT) {
        this->initialize();

        read_json(                                  //
            &this->is_enabled_parallel_evaluation,  //
            "is_enabled_parallel_evaluation", a_OBJECT);

        read_json(                                           //
            &this->is_enabled_parallel_neighborhood_update,  //
            "is_enabled_parallel_neighborhood_update", a_OBJECT);

        read_json(                                                   //
            &this->is_enabled_automatic_evaluation_parallelization,  //
            "is_enabled_automatic_evaluation_parallelization", a_OBJECT);

        read_json(                                                            //
            &this->is_enabled_automatic_neighborhood_update_parallelization,  //
            "is_enabled_automatic_neighborhood_update_parallelization",
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
        return obj;
    }
};
}  // namespace printemps::option

#endif
/*****************************************************************************/
// END
/*****************************************************************************/