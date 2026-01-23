/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_UTILITY_CONSOLE_UTILITY_H__
#define PRINTEMPS_UTILITY_CONSOLE_UTILITY_H__

namespace printemps::utility {
/******************************************************************************/
inline void print_single_line(const bool a_IS_ENABLED) {
    if (!a_IS_ENABLED) {
        return;
    }
    std::string single_line(80, '_');
    std::cout << single_line << std::endl;
}

/******************************************************************************/
inline void print_single_broken_line(const bool a_IS_ENABLED) {
    if (!a_IS_ENABLED) {
        return;
    }
    std::string single_broken_line(80, '-');
    std::cout << single_broken_line << std::endl;
}

/******************************************************************************/
inline void print_dot_line(const bool a_IS_ENABLED) {
    if (!a_IS_ENABLED) {
        return;
    }
    for (auto i = 0; i < 40; i++) {
        std::cout << "- ";
    }
    std::cout << std::endl;
}

/******************************************************************************/
inline void print_double_line(const bool a_IS_ENABLED) {
    if (!a_IS_ENABLED) {
        return;
    }
    std::string double_line(80, '=');
    std::cout << double_line << std::endl;
}

/******************************************************************************/
inline void print_line_break(int        a_NUMBER_OF_LINE_BREAKS,
                             const bool a_IS_ENABLED) {
    if (!a_IS_ENABLED) {
        return;
    }
    for (auto i = 0; i < a_NUMBER_OF_LINE_BREAKS; i++) {
        std::cout << std::endl;
    }
}

/******************************************************************************/
inline void print(const std::string &a_LINE) {
    std::cout << a_LINE << std::endl;
}

/******************************************************************************/
inline void print(const std::string &a_LINE, const bool a_IS_ENABLED) {
    if (!a_IS_ENABLED) {
        return;
    }
    const std::string str = a_LINE;

    constexpr int NUMBER_OF_CHARACTERS_PER_LINE = 80;

    int number_of_lines = (str.size() - 1) / NUMBER_OF_CHARACTERS_PER_LINE + 1;
    for (auto i = 0; i < number_of_lines; i++) {
        if (i == number_of_lines - 1) {
            std::cout << str.substr(i * NUMBER_OF_CHARACTERS_PER_LINE)
                      << std::endl;
        } else {
            std::cout << str.substr(i * NUMBER_OF_CHARACTERS_PER_LINE,
                                    NUMBER_OF_CHARACTERS_PER_LINE)
                      << std::endl;
        }
    }
}

/******************************************************************************/
inline void print_with_label(const std::string &a_HEADER_LABEL,
                             const std::string &a_MESSAGE,
                             const bool         a_IS_ENABLED) {
    if (!a_IS_ENABLED) {
        return;
    }
    const std::string LABEL_FIRST_ROW = a_HEADER_LABEL;
    const std::string LABEL_OTHER_ROW(LABEL_FIRST_ROW.size(), ' ');

    const int NUMBER_OF_CHARACTERS_PER_LINE = 80 - LABEL_FIRST_ROW.size();

    int number_of_lines =
        (a_MESSAGE.size() - 1) / NUMBER_OF_CHARACTERS_PER_LINE + 1;
    for (auto i = 0; i < number_of_lines; i++) {
        std::string label = (i == 0) ? LABEL_FIRST_ROW : LABEL_OTHER_ROW;
        if (i == number_of_lines - 1) {
            std::cout << label
                      << a_MESSAGE.substr(i * NUMBER_OF_CHARACTERS_PER_LINE)
                      << std::endl;
        } else {
            std::cout << label
                      << a_MESSAGE.substr(i * NUMBER_OF_CHARACTERS_PER_LINE,
                                          NUMBER_OF_CHARACTERS_PER_LINE)
                      << std::endl;
        }
    }
}

/******************************************************************************/
inline void print_no_label(const std::string &a_MESSAGE,
                           const bool         a_IS_ENABLED) {
    print_with_label("        : ", a_MESSAGE, a_IS_ENABLED);
}

/******************************************************************************/
inline void print_message(const std::string &a_MESSAGE,
                          const bool         a_IS_ENABLED) {
    print_with_label("MESSAGE : ", a_MESSAGE, a_IS_ENABLED);
}

/******************************************************************************/
inline void print_error(const std::string &a_MESSAGE, const bool a_IS_ENABLED) {
    print_with_label("ERROR   : ", a_MESSAGE, a_IS_ENABLED);
}

/******************************************************************************/
inline void print_warning(const std::string &a_MESSAGE,
                          const bool         a_IS_ENABLED) {
    print_with_label("WARNING : ", a_MESSAGE, a_IS_ENABLED);
}

/******************************************************************************/
inline void print_info(const std::string &a_MESSAGE, const bool a_IS_ENABLED) {
    print_with_label("INFO    : ", a_MESSAGE, a_IS_ENABLED);
}

/******************************************************************************/
inline void print_debug(const std::string &a_MESSAGE, const bool a_IS_ENABLED) {
    print_with_label("DEBUG   : ", a_MESSAGE, a_IS_ENABLED);
}
}  // namespace printemps::utility

/******************************************************************************/
// END
/******************************************************************************/
#endif