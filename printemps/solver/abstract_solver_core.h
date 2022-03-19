/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_ABSTRACT_SOLVER_CORE_H__
#define PRINTEMPS_SOLVER_ABSTRACT_SOLVER_CORE_H__

#include "memory.h"
#include "status.h"
#include "result.h"

namespace printemps {
namespace solver {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class AbstractSolverCore {
   protected:
    model::Model<T_Variable, T_Expression>* m_model_ptr;

    std::vector<multi_array::ValueProxy<T_Variable>>
        m_initial_variable_value_proxies;

    solution::IncumbentHolder<T_Variable, T_Expression>* m_incumbent_holder_ptr;
    Memory<T_Variable, T_Expression>*                    m_memory_ptr;
    option::Option                                       m_option;

    std::vector<solution::SparseSolution<T_Variable, T_Expression>>
        m_feasible_solutions;

   public:
    /*************************************************************************/
    AbstractSolverCore(void) {
        this->initialize();
    }

    /*************************************************************************/
    AbstractSolverCore(
        model::Model<T_Variable, T_Expression>* a_model_ptr,       //
        const std::vector<multi_array::ValueProxy<T_Variable>>&    //
            a_INITIAL_VARIABLE_VALUE_PROXIES,                      //
        solution::IncumbentHolder<T_Variable, T_Expression>*       //
                                          a_incumbent_holder_ptr,  //
        Memory<T_Variable, T_Expression>* a_memory_ptr,            //
        const option::Option&             a_OPTION) {
        this->setup(a_model_ptr,                       //
                    a_INITIAL_VARIABLE_VALUE_PROXIES,  //
                    a_incumbent_holder_ptr,            //
                    a_memory_ptr,                      //
                    a_OPTION);
    }

    /*************************************************************************/
    virtual ~AbstractSolverCore(void) {
        /// nothing to do
    }

    /*************************************************************************/
    inline void initialize(void) {
        m_model_ptr = nullptr;
        m_initial_variable_value_proxies.clear();
        m_incumbent_holder_ptr = nullptr;
        m_memory_ptr           = nullptr;
        m_option.initialize();
        m_feasible_solutions.clear();
    }

    /*************************************************************************/
    inline void setup(                                             //
        model::Model<T_Variable, T_Expression>* a_model_ptr,       //
        const std::vector<multi_array::ValueProxy<T_Variable>>&    //
            a_INITIAL_VARIABLE_VALUE_PROXIES,                      //
        solution::IncumbentHolder<T_Variable, T_Expression>*       //
                                          a_incumbent_holder_ptr,  //
        Memory<T_Variable, T_Expression>* a_memory_ptr,            //
        const option::Option&             a_OPTION) {
        m_model_ptr                      = a_model_ptr;
        m_initial_variable_value_proxies = a_INITIAL_VARIABLE_VALUE_PROXIES;
        m_incumbent_holder_ptr           = a_incumbent_holder_ptr;
        m_memory_ptr                     = a_memory_ptr;
        m_option                         = a_OPTION;
        m_feasible_solutions.clear();
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
    inline constexpr const std::vector<
        solution::SparseSolution<T_Variable, T_Expression>>&
    feasible_solutions(void) const {
        return m_feasible_solutions;
    }

};  // namespace solver
}  // namespace solver
}  // namespace printemps

#endif
/*****************************************************************************/
// END
/*****************************************************************************/