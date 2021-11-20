/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_UTILITY_Q_LEARNING_LEARNER_H__
#define PRINTEMPS_UTILITY_Q_LEARNING_LEARNER_H__

namespace printemps {
namespace utility {
namespace q_learning {
/*****************************************************************************/
template <class T_StateBody, class T_ActionBody>
struct Learner;

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
    ~Learner(void) {
        /// nothing to do
    }

    /*************************************************************************/
    void set_seed(const std::uint_fast32_t &a_SEED) {
        m_get_rand_mt.seed(a_SEED);
    }

    /*************************************************************************/
    void initialize(void) {
        m_states.clear();
        m_current_state_ptr = nullptr;
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
    inline constexpr std::vector<State<T_StateBody, T_ActionBody>> &states(
        void) {
        return m_states;
    }

    /*************************************************************************/
    inline constexpr const std::vector<State<T_StateBody, T_ActionBody>>
        &states(void) const {
        return m_states;
    }

    /*************************************************************************/
    inline constexpr const State<T_StateBody, T_ActionBody> &current_state(
        void) const {
        return *m_current_state_ptr;
    }

    /*************************************************************************/
    inline constexpr const Action<T_StateBody, T_ActionBody> &current_action(
        void) const {
        return *m_current_action_ptr;
    }

    /**************************************************************************/
    inline constexpr void setup(void) {
        m_current_state_ptr = &(m_states.front());
        for (auto &&state : m_states) {
            state.update_best_action();
        }
    }

    /**************************************************************************/
    inline constexpr Action<T_StateBody, T_ActionBody>
        &epsilon_greedy_selected_action(const double a_RATIO) {
        m_current_action_ptr =
            m_current_state_ptr->epsilon_greedy_selected_action_ptr(
                a_RATIO, &m_get_rand_mt);
        return *m_current_action_ptr;
    }
};

}  // namespace q_learning
}  // namespace utility
}  // namespace printemps

#endif
/*****************************************************************************/
// END
/*****************************************************************************/