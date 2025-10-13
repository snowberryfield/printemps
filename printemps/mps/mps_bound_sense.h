/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_MPS_MPS_BOUND_SENSE_H__
#define PRINTEMPS_MPS_MPS_BOUND_SENSE_H__

namespace printemps::mps {
/*****************************************************************************/
enum class MPSBoundSense : uint8_t {
    BV,
    FR,
    MI,
    PL,
    LO,
    LI,
    UP,
    UI,
    FX,
    UNKNOWN
};

/*****************************************************************************/
inline MPSBoundSense bound_sense_map(
    const std::string_view &a_SENSE_SV) noexcept {
    switch (a_SENSE_SV.size()) {
        case 2: {
            const char c0 = a_SENSE_SV[0];
            const char c1 = a_SENSE_SV[1];
            if (c0 == 'B')
                return MPSBoundSense::BV;
            if (c0 == 'F' && c1 == 'R')
                return MPSBoundSense::FR;
            if (c0 == 'M' && c1 == 'I')
                return MPSBoundSense::MI;
            if (c0 == 'P' && c1 == 'L')
                return MPSBoundSense::PL;
            if (c0 == 'L' && c1 == 'O')
                return MPSBoundSense::LO;
            if (c0 == 'L' && c1 == 'I')
                return MPSBoundSense::LI;
            if (c0 == 'U' && c1 == 'P')
                return MPSBoundSense::UP;
            if (c0 == 'U' && c1 == 'I')
                return MPSBoundSense::UI;
            if (c0 == 'F' && c1 == 'X')
                return MPSBoundSense::FX;
            break;
        }
        default:
            break;
    }
    return MPSBoundSense::UNKNOWN;
}
}  // namespace printemps::mps
#endif
/*****************************************************************************/
// END
/*****************************************************************************/