/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef CPPMH_UTILITY_CSV_READER_H__
#define CPPMH_UTILITY_CSV_READER_H__

#include <vector>
#include <string>
#include <fstream>

#include "string_utility.h"

namespace cppmh {
namespace utility {
/******************************************************************************/
class CSVReader {
   private:
    std::vector<std::vector<std::string>> m_rows;
    std::vector<std::vector<std::string>> m_columns;

   public:
    /**************************************************************************/
    CSVReader(void) {
        this->initialize();
    }

    /**************************************************************************/
    CSVReader(const std::string &a_FILE_NAME) {
        this->initialize();
        this->read(a_FILE_NAME);
    }

    /**************************************************************************/
    virtual ~CSVReader(void) {
        /// nothing to do
    }

    /**************************************************************************/
    inline void initialize(void) {
        m_rows.clear();
        m_columns.clear();
    }

    /**************************************************************************/
    inline void read(const std::string &a_FILE_NAME) {
        const char DELIMITER = ',';

        std::ifstream input_file_stream;
        std::string   line;
        std::string   sub_string;
        std::size_t   max_column = 0;

        input_file_stream.open(a_FILE_NAME.c_str());

        while (std::getline(input_file_stream, line)) {
            std::size_t              start_position = 0;
            std::vector<std::string> row;
            sub_string = delete_crlf(line);

            while (true) {
                sub_string          = sub_string.substr(start_position);
                size_t end_position = sub_string.find(DELIMITER);
                if (end_position == std::string::npos) {
                    std::string item = trim(sub_string);

                    if (item.size() > 0) {
                        row.push_back(item);
                    }
                    break;
                } else {
                    std::string item = trim(sub_string.substr(0, end_position));
                    if (item.size() > 0) {
                        row.push_back(item);
                    }
                    start_position = end_position + 1;
                }
            }
            if (row.size() > 0) {
                m_rows.push_back(row);
                max_column = row.size() > max_column ? row.size() : max_column;
            }
        }

        for (std::size_t i = 0; i < max_column; i++) {
            std::vector<std::string> column;
            for (std::size_t j = 0; j < m_rows.size(); j++) {
                column.push_back(m_rows[j].size() < max_column ? " "
                                                               : m_rows[j][i]);
            }
            m_columns.push_back(column);
        }
    }
    /**************************************************************************/
    inline std::vector<double> search_by_row_to_double(
        const std::string &a_KEYWORD) {
        std::vector<double> values;
        for (std::size_t i_row = 0; i_row < m_rows.size(); i_row++) {
            if (m_rows[i_row][0] == trim(a_KEYWORD)) {
                for (std::size_t i_column = 1; i_column < m_rows[i_row].size();
                     i_column++) {
                    values.push_back(atof(m_rows[i_row][i_column].c_str()));
                }
                break;
            }
        }
        return values;
    }

    /**************************************************************************/
    inline std::vector<double> search_by_column_to_double(
        const std::string &a_KEYWORD) {
        std::vector<double> values;
        for (std::size_t i_column = 0; i_column < m_columns.size();
             i_column++) {
            if (m_columns[i_column][0] == trim(a_KEYWORD)) {
                for (std::size_t i_row = 1; i_row < m_columns[i_column].size();
                     i_row++) {
                    values.push_back(atof(m_columns[i_column][i_row].c_str()));
                }
                break;
            }
        }
        return values;
    }

    /**************************************************************************/
    inline std::vector<int> search_by_row_to_int(const std::string &a_KEYWORD) {
        std::vector<int> values;
        for (std::size_t i_row = 0; i_row < m_rows.size(); i_row++) {
            if (m_rows[i_row][0] == trim(a_KEYWORD)) {
                for (std::size_t i_column = 1; i_column < m_rows[i_row].size();
                     i_column++) {
                    values.push_back(atoi(m_rows[i_row][i_column].c_str()));
                }
                break;
            }
        }
        return values;
    }

    /**************************************************************************/
    inline std::vector<int> search_by_column_to_int(
        const std::string &a_KEYWORD) {
        std::vector<int> values;
        for (std::size_t i_column = 0; i_column < m_columns.size();
             i_column++) {
            if (m_columns[i_column][0] == trim(a_KEYWORD)) {
                for (std::size_t i_row = 1; i_row < m_columns[i_row].size();
                     i_row++) {
                    values.push_back(atoi(m_columns[i_column][i_row].c_str()));
                }
                break;
            }
        }
        return values;
    }
    /**************************************************************************/
    inline std::vector<std::string> search_by_row_to_string(
        const std::string &a_KEYWORD) {
        std::vector<std::string> values;
        for (std::size_t i_row = 0; i_row < m_rows.size(); i_row++) {
            if (m_rows[i_row][0] == trim(a_KEYWORD)) {
                for (std::size_t i_column = 1; i_column < m_rows[i_row].size();
                     i_column++) {
                    values.push_back(m_rows[i_row][i_column]);
                }
                break;
            }
        }
        return values;
    }

    /**************************************************************************/
    inline std::vector<std::string> search_by_column_to_string(
        const std::string &a_KEYWORD) {
        std::vector<std::string> values;
        for (std::size_t i_column = 0; i_column < m_columns.size();
             i_column++) {
            if (m_columns[i_column][0] == trim(a_KEYWORD)) {
                for (std::size_t i_row = 1; i_row < m_columns[i_column].size();
                     i_row++) {
                    values.push_back(m_columns[i_column][i_row]);
                }
                break;
            }
        }
        return values;
    }

    /**************************************************************************/
    inline void read_parameter(int *a_result, const std::string &a_KEYWORD) {
        std::vector<int> temp = this->search_by_row_to_int(a_KEYWORD);
        if (temp.size() > 0) {
            *a_result = temp.front();
        }
    }

    /**************************************************************************/
    void read_parameter(double *a_result, const std::string &a_KEYWORD) {
        std::vector<double> temp = this->search_by_row_to_double(a_KEYWORD);
        if (temp.size() > 0) {
            *a_result = temp.front();
        }
    }

    /**************************************************************************/
    inline const std::vector<std::vector<std::string>> &rows(void) const {
        return m_rows;
    }

    /**************************************************************************/
    inline const std::vector<std::vector<std::string>> &columns(void) const {
        return m_columns;
    }

    /**************************************************************************/
    inline const std::vector<std::string> &row(std::size_t a_n_row) const {
        return m_rows[a_n_row];
    }

    /**************************************************************************/
    inline const std::vector<std::string> &column(
        std::size_t a_n_column) const {
        return m_columns[a_n_column];
    }
};
}  // namespace utility
}  // namespace cppmh
/******************************************************************************/
// END
/******************************************************************************/
#endif