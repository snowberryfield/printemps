/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_UTILITY_Q_LEARNING_ACTION_H__
#define PRINTEMPS_UTILITY_Q_LEARNING_ACTION_H__

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
struct Action {
    State<T_StateBody, T_ActionBody> *state_ptr;
    T_ActionBody                      body;
    double                            q_value;

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
        state_ptr = nullptr;
        q_value   = 0.0;
    }
};

}  // namespace printemps::utility::q_learning
#endif
/*****************************************************************************/
// END
/*****************************************************************************/