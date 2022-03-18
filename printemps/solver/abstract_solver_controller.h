/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_ABSTRACT_SOLVER_CONTROLLER_H__
#define PRINTEMPS_SOLVER_ABSTRACT_SOLVER_CONTROLLER_H__

#include "memory.h"
#include "status.h"
#include "result.h"

namespace printemps {
namespace solver {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class AbstractSolverController {
   protected:
    model::Model<T_Variable, T_Expression>*              m_model_ptr;
    solution::DenseSolution<T_Variable, T_Expression>    m_initial_solution;
    solution::IncumbentHolder<T_Variable, T_Expression>* m_incumbent_holder_ptr;
    Memory<T_Variable, T_Expression>*                    m_memory_ptr;
    solution::SolutionArchive<T_Variable, T_Expression>* m_solution_archive_ptr;
    utility::TimeKeeper                                  m_time_keeper;
    option::Option                                       m_master_option;

    /*************************************************************************/
    inline void print_total_elapsed_time(const double a_TOTAL_ELAPSED_TIME,
                                         const bool   a_IS_ENABLED_PRINT) {
        utility::print_info(  //
            " -- Total elapsed time: " +
                utility::to_string(a_TOTAL_ELAPSED_TIME, "%.3f") + "sec",
            a_IS_ENABLED_PRINT);
    }

    /*************************************************************************/
    inline void print_incumbent_summary(const bool a_IS_ENABLED_PRINT) {
        utility::print_info(
            " -- Global augmented incumbent objective: " +
                utility::to_string(
                    m_incumbent_holder_ptr
                            ->global_augmented_incumbent_objective() *
                        m_model_ptr->sign(),
                    "%.3f"),
            a_IS_ENABLED_PRINT);

        utility::print_info(
            " -- Feasible incumbent objective: " +
                utility::to_string(
                    m_incumbent_holder_ptr->feasible_incumbent_objective() *
                        m_model_ptr->sign(),
                    "%.3f"),
            a_IS_ENABLED_PRINT);
    }

    /*************************************************************************/
    inline constexpr void bound_variables(const double a_OBJECTIVE,
                                          const bool   a_IS_ENABLED_PRINT) {
        auto number_of_newly_fixed_variables =
            m_model_ptr->update_variable_bounds(a_OBJECTIVE,
                                                a_IS_ENABLED_PRINT);

        /**
         * If there is new fixed variable, setup the variable category
         * and the binary/integer neighborhood again.
         */
        if (number_of_newly_fixed_variables > 0) {
            m_model_ptr->categorize_variables();
            m_model_ptr->neighborhood().binary().setup(
                m_model_ptr->variable_reference().binary_variable_ptrs);
            m_model_ptr->neighborhood().integer().setup(
                m_model_ptr->variable_reference().integer_variable_ptrs);
        }
    }

   public:
    /*************************************************************************/
    AbstractSolverController(void) {
        this->initialize();
    }

    /*************************************************************************/
    AbstractSolverController(                                 //
        model::Model<T_Variable, T_Expression>* a_model_ptr,  //
        const solution::DenseSolution<T_Variable, T_Expression>&
            a_CURRENT_SOLUTION,  //
        solution::IncumbentHolder<T_Variable, T_Expression>*
                                          a_incumbent_holder_ptr,
        Memory<T_Variable, T_Expression>* a_memory_ptr,  //
        solution::SolutionArchive<T_Variable, T_Expression>*
                                   a_solution_archive_ptr,  //
        const utility::TimeKeeper& a_TIME_KEEPER,           //
        const option::Option&      a_OPTION) {
        this->initialize();
        this->setup(a_model_ptr,             //
                    a_CURRENT_SOLUTION,      //
                    a_incumbent_holder_ptr,  //
                    a_memory_ptr,            //
                    a_solution_archive_ptr,  //
                    a_TIME_KEEPER,           //
                    a_OPTION);
    }

    /*************************************************************************/
    virtual ~AbstractSolverController(void) {
        /// nothing to do
    }

    /*************************************************************************/
    inline void initialize(void) {
        m_model_ptr = nullptr;
        m_initial_solution.initialize();
        m_incumbent_holder_ptr = nullptr;
        m_memory_ptr           = nullptr;
        m_solution_archive_ptr = nullptr;
        m_time_keeper.initialize();
        m_master_option.initialize();
    }

    /*************************************************************************/
    inline void setup(                                        //
        model::Model<T_Variable, T_Expression>* a_model_ptr,  //
        const solution::DenseSolution<T_Variable, T_Expression>&
            a_INITIAL_SOLUTION,  //
        solution::IncumbentHolder<T_Variable, T_Expression>*
                                          a_incumbent_holder_ptr,
        Memory<T_Variable, T_Expression>* a_memory_ptr,  //
        solution::SolutionArchive<T_Variable, T_Expression>*
                                   a_solution_archive_ptr,  //
        const utility::TimeKeeper& a_TIME_KEEPER,           //
        const option::Option&      a_OPTION) {
        m_model_ptr            = a_model_ptr;
        m_initial_solution     = a_INITIAL_SOLUTION;
        m_incumbent_holder_ptr = a_incumbent_holder_ptr;
        m_memory_ptr           = a_memory_ptr;
        m_solution_archive_ptr = a_solution_archive_ptr;
        m_time_keeper          = a_TIME_KEEPER;
        m_master_option        = a_OPTION;
    }

    /*************************************************************************/
    inline void update_archive(
        const std::vector<solution::SparseSolution<T_Variable, T_Expression>>&
            a_FEASIBLE_SOLUTIONS) {
        if (a_FEASIBLE_SOLUTIONS.size() > 0) {
            m_solution_archive_ptr->push(a_FEASIBLE_SOLUTIONS);
        }
    }

    /*************************************************************************/
    inline constexpr model::Model<T_Variable, T_Expression>* model_ptr(void) {
        return m_model_ptr;
    }

    /*************************************************************************/
    inline constexpr solution::IncumbentHolder<T_Variable, T_Expression>*
    incumbent_holder_ptr(void) {
        return m_incumbent_holder_ptr;
    }

    /*************************************************************************/
    inline constexpr Memory<T_Variable, T_Expression>* memory_ptr(void) {
        return m_memory_ptr;
    }

    /*************************************************************************/
    inline constexpr solution::SolutionArchive<T_Variable, T_Expression>*
    solution_archive_ptr(void) {
        return m_solution_archive_ptr;
    }

    /*************************************************************************/
    inline utility::TimeKeeper time_keeper(void) const {
        return m_time_keeper;
    }
};  // namespace solver
}  // namespace solver
}  // namespace printemps

#endif
/*****************************************************************************/
// END
/*****************************************************************************/