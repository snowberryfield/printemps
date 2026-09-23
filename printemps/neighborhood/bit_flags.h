/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_NEIGHBORHOOD_BIT_FLAGS_H__
#define PRINTEMPS_NEIGHBORHOOD_BIT_FLAGS_H__

namespace printemps::neighborhood {
/*****************************************************************************/
class BitFlags {
   private:
    std::vector<std::uint64_t> m_words;
    int                        m_size;

   public:
    /*************************************************************************/
    /*************************************************************************/
    struct BitReference {
        std::uint64_t &word;
        const int      shift;

        inline BitReference(std::uint64_t &a_word, const int a_SHIFT)
            : word(a_word), shift(a_SHIFT) {
        }

        inline BitReference &operator=(const bool a_VALUE) noexcept {
            if (a_VALUE) {
                this->word |= (1ULL << this->shift);
            } else {
                this->word &= ~(1ULL << this->shift);
            }
            return *this;
        }

        inline BitReference &operator=(const BitReference &a_OTHER) noexcept {
            return *this = static_cast<bool>(a_OTHER);
        }

        inline operator bool() const noexcept {
            return (this->word >> this->shift) & 1ULL;
        }

        inline bool operator==(const int a_VALUE) const noexcept {
            return static_cast<int>((this->word >> this->shift) & 1ULL) ==
                   a_VALUE;
        }

        inline bool operator!=(const int a_VALUE) const noexcept {
            return static_cast<int>((this->word >> this->shift) & 1ULL) !=
                   a_VALUE;
        }
    };

    /*************************************************************************/
    /*************************************************************************/
    struct const_iterator {
        const BitFlags *flags_ptr;
        int             index;

        inline short operator*() const noexcept {
            return (*this->flags_ptr)[this->index];
        }

        inline const_iterator &operator++() noexcept {
            ++this->index;
            return *this;
        }

        inline bool operator!=(const const_iterator &a_OTHER) const noexcept {
            return this->index != a_OTHER.index;
        }

        inline bool operator==(const const_iterator &a_OTHER) const noexcept {
            return this->index == a_OTHER.index;
        }
    };

    /*************************************************************************/
    BitFlags(void) : m_size(0) {
    }

    /*************************************************************************/
    explicit BitFlags(const int a_SIZE) {
        this->resize(a_SIZE);
    }

    /*************************************************************************/
    BitFlags(const std::vector<short> &a_FLAGS) {
        *this = a_FLAGS;
    }

    /*************************************************************************/
    inline void resize(const int a_SIZE) {
        m_size                  = a_SIZE;
        const int NUMBER_OF_WORDS = (a_SIZE + 63) / 64;
        m_words.assign(NUMBER_OF_WORDS, 0ULL);
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
    inline void set_word(const int           a_WORD_INDEX,
                         const std::uint64_t a_WORD) noexcept {
        m_words[a_WORD_INDEX] = a_WORD;
    }

    /*************************************************************************/
    inline void set(const int a_INDEX, const bool a_VALUE) noexcept {
        const int WORD_INDEX = a_INDEX / 64;
        const int BIT_OFFSET = a_INDEX % 64;
        if (a_VALUE) {
            m_words[WORD_INDEX] |= (1ULL << BIT_OFFSET);
        } else {
            m_words[WORD_INDEX] &= ~(1ULL << BIT_OFFSET);
        }
    }

    /*************************************************************************/
    inline bool get(const int a_INDEX) const noexcept {
        return (m_words[a_INDEX / 64] >> (a_INDEX % 64)) & 1ULL;
    }

    /*************************************************************************/
    inline BitReference operator[](const int a_INDEX) noexcept {
        return BitReference(m_words[a_INDEX / 64], a_INDEX % 64);
    }

    /*************************************************************************/
    inline short operator[](const int a_INDEX) const noexcept {
        return static_cast<short>((m_words[a_INDEX / 64] >> (a_INDEX % 64)) &
                                  1ULL);
    }

    /*************************************************************************/
    inline const_iterator begin(void) const noexcept {
        return const_iterator{this, 0};
    }

    /*************************************************************************/
    inline const_iterator end(void) const noexcept {
        return const_iterator{this, m_size};
    }

    /*************************************************************************/
    inline BitFlags &operator=(const std::vector<short> &a_FLAGS) {
        const int SIZE = static_cast<int>(a_FLAGS.size());
        this->resize(SIZE);
        for (int i = 0; i < SIZE; ++i) {
            if (a_FLAGS[i]) {
                this->set(i, true);
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
