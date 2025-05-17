/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_OPB_OPB_H__
#define PRINTEMPS_OPB_OPB_H__

#include "opb_objective.h"
#include "opb_constraint.h"
#include "opb_metadata.h"
#include "opb_top_cost.h"

namespace printemps::opb {
/*****************************************************************************/
struct OPB {
    std::string                name;
    OPBObjective               objective;
    std::vector<OPBConstraint> soft_constraints;
    std::vector<OPBConstraint> hard_constraints;

    OPBMetadata metadata;
    OPBTopCost  top_cost;

    std::vector<std::string> variable_names;
    std::vector<std::string> negated_variable_names;
    std::vector<std::pair<std::string, std::vector<std::string>>>
        product_variable_names;

    /*************************************************************************/
    OPB(void) {
        this->initialize();
    }

    /*************************************************************************/
    OPB(const std::string &a_FILE_NAME) {
        this->initialize();
        this->read_opb(a_FILE_NAME);
    }

    /*************************************************************************/
    void initialize(void) {
        this->name = "";
        this->objective.initialize();
        this->soft_constraints.clear();
        this->hard_constraints.clear();

        this->metadata.initialize();
        this->top_cost.initialize();

        this->variable_names.clear();
        this->negated_variable_names.clear();
        this->product_variable_names.clear();
    }

    /*************************************************************************/
    inline void print(void) const {
        std::cout << "[metadata]" << std::endl;
        std::cout << " #variable: " << this->metadata.number_of_variables
                  << std::endl;
        std::cout << " #constraint: " << this->metadata.number_of_constraints
                  << std::endl;
        std::cout << " #equal: " << this->metadata.number_of_equals
                  << std::endl;
        std::cout << " intsize: " << this->metadata.intsize << std::endl;
        std::cout << " sizeproduct: " << this->metadata.sizeproduct
                  << std::endl;
        std::cout << " #soft: " << this->metadata.number_of_soft_constraints
                  << std::endl;
        std::cout << " mincost: " << this->metadata.mincost << std::endl;
        std::cout << " maxcost: " << this->metadata.maxcost << std::endl;
        std::cout << " sumcost: " << this->metadata.sumcost << std::endl;

        std::cout << "[objective]" << std::endl;
        for (const auto &term : this->objective.terms) {
            std::cout << " " << term.coefficient << " "
                      << term.concated_variable_name();
        }
        std::cout << std::endl;

        std::cout << "[soft constrtaints]" << std::endl;
        for (const auto &constraint : this->soft_constraints) {
            std::cout << constraint.name << "(weight: " << constraint.weight
                      << ")" << std::endl;
            for (const auto &term : constraint.terms) {
                std::cout << " " << term.coefficient << " "
                          << term.concated_variable_name();
            }
            switch (constraint.sense) {
                case OPBConstraintSense::Less: {
                    std::cout << " <= ";
                    break;
                }
                case OPBConstraintSense::Equal: {
                    std::cout << " <= ";
                    break;
                }
                case OPBConstraintSense::Greater: {
                    std::cout << " >= ";
                    break;
                }
            }
            std::cout << constraint.rhs << std::endl;
        }

        std::cout << "[hard constrtaints]" << std::endl;
        for (const auto &constraint : this->hard_constraints) {
            std::cout << constraint.name << "(weight: " << constraint.weight
                      << ")" << std::endl;
            for (const auto &term : constraint.terms) {
                std::cout << " " << term.coefficient << " "
                          << term.concated_variable_name();
            }
            switch (constraint.sense) {
                case OPBConstraintSense::Less: {
                    std::cout << " <= ";
                    break;
                }
                case OPBConstraintSense::Equal: {
                    std::cout << " = ";
                    break;
                }
                case OPBConstraintSense::Greater: {
                    std::cout << " >= ";
                    break;
                }
            }
            std::cout << constraint.rhs << std::endl;
        }

        std::cout << "[variable names]" << std::endl;
        for (const auto &variable_name : this->variable_names) {
            std::cout << " " << variable_name << std::endl;
        }

        std::cout << "[negated variable names]" << std::endl;
        for (const auto &variable_name : this->negated_variable_names) {
            std::cout << " " << variable_name << std::endl;
        }

        std::cout << "[product variable_names]" << std::endl;
        for (const auto &product_variable_name : this->product_variable_names) {
            std::cout << " " << product_variable_name.first << " = ";
            for (const auto &variable_name : product_variable_name.second) {
                std::cout << variable_name << " ";
            }
            std::cout << std::endl;
        }

        std::cout << "[top cost]" << std::endl;
        std::cout << " " << this->top_cost.is_defined << " "
                  << this->top_cost.value << std::endl;
    }

    /*************************************************************************/
    inline static OPBMetadata parse_metadata(const std::string &a_LINE) {
        OPBMetadata metadata;
        if (a_LINE.size() <= 1) {
            return metadata;
        }

        std::stringstream stream(a_LINE.substr(1));
        std::string       token;

        bool is_valid = true;
        while (stream >> token) {
            if (token == "#variable=") {
                if (stream >> token) {
                    metadata.number_of_variables = std::stoi(token);
                } else {
                    is_valid = false;
                }
            } else if (token == "#constraint=") {
                if (stream >> token) {
                    metadata.number_of_constraints = std::stoi(token);
                } else {
                    is_valid = false;
                }
            } else if (token == "#equal=") {
                if (stream >> token) {
                    metadata.number_of_equals = std::stoi(token);
                } else {
                    is_valid = false;
                }
            } else if (token == "#product=") {
                if (stream >> token) {
                    metadata.number_of_products = std::stoi(token);
                } else {
                    is_valid = false;
                }
            } else if (token == "intsize=") {
                if (stream >> token) {
                    metadata.intsize = std::stoi(token);
                } else {
                    is_valid = false;
                }
            } else if (token == "sizeproduct=") {
                if (stream >> token) {
                    metadata.sizeproduct = std::stoi(token);
                } else {
                    is_valid = false;
                }
            } else if (token == "#soft=") {
                if (stream >> token) {
                    metadata.number_of_soft_constraints = std::stoi(token);
                } else {
                    is_valid = false;
                }
            } else if (token == "mincost=") {
                if (stream >> token) {
                    metadata.mincost = std::stoi(token);
                } else {
                    is_valid = false;
                }
            } else if (token == "maxcost=") {
                if (stream >> token) {
                    metadata.maxcost = std::stoi(token);
                } else {
                    is_valid = false;
                }
            } else if (token == "sumcost=") {
                if (stream >> token) {
                    metadata.sumcost = std::stoi(token);
                } else {
                    is_valid = false;
                }
            }

            if (!is_valid) {
                throw std::runtime_error(utility::format_error_location(
                    __FILE__, __LINE__, __func__,
                    "The OPB file has something wrong in metadata."));
            }
        }
        return metadata;
    }

    /*************************************************************************/
    inline static OPBTopCost parse_top_cost(const std::string &a_LINE) {
        OPBTopCost top_cost;

        if (a_LINE.size() < 5 || (a_LINE.substr(0, 5) != "soft:" &&
                                  a_LINE.substr(0, 5) != "Soft:")) {
            throw std::runtime_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The OPB file has something wrong in top cost definition."));
        }

        const std::string        LINE_REST_PART = a_LINE.substr(5);
        std::stringstream        stream(LINE_REST_PART);
        std::string              token;
        std::vector<std::string> tokens;

        while (stream >> token) {
            tokens.push_back(token);
        }
        if (tokens.size() == 0) {
            top_cost.is_defined = false;
            top_cost.value      = std::numeric_limits<int>::max();
        } else if (tokens.size() == 1) {
            top_cost.is_defined = true;
            top_cost.value      = std::stoi(tokens[0]);
        } else {
            throw std::runtime_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The OPB file has something wrong in top cost definition."));
        }
        return top_cost;
    }

    /*************************************************************************/
    inline static OPBObjective parse_objective(const std::string &a_LINE) {
        OPBObjective objective;
        bool         is_valid = false;
        if (a_LINE.size() >= 4) {
            const auto SENSE = a_LINE.substr(0, 4);
            if (SENSE == "min:" || SENSE == "Min:") {
                is_valid                  = true;
                objective.is_minimization = true;
            } else if (SENSE == "max:" || SENSE == "Max:") {
                is_valid                  = true;
                objective.is_minimization = false;
            }
        }

        if (!is_valid) {
            throw std::runtime_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The OPB file has something wrong in objective function "
                "definition."));
        }

        objective.name  = "objective";
        objective.terms = OPB::parse_terms(a_LINE.substr(4));

        return objective;
    }

    /*************************************************************************/
    inline static OPBConstraint parse_soft_constraint(const std::string &a_LINE,
                                                      const int a_INDEX) {
        const size_t SQUARE_BRACKET_START = a_LINE.find('[');
        const size_t SQUARE_BRACKET_END   = a_LINE.find(']');

        double weight = 0.0;

        if (SQUARE_BRACKET_START != std::string::npos &&
            SQUARE_BRACKET_END != std::string::npos &&
            SQUARE_BRACKET_START < SQUARE_BRACKET_END) {
            weight = std::stoi(
                a_LINE.substr(SQUARE_BRACKET_START + 1,
                              SQUARE_BRACKET_END - SQUARE_BRACKET_START - 1));
        } else {
            throw std::runtime_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The OPB file has something wrong in soft constraint "
                "definition."));
        }

        const std::string LINE_REST_PART =
            a_LINE.substr(SQUARE_BRACKET_END + 1);
        OPBConstraint soft_constraint = OPB::parse_constraint(LINE_REST_PART);
        soft_constraint.weight        = weight;
        soft_constraint.name = "soft_constraint_" + std::to_string(a_INDEX);

        return soft_constraint;
    }

    /*************************************************************************/
    inline static OPBConstraint parse_hard_constraint(const std::string &a_LINE,
                                                      const int a_INDEX) {
        OPBConstraint hard_constraint = OPB::parse_constraint(a_LINE);
        hard_constraint.weight        = std::numeric_limits<int>::max();
        hard_constraint.name = "hard_constraint_" + std::to_string(a_INDEX);
        return hard_constraint;
    }

    /*************************************************************************/
    inline static OPBConstraint parse_constraint(
        const std::string &a_CONSTRAINT_STRING) {
        size_t             pos;
        std::string        op;
        OPBConstraintSense sense;

        if ((pos = a_CONSTRAINT_STRING.find("<=")) != std::string::npos) {
            op    = "<=";
            sense = OPBConstraintSense::Less;
        } else if ((pos = a_CONSTRAINT_STRING.find(">=")) !=
                   std::string::npos) {
            op    = ">=";
            sense = OPBConstraintSense::Greater;
        } else if ((pos = a_CONSTRAINT_STRING.find("=")) != std::string::npos) {
            op    = "=";
            sense = OPBConstraintSense::Equal;
        } else {
            throw std::runtime_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The OPB file has something wrong in constraint definition."));
        }

        const std::string LHS_STRING = a_CONSTRAINT_STRING.substr(0, pos);
        const std::string RHS_STRING =
            a_CONSTRAINT_STRING.substr(pos + op.size());
        OPBConstraint constraint;
        constraint.weight = std::numeric_limits<int>::max();
        constraint.sense  = sense;
        constraint.name   = "";
        constraint.terms  = OPB::parse_terms(LHS_STRING);
        constraint.rhs    = std::stoi(RHS_STRING);
        return constraint;
    }

    /*************************************************************************/
    inline static std::vector<OPBTerm> parse_terms(
        const std::string &a_TERM_STRING) {
        std::stringstream        stream(a_TERM_STRING);
        std::string              token;
        std::vector<std::string> tokens;

        while (stream >> token) {
            tokens.push_back(token);
        }

        OPBTerm              term;
        std::vector<OPBTerm> terms;

        bool   is_number_last_read  = false;
        double previous_coefficient = 0.0;
        double current_coefficient  = 0.0;

        std::unordered_set<std::string> variable_names_set;

        const int TERM_TOKENS_SIZE = tokens.size();
        for (auto i = 0; i < TERM_TOKENS_SIZE; i++) {
            if (std::isalpha(tokens[i].front()) || tokens[i].front() == '~') {
                if (i == 0) {
                    throw std::runtime_error(utility::format_error_location(
                        __FILE__, __LINE__, __func__,
                        "Expression must begin with coefficient value."));
                }
                variable_names_set.insert(tokens[i]);
                is_number_last_read = false;
            } else {
                if (is_number_last_read) {
                    throw std::runtime_error(utility::format_error_location(
                        __FILE__, __LINE__, __func__,
                        "Expression must not include two ore more consecutive "
                        "coefficients."));
                }
                previous_coefficient = current_coefficient;
                current_coefficient  = std::stoi(tokens[i]);
                is_number_last_read  = true;
            }

            if (variable_names_set.size() > 0 &&
                (is_number_last_read || i == TERM_TOKENS_SIZE - 1)) {
                std::vector<std::string> variable_names(
                    variable_names_set.begin(), variable_names_set.end());
                std::stable_sort(variable_names.begin(), variable_names.end());

                if (i == TERM_TOKENS_SIZE - 1) {
                    term.coefficient = current_coefficient;
                } else {
                    term.coefficient = previous_coefficient;
                }
                term.variable_names = variable_names;
                terms.push_back(term);

                term.initialize();
                variable_names_set.clear();
            }
        }
        return terms;
    }

    /*************************************************************************/
    inline void setup_variable_information(void) {
        std::unordered_set<std::string> variable_names_set;
        std::unordered_set<std::string> nagated_variable_names_set;
        std::unordered_map<std::string, std::vector<std::string>>
            product_variable_names_map;

        for (const auto &term : this->objective.terms) {
            for (const auto &variable_name : term.variable_names) {
                if (variable_name.front() == '~') {
                    const auto ORIGINAL_VARIABLE_NAME = variable_name.substr(1);
                    variable_names_set.insert(ORIGINAL_VARIABLE_NAME);
                    nagated_variable_names_set.insert(ORIGINAL_VARIABLE_NAME);
                } else {
                    variable_names_set.insert(variable_name);
                }
            }
            if (term.variable_names.size() > 1) {
                product_variable_names_map[term.concated_variable_name()] =
                    term.variable_names;
            }
        }

        for (const auto &constraint : this->soft_constraints) {
            for (const auto &term : constraint.terms) {
                for (const auto &variable_name : term.variable_names) {
                    if (variable_name.front() == '~') {
                        const auto ORIGINAL_VARIABLE_NAME =
                            variable_name.substr(1);
                        variable_names_set.insert(ORIGINAL_VARIABLE_NAME);
                        nagated_variable_names_set.insert(
                            ORIGINAL_VARIABLE_NAME);
                    } else {
                        variable_names_set.insert(variable_name);
                    }
                }
                if (term.variable_names.size() > 1) {
                    product_variable_names_map[term.concated_variable_name()] =
                        term.variable_names;
                }
            }
        }

        for (const auto &constraint : this->hard_constraints) {
            for (const auto &term : constraint.terms) {
                for (const auto &variable_name : term.variable_names) {
                    if (variable_name.front() == '~') {
                        const auto ORIGINAL_VARIABLE_NAME =
                            variable_name.substr(1);
                        variable_names_set.insert(ORIGINAL_VARIABLE_NAME);
                        nagated_variable_names_set.insert(
                            ORIGINAL_VARIABLE_NAME);
                    } else {
                        variable_names_set.insert(variable_name);
                    }
                }
                if (term.variable_names.size() > 1) {
                    product_variable_names_map[term.concated_variable_name()] =
                        term.variable_names;
                }
            }
        }

        std::vector<std::string> variable_names(variable_names_set.begin(),
                                                variable_names_set.end());
        std::stable_sort(variable_names.begin(), variable_names.end());
        this->variable_names = variable_names;

        std::vector<std::string> negated_variable_names(
            nagated_variable_names_set.begin(),
            nagated_variable_names_set.end());
        std::stable_sort(negated_variable_names.begin(),
                         negated_variable_names.end());
        this->negated_variable_names = negated_variable_names;

        std::vector<std::pair<std::string, std::vector<std::string>>>
            product_variable_names(product_variable_names_map.begin(),
                                   product_variable_names_map.end());
        std::stable_sort(product_variable_names.begin(),
                         product_variable_names.end());
        this->product_variable_names = product_variable_names;
    }

    /*************************************************************************/
    inline void read_opb(const std::string &a_FILE_NAME) {
        std::vector<std::string> lines;
        /**
         * Read and store entire part of the opb file.
         */
        {
            std::ifstream ifs;
            std::string   buffer;

            ifs.open(a_FILE_NAME.c_str());
            if (ifs.fail()) {
                throw std::runtime_error(utility::format_error_location(
                    __FILE__, __LINE__, __func__,
                    "Cannot open the specified OPB file: " + a_FILE_NAME));
            }
            while (std::getline(ifs, buffer)) {
                if (!buffer.empty() && buffer.back() == ';') {
                    buffer.pop_back();
                }
                utility::trim(buffer);
                lines.push_back(buffer);
            }
            ifs.close();
        }
        const int        LINES_SIZE = lines.size();
        std::vector<int> soft_constraint_lines;
        std::vector<int> hard_constraint_lines;

        soft_constraint_lines.reserve(LINES_SIZE);
        hard_constraint_lines.reserve(LINES_SIZE);

        for (auto i = 0; i < LINES_SIZE; i++) {
            auto &line = lines[i];

            if (line.empty()) {
                continue;
            }

            if (line.front() == '*') {
                if (i == 0) {
                    this->metadata = OPB::parse_metadata(line);
                }
                continue;
            }

            if (line.front() == 's' || line.front() == 'S') {
                this->top_cost = OPB::parse_top_cost(line);
            } else if (line.front() == 'm' || line.front() == 'M') {
                this->objective = OPB::parse_objective(line);
            } else if (line.front() == '[') {
                soft_constraint_lines.push_back(i);
            } else {
                hard_constraint_lines.push_back(i);
            }
        }

        this->soft_constraints.resize(soft_constraint_lines.size());
        this->hard_constraints.resize(hard_constraint_lines.size());

        const int SOFT_CONSTRAINTS_SIZE = soft_constraints.size();
#ifdef _OPENMP
#pragma omp parallel for schedule(static)
#endif
        for (auto i = 0; i < SOFT_CONSTRAINTS_SIZE; i++) {
            this->soft_constraints[i] =
                OPB::parse_soft_constraint(lines[soft_constraint_lines[i]], i);
        }
        const int HARD_CONSTRAINTS_SIZE = hard_constraints.size();
#ifdef _OPENMP
#pragma omp parallel for schedule(static)
#endif
        for (auto i = 0; i < HARD_CONSTRAINTS_SIZE; i++) {
            this->hard_constraints[i] =
                OPB::parse_hard_constraint(lines[hard_constraint_lines[i]], i);
        }

        this->setup_variable_information();
    }
};
}  // namespace printemps::opb
#endif
/*****************************************************************************/
// END
/*****************************************************************************/