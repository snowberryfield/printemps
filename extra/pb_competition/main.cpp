/*****************************************************************************/
// Copyright (c) 2020-2026 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <printemps.h>
#include "pb_competition_solver.h"

static void print_comment_lines(const char *msg) {
    std::istringstream iss(msg);
    std::string        line;
    while (std::getline(iss, line)) {
        std::cout << "c " << line << std::endl;
    }
}

int main(const int argc, const char *argv[]) {
    try {
        printemps::extra::pb_competition::PBCompetitionSolver(argc, argv).run();
    } catch (const printemps::error_handler::InfeasibleError &e) {
        print_comment_lines(e.what());
        std::cout << "s UNSATISFIABLE" << std::endl;
    } catch (const std::exception &e) {
        print_comment_lines(e.what());
        std::cout << "s UNSUPPORTED" << std::endl;
    }
    return 0;
}
/*****************************************************************************/
// END
/*****************************************************************************/
