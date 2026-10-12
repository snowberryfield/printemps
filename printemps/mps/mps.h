/*****************************************************************************/
// Copyright (c) 2020-2026 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_MPS_MPS_H__
#define PRINTEMPS_MPS_MPS_H__

#include "mps_variable.h"
#include "mps_constraint.h"
#include "mps_objective.h"
#include "mps_read_mode.h"
#include "mps_bound_type.h"

namespace printemps::mps {
/*****************************************************************************/
struct MPS {
    std::string                name;
    std::vector<MPSVariable>   variables;
    std::vector<MPSConstraint> constraints;
    MPSObjective               objective;

    std::vector<std::string>                  variable_names;
    std::vector<std::string>                  constraint_names;
    std::unordered_map<std::string_view, int> variable_name_to_index_map;
    std::unordered_map<std::string_view, int> constraint_name_to_index_map;

    std::string_view last_column_name;
    int              last_column_id;
    std::string      buffer;

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
        this->variable_name_to_index_map.clear();
        this->constraint_name_to_index_map.clear();
        this->last_column_name = "";
        this->last_column_id   = -1;
        this->buffer.clear();
    }

    /*************************************************************************/
    inline MPSVariable &variable(const std::string_view &a_NAME) {
        return this->variables[this->variable_name_to_index_map.at(a_NAME)];
    }

    /*************************************************************************/
    inline const MPSVariable &variable(const std::string_view &a_NAME) const {
        return this->variables[this->variable_name_to_index_map.at(a_NAME)];
    }

    /*************************************************************************/
    inline MPSConstraint &constraint(const std::string_view &a_NAME) {
        return this->constraints[this->constraint_name_to_index_map.at(a_NAME)];
    }

    /*************************************************************************/
    inline const MPSConstraint &constraint(
        const std::string_view &a_NAME) const {
        return this->constraints[this->constraint_name_to_index_map.at(a_NAME)];
    }

    /*************************************************************************/
    inline double parse_value(const std::string_view &a_VALUE_SV) {
        switch (a_VALUE_SV.size()) {
            case 1: {
                if (a_VALUE_SV[0] == '1') {
                    return 1.0;
                } else if (a_VALUE_SV[0] == '0') {
                    return 0.0;
                }
                break;
            }
            case 2: {
                if (a_VALUE_SV[0] == '-' && a_VALUE_SV[1] == '1') {
                    return -1.0;
                }
                break;
            }
            default: {
                break;
            }
        }

        char       *end   = nullptr;
        const char *begin = a_VALUE_SV.data();

        double value = std::strtod(begin, &end);

        if (end == begin + a_VALUE_SV.size()) {
            return value;
        }

        throw std::runtime_error(utility::format_error_location(
            __FILE__, __LINE__, __func__,
            "The MPS file has something wrong in a numeric value."));
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

        if (ITEMS_SIZE < 2) {
            throw std::runtime_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The MPS file has something wrong in ROWS section."));
        }

        const auto &SENSE_SV    = a_ITEMS[0];
        const auto &ROW_NAME_SV = a_ITEMS[1];

        const std::string ROW_NAME(ROW_NAME_SV);

        if (SENSE_SV[0] == 'N' && (this->objective.name.empty() ||
                                   ROW_NAME == this->objective.name)) {
            this->objective.name = ROW_NAME;
            return;
        }

        MPSConstraintSense sense;
        switch (SENSE_SV[0]) {
            case 'L': {
                sense = MPSConstraintSense::Less;
                break;
            }
            case 'E': {
                sense = MPSConstraintSense::Equal;
                break;
            }
            case 'G': {
                sense = MPSConstraintSense::Greater;
                break;
            }
            default: {
                throw std::runtime_error(utility::format_error_location(
                    __FILE__, __LINE__, __func__,
                    "The MPS file has something wrong in ROWS section."));
            }
        }

        const int ROW_ID = static_cast<int>(this->constraints.size());
        this->constraints.emplace_back();
        auto &constraint = this->constraints.back();
        constraint.sense = sense;
        constraint.name  = ROW_NAME;
        this->constraint_names.emplace_back(ROW_NAME);
        this->constraint_name_to_index_map[ROW_NAME_SV] = ROW_ID;
    }

    /*************************************************************************/
    inline void parse_columns(const std::vector<std::string_view> &a_ITEMS,
                              MPSVariableType *a_variable_type_ptr) {
        const std::size_t ITEMS_SIZE = a_ITEMS.size();

        if (ITEMS_SIZE < 3 || (ITEMS_SIZE & 1) == 0) {
            throw std::runtime_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The MPS file has something wrong in COLUMNS section."));
        }

        if (a_ITEMS[2].size() > 0 && a_ITEMS[2].front() == '\'') {
            if (a_ITEMS[2] == "'INTORG'") {
                *a_variable_type_ptr = MPSVariableType::Integer;
                return;
            } else if (a_ITEMS[2] == "'INTEND'") {
                *a_variable_type_ptr = MPSVariableType::Continuous;
                return;
            }
        }

        const auto &COLUMN_NAME_SV = a_ITEMS[0];
        int         column_id      = -1;

        if (this->last_column_name == COLUMN_NAME_SV) {
            column_id = this->last_column_id;
        } else {
            auto it = this->variable_name_to_index_map.find(COLUMN_NAME_SV);
            if (it != this->variable_name_to_index_map.end()) {
                column_id = it->second;
            } else {
                column_id = static_cast<int>(this->variables.size());
                this->variables.emplace_back();
                auto &variable = this->variables.back();
                variable.type  = *a_variable_type_ptr;
                variable.name  = std::string(COLUMN_NAME_SV);
                this->variable_names.emplace_back(variable.name);
                this->variable_name_to_index_map[COLUMN_NAME_SV] = column_id;
            }
            this->last_column_name = COLUMN_NAME_SV;
            this->last_column_id   = column_id;
        }

        const int WIDTH = static_cast<int>((ITEMS_SIZE - 1) / 2);

        for (int i = 0; i < WIDTH; i++) {
            const int K = 2 * i;

            const auto &EXPRESSION_NAME_SV = a_ITEMS[K + 1];
            const auto &SENSITIVITY_SV     = a_ITEMS[K + 2];

            const double SENSITIVITY = this->parse_value(SENSITIVITY_SV);
            if (std::fabs(SENSITIVITY) < constant::EPSILON_10) {
                continue;
            }

            const auto &OBJ_NAME = this->objective.name;
            if (!OBJ_NAME.empty() && EXPRESSION_NAME_SV == OBJ_NAME) {
                this->objective.sensitivities.emplace_back(column_id,
                                                           SENSITIVITY);
            } else {
                auto it =
                    this->constraint_name_to_index_map.find(EXPRESSION_NAME_SV);
                if (it != this->constraint_name_to_index_map.end()) {
                    this->constraints[it->second].sensitivities.emplace_back(
                        column_id, SENSITIVITY);
                } else {
                    throw std::runtime_error(utility::format_error_location(
                        __FILE__, __LINE__, __func__,
                        "An undefined constraint or objective function "
                        "name is specified in COLUMNS section: " +
                            std::string(EXPRESSION_NAME_SV)));
                }
            }
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
        for (int i = 0; i < WIDTH; i++) {
            const int K = 2 * i;

            const auto &ROW_NAME_SV = a_ITEMS[K + 1];
            const auto &RHS_SV      = a_ITEMS[K + 2];

            auto it = this->constraint_name_to_index_map.find(ROW_NAME_SV);
            if (it != this->constraint_name_to_index_map.end()) {
                this->constraints[it->second].rhs = this->parse_value(RHS_SV);
            }
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

        const int WIDTH = static_cast<int>((ITEMS_SIZE - 1) / 2);
        for (int i = 0; i < WIDTH; i++) {
            const int K = 2 * i;

            const auto &ROW_NAME_SV = a_ITEMS[K + 1];
            name                    = std::string(ROW_NAME_SV);
            name_new                = name + "_range";

            const auto &RANGE_SV = a_ITEMS[K + 2];
            range                = this->parse_value(RANGE_SV);

            auto it = this->constraint_name_to_index_map.find(ROW_NAME_SV);
            if (it == this->constraint_name_to_index_map.end()) {
                continue;
            }

            const int CONSTRAINT_ID = it->second;
            const int NEW_CONSTRAINT_ID =
                static_cast<int>(this->constraints.size());
            this->constraints.emplace_back();
            auto &constraint     = this->constraints[CONSTRAINT_ID];
            auto &constraint_new = this->constraints.back();

            constraint_new.name          = name_new;
            constraint_new.sensitivities = constraint.sensitivities;
            const double FABS_RANGE      = std::fabs(range);

            switch (constraint.sense) {
                case MPSConstraintSense::Less: {
                    constraint_new.sense = MPSConstraintSense::Greater;
                    constraint_new.rhs   = constraint.rhs - FABS_RANGE;
                    break;
                }
                case MPSConstraintSense::Greater: {
                    constraint_new.sense = MPSConstraintSense::Less;
                    constraint_new.rhs   = constraint.rhs + FABS_RANGE;
                    break;
                }
                case MPSConstraintSense::Equal: {
                    if (range > 0) {
                        constraint.sense     = MPSConstraintSense::Greater;
                        constraint_new.sense = MPSConstraintSense::Less;
                        constraint_new.rhs   = constraint.rhs + FABS_RANGE;
                    } else {
                        constraint.sense     = MPSConstraintSense::Less;
                        constraint_new.sense = MPSConstraintSense::Greater;
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
            this->constraint_name_to_index_map[this->constraint_names.back()] =
                NEW_CONSTRAINT_ID;
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

        const auto &COLUMN_NAME_SV = a_ITEMS[2];
        const auto &BOUND_TYPE_SV  = a_ITEMS.front();

        int  column_id = -1;
        auto it        = this->variable_name_to_index_map.find(COLUMN_NAME_SV);
        if (it != this->variable_name_to_index_map.end()) {
            column_id = it->second;
        } else {
            column_id = static_cast<int>(this->variables.size());
            this->variables.emplace_back();
            auto &variable = this->variables.back();
            variable.type  = MPSVariableType::Continuous;
            variable.name  = std::string(COLUMN_NAME_SV);
            this->variable_names.emplace_back(variable.name);
            this->variable_name_to_index_map[COLUMN_NAME_SV] = column_id;
        }

        const auto BOUND_TYPE = bound_type_map(BOUND_TYPE_SV);
        auto      &variable   = this->variables[column_id];

        if (a_ITEMS.size() == 3) {
            switch (BOUND_TYPE) {
                case MPSBoundType::FR:
                    variable.is_bound_defined       = true;
                    variable.integer_lower_bound    = constant::INT_HALF_MIN;
                    variable.integer_upper_bound    = constant::INT_HALF_MAX;
                    variable.continuous_lower_bound = -HUGE_VAL;
                    variable.continuous_upper_bound = HUGE_VAL;
                    break;

                case MPSBoundType::BV:
                    variable.type                   = MPSVariableType::Integer;
                    variable.is_bound_defined       = true;
                    variable.integer_lower_bound    = 0;
                    variable.integer_upper_bound    = 1;
                    variable.continuous_lower_bound = 0.0;
                    variable.continuous_upper_bound = 1;
                    break;

                case MPSBoundType::MI:
                    variable.is_bound_defined       = true;
                    variable.integer_lower_bound    = constant::INT_HALF_MIN;
                    variable.integer_upper_bound    = 0;
                    variable.continuous_lower_bound = -HUGE_VAL;
                    variable.continuous_upper_bound = 0.0;
                    break;

                case MPSBoundType::PL:
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

            switch (BOUND_TYPE) {
                case MPSBoundType::LO:
                    variable.is_bound_defined       = true;
                    variable.integer_lower_bound    = INTEGER_VALUE;
                    variable.continuous_lower_bound = CONTINUOUS_VALUE;
                    break;

                case MPSBoundType::LI:
                    variable.type                   = MPSVariableType::Integer;
                    variable.is_bound_defined       = true;
                    variable.integer_lower_bound    = INTEGER_VALUE;
                    variable.continuous_lower_bound = CONTINUOUS_VALUE;
                    break;

                case MPSBoundType::UP:
                    variable.is_bound_defined       = true;
                    variable.integer_upper_bound    = INTEGER_VALUE;
                    variable.continuous_upper_bound = CONTINUOUS_VALUE;
                    break;

                case MPSBoundType::UI:
                    variable.type                   = MPSVariableType::Integer;
                    variable.is_bound_defined       = true;
                    variable.integer_upper_bound    = INTEGER_VALUE;
                    variable.continuous_upper_bound = CONTINUOUS_VALUE;
                    break;

                case MPSBoundType::FX:
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

        this->buffer.resize(static_cast<size_t>(file_size));
        if (!ifs.read(this->buffer.data(), file_size)) {
            throw std::runtime_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "Failed to read the MPS file into buffer."));
        }
        ifs.close();

        std::string_view content(this->buffer);

        const std::size_t ESTIMATED_LINES = static_cast<size_t>(
            std::count(content.begin(), content.end(), '\n') + 1);
        const std::size_t RESERVE_SIZE = ESTIMATED_LINES / 10 + 16;
        this->constraints.reserve(RESERVE_SIZE);
        this->variables.reserve(RESERVE_SIZE);
        this->variable_names.reserve(RESERVE_SIZE);
        this->constraint_names.reserve(RESERVE_SIZE);
        this->variable_name_to_index_map.reserve(RESERVE_SIZE);
        this->constraint_name_to_index_map.reserve(RESERVE_SIZE);

        this->last_column_name = "";
        this->last_column_id   = -1;

        MPSReadMode     read_mode     = MPSReadMode::Initial;
        MPSVariableType variable_type = MPSVariableType::Continuous;

        bool is_valid         = false;
        bool is_read_name     = false;
        bool is_read_objsense = false;
        bool is_read_objname  = false;

        std::size_t       position = 0;
        const std::size_t SIZE     = content.size();

        std::vector<std::string_view> items;
        items.reserve(8);

        while (position < SIZE) {
            std::size_t line_end = position;
            while (line_end < SIZE && content[line_end] != '\n' &&
                   content[line_end] != '\r') {
                line_end++;
            }

            const std::size_t LINE_LENGTH = line_end - position;
            std::string_view  line_view = content.substr(position, LINE_LENGTH);

            position = line_end;
            while (position < SIZE &&
                   (content[position] == '\n' || content[position] == '\r')) {
                position++;
            }

            if (line_view.empty()) {
                continue;
            }

            const char FIRST_CHAR = line_view[0];

            if (FIRST_CHAR != ' ' && FIRST_CHAR != '\t') {
                if (FIRST_CHAR == '*') {
                    continue;
                }

                utility::split_items(line_view, &items);

                if (items.empty()) {
                    continue;
                }

                auto first = items.front();

                if (!is_read_name && (first == "NAME" || first == "name" ||
                                      utility::to_uppercase(first) == "NAME")) {
                    read_mode = MPSReadMode::Name;
                    if (items.size() == 1) {
                        continue;
                    }
                }

                if (!is_read_objsense &&
                    (first == "OBJSENSE" || first == "objsense" ||
                     utility::to_uppercase(first) == "OBJSENSE")) {
                    read_mode = MPSReadMode::Objsense;
                    if (items.size() == 1) {
                        continue;
                    }
                }

                if (!is_read_objname &&
                    (first == "OBJNAME" || first == "objname" ||
                     utility::to_uppercase(first) == "OBJNAME")) {
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
            } else {
                utility::split_items(line_view, &items);

                if (items.empty()) {
                    continue;
                }

                if (!items.front().empty() && items.front().front() == '*') {
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
                    this->parse_columns(items, &variable_type);
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
            if (!variable.is_bound_defined &&
                variable.type == MPSVariableType::Integer) {
                variable.is_bounded          = true;
                variable.integer_lower_bound = 0;
                variable.integer_upper_bound = 1;
            }
        }
    }
};
}  // namespace printemps::mps
#endif
/*****************************************************************************/
// END
/*****************************************************************************/
