/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_WCNF_WCNF_METADATA_H__
#define PRINTEMPS_WCNF_WCNF_METADATA_H__

namespace printemps::wcnf {
/*****************************************************************************/
struct WCNFMetadata {
    int      number_of_variables;
    int      number_of_clauses;
    int      number_of_hard_clauses;
    int      number_of_soft_clauses;
    uint64_t weight_sum;
    uint64_t max_weight;

    /*************************************************************************/
    WCNFMetadata(void) {
        this->initialize();
    }

    /*************************************************************************/
    inline void initialize(void) {
        this->number_of_variables    = 0;
        this->number_of_clauses      = 0;
        this->number_of_hard_clauses = 0;
        this->number_of_soft_clauses = 0;
        this->weight_sum             = 0;
        this->max_weight             = 0;
    }
};
}  // namespace printemps::wcnf
#endif
/*****************************************************************************/
// END
/*****************************************************************************/
