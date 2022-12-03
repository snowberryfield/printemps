/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_UTILITY_UCB1_LEARNER_H__
#define PRINTEMPS_UTILITY_UCB1_LEARNER_H__

#include "action.h"

namespace printemps::utility::ucb1 {
/*****************************************************************************/
template <class T_ActionBody>
class Learner {
   private:
    std::vector<Action<T_ActionBody>> m_actions;
    Action<T_ActionBody> *            m_best_action_ptr;

    long   m_total_number_of_samples;
    double m_total_number_of_effective_samples;

    double m_discount_factor;

   public:
    /*************************************************************************/
    Learner(const std::vector<Action<T_ActionBody>> &a_ACTIONS,
            const double                             a_DISCOUNT_FACTOR) {
        this->setup(a_ACTIONS, a_DISCOUNT_FACTOR);
    }

    /*************************************************************************/
    Learner(void) {
        this->initialize();
    }

    /*************************************************************************/
    ~Learner(void) {
        /// nothing to do
    }

    /*************************************************************************/
    inline void initialize(void) {
        m_actions.clear();
        m_best_action_ptr = nullptr;

        m_total_number_of_samples           = 0;
        m_total_number_of_effective_samples = 0.0;
        m_discount_factor                   = 0.0;
    }

    /*************************************************************************/
    inline void setup(const std::vector<Action<T_ActionBody>> &a_ACTIONS,
                      const double a_DISCOUNT_FACTOR) {
        this->initialize();
        m_actions         = a_ACTIONS;
        m_discount_factor = a_DISCOUNT_FACTOR;
        m_best_action_ptr = &(m_actions.front());
    }

    /*************************************************************************/
    inline constexpr void learn(const double a_SCORE) {
        /**
         * NOTE: Policy for selecting arms are updated by Discounted UCB[1].
         * [1] A. Garivier, E. Moulines: On Upper-Confidence Bound Policies for
         * Non-Stationary Bandit Problems, Technical report, LTCI, 2008.
         */
        m_total_number_of_samples++;
        m_total_number_of_effective_samples =
            1.0 + m_discount_factor * m_total_number_of_effective_samples;

        for (auto &&action : m_actions) {
            if (&action == m_best_action_ptr) {
                action.learn(a_SCORE, m_discount_factor);
            } else {
                action.forget(m_discount_factor);
            }
            action.update_confidence(m_total_number_of_effective_samples);
        }

        std::vector<Action<T_ActionBody> *> no_data_action_ptrs;
        for (auto &&action : m_actions) {
            if (action.number_of_samples == 0) {
                no_data_action_ptrs.push_back(&action);
            }
        }

        Action<T_ActionBody> *best_action_ptr = nullptr;
        if (no_data_action_ptrs.size() > 0) {
            best_action_ptr = no_data_action_ptrs.front();
        } else {
            double argmax_upper_confidence =
                std::numeric_limits<double>::lowest();
            for (auto &&action : m_actions) {
                const double UPPER_CONFIDENCE = action.mean + action.confidence;
                if (UPPER_CONFIDENCE > argmax_upper_confidence) {
                    argmax_upper_confidence = UPPER_CONFIDENCE;
                    best_action_ptr         = &action;
                }
            }
        }
        m_best_action_ptr = best_action_ptr;
    }

    /*************************************************************************/
    inline constexpr std::vector<Action<T_ActionBody>> &actions(void) noexcept {
        return m_actions;
    }

    /*************************************************************************/
    inline constexpr const std::vector<Action<T_ActionBody>> &actions(
        void) const noexcept {
        return m_actions;
    }

    /*************************************************************************/
    inline constexpr Action<T_ActionBody> &best_action(void) noexcept {
        return *m_best_action_ptr;
    }

    /*************************************************************************/
    inline constexpr const Action<T_ActionBody> &best_action(void) const
        noexcept {
        return *m_best_action_ptr;
    }

    /*************************************************************************/
    inline constexpr long total_number_of_samples(void) const noexcept {
        return m_total_number_of_samples;
    }

    /*************************************************************************/
    inline constexpr double total_number_of_effective_samples(void) const
        noexcept {
        return m_total_number_of_effective_samples;
    }

    /*************************************************************************/
    inline constexpr double discount_factor(void) const noexcept {
        return m_discount_factor;
    }
};
}  // namespace printemps::utility::ucb1
#endif
/*****************************************************************************/
// END
/*****************************************************************************/