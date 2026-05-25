/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_WCNF_WCNF_H__
#define PRINTEMPS_WCNF_WCNF_H__

#include "wcnf_clause.h"
#include "wcnf_metadata.h"

namespace printemps::wcnf {
/*****************************************************************************/
struct WCNF {
    std::string             name;
    WCNFMetadata            metadata;
    std::vector<WCNFClause> hard_clauses;
    std::vector<WCNFClause> soft_clauses;
    int                     number_of_variables;

    /*************************************************************************/
    WCNF(void) {
        this->initialize();
    }

    /*************************************************************************/
    WCNF(const std::string &a_FILE_NAME) {
        this->initialize();
        this->read_wcnf(a_FILE_NAME);
    }

    /*************************************************************************/
    inline void initialize(void) {
        this->name.clear();
        this->metadata.initialize();
        this->hard_clauses.clear();
        this->soft_clauses.clear();
        this->number_of_variables = 0;
    }

    /*************************************************************************/
    inline static WCNFClause parse_clause(const std::string &a_LINE,
                                          const int          a_INDEX) {
        WCNFClause        clause;
        std::stringstream stream(a_LINE);
        std::string       token;

        if (!(stream >> token)) {
            throw std::runtime_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The WCNF file has an empty clause line."));
        }

        if (token == "h" || token == "H") {
            clause.is_hard = true;
            clause.weight  = 0;
            clause.name    = "hard_clause_" + std::to_string(a_INDEX);
        } else {
            /**
             * The first token is the soft-clause weight (>= 1, <= 2^63 - 1).
             */
            try {
                const long long PARSED = std::stoll(token);
                if (PARSED < 1) {
                    throw std::runtime_error(utility::format_error_location(
                        __FILE__, __LINE__, __func__,
                        "The WCNF soft clause weight must be at least 1."));
                }
                clause.weight = static_cast<uint64_t>(PARSED);
            } catch (const std::invalid_argument &) {
                throw std::runtime_error(utility::format_error_location(
                    __FILE__, __LINE__, __func__,
                    "The WCNF clause has a malformed weight token: " + token));
            } catch (const std::out_of_range &) {
                throw std::runtime_error(utility::format_error_location(
                    __FILE__, __LINE__, __func__,
                    "The WCNF clause weight is out of range: " + token));
            }
            clause.is_hard = false;
            clause.name    = "soft_clause_" + std::to_string(a_INDEX);
        }

        bool has_terminator = false;
        while (stream >> token) {
            int literal = 0;
            try {
                literal = std::stoi(token);
            } catch (const std::invalid_argument &) {
                throw std::runtime_error(utility::format_error_location(
                    __FILE__, __LINE__, __func__,
                    "The WCNF clause has a malformed literal token: " + token));
            } catch (const std::out_of_range &) {
                throw std::runtime_error(utility::format_error_location(
                    __FILE__, __LINE__, __func__,
                    "The WCNF clause literal is out of range: " + token));
            }

            if (literal == 0) {
                has_terminator = true;
                /**
                 * Per the WCNF format, the terminating 0 must be the last
                 * token; trailing tokens after it are invalid.
                 */
                std::string trailing;
                if (stream >> trailing) {
                    throw std::runtime_error(utility::format_error_location(
                        __FILE__, __LINE__, __func__,
                        "The WCNF clause has tokens after the 0 terminator."));
                }
                break;
            }
            clause.literals.push_back(literal);
        }

        if (!has_terminator) {
            throw std::runtime_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The WCNF clause is missing the 0 terminator."));
        }

        return clause;
    }

    /*************************************************************************/
    inline void read_wcnf(const std::string &a_FILE_NAME) {
        std::ifstream ifs;
        ifs.open(a_FILE_NAME.c_str());
        if (ifs.fail()) {
            throw std::runtime_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "Cannot open the specified WCNF file: " + a_FILE_NAME));
        }

        int         clause_index = 0;
        std::string buffer;
        while (std::getline(ifs, buffer)) {
            const std::string LINE = utility::trim(buffer);
            if (LINE.empty()) {
                continue;
            }
            /**
             * Comment lines and the legacy "p" header line (if present in any
             * legacy file) are skipped.
             */
            if (LINE.front() == 'c' || LINE.front() == 'C' ||
                LINE.front() == 'p' || LINE.front() == 'P') {
                continue;
            }

            WCNFClause clause = WCNF::parse_clause(LINE, clause_index++);

            for (const auto literal : clause.literals) {
                const int VAR_INDEX = std::abs(literal);
                if (VAR_INDEX > this->number_of_variables) {
                    this->number_of_variables = VAR_INDEX;
                }
            }

            if (clause.is_hard) {
                this->hard_clauses.push_back(std::move(clause));
            } else {
                this->soft_clauses.push_back(std::move(clause));
            }
        }
        ifs.close();

        /**
         * Populate metadata.
         */
        this->metadata.number_of_variables    = this->number_of_variables;
        this->metadata.number_of_hard_clauses = this->hard_clauses.size();
        this->metadata.number_of_soft_clauses = this->soft_clauses.size();
        this->metadata.number_of_clauses =
            this->metadata.number_of_hard_clauses +
            this->metadata.number_of_soft_clauses;

        uint64_t weight_sum = 0;
        uint64_t max_weight = 0;
        for (const auto &clause : this->soft_clauses) {
            /**
             * Detect overflow of the weight sum. The MSE rules require
             * weight_sum < 2^64 - 1.
             */
            if (weight_sum > std::numeric_limits<uint64_t>::max() -
                                 clause.weight - 1) {
                throw std::runtime_error(utility::format_error_location(
                    __FILE__, __LINE__, __func__,
                    "The sum of soft clause weights overflows uint64_t."));
            }
            weight_sum += clause.weight;
            if (clause.weight > max_weight) {
                max_weight = clause.weight;
            }
        }
        this->metadata.weight_sum = weight_sum;
        this->metadata.max_weight = max_weight;
    }

    /*************************************************************************/
    inline void print(void) const {
        std::cout << "[metadata]" << std::endl;
        std::cout << " #variable: " << this->metadata.number_of_variables
                  << std::endl;
        std::cout << " #clause: " << this->metadata.number_of_clauses
                  << std::endl;
        std::cout << " #hard: " << this->metadata.number_of_hard_clauses
                  << std::endl;
        std::cout << " #soft: " << this->metadata.number_of_soft_clauses
                  << std::endl;
        std::cout << " weight_sum: " << this->metadata.weight_sum << std::endl;
        std::cout << " max_weight: " << this->metadata.max_weight << std::endl;

        std::cout << "[hard clauses]" << std::endl;
        for (const auto &clause : this->hard_clauses) {
            std::cout << " " << clause.name << ":";
            for (const auto literal : clause.literals) {
                std::cout << " " << literal;
            }
            std::cout << std::endl;
        }

        std::cout << "[soft clauses]" << std::endl;
        for (const auto &clause : this->soft_clauses) {
            std::cout << " " << clause.name << " (w=" << clause.weight << "):";
            for (const auto literal : clause.literals) {
                std::cout << " " << literal;
            }
            std::cout << std::endl;
        }
    }
};
}  // namespace printemps::wcnf
#endif
/*****************************************************************************/
// END
/*****************************************************************************/
