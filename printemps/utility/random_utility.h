/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_UTILITY_RANDOM_UTILITY_H__
#define PRINTEMPS_UTILITY_RANDOM_UTILITY_H__

namespace printemps {
namespace utility {
/*****************************************************************************/
class IntegerUniformRandom {
   private:
    std::mt19937                     m_engine;
    std::uniform_int_distribution<> *m_distribution;

   public:
    /*************************************************************************/
    IntegerUniformRandom(void) {
        /// nothing to do
    }

    /*************************************************************************/
    IntegerUniformRandom(const int a_MIN, const int a_MAX,
                         const unsigned int a_SEED) {
        this->setup(a_MIN, a_MAX, a_SEED);
    }

    /*************************************************************************/
    virtual ~IntegerUniformRandom(void) {
        delete m_distribution;
    }

    /*************************************************************************/
    void setup(const int a_MIN, const int a_MAX, const unsigned int a_SEED) {
        m_distribution = new std::uniform_int_distribution<>(a_MIN, a_MAX);
        m_engine.seed(a_SEED);
    }

    /*************************************************************************/
    inline void seed(const unsigned int a_SEED) {
        m_engine.seed(a_SEED);
    }

    /*************************************************************************/
    inline int generate_random() {
        return (*m_distribution)(m_engine);
    }
};
}  // namespace utility
}  // namespace printemps

#endif
/*****************************************************************************/
// END
/*****************************************************************************/