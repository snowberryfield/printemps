/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_UTILITY_Q_LEARNING_LEARNER_H__
#define PRINTEMPS_UTILITY_Q_LEARNING_LEARNER_H__

namespace printemps::utility::q_learning {
/*****************************************************************************/
template <class T_StateBody, class T_ActionBody>
struct State;

/*****************************************************************************/
template <class T_StateBody, class T_ActionBody>
struct Action;

/*****************************************************************************/
template <class T_StateBody, class T_ActionBody>
class Learner {
   private:
    friend State<T_StateBody, T_ActionBody>;
    std::vector<State<T_StateBody, T_ActionBody>> m_states;

    State<T_StateBody, T_ActionBody> * m_current_state_ptr;
    Action<T_StateBody, T_ActionBody> *m_current_action_ptr;
    std::mt19937                       m_get_rand_mt;

   public:
    /*************************************************************************/
    Learner(void) {
        this->initialize();
    }

    /*************************************************************************/
    void set_seed(const std::uint_fast32_t &a_SEED) {
        m_get_rand_mt.seed(a_SEED);
    }

    /*************************************************************************/
    void initialize(void) {
        m_states.clear();
        m_current_state_ptr  = nullptr;
        m_current_action_ptr = nullptr;
        m_get_rand_mt.seed(0);
    }

    /*************************************************************************/
    inline State<T_StateBody, T_ActionBody> &add_state(
        const T_StateBody &a_STATE_BODY) {
        m_states.emplace_back(a_STATE_BODY);
        m_states.back().learner_ptr = this;
        return m_states.back();
    }

    /*************************************************************************/
    inline void add_states(
        const std::vector<State<T_StateBody, T_ActionBody>> &a_STATES) {
        for (const auto &state : a_STATES) {
            m_states.push_back(state);
            m_states.back().learner_ptr = this;
        }
    }

    /*************************************************************************/
    inline std::vector<State<T_StateBody, T_ActionBody>> &states(void) {
        return m_states;
    }

    /*************************************************************************/
    inline const std::vector<State<T_StateBody, T_ActionBody>> &states(
        void) const {
        return m_states;
    }

    /*************************************************************************/
    inline const State<T_StateBody, T_ActionBody> &current_state(void) const {
        return *m_current_state_ptr;
    }

    /*************************************************************************/
    inline const Action<T_StateBody, T_ActionBody> &current_action(void) const {
        return *m_current_action_ptr;
    }

    /**************************************************************************/
    inline void setup(void) {
        m_current_state_ptr  = &(m_states.front());
        m_current_action_ptr = &(m_current_state_ptr->actions.front());
        for (auto &&state : m_states) {
            state.update_best_action();
        }
    }

    /**************************************************************************/
    inline Action<T_StateBody, T_ActionBody> &epsilon_greedy_selected_action(
        const double a_RATIO) {
        m_current_action_ptr =
            m_current_state_ptr->epsilon_greedy_selected_action_ptr(
                a_RATIO, &m_get_rand_mt);
        return *m_current_action_ptr;
    }
};

}  // namespace printemps::utility::q_learning

#endif
/*****************************************************************************/
// END
/*****************************************************************************/