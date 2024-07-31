/*****************************************************************************/
// Copyright (c) 2020-2024 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_TABU_SEARCH_CORE_TABU_SEARCH_CORE_MOVE_EVALUATOR_H__
#define PRINTEMPS_SOLVER_TABU_SEARCH_CORE_TABU_SEARCH_CORE_MOVE_EVALUATOR_H__

namespace printemps::solver::tabu_search::core {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class TabuSearchCoreMoveEvaluator {
   private:
    model::Model<T_Variable, T_Expression> *m_model_ptr;
    Memory<T_Variable, T_Expression> *      m_memory_ptr;
    option::Option                          m_option;

   public:
    /*************************************************************************/
    TabuSearchCoreMoveEvaluator(void) {
        this->initialize();
    }

    /*************************************************************************/
    TabuSearchCoreMoveEvaluator(
        model::Model<T_Variable, T_Expression> *a_model_ptr,
        Memory<T_Variable, T_Expression> *      a_memory_ptr,
        const option::Option &                  a_option) {
        this->setup(a_model_ptr, a_memory_ptr, a_option);
    }

    /*************************************************************************/
    inline void setup(model::Model<T_Variable, T_Expression> *a_model_ptr,
                      Memory<T_Variable, T_Expression> *      a_memory_ptr,
                      const option::Option &                  a_option) {
        this->initialize();
        m_model_ptr  = a_model_ptr;
        m_memory_ptr = a_memory_ptr;
        m_option     = a_option;
    }

    /*************************************************************************/
    inline void initialize(void) {
        m_model_ptr  = nullptr;
        m_memory_ptr = nullptr;
        m_option.initialize();
    }

    /*************************************************************************/
    inline bool compute_permissibility(
        const neighborhood::Move<T_Variable, T_Expression> &a_MOVE,  //
        const int a_DURATION) const noexcept {
        if (m_option.tabu_search.tabu_mode == option::tabu_mode::All &&
            a_MOVE.sense != neighborhood::MoveSense::Selection) {
            /**
             * "All" tabu mode
             * The move is regarded as "tabu" if all of the variable to
             * be altered are included in the tabu list.
             */
            for (const auto &alteration : a_MOVE.alterations) {
                if (a_DURATION >=
                    alteration.first->local_last_update_iteration()) {
                    return true;
                }
            }
            return false;
        } else {
            /**
             * "Any" tabu mode
             * The move is regarded as "tabu" if it includes any alteration
             * about a variable in the tabu list.
             */
            for (const auto &alteration : a_MOVE.alterations) {
                if (a_DURATION <
                    alteration.first->local_last_update_iteration()) {
                    return false;
                }
            }
            return true;
        }
        /**
         * If the process is normal, this return statement is not reached.
         */
        return true;
    }

    /*************************************************************************/
    inline double compute_frequency_penalty(
        const neighborhood::Move<T_Variable, T_Expression> &a_MOVE,
        const int a_ITERATION) const noexcept {
        if (a_ITERATION == 0) {
            return 0.0;
        }

        int total_update_count = 0;
        for (const auto &alteration : a_MOVE.alterations) {
            total_update_count += alteration.first->update_count();
        }

        return total_update_count *
               m_memory_ptr->total_update_count_reciprocal() *
               m_option.tabu_search.frequency_penalty_coefficient;
    }

    /*************************************************************************/
    inline double compute_lagrangian_penalty(
        const neighborhood::Move<T_Variable, T_Expression> &a_MOVE) const
        noexcept {
        double lagrangian_penalty = 0.0;
        for (const auto &alteration : a_MOVE.alterations) {
            lagrangian_penalty +=
                alteration.first->lagrangian_coefficient() * alteration.second;
        }
        return lagrangian_penalty *
               m_option.tabu_search.lagrangian_penalty_coefficient;
    }

    /*************************************************************************/
    inline void evaluate(
        TabuSearchCoreMoveScore *                           a_score_ptr,  //
        const neighborhood::Move<T_Variable, T_Expression> &a_MOVE,       //
        const int                                           a_ITERATION,  //
        const int a_DURATION) const noexcept {
        /**
         * Check if the move is permissible or not.
         */
        a_score_ptr->is_permissible  //
            = this->compute_permissibility(a_MOVE, a_DURATION);

        /**
         * Compute the frequency penalty of the move.
         */
        a_score_ptr->frequency_penalty  //
            = this->compute_frequency_penalty(a_MOVE, a_ITERATION);

        /**
         * Compute the lagrangian penalty of the move.
         */
        a_score_ptr->lagrangian_penalty = 0.0;
        if (m_option.lagrange_dual.is_enabled) {
            a_score_ptr->lagrangian_penalty  //
                = this->compute_lagrangian_penalty(a_MOVE);
        }
    }
};
}  // namespace printemps::solver::tabu_search::core
#endif
/*****************************************************************************/
// END
/*****************************************************************************/