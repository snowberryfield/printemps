/*****************************************************************************/
// Copyright (c) 2020-2023 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_UTILITY_SPARSE_DIAGONAL_H__
#define PRINTEMPS_UTILITY_SPARSE_DIAGONAL_H__

namespace printemps::utility::sparse {
struct Diagonal {
    Vector diagonal_vector;

    /*************************************************************************/
    Diagonal(void) {
        this->initialize();
    }

    /*************************************************************************/
    Diagonal(const int a_SIZE) {
        this->setup(a_SIZE);
    }

    /*************************************************************************/
    inline void initialize(void) {
        this->diagonal_vector.initialize();
    }

    /*************************************************************************/
    inline void setup(const int a_SIZE) {
        this->diagonal_vector.values = std::vector<double>(a_SIZE, 1.0);
    }

    /*************************************************************************/
    inline double& operator[](const int a_INDEX) noexcept {
        return this->diagonal_vector[a_INDEX];
    }

    /*************************************************************************/
    inline const double& operator[](const int a_INDEX) const noexcept {
        return this->diagonal_vector[a_INDEX];
    }

    /*************************************************************************/
    inline Diagonal inverse(void) const {
        Diagonal result = *this;
        for (auto&& v : result.diagonal_vector.values) {
            v = 1.0 / v;
        }
        return result;
    }

    /*************************************************************************/
    inline std::size_t size(void) const noexcept {
        return this->diagonal_vector.size();
    }

    /*************************************************************************/
    inline Diagonal operator*=(const double& a_VALUE) {
        const int SIZE = this->size();
        for (auto i = 0; i < SIZE; i++) {
            this->diagonal_vector[i] *= a_VALUE;
        }
        return *this;
    }

    /*************************************************************************/
    inline Diagonal operator*=(const Diagonal& a_DIAGONAL) {
        if (this->size() != a_DIAGONAL.size()) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "Matrix sizes are inconsistent."));
        }

        const int SIZE = this->size();

        for (auto i = 0; i < SIZE; i++) {
            this->diagonal_vector[i] *= a_DIAGONAL.diagonal_vector[i];
        }
        return *this;
    }

    /*************************************************************************/
    inline void dot(Vector* a_result_ptr, const Vector& a_VECTOR) const {
        if (this->size() != a_VECTOR.size()) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "Matrix and vector sizes are inconsistent."));
        }

        *a_result_ptr = a_VECTOR;

        const int SIZE = this->size();
        for (auto i = 0; i < SIZE; i++) {
            a_result_ptr->values[i] *= this->diagonal_vector[i];
        }
    }

    /*************************************************************************/
    inline Vector dot(const Vector& a_VECTOR) const {
        if (this->size() != a_VECTOR.size()) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "Matrix and vector sizes are inconsistent."));
        }

        Vector result = a_VECTOR;
        this->dot(&result, a_VECTOR);
        return result;
    }

    /*************************************************************************/
    inline void dot(Vector* a_result_ptr) const {
        if (this->size() != a_result_ptr->size()) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "Matrix and vector sizes are inconsistent."));
        }

        const int SIZE = this->size();
        for (auto i = 0; i < SIZE; i++) {
            a_result_ptr->values[i] *= this->diagonal_vector[i];
        }
    }
};
}  // namespace printemps::utility::sparse

#endif
/*****************************************************************************/
// END
/*****************************************************************************/