/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_UTILITY_UCB1_LEARNER_H__
#define PRINTEMPS_UTILITY_UCB1_LEARNER_H__

#include "action.h"

namespace printemps {
namespace utility {
namespace ucb1 {
/*****************************************************************************/
template <class T_ActionBody>
class Learner {
   private:
    std::vector<Action<T_ActionBody>> m_actions;
    Action<T_ActionBody> *            m_best_action_ptr;

    double m_min_score;
    double m_max_score;
    long   m_total_number_of_samples;

   public:
    /*************************************************************************/
    Learner(const std::vector<Action<T_ActionBody>> &a_ACTIONS) {
        this->setup(a_ACTIONS);
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
        m_min_score               = std::numeric_limits<double>::max();
        m_max_score               = std::numeric_limits<double>::lowest();
        m_total_number_of_samples = 0;
    }

    /*************************************************************************/
    inline void setup(const std::vector<Action<T_ActionBody>> &a_ACTIONS) {
        this->initialize();
        m_actions         = a_ACTIONS;
        m_best_action_ptr = &(m_actions.front());
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
    inline constexpr double min_score(void) const noexcept {
        return m_min_score;
    }

    /*************************************************************************/
    inline constexpr double max_score(void) const noexcept {
        return m_max_score;
    }

    /*************************************************************************/
    inline constexpr long total_number_of_samples(void) const noexcept {
        return m_total_number_of_samples;
    }

    /*************************************************************************/
    inline constexpr void learn(const double a_SCORE) {
        m_best_action_ptr->number_of_samples++;
        m_best_action_ptr->total_score += a_SCORE;
        m_best_action_ptr->mean = m_best_action_ptr->total_score /
                                  m_best_action_ptr->number_of_samples;

        m_total_number_of_samples++;
        m_max_score = std::max(m_max_score, a_SCORE);
        m_min_score = std::min(m_min_score, a_SCORE);

        double range = m_max_score - m_min_score;

        for (auto &&action : m_actions) {
            if (action.number_of_samples > 0) {
                action.confidence =
                    range * sqrt(2.0 * log(m_total_number_of_samples) /
                                 action.number_of_samples);
            }
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
                double upper_confidence = action.mean + action.confidence;
                if (upper_confidence > argmax_upper_confidence) {
                    argmax_upper_confidence = upper_confidence;
                    best_action_ptr         = &action;
                }
            }
        }
        m_best_action_ptr = best_action_ptr;
    }
};
}  // namespace ucb1
}  // namespace utility
}  // namespace printemps
#endif
/*****************************************************************************/
// END
/*****************************************************************************/