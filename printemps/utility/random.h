/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_UTILITY_RANDOM_H__
#define PRINTEMPS_UTILITY_RANDOM_H__

namespace printemps {
namespace utility {
/*****************************************************************************/
template <class T_Generator, class T_Value>
class UniformRandom {
   private:
    std::mt19937 m_engine;
    T_Generator  m_distribution;

   public:
    /*************************************************************************/
    UniformRandom(void) {
        /// nothing to do
    }

    /*************************************************************************/
    UniformRandom(const T_Value a_MIN, const T_Value a_MAX,
                  const unsigned int a_SEED) {
        this->setup(a_MIN, a_MAX, a_SEED);
    }

    /*************************************************************************/
    virtual ~UniformRandom(void) {
        /// nothing to do
    }

    /*************************************************************************/
    void setup(const T_Value a_MIN, const T_Value a_MAX,
               const unsigned int a_SEED) {
        m_distribution = T_Generator(a_MIN, a_MAX);
        m_engine.seed(a_SEED);
    }

    /*************************************************************************/
    inline void seed(const unsigned int a_SEED) {
        m_engine.seed(a_SEED);
    }

    /*************************************************************************/
    inline T_Value generate_random() {
        return m_distribution(m_engine);
    }
};
}  // namespace utility
}  // namespace printemps

#endif
/*****************************************************************************/
// END
/*****************************************************************************/