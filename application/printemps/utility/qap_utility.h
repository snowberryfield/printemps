/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/

#ifndef PRINTEMPS_UTILITY_QAP_UTILITY_H__
#define PRINTEMPS_UTILITY_QAP_UTILITY_H__

#include <fstream>
#include <sstream>

#include <printemps.h>

namespace printemps {
namespace utility {
/*****************************************************************************/
struct QuadraticAssignmentProblem {
    int                              N;
    std::vector<std::vector<double>> A;
    std::vector<std::vector<double>> B;
};

/*****************************************************************************/
QuadraticAssignmentProblem read_qaplib(const std::string& a_FILE_NAME) {
    QuadraticAssignmentProblem problem_instance;

    std::ifstream ifs;
    std::string   line;
    std::string   buffer;
    ifs.open(a_FILE_NAME.c_str());
    if (ifs.fail()) {
        throw std::logic_error(utility::format_error_location(
            __FILE__, __LINE__, __func__,
            "Cannot open the specified QAPLIB file: " + a_FILE_NAME));
    }

    /// Read the problem size.
    while (true) {
        std::getline(ifs, line);
        if (trim(line).size() > 0) {
            problem_instance.N = atoi(line.c_str());
            problem_instance.A.resize(problem_instance.N);
            problem_instance.B.resize(problem_instance.N);
            break;
        }
    }

    auto count = 0;

    // Read the matrix A.
    while (count < problem_instance.N) {
        std::getline(ifs, line);
        if (trim(line).size() > 0) {
            std::stringstream stream(line);
            while (stream >> buffer) {
                problem_instance.A[count].push_back(atof(buffer.c_str()));
            }
            count++;
        }
    }

    // Read the matrix B.
    count = 0;
    while (count < problem_instance.N) {
        std::getline(ifs, line);
        if (trim(line).size() > 0) {
            std::stringstream stream(line);
            while (stream >> buffer) {
                problem_instance.B[count].push_back(atof(buffer.c_str()));
            }
            count++;
        }
    }

    return problem_instance;
}

/*****************************************************************************/
class QAPLIBReader {
   private:
    model::IPModel m_model;

   public:
    /*************************************************************************/
    QAPLIBReader(void) {
        this->initialize();
    }

    /*************************************************************************/
    ~QAPLIBReader(void) {
        /* nothing to do */
    }

    /*************************************************************************/
    inline void initialize(void) {
        m_model.initialize();
    }

    /*****************************************************************************/
    inline model::IPModel& create_model_from_qaplib(
        const std::string& a_FILE_NAME) {
        QuadraticAssignmentProblem qap = read_qaplib(a_FILE_NAME);

        /*************************************************************************/
        /// Decision variable definitions
        /*************************************************************************/
        auto& p = m_model.create_variables("p", qap.N, 0, qap.N - 1);

        /*************************************************************************/
        /// Objective function definition
        /*************************************************************************/
        std::function<double(const model::IPMove&)> f =
            [qap, &p](const model::IPMove& a_MOVE) {
                double f = 0.0;

                std::vector<int> p_values(qap.N);
                for (auto n = 0; n < qap.N; n++) {
                    p_values[n] = p(n).evaluate(a_MOVE);
                }

                for (auto n = 0; n < qap.N; n++) {
                    for (auto m = 0; m < qap.N; m++) {
                        f += qap.A[n][m] * qap.B[p_values[n]][p_values[m]];
                    }
                }
                return f;
            };
        m_model.minimize(f);

        for (auto n = 0; n < qap.N; n++) {
            p(n) = n;
        }

        std::function<void(std::vector<model::IPMove>*)> move_updater =
            [qap, &p](std::vector<model::IPMove>* a_moves) {
                a_moves->resize(qap.N * (qap.N - 1) / 2 +
                                qap.N * (qap.N - 1) * (qap.N - 2) / 3);
                auto count = 0;

                /// Swap moves between two components
                for (auto n = 0; n < qap.N; n++) {
                    for (auto m = n + 1; m < qap.N; m++) {
                        (*a_moves)[count].alterations.clear();
                        (*a_moves)[count].alterations.emplace_back(
                            &p(n), p(m).value());
                        (*a_moves)[count].alterations.emplace_back(
                            &p(m), p(n).value());
                        count++;
                    }
                }
                /// Swap moves between three components
                for (auto n = 0; n < qap.N; n++) {
                    for (auto m = n + 1; m < qap.N; m++) {
                        for (auto k = m + 1; k < qap.N; k++) {
                            (*a_moves)[count].alterations.clear();
                            (*a_moves)[count].alterations.emplace_back(
                                &p(n), p(m).value());
                            (*a_moves)[count].alterations.emplace_back(
                                &p(m), p(k).value());
                            (*a_moves)[count].alterations.emplace_back(
                                &p(k), p(n).value());
                            count++;

                            (*a_moves)[count].alterations.clear();
                            (*a_moves)[count].alterations.emplace_back(
                                &p(n), p(k).value());
                            (*a_moves)[count].alterations.emplace_back(
                                &p(m), p(n).value());
                            (*a_moves)[count].alterations.emplace_back(
                                &p(k), p(m).value());
                            count++;
                        }
                    }
                }
            };
        m_model.neighborhood().set_user_defined_move_updater(move_updater);
        return m_model;
    }
};

}  // namespace utility
}  // namespace printemps

#endif
/*****************************************************************************/
// END
/*****************************************************************************/