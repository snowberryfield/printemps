/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_PRESOLVER_PRESOLVER_H__
#define PRINTEMPS_PRESOLVER_PRESOLVER_H__

namespace printemps {
namespace model {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class Model;
}  // namespace model
}  // namespace printemps

#include "problem_size_reducer.h"
#include "selection_extractor.h"
#include "dependent_intermediate_variable_extractor.h"
#include "gf2_solver.h"
#include "flippable_variable_pair_extractor.h"

#endif
/*****************************************************************************/
// END
/*****************************************************************************/