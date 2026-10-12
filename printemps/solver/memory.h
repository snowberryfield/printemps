/*****************************************************************************/
// Copyright (c) 2020-2026 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_MEMORY_H__
#define PRINTEMPS_SOLVER_MEMORY_H__

#include "../utility/intensity.h"

namespace printemps::solver {
/*****************************************************************************/
struct MemoryConstant {
    static constexpr int INITIAL_LOCAL_LAST_UPDATE_ITERATION = -1000;
};

/*****************************************************************************/
template <class T_Variable, class T_Expression>
class Memory {
   private:
    model::Model<T_Variable, T_Expression> *m_model_ptr;

    utility::Intensity<int> m_primal_intensity;
    utility::Intensity<int> m_dual_intensity;

   public:
    /*************************************************************************/
    Memory(void) {
        this->initialize();
    }

    /*************************************************************************/
    Memory(model::Model<T_Variable, T_Expression> *a_model_ptr) {
        this->setup(a_model_ptr);
    }

    /*************************************************************************/
    void initialize(void) {
        m_model_ptr = nullptr;
        m_primal_intensity.reset();
        m_dual_intensity.reset();
    }

    /*************************************************************************/
    inline void setup(model::Model<T_Variable, T_Expression> *a_model_ptr) {
        this->initialize();
        m_model_ptr = a_model_ptr;

        auto &variable_ptrs = m_model_ptr->reference().variable.variable_ptrs;
        for (auto &&variable_ptr : variable_ptrs) {
            variable_ptr->reset_local_last_update_iteration();
            variable_ptr->reset_update_count();
        }

        auto &constraint_ptrs =
            m_model_ptr->reference().constraint.constraint_ptrs;
        for (auto &&constraint_ptr : constraint_ptrs) {
            constraint_ptr->reset_violation_count();
        }
    }

    /*************************************************************************/
    inline double primal_intensity(void) const noexcept {
        return m_primal_intensity.value();
    }

    /*************************************************************************/
    inline double dual_intensity(void) const noexcept {
        return m_dual_intensity.value();
    }

    /*************************************************************************/
    inline void update(
        const neighborhood::Move<T_Variable, T_Expression> &a_MOVE,
        const int a_ITERATION) noexcept {
        this->update(a_MOVE, a_ITERATION, 0, nullptr);
    }

    /*************************************************************************/
    inline void update(
        const neighborhood::Move<T_Variable, T_Expression> &a_MOVE,
        const int                                           a_ITERATION,     //
        const int                                           a_RANDOM_WIDTH,  //
        std::mt19937 *get_rand_mt) noexcept {
        for (const auto &alteration : a_MOVE.alterations) {
            if (alteration.first->value() == alteration.second) {
                continue;
            }

            int randomness = 0;
            if (a_RANDOM_WIDTH > 0) {
                randomness =
                    (*get_rand_mt)() % (2 * a_RANDOM_WIDTH) - a_RANDOM_WIDTH;
            }

            alteration.first->set_local_last_update_iteration(a_ITERATION +
                                                              randomness);
            alteration.first->set_global_last_update_iteration(
                m_primal_intensity.total_count());

            m_primal_intensity.update_with_previous_count(
                alteration.first->update_count());
            alteration.first->increment_update_count();
        }

        for (const auto &constraint_ptr :
             m_model_ptr->current_violative_constraint_ptrs()) {
            m_dual_intensity.update_with_previous_count(
                constraint_ptr->violation_count());
            constraint_ptr->update_violation_count();
        }
    }

    /*************************************************************************/
    inline void reset_local_last_update_iterations(void) {
        auto &variable_ptrs = m_model_ptr->reference().variable.variable_ptrs;
        for (auto &&variable_ptr : variable_ptrs) {
            variable_ptr->reset_local_last_update_iteration();
        }
    }

    /*************************************************************************/
    inline long long total_update_count(void) const noexcept {
        return m_primal_intensity.total_count();
    }

    /*************************************************************************/
    inline double total_update_count_reciprocal(void) const noexcept {
        return m_primal_intensity.total_count_reciprocal();
    }

    /*************************************************************************/
    inline long long total_violation_count(void) const noexcept {
        return m_dual_intensity.total_count();
    }

    /*************************************************************************/
    inline double total_violation_count_reciprocal(void) const noexcept {
        return m_dual_intensity.total_count_reciprocal();
    }
};
}  // namespace printemps::solver

#endif
/*****************************************************************************/
// END
/*****************************************************************************/