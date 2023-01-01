/*****************************************************************************/
// Copyright (c) 2020-2023 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/

#define _PRINTEMPS_LINEAR_MINIMIZATION
#define _PRINTEMPS_STYLING

#include <printemps.h>

int main(int argc, char *argv[]) {
    printemps::standalone::MPSSolver(argc, argv).run();
    return 0;
}
/*****************************************************************************/
// END
/*****************************************************************************/
