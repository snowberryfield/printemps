/*****************************************************************************/
// Copyright (c) 2020-2024 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_UTILITY_SPARSE_SPARSE_MATRIX_H__
#define PRINTEMPS_UTILITY_SPARSE_SPARSE_MATRIX_H__

namespace printemps::utility::sparse {
/*****************************************************************************/
struct SparseMatrix {
    std::vector<double> values;
    std::vector<int>    row_indices;
    std::vector<int>    column_indices;
    std::vector<int>    row_ptrs;

    Diagonal row_scaler;
    Diagonal column_scaler;

    int number_of_rows;
    int number_of_columns;

    double norm_one;
    double norm_infty;

    /*************************************************************************/
    SparseMatrix(void) {
        this->initialize();
    }

    /*************************************************************************/
    SparseMatrix(const std::vector<double>& a_VALUES,
                 const std::vector<int>&    a_ROW_INDICES,
                 const std::vector<int>&    a_COLUMN_INDICES,
                 const int a_NUMBER_OF_ROWS, const int a_NUMBER_OF_COLUMNS) {
        this->setup(a_VALUES, a_ROW_INDICES, a_COLUMN_INDICES, a_NUMBER_OF_ROWS,
                    a_NUMBER_OF_COLUMNS);
    }

    /*************************************************************************/
    inline void initialize(void) {
        this->values.clear();
        this->row_indices.clear();
        this->column_indices.clear();
        this->row_scaler.initialize();
        this->column_scaler.initialize();
        this->number_of_rows    = 0;
        this->number_of_columns = 0;
        this->norm_one          = 0.0;
        this->norm_infty        = 0.0;
    }

    /*************************************************************************/
    inline void setup(const std::vector<double>& a_VALUES,
                      const std::vector<int>&    a_ROW_INDICES,
                      const std::vector<int>&    a_COLUMN_INDICES,
                      const int                  a_NUMBER_OF_ROWS,
                      const int                  a_NUMBER_OF_COLUMNS) {
        this->values            = a_VALUES;
        this->row_indices       = a_ROW_INDICES;
        this->column_indices    = a_COLUMN_INDICES;
        this->number_of_rows    = a_NUMBER_OF_ROWS;
        this->number_of_columns = a_NUMBER_OF_COLUMNS;
        this->row_scaler        = Diagonal(this->number_of_rows);
        this->column_scaler     = Diagonal(this->number_of_columns);

        this->row_ptrs.resize(this->number_of_rows + 1, 0);

        const int NONZERO_SIZE = this->values.size();

        for (auto i = 0; i < NONZERO_SIZE; i++) {
            this->row_ptrs[this->row_indices[i] + 1]++;
        }
        for (auto i = 0; i < this->number_of_rows; i++) {
            this->row_ptrs[i + 1] += this->row_ptrs[i];
        }

        this->setup_norm();
    }

    /*************************************************************************/
    inline void setup_norm(void) {
        const int           NONZERO_SIZE = this->values.size();
        std::vector<double> r_temp(this->number_of_rows, 0);
        std::vector<double> c_temp(this->number_of_columns, 0);
        double              norm_infty = 0.0;

        for (auto i = 0; i < NONZERO_SIZE; i++) {
            const auto VALUE = fabs(this->values[i]);
            r_temp[this->row_indices[i]] += VALUE;
            c_temp[this->column_indices[i]] += VALUE;
            norm_infty = std::max(norm_infty, VALUE);
        }
        this->norm_one   = *std::max_element(c_temp.begin(), c_temp.end());
        this->norm_infty = norm_infty;
    }

    /*************************************************************************/
    inline void pock_chambolle_scaling() {
        const int NONZERO_SIZE = this->values.size();
        Diagonal  row_scaler(this->number_of_rows);
        Diagonal  column_scaler(this->number_of_columns);

        for (auto i = 0; i < NONZERO_SIZE; i++) {
            const auto VALUE = fabs(this->values[i]);
            row_scaler[this->row_indices[i]] += VALUE;
            column_scaler[this->column_indices[i]] += VALUE;
        }
        for (auto i = 0; i < this->number_of_rows; i++) {
            if (row_scaler[i] < constant::EPSILON_10) {
                row_scaler[i] = 1.0;
            } else {
                row_scaler[i] = sqrt(row_scaler[i]);
            }
        }

        for (auto i = 0; i < this->number_of_columns; i++) {
            if (column_scaler[i] < constant::EPSILON_10) {
                column_scaler[i] = 1.0;
            } else {
                column_scaler[i] = sqrt(column_scaler[i]);
            }
        }
        for (auto i = 0; i < NONZERO_SIZE; i++) {
            const auto ROW_INDEX    = this->row_indices[i];
            const auto COLUMN_INDEX = this->column_indices[i];
            this->values[i] /=
                (row_scaler[ROW_INDEX] * column_scaler[COLUMN_INDEX]);
        }

        this->row_scaler *= row_scaler;
        this->column_scaler *= column_scaler;
        this->setup_norm();
    }

    /*************************************************************************/
    inline void ruiz_scaling(const int a_NUMBER_OF_ITERATIONS) {
        const int NONZERO_SIZE = this->values.size();
        Diagonal  row_scaler(this->number_of_rows);
        Diagonal  column_scaler(this->number_of_columns);

        for (auto k = 0; k < a_NUMBER_OF_ITERATIONS; k++) {
            for (auto i = 0; i < NONZERO_SIZE; i++) {
                const auto VALUE = fabs(this->values[i]);
                row_scaler[this->row_indices[i]] =
                    std::max(row_scaler[this->row_indices[i]], VALUE);
                column_scaler[this->column_indices[i]] =
                    std::max(column_scaler[this->column_indices[i]], VALUE);
            }

            for (auto i = 0; i < this->number_of_rows; i++) {
                if (row_scaler[i] < constant::EPSILON_10) {
                    row_scaler[i] = 1.0;
                } else {
                    row_scaler[i] = sqrt(row_scaler[i]);
                }
            }

            for (auto i = 0; i < this->number_of_columns; i++) {
                if (column_scaler[i] < constant::EPSILON_10) {
                    column_scaler[i] = 1.0;
                } else {
                    column_scaler[i] = sqrt(column_scaler[i]);
                }
            }

            for (auto i = 0; i < NONZERO_SIZE; i++) {
                const auto ROW_INDEX    = this->row_indices[i];
                const auto COLUMN_INDEX = this->column_indices[i];
                this->values[i] /=
                    (row_scaler[ROW_INDEX] * column_scaler[COLUMN_INDEX]);
            }

            this->row_scaler *= row_scaler;
            this->column_scaler *= column_scaler;
            this->setup_norm();
        }
    }

    /*************************************************************************/
    inline SparseMatrix transpose(void) const noexcept {
        const int           NONZERO_SIZE = this->values.size();
        std::vector<double> values_trans(NONZERO_SIZE);
        std::vector<int>    row_indices_trans(NONZERO_SIZE);
        std::vector<int>    column_indices_trans(NONZERO_SIZE);
        std::vector<int>    permutation(NONZERO_SIZE);
        std::iota(permutation.begin(), permutation.end(), 0);

        std::stable_sort(permutation.begin(), permutation.end(),
                         [this](const int a_FIRST, const int a_SECOND) {
                             return this->row_indices[a_FIRST] <
                                    this->row_indices[a_SECOND];
                         });

        std::stable_sort(permutation.begin(), permutation.end(),
                         [this](const int a_FIRST, const int a_SECOND) {
                             return this->column_indices[a_FIRST] <
                                    this->column_indices[a_SECOND];
                         });
        for (auto i = 0; i < NONZERO_SIZE; i++) {
            values_trans[i]         = this->values[permutation[i]];
            row_indices_trans[i]    = this->row_indices[permutation[i]];
            column_indices_trans[i] = this->column_indices[permutation[i]];
        }

        return SparseMatrix(values_trans, column_indices_trans,
                            row_indices_trans, this->number_of_columns,
                            this->number_of_rows);
    }
    /*************************************************************************/
    inline void dot(Vector* a_result_ptr, const Vector& a_X) const {
        if (this->number_of_columns != static_cast<int>(a_X.size())) {
            throw std::runtime_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The matrix and vector sizes are inconsistent."));
        }

        const auto& VALUES         = this->values;
        const auto& ROW_PTRS       = this->row_ptrs;
        const auto& COLUMN_INDICES = this->column_indices;

#ifdef _OPENMP
#pragma omp parallel for schedule(guided)
#endif
        for (int i = 0; i < this->number_of_rows; i++) {
            (*a_result_ptr)[i] = 0;
            for (auto j = ROW_PTRS[i]; j < ROW_PTRS[i + 1]; j++) {
                (*a_result_ptr)[i] += VALUES[j] * a_X[COLUMN_INDICES[j]];
            }
        }
    }

    /*************************************************************************/
    inline Vector dot(const Vector& a_X) const {
        Vector result(this->number_of_rows);
        this->dot(&result, a_X);
        return result;
    }

    /*************************************************************************/
    inline void scale_row(SparseMatrix*   a_result_ptr,
                          const Diagonal& a_DIAGONAL) const {
        if (this->number_of_rows != static_cast<int>(a_DIAGONAL.size())) {
            throw std::runtime_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The matrix sizes are inconsistent."));
        }

        *a_result_ptr = *this;

        for (auto i = 0; i < this->number_of_rows; i++) {
            for (int j = this->row_ptrs[i]; j < this->row_ptrs[i + 1]; j++) {
                a_result_ptr->values[j] *= a_DIAGONAL[i];
            }
        }
    }

    /*************************************************************************/
    inline SparseMatrix scale_row(const Diagonal& a_DIAGONAL) const {
        SparseMatrix result;
        this->scale_row(&result, a_DIAGONAL);
        return result;
    }

    /*************************************************************************/
    inline void scale_column(SparseMatrix*   a_result_ptr,
                             const Diagonal& a_DIAGONAL) const {
        if (this->number_of_columns != static_cast<int>(a_DIAGONAL.size())) {
            throw std::runtime_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The matrix sizes are inconsistent."));
        }

        *a_result_ptr          = *this;
        const int NONZERO_SIZE = this->values.size();
        for (auto i = 0; i < NONZERO_SIZE; i++) {
            a_result_ptr->values[i] *=
                a_DIAGONAL[a_result_ptr->column_indices[i]];
        }
    }

    /*************************************************************************/
    inline SparseMatrix scale_column(const Diagonal& a_DIAGONAL) const {
        SparseMatrix result;
        this->scale_column(&result, a_DIAGONAL);
        return result;
    }

    /*************************************************************************/
    inline void print_as_sparse(void) const {
        for (auto i = 0; i < this->number_of_rows; i++) {
            for (int j = this->row_ptrs[i]; j < this->row_ptrs[i + 1]; j++) {
                std::cout << i << " " << this->column_indices[j] << " "
                          << this->values[j] << std::endl;
            }
        }
    }
    /*************************************************************************/
    inline void print_as_dense(void) const {
        std::vector<std::vector<double>> dense(
            this->number_of_rows,
            std::vector<double>(this->number_of_columns, 0));

        for (auto i = 0; i < this->number_of_rows; i++) {
            for (int j = this->row_ptrs[i]; j < this->row_ptrs[i + 1]; j++) {
                dense[i][this->column_indices[j]] = this->values[j];
            }
        }

        for (auto i = 0; i < this->number_of_rows; i++) {
            for (auto j = 0; j < this->number_of_columns; j++) {
                std::cout << dense[i][j] << " ";
            }
            std::cout << std::endl;
        }
    }
};
}  // namespace printemps::utility::sparse

#endif
/*****************************************************************************/
// END
/*****************************************************************************/