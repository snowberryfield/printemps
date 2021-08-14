/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_UTILITY_DATE_TIME_UTILITY_H__
#define PRINTEMPS_UTILITY_DATE_TIME_UTILITY_H__

namespace printemps {
namespace utility {
/******************************************************************************/
inline std::string date_time(void) {
    using std::chrono::system_clock;
    system_clock::time_point p = system_clock::now();

    std::time_t       t  = system_clock::to_time_t(p);
    const std::tm*    lt = std::localtime(&t);
    std::stringstream buffer;
    buffer << std::put_time(lt, "%c");
    return buffer.str();
}
}  // namespace utility
}  // namespace printemps

/******************************************************************************/
// END
/******************************************************************************/
#endif