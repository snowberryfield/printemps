/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_MODEL_HANDLER_MODEL_HANDLER_H__
#define PRINTEMPS_MODEL_HANDLER_MODEL_HANDLER_H__

namespace printemps::model {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class Model;
}  // namespace printemps::model

#include "reference.h"
#include "component_creator.h"
#include "builder.h"
#include "evaluator.h"
#include "updater.h"
#include "initial_solution_handler.h"
#include "json_handler.h"
#include "matrix_model_handler.h"
#include "mps_handler.h"
#include "opb_handler.h"
#include "wcnf_handler.h"
#include "printer.h"
#include "state_inspector.h"
#include "inspector.h"

#endif
/*****************************************************************************/
// END
/*****************************************************************************/