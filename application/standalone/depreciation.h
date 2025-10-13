/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_DEPRECIATION_H__
#define PRINTEMPS_DEPRECIATION_H__

#include <printemps.h>

namespace printemps {
/*****************************************************************************/
bool is_deprecated_executable_name(const std::string& a_SOLVER_NAME) {
    const std::string EXECUTABLE_NAME = utility::to_lowercase(a_SOLVER_NAME);
    return (EXECUTABLE_NAME.find("mps_solver") != std::string::npos ||
            EXECUTABLE_NAME.find("opb_solver") != std::string::npos);
}

/*****************************************************************************/
void print_deprecation_warning() {
#ifdef _PRINTEMPS_STYLING
    std::cerr << printemps::constant::YELLOW;
#endif
    std::cerr
        << "WARNING: The standalone solvers 'mps_solver' and 'opb_solver' are "
           "now deprecated. They have been fully integrated into the new "
           "executable 'printemps'. Please migrate your workflows to "
           "'printemps', as 'mps_solver' and 'opb_solver' will be removed in "
           "future releases."
        << std::endl;
#ifdef _PRINTEMPS_STYLING
    std::cerr << printemps::constant::NO_COLOR;
#endif
}

/*****************************************************************************/
void replace_deprecated_arguments(int argc, char* argv[]) {
    static const std::unordered_map<std::string, std::string>
        replacement_table = {
            {"--include-mps-loading-time", "--include-instance-loading-time"},
            {"--include-opb-loading-time", "--include-instance-loading-time"},
        };

    static std::vector<std::string> replaced_storage;

    for (int i = 1; i < argc; ++i) {
        auto it = replacement_table.find(argv[i]);
        if (it != replacement_table.end()) {
#ifdef _PRINTEMPS_STYLING
            std::cerr << printemps::constant::YELLOW;
#endif
            std::cerr
                << "WARNING: Option '" << it->first
                << "' is deprecated and has been automatically replaced with '"
                << it->second << "'." << std::endl;
#ifdef _PRINTEMPS_STYLING
            std::cerr << printemps::constant::NO_COLOR;
#endif

            replaced_storage.emplace_back(it->second);
            argv[i] = replaced_storage.back().data();
        }
    }
}
}  // namespace printemps
#endif

/*****************************************************************************/
// END
/*****************************************************************************/
