/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_MPS_MPS_H__
#define PRINTEMPS_MPS_MPS_H__

#include "mps_variable.h"
#include "mps_constraint.h"
#include "mps_objective.h"
#include "mps_read_mode.h"

namespace printemps::mps {
/*****************************************************************************/
struct MPS {
    std::string                                    name;
    std::unordered_map<std::string, MPSVariable>   variables;
    std::unordered_map<std::string, MPSConstraint> constraints;
    MPSObjective                                   objective;

    std::vector<std::string> variable_names;
    std::vector<std::string> constraint_names;

    int number_of_variables;
    int number_of_lesser_constraints;
    int number_of_equal_constraints;
    int number_of_greater_constraints;

    bool is_minimization;

    /*************************************************************************/
    MPS(void) {
        this->initialize();
    }

    /*************************************************************************/
    MPS(const std::string &a_FILE_NAME) {
        this->initialize();
        this->read_mps(a_FILE_NAME);
    }

    /*************************************************************************/
    void initialize(void) {
        this->name = "";
        this->variables.clear();
        this->constraints.clear();
        this->objective.initialize();

        this->variable_names.clear();
        this->constraint_names.clear();

        this->number_of_variables           = 0;
        this->number_of_lesser_constraints  = 0;
        this->number_of_equal_constraints   = 0;
        this->number_of_greater_constraints = 0;

        this->is_minimization = true;
    }

    /*************************************************************************/
    inline void read_mps(const std::string &a_FILE_NAME) {
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
                throw std::runtime_error(utility::format_error_location(
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

            if (items.front().front() == '*') {
                continue;
            }

            if (items.front() == "NAME") {
                read_mode = MPSReadMode::Name;
            } else if (items.front() == "OBJSENSE" && ITEMS_SIZE == 1) {
                read_mode = MPSReadMode::Objsense;
                continue;
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
                        throw std::runtime_error(utility::format_error_location(
                            __FILE__, __LINE__, __func__,
                            "The MPS file has something wrong in NAME "
                            "section."));
                    } else {
                        this->name = items[1];
                    }
                    break;
                }
                case MPSReadMode::Objsense: {
                    if (ITEMS_SIZE > 1) {
                        throw std::runtime_error(utility::format_error_location(
                            __FILE__, __LINE__, __func__,
                            "The MPS file has something wrong in OBJSENSE "
                            "section."));
                    } else {
                        if (items.front() == "MINIMIZATION" ||
                            items.front() == "MINIMIZE" ||
                            items.front() == "MIN" ||
                            items.front() == "minimization" ||
                            items.front() == "minimize" ||
                            items.front() == "min") {
                            this->is_minimization = true;
                        } else if (items.front() == "MAXIMIZATION" ||
                                   items.front() == "MAXIMIZE" ||
                                   items.front() == "MAX" ||
                                   items.front() == "maximization" ||
                                   items.front() == "maximize" ||
                                   items.front() == "max") {
                            this->is_minimization = false;
                        } else {
                            throw std::runtime_error(
                                utility::format_error_location(
                                    __FILE__, __LINE__, __func__,
                                    "The MPS file has something wrong in "
                                    "OBJSENSE "
                                    "section."));
                        }
                    }
                    break;
                }
                case MPSReadMode::Rows: {
                    if (items.front() == "N") {
                        name                 = items[1];
                        this->objective.name = name;
                    } else if (items.front() == "L") {
                        name = items[1];
                        this->constraints[name].sense =
                            MPSConstraintSense::Less;
                        this->constraints[name].name = name;
                        this->constraint_names.push_back(name);
                        this->number_of_lesser_constraints++;
                    } else if (items.front() == "E") {
                        name = items[1];
                        this->constraints[name].sense =
                            MPSConstraintSense::Equal;
                        this->constraints[name].name = name;
                        this->constraint_names.push_back(name);
                        this->number_of_equal_constraints++;
                    } else if (items.front() == "G") {
                        name = items[1];
                        this->constraints[name].sense =
                            MPSConstraintSense::Greater;
                        this->constraints[name].name = name;
                        this->constraint_names.push_back(name);
                        this->number_of_greater_constraints++;
                    } else {
                        throw std::runtime_error(utility::format_error_location(
                            __FILE__, __LINE__, __func__,
                            "The MPS file has something wrong in ROWS "
                            "section."));
                    }
                    break;
                }
                case MPSReadMode::Columns: {
                    if (ITEMS_SIZE < 3) {
                        throw std::runtime_error(utility::format_error_location(
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
                        double sensitivity = atof(items[2 * i + 2].c_str());

                        if (fabs(sensitivity) > constant::EPSILON_10) {
                            if (expression_name == this->objective.name) {
                                this->objective.sensitivities[name] =
                                    sensitivity;
                            } else if (this->constraints.find(
                                           expression_name) !=
                                       this->constraints.end()) {
                                this->constraints[expression_name]
                                    .sensitivities[name] = sensitivity;
                            } else {
                                throw std::runtime_error(
                                    utility::format_error_location(
                                        __FILE__, __LINE__, __func__,
                                        "An undefined constraint or objective "
                                        "function "
                                        "name is specified in COLUMNS section."));
                            }
                        }
                    }
                    if (this->variables.find(name) == this->variables.end()) {
                        this->variables[name].sense = variable_sense;
                        this->variables[name].name  = name;
                        this->variable_names.push_back(name);
                        this->number_of_variables++;
                    }

                    break;
                }
                case MPSReadMode::Rhs: {
                    if (ITEMS_SIZE < 3) {
                        throw std::runtime_error(utility::format_error_location(
                            __FILE__, __LINE__, __func__,
                            "The MPS file has something wrong in RHS "
                            "section."));
                    }

                    for (auto i = 0; i < (ITEMS_SIZE - 1) / 2; i++) {
                        name       = items[2 * i + 1];
                        double rhs = atof(items[2 * i + 2].c_str());

                        this->constraints[name].rhs = rhs;
                    }
                    break;
                }
                case MPSReadMode::Ranges: {
                    /**
                     * The specification of RANGES section is based on the
                     * following site:
                     * https://www.ibm.com/support/knowledgecenter/vi/SSSA5P_20.1.0/ilog.odms.cplex.help/CPLEX/FileFormats/topics/MPS_records.html
                     */
                    if (ITEMS_SIZE < 3) {
                        throw std::runtime_error(utility::format_error_location(
                            __FILE__, __LINE__, __func__,
                            "The MPS file has something wrong in RANGES "
                            "section."));
                    }

                    for (auto i = 0; i < (ITEMS_SIZE - 1) / 2; i++) {
                        name         = items[2 * i + 1];
                        double range = atof(items[2 * i + 2].c_str());
                        name_new     = name + "_range";

                        this->constraints[name_new].name = name_new;
                        this->constraints[name_new].sensitivities =
                            this->constraints[name].sensitivities;
                        switch (this->constraints[name].sense) {
                            case MPSConstraintSense::Less: {
                                this->constraints[name_new].sense =
                                    MPSConstraintSense::Greater;
                                this->constraints[name_new].rhs =
                                    this->constraints[name].rhs - fabs(range);
                                break;
                            }
                            case MPSConstraintSense::Greater: {
                                this->constraints[name_new].sense =
                                    MPSConstraintSense::Less;
                                this->constraints[name_new].rhs =
                                    this->constraints[name].rhs + fabs(range);
                                break;
                            }
                            case MPSConstraintSense::Equal: {
                                if (range > 0) {
                                    this->constraints[name].sense =
                                        MPSConstraintSense::Greater;

                                    this->constraints[name_new].sense =
                                        MPSConstraintSense::Less;
                                    this->constraints[name_new].rhs =
                                        this->constraints[name].rhs +
                                        fabs(range);
                                } else {
                                    this->constraints[name].sense =
                                        MPSConstraintSense::Less;

                                    this->constraints[name_new].sense =
                                        MPSConstraintSense::Greater;
                                    this->constraints[name_new].rhs =
                                        this->constraints[name].rhs -
                                        fabs(range);
                                }
                                break;
                            }
                            default: {
                                throw std::runtime_error(
                                    utility::format_error_location(
                                        __FILE__, __LINE__, __func__,
                                        "The MPS file has something wrong in "
                                        "RANGES section."));
                            }
                        }

                        this->constraint_names.push_back(name_new);
                        this->number_of_lesser_constraints++;
                    }
                    break;
                }
                case MPSReadMode::Bounds: {
                    if (ITEMS_SIZE < 3) {
                        throw std::runtime_error(utility::format_error_location(
                            __FILE__, __LINE__, __func__,
                            "The MPS file has something wrong in BOUNDS "
                            "section."));
                    }

                    category = items.front();
                    name     = items[2];
                    if (this->variables.find(name) == this->variables.end()) {
                        throw std::runtime_error(utility::format_error_location(
                            __FILE__, __LINE__, __func__,
                            "An undefined variable name is specified in RHS "
                            "section."));
                    }
                    if (ITEMS_SIZE == 3) {
                        if (category == "FR") {
                            this->variables[name].is_bound_defined = true;
                            this->variables[name].integer_lower_bound =
                                constant::INT_HALF_MIN;
                            this->variables[name].integer_upper_bound =
                                constant::INT_HALF_MAX;
                            this->variables[name].continuous_lower_bound =
                                -HUGE_VAL;
                            this->variables[name].continuous_upper_bound =
                                HUGE_VAL;
                        } else if (category == "BV") {
                            this->variables[name].sense =
                                MPSVariableSense::Integer;
                            this->variables[name].is_bound_defined       = true;
                            this->variables[name].integer_lower_bound    = 0;
                            this->variables[name].integer_upper_bound    = 1;
                            this->variables[name].continuous_lower_bound = 0;
                            this->variables[name].continuous_upper_bound = 1;
                        } else if (category == "MI") {
                            this->variables[name].is_bound_defined = true;
                            this->variables[name].integer_lower_bound =
                                constant::INT_HALF_MIN;
                            this->variables[name].integer_upper_bound = 0;
                            this->variables[name].continuous_lower_bound =
                                -HUGE_VAL;
                            this->variables[name].continuous_upper_bound = 0;
                        } else if (category == "PL") {
                            this->variables[name].is_bound_defined    = true;
                            this->variables[name].integer_lower_bound = 0;
                            this->variables[name].integer_upper_bound =
                                constant::INT_HALF_MAX;
                            this->variables[name].continuous_lower_bound = 0;
                            this->variables[name].continuous_upper_bound =
                                HUGE_VAL;
                        }
                    } else if (ITEMS_SIZE == 4) {
                        double continuous_value = atof(items[3].c_str());
                        int integer_value = static_cast<int>(continuous_value);

                        if (category == "LO") {
                            this->variables[name].is_bound_defined = true;
                            this->variables[name].integer_lower_bound =
                                integer_value;
                            this->variables[name].continuous_lower_bound =
                                continuous_value;
                        } else if (category == "LI") {
                            this->variables[name].sense =
                                MPSVariableSense::Integer;
                            this->variables[name].is_bound_defined = true;
                            this->variables[name].integer_lower_bound =
                                integer_value;
                            this->variables[name].continuous_lower_bound =
                                continuous_value;
                        } else if (category == "UP") {
                            this->variables[name].is_bound_defined = true;
                            this->variables[name].integer_upper_bound =
                                integer_value;
                            this->variables[name].continuous_upper_bound =
                                continuous_value;
                        } else if (category == "UI") {
                            this->variables[name].sense =
                                MPSVariableSense::Integer;
                            this->variables[name].is_bound_defined = true;
                            this->variables[name].integer_upper_bound =
                                integer_value;
                            this->variables[name].continuous_upper_bound =
                                continuous_value;
                        } else if (category == "FX") {
                            this->variables[name].is_bound_defined = true;
                            this->variables[name].is_fixed         = true;
                            this->variables[name].integer_fixed_value =
                                integer_value;
                            this->variables[name].continuous_fixed_value =
                                continuous_value;
                            this->variables[name].integer_lower_bound =
                                integer_value;
                            this->variables[name].integer_upper_bound =
                                integer_value;
                            this->variables[name].continuous_lower_bound =
                                continuous_value;
                            this->variables[name].continuous_upper_bound =
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
            throw std::runtime_error(utility::format_error_location(
                __FILE__, __LINE__, __func__, "The MPS file is not valid."));
        }

        /**
         * The default lower and upper bounds for integer variable is 0 and 1,
         * respectively.
         * http://webpages.iust.ac.ir/yaghini/Courses/RTP_882/ILOG_CPLEX_03.pdf
         */
        for (auto &&variable : this->variables) {
            if (!variable.second.is_bound_defined &&
                variable.second.sense == MPSVariableSense::Integer) {
                variable.second.is_bounded          = true;
                variable.second.integer_lower_bound = 0;
                variable.second.integer_upper_bound = 1;
            }
        }
    }
};
}  // namespace printemps::mps
#endif
/*****************************************************************************/
// END
/*****************************************************************************/