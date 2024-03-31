/*****************************************************************************/
// Copyright (c) 2020-2024 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_ABSTRACT_SOLVER_CONTROLLER_H__
#define PRINTEMPS_SOLVER_ABSTRACT_SOLVER_CONTROLLER_H__

#include "memory.h"

namespace printemps::solver {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class AbstractSolverController {
   protected:
    model::Model<T_Variable, T_Expression>*            m_model_ptr;
    GlobalState<T_Variable, T_Expression>*             m_global_state_ptr;
    solution::SparseSolution<T_Variable, T_Expression> m_initial_solution;
    utility::TimeKeeper                                m_time_keeper;
    option::Option                                     m_option;

    /*************************************************************************/
    inline void print_total_elapsed_time(const bool a_IS_ENABLED_PRINT) {
        utility::print_info(  //
            " -- Total elapsed time: " +
                utility::to_string(m_time_keeper.elapsed_time(), "%.3f") +
                "sec",
            a_IS_ENABLED_PRINT);
    }

    /*************************************************************************/
    inline void print_incumbent_summary(const bool a_IS_ENABLED_PRINT) {
        const auto& GLOBAL_INCUMBENT_SOLUTION =
            m_global_state_ptr->incumbent_holder
                .global_augmented_incumbent_solution();
        utility::print_info(
            " -- Incumbent objective: " +
                utility::to_string(GLOBAL_INCUMBENT_SOLUTION.objective, "%.5e"),
            a_IS_ENABLED_PRINT);

        utility::print_info(
            " -- Incumbent violation: " +
                utility::to_string(GLOBAL_INCUMBENT_SOLUTION.total_violation,
                                   "%.5e") +
                " (duplicate constraints included) ",
            a_IS_ENABLED_PRINT);
    }

    /*************************************************************************/
    inline void print_dual_bound(const bool a_IS_ENABLED_PRINT) {
        utility::print_info(
            " -- Dual Bound: " +
                utility::to_string(
                    m_global_state_ptr->incumbent_holder.dual_bound(), "%.5e"),
            a_IS_ENABLED_PRINT);
    }

    /*************************************************************************/
    inline void update_variable_bounds(const double a_OBJECTIVE,
                                       const bool   a_IS_PRIMAL,
                                       const bool   a_IS_ENABLED_PRINT) {
        auto number_of_newly_fixed_variables =
            m_model_ptr->update_variable_bounds(a_OBJECTIVE, a_IS_PRIMAL,
                                                a_IS_ENABLED_PRINT);

        /**
         * If there is new fixed variable, setup the variable category
         * and the binary/integer neighborhood again.
         */
        if (number_of_newly_fixed_variables > 0) {
            m_model_ptr->categorize_variables();
            m_model_ptr->neighborhood().binary().setup(
                m_model_ptr->variable_type_reference().binary_variable_ptrs);
            m_model_ptr->neighborhood().integer().setup(
                m_model_ptr->variable_type_reference().integer_variable_ptrs);
        }
    }

   public:
    /*************************************************************************/
    AbstractSolverController(void) {
        this->initialize();
    }

    /*************************************************************************/
    AbstractSolverController(                                        //
        model::Model<T_Variable, T_Expression>* a_model_ptr,         //
        GlobalState<T_Variable, T_Expression>*  a_global_state_ptr,  //
        const solution::SparseSolution<T_Variable, T_Expression>&
                                   a_INITIAL_SOLUTION,  //
        const utility::TimeKeeper& a_TIME_KEEPER,       //
        const option::Option&      a_OPTION) {
        this->setup(a_model_ptr,         //
                    a_global_state_ptr,  //
                    a_INITIAL_SOLUTION,  //
                    a_TIME_KEEPER,       //
                    a_OPTION);
    }

    /*************************************************************************/
    inline void initialize(void) {
        m_model_ptr        = nullptr;
        m_global_state_ptr = nullptr;
        m_initial_solution.initialize();
        m_time_keeper.initialize();
        m_option.initialize();
    }

    /*************************************************************************/
    inline void setup(                                               //
        model::Model<T_Variable, T_Expression>* a_model_ptr,         //
        GlobalState<T_Variable, T_Expression>*  a_global_state_ptr,  //
        const solution::SparseSolution<T_Variable, T_Expression>&
                                   a_INITIAL_SOLUTION,  //
        const utility::TimeKeeper& a_TIME_KEEPER,       //
        const option::Option&      a_OPTION) {
        m_model_ptr        = a_model_ptr;
        m_global_state_ptr = a_global_state_ptr;
        m_initial_solution = a_INITIAL_SOLUTION;
        m_time_keeper      = a_TIME_KEEPER;
        m_option           = a_OPTION;
    }

    /*************************************************************************/
    inline void update_feasible_solution_archive(
        const std::vector<solution::SparseSolution<T_Variable, T_Expression>>&
            a_SOLUTIONS) {
        if (a_SOLUTIONS.size() == 0) {
            return;
        }
        m_global_state_ptr->feasible_solution_archive.push(a_SOLUTIONS);
    }

    /*************************************************************************/
    inline void update_incumbent_solution_archive_and_search_tree(
        const std::vector<solution::SparseSolution<T_Variable, T_Expression>>&
            a_SOLUTIONS) {
        if (a_SOLUTIONS.size() == 0) {
            return;
        }
        auto& incumbent_holder = m_global_state_ptr->incumbent_holder;
        auto& incumbent_solution_archive =
            m_global_state_ptr->incumbent_solution_archive;
        auto& search_tree = m_global_state_ptr->search_tree;

        incumbent_solution_archive.push(a_SOLUTIONS);

        if (!incumbent_solution_archive.has_feasible_solution()) {
            if (incumbent_solution_archive.update_has_feasible_solution(
                    a_SOLUTIONS)) {
                incumbent_solution_archive.remove_infeasible_solutions();
                search_tree.initialize();
            }
        }

        search_tree.update(
            incumbent_solution_archive,
            incumbent_holder.global_augmented_incumbent_solution().to_sparse());
    }

    /*************************************************************************/
    inline model::Model<T_Variable, T_Expression>* model_ptr(void) {
        return m_model_ptr;
    }

    /*************************************************************************/
    inline GlobalState<T_Variable, T_Expression>* global_state_ptr(void) {
        return m_global_state_ptr;
    }
};
}  // namespace printemps::solver

#endif
/*****************************************************************************/
// END
/*****************************************************************************/