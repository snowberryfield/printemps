/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/

#ifndef CPPMH_UTILITY_QAPLIB_UTILITY_H__
#ifndef CPPMH_UTILITY_QAPLIB_UTILITY_H__

#include <fstream>
#include <sstream>

#include <cppmh.h>

namespace cppmh {
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

    std::ifstream input_file_stream;
    std::string   line;
    std::string   buffer;
    input_file_stream.open(a_FILE_NAME.c_str());

    /// Read the problem size.
    while (true) {
        std::getline(input_file_stream, line);
        if (cppmh::utility::trim(line).size() > 0) {
            problem_instance.N = atoi(line.c_str());
            problem_instance.A.resize(problem_instance.N);
            problem_instance.B.resize(problem_instance.N);
            break;
        }
    }

    auto count = 0;

    // Read the matrix A.
    while (count < problem_instance.N) {
        std::getline(input_file_stream, line);
        if (cppmh::utility::trim(line).size() > 0) {
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
        std::getline(input_file_stream, line);
        if (cppmh::utility::trim(line).size() > 0) {
            std::stringstream stream(line);
            while (stream >> buffer) {
                problem_instance.B[count].push_back(atof(buffer.c_str()));
            }
            count++;
        }
    }

    return problem_instance;
}
}  // namespace utility
}  // namespace cppmh

/*****************************************************************************/
// END
/*****************************************************************************/