/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_UTILITY_GF2_H__
#define PRINTEMPS_UTILITY_GF2_H__

namespace printemps {
namespace utility {
/*****************************************************************************/
class GF2Matrix {
   private:
    std::vector<std::vector<int>> m_rows;

   public:
    /*************************************************************************/
    GF2Matrix(void) {
        this->initialize();
    }

    /*************************************************************************/
    GF2Matrix(const int a_NUMBER_OF_ROWS, const int a_NUMBER_OF_COLUMNS) {
        this->setup(a_NUMBER_OF_ROWS, a_NUMBER_OF_COLUMNS);
    }

    /*************************************************************************/
    inline void initialize(void) {
        m_rows.clear();
    }

    /*************************************************************************/
    inline void setup(const int a_NUMBER_OF_ROWS,
                      const int a_NUMBER_OF_COLUMNS) {
        m_rows.resize(a_NUMBER_OF_ROWS,
                      std::vector<int>(a_NUMBER_OF_COLUMNS, 0));
    }

    /*************************************************************************/
    inline int number_of_rows(void) const {
        return m_rows.size();
    }

    /*************************************************************************/
    inline int number_of_columns(void) const {
        return m_rows.front().size();
    }

    /*************************************************************************/
    inline std::vector<int> &operator[](const int a_ROW) {
        return m_rows[a_ROW];
    }

    /*************************************************************************/
    inline std::vector<int> const &operator[](const int a_ROW) const {
        return m_rows[a_ROW];
    }

    /*************************************************************************/
    inline void print(void) const {
        const int NUMBER_OF_ROWS    = this->number_of_rows();
        const int NUMBER_OF_COLUMNS = this->number_of_columns();
        for (auto i = 0; i < NUMBER_OF_ROWS; i++) {
            for (auto j = 0; j < NUMBER_OF_COLUMNS; j++) {
                std::cout << m_rows[i][j] << " ";
            }
            std::cout << std::endl;
        }
    }

    /*************************************************************************/
    inline std::pair<GF2Matrix, int> inverse_and_rank(void) const {
        const int SIZE = this->number_of_rows();
        GF2Matrix A    = *this;
        GF2Matrix B(SIZE, SIZE);
        int       rank = 0;

        for (auto i = 0; i < SIZE; i++) {
            B[i][i] = 1;
        }

        for (auto j = 0; j < SIZE; j++) {
            int row = -1;
            for (auto i = rank; i < SIZE; i++) {
                if (A[i][j] == 1) {
                    row = i;
                    break;
                }
            }
            if (row == -1) {
                continue;
            }

            if (row != rank) {
                swap(A[row], A[rank]);
                swap(B[row], B[rank]);
            }

            for (auto i = rank + 1; i < SIZE; i++) {
                if (A[i][j] == 1) {
                    for (auto k = 0; k < SIZE; k++) {
                        A[i][k] = (A[i][k] + A[rank][k]) & 1;
                        B[i][k] = (B[i][k] + B[rank][k]) & 1;
                    }
                }
            }
            rank++;
        }

        if (rank == SIZE) {
            for (auto j = SIZE - 1; j > 0; j--) {
                for (auto i = j - 1; i >= 0; i--) {
                    if (A[i][j] == 1) {
                        for (auto k = 0; k < SIZE; k++) {
                            A[i][k] = (A[j][k] + A[i][k]) & 1;
                            B[i][k] = (B[j][k] + B[i][k]) & 1;
                        }
                    }
                }
            }
        }

        return {B, rank};
    }

    /*************************************************************************/
    inline std::vector<int> dot(const std::vector<int> &a_VECTOR) const {
        std::vector<int> result(m_rows.size(), 0);
        const int        NUMBER_OF_ROWS    = this->number_of_rows();
        const int        NUMBER_OF_COLUMNS = this->number_of_columns();
        for (auto i = 0; i < NUMBER_OF_ROWS; i++) {
            for (auto j = 0; j < NUMBER_OF_COLUMNS; j++) {
                result[i] += m_rows[i][j] * a_VECTOR[j];
            }
            result[i] &= 1;
        }
        return result;
    }

    /*************************************************************************/
    inline GF2Matrix dot(const GF2Matrix &a_MATRIX) const {
        const int NUMBER_OF_ROWS           = this->number_of_rows();
        const int NUMBER_OF_COLUMNS        = this->number_of_columns();
        const int RESULT_NUMBER_OF_COLUMNS = a_MATRIX.number_of_columns();

        GF2Matrix result(NUMBER_OF_ROWS, RESULT_NUMBER_OF_COLUMNS);

        for (auto i = 0; i < NUMBER_OF_ROWS; i++) {
            for (auto j = 0; j < RESULT_NUMBER_OF_COLUMNS; j++) {
                for (auto k = 0; k < NUMBER_OF_COLUMNS; k++) {
                    result[i][j] += m_rows[i][k] * a_MATRIX[k][j];
                }
                result[i][j] &= 1;
            }
        }
        return result;
    }
};
}  // namespace utility
}  // namespace printemps

#endif
/*****************************************************************************/
// END
/*****************************************************************************/