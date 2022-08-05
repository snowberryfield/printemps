/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_OPTION_IMPROVABILITY_SCREENING_MODE_H__
#define PRINTEMPS_OPTION_IMPROVABILITY_SCREENING_MODE_H__

namespace printemps::option::improvability_screening_mode {
/*****************************************************************************/
enum ImprovabilityScreeningMode : int {  //
    Off,
    Soft,
    Aggressive,
    Automatic,
    Intensive
};
}  // namespace printemps::option::improvability_screening_mode

#endif
/*****************************************************************************/
// END
/*****************************************************************************/