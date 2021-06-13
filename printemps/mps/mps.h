/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_MPS_MPS_H__
#define PRINTEMPS_MPS_MPS_H__

#include "mps_variable.h"
#include "mps_constraint.h"
#include "mps_objective.h"
#include "mps_read_mode.h"

namespace printemps {
namespace mps {
/*****************************************************************************/
struct MPS {
    std::string                                    name;
    std::unordered_map<std::string, MPSVariable>   variables;
    std::unordered_map<std::string, MPSConstraint> constraints;
    MPSObjective                                   objective;

    std::vector<std::string> variable_names;
    std::vector<std::string> constraint_names;

    int number_of_variables;
    int number_of_lower_constraints;
    int number_of_equal_constraints;
    int number_of_upper_constraints;

    /*************************************************************************/
    MPS(void) {
        this->initialize();
    }

    /*************************************************************************/
    void initialize(void) {
        this->name = "";
        this->variables.clear();
        this->constraints.clear();
        this->objective.initialize();

        this->variable_names.clear();
        this->constraint_names.clear();

        this->number_of_variables         = 0;
        this->number_of_lower_constraints = 0;
        this->number_of_equal_constraints = 0;
        this->number_of_upper_constraints = 0;
    }
};

/*****************************************************************************/
inline MPS read_mps(const std::string &a_FILE_NAME) {
    MPS mps;

    bool is_valid = false;

    std::vector<std::string> lines;
    std::string              item;

    /**
     * Read and store entire part of the mps file.
     */
    {
        std::ifstream ifs;
        std::string   buffer;

        ifs.open(a_FILE_NAME.c_str());
        if (ifs.fail()) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "Cannot open the specified MPS file: " + a_FILE_NAME));
        }
        while (std::getline(ifs, buffer)) {
            lines.push_back(buffer);
        }
        ifs.close();
    }

    MPSReadMode      read_mode      = MPSReadMode::Initial;
    MPSVariableSense variable_sense = MPSVariableSense::Continuous;

    std::stringstream        stream;
    std::vector<std::string> items;
    std::string              name;
    std::string              name_new;
    std::string              category;

    /**
     * Parse the mps file.
     */
    for (const auto &line : lines) {
        stream.str(line);
        stream.clear(std::stringstream::goodbit);
        items.clear();

        while (stream >> item) {
            items.push_back(item);
        }
        const int ITEMS_SIZE = items.size();

        if (ITEMS_SIZE == 0) {
            continue;
        }

        if (items.front() == "NAME") {
            read_mode = MPSReadMode::Name;
        } else if (items.front() == "ROWS" && ITEMS_SIZE == 1) {
            read_mode = MPSReadMode::Rows;
            continue;
        } else if (items.front() == "COLUMNS" && ITEMS_SIZE == 1) {
            read_mode = MPSReadMode::Columns;
            continue;
        } else if (items.front() == "RHS" && ITEMS_SIZE == 1) {
            read_mode = MPSReadMode::Rhs;
            continue;
        } else if (items.front() == "RANGES" && ITEMS_SIZE == 1) {
            read_mode = MPSReadMode::Ranges;
            continue;
        } else if (items.front() == "BOUNDS" && ITEMS_SIZE == 1) {
            read_mode = MPSReadMode::Bounds;
            continue;
        } else if (items.front() == "ENDATA" && ITEMS_SIZE == 1) {
            read_mode = MPSReadMode::Endata;
            is_valid  = true;
            break;
        }
        switch (read_mode) {
            case MPSReadMode::Name: {
                if (ITEMS_SIZE < 2) {
                    throw std::logic_error(utility::format_error_location(
                        __FILE__, __LINE__, __func__,
                        "The MPS file has something wrong in NAME section."));
                } else {
                    mps.name = items[1];
                }
                break;
            }
            case MPSReadMode::Rows: {
                if (items.front() == "N") {
                    name               = items[1];
                    mps.objective.name = name;
                } else if (items.front() == "L") {
                    name                        = items[1];
                    mps.constraints[name].sense = MPSConstraintSense::Less;
                    mps.constraints[name].name  = name;
                    mps.constraint_names.push_back(name);
                    mps.number_of_lower_constraints++;
                } else if (items.front() == "E") {
                    name                        = items[1];
                    mps.constraints[name].sense = MPSConstraintSense::Equal;
                    mps.constraints[name].name  = name;
                    mps.constraint_names.push_back(name);
                    mps.number_of_equal_constraints++;
                } else if (items.front() == "G") {
                    name                        = items[1];
                    mps.constraints[name].sense = MPSConstraintSense::Greater;
                    mps.constraints[name].name  = name;
                    mps.constraint_names.push_back(name);
                    mps.number_of_upper_constraints++;
                } else {
                    throw std::logic_error(utility::format_error_location(
                        __FILE__, __LINE__, __func__,
                        "The MPS file has something wrong in ROWS section."));
                }
                break;
            }
            case MPSReadMode::Columns: {
                if (ITEMS_SIZE < 3) {
                    throw std::logic_error(utility::format_error_location(
                        __FILE__, __LINE__, __func__,
                        "The MPS file has something wrong in COLUMNS "
                        "section."));
                }

                if (items[2] == "'INTORG'") {
                    variable_sense = MPSVariableSense::Integer;
                    break;
                } else if (items[2] == "'INTEND'") {
                    variable_sense = MPSVariableSense::Continuous;
                    break;
                }

                name = items[0];
                for (auto i = 0; i < (ITEMS_SIZE - 1) / 2; i++) {
                    std::string expression_name = items[2 * i + 1];
                    double      sensitivity = atof(items[2 * i + 2].c_str());

                    if (fabs(sensitivity) > constant::EPSILON_10) {
                        if (expression_name == mps.objective.name) {
                            mps.objective.sensitivities[name] = sensitivity;
                        } else if (mps.constraints.find(expression_name) !=
                                   mps.constraints.end()) {
                            mps.constraints[expression_name]
                                .sensitivities[name] = sensitivity;
                        } else {
                            throw std::logic_error(
                                utility::format_error_location(
                                    __FILE__, __LINE__, __func__,
                                    "An undefined constraint or objective "
                                    "function "
                                    "name is specified in RHS section."));
                        }
                    }
                }
                if (mps.variables.find(name) == mps.variables.end()) {
                    mps.variables[name].sense = variable_sense;
                    mps.variables[name].name  = name;
                    mps.variable_names.push_back(name);
                    mps.number_of_variables++;
                }

                break;
            }
            case MPSReadMode::Rhs: {
                if (ITEMS_SIZE < 3) {
                    throw std::logic_error(utility::format_error_location(
                        __FILE__, __LINE__, __func__,
                        "The MPS file has something wrong in RHS section."));
                }

                for (auto i = 0; i < (ITEMS_SIZE - 1) / 2; i++) {
                    name       = items[2 * i + 1];
                    double rhs = atof(items[2 * i + 2].c_str());

                    mps.constraints[name].rhs = rhs;
                }
                break;
            }
            case MPSReadMode::Ranges: {
                /**
                 * The specification of RANGES section is based on the following
                 * site:
                 * https://www.ibm.com/support/knowledgecenter/vi/SSSA5P_20.1.0/ilog.odms.cplex.help/CPLEX/FileFormats/topics/MPS_records.html
                 */
                if (ITEMS_SIZE < 3) {
                    throw std::logic_error(utility::format_error_location(
                        __FILE__, __LINE__, __func__,
                        "The MPS file has something wrong in RANGES section."));
                }

                for (auto i = 0; i < (ITEMS_SIZE - 1) / 2; i++) {
                    name         = items[2 * i + 1];
                    double range = atof(items[2 * i + 2].c_str());
                    name_new     = name + "_range";

                    mps.constraints[name_new].name = name_new;
                    mps.constraints[name_new].sensitivities =
                        mps.constraints[name].sensitivities;
                    switch (mps.constraints[name].sense) {
                        case MPSConstraintSense::Less: {
                            mps.constraints[name_new].sense =
                                MPSConstraintSense::Greater;
                            mps.constraints[name_new].rhs =
                                mps.constraints[name].rhs + fabs(range);
                            break;
                        }
                        case MPSConstraintSense::Greater: {
                            mps.constraints[name_new].sense =
                                MPSConstraintSense::Less;
                            mps.constraints[name_new].rhs =
                                mps.constraints[name].rhs - fabs(range);
                            break;
                        }
                        case MPSConstraintSense::Equal: {
                            if (range > 0) {
                                mps.constraints[name_new].sense =
                                    MPSConstraintSense::Greater;
                                mps.constraints[name_new].rhs =
                                    mps.constraints[name].rhs + fabs(range);
                            } else {
                                mps.constraints[name_new].sense =
                                    MPSConstraintSense::Less;
                                mps.constraints[name_new].rhs =
                                    mps.constraints[name].rhs - fabs(range);
                            }
                            break;
                        }
                        default: {
                            throw std::logic_error(
                                utility::format_error_location(
                                    __FILE__, __LINE__, __func__,
                                    "The MPS file has something wrong in "
                                    "RANGES section."));
                        }
                    }

                    mps.constraint_names.push_back(name_new);
                    mps.number_of_lower_constraints++;
                }
                break;
            }
            case MPSReadMode::Bounds: {
                if (ITEMS_SIZE < 3) {
                    throw std::logic_error(utility::format_error_location(
                        __FILE__, __LINE__, __func__,
                        "The MPS file has something wrong in BOUNDS section."));
                }

                category = items.front();
                name     = items[2];
                if (mps.variables.find(name) == mps.variables.end()) {
                    throw std::logic_error(utility::format_error_location(
                        __FILE__, __LINE__, __func__,
                        "An undefined decision variable name is specified in "
                        "RHS section."));
                }
                if (ITEMS_SIZE == 3) {
                    if (category == "FR") {
                        mps.variables[name].is_bound_defined = true;
                        mps.variables[name].integer_lower_bound =
                            constant::INT_HALF_MIN;
                        mps.variables[name].integer_upper_bound =
                            constant::INT_HALF_MAX;
                        mps.variables[name].continuous_lower_bound = -HUGE_VAL;
                        mps.variables[name].continuous_upper_bound = HUGE_VAL;
                    } else if (category == "BV") {
                        mps.variables[name].sense = MPSVariableSense::Integer;
                        mps.variables[name].is_bound_defined       = true;
                        mps.variables[name].integer_lower_bound    = 0;
                        mps.variables[name].integer_upper_bound    = 1;
                        mps.variables[name].continuous_lower_bound = 0;
                        mps.variables[name].continuous_upper_bound = 1;
                    } else if (category == "MI") {
                        mps.variables[name].is_bound_defined = true;
                        mps.variables[name].integer_lower_bound =
                            constant::INT_HALF_MIN;
                        mps.variables[name].integer_upper_bound    = 0;
                        mps.variables[name].continuous_lower_bound = -HUGE_VAL;
                        mps.variables[name].continuous_upper_bound = 0;
                    } else if (category == "PL") {
                        mps.variables[name].is_bound_defined    = true;
                        mps.variables[name].integer_lower_bound = 0;
                        mps.variables[name].integer_upper_bound =
                            constant::INT_HALF_MAX;
                        mps.variables[name].continuous_lower_bound = 0;
                        mps.variables[name].continuous_upper_bound = HUGE_VAL;
                    }
                } else if (ITEMS_SIZE == 4) {
                    double continuous_value = atof(items[3].c_str());
                    int    integer_value = static_cast<int>(continuous_value);

                    if (category == "LO") {
                        mps.variables[name].is_bound_defined    = true;
                        mps.variables[name].integer_lower_bound = integer_value;
                        mps.variables[name].continuous_lower_bound =
                            continuous_value;
                    } else if (category == "LI") {
                        mps.variables[name].sense = MPSVariableSense::Integer;
                        mps.variables[name].is_bound_defined    = true;
                        mps.variables[name].integer_lower_bound = integer_value;
                        mps.variables[name].continuous_lower_bound =
                            continuous_value;
                    } else if (category == "UP") {
                        mps.variables[name].is_bound_defined    = true;
                        mps.variables[name].integer_upper_bound = integer_value;
                        mps.variables[name].continuous_upper_bound =
                            continuous_value;
                    } else if (category == "UI") {
                        mps.variables[name].sense = MPSVariableSense::Integer;
                        mps.variables[name].is_bound_defined    = true;
                        mps.variables[name].integer_upper_bound = integer_value;
                        mps.variables[name].continuous_upper_bound =
                            continuous_value;
                    } else if (category == "FX") {
                        mps.variables[name].is_bound_defined    = true;
                        mps.variables[name].is_fixed            = true;
                        mps.variables[name].integer_fixed_value = integer_value;
                        mps.variables[name].continuous_fixed_value =
                            continuous_value;
                        mps.variables[name].integer_lower_bound = integer_value;
                        mps.variables[name].integer_upper_bound = integer_value;
                        mps.variables[name].continuous_lower_bound =
                            continuous_value;
                        mps.variables[name].continuous_upper_bound =
                            continuous_value;
                    }
                }
                break;
            }
            default: {
                /// nothing to do
            }
        }
    }

    if (!is_valid) {
        throw std::logic_error(utility::format_error_location(
            __FILE__, __LINE__, __func__, "The MPS file is not valid."));
    }

    /**
     * The default lower and upper bounds for integer decision variable is 0
     * and 1, respectively.
     * http://webpages.iust.ac.ir/yaghini/Courses/RTP_882/ILOG_CPLEX_03.pdf
     */
    for (auto &&variable : mps.variables) {
        if (!variable.second.is_bound_defined &&
            variable.second.sense == MPSVariableSense::Integer) {
            variable.second.is_bounded          = true;
            variable.second.integer_lower_bound = 0;
            variable.second.integer_upper_bound = 1;
        }
    }

    return mps;
}
}  // namespace mps
}  // namespace printemps
#endif
/*****************************************************************************/
// END
/*****************************************************************************/