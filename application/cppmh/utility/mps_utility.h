/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef CPPMH_UTILITY_MPS_UTILITY_H__
#define CPPMH_UTILITY_MPS_UTILITY_H__

#include <iostream>
#include <cmath>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <unordered_map>

#include <cppmh.h>

namespace cppmh {
namespace utility {

/*****************************************************************************/
enum class MPSVariableSense { Integer, Continuous };

/*****************************************************************************/
enum class MPSConstraintSense { Lower, Equal, Upper };

/*****************************************************************************/
enum class MPSObjectiveSense { Minimize, Maximize };

/*****************************************************************************/
enum class MPSReadMode { Initial, Name, Rows, Columns, Rhs, Bounds, Endata };

/*****************************************************************************/
struct MPSVariable {
    MPSVariableSense sense;
    std::string      name;
    int              integer_lower_bound;
    int              integer_upper_bound;
    int              integer_fixed_value;
    double           continuous_lower_bound;
    double           continuous_upper_bound;
    double           continuous_fixed_value;
    bool             is_bounded;
    bool             is_bound_defined;
    bool             is_fixed;

    /*************************************************************************/
    MPSVariable(void) {
        this->initialize();
    }

    /*************************************************************************/
    ~MPSVariable(void) {
        /// nothing to do
    }

    /*************************************************************************/
    inline void initialize(void) {
        sense                  = MPSVariableSense::Continuous;
        name                   = "";
        integer_lower_bound    = 0;
        integer_upper_bound    = std::numeric_limits<int>::max() - 1;
        integer_fixed_value    = 0;
        continuous_lower_bound = 0;
        continuous_upper_bound = HUGE_VAL;
        continuous_fixed_value = 0;
        is_bounded             = false;
        is_bound_defined       = false;
        is_fixed               = false;
    }
};

/*****************************************************************************/
struct MPSConstraint {
    MPSConstraintSense                      sense;
    std::string                             name;
    std::unordered_map<std::string, double> sensitivities;
    double                                  rhs;

    /*************************************************************************/
    MPSConstraint(void) {
        this->initialize();
    }

    /*************************************************************************/
    ~MPSConstraint(void) {
        /// nothing to do
    }

    /*************************************************************************/
    inline void initialize(void) {
        sense = MPSConstraintSense::Lower;
        name  = "";
        sensitivities.clear();
        rhs = 0.0;
    }
};

/*****************************************************************************/
struct MPSObjective {
    MPSObjectiveSense                       sense;
    std::string                             name;
    std::unordered_map<std::string, double> sensitivities;

    /*************************************************************************/
    MPSObjective(void) {
        this->initialize();
    }

    /*************************************************************************/
    ~MPSObjective(void) {
        /// nothing to do
    }

    /*************************************************************************/
    inline void initialize(void) {
        sense = MPSObjectiveSense::Minimize;
        name  = "";
        sensitivities.clear();
    }
};

/*****************************************************************************/
struct MPS {
    std::string                                    name;
    std::unordered_map<std::string, MPSVariable>   variables;
    std::unordered_map<std::string, MPSConstraint> constraints;
    MPSObjective                                   objective;

    std::vector<std::string> variable_names;
    std::vector<std::string> constraint_names;

    /*************************************************************************/
    MPS(void) {
        this->initialize();
    }

    /*************************************************************************/
    inline void initialize(void) {
        name = "";
        variables.clear();
        constraints.clear();
        objective.initialize();

        variable_names.clear();
        constraint_names.clear();
    }
};

/*****************************************************************************/
inline MPS read_mps(const std::string &a_FILE_NAME) {
    MPS mps;

    std::vector<std::string> lines;
    std::string              item;

    /**
     * Read and store entire part of the mps file.
     */
    {
        std::ifstream input_file_stream;
        std::string   buffer;

        input_file_stream.open(a_FILE_NAME.c_str());
        while (std::getline(input_file_stream, buffer)) {
            lines.push_back(buffer);
        }
        input_file_stream.close();
    }

    MPSReadMode      read_mode      = MPSReadMode::Initial;
    MPSVariableSense variable_sense = MPSVariableSense::Continuous;

    /**
     * Parse the mps file.
     */
    for (const auto &line : lines) {
        std::stringstream        stream(line);
        std::vector<std::string> items;
        while (stream >> item) {
            items.push_back(item);
        }
        int items_size = items.size();

        if (items.front() == "NAME") {
            read_mode = MPSReadMode::Name;

        } else if (items.front() == "ROWS") {
            read_mode = MPSReadMode::Rows;
            continue;
        } else if (items.front() == "COLUMNS") {
            read_mode = MPSReadMode::Columns;
            continue;
        } else if (items.front() == "RHS") {
            read_mode = MPSReadMode::Rhs;
            continue;
        } else if (items.front() == "BOUNDS") {
            read_mode = MPSReadMode::Bounds;
            continue;
        } else if (items.front() == "ENDATA") {
            read_mode = MPSReadMode::Endata;
            break;
        }
        switch (read_mode) {
            case MPSReadMode::Name: {
                if (items_size < 2) {
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
                    std::string o_name = items[1];
                    mps.objective.name = o_name;
                } else if (items.front() == "L") {
                    std::string c_name            = items[1];
                    mps.constraints[c_name].sense = MPSConstraintSense::Lower;
                    mps.constraints[c_name].name  = c_name;
                    mps.constraint_names.push_back(c_name);
                } else if (items.front() == "E") {
                    std::string c_name            = items[1];
                    mps.constraints[c_name].sense = MPSConstraintSense::Equal;
                    mps.constraints[c_name].name  = c_name;
                    mps.constraint_names.push_back(c_name);
                } else if (items.front() == "G") {
                    std::string c_name            = items[1];
                    mps.constraints[c_name].sense = MPSConstraintSense::Upper;
                    mps.constraints[c_name].name  = c_name;
                    mps.constraint_names.push_back(c_name);
                } else {
                    throw std::logic_error(utility::format_error_location(
                        __FILE__, __LINE__, __func__,
                        "The MPS file has something wrong in ROWS section."));
                }
                break;
            }
            case MPSReadMode::Columns: {
                if (items_size < 3) {
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

                std::string v_name = items[0];
                for (auto i = 0; i < (items_size - 1) / 2; i++) {
                    std::string expression_name = items[2 * i + 1];
                    double      sensitivity = atof(items[2 * i + 2].c_str());

                    if (expression_name == mps.objective.name) {
                        mps.objective.sensitivities[v_name] = sensitivity;
                    } else if (mps.constraints.find(expression_name) !=
                               mps.constraints.end()) {
                        mps.constraints[expression_name].sensitivities[v_name] =
                            sensitivity;
                    } else {
                        throw std::logic_error(utility::format_error_location(
                            __FILE__, __LINE__, __func__,
                            "An undefined constraint or objective function "
                            "name is specified in RHS section."));
                    }
                }
                if (mps.variables.find(v_name) == mps.variables.end()) {
                    mps.variables[v_name].sense = variable_sense;
                    mps.variables[v_name].name  = v_name;
                    mps.variable_names.push_back(v_name);
                }

                break;
            }
            case MPSReadMode::Rhs: {
                if (items_size < 3) {
                    throw std::logic_error(utility::format_error_location(
                        __FILE__, __LINE__, __func__,
                        "The MPS file has something wrong in RHS section."));
                }

                for (auto i = 0; i < (items_size - 1) / 2; i++) {
                    std::string c_name = items[2 * i + 1];
                    double      rhs    = atof(items[2 * i + 2].c_str());
                    mps.constraints[c_name].rhs = rhs;
                }
                break;
            }
            case MPSReadMode::Bounds: {
                if (items_size < 3) {
                    throw std::logic_error(utility::format_error_location(
                        __FILE__, __LINE__, __func__,
                        "The MPS file has something wrong in BOUNDS section."));
                }

                std::string category = items.front();
                std::string v_name   = items[2];
                if (mps.variables.find(v_name) == mps.variables.end()) {
                    throw std::logic_error(utility::format_error_location(
                        __FILE__, __LINE__, __func__,
                        "An undefined decision variable name is specified in "
                        "RHS section."));
                }
                if (items_size == 3) {
                    if (category == "FR") {
                        mps.variables[v_name].is_bounded       = false;
                        mps.variables[v_name].is_bound_defined = true;
                        mps.variables[v_name].integer_lower_bound =
                            std::numeric_limits<int>::min() + 1;
                        mps.variables[v_name].integer_upper_bound =
                            std::numeric_limits<int>::max() - 1;
                        mps.variables[v_name].continuous_lower_bound =
                            -HUGE_VAL;
                        mps.variables[v_name].continuous_upper_bound = HUGE_VAL;
                    } else if (category == "BV") {
                        mps.variables[v_name].is_bounded          = true;
                        mps.variables[v_name].is_bound_defined    = true;
                        mps.variables[v_name].integer_lower_bound = 0;
                        mps.variables[v_name].integer_upper_bound = 1;
                    }
                } else if (items_size == 4) {
                    int    integer_value    = atoi(items[3].c_str());
                    double continuous_value = atof(items[3].c_str());
                    if (category == "LO") {
                        mps.variables[v_name].is_bounded       = true;
                        mps.variables[v_name].is_bound_defined = true;
                        mps.variables[v_name].integer_lower_bound =
                            integer_value;
                        mps.variables[v_name].continuous_lower_bound =
                            continuous_value;
                    } else if (category == "LI") {
                        mps.variables[v_name].is_bounded       = true;
                        mps.variables[v_name].is_bound_defined = true;
                        mps.variables[v_name].integer_lower_bound =
                            integer_value;
                    } else if (category == "UP") {
                        mps.variables[v_name].is_bounded       = true;
                        mps.variables[v_name].is_bound_defined = true;
                        mps.variables[v_name].integer_upper_bound =
                            integer_value;
                        mps.variables[v_name].continuous_upper_bound =
                            continuous_value;
                    } else if (category == "UI") {
                        mps.variables[v_name].is_bounded       = true;
                        mps.variables[v_name].is_bound_defined = true;
                        mps.variables[v_name].integer_upper_bound =
                            integer_value;
                    } else if (category == "FX") {
                        mps.variables[v_name].is_bounded       = false;
                        mps.variables[v_name].is_bound_defined = true;
                        mps.variables[v_name].is_fixed         = true;
                        mps.variables[v_name].integer_fixed_value =
                            integer_value;
                        mps.variables[v_name].continuous_fixed_value =
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

/*****************************************************************************/
class MPSReader {
   private:
    model::IPModel m_model;

   public:
    /*************************************************************************/
    MPSReader(void) {
        this->initialize();
    }

    /*************************************************************************/
    ~MPSReader(void) {
        /* nothing to do */
    }

    /*************************************************************************/
    inline void initialize(void) {
        m_model.initialize();
    }

    /*************************************************************************/
    inline model::IPModel &create_model_from_mps(
        const std::string &a_FILE_NAME) {
        MPS mps = read_mps(a_FILE_NAME);

        std::unordered_map<std::string, model::IPVariable *> variable_ptrs;

        auto &variable_proxy =
            m_model.create_variables("variables", mps.variables.size());
        int count = 0;
        for (const auto &name : mps.variable_names) {
            auto &variable = mps.variables[name];

            if (variable.sense == MPSVariableSense::Continuous) {
                throw std::logic_error(utility::format_error_location(
                    __FILE__, __LINE__, __func__,
                    "The problem defined in the MPS file must not include "
                    "continuous variables."));
            }
            if (variable.is_bounded) {
                variable_proxy(count).set_bound(variable.integer_lower_bound,
                                                variable.integer_upper_bound);
            }
            if (variable.is_fixed) {
                variable_proxy(count).fix_by(variable.integer_fixed_value);
            }

            variable_proxy(count).set_name(name);
            variable_ptrs[name] = &variable_proxy(count);

            count++;
        }

        auto &constraint_proxy =
            m_model.create_constraints("constraints", mps.constraints.size());

        count = 0;
        for (const auto &name : mps.constraint_names) {
            auto &constraint      = mps.constraints[name];
            auto  expression_temp = model::IPExpression::create_instance();

            for (const auto &sensitivity : constraint.sensitivities) {
                std::string v_name      = sensitivity.first;
                double      coefficient = sensitivity.second;
                expression_temp += coefficient * (*variable_ptrs[v_name]);
            }

            switch (constraint.sense) {
                case cppmh::utility::MPSConstraintSense::Lower: {
                    constraint_proxy(count) = expression_temp <= constraint.rhs;
                    break;
                }

                case cppmh::utility::MPSConstraintSense::Equal: {
                    constraint_proxy(count) = expression_temp == constraint.rhs;
                    break;
                }

                case cppmh::utility::MPSConstraintSense::Upper: {
                    constraint_proxy(count) = expression_temp >= constraint.rhs;
                    break;
                }
            }

            constraint_proxy(count).set_name(name);
            count++;
        }

        auto objective_temp = model::IPExpression::create_instance();
        for (const auto &sensitivity : mps.objective.sensitivities) {
            std::string v_name      = sensitivity.first;
            double      coefficient = sensitivity.second;
            objective_temp += coefficient * (*variable_ptrs[v_name]);
        }
        m_model.minimize(objective_temp);

        return m_model;
    }
};

// namespace utility
}  // namespace utility
}  // namespace cppmh
#endif
/*****************************************************************************/
// END
/*****************************************************************************/