/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_SOLVER_MEMORY_H__
#define PRINTEMPS_SOLVER_MEMORY_H__

namespace printemps {
namespace solver {
/*****************************************************************************/
struct MemoryConstant {
    static constexpr int INITIAL_LAST_UPDATE_ITERATION = -1000;
};

/*****************************************************************************/
class Memory {
   private:
    std::vector<std::string>                  m_variable_names;
    std::vector<multi_array::ValueProxy<int>> m_last_update_iterations;
    std::vector<multi_array::ValueProxy<int>> m_update_counts;
    long                                      m_total_update_count;
    double                                    m_total_update_count_reciprocal;

    double m_intensity_numerator;
    double m_intensity_denominator_reciprocal;
    double m_intensity;

   public:
    /*************************************************************************/
    Memory(void) {
        this->initialize();
    }

    /*************************************************************************/
    template <class T_Variable, class T_Expression>
    Memory(model::Model<T_Variable, T_Expression> *a_model_ptr) {
        this->setup(a_model_ptr);
    }

    /*************************************************************************/
    virtual ~Memory(void) {
        /// nothing to do
    }

    /*************************************************************************/
    void initialize(void) {
        m_variable_names.clear();
        m_last_update_iterations.clear();
        m_update_counts.clear();
        m_total_update_count            = 0;
        m_total_update_count_reciprocal = 0.0;

        m_intensity_numerator              = 0.0;
        m_intensity_denominator_reciprocal = 0.0;
        m_intensity                        = 0.0;
    }

    /*************************************************************************/
    template <class T_Variable, class T_Expression>
    inline constexpr void setup(
        model::Model<T_Variable, T_Expression> *a_model_ptr) {
        this->initialize();
        /**
         * Short-term memory:
         * The short-term memory records the iteration count at which each
         * variable has been updated last. The initial value of the short-term
         * memory must be sufficiently large and finite negative value. The
         * finiteness is required so that an operation a_ITERATION -
         * last_update_iterations[proxy_index][index] in
         * tabu_search_move_score.h can return finite integer value.
         */
        m_last_update_iterations =
            a_model_ptr->generate_variable_parameter_proxies(
                MemoryConstant::INITIAL_LAST_UPDATE_ITERATION);

        /* Long-term memory:
         * The Long-term memory records the number of times which each variable
         * has been updated. The initial value of the long-term memory is 0.
         */
        m_update_counts = a_model_ptr->generate_variable_parameter_proxies(0);

        m_variable_names = a_model_ptr->variable_names();
    }

    /*************************************************************************/
    void print_last_update_iterations(void) {
        /// This method is for debug.
        const int VARIABLE_PROXIES_SIZE = m_variable_names.size();
        for (auto i = 0; i < VARIABLE_PROXIES_SIZE; i++) {
            auto &last_update_iteration = m_last_update_iterations[i];
            const std::string NAME      = m_variable_names[i];
            const int         NUMBER_OF_ELEMENTS =
                last_update_iteration.number_of_elements();
            for (auto j = 0; j < NUMBER_OF_ELEMENTS; j++) {
                utility::print(
                    NAME + last_update_iteration.indices_label(j) + " = " +
                    std::to_string(
                        last_update_iteration.flat_indexed_values(j)));
            }
        }
    }

    /*************************************************************************/
    void print_update_counts(void) {
        /// This method is for debug.
        const int VARIABLE_PROXIES_SIZE = m_variable_names.size();
        for (auto i = 0; i < VARIABLE_PROXIES_SIZE; i++) {
            auto &            update_counts = m_update_counts[i];
            const std::string NAME          = m_variable_names[i];
            const int NUMBER_OF_ELEMENTS = update_counts.number_of_elements();
            for (auto j = 0; j < NUMBER_OF_ELEMENTS; j++) {
                utility::print(
                    NAME + update_counts.indices_label(j) + " = " +
                    std::to_string(update_counts.flat_indexed_values(j)));
            }
        }
    }

    /*************************************************************************/
    void print_frequency(void) {
        /// This method is for debug.
        const int VARIABLE_PROXIES_SIZE = m_variable_names.size();
        for (auto i = 0; i < VARIABLE_PROXIES_SIZE; i++) {
            auto &            update_counts = m_update_counts[i];
            const std::string NAME          = m_variable_names[i];
            const int NUMBER_OF_ELEMENTS = update_counts.number_of_elements();
            for (auto j = 0; j < NUMBER_OF_ELEMENTS; j++) {
                utility::print(
                    NAME + update_counts.indices_label(j) + " = " +
                    std::to_string(update_counts.flat_indexed_values(j) *
                                   m_total_update_count_reciprocal));
            }
        }
    }

    /*************************************************************************/
    void print_intensity(void) {
        /// This method is for debug.
        utility::print(std::to_string(m_intensity));
    }

    /*************************************************************************/
    inline constexpr double intensity(void) const noexcept {
        return m_intensity;
    }

    /*************************************************************************/
    template <class T_Variable, class T_Expression>
    constexpr void update(
        const neighborhood::Move<T_Variable, T_Expression> &a_MOVE,
        const int a_ITERATION) noexcept {
        for (const auto &alteration : a_MOVE.alterations) {
            int proxy_index = alteration.first->proxy_index();
            int flat_index  = alteration.first->flat_index();

            m_last_update_iterations[proxy_index][flat_index] = a_ITERATION;
            m_intensity_numerator +=
                2.0 * m_update_counts[proxy_index][flat_index] + 1;
            m_update_counts[proxy_index][flat_index]++;
            m_total_update_count++;
        }
        m_total_update_count_reciprocal =
            1.0 / static_cast<double>(m_total_update_count);
        m_intensity_denominator_reciprocal =
            m_total_update_count_reciprocal * m_total_update_count_reciprocal;
        m_intensity =
            m_intensity_numerator * m_intensity_denominator_reciprocal;
    }

    /*************************************************************************/
    template <class T_Variable, class T_Expression>
    constexpr void update(
        const neighborhood::Move<T_Variable, T_Expression> &a_MOVE,
        const int                                           a_ITERATION,     //
        const int                                           a_RANDOM_WIDTH,  //
        std::mt19937 *get_rand_mt) noexcept {
        if (a_RANDOM_WIDTH == 0) {
            this->update(a_MOVE, a_ITERATION);
        } else {
            for (const auto &alteration : a_MOVE.alterations) {
                int proxy_index = alteration.first->proxy_index();
                int flat_index  = alteration.first->flat_index();
                int randomness =
                    (*get_rand_mt)() % (2 * a_RANDOM_WIDTH) - a_RANDOM_WIDTH;

                m_last_update_iterations[proxy_index][flat_index] =
                    a_ITERATION + randomness;
                m_intensity_numerator +=
                    2.0 * m_update_counts[proxy_index][flat_index] + 1;
                m_update_counts[proxy_index][flat_index]++;
                m_total_update_count++;
            }
            m_total_update_count_reciprocal =
                1.0 / static_cast<double>(m_total_update_count);
            m_intensity_denominator_reciprocal =
                m_total_update_count_reciprocal *
                m_total_update_count_reciprocal;
            m_intensity =
                m_intensity_numerator * m_intensity_denominator_reciprocal;
        }
    }

    /*************************************************************************/
    inline void reset_last_update_iterations(void) {
        /// This method cannot be constexpr.
        for (auto &&proxy : m_last_update_iterations) {
            for (auto &&value : proxy.flat_indexed_values()) {
                value = MemoryConstant::INITIAL_LAST_UPDATE_ITERATION;
            }
        }
    }
    /*************************************************************************/
    inline void revert_last_update_iterations(const int a_ITERATION) {
        /// This method cannot be constexpr.
        for (auto &&proxy : m_last_update_iterations) {
            for (auto &&value : proxy.flat_indexed_values()) {
                if (value > a_ITERATION) {
                    value = MemoryConstant::INITIAL_LAST_UPDATE_ITERATION;
                }
                value -= (a_ITERATION + 1);
            }
        }
    }

    /*************************************************************************/
    inline constexpr const std::vector<multi_array::ValueProxy<int>>
        &last_update_iterations(void) const {
        return m_last_update_iterations;
    }

    /*************************************************************************/
    inline constexpr const std::vector<multi_array::ValueProxy<int>>
        &update_counts(void) const {
        return m_update_counts;
    }

    /*************************************************************************/
    inline constexpr long total_update_count(void) const {
        return m_total_update_count;
    }

    /*************************************************************************/
    inline constexpr double total_update_count_reciprocal(void) const {
        return m_total_update_count_reciprocal;
    }
};  // namespace solver
}  // namespace solver
}  // namespace printemps

#endif
/*****************************************************************************/
// END
/*****************************************************************************/