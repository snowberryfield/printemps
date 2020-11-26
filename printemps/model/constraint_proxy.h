/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_MODEL_CONSTRAINT_PROXY_H__
#define PRINTEMPS_MODEL_CONSTRAINT_PROXY_H__

#include "abstract_multi_array.h"
#include "constraint.h"

namespace printemps {
namespace model {
/*****************************************************************************/
template <class T_Value>
class ValueProxy;

/*****************************************************************************/
template <class T_Variable, class T_Expression>
class ConstraintProxy : public AbstractMultiArray {
    /**
     * [Access controls for special member functions]
     *  -- Default constructor : default, private
     *  -- Copy constructor    : default, private
     *  -- Copy assignment     : delete,  (private)
     *  -- Move constructor    : default, public
     *  -- Move assignment     : default, public
     */

   private:
    std::vector<Constraint<T_Variable, T_Expression>> m_constraints;

    /*************************************************************************/
    /// Default constructor
    ConstraintProxy(void) {
        /* nothing to do*/
    }

    /*************************************************************************/
    /// Copy constructor
    ConstraintProxy(const ConstraintProxy<T_Variable, T_Expression> &) =
        default;

    /*************************************************************************/
    /// Copy assignment
    ConstraintProxy<T_Variable, T_Expression> &operator    =(
        const ConstraintProxy<T_Variable, T_Expression> &) = delete;

    /*************************************************************************/
    ConstraintProxy(const int a_ID) : AbstractMultiArray(a_ID) {
        this->setup_constraints();
    }

    /*************************************************************************/
    ConstraintProxy(const int a_ID, const int a_NUMBER_OF_ELEMENTS)
        : AbstractMultiArray(a_ID, a_NUMBER_OF_ELEMENTS) {
        this->setup_constraints();
    }

    /*************************************************************************/
    ConstraintProxy(int a_ID, const std::vector<int> &a_SHAPE)
        : AbstractMultiArray(a_ID, a_SHAPE) {
        this->setup_constraints();
    }

    /*************************************************************************/
    inline void setup_constraints(void) {
        /**
         * m_constraints.resize(this->number_of_elements()) does not work
         * because default constructor of Expression class are deleted. for
         * reference:
         * https://cpprefjp.github.io/reference/vector/vector/resize.html
         */
        m_constraints.reserve(this->number_of_elements());
        for (auto i = 0; i < this->number_of_elements(); i++) {
            m_constraints.emplace_back(
                Constraint<T_Variable, T_Expression>::create_instance());
        }

        int flat_index = 0;

        std::vector<int> multi_dimensional_index(this->number_of_dimensions());
        for (auto &&constraint : m_constraints) {
            constraint.set_id(m_id);
            constraint.set_flat_index(flat_index);
            this->update_multi_dimensional_index(&multi_dimensional_index,
                                                 flat_index);
            constraint.set_multi_dimensional_index(multi_dimensional_index);
            flat_index++;
        }
    }

   public:
    /*************************************************************************/
    /// Move constructor
    ConstraintProxy(ConstraintProxy<T_Variable, T_Expression> &&) = default;

    /*************************************************************************/
    /// Move assignment
    ConstraintProxy<T_Variable, T_Expression> &operator=(
        ConstraintProxy<T_Variable, T_Expression> &&) = default;

    /*************************************************************************/
    inline static constexpr ConstraintProxy<T_Variable, T_Expression>
    create_instance(const int a_ID) {
        /**
         * When instantiation, instead of constructor, create_instance() should
         * be called.
         */
        ConstraintProxy<T_Variable, T_Expression> proxy(a_ID);
        return proxy;
    }

    /*************************************************************************/
    inline static constexpr ConstraintProxy<T_Variable, T_Expression>
    create_instance(const int a_ID, const int a_NUMBER_OF_ELEMENTS) {
        /**
         * When instantiation, instead of constructor, create_instance() should
         * be called.
         */
        ConstraintProxy<T_Variable, T_Expression> proxy(a_ID,
                                                        a_NUMBER_OF_ELEMENTS);
        return proxy;
    }

    /*************************************************************************/
    inline static constexpr ConstraintProxy<T_Variable, T_Expression>
    create_instance(const int a_ID, const std::vector<int> &a_SHAPE) {
        /**
         * When instantiation, instead of constructor, create_instance() should
         * be called.
         */
        ConstraintProxy<T_Variable, T_Expression> proxy(a_ID, a_SHAPE);
        return proxy;
    }

    /*************************************************************************/
    inline constexpr void set_name(const std::string &a_NAME) {
        if (this->number_of_elements() != 1) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of elements is not one."));
        }
        m_constraints[0].set_name(a_NAME);
    }

    /*************************************************************************/
    inline constexpr const std::string &name(void) const {
        if (this->number_of_elements() != 1) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of elements is not one."));
        }
        return m_constraints[0].name();
    }

    /*************************************************************************/
    inline constexpr std::vector<Constraint<T_Variable, T_Expression>>
        &flat_indexed_constraints(void) {
        return m_constraints;
    }

    /*************************************************************************/
    inline constexpr const std::vector<Constraint<T_Variable, T_Expression>>
        &flat_indexed_constraints(void) const {
        return m_constraints;
    }

    /*************************************************************************/
    inline constexpr Constraint<T_Variable, T_Expression>
        &flat_indexed_constraints(const int a_FLAT_INDEX) {
        return m_constraints[a_FLAT_INDEX];
    }

    /*************************************************************************/
    inline constexpr const Constraint<T_Variable, T_Expression>
        &flat_indexed_constraints(const int a_FLAT_INDEX) const {
        return m_constraints[a_FLAT_INDEX];
    }

    /*************************************************************************/
    inline constexpr T_Expression constraint_value(void) const {
        if (this->number_of_elements() != 1) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of elements is not one."));
        }
        return m_constraints[0].constraint_value();
    }

    /*************************************************************************/
    inline constexpr T_Expression violation_value(void) const {
        if (this->number_of_elements() != 1) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of elements is not one."));
        }
        return m_constraints[0].violation_value();
    }

    /*************************************************************************/
    inline constexpr ValueProxy<T_Expression> export_values_and_names(
        void) const {
        ValueProxy<T_Expression> proxy(m_id, m_shape);

        int number_of_elements = this->number_of_elements();
        for (auto i = 0; i < number_of_elements; i++) {
            proxy.flat_indexed_values()[i] =
                m_constraints[i].constraint_value();
            proxy.flat_indexed_names()[i] = m_constraints[i].name();
        }
        return proxy;
    }

    /*************************************************************************/
    inline constexpr ValueProxy<T_Expression> export_violations_and_names(
        void) const {
        ValueProxy<T_Expression> proxy(m_id, m_shape);

        int number_of_elements = this->number_of_elements();
        for (auto i = 0; i < number_of_elements; i++) {
            proxy.flat_indexed_values()[i] = m_constraints[i].violation_value();
            proxy.flat_indexed_names()[i]  = m_constraints[i].name();
        }
        return proxy;
    }

    /*************************************************************************/
    inline constexpr bool is_enabled(void) const {
        if (this->number_of_elements() != 1) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of elements is not one."));
        }
        return m_constraints[0].is_enabled();
    }

    /*************************************************************************/
    inline constexpr void enable(void) {
        /// This method enables all expressions simultaneously.
        for (auto &&constraint : m_constraints) {
            constraint.enable();
        }
    }

    /*************************************************************************/
    inline constexpr void disable(void) {
        /// This method disables all expressions simultaneously.
        for (auto &&constraint : m_constraints) {
            constraint.disable();
        }
    }

    /*************************************************************************/
    inline constexpr Constraint<T_Variable, T_Expression> &operator[](
        int a_FLAT_INDEX) {
        return m_constraints[a_FLAT_INDEX];
    }

    /*************************************************************************/
    inline constexpr const Constraint<T_Variable, T_Expression> &operator[](
        int a_FLAT_INDEX) const {
        return m_constraints[a_FLAT_INDEX];
    }

    /*************************************************************************/
    inline constexpr Constraint<T_Variable, T_Expression> &operator()(
        const std::vector<int> &a_MULTI_DIMENSIONAL_INDEX) {
        int multi_dimensional_index_size = a_MULTI_DIMENSIONAL_INDEX.size();
        if (this->number_of_dimensions() != multi_dimensional_index_size) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of dimensions does not match."));
        }

        auto flat_index = std::inner_product(a_MULTI_DIMENSIONAL_INDEX.begin(),
                                             a_MULTI_DIMENSIONAL_INDEX.end(),
                                             m_strides.begin(), 0);

        return m_constraints[flat_index];
    }

    /*************************************************************************/
    inline constexpr const Constraint<T_Variable, T_Expression> &operator()(
        const std::vector<int> &a_MULTI_DIMENSIONAL_INDEX) const {
        if (this->number_of_dimensions() != a_MULTI_DIMENSIONAL_INDEX.size()) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of dimensions does not match."));
        }

        auto flat_index = std::inner_product(a_MULTI_DIMENSIONAL_INDEX.begin(),
                                             a_MULTI_DIMENSIONAL_INDEX.end(),
                                             m_strides.begin(), 0);

        return m_constraints[flat_index];
    }

    /*************************************************************************/
    template <class... Args>
    inline constexpr Constraint<T_Variable, T_Expression> &operator()(
        Args... args) {
        return this->operator()({args...});
    }

    /*************************************************************************/
    template <class... Args>
    inline constexpr const Constraint<T_Variable, T_Expression> &operator()(
        Args... args) const {
        return this->operator()({args...});
    }

    /*************************************************************************/
    inline constexpr ConstraintProxy<T_Variable, T_Expression> &operator=(
        const Constraint<T_Variable, T_Expression> &a_CONSTRAINT) {
        if (this->number_of_elements() != 1) {
            throw std::logic_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The number of elements is not one."));
        }
        m_constraints[0] = a_CONSTRAINT;
        return *this;
    }
};
using IPConstraintProxy = ConstraintProxy<int, double>;
}  // namespace model
}  // namespace printemps
#endif
/*****************************************************************************/
// END
/*****************************************************************************/