/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_OPTION_OPTION_H__
#define PRINTEMPS_OPTION_OPTION_H__

#include "enumerate/enumerate.h"
#include "read_json.h"

#include "general_option.h"
#include "penalty_option.h"
#include "parallel_option.h"
#include "preprocess_option.h"
#include "restart_option.h"
#include "neighborhood_option.h"
#include "output_option.h"
#include "pdlp_option.h"
#include "lagrange_dual_option.h"
#include "local_search_option.h"
#include "tabu_search_option.h"

namespace printemps::option {
/*****************************************************************************/
struct Option {
    GeneralOption      general;
    PenaltyOption      penalty;
    ParallelOption     parallel;
    PreprocessOption   preprocess;
    RestartOption      restart;
    NeighborhoodOption neighborhood;
    OutputOption       output;
    PDLPOption         pdlp;
    LagrangeDualOption lagrange_dual;
    LocalSearchOption  local_search;
    TabuSearchOption   tabu_search;

    /*************************************************************************/
    Option(void) {
        this->initialize();
    }

    /*************************************************************************/
    Option(const std::string &a_FILE_NAME) {
        this->setup(a_FILE_NAME);
    }

    /*************************************************************************/
    void initialize(void) {
        this->general.initialize();
        this->penalty.initialize();
        this->parallel.initialize();
        this->preprocess.initialize();
        this->restart.initialize();
        this->neighborhood.initialize();
        this->output.initialize();
        this->pdlp.initialize();
        this->lagrange_dual.initialize();
        this->local_search.initialize();
        this->tabu_search.initialize();
    }

    /*************************************************************************/
    void print(void) const {
        utility::print_single_line(true);
        utility::print_info("The values for options are specified as follows:",
                            true);

        this->general.print();
        this->penalty.print();
        this->parallel.print();
        this->preprocess.print();
        this->restart.print();
        this->neighborhood.print();
        this->output.print();
        this->pdlp.print();
        this->lagrange_dual.print();
        this->local_search.print();
        this->tabu_search.print();
    }

   public:
    /**************************************************************************/
    inline void setup(const std::string &a_FILE_NAME) {
        auto   option_object = utility::json::read_json_object(a_FILE_NAME);
        Option option;

        if (option_object.find("general")) {
            option.general =
                GeneralOption(std::any_cast<utility::json::JsonObject>(
                    option_object.at("general")));
        }

        if (option_object.find("penalty")) {
            option.penalty =
                PenaltyOption(std::any_cast<utility::json::JsonObject>(
                    option_object.at("penalty")));
        }

        if (option_object.find("parallel")) {
            option.parallel =
                ParallelOption(std::any_cast<utility::json::JsonObject>(
                    option_object.at("parallel")));
        }

        if (option_object.find("preprocess")) {
            option.preprocess =
                PreprocessOption(std::any_cast<utility::json::JsonObject>(
                    option_object.at("preprocess")));
        }

        if (option_object.find("restart")) {
            option.restart =
                RestartOption(std::any_cast<utility::json::JsonObject>(
                    option_object.at("restart")));
        }

        if (option_object.find("neighborhood")) {
            option.neighborhood =
                NeighborhoodOption(std::any_cast<utility::json::JsonObject>(
                    option_object.at("neighborhood")));
        }

        if (option_object.find("output")) {
            option.output =
                OutputOption(std::any_cast<utility::json::JsonObject>(
                    option_object.at("output")));
        }

        if (option_object.find("pdlp")) {
            option.pdlp = PDLPOption(std::any_cast<utility::json::JsonObject>(
                option_object.at("pdlp")));
        }

        if (option_object.find("lagrange_dual")) {
            option.lagrange_dual =
                LagrangeDualOption(std::any_cast<utility::json::JsonObject>(
                    option_object.at("lagrange_dual")));
        }

        if (option_object.find("local_search")) {
            option.local_search =
                LocalSearchOption(std::any_cast<utility::json::JsonObject>(
                    option_object.at("local_search")));
        }

        if (option_object.find("tabu_search")) {
            option.tabu_search =
                TabuSearchOption(std::any_cast<utility::json::JsonObject>(
                    option_object.at("tabu_search")));
        }

        *this = option;
    }

    /**************************************************************************/
    inline utility::json::JsonObject to_json(void) const {
        utility::json::JsonObject obj;
        obj.emplace_back("general", this->general.to_json());
        obj.emplace_back("penalty", this->penalty.to_json());
        obj.emplace_back("parallel", this->parallel.to_json());
        obj.emplace_back("preprocess", this->preprocess.to_json());
        obj.emplace_back("restart", this->restart.to_json());
        obj.emplace_back("neighborhood", this->neighborhood.to_json());
        obj.emplace_back("output", this->output.to_json());
        obj.emplace_back("pdlp", this->pdlp.to_json());
        obj.emplace_back("lagrange_dual", this->lagrange_dual.to_json());
        obj.emplace_back("local_search", this->local_search.to_json());
        obj.emplace_back("tabu_search", this->tabu_search.to_json());

        return obj;
    };
};
}  // namespace printemps::option

#endif
/*****************************************************************************/
// END
/*****************************************************************************/