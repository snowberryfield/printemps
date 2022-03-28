/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLUTION_SOLUTION_SCORE_H__
#define PRINTEMPS_SOLUTION_SOLUTION_SCORE_H__

namespace printemps {
namespace solution {
/*****************************************************************************/
struct SolutionScore {
    double objective             = 0.0;
    double objective_improvement = 0.0;
    double total_violation       = 0.0;

    double local_penalty              = 0.0;
    double global_penalty             = 0.0;
    double local_augmented_objective  = 0.0;
    double global_augmented_objective = 0.0;

    bool is_feasible               = false;
    bool is_objective_improvable   = false;
    bool is_feasibility_improvable = false;
};

/*****************************************************************************/
inline int argmax_index_objective_improvement(
    const std::vector<SolutionScore> &a_SCORES) {
    int       argmax      = 0;
    double    max         = a_SCORES[0].objective_improvement;
    const int SCORES_SIZE = a_SCORES.size();
    for (auto i = 1; i < SCORES_SIZE; i++) {
        if (a_SCORES[i].objective_improvement > max) {
            argmax = i;
            max    = a_SCORES[i].objective_improvement;
        }
    }
    return argmax;
}

/*****************************************************************************/
inline int argmin_index_objective(const std::vector<SolutionScore> &a_SCORES) {
    int       argmin      = 0;
    double    min         = a_SCORES[0].objective;
    const int SCORES_SIZE = a_SCORES.size();
    for (auto i = 1; i < SCORES_SIZE; i++) {
        if (a_SCORES[i].objective < min) {
            argmin = i;
            min    = a_SCORES[i].objective;
        }
    }
    return argmin;
}

/*****************************************************************************/
inline int argmin_index_local_penalty(
    const std::vector<SolutionScore> &a_SCORES) {
    int       argmin      = 0;
    double    min         = a_SCORES[0].local_penalty;
    const int SCORES_SIZE = a_SCORES.size();

    for (auto i = 1; i < SCORES_SIZE; i++) {
        if (a_SCORES[i].local_penalty < min) {
            argmin = i;
            min    = a_SCORES[i].local_penalty;
        }
    }
    return argmin;
}

/*****************************************************************************/
inline int argmin_index_global_penalty(
    const std::vector<SolutionScore> &a_SCORES) {
    int       argmin      = 0;
    double    min         = a_SCORES[0].global_penalty;
    const int SCORES_SIZE = a_SCORES.size();
    for (auto i = 1; i < SCORES_SIZE; i++) {
        if (a_SCORES[i].global_penalty < min) {
            argmin = i;
            min    = a_SCORES[i].global_penalty;
        }
    }
    return argmin;
}

/*****************************************************************************/
inline int argmin_index_local_augmented_objective(
    const std::vector<SolutionScore> &a_SCORES) {
    int       argmin      = 0;
    double    min         = a_SCORES[0].local_augmented_objective;
    const int SCORES_SIZE = a_SCORES.size();
    for (auto i = 1; i < SCORES_SIZE; i++) {
        if (a_SCORES[i].local_augmented_objective < min) {
            argmin = i;
            min    = a_SCORES[i].local_augmented_objective;
        }
    }
    return argmin;
}

/*****************************************************************************/
inline int argmin_index_global_augmented_objective(
    const std::vector<SolutionScore> &a_SCORES) {
    int       argmin      = 0;
    double    min         = a_SCORES[0].global_augmented_objective;
    const int SCORES_SIZE = a_SCORES.size();
    for (auto i = 1; i < SCORES_SIZE; i++) {
        if (a_SCORES[i].global_augmented_objective < min) {
            argmin = i;
            min    = a_SCORES[i].global_augmented_objective;
        }
    }
    return argmin;
}

}  // namespace solution
}  // namespace printemps
#endif
/*****************************************************************************/
// END
/*****************************************************************************/