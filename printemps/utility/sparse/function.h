/*****************************************************************************/
// Copyright (c) 2020-2023 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_UTILITY_SPARSE_FUNCTION_H__
#define PRINTEMPS_UTILITY_SPARSE_FUNCTION_H__

namespace printemps::utility::sparse {
/*****************************************************************************/
inline double distance(const Vector& a_FIRST_VECTOR,
                       const Vector& a_SECOND_VECTOR) {
    const int DIMENSION = a_FIRST_VECTOR.size();
    double    sum       = 0.0;

    for (auto i = 0; i < DIMENSION; i++) {
        const double DIFF = a_FIRST_VECTOR[i] - a_SECOND_VECTOR[i];
        sum += DIFF * DIFF;
    }
    return std::sqrt(sum);
}

/*****************************************************************************/
inline void sum(Vector* a_result_ptr, const Vector& a_FIRST,
                const Vector& a_SECOND) {
    const int SIZE = a_FIRST.size();

#ifdef _OPENMP
#pragma omp parallel for schedule(guided)
#endif
    for (auto i = 0; i < SIZE; i++) {
        (*a_result_ptr)[i] = a_FIRST[i] + a_SECOND[i];
    }
}

/*****************************************************************************/
inline void subtract(Vector* a_result_ptr, const Vector& a_FIRST,
                     const Vector& a_SECOND) {
    const int SIZE = a_FIRST.size();

#ifdef _OPENMP
#pragma omp parallel for schedule(guided)
#endif
    for (auto i = 0; i < SIZE; i++) {
        (*a_result_ptr)[i] = a_FIRST[i] - a_SECOND[i];
    }
}

/*****************************************************************************/
inline void linear_combination(Vector*       a_result_ptr,
                               const double  a_FIRST_COEFFICIENT,
                               const Vector& a_FIRST_VECTOR,
                               const double  a_SECOND_COEFFICIENT,
                               const Vector& a_SECOND_VECTOR) {
    const int SIZE = a_FIRST_VECTOR.size();

#ifdef _OPENMP
#pragma omp parallel for schedule(guided)
#endif
    for (auto i = 0; i < SIZE; i++) {
        (*a_result_ptr)[i] = a_FIRST_COEFFICIENT * a_FIRST_VECTOR[i] +
                             a_SECOND_COEFFICIENT * a_SECOND_VECTOR[i];
    }
}

/*****************************************************************************/
inline void linear_combination(Vector*       a_result_ptr,
                               const double  a_FIRST_COEFFICIENT,
                               const Vector& a_FIRST_VECTOR,
                               const double  a_SECOND_COEFFICIENT,
                               const Vector& a_SECOND_VECTOR,
                               const double  a_THIRD_COEFFICIENT,
                               const Vector& a_THIRD_VECTOR) {
    const int SIZE = a_FIRST_VECTOR.size();

#ifdef _OPENMP
#pragma omp parallel for schedule(guided)
#endif
    for (auto i = 0; i < SIZE; i++) {
        (*a_result_ptr)[i] = a_FIRST_COEFFICIENT * a_FIRST_VECTOR[i] +
                             a_SECOND_COEFFICIENT * a_SECOND_VECTOR[i] +
                             a_THIRD_COEFFICIENT * a_THIRD_VECTOR[i];
    }
}

/*****************************************************************************/
inline void linear_combination(Vector*                     a_result_ptr,
                               const std::vector<double>   a_COEFFICIENTS,
                               const std::vector<Vector*>& a_VECTOR_PTRS) {
    const int NUMBER_OF_TERMS = a_VECTOR_PTRS.size();
    const int SIZE            = a_VECTOR_PTRS.front()->size();

#ifdef _OPENMP
#pragma omp parallel for schedule(guided)
#endif
    for (auto i = 0; i < SIZE; i++) {
        (*a_result_ptr)[i] = 0.0;
        for (auto j = 0; j < NUMBER_OF_TERMS; j++) {
            (*a_result_ptr)[i] += a_COEFFICIENTS[j] * (*a_VECTOR_PTRS[j])[i];
        }
    }
}

/*****************************************************************************/
inline void linear_combination(Vector*             a_result_ptr,
                               const double        a_FIRST_COEFFICIENT,
                               const SparseMatrix& a_FIRST_MATRIX,
                               const Vector&       a_FIRST_VECTOR,
                               const double        a_SECOND_COEFFICIENT,
                               const Vector&       a_SECOND_VECTOR) {
    const auto& VALUES         = a_FIRST_MATRIX.values;
    const auto& ROW_PTRS       = a_FIRST_MATRIX.row_ptrs;
    const auto& COLUMN_INDICES = a_FIRST_MATRIX.column_indices;
    const int   SIZE           = a_FIRST_MATRIX.number_of_rows;

#ifdef _OPENMP
#pragma omp parallel for schedule(guided)
#endif
    for (int i = 0; i < SIZE; i++) {
        (*a_result_ptr)[i] = a_SECOND_COEFFICIENT * a_SECOND_VECTOR[i];
        for (auto j = ROW_PTRS[i]; j < ROW_PTRS[i + 1]; j++) {
            (*a_result_ptr)[i] += a_FIRST_COEFFICIENT * VALUES[j] *
                                  a_FIRST_VECTOR[COLUMN_INDICES[j]];
        }
    }
}

/*****************************************************************************/
inline void linear_combination(
    Vector* a_result_ptr, const double a_FIRST_COEFFICIENT,
    const SparseMatrix& a_FIRST_MATRIX, const Vector& a_FIRST_VECTOR,
    const double a_SECOND_COEFFICIENT, const Vector& a_SECOND_VECTOR,
    const double a_THIRD_COEFFICIENT, const Vector& a_THIRD_VECTOR) {
    const auto& VALUES         = a_FIRST_MATRIX.values;
    const auto& ROW_PTRS       = a_FIRST_MATRIX.row_ptrs;
    const auto& COLUMN_INDICES = a_FIRST_MATRIX.column_indices;
    const int   SIZE           = a_FIRST_MATRIX.number_of_rows;

#ifdef _OPENMP
#pragma omp parallel for schedule(guided)
#endif
    for (int i = 0; i < SIZE; i++) {
        (*a_result_ptr)[i] = a_SECOND_COEFFICIENT * a_SECOND_VECTOR[i] +
                             a_THIRD_COEFFICIENT * a_THIRD_VECTOR[i];
        for (auto j = ROW_PTRS[i]; j < ROW_PTRS[i + 1]; j++) {
            (*a_result_ptr)[i] += a_FIRST_COEFFICIENT * VALUES[j] *
                                  a_FIRST_VECTOR[COLUMN_INDICES[j]];
        }
    }
}

/*****************************************************************************/
inline void linear_combination(Vector*                     a_result_ptr,
                               const double                a_FIRST_COEFFICIENT,
                               const SparseMatrix&         a_FIRST_MATRIX,
                               const Vector&               a_FIRST_VECTOR,
                               const std::vector<double>   a_REST_COEFFICIENTS,
                               const std::vector<Vector*>& a_REST_VECTOR_PTRS) {
    const auto& VALUES         = a_FIRST_MATRIX.values;
    const auto& ROW_PTRS       = a_FIRST_MATRIX.row_ptrs;
    const auto& COLUMN_INDICES = a_FIRST_MATRIX.column_indices;

    const int NUMBER_OF_REST_TERMS = a_REST_VECTOR_PTRS.size();
    const int SIZE                 = a_REST_VECTOR_PTRS.front()->size();

#ifdef _OPENMP
#pragma omp parallel for schedule(guided)
#endif
    for (int i = 0; i < SIZE; i++) {
        (*a_result_ptr)[i] = 0.0;

        for (auto j = 0; j < NUMBER_OF_REST_TERMS; j++) {
            (*a_result_ptr)[i] +=
                a_REST_COEFFICIENTS[j] * (*a_REST_VECTOR_PTRS[j])[i];
        }

        for (auto j = ROW_PTRS[i]; j < ROW_PTRS[i + 1]; j++) {
            (*a_result_ptr)[i] += a_FIRST_COEFFICIENT * VALUES[j] *
                                  a_FIRST_VECTOR[COLUMN_INDICES[j]];
        }
    }
}

/*****************************************************************************/
inline double dot_dot(Vector* a_result_ptr, const Vector& a_COLUMN_VECTOR,
                      const SparseMatrix& a_MATRIX,
                      const Vector&       a_ROW_VECTOR) {
    a_MATRIX.dot(a_result_ptr, a_ROW_VECTOR);
    return a_COLUMN_VECTOR.dot(*a_result_ptr);
}

}  // namespace printemps::utility::sparse
#endif
/*****************************************************************************/
// END
/*****************************************************************************/