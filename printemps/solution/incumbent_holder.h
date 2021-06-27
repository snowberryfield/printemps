/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLUTION_INCUMBENT_HOLDER_H__
#define PRINTEMPS_SOLUTION_INCUMBENT_HOLDER_H__

namespace printemps {
namespace solution {
/*****************************************************************************/
struct IncumbentHolderConstant {
    static constexpr bool   DEFAULT_IS_FOUND_FEASIBLE_SOLUTION      = false;
    static constexpr double DEFAULT_OBJECTIVE                       = HUGE_VALF;
    static constexpr int    STATUS_NO_UPDATED                       = 0;
    static constexpr int    STATUS_LOCAL_AUGMENTED_INCUMBENT_UPDATE = 1;
    static constexpr int    STATUS_GLOBAL_AUGMENTED_INCUMBENT_UPDATE = 2;
    static constexpr int    STATUS_FEASIBLE_INCUMBENT_UPDATE         = 4;
};

/*****************************************************************************/
template <class T_Variable, class T_Expression>
class IncumbentHolder {
   private:
    bool m_is_found_feasible_solution;

    DenseSolution<T_Variable, T_Expression>
        m_local_augmented_incumbent_solution;
    DenseSolution<T_Variable, T_Expression>
                                            m_global_augmented_incumbent_solution;
    DenseSolution<T_Variable, T_Expression> m_feasible_incumbent_solution;

    /**
     * following double-type members contain incumbent objective values as
     * minimization problems
     */
    double m_local_augmented_incumbent_objective;
    double m_global_augmented_incumbent_objective;
    double m_feasible_incumbent_objective;

    SolutionScore m_local_augmented_incumbent_score;
    SolutionScore m_global_augmented_incumbent_score;
    SolutionScore m_feasible_incumbent_score;

   public:
    /*************************************************************************/
    IncumbentHolder(void) {
        this->initialize();
    }

    /*************************************************************************/
    virtual ~IncumbentHolder(void) {
        /// nothing to do
    }

    /*************************************************************************/
    void initialize(void) {
        m_is_found_feasible_solution =
            IncumbentHolderConstant::DEFAULT_IS_FOUND_FEASIBLE_SOLUTION;

        m_local_augmented_incumbent_objective =
            IncumbentHolderConstant::DEFAULT_OBJECTIVE;

        m_global_augmented_incumbent_objective =
            IncumbentHolderConstant::DEFAULT_OBJECTIVE;

        m_feasible_incumbent_objective =
            IncumbentHolderConstant::DEFAULT_OBJECTIVE;
    }

    /*************************************************************************/
    constexpr int try_update_incumbent(
        const DenseSolution<T_Variable, T_Expression> &a_SOLUTION,
        const SolutionScore &                          a_SCORE) {
        int status = IncumbentHolderConstant::STATUS_NO_UPDATED;

        /**
         * Following comparations must be based on SolutionScore objects
         * of which scores are computed as minimization problems.
         */
        if (a_SCORE.local_augmented_objective + constant::EPSILON <
            m_local_augmented_incumbent_objective) {
            status += IncumbentHolderConstant::
                STATUS_LOCAL_AUGMENTED_INCUMBENT_UPDATE;

            m_local_augmented_incumbent_solution = a_SOLUTION;
            m_local_augmented_incumbent_score    = a_SCORE;
            m_local_augmented_incumbent_objective =
                a_SCORE.local_augmented_objective;
        }

        if (a_SCORE.global_augmented_objective + constant::EPSILON <
            m_global_augmented_incumbent_objective) {
            status += IncumbentHolderConstant::
                STATUS_GLOBAL_AUGMENTED_INCUMBENT_UPDATE;

            m_global_augmented_incumbent_solution = a_SOLUTION;
            m_global_augmented_incumbent_score    = a_SCORE;
            m_global_augmented_incumbent_objective =
                a_SCORE.global_augmented_objective;
        }

        if (a_SCORE.is_feasible) {
            m_is_found_feasible_solution = true;

            if (a_SCORE.objective + constant::EPSILON <
                m_feasible_incumbent_objective) {
                status +=
                    IncumbentHolderConstant::STATUS_FEASIBLE_INCUMBENT_UPDATE;
                m_feasible_incumbent_score     = a_SCORE;
                m_feasible_incumbent_solution  = a_SOLUTION;
                m_feasible_incumbent_objective = a_SCORE.objective;
            }
        }
        return status;
    }

    /*************************************************************************/
    constexpr int try_update_incumbent(
        model::Model<T_Variable, T_Expression> *a_model_ptr,
        const SolutionScore &                   a_SCORE) {
        /// solution here defined is not substituted when no improvement
        DenseSolution<T_Variable, T_Expression> solution;

        bool is_solution_updated = false;

        int status = IncumbentHolderConstant::STATUS_NO_UPDATED;
        if (a_SCORE.local_augmented_objective + constant::EPSILON <
            m_local_augmented_incumbent_objective) {
            status += IncumbentHolderConstant::
                STATUS_LOCAL_AUGMENTED_INCUMBENT_UPDATE;

            if (!is_solution_updated) {
                solution            = a_model_ptr->export_solution();
                is_solution_updated = true;
            }

            m_local_augmented_incumbent_solution = solution;
            m_local_augmented_incumbent_score    = a_SCORE;
            m_local_augmented_incumbent_objective =
                a_SCORE.local_augmented_objective;
        }

        if (a_SCORE.global_augmented_objective + constant::EPSILON <
            m_global_augmented_incumbent_objective) {
            status += IncumbentHolderConstant::
                STATUS_GLOBAL_AUGMENTED_INCUMBENT_UPDATE;

            if (!is_solution_updated) {
                solution            = a_model_ptr->export_solution();
                is_solution_updated = true;
            }

            m_global_augmented_incumbent_solution = solution;
            m_global_augmented_incumbent_score    = a_SCORE;
            m_global_augmented_incumbent_objective =
                a_SCORE.global_augmented_objective;
        }

        if (a_SCORE.is_feasible) {
            m_is_found_feasible_solution = true;

            if (a_SCORE.objective + constant::EPSILON <
                m_feasible_incumbent_objective) {
                status +=
                    IncumbentHolderConstant::STATUS_FEASIBLE_INCUMBENT_UPDATE;

                if (!is_solution_updated) {
                    solution            = a_model_ptr->export_solution();
                    is_solution_updated = true;
                }

                m_feasible_incumbent_solution  = solution;
                m_feasible_incumbent_score     = a_SCORE;
                m_feasible_incumbent_objective = a_SCORE.objective;
            }
        }
        return status;
    }

    /*************************************************************************/
    inline constexpr void reset_local_augmented_incumbent(void) {
        m_local_augmented_incumbent_objective =
            IncumbentHolderConstant::DEFAULT_OBJECTIVE;
    }

    /*************************************************************************/
    inline constexpr bool is_found_feasible_solution(void) const {
        return m_is_found_feasible_solution;
    }

    /*************************************************************************/
    inline constexpr const DenseSolution<T_Variable, T_Expression>
        &local_augmented_incumbent_solution(void) const {
        return m_local_augmented_incumbent_solution;
    }

    /*************************************************************************/
    inline constexpr const DenseSolution<T_Variable, T_Expression>
        &global_augmented_incumbent_solution(void) const {
        return m_global_augmented_incumbent_solution;
    }

    /*************************************************************************/
    inline constexpr const DenseSolution<T_Variable, T_Expression>
        &feasible_incumbent_solution(void) const {
        return m_feasible_incumbent_solution;
    }

    /*************************************************************************/
    inline constexpr double local_augmented_incumbent_objective(void) const {
        /**
         * This method returns the local augmented incumbent objective as
         * minimization problem; it will be nagated for maximization
         * problem.
         */
        return m_local_augmented_incumbent_objective;
    }

    /*************************************************************************/
    inline constexpr double global_augmented_incumbent_objective(void) const {
        /**
         * This method returns the global augmented incumbent objective as
         * minimization problem; it will be nagated for maximization
         * problem.
         */
        return m_global_augmented_incumbent_objective;
    }

    /*************************************************************************/
    inline constexpr double feasible_incumbent_objective(void) const {
        /**
         * This method returns the local augmented incumbent objective as
         * minimization problem; it will be nagated for maximization
         * problem.
         */
        return m_feasible_incumbent_objective;
    }

    /*************************************************************************/
    inline const SolutionScore &local_augmented_incumbent_score(void) const {
        /// cannot be constexpr by clang
        return m_local_augmented_incumbent_score;
    }

    /*************************************************************************/
    inline const SolutionScore &global_augmented_incumbent_score(void) const {
        /// cannot be constexpr by clang
        return m_global_augmented_incumbent_score;
    }

    /*************************************************************************/
    inline const SolutionScore &feasible_incumbent_score(void) const {
        /// cannot be constexpr by clang
        return m_feasible_incumbent_score;
    }
};
}  // namespace solution
}  // namespace printemps

#endif
/*****************************************************************************/
// END
/*****************************************************************************/