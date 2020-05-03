/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef CPPMH_UTILITY_CONSOLE_UTILITY_H__
#define CPPMH_UTILITY_CONSOLE_UTILITY_H__

#include <iostream>

namespace cppmh {
namespace utility {
/******************************************************************************/
inline void print_single_line(const bool a_IS_ENABLED) {
    if (!a_IS_ENABLED) {
        return;
    }
    std::cout << "_____________________________________________________________"
                 "___________________"
              << std::endl;
}

/******************************************************************************/
inline void print_single_broken_line(const bool a_IS_ENABLED) {
    if (!a_IS_ENABLED) {
        return;
    }
    std::cout << "-------------------------------------------------------------"
                 "-------------------"
              << std::endl;
}

/******************************************************************************/
inline void print_double_line(const bool a_IS_ENABLED) {
    if (!a_IS_ENABLED) {
        return;
    }
    std::cout << "============================================================="
                 "==================="
              << std::endl;
}

/******************************************************************************/
inline void print_line_break(int        a_number_of_line_breaks,
                             const bool a_IS_ENABLED) {
    if (!a_IS_ENABLED) {
        return;
    }
    for (int i = 0; i < a_number_of_line_breaks; i++) {
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
inline void print_parameter(const std::string &a_NAME, const double a_VALUE,
                            const bool a_IS_ENABLED) {
    if (!a_IS_ENABLED) {
        return;
    }
    char temp[80];
    sprintf(temp, "%25s : %.6e", a_NAME.c_str(), a_VALUE);
    std::cout << temp << std::endl;
}

/******************************************************************************/
inline void print_parameter(const std::string &a_NAME, const int a_VALUE,
                            const bool a_IS_ENABLED) {
    if (!a_IS_ENABLED) {
        return;
    }
    char temp[80];
    sprintf(temp, "%25s : %d", a_NAME.c_str(), a_VALUE);
    std::cout << temp << std::endl;
}

/******************************************************************************/
inline void print_parameter(const std::string &a_NAME,
                            const std::string  a_VALUE,
                            const bool         a_IS_ENABLED) {
    if (!a_IS_ENABLED) {
        return;
    }
    char temp[80];
    sprintf(temp, "%25s : %s", a_NAME.c_str(), a_VALUE.c_str());
    std::cout << temp << std::endl;
}

/******************************************************************************/
inline void print_parameter(const std::string &a_NAME, const double a_BEGIN,
                            const double a_END, const bool a_IS_ENABLED) {
    if (!a_IS_ENABLED) {
        return;
    }
    char temp[80];
    sprintf(temp, "%25s : %.6e -> %.6e", a_NAME.c_str(), a_BEGIN, a_END);
    std::cout << temp << std::endl;
}

/******************************************************************************/
inline void print_message(const std::string &a_LINE, const bool a_IS_ENABLED) {
    if (!a_IS_ENABLED) {
        return;
    }
    const std::string str = "MESSAGE : " + a_LINE;

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
inline void print_error(const std::string &a_LINE, const bool a_IS_ENABLED) {
    if (!a_IS_ENABLED) {
        return;
    }
    const std::string str = "ERROR   : " + a_LINE;

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
inline void print_warning(const std::string &a_LINE, const bool a_IS_ENABLED) {
    if (!a_IS_ENABLED) {
        return;
    }
    const std::string str = "WARNING : " + a_LINE;

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
inline void print_info(const std::string &a_LINE, const bool a_IS_ENABLED) {
    if (!a_IS_ENABLED) {
        return;
    }
    const std::string str = "INFO    : " + a_LINE;

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
}  // namespace utility
}  // namespace cppmh

/******************************************************************************/
// END
/******************************************************************************/
#endif