/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_EXTRA_MAXSAT_EVALUATION_MAXSAT_EVALUATION_SOLVER_ARGPARSER_H__
#define PRINTEMPS_EXTRA_MAXSAT_EVALUATION_MAXSAT_EVALUATION_SOLVER_ARGPARSER_H__

namespace printemps::extra::maxsat_evaluation {
/*****************************************************************************/
struct MaxSATEvaluationSolverArgparser {
    std::string wcnf_file_name;
    double      timeout_seconds;
    bool        is_specified_timeout;

    /*************************************************************************/
    MaxSATEvaluationSolverArgparser(void) {
        this->initialize();
    }

    /*************************************************************************/
    inline void initialize(void) {
        this->wcnf_file_name.clear();
        this->timeout_seconds      = 0.0;
        this->is_specified_timeout = false;
    }

    /*************************************************************************/
    inline void print_usage(void) const {
        std::cout << std::endl;
        std::cout << "PRINTEMPS " + constant::VERSION + " (" +
                         constant::PROJECT_URL + ")"
                  << std::endl;
        std::cout << std::endl;

        std::cout << "Usage: ./maxsat_evaluation_solver "
                     "<input.wcnf> <timeout_seconds>"
                  << std::endl;
        std::cout << std::endl;
        std::cout
            << "  <input.wcnf>: Path to a WCNF instance (new format,"
               " post-2022)."
            << std::endl;
        std::cout
            << "  <timeout_seconds>: Wall-clock time limit in seconds."
            << std::endl;
    }

    /*************************************************************************/
    inline void parse(const int argc, const char *argv[]) {
        /**
         * The MaxSAT Evaluation 2026 anytime track requires the solver to be
         * invoked as `./solver <input> <timeout>`, with no flags. Anything
         * else is treated as a usage error by the caller.
         */
        if (argc < 2) {
            throw std::runtime_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "Two positional arguments are required: "
                "<input.wcnf> <timeout_seconds>."));
        }

        this->wcnf_file_name = argv[1];

        if (argc >= 3) {
            try {
                this->timeout_seconds = std::stod(argv[2]);
            } catch (const std::exception &) {
                throw std::runtime_error(utility::format_error_location(
                    __FILE__, __LINE__, __func__,
                    std::string("Invalid timeout argument: ") + argv[2]));
            }
            if (this->timeout_seconds <= 0.0) {
                throw std::runtime_error(utility::format_error_location(
                    __FILE__, __LINE__, __func__,
                    std::string("Timeout must be positive: ") + argv[2]));
            }
            this->is_specified_timeout = true;
        }
    }
};
}  // namespace printemps::extra::maxsat_evaluation
#endif
/*****************************************************************************/
// END
/*****************************************************************************/
