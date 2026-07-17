/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_MPS_MPS_BOUND_TYPE_H__
#define PRINTEMPS_MPS_MPS_BOUND_TYPE_H__

namespace printemps::mps {
/*****************************************************************************/
enum class MPSBoundType : uint8_t {
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
inline MPSBoundType bound_type_map(
    const std::string_view &a_STYPE_SV) noexcept {
    switch (a_STYPE_SV.size()) {
        case 2: {
            const char c0 = a_STYPE_SV[0];
            const char c1 = a_STYPE_SV[1];
            if (c0 == 'B')
                return MPSBoundType::BV;
            if (c0 == 'F' && c1 == 'R')
                return MPSBoundType::FR;
            if (c0 == 'M' && c1 == 'I')
                return MPSBoundType::MI;
            if (c0 == 'P' && c1 == 'L')
                return MPSBoundType::PL;
            if (c0 == 'L' && c1 == 'O')
                return MPSBoundType::LO;
            if (c0 == 'L' && c1 == 'I')
                return MPSBoundType::LI;
            if (c0 == 'U' && c1 == 'P')
                return MPSBoundType::UP;
            if (c0 == 'U' && c1 == 'I')
                return MPSBoundType::UI;
            if (c0 == 'F' && c1 == 'X')
                return MPSBoundType::FX;
            break;
        }
        default:
            break;
    }
    return MPSBoundType::UNKNOWN;
}
}  // namespace printemps::mps
#endif
/*****************************************************************************/
// END
/*****************************************************************************/