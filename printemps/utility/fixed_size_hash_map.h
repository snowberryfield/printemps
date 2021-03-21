/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_UTILITY_FIXED_SIZE_HASH_MAP__
#define PRINTEMPS_UTILITY_FIXED_SIZE_HASH_MAP__

namespace printemps {
namespace utility {
/*****************************************************************************/
struct FixedSizeHashMapConstant {
    static constexpr std::uint_fast32_t DEFAULT_BUCKET_SIZE = 1;
    static constexpr int                LOAD_MARGIN         = 10;
};

/*****************************************************************************/
template <class T_Key, class T_Value>
class FixedSizeHashMap {
   private:
    std::uint_fast8_t  m_shift_size;
    std::uint_fast32_t m_bucket_size;
    std::uint_fast32_t m_mask;

    bool     m_is_memory_allocated;
    T_Key *  m_keys;
    T_Value *m_values;
    bool *   m_is_occupied;

    /*************************************************************************/
    inline constexpr std::uint_fast32_t compute_hash(
        const T_Key a_KEY) const noexcept {
        return reinterpret_cast<std::uint_fast64_t>(a_KEY) >> m_shift_size;
    }

    /*************************************************************************/
    inline constexpr std::uint_fast32_t compute_index(
        const std::uint_fast32_t a_HASH) const noexcept {
        return a_HASH & m_mask;
    }

    /*************************************************************************/
    inline void insert(const T_Key a_KEY, const T_Value a_VALUE) {
        /**
         * This method is provided as private and is called only by setup().
         */
        std::uint_fast32_t index = this->compute_hash(a_KEY) & m_mask;
        while (m_is_occupied[index]) {
            index = (index + 1) & m_mask;
        }

        m_is_occupied[index] = 1;
        m_keys[index]        = a_KEY;
        m_values[index]      = a_VALUE;
    }

   public:
    /*************************************************************************/
    FixedSizeHashMap(void) {
        this->initialize();
    }

    /*************************************************************************/
    virtual ~FixedSizeHashMap(void) {
        if (m_is_memory_allocated) {
            delete[] m_is_occupied;
            delete[] m_keys;
            delete[] m_values;
            m_is_memory_allocated = false;
        }
    }

    /*************************************************************************/
    FixedSizeHashMap(const std::unordered_map<T_Key, T_Value> &a_UNORDERED_MAP,
                     const std::uint_fast32_t                  a_KEY_SIZE) {
        this->setup(a_UNORDERED_MAP, a_KEY_SIZE);
    }

    /*************************************************************************/
    inline constexpr void initialize(void) {
        m_shift_size          = 0;
        m_bucket_size         = FixedSizeHashMapConstant::DEFAULT_BUCKET_SIZE;
        m_mask                = m_bucket_size - 1;
        m_is_memory_allocated = false;

        m_keys        = nullptr;
        m_values      = nullptr;
        m_is_occupied = nullptr;
    }

    /*************************************************************************/
    inline void setup(const std::unordered_map<T_Key, T_Value> &a_UNORDERED_MAP,
                      const std::uint_fast32_t                  a_KEY_SIZE) {
        m_shift_size = floor(log2(a_KEY_SIZE));

        std::uint_fast32_t minimum_bucket_size =
            a_UNORDERED_MAP.size() * FixedSizeHashMapConstant::LOAD_MARGIN;
        std::uint_fast32_t bucket_size = 1;
        while (bucket_size < minimum_bucket_size) {
            bucket_size <<= 1;
        }
        m_bucket_size = bucket_size;
        m_mask        = m_bucket_size - 1;

        m_keys                = new T_Key[m_bucket_size];
        m_values              = new T_Value[m_bucket_size];
        m_is_occupied         = new bool[m_bucket_size];
        m_is_memory_allocated = true;

        for (std::uint_fast32_t i = 0; i < bucket_size; i++) {
            m_keys[i]        = static_cast<T_Key>(0);
            m_values[i]      = static_cast<T_Value>(0);
            m_is_occupied[i] = false;
        }

        for (const auto &item : a_UNORDERED_MAP) {
            this->insert(item.first, item.second);
        }
    }

    /*************************************************************************/
    inline constexpr T_Value at(const T_Key a_KEY) const noexcept {
        std::uint_fast32_t index =
            (reinterpret_cast<std::uint_fast64_t>(a_KEY) >> m_shift_size) &
            m_mask;

        if (!m_is_occupied[index]) {
            return 0;
        }

        while (m_keys[index] != a_KEY) {
            if (!m_is_occupied[index]) {
                return 0;
            }
            index = (index + 1) & m_mask;
        }
        return m_values[index];
    }

    /*************************************************************************/
    inline constexpr std::uint_fast8_t shift_size(void) const {
        return m_shift_size;
    }

    /*************************************************************************/
    inline constexpr std::uint_fast32_t bucket_size(void) const {
        return m_bucket_size;
    }
};
}  // namespace utility
}  // namespace printemps
#endif
/*****************************************************************************/
// END
/*****************************************************************************/