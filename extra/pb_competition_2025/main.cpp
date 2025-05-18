/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <printemps.h>
#include "pb_competition_2025_solver.h"

int main(const int argc, const char *argv[]) {
    try {
        printemps::extra::pb_competition_2025::PBCompetition2025Solver(argc,
                                                                       argv)
            .run();
    } catch (const std::exception &e) {
        std::cout << "c Unsupported file format or missing file." << std::endl;
        std::cout << "s UNSUPPORTED" << std::endl;
    }
    return 0;
}
/*****************************************************************************/
// END
/*****************************************************************************/
