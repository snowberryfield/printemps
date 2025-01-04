/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_UTILITY_SPARSE_VECTOR_H__
#define PRINTEMPS_UTILITY_SPARSE_VECTOR_H__

namespace printemps::utility::sparse {
struct Vector {
    std::vector<double> values;

    /*************************************************************************/
    Vector(void) {
        this->initialize();
    }

    /*************************************************************************/
    Vector(const int a_SIZE) {
        this->setup(a_SIZE, 0.0);
    }

    /*************************************************************************/
    Vector(const int a_SIZE, const double a_VALUE) {
        this->setup(a_SIZE, a_VALUE);
    }

    /*************************************************************************/
    inline void initialize(void) {
        this->values.clear();
    }

    /*************************************************************************/
    inline void setup(const int a_SIZE, const double a_VALUE) {
        this->values.resize(a_SIZE, a_VALUE);
    }

    /*************************************************************************/
    inline double& operator[](const int a_INDEX) noexcept {
        return this->values[a_INDEX];
    }

    /*************************************************************************/
    inline const double& operator[](const int a_INDEX) const noexcept {
        return this->values[a_INDEX];
    }

    /*************************************************************************/
    inline double max(void) const noexcept {
        return *std::max_element(this->values.begin(), this->values.end());
    }

    /*************************************************************************/
    inline double min(void) const noexcept {
        return *std::min_element(this->values.begin(), this->values.end());
    }

    /*************************************************************************/
    inline double square_sum(void) const noexcept {
        return std::inner_product(this->values.begin(), this->values.end(),
                                  this->values.begin(), 0.0);
    }

    /*************************************************************************/
    inline double norm(void) const noexcept {
        return sqrt(std::inner_product(this->values.begin(), this->values.end(),
                                       this->values.begin(), 0.0));
    }

    /*************************************************************************/
    inline double norm(const double a_P) const noexcept {
        double result = 0.0;
        for (const auto& v : this->values) {
            result += std::pow(fabs(v), a_P);
        }
        return std::pow(result, 1.0 / a_P);
    }

    /*************************************************************************/
    inline std::size_t size(void) const noexcept {
        return this->values.size();
    }

    /*************************************************************************/
    inline void reset(void) noexcept {
        for (auto&& v : this->values) {
            v = 0.0;
        }
    }

    /*************************************************************************/
    inline Vector operator+(void) const noexcept {
        return *this;
    }

    /*************************************************************************/
    inline Vector operator-(void) const noexcept {
        auto result = *this;
        for (auto&& v : result.values) {
            v *= -1;
        }
        return result;
    }

    /*************************************************************************/
    inline Vector operator+=(const double a_VALUE) noexcept {
        for (auto&& v : this->values) {
            v += a_VALUE;
        }
        return *this;
    }

    /*************************************************************************/
    inline Vector operator+=(const Vector& a_VECTOR) {
        if (this->size() != a_VECTOR.size()) {
            throw std::runtime_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "Vector sizes are inconsistent."));
        }

        const int SIZE = this->values.size();
#ifdef _OPENMP
#pragma omp parallel for schedule(static)
#endif
        for (auto i = 0; i < SIZE; i++) {
            this->values[i] += a_VECTOR[i];
        }
        return *this;
    }

    /*************************************************************************/
    inline Vector operator-=(const double a_VALUE) noexcept {
        for (auto&& v : this->values) {
            v -= a_VALUE;
        }
        return *this;
    }

    /*************************************************************************/
    inline Vector operator-=(const Vector& a_VECTOR) {
        if (this->size() != a_VECTOR.size()) {
            throw std::runtime_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "Vector sizes are inconsistent."));
        }

        const int SIZE = this->values.size();
#ifdef _OPENMP
#pragma omp parallel for schedule(static)
#endif
        for (auto i = 0; i < SIZE; i++) {
            this->values[i] -= a_VECTOR[i];
        }
        return *this;
    }

    /*************************************************************************/
    inline Vector operator*=(const double a_VALUE) noexcept {
        for (auto&& v : this->values) {
            v *= a_VALUE;
        }
        return *this;
    }

    /*************************************************************************/
    inline Vector operator/=(const double a_VALUE) noexcept {
        for (auto&& v : this->values) {
            v /= a_VALUE;
        }
        return *this;
    }

    /*************************************************************************/
    inline void clamp(const Vector& a_LOWER_BOUNDS,
                      const Vector& a_UPPER_BOUNDS) {
        if (this->size() != a_LOWER_BOUNDS.size() ||
            this->size() != a_UPPER_BOUNDS.size()) {
            throw std::runtime_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "Vector sizes are inconsistent."));
        }
        const int SIZE = this->size();
#ifdef _OPENMP
#pragma omp parallel for schedule(static)
#endif
        for (auto i = 0; i < SIZE; i++) {
            this->values[i] = std::clamp(this->values[i], a_LOWER_BOUNDS[i],
                                         a_UPPER_BOUNDS[i]);
        }
    }

    /*************************************************************************/
    inline void clamp_positive(void) {
        const int SIZE = this->size();
#ifdef _OPENMP
#pragma omp parallel for schedule(static)
#endif
        for (auto i = 0; i < SIZE; i++) {
            this->values[i] = std::max(this->values[i], 0.0);
        }
    }

    /*************************************************************************/
    inline void clamp_negative(void) {
        const int SIZE = this->size();
#ifdef _OPENMP
#pragma omp parallel for schedule(static)
#endif
        for (auto i = 0; i < SIZE; i++) {
            this->values[i] = std::min(this->values[i], 0.0);
        }
    }

    /*************************************************************************/
    inline double dot(const Vector& a_VECTOR) const {
        if (this->size() != a_VECTOR.size()) {
            throw std::runtime_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "Vector sizes are inconsistent."));
        }

        return std::inner_product(this->values.begin(), this->values.end(),
                                  a_VECTOR.values.begin(), 0.0);
    }

    /*************************************************************************/
    inline Vector extend(const Vector& a_VECTOR) {
        this->values.insert(this->values.end(), a_VECTOR.values.begin(),
                            a_VECTOR.values.end());
        return *this;
    }

    /*************************************************************************/
    inline void learn(const Vector& a_VECTOR, const double a_LEARNING_RATE) {
        if (this->size() != a_VECTOR.size()) {
            throw std::runtime_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "Vector sizes are inconsistent."));
        }
        const int    SIZE        = this->size();
        const double FORGET_RATE = 1.0 - a_LEARNING_RATE;
#ifdef _OPENMP
#pragma omp parallel for schedule(static)
#endif
        for (auto i = 0; i < SIZE; i++) {
            this->values[i] =
                FORGET_RATE * this->values[i] + a_LEARNING_RATE * a_VECTOR[i];
        }
    }
};

}  // namespace printemps::utility::sparse

#endif
/*****************************************************************************/
// END
/*****************************************************************************/