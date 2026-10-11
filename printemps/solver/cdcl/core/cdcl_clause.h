/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_CDCL_CORE_CDCL_CLAUSE_H__
#define PRINTEMPS_SOLVER_CDCL_CORE_CDCL_CLAUSE_H__

namespace printemps::solver::cdcl::core {

/*****************************************************************************/
/**
 * Literal manipulation helpers:
 * A literal L encodes a variable index and polarity:
 * - L = 2 * v + 0: positive literal (x_v), satisfied when x_v == 1, falsified
 * when x_v == 0
 * - L = 2 * v + 1: negative literal (~x_v), satisfied when x_v == 0, falsified
 * when x_v == 1
 */
inline constexpr int literal_variable(const int a_LITERAL) noexcept {
    return a_LITERAL >> 1;
}

inline constexpr int literal_sign(const int a_LITERAL) noexcept {
    return a_LITERAL & 1;
}

inline constexpr int literal_negate(const int a_LITERAL) noexcept {
    return a_LITERAL ^ 1;
}

inline constexpr int make_literal(const int a_VARIABLE,
                                  const int a_SIGN) noexcept {
    return (a_VARIABLE << 1) | a_SIGN;
}

/*****************************************************************************/
/**
 * Watcher: represents a clause watching a literal.
 * blocker_literal is another literal in the clause that is frequently already
 * satisfied. If blocker_literal is satisfied, we can skip fetching the full clause
 * from memory.
 */
struct CDCLWatcher {
    int constraint_index;
    int blocker_literal;
};

/*****************************************************************************/
/**
 * Learned Clause structure for Two-Watched Literals (2WL).
 * literals[0] and literals[1] are the two watched literals.
 */
struct CDCLClause {
    int              index;
    int              literal_block_distance;
    bool             is_deleted;
    bool             is_learned;
    std::vector<int> literals;
    std::vector<int> positive_variable_indices;
    std::vector<int> negative_variable_indices;

    /*************************************************************************/
    CDCLClause(void)
        : index(-1),
          literal_block_distance(0),
          is_deleted(false),
          is_learned(false),
          literals(),
          positive_variable_indices(),
          negative_variable_indices() {
    }

    /*************************************************************************/
    CDCLClause(const int a_INDEX, const int a_LITERAL_BLOCK_DISTANCE,
               const bool a_IS_LEARNED, std::vector<int>&& a_LITERALS)
        : index(a_INDEX),
          literal_block_distance(a_LITERAL_BLOCK_DISTANCE),
          is_deleted(false),
          is_learned(a_IS_LEARNED),
          literals(std::move(a_LITERALS)),
          positive_variable_indices(),
          negative_variable_indices() {
    }

    /*************************************************************************/
    CDCLClause(const int a_INDEX, const int a_LITERAL_BLOCK_DISTANCE,
               const bool a_IS_LEARNED, std::vector<int>&& a_LITERALS,
               std::vector<int>&& a_POSITIVE_VARIABLE_INDICES,
               std::vector<int>&& a_NEGATIVE_VARIABLE_INDICES)
        : index(a_INDEX),
          literal_block_distance(a_LITERAL_BLOCK_DISTANCE),
          is_deleted(false),
          is_learned(a_IS_LEARNED),
          literals(std::move(a_LITERALS)),
          positive_variable_indices(
              std::move(a_POSITIVE_VARIABLE_INDICES)),
          negative_variable_indices(
              std::move(a_NEGATIVE_VARIABLE_INDICES)) {
    }

    /*************************************************************************/
    inline int size(void) const noexcept {
        return static_cast<int>(this->literals.size());
    }

    /*************************************************************************/
    inline bool empty(void) const noexcept {
        return this->literals.empty();
    }
};

using CDCLLearnedClause = CDCLClause;

}  // namespace printemps::solver::cdcl::core

#endif
/*****************************************************************************/
// END
/*****************************************************************************/
