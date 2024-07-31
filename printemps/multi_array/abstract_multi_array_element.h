/*****************************************************************************/
// Copyright (c) 2020-2024 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_MULTI_ARRAY_ABSTRACT_MULTI_ARRAY_ELEMENT_H__
#define PRINTEMPS_MULTI_ARRAY_ABSTRACT_MULTI_ARRAY_ELEMENT_H__

namespace printemps::multi_array {
/*****************************************************************************/
struct AbstractMultiArrayElementExtension {
    int              proxy_index;
    int              flat_index;
    std::vector<int> multi_dimensional_index;
    std::string      name;
};

/*****************************************************************************/
class AbstractMultiArrayElement {
   protected:
    std::shared_ptr<AbstractMultiArrayElementExtension> m_extension;

    /*************************************************************************/
    AbstractMultiArrayElement(void)
        : m_extension(std::make_shared<AbstractMultiArrayElementExtension>()) {
        this->initialize();
    }

    /*************************************************************************/
    void initialize(void) {
        m_extension->proxy_index             = 0;
        m_extension->flat_index              = 0;
        m_extension->multi_dimensional_index = {0};
        m_extension->name                    = "";
    }

   public:
    /*************************************************************************/
    inline void set_proxy_index(const int a_proxy_index) {
        m_extension->proxy_index = a_proxy_index;
    }

    /*************************************************************************/
    inline int proxy_index(void) const {
        return m_extension->proxy_index;
    }

    /*************************************************************************/
    inline void set_flat_index(const int a_FLAT_INDEX) {
        m_extension->flat_index = a_FLAT_INDEX;
    }

    /*************************************************************************/
    inline int flat_index(void) const {
        return m_extension->flat_index;
    }

    /*************************************************************************/
    inline void set_multi_dimensional_index(
        const std::vector<int> &a_MULTI_DIMENSIONAL_INDEX) {
        m_extension->multi_dimensional_index = a_MULTI_DIMENSIONAL_INDEX;
    }

    /*************************************************************************/
    inline const std::vector<int> &multi_dimensional_index(void) const {
        return m_extension->multi_dimensional_index;
    }

    /*************************************************************************/
    inline void set_name(const std::string &a_NAME) {
        m_extension->name = a_NAME;
    }

    /*************************************************************************/
    inline const std::string &name(void) const {
        return m_extension->name;
    }
};
}  // namespace printemps::multi_array
#endif
/*****************************************************************************/
// END
/*****************************************************************************/