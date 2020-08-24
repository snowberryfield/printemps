/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef CPPMH_UTILITY_PLAIN_SOLUTION_POOL_H__
#define CPPMH_UTILITY_PLAIN_SOLUTION_POOL_H__

#include <algorithm>
#include <vector>

namespace cppmh {
namespace model {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class PlainSolution;

/*****************************************************************************/
template <class T_Variable, class T_Expression>
class PlainSolutionPool {
   private:
    int                                                  m_max_size;
    bool                                                 m_is_ascending;
    std::vector<PlainSolution<T_Variable, T_Expression>> m_solutions;

   public:
    /*************************************************************************/
    PlainSolutionPool(void) {
        this->initialize();
    }

    /*************************************************************************/
    virtual ~PlainSolutionPool(void) {
        /// nothing to do
    }

    /*************************************************************************/
    PlainSolutionPool(const int a_MAX_SIZE, const bool a_is_ASCENDING) {
        this->setup(a_MAX_SIZE, a_is_ASCENDING);
    }

    /*************************************************************************/
    inline constexpr void initialize(void) {
        m_max_size     = 0;
        m_is_ascending = true;
        m_solutions.clear();
    }

    /*************************************************************************/
    inline constexpr void setup(const int  a_MAX_SIZE,
                                const bool a_is_ASCENDING) {
        m_max_size     = a_MAX_SIZE;
        m_is_ascending = a_is_ASCENDING;
        m_solutions.clear();
    }

    /*************************************************************************/
    inline constexpr void push(
        const PlainSolution<T_Variable, T_Expression>& a_SOLUTION) {
        std::vector<PlainSolution<T_Variable, T_Expression>> solutions = {
            a_SOLUTION};
        this->push(solutions);
    }

    /*************************************************************************/
    inline constexpr void push(
        const std::vector<PlainSolution<T_Variable, T_Expression>>&
            a_SOLUTIONS) {
        auto& solutions = m_solutions;
        solutions.insert(solutions.end(), a_SOLUTIONS.begin(),
                         a_SOLUTIONS.end());

        if (m_is_ascending) {
            std::sort(solutions.begin(), solutions.end(),
                      [](auto const& a_LHS, auto const& a_RHS) {
                          return a_LHS.objective < a_RHS.objective;
                      });
        } else {
            std::sort(solutions.begin(), solutions.end(),
                      [](auto const& a_LHS, auto const& a_RHS) {
                          return a_LHS.objective > a_RHS.objective;
                      });
        }

        solutions.erase(std::unique(solutions.begin(), solutions.end(),
                                    [](auto const& a_LHS, auto const& a_RHS) {
                                        return a_LHS.variables ==
                                               a_RHS.variables;
                                    }),
                        solutions.end());

        if (static_cast<int>(solutions.size()) > m_max_size) {
            solutions.resize(m_max_size);
        }
    }

    /*************************************************************************/
    inline constexpr int size(void) const {
        return m_solutions.size();
    }

    /*************************************************************************/
    inline constexpr int max_size(void) const {
        return m_max_size;
    }

    /*************************************************************************/
    inline constexpr bool is_ascending(void) const {
        return m_is_ascending;
    }

    /*************************************************************************/
    inline constexpr const std::vector<PlainSolution<T_Variable, T_Expression>>&
    solutions(void) const {
        return m_solutions;
    }
};
}  // namespace model
}  // namespace cppmh

#endif
/*****************************************************************************/
// END
/*****************************************************************************/