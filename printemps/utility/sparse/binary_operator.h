/*****************************************************************************/
// Copyright (c) 2020-2023 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_UTILITY_SPARSE_BINARY_OPERATOR_H__
#define PRINTEMPS_UTILITY_SPARSE_BINARY_OPERATOR_H__

namespace printemps::utility::sparse {
/*****************************************************************************/
inline Vector operator+(const double a_VALUE, const Vector &a_VECTOR) {
    auto result = a_VECTOR;
    return (result += a_VALUE);
}

/*****************************************************************************/
inline Vector operator+(const Vector &a_VECTOR, const double a_VALUE) {
    auto result = a_VECTOR;
    return (result += a_VALUE);
}

/*****************************************************************************/
inline Vector operator+(const Vector &a_VECTOR_FIRST,
                        const Vector &a_VECTOR_SECOND) {
    auto result = a_VECTOR_FIRST;
    return (result += a_VECTOR_SECOND);
}

/*****************************************************************************/
inline Vector operator-(const double a_VALUE, const Vector &a_VECTOR) {
    auto result = -a_VECTOR;
    return (result += a_VALUE);
}

/*****************************************************************************/
inline Vector operator-(const Vector &a_VECTOR, const double a_VALUE) {
    auto result = a_VECTOR;
    return (result -= a_VALUE);
}

/*****************************************************************************/
inline Vector operator-(const Vector &a_VECTOR_FIRST,
                        const Vector &a_VECTOR_SECOND) {
    auto result = a_VECTOR_FIRST;
    return (result -= a_VECTOR_SECOND);
}

/*****************************************************************************/
inline Vector operator*(const double a_VALUE, const Vector &a_VECTOR) {
    auto result = a_VECTOR;
    return (result *= a_VALUE);
}

/*****************************************************************************/
inline Vector operator*(const Vector &a_VECTOR, const double a_VALUE) {
    auto result = a_VECTOR;
    return (result *= a_VALUE);
}

/*****************************************************************************/
inline Vector operator/(const Vector &a_VECTOR, const double a_VALUE) {
    auto result = a_VECTOR;
    return (result /= a_VALUE);
}

/*****************************************************************************/
inline Vector operator*(const SparseMatrix &a_SparseMatrix, const Vector &a_X) {
    return a_SparseMatrix.dot(a_X);
}

/*****************************************************************************/
inline SparseMatrix operator*(const SparseMatrix &a_SparseMatrix,
                              const Diagonal &    a_DIAGONAL) {
    return a_SparseMatrix.scale_column(a_DIAGONAL);
}

/*****************************************************************************/
inline SparseMatrix operator*(const Diagonal &    a_DIAGONAL,
                              const SparseMatrix &a_SparseMatrix) {
    return a_SparseMatrix.scale_row(a_DIAGONAL);
}

}  // namespace printemps::utility::sparse
#endif
/*****************************************************************************/
// END
/*****************************************************************************/