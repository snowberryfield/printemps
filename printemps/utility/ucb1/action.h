/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_UTILITY_UCB1_ACTION_H__
#define PRINTEMPS_UTILITY_UCB1_ACTION_H__

namespace printemps::utility::ucb1 {
/*****************************************************************************/
template <class T_ActionBody>
struct Action {
    T_ActionBody body;
    long         number_of_samples;
    double       total_score;
    double       mean;
    double       confidence;

    /*************************************************************************/
    Action(void) {
        this->initialize();
    }

    /*************************************************************************/
    Action(const T_ActionBody &a_ACTION_BODY) {
        this->initialize();
        this->body = a_ACTION_BODY;
    }

    /*************************************************************************/
    inline void initialize(void) {
        this->number_of_samples = 0;
        this->total_score       = 0.0;
        this->mean              = 0.0;
        this->confidence        = 0.0;
    }
};
}  // namespace printemps::utility::ucb1
#endif
/*****************************************************************************/
// END
/*****************************************************************************/