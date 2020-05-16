/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef CPPMH_SOLVER_MEMORY_H__
#define CPPMH_SOLVER_MEMORY_H__

#include <vector>

namespace cppmh {
namespace solver {

/*****************************************************************************/
struct MemoryConstant {
    static constexpr int INITIAL_LAST_UPDATE_ITERATION = -1000;
};

/*****************************************************************************/
class Memory {
   private:
    std::vector<std::string>            m_variable_names;
    std::vector<model::ValueProxy<int>> m_last_update_iterations;
    std::vector<model::ValueProxy<int>> m_update_counts;
    long                                m_total_update_counts;

   public:
    /*************************************************************************/
    Memory(void) {
        this->initialize();
    }

    /*************************************************************************/
    template <class T_Variable, class T_Expression>
    Memory(model::Model<T_Variable, T_Expression> *a_model) {
        this->setup(a_model);
    }

    /*************************************************************************/
    virtual ~Memory(void) {
        /// nothing to do
    }

    /*************************************************************************/
    inline void initialize(void) {
        m_variable_names.clear();
        m_last_update_iterations.clear();
        m_update_counts.clear();
        m_total_update_counts = 0;
    }

    /*************************************************************************/
    template <class T_Variable, class T_Expression>
    inline constexpr void setup(
        model::Model<T_Variable, T_Expression> *a_model) {
        this->initialize();
        /**
         * Short-term memory:
         * The short-term memory records the iteration count at which each
         * variable has been updated last. The initial value of the short-term
         * memory must be sufficiently large and finite negative value. The
         * finiteness is required so that an operation a_ITERATION -
         * last_update_iterations[id][index] in tabu_search_move_score.h can
         * return finite integer value.
         */
        m_last_update_iterations = a_model->generate_variable_parameter_proxies(
            MemoryConstant::INITIAL_LAST_UPDATE_ITERATION);

        /* Long-term memory:
         * The Long-term memory records the number of times which each variable
         * has been updated. The initial value of the long-term memory is 0.
         */
        m_update_counts = a_model->generate_variable_parameter_proxies(0);

        m_variable_names = a_model->variable_names();
    }

    /*************************************************************************/
    inline void print_last_update_iterations(void) {
        /// This method is for debug.
        int variable_proxies_size = m_variable_names.size();
        for (auto i = 0; i < variable_proxies_size; i++) {
            auto &      last_update_iteration = m_last_update_iterations[i];
            std::string name                  = m_variable_names[i];
            int number_of_elements = last_update_iteration.number_of_elements();
            for (auto j = 0; j < number_of_elements; j++) {
                utility::print(
                    name + last_update_iteration.indices_label(j) + " = " +
                    std::to_string(
                        last_update_iteration.flat_indexed_values(j)));
            }
        }
    }

    /*************************************************************************/
    inline void print_update_counts(void) {
        /// This method is for debug.
        int variable_proxies_size = m_variable_names.size();
        for (auto i = 0; i < variable_proxies_size; i++) {
            auto &      update_count = m_update_counts[i];
            std::string name         = m_variable_names[i];

            int number_of_elements = update_count.number_of_elements();
            for (auto j = 0; j < number_of_elements; j++) {
                utility::print(
                    name + update_count.indices_label(j) + " = " +
                    std::to_string(update_count.flat_indexed_values(j)));
            }
        }
    }

    /*************************************************************************/
    inline void print_frequency(void) {
        /// This method is for debug.
        int variable_proxies_size = m_variable_names.size();
        for (auto i = 0; i < variable_proxies_size; i++) {
            auto &      update_count       = m_update_counts[i];
            std::string name               = m_variable_names[i];
            int         number_of_elements = update_count.number_of_elements();
            for (auto j = 0; j < number_of_elements; j++) {
                utility::print(
                    name + update_count.indices_label(j) + " = " +
                    std::to_string(update_count.flat_indexed_values(j) /
                                   static_cast<double>(m_total_update_counts)));
            }
        }
    }

    /*************************************************************************/
    inline void print_bias(void) {
        /// This method is for debug.
        utility::print(std::to_string(this->bias()));
    }

    /*************************************************************************/
    inline double bias(void) const {
        double result                = 0.0;
        int    variable_proxies_size = m_variable_names.size();
        for (auto i = 0; i < variable_proxies_size; i++) {
            auto &update_count       = m_update_counts[i];
            int   number_of_elements = update_count.number_of_elements();
            for (auto j = 0; j < number_of_elements; j++) {
                double frequency = update_count.flat_indexed_values(j) /
                                   static_cast<double>(m_total_update_counts);
                result += frequency * frequency;
            }
        }
        return result;
    }

    /*************************************************************************/
    template <class T_Variable, class T_Expression>
    inline constexpr void update(
        const model::Move<T_Variable, T_Expression> &a_MOVE,
        const int                                    a_ITERATION) {
        for (const auto &alteration : a_MOVE.alterations) {
            int id         = alteration.first->id();
            int flat_index = alteration.first->flat_index();

            m_last_update_iterations[id][flat_index] = a_ITERATION;
            m_update_counts[id][flat_index]++;
            m_total_update_counts++;
        }
    }

    /*************************************************************************/
    inline constexpr const std::vector<model::ValueProxy<int>>
        &last_update_iterations(void) const {
        return m_last_update_iterations;
    }

    /*************************************************************************/
    inline constexpr const std::vector<model::ValueProxy<int>> &update_counts(
        void) const {
        return m_update_counts;
    }

    /*************************************************************************/
    inline constexpr const long &total_update_counts(void) const {
        return m_total_update_counts;
    }
};  // namespace solver
}  // namespace solver
}  // namespace cppmh

#endif
/*****************************************************************************/
// END
/*****************************************************************************/