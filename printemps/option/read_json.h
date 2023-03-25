/*****************************************************************************/
// Copyright (c) 2020-2023 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_OPTION_READ_JSON_H__
#define PRINTEMPS_OPTION_READ_JSON_H__

namespace printemps::option {
/*****************************************************************************/
inline bool read_json(double *a_parameter, const std::string &a_NAME,
                      const utility::json::JsonObject &a_JSON) {
    if (a_JSON.find(a_NAME)) {
        if (a_JSON.at(a_NAME).type() == typeid(int)) {
            *a_parameter = a_JSON.get<int>(a_NAME);
        } else {
            *a_parameter = a_JSON.get<double>(a_NAME);
        }
        return true;
    }
    return false;
}

/**************************************************************************/
inline bool read_json(restart_mode::RestartMode *      a_parameter,
                      const std::string &              a_NAME,
                      const utility::json::JsonObject &a_JSON) {
    if (a_JSON.find(a_NAME)) {
        if (a_JSON.at(a_NAME).type() == typeid(int)) {
            *a_parameter =
                static_cast<restart_mode::RestartMode>(a_JSON.get<int>(a_NAME));
            return true;
        }

        if (a_JSON.at(a_NAME).type() == typeid(std::string)) {
            *a_parameter = restart_mode::RestartModeMap.at(
                a_JSON.get<std::string>(a_NAME));
            return true;
        }

        return true;
    }
    return false;
}

/*****************************************************************************/
inline bool read_json(chain_move_reduce_mode::ChainMoveReduceMode *a_parameter,
                      const std::string &                          a_NAME,
                      const utility::json::JsonObject &            a_JSON) {
    if (a_JSON.find(a_NAME)) {
        if (a_JSON.at(a_NAME).type() == typeid(int)) {
            *a_parameter =
                static_cast<chain_move_reduce_mode::ChainMoveReduceMode>(
                    a_JSON.get<int>(a_NAME));
            return true;
        }

        if (a_JSON.at(a_NAME).type() == typeid(std::string)) {
            *a_parameter = chain_move_reduce_mode::ChainMoveReduceModeMap.at(
                a_JSON.get<std::string>(a_NAME));
            return true;
        }
    }
    return false;
}

/**************************************************************************/
inline bool read_json(selection_mode::SelectionMode *  a_parameter,
                      const std::string &              a_NAME,
                      const utility::json::JsonObject &a_JSON) {
    if (a_JSON.find(a_NAME)) {
        if (a_JSON.at(a_NAME).type() == typeid(int)) {
            *a_parameter = static_cast<selection_mode::SelectionMode>(
                a_JSON.get<int>(a_NAME));
            return true;
        }

        if (a_JSON.at(a_NAME).type() == typeid(std::string)) {
            *a_parameter = selection_mode::SelectionModeMap.at(
                a_JSON.get<std::string>(a_NAME));
            return true;
        }

        return true;
    }
    return false;
}

/**************************************************************************/
inline bool read_json(
    improvability_screening_mode::ImprovabilityScreeningMode *a_parameter,
    const std::string &a_NAME, const utility::json::JsonObject &a_JSON) {
    if (a_JSON.find(a_NAME)) {
        if (a_JSON.at(a_NAME).type() == typeid(int)) {
            *a_parameter = static_cast<
                improvability_screening_mode::ImprovabilityScreeningMode>(
                a_JSON.get<int>(a_NAME));
            return true;
        }

        if (a_JSON.at(a_NAME).type() == typeid(std::string)) {
            *a_parameter =
                improvability_screening_mode::ImprovabilityScreeningModeMap.at(
                    a_JSON.get<std::string>(a_NAME));
            return true;
        }

        return true;
    }
    return false;
}

/**************************************************************************/
inline bool read_json(verbose::Verbose *a_parameter, const std::string &a_NAME,
                      const utility::json::JsonObject &a_JSON) {
    if (a_JSON.find(a_NAME)) {
        if (a_JSON.at(a_NAME).type() == typeid(int)) {
            *a_parameter =
                static_cast<verbose::Verbose>(a_JSON.get<int>(a_NAME));
            return true;
        }
        if (a_JSON.at(a_NAME).type() == typeid(std::string)) {
            *a_parameter =
                verbose::VerboseMap.at(a_JSON.get<std::string>(a_NAME));
            return true;
        }

        return true;
    }
    return false;
}

/**************************************************************************/
inline bool read_json(tabu_mode::TabuMode *            a_parameter,
                      const std::string &              a_NAME,
                      const utility::json::JsonObject &a_JSON) {
    if (a_JSON.find(a_NAME)) {
        if (a_JSON.at(a_NAME).type() == typeid(int)) {
            *a_parameter =
                static_cast<tabu_mode::TabuMode>(a_JSON.get<int>(a_NAME));
            return true;
        }

        if (a_JSON.at(a_NAME).type() == typeid(std::string)) {
            *a_parameter =
                tabu_mode::TabuModeMap.at(a_JSON.get<std::string>(a_NAME));
            return true;
        }
    }
    return false;
}

/**************************************************************************/
template <class T>
inline bool read_json(T *a_parameter, const std::string &a_NAME,
                      const utility::json::JsonObject &a_JSON) {
    if (a_JSON.find(a_NAME)) {
        *a_parameter = a_JSON.get<T>(a_NAME);
        return true;
    }
    return false;
}
}  // namespace printemps::option
#endif
/*****************************************************************************/
// END
/*****************************************************************************/