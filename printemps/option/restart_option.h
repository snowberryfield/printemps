/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_OPTION_RESTART_OPTION_H__
#define PRINTEMPS_OPTION_RESTART_OPTION_H__

namespace printemps::option {
/*****************************************************************************/
struct RestartOptionConstant {
    static constexpr restart_mode::RestartMode DEFAULT_RESTART_MODE =
        restart_mode::RestartMode::Smart;
};

/*****************************************************************************/
struct RestartOption {
    restart_mode::RestartMode restart_mode;

    /*************************************************************************/
    RestartOption(void) {
        this->initialize();
    }

    /*************************************************************************/
    RestartOption(const utility::json::JsonObject &a_OBJECT) {
        this->setup(a_OBJECT);
    }

    /*************************************************************************/
    void initialize(void) {
        this->restart_mode  //
            = RestartOptionConstant::DEFAULT_RESTART_MODE;
    }

    /*************************************************************************/
    void print(void) const {
        utility::print(                     //
            " -- restart.restart_mode: " +  //
            restart_mode::RestartModeInverseMap.at(this->restart_mode));
    }

    /**************************************************************************/
    inline void setup(const utility::json::JsonObject &a_OBJECT) {
        this->initialize();
        read_json(&this->restart_mode, "restart_mode", a_OBJECT);
    }

    /**************************************************************************/
    inline utility::json::JsonObject to_json(void) const {
        utility::json::JsonObject obj;
        obj.emplace_back(    //
            "restart_mode",  //
            restart_mode::RestartModeInverseMap.at(this->restart_mode));

        return obj;
    }
};
}  // namespace printemps::option

#endif
/*****************************************************************************/
// END
/*****************************************************************************/