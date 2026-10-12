/*****************************************************************************/
// Copyright (c) 2020-2026 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_ERROR_HANDLER_ERROR_HANDLER_H__
#define PRINTEMPS_ERROR_HANDLER_ERROR_HANDLER_H__

namespace printemps::error_handler {
/*****************************************************************************/
class InfeasibleError : public std::runtime_error {
   public:
    explicit InfeasibleError(const std::string &a_MESSAGE)
        : std::runtime_error(a_MESSAGE) {
    }
};
}  // namespace printemps::error_handler
#endif
/*****************************************************************************/
// END
/*****************************************************************************/
