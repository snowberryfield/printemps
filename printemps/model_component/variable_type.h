/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_MODEL_COMPONENT_VARIABLE_TYPE_H__
#define PRINTEMPS_MODEL_COMPONENT_VARIABLE_TYPE_H__

namespace printemps::model_component {
/*****************************************************************************/
enum class VariableType {
    Binary,           //
    Integer,          //
    Selection,        //
    DependentBinary,  //
    DependentInteger  //
};

/*****************************************************************************/
static std::unordered_map<std::string, VariableType> VariableTypeMap = {
    {"Binary", VariableType::Binary},
    {"Integer", VariableType::Integer},
    {"Selection", VariableType::Selection},
    {"DependentBinary", VariableType::DependentBinary},
    {"DependentInteger", VariableType::DependentInteger},

};

/*****************************************************************************/
static std::unordered_map<VariableType, std::string> VariableTypeInverseMap = {
    {VariableType::Binary, "Binary"},
    {VariableType::Integer, "Integer"},
    {VariableType::Selection, "Selection"},
    {VariableType::DependentBinary, "DependentBinary"},
    {VariableType::DependentInteger, "DependentInteger"},
};

}  // namespace printemps::model_component
#endif
/*****************************************************************************/
// END
/*****************************************************************************/
