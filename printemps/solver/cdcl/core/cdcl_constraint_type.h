/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_CDCL_CORE_CDCL_CONSTRAINT_TYPE_H__
#define PRINTEMPS_SOLVER_CDCL_CORE_CDCL_CONSTRAINT_TYPE_H__

namespace printemps::solver::cdcl::core {
/*****************************************************************************/
enum class CDCLConstraintType {
    NONE,
    PSEUDO_BOOLEAN,
    CARDINALITY,
    LEARNED,
};

/*****************************************************************************/
static const std::unordered_map<std::string, CDCLConstraintType>
    CDCLConstraintTypeMap = {
        {"NONE", CDCLConstraintType::NONE},
        {"PSEUDO_BOOLEAN", CDCLConstraintType::PSEUDO_BOOLEAN},
        {"CARDINALITY", CDCLConstraintType::CARDINALITY},
        {"LEARNED", CDCLConstraintType::LEARNED},
};

/*****************************************************************************/
static const std::unordered_map<CDCLConstraintType, std::string>
    CDCLConstraintTypeInverseMap = {
        {CDCLConstraintType::NONE, "NONE"},
        {CDCLConstraintType::PSEUDO_BOOLEAN, "PSEUDO_BOOLEAN"},
        {CDCLConstraintType::CARDINALITY, "CARDINALITY"},
        {CDCLConstraintType::LEARNED, "LEARNED"},
};

}  // namespace printemps::solver::cdcl::core

#endif
/*****************************************************************************/
// END
/*****************************************************************************/
