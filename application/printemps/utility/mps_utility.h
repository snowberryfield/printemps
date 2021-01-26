/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_UTILITY_MPS_UTILITY_H__
#define PRINTEMPS_UTILITY_MPS_UTILITY_H__

#include <iostream>
#include <cmath>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <unordered_map>

#ifdef _OPENMP
#include <omp.h>
#endif

#include <printemps.h>

namespace printemps {
namespace utility {
/*****************************************************************************/
enum class MPSVariableSense { Integer, Continuous };

/*****************************************************************************/
enum class MPSConstraintSense { Lower, Equal, Upper };

/*****************************************************************************/
enum class MPSObjectiveSense { Minimize, Maximize };

/*****************************************************************************/
enum class MPSReadMode {
    Initial,
    Name,
    Rows,
    Columns,
    Rhs,
    Ranges,
    Bounds,
    Endata
};

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
    void initialize(void) {
        sense                  = MPSVariableSense::Continuous;
        name                   = "";
        integer_lower_bound    = 0;
        integer_upper_bound    = constant::INT_HALF_MAX;
        integer_fixed_value    = 0;
        continuous_lower_bound = 0;
        continuous_upper_bound = HUGE_VAL;
        continuous_fixed_value = 0;
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
    void initialize(void) {
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
    void initialize(void) {
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
        name = "";
        variables.clear();
        constraints.clear();
        objective.initialize();

        variable_names.clear();
        constraint_names.clear();

        number_of_variables         = 0;
        number_of_lower_constraints = 0;
        number_of_equal_constraints = 0;
        number_of_upper_constraints = 0;
    }
};

/*****************************************************************************/
MPS read_mps(const std::string &a_FILE_NAME) {
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
        int items_size = items.size();

        if (items_size == 0) {
            continue;
        }

        if (items.front() == "NAME") {
            read_mode = MPSReadMode::Name;
        } else if (items.front() == "ROWS" && items_size == 1) {
            read_mode = MPSReadMode::Rows;
            continue;
        } else if (items.front() == "COLUMNS" && items_size == 1) {
            read_mode = MPSReadMode::Columns;
            continue;
        } else if (items.front() == "RHS" && items_size == 1) {
            read_mode = MPSReadMode::Rhs;
            continue;
        } else if (items.front() == "RANGES" && items_size == 1) {
            read_mode = MPSReadMode::Ranges;
            continue;
        } else if (items.front() == "BOUNDS" && items_size == 1) {
            read_mode = MPSReadMode::Bounds;
            continue;
        } else if (items.front() == "ENDATA" && items_size == 1) {
            read_mode = MPSReadMode::Endata;
            is_valid  = true;
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
                    name               = items[1];
                    mps.objective.name = name;
                } else if (items.front() == "L") {
                    name                        = items[1];
                    mps.constraints[name].sense = MPSConstraintSense::Lower;
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
                    mps.constraints[name].sense = MPSConstraintSense::Upper;
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

                name = items[0];
                for (auto i = 0; i < (items_size - 1) / 2; i++) {
                    std::string expression_name = items[2 * i + 1];
                    double      sensitivity = atof(items[2 * i + 2].c_str());

                    if (expression_name == mps.objective.name) {
                        mps.objective.sensitivities[name] = sensitivity;
                    } else if (mps.constraints.find(expression_name) !=
                               mps.constraints.end()) {
                        mps.constraints[expression_name].sensitivities[name] =
                            sensitivity;
                    } else {
                        throw std::logic_error(utility::format_error_location(
                            __FILE__, __LINE__, __func__,
                            "An undefined constraint or objective function "
                            "name is specified in RHS section."));
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
                if (items_size < 3) {
                    throw std::logic_error(utility::format_error_location(
                        __FILE__, __LINE__, __func__,
                        "The MPS file has something wrong in RHS section."));
                }

                for (auto i = 0; i < (items_size - 1) / 2; i++) {
                    name       = items[2 * i + 1];
                    double rhs = atof(items[2 * i + 2].c_str());

                    mps.constraints[name].rhs = rhs;
                }
                break;
            }
            case MPSReadMode::Ranges: {
                throw std::logic_error(utility::format_error_location(
                    __FILE__, __LINE__, __func__,
                    "This program does not compatible to RANGES section."));
                break;
            }
            case MPSReadMode::Bounds: {
                if (items_size < 3) {
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
                if (items_size == 3) {
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
                } else if (items_size == 4) {
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
    void initialize(void) {
        m_model.initialize();
    }

    /*************************************************************************/
    model::IPModel &create_model_from_mps(
        const std::string &a_FILE_NAME,
        const bool         a_IS_ENABLED_SEPARATE_EQUALITY,
        const bool         a_ACCEPT_CONTINUOUS) {
        MPS mps = read_mps(a_FILE_NAME);
        std::unordered_map<std::string, model::IPVariable *> variable_ptrs;

        auto &variable_proxy =
            m_model.create_variables("variables", mps.variables.size());

        /**
         * Set up the decision variables.
         */
        int number_of_variables = mps.variable_names.size();

        for (auto i = 0; i < number_of_variables; i++) {
            auto &name     = mps.variable_names[i];
            auto &variable = mps.variables[name];

            if (variable.sense == MPSVariableSense::Continuous) {
                if (a_ACCEPT_CONTINUOUS) {
                    utility::print_warning(
                        "The continuous variable " + name +
                            " will be regarded as an integer variable.",
                        true);
                } else {
                    throw std::logic_error(utility::format_error_location(
                        __FILE__, __LINE__, __func__,
                        "The MPS file includes continuous variables."));
                }
            }

            variable_proxy(i).set_bound(variable.integer_lower_bound,
                                        variable.integer_upper_bound);

            if (variable.is_fixed) {
                variable_proxy(i).fix_by(variable.integer_fixed_value);
            }

            variable_proxy(i).set_name(name);
            variable_ptrs[name] = &variable_proxy(i);
        }

        /**
         * Set up the constraints.
         */
        int raw_number_of_constraints = mps.constraint_names.size();
        int mod_number_of_constraints = raw_number_of_constraints;
        std::vector<int> offsets(raw_number_of_constraints);

        if (a_IS_ENABLED_SEPARATE_EQUALITY) {
            mod_number_of_constraints = mps.number_of_lower_constraints +
                                        2 * mps.number_of_equal_constraints +
                                        mps.number_of_upper_constraints;
        }

        auto &constraint_proxy = m_model.create_constraints(
            "constraints", mod_number_of_constraints);

        int offset = 0;
        for (auto i = 0; i < raw_number_of_constraints; i++) {
            auto &name       = mps.constraint_names[i];
            auto &constraint = mps.constraints[name];
            offsets[i]       = offset++;
            if (a_IS_ENABLED_SEPARATE_EQUALITY &&
                (constraint.sense == MPSConstraintSense::Equal)) {
                offset++;
            }
        }
#ifdef _OPENMP
#pragma omp parallel for schedule(static)
#endif
        for (auto i = 0; i < raw_number_of_constraints; i++) {
            auto &name       = mps.constraint_names[i];
            auto &constraint = mps.constraints[name];
            int   offset     = offsets[i];
            auto  expression = model::IPExpression::create_instance();

            std::unordered_map<model::IPVariable *, double>
                expression_sensitivities;
            for (const auto &sensitivity : constraint.sensitivities) {
                std::string variable_name = sensitivity.first;
                double      coefficient   = sensitivity.second;
                expression_sensitivities[variable_ptrs[variable_name]] =
                    coefficient;
            }
            expression.set_sensitivities(expression_sensitivities);

            switch (constraint.sense) {
                case MPSConstraintSense::Lower: {
                    constraint_proxy(offset) = (expression <= constraint.rhs);
                    constraint_proxy(offset).set_name(name);
                    break;
                }

                case MPSConstraintSense::Equal: {
                    if (a_IS_ENABLED_SEPARATE_EQUALITY) {
                        constraint_proxy(offset) =
                            (expression <= constraint.rhs);
                        constraint_proxy(offset).set_name(name + "_lower");

                        constraint_proxy(offset + 1) =
                            (expression >= constraint.rhs);
                        constraint_proxy(offset + 1).set_name(name + "_upper");

                    } else {
                        constraint_proxy(offset) =
                            (expression == constraint.rhs);
                        constraint_proxy(offset).set_name(name);
                    }
                    break;
                }

                case MPSConstraintSense::Upper: {
                    constraint_proxy(offset) = (expression >= constraint.rhs);
                    constraint_proxy(offset).set_name(name);
                    break;
                }
            }
        }

        /**
         * Set up the objective function.
         */
        auto objective = model::IPExpression::create_instance();
        std::unordered_map<model::IPVariable *, double> objective_sensitivities;
        for (const auto &sensitivity : mps.objective.sensitivities) {
            std::string variable_name = sensitivity.first;
            double      coefficient   = sensitivity.second;
            objective_sensitivities[variable_ptrs[variable_name]] = coefficient;
        }
        objective.set_sensitivities(objective_sensitivities);
        m_model.minimize(objective);

        return m_model;
    }
};

/*****************************************************************************/
std::unordered_map<std::string, int> read_solution(
    const std::string &a_FILE_NAME) {
    std::unordered_map<std::string, int> solution;

    std::vector<std::string> lines;
    std::string              item;

    /**
     * Read and store entire part of the solution file.
     */
    {
        std::ifstream ifs;
        std::string   buffer;

        ifs.open(a_FILE_NAME.c_str());
        if (ifs.fail()) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "Cannot open the specified solution file: " + a_FILE_NAME));
        }
        while (std::getline(ifs, buffer)) {
            lines.push_back(buffer);
        }
        ifs.close();
    }

    /**
     * Parse the solution file.
     */
    for (const auto &line : lines) {
        std::stringstream        stream(line);
        std::vector<std::string> items;
        while (stream >> item) {
            items.push_back(item);
        }
        int items_size = items.size();

        if (items_size != 2) {
            continue;
        }
        solution[items[0]] =
            static_cast<int>(floor(0.5 + atof(items[1].c_str())));
    }

    return solution;
}
}  // namespace utility
}  // namespace printemps
#endif
/*****************************************************************************/
// END
/*****************************************************************************/