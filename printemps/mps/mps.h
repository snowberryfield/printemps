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
#include "mps_bound_sense.h"

namespace printemps::mps {
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
    MPS(const std::string &a_FILE_NAME) {
        this->initialize();
        this->read_mps(a_FILE_NAME);
    }

    /*************************************************************************/
    inline void initialize(void) {
        this->name.clear();
        this->variables.clear();
        this->constraints.clear();
        this->objective.initialize();
        this->variable_names.clear();
        this->constraint_names.clear();
    }

    /*************************************************************************/
    inline double parse_value(const std::string_view &a_VALUE_SV) {
        switch (a_VALUE_SV.size()) {
            case 1:
                if (a_VALUE_SV[0] == '1') {
                    return 1.0;
                }
                break;
            case 2:
                if (a_VALUE_SV[0] == '-' && a_VALUE_SV[1] == '1') {
                    return -1.0;
                }
                break;
            default:
                break;
        }

        double value  = 0.0;
        auto   result = std::from_chars(
            a_VALUE_SV.data(), a_VALUE_SV.data() + a_VALUE_SV.size(), value);

        if (result.ec != std::errc()) [[unlikely]] {
            throw std::runtime_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The MPS file has something wrong in a numeric value."));
        }

        return value;
    }

    /*************************************************************************/
    inline void parse_name(const std::vector<std::string_view> &a_ITEMS) {
        const std::size_t ITEMS_SIZE = a_ITEMS.size();

        if (ITEMS_SIZE > 2) {
            throw std::runtime_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The MPS file has something wrong in NAME section."));
        }

        if (ITEMS_SIZE == 1 &&
            utility::to_uppercase(a_ITEMS.front()) == "NAME") {
            this->name = "";
        } else {
            this->name = std::string(a_ITEMS.back());
        }
        return;
    }

    /*************************************************************************/
    inline void parse_objsense(const std::vector<std::string_view> &a_ITEMS) {
        const std::size_t ITEMS_SIZE = a_ITEMS.size();

        if (ITEMS_SIZE > 2) {
            throw std::runtime_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The MPS file has something wrong in OBJSENSE section."));
        }

        if (ITEMS_SIZE == 1 &&
            utility::to_uppercase(a_ITEMS.front()) == "OBJSENSE") {
            this->objective.is_minimization = true;
        } else {
            const auto &TOKEN = utility::to_uppercase(a_ITEMS.back());
            if (TOKEN == "MINIMIZATION" || TOKEN == "MINIMIZE" ||
                TOKEN == "MIN") {
                this->objective.is_minimization = true;
            } else if (TOKEN == "MAXIMIZATION" || TOKEN == "MAXIMIZE" ||
                       TOKEN == "MAX") {
                this->objective.is_minimization = false;
            } else {
                throw std::runtime_error(utility::format_error_location(
                    __FILE__, __LINE__, __func__,
                    "The MPS file has something wrong in OBJSENSE section."));
            }
        }
        return;
    }

    /*************************************************************************/
    inline void parse_objname(const std::vector<std::string_view> &a_ITEMS) {
        const std::size_t ITEMS_SIZE = a_ITEMS.size();

        if (ITEMS_SIZE > 2) {
            throw std::runtime_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The MPS file has something wrong in OBJNAME section."));
        }

        if (ITEMS_SIZE == 1 &&
            utility::to_uppercase(a_ITEMS.front()) == "OBJNAME") {
            this->objective.name = "";
        } else {
            this->objective.name = std::string(a_ITEMS.back());
        }
        return;
    }

    /*************************************************************************/
    inline void parse_rows(const std::vector<std::string_view> &a_ITEMS) {
        const std::size_t ITEMS_SIZE = a_ITEMS.size();

        if (ITEMS_SIZE != 2) {
            throw std::runtime_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The MPS file has something wrong in ROWS section."));
        }

        const auto       &SENSE_SV = a_ITEMS[0];
        const std::string ROW_NAME(a_ITEMS[1]);

        if (SENSE_SV[0] == 'N' && (this->objective.name.empty() ||
                                   ROW_NAME == this->objective.name)) {
            this->objective.name = ROW_NAME;
            return;
        }

        MPSConstraintSense sense;
        switch (SENSE_SV[0]) {
            case 'L':
                sense = MPSConstraintSense::Less;
                break;
            case 'E':
                sense = MPSConstraintSense::Equal;
                break;
            case 'G':
                sense = MPSConstraintSense::Greater;
                break;
            default:
                throw std::runtime_error(utility::format_error_location(
                    __FILE__, __LINE__, __func__,
                    "The MPS file has something wrong in ROWS section."));
        }

        auto &constraint = this->constraints[ROW_NAME];
        constraint.sense = sense;
        constraint.name  = ROW_NAME;
        this->constraint_names.emplace_back(ROW_NAME);
    }

    /*************************************************************************/
    inline void parse_columns(const std::vector<std::string_view> &a_ITEMS,
                              MPSVariableSense *a_variable_sense_ptr) {
        const std::size_t ITEMS_SIZE = a_ITEMS.size();

        if (ITEMS_SIZE < 3 || (ITEMS_SIZE & 1) == 0) {
            throw std::runtime_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The MPS file has something wrong in COLUMNS section."));
        }

        if (a_ITEMS[2].size() > 0 && a_ITEMS[2].front() == '\'') {
            if (a_ITEMS[2] == "'INTORG'") {
                *a_variable_sense_ptr = MPSVariableSense::Integer;
                return;
            } else if (a_ITEMS[2] == "'INTEND'") {
                *a_variable_sense_ptr = MPSVariableSense::Continuous;
                return;
            }
        }

        const std::string COLUMN_NAME(a_ITEMS[0]);
        const int         WIDTH = static_cast<int>((ITEMS_SIZE - 1) / 2);

        for (auto i = 0; i < WIDTH; ++i) {
            const auto k = 2 * i;

            const std::string EXPRESSION_NAME(a_ITEMS[k + 1]);
            const auto       &SENSITIVITY_SV = a_ITEMS[k + 2];

            const double SENSITIVITY = this->parse_value(SENSITIVITY_SV);
            if (std::fabs(SENSITIVITY) < constant::EPSILON_10) {
                continue;
            }

            const auto &OBJ_NAME = this->objective.name;
            if (!this->objective.name.empty() && EXPRESSION_NAME == OBJ_NAME) {
                this->objective.sensitivities[COLUMN_NAME] = SENSITIVITY;
            } else {
                auto it = this->constraints.find(EXPRESSION_NAME);
                if (it != this->constraints.end()) {
                    it->second.sensitivities[COLUMN_NAME] = SENSITIVITY;
                } else {
                    throw std::runtime_error(utility::format_error_location(
                        __FILE__, __LINE__, __func__,
                        "An undefined constraint or objective function "
                        "name is specified in COLUMNS section: " +
                            EXPRESSION_NAME));
                }
            }
        }

        auto [it, inserted] = this->variables.try_emplace(COLUMN_NAME);
        if (inserted) {
            auto &variable = it->second;
            variable.sense = *a_variable_sense_ptr;
            variable.name  = it->first;
            this->variable_names.emplace_back(it->first);
        }
    }

    /*************************************************************************/
    inline void parse_rhs(const std::vector<std::string_view> &a_ITEMS) {
        const std::size_t ITEMS_SIZE = a_ITEMS.size();

        if (ITEMS_SIZE < 3 || (ITEMS_SIZE & 1) == 0) {
            throw std::runtime_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The MPS file has something wrong in RHS section."));
        }

        const int WIDTH = static_cast<int>((ITEMS_SIZE - 1) / 2);
        for (auto i = 0; i < WIDTH; i++) {
            const auto k = 2 * i;

            const std::string ROW_NAME(a_ITEMS[k + 1]);
            const auto       &RHS_SV = a_ITEMS[k + 2];

            this->constraints[ROW_NAME].rhs = this->parse_value(RHS_SV);
        }
    }

    /*************************************************************************/
    inline void parse_range(const std::vector<std::string_view> &a_ITEMS) {
        const std::size_t ITEMS_SIZE = a_ITEMS.size();

        if (ITEMS_SIZE < 3 || (ITEMS_SIZE & 1) == 0) {
            throw std::runtime_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The MPS file has something wrong in RANGES section."));
        }

        std::string name;
        std::string name_new;
        double      range = 0.0;

        constexpr auto Less    = MPSConstraintSense::Less;
        constexpr auto Equal   = MPSConstraintSense::Equal;
        constexpr auto Greater = MPSConstraintSense::Greater;

        const int WIDTH = static_cast<int>((ITEMS_SIZE - 1) / 2);
        for (auto i = 0; i < WIDTH; i++) {
            const auto k = 2 * i;

            name     = std::string(a_ITEMS[k + 1]);
            name_new = name + "_range";

            const auto &RANGE_SV = a_ITEMS[k + 2];
            range                = this->parse_value(RANGE_SV);

            auto &constraint     = this->constraints[name];
            auto &constraint_new = this->constraints[name_new];

            constraint_new.name          = name_new;
            constraint_new.sensitivities = constraint.sensitivities;
            const double FABS_RANGE      = std::fabs(range);

            switch (constraint.sense) {
                case Less: {
                    constraint_new.sense = Greater;
                    constraint_new.rhs   = constraint.rhs - FABS_RANGE;
                    break;
                }
                case Greater: {
                    constraint_new.sense = Less;
                    constraint_new.rhs   = constraint.rhs + FABS_RANGE;
                    break;
                }
                case Equal: {
                    if (range > 0) {
                        constraint.sense     = Greater;
                        constraint_new.sense = Less;
                        constraint_new.rhs   = constraint.rhs + FABS_RANGE;
                    } else {
                        constraint.sense     = Less;
                        constraint_new.sense = Greater;
                        constraint_new.rhs   = constraint.rhs - FABS_RANGE;
                    }
                    break;
                }
                default: {
                    throw std::runtime_error(utility::format_error_location(
                        __FILE__, __LINE__, __func__,
                        "The MPS file has something wrong in RANGES section."));
                }
            }

            this->constraint_names.emplace_back(name_new);
        }
    }

    /*************************************************************************/
    inline void parse_bounds(const std::vector<std::string_view> &a_ITEMS) {
        const std::size_t ITEMS_SIZE = a_ITEMS.size();

        if (ITEMS_SIZE < 3) {
            throw std::runtime_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The MPS file has something wrong in BOUNDS section."));
        }

        const std::string COLUMN_NAME(a_ITEMS[2]);
        const auto       &BOUND_SENSE_SV = a_ITEMS.front();

        auto [it, inserted] = this->variables.try_emplace(COLUMN_NAME);

        if (inserted) {
            auto &variable = it->second;
            variable.sense = MPSVariableSense::Continuous;
            variable.name  = it->first;
            this->variable_names.emplace_back(it->first);
        }

        const auto BOUND_SENSE = bound_sense_map(BOUND_SENSE_SV);
        auto      &variable    = this->variables[COLUMN_NAME];

        if (a_ITEMS.size() == 3) {
            switch (BOUND_SENSE) {
                case MPSBoundSense::FR:
                    variable.is_bound_defined       = true;
                    variable.integer_lower_bound    = constant::INT_HALF_MIN;
                    variable.integer_upper_bound    = constant::INT_HALF_MAX;
                    variable.continuous_lower_bound = -HUGE_VAL;
                    variable.continuous_upper_bound = HUGE_VAL;
                    break;

                case MPSBoundSense::BV:
                    variable.sense                  = MPSVariableSense::Integer;
                    variable.is_bound_defined       = true;
                    variable.integer_lower_bound    = 0;
                    variable.integer_upper_bound    = 1;
                    variable.continuous_lower_bound = 0.0;
                    variable.continuous_upper_bound = 1;
                    break;

                case MPSBoundSense::MI:
                    variable.is_bound_defined       = true;
                    variable.integer_lower_bound    = constant::INT_HALF_MIN;
                    variable.integer_upper_bound    = 0;
                    variable.continuous_lower_bound = -HUGE_VAL;
                    variable.continuous_upper_bound = 0.0;
                    break;

                case MPSBoundSense::PL:
                    variable.is_bound_defined       = true;
                    variable.integer_lower_bound    = 0;
                    variable.integer_upper_bound    = constant::INT_HALF_MAX;
                    variable.continuous_lower_bound = 0.0;
                    variable.continuous_upper_bound = HUGE_VAL;
                    break;

                default:
                    throw std::runtime_error(utility::format_error_location(
                        __FILE__, __LINE__, __func__,
                        "The MPS file has something wrong in BOUNDS section."));
            }
        } else if (a_ITEMS.size() == 4) {
            const auto  &VALUE_SV         = a_ITEMS[3];
            const double CONTINUOUS_VALUE = this->parse_value(VALUE_SV);
            const int    INTEGER_VALUE    = static_cast<int>(CONTINUOUS_VALUE);

            switch (BOUND_SENSE) {
                case MPSBoundSense::LO:
                    variable.is_bound_defined       = true;
                    variable.integer_lower_bound    = INTEGER_VALUE;
                    variable.continuous_lower_bound = CONTINUOUS_VALUE;
                    break;

                case MPSBoundSense::LI:
                    variable.sense                  = MPSVariableSense::Integer;
                    variable.is_bound_defined       = true;
                    variable.integer_lower_bound    = INTEGER_VALUE;
                    variable.continuous_lower_bound = CONTINUOUS_VALUE;
                    break;

                case MPSBoundSense::UP:
                    variable.is_bound_defined       = true;
                    variable.integer_upper_bound    = INTEGER_VALUE;
                    variable.continuous_upper_bound = CONTINUOUS_VALUE;
                    break;

                case MPSBoundSense::UI:
                    variable.sense                  = MPSVariableSense::Integer;
                    variable.is_bound_defined       = true;
                    variable.integer_upper_bound    = INTEGER_VALUE;
                    variable.continuous_upper_bound = CONTINUOUS_VALUE;
                    break;

                case MPSBoundSense::FX:
                    variable.is_bound_defined       = true;
                    variable.is_fixed               = true;
                    variable.integer_fixed_value    = INTEGER_VALUE;
                    variable.continuous_fixed_value = CONTINUOUS_VALUE;
                    variable.integer_lower_bound    = INTEGER_VALUE;
                    variable.integer_upper_bound    = INTEGER_VALUE;
                    variable.continuous_lower_bound = CONTINUOUS_VALUE;
                    variable.continuous_upper_bound = CONTINUOUS_VALUE;
                    break;

                default:
                    throw std::runtime_error(utility::format_error_location(
                        __FILE__, __LINE__, __func__,
                        "The MPS file has something wrong in BOUNDS section."));
            }
        } else {
            throw std::runtime_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The MPS file has something wrong in BOUNDS section."));
        }
    }

    /*************************************************************************/
    inline void read_mps(const std::string &a_FILE_NAME) {
        std::ifstream ifs(a_FILE_NAME, std::ios::binary | std::ios::in);
        if (!ifs) {
            throw std::runtime_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "Cannot open the specified MPS file: " + a_FILE_NAME));
        }

        ifs.seekg(0, std::ios::end);
        std::streamsize file_size = ifs.tellg();
        ifs.seekg(0, std::ios::beg);

        std::string buffer;
        buffer.resize(static_cast<size_t>(file_size));
        if (!ifs.read(buffer.data(), file_size)) {
            throw std::runtime_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "Failed to read the MPS file into buffer."));
        }
        ifs.close();

        std::string_view content(buffer);

        const std::size_t estimated_lines = static_cast<size_t>(
            std::count(content.begin(), content.end(), '\n') + 1);
        this->constraints.reserve(estimated_lines / 10 + 16);
        this->variables.reserve(estimated_lines / 10 + 16);
        this->variable_names.reserve(estimated_lines / 10 + 16);
        this->constraint_names.reserve(estimated_lines / 10 + 16);

        MPSReadMode      read_mode      = MPSReadMode::Initial;
        MPSVariableSense variable_sense = MPSVariableSense::Continuous;

        bool is_valid         = false;
        bool is_read_name     = false;
        bool is_read_objsense = false;
        bool is_read_objname  = false;

        std::size_t       pos  = 0;
        const std::size_t SIZE = content.size();

        std::vector<std::string_view> items;
        items.reserve(8);

        while (pos < SIZE) {
            std::size_t line_end = pos;
            while (line_end < SIZE && content[line_end] != '\n' &&
                   content[line_end] != '\r') {
                line_end++;
            }

            const std::size_t LINE_LENGTH = line_end - pos;
            std::string_view  line_view   = content.substr(pos, LINE_LENGTH);

            pos = line_end;
            while (pos < SIZE &&
                   (content[pos] == '\n' || content[pos] == '\r')) {
                pos++;
            }

            utility::split_items(line_view, &items);

            if (items.empty()) {
                continue;
            }

            if (!items.front().empty() && items.front().front() == '*') {
                continue;
            }

            auto first = items.front();

            if (!is_read_name && utility::to_uppercase(first) == "NAME") {
                read_mode = MPSReadMode::Name;
                if (items.size() == 1) {
                    continue;
                }
            }

            if (!is_read_objsense &&
                utility::to_uppercase(first) == "OBJSENSE") {
                read_mode = MPSReadMode::Objsense;
                if (items.size() == 1) {
                    continue;
                }
            }

            if (!is_read_objname && utility::to_uppercase(first) == "OBJNAME") {
                read_mode = MPSReadMode::Objname;
                if (items.size() == 1) {
                    continue;
                }
            }

            if (items.size() == 1) {
                const std::string TOKEN = utility::to_uppercase(first);
                auto              it    = MPSReadModeMap.find(TOKEN);
                if (it != MPSReadModeMap.end()) {
                    if (it->second == MPSReadMode::Endata) {
                        is_valid = true;
                        break;
                    }
                    read_mode = it->second;
                    continue;
                }
            }

            switch (read_mode) {
                case MPSReadMode::Name: {
                    this->parse_name(items);
                    is_read_name = true;
                    break;
                }
                case MPSReadMode::Objsense: {
                    this->parse_objsense(items);
                    is_read_objsense = true;
                    break;
                }
                case MPSReadMode::Objname: {
                    this->parse_objname(items);
                    is_read_objname = true;
                    break;
                }
                case MPSReadMode::Rows: {
                    this->parse_rows(items);
                    break;
                }
                case MPSReadMode::Columns: {
                    this->parse_columns(items, &variable_sense);
                    break;
                }
                case MPSReadMode::Rhs: {
                    this->parse_rhs(items);
                    break;
                }
                case MPSReadMode::Ranges: {
                    this->parse_range(items);
                    break;
                }
                case MPSReadMode::Bounds: {
                    this->parse_bounds(items);
                    break;
                }
                default: {
                    break;
                }
            }
        }

        if (!is_valid) {
            throw std::runtime_error(utility::format_error_location(
                __FILE__, __LINE__, __func__, "The MPS file is not valid."));
        }

        // default bounds for integer variables
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
