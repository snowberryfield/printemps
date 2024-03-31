/*****************************************************************************/
// Copyright (c) 2020-2024 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_MULTI_ARRAY_ABSTRACT_MULTI_ARRAY_ELEMENT_H__
#define PRINTEMPS_MULTI_ARRAY_ABSTRACT_MULTI_ARRAY_ELEMENT_H__

namespace printemps::multi_array {
/*****************************************************************************/
class AbstractMultiArrayElement {
   protected:
    int              m_proxy_index;
    int              m_flat_index;
    std::vector<int> m_multi_dimensional_index;
    std::string      m_name;

    /*************************************************************************/
    AbstractMultiArrayElement(void) {
        this->initialize();
    }

    /*************************************************************************/
    void initialize(void) {
        m_proxy_index             = 0;
        m_flat_index              = 0;
        m_multi_dimensional_index = {0};
        m_name                    = "";
    }

   public:
    /*************************************************************************/
    inline void set_proxy_index(const int a_proxy_index) {
        m_proxy_index = a_proxy_index;
    }

    /*************************************************************************/
    inline int proxy_index(void) const {
        return m_proxy_index;
    }

    /*************************************************************************/
    inline void set_flat_index(const int a_FLAT_INDEX) {
        m_flat_index = a_FLAT_INDEX;
    }

    /*************************************************************************/
    inline int flat_index(void) const {
        return m_flat_index;
    }

    /*************************************************************************/
    inline void set_multi_dimensional_index(
        const std::vector<int> &a_MULTI_DIMENSIONAL_INDEX) {
        m_multi_dimensional_index = a_MULTI_DIMENSIONAL_INDEX;
    }

    /*************************************************************************/
    inline const std::vector<int> &multi_dimensional_index(void) const {
        return m_multi_dimensional_index;
    }

    /*************************************************************************/
    inline void set_name(const std::string &a_NAME) {
        m_name = a_NAME;
    }

    /*************************************************************************/
    inline const std::string &name(void) const {
        return m_name;
    }
};
}  // namespace printemps::multi_array
#endif
/*****************************************************************************/
// END
/*****************************************************************************/