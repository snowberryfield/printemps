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
inline constexpr int lit_var(const int a_LIT) noexcept {
    return a_LIT >> 1;
}

inline constexpr int lit_sign(const int a_LIT) noexcept {
    return a_LIT & 1;
}

inline constexpr int lit_neg(const int a_LIT) noexcept {
    return a_LIT ^ 1;
}

inline constexpr int make_lit(const int a_VAR, const int a_SIGN) noexcept {
    return (a_VAR << 1) | a_SIGN;
}

/*****************************************************************************/
/**
 * Watcher: represents a clause watching a literal.
 * blocker_lit is another literal in the clause that is frequently already
 * satisfied. If blocker_lit is satisfied, we can skip fetching the full clause
 * from memory.
 */
struct CDCLWatcher {
    int constraint_index;
    int blocker_lit;
};

/*****************************************************************************/
/**
 * Learned Clause structure for Two-Watched Literals (2WL).
 * literals[0] and literals[1] are the two watched literals.
 */
struct CDCLLearnedClause {
    int              index;
    int              literal_block_distance;
    bool             is_deleted;
    std::vector<int> literals;
    std::vector<int> positive_variable_indices;
    std::vector<int> negative_variable_indices;

    CDCLLearnedClause(void)
        : index(-1),
          literal_block_distance(0),
          is_deleted(false),
          literals(),
          positive_variable_indices(),
          negative_variable_indices() {
    }

    CDCLLearnedClause(const int a_INDEX, const int a_LBD,
                      std::vector<int>&& a_LITERALS, std::vector<int>&& a_POS,
                      std::vector<int>&& a_NEG)
        : index(a_INDEX),
          literal_block_distance(a_LBD),
          is_deleted(false),
          literals(std::move(a_LITERALS)),
          positive_variable_indices(std::move(a_POS)),
          negative_variable_indices(std::move(a_NEG)) {
    }

    inline int size(void) const noexcept {
        return static_cast<int>(literals.size());
    }

    inline bool empty(void) const noexcept {
        return literals.empty();
    }
};

}  // namespace printemps::solver::cdcl::core

#endif
/*****************************************************************************/
// END
/*****************************************************************************/
