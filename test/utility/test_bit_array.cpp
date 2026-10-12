/*****************************************************************************/
// Copyright (c) 2020-2026 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#include <gtest/gtest.h>
#include <printemps.h>

namespace {
using namespace printemps;
/*****************************************************************************/
class TestBitArray : public ::testing::Test {
   protected:
    virtual void SetUp(void) {
        /// nothing to do
    }
    virtual void TearDown(void) {
        /// nothing to do
    }
};

/*****************************************************************************/
TEST_F(TestBitArray, initialize) {
    /// Default constructor
    utility::BitArray array_default;
    EXPECT_EQ(0, array_default.size());
    EXPECT_TRUE(array_default.empty());
    EXPECT_TRUE(array_default.words().empty());

    /// Size constructor
    constexpr int     SIZE = 130;  // requires 3 words (64 * 2 + 2)
    utility::BitArray array_sized(SIZE);
    EXPECT_EQ(SIZE, array_sized.size());
    EXPECT_FALSE(array_sized.empty());
    EXPECT_EQ(3u, array_sized.words().size());
    for (int i = 0; i < SIZE; i++) {
        EXPECT_FALSE(array_sized.get(i));
    }

    /// Vector constructor
    std::vector<short> vec = {1, 0, 1, 1, 0};
    utility::BitArray  array_vec(vec);
    EXPECT_EQ(5, array_vec.size());
    EXPECT_TRUE(array_vec.get(0));
    EXPECT_FALSE(array_vec.get(1));
    EXPECT_TRUE(array_vec.get(2));
    EXPECT_TRUE(array_vec.get(3));
    EXPECT_FALSE(array_vec.get(4));
}

/*****************************************************************************/
TEST_F(TestBitArray, resize_and_clear) {
    utility::BitArray array;

    array.resize(64);
    EXPECT_EQ(64, array.size());
    EXPECT_EQ(1u, array.words().size());

    array.resize(65);
    EXPECT_EQ(65, array.size());
    EXPECT_EQ(2u, array.words().size());

    array.clear();
    EXPECT_EQ(0, array.size());
    EXPECT_TRUE(array.empty());
    EXPECT_TRUE(array.words().empty());
}

/*****************************************************************************/
TEST_F(TestBitArray, set_and_get) {
    utility::BitArray array(150);

    // Initial state: all false
    EXPECT_FALSE(array.get(0));
    EXPECT_FALSE(array.get(63));
    EXPECT_FALSE(array.get(64));
    EXPECT_FALSE(array.get(127));
    EXPECT_FALSE(array.get(128));

    // Set true
    array.set(0, true);
    array.set(63, true);
    array.set(64, true);
    array.set(128, true);

    EXPECT_TRUE(array.get(0));
    EXPECT_TRUE(array.get(63));
    EXPECT_TRUE(array.get(64));
    EXPECT_FALSE(array.get(127));
    EXPECT_TRUE(array.get(128));

    // Set false
    array.set(63, false);
    array.set(64, false);
    EXPECT_FALSE(array.get(63));
    EXPECT_FALSE(array.get(64));
}

/*****************************************************************************/
TEST_F(TestBitArray, subscript_operator) {
    utility::BitArray array(100);

    array.set(10, true);
    array.set(20, false);
    array.set(70, true);  // In the second word

    EXPECT_EQ(1, array[10]);
    EXPECT_EQ(0, array[20]);
    EXPECT_EQ(1, array[70]);

    const auto &const_array = array;
    EXPECT_EQ(1, const_array[10]);
    EXPECT_EQ(0, const_array[20]);
    EXPECT_EQ(1, const_array[70]);
}

/*****************************************************************************/
TEST_F(TestBitArray, words_and_set_word) {
    utility::BitArray array(128);

    array.set_word(0, 0x0000000000000005ULL);  // Bits 0 and 2 set
    array.set_word(1, 0x8000000000000000ULL);  // Bit 127 (64 + 63) set

    EXPECT_TRUE(array.get(0));
    EXPECT_FALSE(array.get(1));
    EXPECT_TRUE(array.get(2));
    EXPECT_FALSE(array.get(3));
    EXPECT_TRUE(array.get(127));

    const auto &words = array.words();
    EXPECT_EQ(0x0000000000000005ULL, words[0]);
    EXPECT_EQ(0x8000000000000000ULL, words[1]);
}

/*****************************************************************************/
TEST_F(TestBitArray, set_bit_to_word_and_set_bits_to_word) {
    std::uint64_t word = 0;

    // set_bit_to_word (set true)
    utility::BitArray::set_bit_to_word(&word, 0);
    utility::BitArray::set_bit_to_word(&word, 3);
    utility::BitArray::set_bit_to_word(&word, 63);
    EXPECT_EQ(0x8000000000000009ULL, word);

    // set_bit_to_word (set false)
    utility::BitArray::set_bit_to_word(&word, 3, false);
    EXPECT_EQ(0x8000000000000001ULL, word);

    // set_bits_to_word
    word = 0;
    utility::BitArray::set_bits_to_word(&word, 4, 0x0FULL);  // Bits 4..7
    EXPECT_EQ(0x00000000000000F0ULL, word);

    utility::BitArray::set_bits_to_word(&word, 8, 0x03ULL);  // Bits 8..9
    EXPECT_EQ(0x00000000000003F0ULL, word);
}

/*****************************************************************************/
TEST_F(TestBitArray, set_bit_in_word_and_set_bits_in_word) {
    utility::BitArray array(130);

    // set_bit_in_word
    array.set_bit_in_word(0, 5, true);
    array.set_bit_in_word(1, 10, true);  // In word 1 (bit 64 + 10 = 74)
    EXPECT_TRUE(array.get(5));
    EXPECT_TRUE(array.get(74));

    array.set_bit_in_word(0, 5, false);
    EXPECT_FALSE(array.get(5));

    // set_bits_in_word
    array.set_bits_in_word(0, 16, 0x05ULL);  // Bits 16 (0x1) and 18 (0x4)
    EXPECT_TRUE(array.get(16));
    EXPECT_FALSE(array.get(17));
    EXPECT_TRUE(array.get(18));
}

/*****************************************************************************/
TEST_F(TestBitArray, vector_assignment_operator) {
    utility::BitArray  array;
    std::vector<short> vec = {0, 1, 0, 1, 1};

    array = vec;
    EXPECT_EQ(5, array.size());
    EXPECT_FALSE(array.get(0));
    EXPECT_TRUE(array.get(1));
    EXPECT_FALSE(array.get(2));
    EXPECT_TRUE(array.get(3));
    EXPECT_TRUE(array.get(4));
}

}  // namespace
/*****************************************************************************/
// END
/*****************************************************************************/
