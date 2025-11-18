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
#include "model_component_creator.h"
#include "model_builder.h"
#include "model_evaluator.h"
#include "model_updater.h"
#include "model_initial_solution_handler.h"
#include "model_json_handler.h"
#include "model_linear_programming_handler.h"
#include "model_mps_handler.h"
#include "model_opb_handler.h"
#include "model_printer.h"
#include "model_state_inspector.h"

#endif
/*****************************************************************************/
// END
/*****************************************************************************/