/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_MODEL_SOLUTION_SCORE_H__
#define PRINTEMPS_MODEL_SOLUTION_SCORE_H__

#include <vector>
#include "../utility/utility.h"

namespace printemps {
namespace model {
/*****************************************************************************/
struct SolutionScore {
    double objective;
    double objective_improvement;
    double total_violation;

    double local_penalty;
    double global_penalty;
    double local_augmented_objective;
    double global_augmented_objective;

    bool is_feasible;
    bool is_objective_improvable;
    bool is_constraint_improvable;
};

/*****************************************************************************/
inline int argmax_index_objective_improvement(
    const std::vector<SolutionScore> &a_SCORES) {
    std::vector<double> objective_improvements(a_SCORES.size());

    int scores_size = a_SCORES.size();
    for (auto i_score = 0; i_score < scores_size; i_score++) {
        objective_improvements[i_score] =
            a_SCORES[i_score].objective_improvement;
    }
    return utility::argmax(objective_improvements);
}

/*****************************************************************************/
inline int argmin_index_objective(const std::vector<SolutionScore> &a_SCORES) {
    std::vector<double> objectives(a_SCORES.size());

    int scores_size = a_SCORES.size();
    for (auto i_score = 0; i_score < scores_size; i_score++) {
        objectives[i_score] = a_SCORES[i_score].objective;
    }
    return utility::argmin(objectives);
}

/*****************************************************************************/
inline int argmin_index_local_penalty(
    const std::vector<SolutionScore> &a_SCORES) {
    std::vector<double> local_penalties(a_SCORES.size());

    int scores_size = a_SCORES.size();
    for (auto i_score = 0; i_score < scores_size; i_score++) {
        local_penalties[i_score] = a_SCORES[i_score].local_penalty;
    }
    return utility::argmin(local_penalties);
}

/*****************************************************************************/
inline int argmin_index_global_penalty(
    const std::vector<SolutionScore> &a_SCORES) {
    std::vector<double> global_penalties(a_SCORES.size());

    int scores_size = a_SCORES.size();
    for (auto i_score = 0; i_score < scores_size; i_score++) {
        global_penalties[i_score] = a_SCORES[i_score].global_penalty;
    }
    return utility::argmin(global_penalties);
}

/*****************************************************************************/
inline int argmin_index_local_augmented_objective(
    const std::vector<SolutionScore> &a_SCORES) {
    std::vector<double> local_augmented_objectives(a_SCORES.size());

    int scores_size = a_SCORES.size();
    for (auto i_score = 0; i_score < scores_size; i_score++) {
        local_augmented_objectives[i_score] =
            a_SCORES[i_score].local_augmented_objective;
    }
    return utility::argmin(local_augmented_objectives);
}

/*****************************************************************************/
inline int argmin_index_global_augmented_objective(
    const std::vector<SolutionScore> &a_SCORES) {
    std::vector<double> global_augmented_objectives(a_SCORES.size());

    int scores_size = a_SCORES.size();
    for (auto i_score = 0; i_score < scores_size; i_score++) {
        global_augmented_objectives[i_score] =
            a_SCORES[i_score].global_augmented_objective;
    }
    return utility::argmin(global_augmented_objectives);
}

}  // namespace model
}  // namespace printemps
#endif
/*****************************************************************************/
// END
/*****************************************************************************/