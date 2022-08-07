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
};

/*****************************************************************************/
struct ParallelOption {
    bool is_enabled_parallel_evaluation;
    bool is_enabled_parallel_neighborhood_update;

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
    }

    /**************************************************************************/
    inline void setup(const utility::json::JsonObject &a_OBJECT) {
        this->initialize();

        read_json(&this->is_enabled_parallel_evaluation,  //
                  "is_enabled_parallel_evaluation", a_OBJECT);

        read_json(&this->is_enabled_parallel_neighborhood_update,  //
                  "is_enabled_parallel_neighborhood_update", a_OBJECT);
    }
};
}  // namespace printemps::option

#endif
/*****************************************************************************/
// END
/*****************************************************************************/