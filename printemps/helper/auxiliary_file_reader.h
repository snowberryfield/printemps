/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_HELPER_AUXILIARY_FILE_READER_H__
#define PRINTEMPS_HELPER_AUXILIARY_FILE_READER_H__

namespace printemps::helper {
/*****************************************************************************/
inline std::unordered_map<std::string, int> read_names_and_values(
    const std::string &a_FILE_NAME) {
    std::unordered_map<std::string, int> names_and_values;

    std::vector<std::string> lines;
    std::string              item;

    /**
     * Read and store entire part of the file.
     */
    {
        std::ifstream ifs;
        std::string   buffer;

        ifs.open(a_FILE_NAME.c_str());
        if (ifs.fail()) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "Cannot open the specified file: " + a_FILE_NAME));
        }
        while (std::getline(ifs, buffer)) {
            lines.push_back(buffer);
        }
        ifs.close();
    }

    /**
     * Extract the names and their corresponding values.
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
        names_and_values[items[0]] =
            static_cast<int>(floor(0.5 + atof(items[1].c_str())));
    }

    return names_and_values;
}

/*****************************************************************************/
inline std::unordered_set<std::string> read_names(
    const std::string &a_FILE_NAME) {
    std::unordered_set<std::string> names;
    std::vector<std::string>        lines;
    std::string                     item;

    /**
     * Read and store entire part of the file.
     */
    {
        std::ifstream ifs;
        std::string   buffer;

        ifs.open(a_FILE_NAME.c_str());
        if (ifs.fail()) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "Cannot open the specified file: " + a_FILE_NAME));
        }
        while (std::getline(ifs, buffer)) {
            lines.push_back(buffer);
        }
        ifs.close();
    }

    /**
     * Extract the names.
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
        names.insert(items[0]);
    }

    return names;
}

/*****************************************************************************/
inline std::vector<std::pair<std::string, std::string>> read_name_pairs(
    const std::string &a_FILE_NAME) {
    std::vector<std::pair<std::string, std::string>> name_pairs;

    std::vector<std::string> lines;
    std::string              item;

    /**
     * Read and store entire part of file.
     */
    {
        std::ifstream ifs;
        std::string   buffer;

        ifs.open(a_FILE_NAME.c_str());
        if (ifs.fail()) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "Cannot open the specified file: " + a_FILE_NAME));
        }
        while (std::getline(ifs, buffer)) {
            lines.push_back(buffer);
        }
        ifs.close();
    }

    /**
     * Extract the name pairs.
     */
    for (const auto &line : lines) {
        std::stringstream        stream(line);
        std::vector<std::string> items;
        while (stream >> item) {
            items.push_back(item);
        }
        int ITEMS_SIZE = items.size();

        if (ITEMS_SIZE < 2) {
            continue;
        }
        name_pairs.emplace_back(items[0], items[1]);
    }

    return name_pairs;
}
}  // namespace printemps::helper
#endif
/*****************************************************************************/
// END
/*****************************************************************************/