/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_HELPER_SOLUTION_FILE_READER_H__
#define PRINTEMPS_HELPER_SOLUTION_FILE_READER_H__

namespace printemps {
namespace helper {
/*****************************************************************************/
inline std::unordered_map<std::string, int> read_variable_names_and_values(
    const std::string &a_FILE_NAME) {
    std::unordered_map<std::string, int> variable_names_and_values;

    std::vector<std::string> lines;
    std::string              item;

    /**
     * Read and store entire part of the solution file.
     */
    {
        std::ifstream ifs;
        std::string   buffer;

        ifs.open(a_FILE_NAME.c_str());
        if (ifs.fail()) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "Cannot open the specified solution file: " + a_FILE_NAME));
        }
        while (std::getline(ifs, buffer)) {
            lines.push_back(buffer);
        }
        ifs.close();
    }

    /**
     * Parse the solution file.
     */
    for (const auto &line : lines) {
        std::stringstream        stream(line);
        std::vector<std::string> items;
        while (stream >> item) {
            items.push_back(item);
        }
        int ITEMS_SIZE = items.size();

        if (ITEMS_SIZE != 2) {
            continue;
        }
        variable_names_and_values[items[0]] =
            static_cast<int>(floor(0.5 + atof(items[1].c_str())));
    }

    return variable_names_and_values;
}

/*****************************************************************************/
inline std::unordered_set<std::string> read_variable_names(
    const std::string &a_FILE_NAME) {
    std::unordered_set<std::string> variable_names;
    std::vector<std::string>        lines;
    std::string                     item;

    /**
     * Read and store entire part of the variable names file.
     */
    {
        std::ifstream ifs;
        std::string   buffer;

        ifs.open(a_FILE_NAME.c_str());
        if (ifs.fail()) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "Cannot open the specified solution file: " + a_FILE_NAME));
        }
        while (std::getline(ifs, buffer)) {
            lines.push_back(buffer);
        }
        ifs.close();
    }

    /**
     * Parse the variable names file.
     */
    for (const auto &line : lines) {
        std::stringstream        stream(line);
        std::vector<std::string> items;
        while (stream >> item) {
            items.push_back(item);
        }
        int ITEMS_SIZE = items.size();

        if (ITEMS_SIZE == 0) {
            continue;
        }
        variable_names.insert(items[0]);
    }

    return variable_names;
}
}  // namespace helper
}  // namespace printemps
#endif
/*****************************************************************************/
// END
/*****************************************************************************/