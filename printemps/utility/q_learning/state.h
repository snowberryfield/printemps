/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_UTILITY_Q_LEARNING_STATE_H__
#define PRINTEMPS_UTILITY_Q_LEARNING_STATE_H__

namespace printemps::utility::q_learning {
/*****************************************************************************/
template <class T_StateBody, class T_ActionBody>
class Learner;

/*****************************************************************************/
template <class T_StateBody, class T_ActionBody>
struct State;

/*****************************************************************************/
template <class T_StateBody, class T_ActionBody>
struct Action;

/*****************************************************************************/
template <class T_StateBody, class T_ActionBody>
struct State {
    Learner<T_StateBody, T_ActionBody> *           learner_ptr;
    std::vector<Action<T_StateBody, T_ActionBody>> actions;
    T_StateBody                                    body;

    Action<T_StateBody, T_ActionBody> *best_action_ptr;

    /*************************************************************************/
    State(void) {
        this->initialize();
    }

    /*************************************************************************/
    State(const T_StateBody &a_STATE_BODY) {
        this->initialize();
        this->body = a_STATE_BODY;
    }

    /*************************************************************************/
    inline void initialize(void) {
        this->learner_ptr = nullptr;
        this->actions.clear();
        this->best_action_ptr = nullptr;
    }

    /*************************************************************************/
    inline State<T_StateBody, T_ActionBody> &add_action(
        const T_ActionBody &a_ACTION_BODY) {
        this->actions.emplace_back(a_ACTION_BODY);
        this->actions.back().state_ptr = this;
        return *this;
    }

    /**************************************************************************/
    inline void update_best_action(void) {
        Action<T_StateBody, T_ActionBody> *best_action_ptr =
            &(this->actions.front());
        double q_value_max = std::numeric_limits<double>::lowest();
        for (auto &&action : this->actions) {
            if (action.q_value > q_value_max) {
                best_action_ptr = &action;
                q_value_max     = action.q_value;
            }
        }
        this->best_action_ptr = best_action_ptr;
    }

    /**************************************************************************/
    inline Action<T_StateBody, T_ActionBody>
        *epsilon_greedy_selected_action_ptr(const double  a_RATIO,
                                            std::mt19937 *a_get_rand_mt) {
        constexpr std::uint_fast32_t RAND_MAX_UINT_32 =
            std::numeric_limits<std::uint_fast32_t>::max();
        if (static_cast<double>((*a_get_rand_mt)()) / RAND_MAX_UINT_32 <
            a_RATIO) {
            return this->best_action_ptr;
        }
        return &(this->actions[(*a_get_rand_mt)() % this->actions.size()]);
    }

    /**************************************************************************/
    inline constexpr void observe(const double a_REWARD,
                                  const double a_LEARNING_RATE,
                                  const double a_DISCOUNT_RATE) {
        auto action_ptr = this->learner_ptr->m_current_action_ptr;
        action_ptr->q_value =
            (1.0 - a_LEARNING_RATE) * action_ptr->q_value +
            a_LEARNING_RATE *
                (a_REWARD + a_DISCOUNT_RATE * this->best_action_ptr->q_value);
        this->learner_ptr->m_current_state_ptr = this;
        this->update_best_action();
    }
};

}  // namespace printemps::utility::q_learning

#endif
/*****************************************************************************/
// END
/*****************************************************************************/