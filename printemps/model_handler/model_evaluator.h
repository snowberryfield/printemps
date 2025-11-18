/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_MODEL_HANDLER_MODEL_EVALUATOR_H__
#define PRINTEMPS_MODEL_HANDLER_MODEL_EVALUATOR_H__

namespace printemps::model_handler {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class ModelEvaluator {
   private:
    const model::Model<T_Variable, T_Expression> *m_model_ptr;

   public:
    /*************************************************************************/
    ModelEvaluator(void) {
        m_model_ptr = nullptr;
    }

    /*************************************************************************/
    ModelEvaluator(const model::Model<T_Variable, T_Expression> *a_model_ptr) {
        this->initialize();
        this->setup(a_model_ptr);
    }

    /*************************************************************************/
    inline void initialize(void) noexcept {
        m_model_ptr = nullptr;
    }

    /*************************************************************************/
    inline void setup(
        const model::Model<T_Variable, T_Expression> *a_model_ptr) noexcept {
        m_model_ptr = a_model_ptr;
    }

    /*************************************************************************/
    inline solution::SolutionScore evaluate(
        const neighborhood::Move<T_Variable, T_Expression> &a_MOVE)
        const noexcept {
        solution::SolutionScore score;
        this->evaluate(&score, a_MOVE);
        return score;
    }

    /*************************************************************************/
    inline solution::SolutionScore evaluate(
        const neighborhood::Move<T_Variable, T_Expression> &a_MOVE,
        const solution::SolutionScore &a_CURRENT_SCORE) const noexcept {
        solution::SolutionScore score;
        if (a_MOVE.alterations.size() == 1) {
            this->evaluate_single(&score, a_MOVE, a_CURRENT_SCORE);
        } else {
            this->evaluate_multi(&score, a_MOVE, a_CURRENT_SCORE);
        }
        return score;
    }

    /*************************************************************************/
    inline void evaluate(solution::SolutionScore *a_score_ptr,  //
                         const neighborhood::Move<T_Variable, T_Expression>
                             &a_MOVE) const noexcept {
        const auto &MODEL = *m_model_ptr;

        double total_violation = 0.0;
        double local_penalty   = 0.0;

        const int CONSTRAINT_PROXIES_SIZE   = MODEL.m_constraint_proxies.size();
        bool      is_feasibility_improvable = false;

        for (auto i = 0; i < CONSTRAINT_PROXIES_SIZE; i++) {
            auto &constraints =
                MODEL.m_constraint_proxies[i].flat_indexed_constraints();

            const int CONSTRAINTS_SIZE = constraints.size();
            for (auto j = 0; j < CONSTRAINTS_SIZE; j++) {
                if (!constraints[j].is_enabled()) {
                    continue;
                }
                double constraint_value =
                    constraints[j].evaluate_constraint(a_MOVE);
                double positive_part = std::max(constraint_value, 0.0);
                double negative_part = std::max(-constraint_value, 0.0);
                double violation     = 0.0;

                if (constraints[j].is_less_or_equal()) {
                    violation = positive_part;
                    total_violation += violation;
                    if (violation + constant::EPSILON <
                        constraints[j].positive_part()) {
                        is_feasibility_improvable = true;
                    }
                    local_penalty +=
                        violation *
                        constraints[j].local_penalty_coefficient_less();
                }

                if (constraints[j].is_greater_or_equal()) {
                    violation = negative_part;
                    total_violation += violation;
                    if (violation + constant::EPSILON <
                        constraints[j].negative_part()) {
                        is_feasibility_improvable = true;
                    }
                    local_penalty +=
                        violation *
                        constraints[j].local_penalty_coefficient_greater();
                }
            }
        }

        const double OBJECTIVE =
            MODEL.m_is_defined_objective
                ? MODEL.m_objective.evaluate(a_MOVE) * MODEL.sign()
                : 0.0;

        const double OBJECTIVE_IMPROVEMENT =
            MODEL.m_objective.value() * MODEL.sign() - OBJECTIVE;

        const double GLOBAL_PENALTY =
            total_violation * MODEL.m_global_penalty_coefficient;

        a_score_ptr->objective                  = OBJECTIVE;
        a_score_ptr->objective_improvement      = OBJECTIVE_IMPROVEMENT;
        a_score_ptr->total_violation            = total_violation;
        a_score_ptr->local_penalty              = local_penalty;
        a_score_ptr->global_penalty             = GLOBAL_PENALTY;
        a_score_ptr->local_augmented_objective  = OBJECTIVE + local_penalty;
        a_score_ptr->global_augmented_objective = OBJECTIVE + GLOBAL_PENALTY;
        a_score_ptr->is_feasible = !(total_violation > constant::EPSILON);
        a_score_ptr->is_objective_improvable =
            OBJECTIVE_IMPROVEMENT > constant::EPSILON;
        a_score_ptr->is_feasibility_improvable = is_feasibility_improvable;
    }

    /*************************************************************************/
    inline void evaluate_single(
        solution::SolutionScore                            *a_score_ptr,  //
        const neighborhood::Move<T_Variable, T_Expression> &a_MOVE,
        const solution::SolutionScore &a_CURRENT_SCORE) const noexcept {
        const auto &MODEL = *m_model_ptr;

        double total_violation  = a_CURRENT_SCORE.total_violation;
        double local_penalty    = a_CURRENT_SCORE.local_penalty;
        double constraint_value = 0.0;
        double violation_diff   = 0.0;

        const auto &variable_ptr = a_MOVE.alterations.front().first;
        const auto  variable_value_diff =
            a_MOVE.alterations.front().second - variable_ptr->value();
        const auto &constraint_sensitivities =
            variable_ptr->constraint_sensitivities();

        for (const auto &sensitivity : constraint_sensitivities) {
            const auto &constraint_ptr = sensitivity.first;
            if (constraint_ptr->is_evaluation_ignorable()) {
                continue;
            }
            constraint_value = constraint_ptr->constraint_value() +
                               sensitivity.second * variable_value_diff;

            if (constraint_ptr->is_less_or_equal()) {
                total_violation +=
                    (violation_diff = std::max(constraint_value, 0.0) -
                                      constraint_ptr->positive_part());

                local_penalty +=
                    violation_diff *
                    constraint_ptr->local_penalty_coefficient_less();
            }

            if (constraint_ptr->is_greater_or_equal()) {
                total_violation -=
                    (violation_diff = std::min(constraint_value, 0.0) +
                                      constraint_ptr->negative_part());

                local_penalty -=
                    violation_diff *
                    constraint_ptr->local_penalty_coefficient_greater();
            }
        }

        const double OBJECTIVE =
            MODEL.m_is_defined_objective
                ? MODEL.m_objective.evaluate(a_MOVE) * MODEL.sign()
                : 0.0;

        const double OBJECTIVE_IMPROVEMENT =
            MODEL.m_objective.value() * MODEL.sign() - OBJECTIVE;

        const double GLOBAL_PENALTY =
            total_violation * MODEL.m_global_penalty_coefficient;

        a_score_ptr->objective                  = OBJECTIVE;
        a_score_ptr->objective_improvement      = OBJECTIVE_IMPROVEMENT;
        a_score_ptr->total_violation            = total_violation;
        a_score_ptr->local_penalty              = local_penalty;
        a_score_ptr->global_penalty             = GLOBAL_PENALTY;
        a_score_ptr->local_augmented_objective  = OBJECTIVE + local_penalty;
        a_score_ptr->global_augmented_objective = OBJECTIVE + GLOBAL_PENALTY;
        a_score_ptr->is_feasible = !(total_violation > constant::EPSILON);
        a_score_ptr->is_objective_improvable =
            OBJECTIVE_IMPROVEMENT > constant::EPSILON;
        a_score_ptr->is_feasibility_improvable = true;  /// do not care.
    }

    /*************************************************************************/
    inline void evaluate_single_no_ignore(
        solution::SolutionScore                            *a_score_ptr,  //
        const neighborhood::Move<T_Variable, T_Expression> &a_MOVE,
        const solution::SolutionScore &a_CURRENT_SCORE) const noexcept {
        const auto &MODEL = *m_model_ptr;

        double total_violation  = a_CURRENT_SCORE.total_violation;
        double local_penalty    = a_CURRENT_SCORE.local_penalty;
        double constraint_value = 0.0;
        double violation_diff   = 0.0;

        const auto &variable_ptr = a_MOVE.alterations.front().first;
        const auto  variable_value_diff =
            a_MOVE.alterations.front().second - variable_ptr->value();
        const auto &constraint_sensitivities =
            variable_ptr->constraint_sensitivities();

        for (const auto &sensitivity : constraint_sensitivities) {
            /**
             * NOTE: The difference from evaluate_single() is that this method
             * does not skip the evaluation of constraint function values based
             * on the value of constraint_ptr->is_evaluation_ignorable(). In the
             * local_search method that calls this function, the final move is
             * constructed by combining multiple moves evaluated by this method.
             * For such combined moves,constraint_ptr->is_evaluation_ignorable()
             * does not function correctly.
             */
            const auto &constraint_ptr = sensitivity.first;
            if (!constraint_ptr->is_enabled()) {
                continue;
            }
            constraint_value = constraint_ptr->constraint_value() +
                               sensitivity.second * variable_value_diff;

            if (constraint_ptr->is_less_or_equal()) {
                total_violation +=
                    (violation_diff = std::max(constraint_value, 0.0) -
                                      constraint_ptr->positive_part());

                local_penalty +=
                    violation_diff *
                    constraint_ptr->local_penalty_coefficient_less();
            }

            if (constraint_ptr->is_greater_or_equal()) {
                total_violation -=
                    (violation_diff = std::min(constraint_value, 0.0) +
                                      constraint_ptr->negative_part());

                local_penalty -=
                    violation_diff *
                    constraint_ptr->local_penalty_coefficient_greater();
            }
        }

        const double OBJECTIVE =
            MODEL.m_is_defined_objective
                ? MODEL.m_objective.evaluate(a_MOVE) * MODEL.sign()
                : 0.0;

        const double OBJECTIVE_IMPROVEMENT =
            MODEL.m_objective.value() * MODEL.sign() - OBJECTIVE;

        const double GLOBAL_PENALTY =
            total_violation * MODEL.m_global_penalty_coefficient;

        a_score_ptr->objective                  = OBJECTIVE;
        a_score_ptr->objective_improvement      = OBJECTIVE_IMPROVEMENT;
        a_score_ptr->total_violation            = total_violation;
        a_score_ptr->local_penalty              = local_penalty;
        a_score_ptr->global_penalty             = GLOBAL_PENALTY;
        a_score_ptr->local_augmented_objective  = OBJECTIVE + local_penalty;
        a_score_ptr->global_augmented_objective = OBJECTIVE + GLOBAL_PENALTY;
        a_score_ptr->is_feasible = !(total_violation > constant::EPSILON);
        a_score_ptr->is_objective_improvable =
            OBJECTIVE_IMPROVEMENT > constant::EPSILON;
        a_score_ptr->is_feasibility_improvable = true;  /// do not care.
    }

    /*************************************************************************/
    inline void evaluate_selection(
        solution::SolutionScore                            *a_score_ptr,  //
        const neighborhood::Move<T_Variable, T_Expression> &a_MOVE,
        const solution::SolutionScore &a_CURRENT_SCORE) const noexcept {
        const auto &MODEL = *m_model_ptr;

        bool is_feasibility_improvable = false;

        double total_violation  = a_CURRENT_SCORE.total_violation;
        double local_penalty    = a_CURRENT_SCORE.local_penalty;
        double constraint_value = 0.0;
        double violation_diff   = 0.0;

        const auto &RELATED_CONSTRAINT_PTRS = a_MOVE.alterations.front()
                                                  .first->selection_ptr()
                                                  ->related_constraint_ptrs;

        const auto &INDEX_MIN_FIRST =
            a_MOVE.alterations[0]
                .first->related_selection_constraint_ptr_index_min();
        const auto &INDEX_MAX_FIRST =
            a_MOVE.alterations[0]
                .first->related_selection_constraint_ptr_index_max();
        const auto &INDEX_MIN_SECOND =
            a_MOVE.alterations[1]
                .first->related_selection_constraint_ptr_index_min();
        const auto &INDEX_MAX_SECOND =
            a_MOVE.alterations[1]
                .first->related_selection_constraint_ptr_index_max();

        int index_min = -1;
        if (INDEX_MIN_FIRST == -1 && INDEX_MIN_SECOND >= 0) {
            index_min = INDEX_MIN_SECOND;
        } else if (INDEX_MIN_FIRST >= 0 && INDEX_MIN_SECOND == -1) {
            index_min = INDEX_MIN_FIRST;
        } else if (INDEX_MIN_FIRST >= 0 && INDEX_MIN_SECOND >= 0) {
            index_min = std::min(INDEX_MIN_FIRST, INDEX_MIN_SECOND);
        }

        int index_max = -1;
        if (INDEX_MAX_FIRST == -1 && INDEX_MAX_SECOND >= 0) {
            index_max = INDEX_MAX_SECOND;
        } else if (INDEX_MAX_FIRST >= 0 && INDEX_MAX_SECOND == -1) {
            index_max = INDEX_MAX_FIRST;
        } else if (INDEX_MAX_FIRST >= 0 && INDEX_MAX_SECOND >= 0) {
            index_max = std::max(INDEX_MAX_FIRST, INDEX_MAX_SECOND);
        }
        if (index_min >= 0 && index_max >= 0) {
            if ((INDEX_MAX_FIRST < INDEX_MIN_SECOND) ||
                (INDEX_MAX_SECOND < INDEX_MIN_FIRST)) {
                for (const auto &alteration : a_MOVE.alterations) {
                    const auto &variable_ptr = alteration.first;
                    const auto  variable_value_diff =
                        alteration.second - variable_ptr->value();
                    const auto &constraint_sensitivities =
                        variable_ptr->constraint_sensitivities();

                    for (const auto &sensitivity : constraint_sensitivities) {
                        const auto &constraint_ptr = sensitivity.first;
                        if (!constraint_ptr->is_enabled()) {
                            continue;
                        }
                        constraint_value =
                            constraint_ptr->constraint_value() +
                            sensitivity.second * variable_value_diff;

                        if (constraint_ptr->is_less_or_equal()) {
                            total_violation +=
                                (violation_diff =
                                     std::max(constraint_value, 0.0) -
                                     constraint_ptr->positive_part());

                            local_penalty +=
                                violation_diff *
                                constraint_ptr
                                    ->local_penalty_coefficient_less();
                        }

                        if (constraint_ptr->is_greater_or_equal()) {
                            total_violation -=
                                (violation_diff =
                                     std::min(constraint_value, 0.0) +
                                     constraint_ptr->negative_part());

                            local_penalty -=
                                violation_diff *
                                constraint_ptr
                                    ->local_penalty_coefficient_greater();
                        }
                    }
                }
                is_feasibility_improvable = true;
            } else {
                double violation_diff_negative = 0.0;
                double violation_diff_positive = 0.0;

                for (auto i = index_min; i <= index_max; i++) {
                    auto &constraint_ptr = RELATED_CONSTRAINT_PTRS[i];
                    if (!constraint_ptr->is_enabled()) {
                        continue;
                    }

                    constraint_value =
                        constraint_ptr->evaluate_constraint(a_MOVE);

                    if (std::fabs(constraint_value -
                                  constraint_ptr->constraint_value()) <
                        constant::EPSILON_10) {
                        continue;
                    }

                    violation_diff_positive =
                        constraint_ptr->is_less_or_equal()
                            ? std::max(constraint_value, 0.0) -
                                  constraint_ptr->positive_part()
                            : 0.0;

                    violation_diff_negative =
                        constraint_ptr->is_greater_or_equal()
                            ? std::max(-constraint_value, 0.0) -
                                  constraint_ptr->negative_part()
                            : 0.0;
                    violation_diff =
                        violation_diff_positive + violation_diff_negative;
                    local_penalty +=
                        violation_diff_positive *
                            constraint_ptr->local_penalty_coefficient_less() +
                        violation_diff_negative *
                            constraint_ptr->local_penalty_coefficient_greater();

                    total_violation += violation_diff;
                    is_feasibility_improvable |=
                        violation_diff < -constant::EPSILON;
                }
            }
        }

        const double OBJECTIVE =
            MODEL.m_is_defined_objective
                ? MODEL.m_objective.evaluate(a_MOVE) * MODEL.sign()
                : 0.0;

        const double OBJECTIVE_IMPROVEMENT =
            MODEL.m_objective.value() * MODEL.sign() - OBJECTIVE;

        const double GLOBAL_PENALTY =
            total_violation * MODEL.m_global_penalty_coefficient;

        a_score_ptr->objective                  = OBJECTIVE;
        a_score_ptr->objective_improvement      = OBJECTIVE_IMPROVEMENT;
        a_score_ptr->total_violation            = total_violation;
        a_score_ptr->local_penalty              = local_penalty;
        a_score_ptr->global_penalty             = GLOBAL_PENALTY;
        a_score_ptr->local_augmented_objective  = OBJECTIVE + local_penalty;
        a_score_ptr->global_augmented_objective = OBJECTIVE + GLOBAL_PENALTY;
        a_score_ptr->is_feasible = !(total_violation > constant::EPSILON);
        a_score_ptr->is_objective_improvable =
            OBJECTIVE_IMPROVEMENT > constant::EPSILON;
        a_score_ptr->is_feasibility_improvable = is_feasibility_improvable;
    }

    /*************************************************************************/
    inline void evaluate_multi(
        solution::SolutionScore                            *a_score_ptr,  //
        const neighborhood::Move<T_Variable, T_Expression> &a_MOVE,
        const solution::SolutionScore &a_CURRENT_SCORE) const noexcept {
        const auto &MODEL = *m_model_ptr;

        bool is_feasibility_improvable = false;

        double total_violation         = a_CURRENT_SCORE.total_violation;
        double local_penalty           = a_CURRENT_SCORE.local_penalty;
        double constraint_value        = 0.0;
        double violation_diff_negative = 0.0;
        double violation_diff_positive = 0.0;
        double violation_diff          = 0.0;

        for (const auto &constraint_ptr : a_MOVE.related_constraint_ptrs) {
            if (!constraint_ptr->is_enabled()) {
                continue;
            }

            constraint_value = constraint_ptr->evaluate_constraint(a_MOVE);

            if (std::fabs(constraint_value -
                          constraint_ptr->constraint_value()) <
                constant::EPSILON_10) {
                continue;
            }

            violation_diff_positive = constraint_ptr->is_less_or_equal()
                                          ? std::max(constraint_value, 0.0) -
                                                constraint_ptr->positive_part()
                                          : 0.0;

            violation_diff_negative = constraint_ptr->is_greater_or_equal()
                                          ? std::max(-constraint_value, 0.0) -
                                                constraint_ptr->negative_part()
                                          : 0.0;

            violation_diff = violation_diff_positive + violation_diff_negative;
            local_penalty +=
                violation_diff_positive *
                    constraint_ptr->local_penalty_coefficient_less() +
                violation_diff_negative *
                    constraint_ptr->local_penalty_coefficient_greater();

            total_violation += violation_diff;
            is_feasibility_improvable |= violation_diff < -constant::EPSILON;
        }

        const double OBJECTIVE =
            MODEL.m_is_defined_objective
                ? MODEL.m_objective.evaluate(a_MOVE) * MODEL.sign()
                : 0.0;

        const double OBJECTIVE_IMPROVEMENT =
            MODEL.m_objective.value() * MODEL.sign() - OBJECTIVE;

        const double GLOBAL_PENALTY =
            total_violation * MODEL.m_global_penalty_coefficient;

        a_score_ptr->objective                  = OBJECTIVE;
        a_score_ptr->objective_improvement      = OBJECTIVE_IMPROVEMENT;
        a_score_ptr->total_violation            = total_violation;
        a_score_ptr->local_penalty              = local_penalty;
        a_score_ptr->global_penalty             = GLOBAL_PENALTY;
        a_score_ptr->local_augmented_objective  = OBJECTIVE + local_penalty;
        a_score_ptr->global_augmented_objective = OBJECTIVE + GLOBAL_PENALTY;
        a_score_ptr->is_feasible = !(total_violation > constant::EPSILON);
        a_score_ptr->is_objective_improvable =
            OBJECTIVE_IMPROVEMENT > constant::EPSILON;
        a_score_ptr->is_feasibility_improvable = is_feasibility_improvable;
    }

    /*************************************************************************/
    inline double compute_lagrangian(
        const std::vector<multi_array::ValueProxy<double>>
            &a_LAGRANGE_MULTIPLIER_PROXIES) const noexcept {
        const auto &MODEL = *m_model_ptr;

        double lagrangian = MODEL.m_objective.value();

        for (auto &&constraint_ptr :
             MODEL.m_reference.constraint.constraint_ptrs) {
            const int PROXY_INDEX = constraint_ptr->proxy_index();
            const int FLAT_INDEX  = constraint_ptr->flat_index();

            lagrangian +=
                a_LAGRANGE_MULTIPLIER_PROXIES[PROXY_INDEX].flat_indexed_values(
                    FLAT_INDEX) *
                constraint_ptr->constraint_value();
        }
        return lagrangian;
    }

    /*************************************************************************/
    inline double compute_naive_dual_bound(void) const noexcept {
        const auto &MODEL      = *m_model_ptr;
        double      dual_bound = MODEL.m_is_minimization
                                     ? std::numeric_limits<double>::lowest()
                                     : std::numeric_limits<double>::max();

        dual_bound = MODEL.m_objective.expression().constant_value();
        for (auto &&sensitivity :
             MODEL.m_objective.expression().sensitivities()) {
            const auto VARIABLE_PTR = sensitivity.first;
            const auto COEFFICIENT  = sensitivity.second;
            if (VARIABLE_PTR->is_fixed()) {
                dual_bound += VARIABLE_PTR->value() * COEFFICIENT;
            } else {
                const auto LOWER_BOUND = VARIABLE_PTR->lower_bound();
                const auto UPPER_BOUND = VARIABLE_PTR->upper_bound();
                if (MODEL.m_is_minimization == (COEFFICIENT > 0)) {
                    dual_bound += LOWER_BOUND * COEFFICIENT;
                } else {
                    dual_bound += UPPER_BOUND * COEFFICIENT;
                }
            }
        }
        return dual_bound;
    }
};
}  // namespace printemps::model_handler
#endif
/*****************************************************************************/
// END
/*****************************************************************************/