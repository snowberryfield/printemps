/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_TABU_SEARCH_CORE_TABU_SEARCH_MOVE_EVALUATOR_H__
#define PRINTEMPS_SOLVER_TABU_SEARCH_CORE_TABU_SEARCH_MOVE_EVALUATOR_H__

namespace printemps {
namespace solver {
namespace tabu_search {
namespace core {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class TabuSearchMoveEvaluator {
   private:
    model::Model<T_Variable, T_Expression> *m_model_ptr;
    Memory<T_Variable, T_Expression> *      m_memory_ptr;
    option::Option                          m_option;

   public:
    /*************************************************************************/
    TabuSearchMoveEvaluator(void) {
        this->initialize();
    }

    /*************************************************************************/
    TabuSearchMoveEvaluator(model::Model<T_Variable, T_Expression> *a_model_ptr,
                            Memory<T_Variable, T_Expression> *a_memory_ptr,
                            const option::Option &            a_option) {
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
    inline constexpr bool compute_permissibility(
        const neighborhood::Move<T_Variable, T_Expression> &a_MOVE,       //
        const int                                           a_ITERATION,  //
        const int a_TABU_TENURE) const noexcept {
        const auto &LAST_UPDATE_ITERATIONS =
            m_memory_ptr->last_update_iterations();

        if (m_option.tabu_search.tabu_mode == option::tabu_mode::All &&
            a_MOVE.sense != neighborhood::MoveSense::Selection) {
            /**
             * "All" tabu mode
             * The move is regarded as "tabu" if all of the variable to
             * be altered are included in the tabu list.
             */
            for (const auto &alteration : a_MOVE.alterations) {
                const int &LAST_UPDATE_ITERATION =
                    LAST_UPDATE_ITERATIONS[alteration.first->proxy_index()]
                                          [alteration.first->flat_index()];
                if (a_ITERATION - LAST_UPDATE_ITERATION >= a_TABU_TENURE) {
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
                const int &LAST_UPDATE_ITERATION =
                    LAST_UPDATE_ITERATIONS[alteration.first->proxy_index()]
                                          [alteration.first->flat_index()];
                if (a_ITERATION - LAST_UPDATE_ITERATION < a_TABU_TENURE) {
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

    /*****************************************************************************/
    inline constexpr double compute_frequency_penalty(
        const neighborhood::Move<T_Variable, T_Expression> &a_MOVE,
        const int a_ITERATION) const noexcept {
        const auto &UPDATE_COUNTS = m_memory_ptr->update_counts();

        if (a_ITERATION == 0) {
            return 0.0;
        }

        int move_update_count = 0;
        for (const auto &alteration : a_MOVE.alterations) {
            move_update_count += UPDATE_COUNTS[alteration.first->proxy_index()]
                                              [alteration.first->flat_index()];
        }
        return move_update_count *
               m_memory_ptr->total_update_count_reciprocal() *
               m_option.tabu_search.frequency_penalty_coefficient;
    }

    /*****************************************************************************/
    inline constexpr double compute_lagrangian_penalty(
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

    /*****************************************************************************/
    inline constexpr void evaluate(
        TabuSearchMoveScore *                               a_score_ptr,  //
        const neighborhood::Move<T_Variable, T_Expression> &a_MOVE,       //
        const int                                           a_ITERATION,  //
        const int a_TABU_TENURE) const noexcept {
        /**
         * Check if the move is permissible or not.
         */
        a_score_ptr->is_permissible  //
            = compute_permissibility(a_MOVE, a_ITERATION, a_TABU_TENURE);

        /**
         * Compute the frequency penalty of the move.
         */
        a_score_ptr->frequency_penalty  //
            = compute_frequency_penalty(a_MOVE, a_ITERATION);

        /**
         * Compute the lagrangian penalty of the move.
         */
        a_score_ptr->lagrangian_penalty = 0.0;
        if (m_option.is_enabled_lagrange_dual) {
            a_score_ptr->lagrangian_penalty  //
                = compute_lagrangian_penalty(a_MOVE);
        }
    }
};
}  // namespace core
}  // namespace tabu_search
}  // namespace solver
}  // namespace printemps

#endif
/*****************************************************************************/
// END
/*****************************************************************************/