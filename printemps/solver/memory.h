/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
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
template <class T_Variable, class T_Expression>
class Memory {
   private:
    model::Model<T_Variable, T_Expression> *m_model_ptr;

    std::vector<std::string> m_variable_names;
    std::vector<std::string> m_constraint_names;

    std::vector<multi_array::ValueProxy<int>> m_last_update_iterations;
    std::vector<multi_array::ValueProxy<int>> m_update_counts;
    std::vector<multi_array::ValueProxy<int>> m_violation_counts;

    long   m_total_update_count;
    double m_total_update_count_reciprocal;

    long   m_total_violation_count;
    double m_total_violation_count_reciprocal;

    double m_primal_intensity_numerator;
    double m_primal_intensity_denominator_reciprocal;
    double m_primal_intensity;

    double m_dual_intensity_numerator;
    double m_dual_intensity_denominator_reciprocal;
    double m_dual_intensity;

   public:
    /*************************************************************************/
    Memory(void) {
        this->initialize();
    }

    /*************************************************************************/
    Memory(model::Model<T_Variable, T_Expression> *a_model_ptr) {
        this->setup(a_model_ptr);
    }

    /*************************************************************************/
    virtual ~Memory(void) {
        /// nothing to do
    }

    /*************************************************************************/
    void initialize(void) {
        m_model_ptr = nullptr;
        m_variable_names.clear();
        m_last_update_iterations.clear();
        m_update_counts.clear();
        m_violation_counts.clear();
        m_total_update_count            = 0;
        m_total_update_count_reciprocal = 0.0;

        m_primal_intensity_numerator              = 0.0;
        m_primal_intensity_denominator_reciprocal = 0.0;
        m_primal_intensity                        = 0.0;
    }

    /*************************************************************************/
    inline constexpr void setup(
        model::Model<T_Variable, T_Expression> *a_model_ptr) {
        this->initialize();
        m_model_ptr = a_model_ptr;

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

        /**
         * Long-term memory:
         * The Long-term memory records the number of times which each variable
         * has been updated. The initial value of the long-term memory is 0.
         */
        m_update_counts = a_model_ptr->generate_variable_parameter_proxies(0);

        m_violation_counts =
            a_model_ptr->generate_constraint_parameter_proxies(0);

        m_variable_names   = m_model_ptr->variable_names();
        m_constraint_names = m_model_ptr->constraint_names();
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
    void print_primal_intensity(void) {
        /// This method is for debug.
        utility::print(std::to_string(m_primal_intensity));
    }

    /*************************************************************************/
    inline constexpr double primal_intensity(void) const noexcept {
        return m_primal_intensity;
    }

    /*************************************************************************/
    inline constexpr double dual_intensity(void) const noexcept {
        return m_dual_intensity;
    }

    /*************************************************************************/
    constexpr void update(
        const neighborhood::Move<T_Variable, T_Expression> &a_MOVE,
        const int a_ITERATION) noexcept {
        this->update(a_MOVE, a_ITERATION, 0, nullptr);
    }

    /*************************************************************************/
    constexpr void update(
        const neighborhood::Move<T_Variable, T_Expression> &a_MOVE,
        const int                                           a_ITERATION,     //
        const int                                           a_RANDOM_WIDTH,  //
        std::mt19937 *get_rand_mt) noexcept {
        for (const auto &alteration : a_MOVE.alterations) {
            const int PROXY_INDEX = alteration.first->proxy_index();
            const int FLAT_INDEX  = alteration.first->flat_index();
            int       randomness  = 0;
            if (a_RANDOM_WIDTH > 0) {
                randomness =
                    (*get_rand_mt)() % (2 * a_RANDOM_WIDTH) - a_RANDOM_WIDTH;
            }

            m_last_update_iterations[PROXY_INDEX][FLAT_INDEX] =
                a_ITERATION + randomness;
            m_primal_intensity_numerator +=
                2.0 * m_update_counts[PROXY_INDEX][FLAT_INDEX] + 1;
            m_update_counts[PROXY_INDEX][FLAT_INDEX]++;
            m_total_update_count++;
        }

        for (const auto &constraint_ptr :
             m_model_ptr->violative_constraint_ptrs()) {
            const int PROXY_INDEX = constraint_ptr->proxy_index();
            const int FLAT_INDEX  = constraint_ptr->flat_index();
            m_dual_intensity_numerator +=
                2.0 * m_violation_counts[PROXY_INDEX][FLAT_INDEX] + 1;
            m_violation_counts[PROXY_INDEX][FLAT_INDEX]++;
            m_total_violation_count++;
        }

        m_total_update_count_reciprocal =
            1.0 / static_cast<double>(std::max(1L, m_total_update_count));
        m_primal_intensity_denominator_reciprocal =
            m_total_update_count_reciprocal * m_total_update_count_reciprocal;
        m_primal_intensity = m_primal_intensity_numerator *
                             m_primal_intensity_denominator_reciprocal;

        m_total_violation_count_reciprocal =
            1.0 / static_cast<double>(std::max(1L, m_total_violation_count));
        m_dual_intensity_denominator_reciprocal =
            m_total_violation_count_reciprocal *
            m_total_violation_count_reciprocal;
        m_dual_intensity = m_dual_intensity_numerator *
                           m_dual_intensity_denominator_reciprocal;
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

    /*************************************************************************/
    inline constexpr const std::vector<multi_array::ValueProxy<int>>
        &violation_counts(void) const {
        return m_violation_counts;
    }

    /*************************************************************************/
    inline constexpr const std::vector<multi_array::ValueProxy<double>>
    violation_count_rates(void) const {
        auto violation_count_rates =
            m_model_ptr->generate_constraint_parameter_proxies(0.0);
        double min = HUGE_VALF;
        for (auto &&proxy : violation_count_rates) {
            const auto &COUNT_VALUES =
                m_violation_counts[proxy.index()].flat_indexed_values();
            auto &    count_rates = proxy.flat_indexed_values();
            const int SIZE        = COUNT_VALUES.size();
            for (auto i = 0; i < SIZE; i++) {
                count_rates[i] =
                    COUNT_VALUES[i] * m_total_violation_count_reciprocal * SIZE;
                min = std::min(min, count_rates[i]);
            }
        }
        return violation_count_rates;
    }

    /*************************************************************************/
    inline constexpr long total_violation_count(void) const {
        return m_total_violation_count;
    }

    /*************************************************************************/
    inline constexpr double total_violation_count_reciprocal(void) const {
        return m_total_violation_count_reciprocal;
    }
};  // namespace solver
}  // namespace solver
}  // namespace printemps

#endif
/*****************************************************************************/
// END
/*****************************************************************************/