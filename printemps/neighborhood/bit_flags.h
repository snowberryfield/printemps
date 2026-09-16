/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_NEIGHBORHOOD_BIT_FLAGS_H__
#define PRINTEMPS_NEIGHBORHOOD_BIT_FLAGS_H__

#include <cstdint>
#include <vector>

namespace printemps::neighborhood {
/*****************************************************************************/
class BitFlags {
   private:
    std::vector<std::uint64_t> m_words;
    int                        m_size;

   public:
    /*************************************************************************/
    struct BitReference {
        std::uint64_t &word;
        const int      shift;

        inline BitReference(std::uint64_t &a_word, const int a_shift)
            : word(a_word), shift(a_shift) {
        }

        inline BitReference &operator=(const bool a_val) noexcept {
            if (a_val) {
                word |= (1ULL << shift);
            } else {
                word &= ~(1ULL << shift);
            }
            return *this;
        }

        inline BitReference &operator=(const BitReference &a_other) noexcept {
            return *this = static_cast<bool>(a_other);
        }

        inline operator bool() const noexcept {
            return (word >> shift) & 1ULL;
        }

        inline bool operator==(const int a_val) const noexcept {
            return static_cast<int>((word >> shift) & 1ULL) == a_val;
        }

        inline bool operator!=(const int a_val) const noexcept {
            return static_cast<int>((word >> shift) & 1ULL) != a_val;
        }
    };

    /*************************************************************************/
    struct const_iterator {
        const BitFlags *flags_ptr;
        int             index;

        inline short operator*() const noexcept {
            return (*flags_ptr)[index];
        }

        inline const_iterator &operator++() noexcept {
            ++index;
            return *this;
        }

        inline bool operator!=(const const_iterator &a_other) const noexcept {
            return index != a_other.index;
        }

        inline bool operator==(const const_iterator &a_other) const noexcept {
            return index == a_other.index;
        }
    };

    /*************************************************************************/
    BitFlags(void) : m_size(0) {
    }

    /*************************************************************************/
    explicit BitFlags(const int a_size) {
        this->resize(a_size);
    }

    /*************************************************************************/
    BitFlags(const std::vector<short> &a_flags) {
        *this = a_flags;
    }

    /*************************************************************************/
    inline void resize(const int a_size) {
        m_size = a_size;
        const int num_words = (a_size + 63) / 64;
        m_words.assign(num_words, 0ULL);
    }

    /*************************************************************************/
    inline void clear(void) noexcept {
        m_size = 0;
        m_words.clear();
    }

    /*************************************************************************/
    inline int size(void) const noexcept {
        return m_size;
    }

    /*************************************************************************/
    inline bool empty(void) const noexcept {
        return m_size == 0;
    }

    /*************************************************************************/
    inline std::vector<std::uint64_t> &words(void) noexcept {
        return m_words;
    }

    /*************************************************************************/
    inline const std::vector<std::uint64_t> &words(void) const noexcept {
        return m_words;
    }

    /*************************************************************************/
    inline void set_word(const int           a_word_index,
                         const std::uint64_t a_word) noexcept {
        m_words[a_word_index] = a_word;
    }

    /*************************************************************************/
    inline void set(const int a_index, const bool a_value) noexcept {
        const int w = a_index / 64;
        const int b = a_index % 64;
        if (a_value) {
            m_words[w] |= (1ULL << b);
        } else {
            m_words[w] &= ~(1ULL << b);
        }
    }

    /*************************************************************************/
    inline bool get(const int a_index) const noexcept {
        return (m_words[a_index / 64] >> (a_index % 64)) & 1ULL;
    }

    /*************************************************************************/
    inline BitReference operator[](const int a_index) noexcept {
        return BitReference(m_words[a_index / 64], a_index % 64);
    }

    /*************************************************************************/
    inline short operator[](const int a_index) const noexcept {
        return static_cast<short>((m_words[a_index / 64] >> (a_index % 64)) &
                                  1ULL);
    }

    /*************************************************************************/
    inline const_iterator begin() const noexcept {
        return const_iterator{this, 0};
    }

    /*************************************************************************/
    inline const_iterator end() const noexcept {
        return const_iterator{this, m_size};
    }

    /*************************************************************************/
    inline BitFlags &operator=(const std::vector<short> &a_flags) {
        this->resize(a_flags.size());
        for (size_t i = 0; i < a_flags.size(); ++i) {
            if (a_flags[i]) {
                this->set(static_cast<int>(i), true);
            }
        }
        return *this;
    }
};
}  // namespace printemps::neighborhood

/*****************************************************************************/
// END
/*****************************************************************************/
#endif
