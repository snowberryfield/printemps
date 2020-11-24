/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef printemps_UTILITY_STRING_UTILITY_H__
#define printemps_UTILITY_STRING_UTILITY_H__

#include <string>
#include <sstream>

namespace printemps {
namespace utility {
/******************************************************************************/
inline std::string trim(const std::string &a_ORIGINAL) {
    int start_position = 0;
    int end_position   = a_ORIGINAL.size() - 1;

    int size = a_ORIGINAL.size();
    for (auto i_pos = 0; i_pos < size; i_pos++) {
        if (a_ORIGINAL[i_pos] == ' ') {
            start_position++;
        } else {
            break;
        }
    }

    for (auto i_pos = 0; i_pos < size; i_pos++) {
        if (a_ORIGINAL[a_ORIGINAL.size() - 1 - i_pos] == ' ') {
            end_position--;
        } else {
            break;
        }
    }

    if (end_position >= start_position) {
        return a_ORIGINAL.substr(start_position,
                                 end_position - start_position + 1);
    } else {
        return "";
    }
}

/******************************************************************************/
inline bool has_space(const std::string &a_ORIGINAL) {
    for (std::string::const_iterator it = a_ORIGINAL.begin();
         it != a_ORIGINAL.end(); ++it) {
        if (*it == ' ') {
            return true;
        }
    }
    return false;
}

/******************************************************************************/
inline std::string delete_space(const std::string &a_ORIGINAL) {
    std::string result;
    for (std::string::const_iterator it = a_ORIGINAL.begin();
         it != a_ORIGINAL.end(); ++it) {
        if (*it != ' ') {
            result += *it;
        }
    }
    return result;
}

/******************************************************************************/
template <class T_Value>
inline std::string to_string(const T_Value      a_VALUE,
                             const std::string &a_FORMAT) {
    char result[256];
    sprintf(result, a_FORMAT.c_str(), a_VALUE);
    return result;
}

/******************************************************************************/
inline std::string format_error_location(const std::string &a_FILE_NAME,
                                         const int          a_LINE,
                                         const std::string &a_FUNCTION_NAME,
                                         const std::string &a_MESSAGE) {
    std::stringstream ss;
    ss << a_FILE_NAME << ", line " << a_LINE << ", function " << a_FUNCTION_NAME
       << ": " << a_MESSAGE;
    return ss.str();
}

/******************************************************************************/
inline std::string delete_crlf(const std::string &a_ORIGINAL) {
    const char  CR = '\r';
    const char  LF = '\n';
    std::string result;
    for (std::string::const_iterator it = a_ORIGINAL.begin();
         it != a_ORIGINAL.end(); ++it) {
        if (*it != CR && *it != LF) {
            result += *it;
        }
    }
    return result;
}

/*****************************************************************************/
inline std::string indent_spaces(const int a_INDENT_LEVEL) {
    std::string indent_width  = "    ";
    std::string indent_spaces = "";
    for (auto i = 0; i < a_INDENT_LEVEL; i++) {
        indent_spaces += indent_width;
    }
    return indent_spaces;
}

/*****************************************************************************/
inline std::string remove_extension(const std::string &a_ORIGINAL) {
    auto position = a_ORIGINAL.find_last_of(".");
    return a_ORIGINAL.substr(0, position);
}

/*****************************************************************************/
inline std::string remove_path(const std::string &a_ORIGINAL) {
    auto position = a_ORIGINAL.find_last_of("/\\");
    return a_ORIGINAL.substr(position + 1);
}

/*****************************************************************************/
inline std::string base_name(const std::string &a_ORIGINAL) {
    return remove_path(remove_extension(a_ORIGINAL));
}

}  // namespace utility
}  // namespace printemps

/******************************************************************************/
// END
/******************************************************************************/
#endif