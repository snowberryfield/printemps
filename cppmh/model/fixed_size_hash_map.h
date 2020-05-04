/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef CPPMH_MODEL_FIXED_SIZE_HASH_MAP__
#define CPPMH_MODEL_FIXED_SIZE_HASH_MAP__

#include <vector>
#include <unordered_map>
#include <cmath>
#include <iostream>

namespace cppmh {
namespace model {
/*****************************************************************************/
struct FixedSizeHashMapConstant {
    static constexpr std::size_t DEFAULT_BUCKET_SIZE = 16;
    static constexpr int         LOAD_MARGIN         = 2;
};

/*****************************************************************************/
template <class T_Key, class T_Value>
class FixedSizeHashMap {
   private:
    unsigned int m_shift_size;
    std::size_t  m_bucket_size;

    std::vector<T_Key>   m_keys;
    std::vector<T_Value> m_values;
    std::vector<bool>    m_is_occupied;

    /*************************************************************************/
    inline constexpr std::size_t compute_hash(const T_Key a_KEY) const
        noexcept {
        return reinterpret_cast<std::size_t>(a_KEY) >> m_shift_size;
    }

    /*************************************************************************/
    inline constexpr std::size_t compute_index(const std::size_t a_HASH) const
        noexcept {
        return a_HASH & (m_bucket_size - 1);
    }

    /*************************************************************************/
    inline void insert(const T_Key a_KEY, const T_Value a_VALUE) {
        /**
         * This method is provided as private and is called only by setup().
         */
        std::size_t index = this->compute_index(this->compute_hash(a_KEY));
        while (m_is_occupied[index]) {
            index = compute_index(index + 1);
        }

        m_is_occupied[index] = true;
        m_keys[index]        = a_KEY;
        m_values[index]      = a_VALUE;
    }

   public:
    /*************************************************************************/
    FixedSizeHashMap(void) {
        this->initialize();
    }

    /*************************************************************************/
    FixedSizeHashMap(const std::unordered_map<T_Key, T_Value> &a_UNORDERED_MAP,
                     const std::size_t                         a_KEY_SIZE) {
        this->setup(a_UNORDERED_MAP, a_KEY_SIZE);
    }

    /*************************************************************************/
    inline constexpr void initialize(void) {
        m_shift_size  = 0;
        m_bucket_size = FixedSizeHashMapConstant::DEFAULT_BUCKET_SIZE;
        m_keys.resize(m_bucket_size, 0);
        m_values.resize(m_bucket_size, 0);
        m_is_occupied.resize(m_bucket_size, 0);

        std::fill(m_keys.begin(), m_keys.end(), static_cast<T_Key>(0));
        std::fill(m_values.begin(), m_values.end(), static_cast<T_Value>(0));
        std::fill(m_is_occupied.begin(), m_is_occupied.end(), false);
    }

    /*************************************************************************/
    inline void setup(const std::unordered_map<T_Key, T_Value> &a_UNORDERED_MAP,
                      const std::size_t                         a_KEY_SIZE) {
        m_shift_size = floor(log2(a_KEY_SIZE));

        std::size_t minimum_bucket_size =
            a_UNORDERED_MAP.size() * FixedSizeHashMapConstant::LOAD_MARGIN;
        std::size_t bucket_size = 1;
        while (bucket_size < minimum_bucket_size) {
            bucket_size <<= 1;
        }
        m_bucket_size = bucket_size;

        m_keys.resize(m_bucket_size);
        m_values.resize(m_bucket_size);
        m_is_occupied.resize(m_bucket_size);

        std::fill(m_keys.begin(), m_keys.end(), static_cast<T_Key>(0));
        std::fill(m_values.begin(), m_values.end(), static_cast<T_Value>(0));
        std::fill(m_is_occupied.begin(), m_is_occupied.end(), false);

        for (const auto &item : a_UNORDERED_MAP) {
            this->insert(item.first, item.second);
        }
    }

    /*************************************************************************/
    inline constexpr const T_Value at(const T_Key a_KEY) const noexcept {
        std::size_t index = this->compute_index(this->compute_hash(a_KEY));
        if (!m_is_occupied[index]) {
            return 0;
        }

        while (m_keys[index] != a_KEY) {
            if (!m_is_occupied[index]) {
                return 0;
            }
            index = compute_index(index + 1);
        }
        return m_values[index];
    }

    /*************************************************************************/
    inline constexpr unsigned int shift_size(void) const {
        return m_shift_size;
    }

    /*************************************************************************/
    inline constexpr std::size_t bucket_size(void) const {
        return m_bucket_size;
    }

    /*************************************************************************/
    inline constexpr const std::vector<T_Key> &keys(void) const {
        return m_keys;
    }

    /*************************************************************************/
    inline constexpr const std::vector<T_Value> &values(void) const {
        return m_values;
    }

    /*************************************************************************/
    inline constexpr const std::vector<bool> &is_occupied(void) const {
        return m_is_occupied;
    }
};
}  // namespace model
}  // namespace cppmh
#endif
/*****************************************************************************/
// END
/*****************************************************************************/