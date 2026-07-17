/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_INTEGER_STEP_SIZE_ADJUSTER_H__
#define PRINTEMPS_SOLVER_INTEGER_STEP_SIZE_ADJUSTER_H__

namespace printemps::solver {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class IntegerStepSizeAdjuster {
   private:
    model::Model<T_Variable, T_Expression> *m_model_ptr;
    option::Option                          m_option;

   public:
    /*************************************************************************/
    IntegerStepSizeAdjuster(model::Model<T_Variable, T_Expression> *a_model_ptr,
                            const option::Option                   &a_OPTION) {
        this->setup(a_model_ptr, a_OPTION);
    }

    /*************************************************************************/
    IntegerStepSizeAdjuster(void) {
        this->initialize();
    }

    /*************************************************************************/
    void initialize(void) {
        m_model_ptr = nullptr;
        m_option.initialize();
    }

    /*************************************************************************/
    void setup(model::Model<T_Variable, T_Expression> *a_model_ptr,
               const option::Option                   &a_OPTION) {
        m_model_ptr = a_model_ptr;
        m_option    = a_OPTION;
    }

    /*************************************************************************/
    inline void adjust(
        neighborhood::Move<T_Variable, T_Expression> *a_move_ptr,
        const solution::SolutionScore &a_REFERENCE_SOLUTION_SCORE) const {
        auto     &variable_ptr   = a_move_ptr->alterations.back().first;
        const int ORIGINAL       = variable_ptr->value();
        const int INITIAL_TARGET = a_move_ptr->alterations.back().second;
        const int DIRECTION      = INITIAL_TARGET - ORIGINAL > 0 ? 1 : -1;
        auto      move_trial     = *a_move_ptr;
        int       step_size      = 2;

        if (DIRECTION > 0 && INITIAL_TARGET == variable_ptr->upper_bound()) {
            return;
        }
        if (DIRECTION < 0 && INITIAL_TARGET == variable_ptr->lower_bound()) {
            return;
        }

        solution::SolutionScore solution_score_trial;
        const auto             &EVALUATOR = m_model_ptr->evaluator();

        EVALUATOR.evaluate_multi(&solution_score_trial, move_trial,
                                 a_REFERENCE_SOLUTION_SCORE);

        int    target_candidate = INITIAL_TARGET;
        double score_min = solution_score_trial.global_augmented_objective;

        int lower_bound = ORIGINAL;
        int upper_bound = ORIGINAL;

        int target = ORIGINAL + DIRECTION * step_size;

        while (true) {
            move_trial.alterations.back().second = target;
            EVALUATOR.evaluate_multi(&solution_score_trial, move_trial,
                                     a_REFERENCE_SOLUTION_SCORE);

            if (solution_score_trial.global_augmented_objective < score_min) {
                if (DIRECTION > 0) {
                    lower_bound = std::max(lower_bound, target);
                } else {
                    upper_bound = std::min(upper_bound, target);
                }
                score_min = solution_score_trial.global_augmented_objective;
                target_candidate = target;

                step_size *= 2;
                target = ORIGINAL + DIRECTION * step_size;
            } else {
                if (DIRECTION > 0) {
                    upper_bound = target;
                } else {
                    lower_bound = target;
                }
                break;
            }

            if (target <= variable_ptr->lower_bound()) {
                lower_bound = variable_ptr->lower_bound();
                break;
            } else if (target >= variable_ptr->upper_bound()) {
                upper_bound = variable_ptr->upper_bound();
                break;
            }
        }

        while (upper_bound - lower_bound > 1) {
            target = (lower_bound + upper_bound) / 2;
            move_trial.alterations.back().second = target;
            EVALUATOR.evaluate_multi(&solution_score_trial, move_trial,
                                     a_REFERENCE_SOLUTION_SCORE);

            if (solution_score_trial.global_augmented_objective < score_min) {
                if (DIRECTION > 0) {
                    lower_bound = target;
                } else {
                    upper_bound = target;
                }
                score_min = solution_score_trial.global_augmented_objective;
                target_candidate = target;

            } else {
                if (DIRECTION > 0) {
                    upper_bound = target;
                } else {
                    lower_bound = target;
                }
            }
        }

        a_move_ptr->alterations.back().second = target_candidate;
    }
};
}  // namespace printemps::solver

#endif
/*****************************************************************************/
// END
/*****************************************************************************/
