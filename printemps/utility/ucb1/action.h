/*****************************************************************************/
// Copyright (c) 2020-2023 Yuji KOGUMA
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

    double total_score;
    double mean;
    double min;
    double max;
    double range;
    double confidence;

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
        this->min               = std::numeric_limits<double>::max();
        this->max               = std::numeric_limits<double>::lowest();
        this->range             = 0.0;
        this->confidence        = 0.0;
    }

    /*************************************************************************/
    inline void learn(const double a_SCORE,
                      const double a_DECAY_FACTOR) noexcept {
        this->number_of_samples++;
        this->total_score += a_SCORE;

        if (this->number_of_samples == 1) {
            this->mean  = a_SCORE;
            this->max   = a_SCORE;
            this->min   = a_SCORE;
            this->range = fabs(a_SCORE);
            return;
        }

        this->min = std::min(
            (this->min - this->mean) * a_DECAY_FACTOR + this->mean, a_SCORE);
        this->max = std::max(
            (this->max - this->mean) * a_DECAY_FACTOR + this->mean, a_SCORE);
        this->mean =
            a_DECAY_FACTOR * this->mean + (1.0 - a_DECAY_FACTOR) * a_SCORE;
        this->range = this->max - this->min;
    }

    /*************************************************************************/
    inline void update_confidence(
        const double a_TOTAL_NUMBER_OF_SAMPLES) noexcept {
        if (this->number_of_samples == 0) {
            return;
        }
        this->confidence =
            this->range * sqrt(2.0 * log(a_TOTAL_NUMBER_OF_SAMPLES) /
                               this->number_of_samples);
    }
};
}  // namespace printemps::utility::ucb1
#endif
/*****************************************************************************/
// END
/*****************************************************************************/