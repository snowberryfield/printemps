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

    long   number_of_samples;
    double number_of_effective_samples;
    double total_score;
    double mean;
    double variance;
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
        this->number_of_samples           = 0;
        this->number_of_effective_samples = 0.0;
        this->total_score                 = 0.0;
        this->mean                        = 0.0;
        this->variance                    = 0.0;
        this->confidence                  = 0.0;
    }

    /*************************************************************************/
    inline void learn(const double a_SCORE,
                      const double a_DISCOUNT_FACTOR) noexcept {
        this->number_of_samples++;
        this->number_of_effective_samples =
            1.0 + a_DISCOUNT_FACTOR * this->number_of_effective_samples;

        this->total_score += a_SCORE;

        if (this->number_of_samples == 1) {
            this->mean     = a_SCORE;
            this->variance = 0.0;
            return;
        }

        const double MEAN     = this->mean;
        const double VARIANCE = this->variance;
        this->mean =
            a_DISCOUNT_FACTOR * MEAN + (1.0 - a_DISCOUNT_FACTOR) * a_SCORE;
        this->variance  //
            = a_DISCOUNT_FACTOR * (VARIANCE + MEAN * MEAN) +
              (1 - a_DISCOUNT_FACTOR) * a_SCORE * a_SCORE -
              this->mean * this->mean;
    }

    /*************************************************************************/
    inline void forget(const double a_DISCOUNT_FACTOR) noexcept {
        this->number_of_effective_samples *= a_DISCOUNT_FACTOR;
    }

    /*************************************************************************/
    inline constexpr void update_confidence(
        const double a_TOTAL_NUMBER_OF_EFFECTIVE_SAMPLES) noexcept {
        if (this->number_of_samples == 0) {
            return;
        }

        const double NUMERATOR = log(a_TOTAL_NUMBER_OF_EFFECTIVE_SAMPLES);
        const double DENOMINATOR =
            std::max(constant::EPSILON_10, this->number_of_effective_samples);

        const double SCALER = this->number_of_samples == 1
                                  ? fabs(this->mean)
                                  : sqrt(3.0 * fabs(this->variance));

        constexpr double ZETA = 1.0;

        this->confidence = SCALER * 2.0 * sqrt(ZETA * NUMERATOR / DENOMINATOR);
    }
};
}  // namespace printemps::utility::ucb1
#endif
/*****************************************************************************/
// END
/*****************************************************************************/