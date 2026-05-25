/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_WCNF_WCNF_CLAUSE_H__
#define PRINTEMPS_WCNF_WCNF_CLAUSE_H__

namespace printemps::wcnf {
/*****************************************************************************/
struct WCNFClause {
    /**
     * Literals are stored as signed 1-based variable indices: a positive value
     * v means the positive literal of variable v; a negative value -v means the
     * negation. The terminating 0 of the input file is NOT stored here.
     */
    std::vector<int> literals;

    /**
     * For hard clauses, weight is unused (is_hard == true). For soft clauses,
     * weight is the cost paid when the clause is falsified (>= 1).
     */
    uint64_t    weight;
    bool        is_hard;
    std::string name;

    /*************************************************************************/
    WCNFClause(void) {
        this->initialize();
    }

    /*************************************************************************/
    inline void initialize(void) {
        this->literals.clear();
        this->weight  = 0;
        this->is_hard = false;
        this->name.clear();
    }

    /*************************************************************************/
    inline int number_of_negative_literals(void) const {
        int count = 0;
        for (const auto literal : this->literals) {
            if (literal < 0) {
                count++;
            }
        }
        return count;
    }
};
}  // namespace printemps::wcnf
#endif
/*****************************************************************************/
// END
/*****************************************************************************/
