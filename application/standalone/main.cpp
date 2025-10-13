/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#define _PRINTEMPS_STYLING
#include <printemps.h>
#include "depreciation.h"

int main(const int argc, const char *argv[]) {
    if (printemps::is_deprecated_executable_name(argv[0])) {
        printemps::print_deprecation_warning();
    }
    printemps::replace_deprecated_arguments(argc, const_cast<char **>(argv));
    printemps::standalone::Standalone(argc, argv).run();
    return 0;
}
/*****************************************************************************/
// END
/*****************************************************************************/
