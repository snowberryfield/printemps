/*****************************************************************************/
// Copyright (c) 2020-2026 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_UTILITY_BIT_ARRAY_H__
#define PRINTEMPS_UTILITY_BIT_ARRAY_H__

namespace printemps::utility {
/*****************************************************************************/
class BitArray {
   public:
    static constexpr int WORD_SHIFT = 6;
    static constexpr int BIT_MASK   = 63;

   private:
    std::vector<std::uint64_t> m_words;
    int                        m_size;

   public:
    /*************************************************************************/
    BitArray(void) : m_size(0) {
    }

    /*************************************************************************/
    explicit BitArray(const int a_SIZE) {
        this->resize(a_SIZE);
    }

    /*************************************************************************/
    BitArray(const std::vector<short> &a_FLAGS) {
        *this = a_FLAGS;
    }

    /*************************************************************************/
    inline void resize(const int a_SIZE) {
        m_size                    = a_SIZE;
        const int NUMBER_OF_WORDS = (a_SIZE + BIT_MASK) >> WORD_SHIFT;
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
    static inline void set_bit_to_word(std::uint64_t *a_word_ptr,
                                       const int      a_OFFSET,
                                       const bool     a_VALUE = true) noexcept {
        if (a_VALUE) {
            *a_word_ptr |= (1ULL << a_OFFSET);
        } else {
            *a_word_ptr &= ~(1ULL << a_OFFSET);
        }
    }

    /*************************************************************************/
    static inline void set_bits_to_word(std::uint64_t      *a_word_ptr,
                                        const int           a_OFFSET,
                                        const std::uint64_t a_BITS) noexcept {
        *a_word_ptr |= (a_BITS << a_OFFSET);
    }

    /*************************************************************************/
    inline void set_bit_in_word(const int  a_WORD_INDEX,  //
                                const int  a_OFFSET,      //
                                const bool a_VALUE = true) noexcept {
        this->set_bit_to_word(&m_words[a_WORD_INDEX], a_OFFSET, a_VALUE);
    }

    /*************************************************************************/
    inline void set_bits_in_word(const int           a_WORD_INDEX,  //
                                 const int           a_OFFSET,      //
                                 const std::uint64_t a_BITS) noexcept {
        this->set_bits_to_word(&m_words[a_WORD_INDEX], a_OFFSET, a_BITS);
    }

    /*************************************************************************/
    inline void set(const int a_INDEX, const bool a_VALUE) noexcept {
        const int WORD_INDEX = a_INDEX >> WORD_SHIFT;
        const int BIT_OFFSET = a_INDEX & BIT_MASK;
        this->set_bit_to_word(&m_words[WORD_INDEX], BIT_OFFSET, a_VALUE);
    }

    /*************************************************************************/
    inline bool get(const int a_INDEX) const noexcept {
        return (m_words[a_INDEX >> WORD_SHIFT] >> (a_INDEX & BIT_MASK)) & 1ULL;
    }

    /*************************************************************************/
    inline short operator[](const int a_INDEX) const noexcept {
        return static_cast<short>(
            (m_words[a_INDEX >> WORD_SHIFT] >> (a_INDEX & BIT_MASK)) & 1ULL);
    }

    /*************************************************************************/
    inline BitArray &operator=(const std::vector<short> &a_FLAGS) {
        const int SIZE = static_cast<int>(a_FLAGS.size());
        this->resize(SIZE);
        for (int i = 0; i < SIZE; i++) {
            if (a_FLAGS[i]) {
                this->set(i, true);
            }
        }
        return *this;
    }
};

using BitFlags = BitArray;

}  // namespace printemps::utility

#endif
/*****************************************************************************/
// END
/*****************************************************************************/
