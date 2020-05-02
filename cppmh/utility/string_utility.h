/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef CPPMH_UTILITY_STRING_UTILITY_H__
#define CPPMH_UTILITY_STRING_UTILITY_H__

#include <string>
#include <sstream>

/******************************************************************************/
namespace cppmh {
namespace utility {
/******************************************************************************/
inline std::string trim(const std::string &a_ORIGINAL) {
    std::size_t start_position = 0;
    std::size_t end_position   = a_ORIGINAL.size() - 1;

    for (std::size_t i_pos = 0; i_pos < a_ORIGINAL.size(); i_pos++) {
        if (a_ORIGINAL[i_pos] == ' ') {
            start_position++;
        } else {
            break;
        }
    }

    for (std::size_t i_pos = 0; i_pos < a_ORIGINAL.size(); i_pos++) {
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
inline std::string delete_crlf(const std::string &a_TARGET) {
    const char  CR = '\r';
    const char  LF = '\n';
    std::string result;
    for (std::string::const_iterator it = a_TARGET.begin();
         it != a_TARGET.end(); ++it) {
        if (*it != CR && *it != LF) {
            result += *it;
        }
    }
    return result;
}
}  // namespace utility
}  // namespace cppmh

/******************************************************************************/
// END
/******************************************************************************/
#endif