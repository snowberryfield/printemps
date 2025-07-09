/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_MODEL_COMPONENT_VARIABLE_H__
#define PRINTEMPS_MODEL_COMPONENT_VARIABLE_H__

namespace printemps::neighborhood {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
struct Move;
}  // namespace printemps::neighborhood

namespace printemps::model_component {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class Expression;

/*****************************************************************************/
template <class T_Variable, class T_Expression>
class Constraint;

/*****************************************************************************/
template <class T_Variable, class T_Expression>
struct Selection;

/*****************************************************************************/
struct VariableConstant {
    static constexpr int INITIAL_LOCAL_LAST_UPDATE_ITERATION = -1000;
};

/*****************************************************************************/
template <class T_Variable, class T_Expression>
struct VariableExtension {
    VariableSense                        sense;
    double                               lagrangian_coefficient;
    Selection<T_Variable, T_Expression> *selection_ptr;

    std::vector<Constraint<T_Variable, T_Expression> *> related_constraint_ptrs;
    std::vector<Constraint<T_Variable, T_Expression> *>
        related_binary_coefficient_constraint_ptrs;
    Expression<T_Variable, T_Expression> *dependent_expression_ptr;
    std::vector<std::pair<Constraint<T_Variable, T_Expression> *, T_Expression>>
        constraint_sensitivities;

    T_Expression objective_sensitivity;

    std::uint64_t hash;
    int           related_selection_constraint_ptr_index_min;
    int           related_selection_constraint_ptr_index_max;
};

/*****************************************************************************/

class InfeasibleError : public std::runtime_error {
public:
    explicit InfeasibleError(const std::string& msg)
        : std::runtime_error(msg) {}
};

/*****************************************************************************/
template <class T_Variable, class T_Expression>
class Variable : public multi_array::AbstractMultiArrayElement {
    /**
     * [Access controls for special member functions]
     *  -- Default constructor : default, private
     *  -- Copy constructor    : delete, (private)
     *  -- Copy assignment     : delete, (private)
     *  -- Move constructor    : default, public
     *  -- Move assignment     : default, public
     */

   private:
    T_Variable m_value;
    T_Variable m_lower_bound;
    T_Variable m_upper_bound;
    int        m_local_last_update_iteration;
    int        m_global_last_update_iteration;
    long       m_update_count;

    std::unique_ptr<VariableExtension<T_Variable, T_Expression>> m_extension;

    bool m_has_bounds;
    bool m_is_fixed;
    bool m_is_objective_improvable;
    bool m_is_feasibility_improvable;
    bool m_has_lower_bound_margin;
    bool m_has_upper_bound_margin;

    /*************************************************************************/
    /// Default constructor
    Variable(void)
        : m_extension(
              std::make_unique<VariableExtension<T_Variable, T_Expression>>()) {
        this->initialize();
    }

    /*************************************************************************/
    /// Copy constructor
    Variable(const Variable<T_Variable, T_Expression> &) = delete;

    /*************************************************************************/
    /// Copy assignment
    Variable<T_Variable, T_Expression> &operator=(
        const Variable<T_Variable, T_Expression> &) = delete;

   public:
    /*************************************************************************/
    /// Move constructor
    Variable(Variable<T_Variable, T_Expression> &&) = default;

    /*************************************************************************/
    /// Move assignment
    Variable<T_Variable, T_Expression> &operator=(
        Variable<T_Variable, T_Expression> &&) = default;

    /*************************************************************************/
    inline static Variable<T_Variable, T_Expression> create_instance(void) {
        /**
         * When instantiation, instead of constructor, create_instance() should
         * be called.
         */
        Variable<T_Variable, T_Expression> variable;
        return variable;
    }

    /*************************************************************************/
    void initialize(void) {
        multi_array::AbstractMultiArrayElement::initialize();
        m_is_fixed = false;
        m_value    = 0;

        /**
         * The default lower and upper bounds of a variable are set by
         * sufficiently large negative and positive integer, respectively. The
         * default bounds have margin to avoid overflows in calculating moves.
         */
        m_lower_bound = constant::INT_HALF_MIN;
        m_upper_bound = constant::INT_HALF_MAX;
        m_has_bounds  = false;

        m_is_objective_improvable   = false;
        m_is_feasibility_improvable = false;

        m_has_lower_bound_margin = true;
        m_has_upper_bound_margin = true;

        m_local_last_update_iteration =
            VariableConstant::INITIAL_LOCAL_LAST_UPDATE_ITERATION;
        m_global_last_update_iteration = 0;
        m_update_count                 = 0;

        m_extension->sense                  = VariableSense::Integer;
        m_extension->lagrangian_coefficient = 0.0;
        m_extension->selection_ptr          = nullptr;
        m_extension->related_constraint_ptrs.clear();
        m_extension->related_binary_coefficient_constraint_ptrs.clear();
        m_extension->dependent_expression_ptr = nullptr;
        m_extension->constraint_sensitivities.clear();
        m_extension->objective_sensitivity                      = 0.0;
        m_extension->hash                                       = 0;
        m_extension->related_selection_constraint_ptr_index_min = -1;
        m_extension->related_selection_constraint_ptr_index_max = -1;
    }

    /*************************************************************************/
    inline void set_value_force(const T_Variable a_VALUE) {
        m_value = a_VALUE;
        this->update_margin();
    }

    /*************************************************************************/
    inline void set_value_if_mutable(const T_Variable a_VALUE) {
        if (!m_is_fixed) {
            m_value = a_VALUE;
            this->update_margin();
        }
    }

    /*************************************************************************/
    inline T_Variable value(void) const noexcept {
        return m_value;
    }

    /*************************************************************************/
    inline void set_value(T_Variable a_VALUE) {
        if (m_value != a_VALUE && m_is_fixed) {
            throw std::runtime_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "A fixed variable was attempted to be changed."));
        }
        m_value = a_VALUE;
        this->update_margin();
    }

    /*************************************************************************/
    inline T_Expression evaluate(void) const noexcept {
        return this->value();
    }

    /*************************************************************************/
    inline T_Expression evaluate(
        const neighborhood::Move<T_Variable, T_Expression> &a_MOVE)
        const noexcept {
        if (a_MOVE.alterations.size() == 0) {
            return this->value();
        }

        for (const auto &alteration : a_MOVE.alterations) {
            if (this == alteration.first) {
                return alteration.second;
            }
        }
        return this->value();
    }

    /*************************************************************************/
    inline void set_bound(const T_Variable a_LOWER_BOUND,
                          const T_Variable a_UPPER_BOUND) {
        if (a_LOWER_BOUND > a_UPPER_BOUND) {
            throw InfeasibleError(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "The specified lower bound is bigger than the specified upper "
                "bound. lower bound: " +
                    std::to_string(a_LOWER_BOUND) +
                    ", upper bound: " + std::to_string(a_UPPER_BOUND)));
        }
        m_lower_bound = a_LOWER_BOUND;
        m_upper_bound = a_UPPER_BOUND;
        m_has_bounds  = true;
        m_value       = std::min(m_value, m_upper_bound);
        m_value       = std::max(m_value, m_lower_bound);

        this->setup_sense_binary_or_integer();
        this->update_margin();
    }

    /*************************************************************************/
    inline void reset_bound(void) {
        m_lower_bound              = constant::INT_HALF_MIN;
        m_upper_bound              = constant::INT_HALF_MAX;
        m_has_bounds               = false;
        m_extension->sense         = VariableSense::Integer;
        m_extension->selection_ptr = nullptr;
        this->update_margin();
    }

    /*************************************************************************/
    inline T_Variable lower_bound(void) const noexcept {
        return m_lower_bound;
    }

    /*************************************************************************/
    inline T_Variable upper_bound(void) const noexcept {
        return m_upper_bound;
    }

    /*************************************************************************/
    inline bool has_bounds(void) const {
        return m_has_bounds;
    }

    /*************************************************************************/
    inline void set_lower_or_upper_bound(const bool a_IS_LOWER) {
        this->set_value_if_mutable(a_IS_LOWER ? m_lower_bound : m_upper_bound);
    }

    /*************************************************************************/
    inline T_Variable range(void) const {
        return m_upper_bound - m_lower_bound;
    }

    /*************************************************************************/
    inline void set_local_last_update_iteration(
        const int a_LOCAL_LAST_UPDATE_ITERATION) noexcept {
        m_local_last_update_iteration = a_LOCAL_LAST_UPDATE_ITERATION;
    }

    /*************************************************************************/
    inline void reset_local_last_update_iteration(void) noexcept {
        m_local_last_update_iteration =
            VariableConstant::INITIAL_LOCAL_LAST_UPDATE_ITERATION;
    }

    /*************************************************************************/
    inline int local_last_update_iteration(void) const noexcept {
        return m_local_last_update_iteration;
    }

    /*************************************************************************/
    inline void set_global_last_update_iteration(
        const int a_GLOBAL_LAST_UPDATE_ITERATION) noexcept {
        m_global_last_update_iteration = a_GLOBAL_LAST_UPDATE_ITERATION;
    }

    /*************************************************************************/
    inline void reset_global_last_update_iteration(void) noexcept {
        m_global_last_update_iteration = 0;
    }

    /*************************************************************************/
    inline int global_last_update_iteration(void) const noexcept {
        return m_global_last_update_iteration;
    }

    /*************************************************************************/
    inline void increment_update_count(void) noexcept {
        m_update_count++;
    }

    /*************************************************************************/
    inline void reset_update_count(void) noexcept {
        m_update_count = 0;
    }

    /*************************************************************************/
    inline long update_count(void) const noexcept {
        return m_update_count;
    }

    /*************************************************************************/
    inline void fix(void) {
        m_is_fixed = true;
    }

    /*************************************************************************/
    inline void unfix(void) {
        m_is_fixed = false;
    }

    /*************************************************************************/
    inline bool is_fixed(void) const {
        return m_is_fixed;
    }

    /*************************************************************************/
    inline void fix_by(const T_Variable a_VALUE) {
        m_value    = a_VALUE;
        m_is_fixed = true;
        this->update_margin();
    }

    /*************************************************************************/
    inline void set_is_objective_improvable(
        const bool a_IS_OBJECTIVE_IMPROVABLE) noexcept {
        m_is_objective_improvable = a_IS_OBJECTIVE_IMPROVABLE;
    }

    /*************************************************************************/
    inline void set_is_objective_improvable_or(
        const bool a_IS_OBJECTIVE_IMPROVABLE) noexcept {
        m_is_objective_improvable |= a_IS_OBJECTIVE_IMPROVABLE;
    }

    /*************************************************************************/
    inline void set_is_objective_improvable_and(
        const bool a_IS_OBJECTIVE_IMPROVABLE) noexcept {
        m_is_objective_improvable &= a_IS_OBJECTIVE_IMPROVABLE;
    }

    /*************************************************************************/
    inline bool is_objective_improvable(void) const noexcept {
        return m_is_objective_improvable;
    }

    /*************************************************************************/
    inline void set_is_feasibility_improvable(
        const bool a_IS_FEASIBILITY_IMPROVABLE) noexcept {
        m_is_feasibility_improvable = a_IS_FEASIBILITY_IMPROVABLE;
    }

    /*************************************************************************/
    inline void set_is_feasibility_improvable_or(
        const bool a_IS_FEASIBILITY_IMPROVABLE) noexcept {
        m_is_feasibility_improvable |= a_IS_FEASIBILITY_IMPROVABLE;
    }

    /*************************************************************************/
    inline void set_is_feasibility_improvable_and(
        const bool a_IS_FEASIBILITY_IMPROVABLE) noexcept {
        m_is_feasibility_improvable &= a_IS_FEASIBILITY_IMPROVABLE;
    }

    /*************************************************************************/
    inline void set_is_feasibility_improvable_if_has_lower_bound_margin(
        void) noexcept {
        m_is_feasibility_improvable |= m_has_lower_bound_margin;
    }

    /*************************************************************************/
    inline void set_is_feasibility_improvable_if_has_upper_bound_margin(
        void) noexcept {
        m_is_feasibility_improvable |= m_has_upper_bound_margin;
    }

    /*************************************************************************/
    inline bool is_feasibility_improvable(void) const noexcept {
        return m_is_feasibility_improvable;
    }

    /*************************************************************************/
    inline bool is_improvable(void) const noexcept {
        return m_is_feasibility_improvable || m_is_objective_improvable;
    }

    /*************************************************************************/
    inline void update_margin(void) {
        m_has_lower_bound_margin = m_value > m_lower_bound;
        m_has_upper_bound_margin = m_value < m_upper_bound;
    }

    /*************************************************************************/
    inline bool has_lower_bound_margin(void) const noexcept {
        return m_has_lower_bound_margin;
    }

    /*************************************************************************/
    inline bool has_upper_bound_margin(void) const noexcept {
        return m_has_upper_bound_margin;
    }

    /*************************************************************************/
    inline void set_sense(const VariableSense &a_SENSE) noexcept {
        m_extension->sense = a_SENSE;
    }

    /*************************************************************************/
    inline void setup_sense_binary_or_integer(void) {
        if ((m_lower_bound == 0 && m_upper_bound == 1) ||
            (m_lower_bound == 0 && m_upper_bound == 0) ||
            (m_lower_bound == 1 && m_upper_bound == 1)) {
            m_extension->sense = VariableSense::Binary;
        } else {
            m_extension->sense = VariableSense::Integer;
        }
    }

    /*************************************************************************/
    inline VariableSense sense(void) const noexcept {
        return m_extension->sense;
    }

    /*************************************************************************/
    inline std::string sense_label(void) const noexcept {
        switch (m_extension->sense) {
            case VariableSense::Binary: {
                return "Binary";
            }
            case VariableSense::Integer: {
                return "Integer";
            }
            case VariableSense::Selection: {
                return "Selection";
            }
            case VariableSense::DependentBinary: {
                return "DependentBinary";
            }
            case VariableSense::DependentInteger: {
                return "DependentInteger";
            }
            default: {
                return "Undefined";
            }
        }
        return "Undefined";
    }

    /*************************************************************************/
    inline void set_lagrangian_coefficient(
        const double a_LAGRANGIAN_COEFFICIENT) {
        m_extension->lagrangian_coefficient = a_LAGRANGIAN_COEFFICIENT;
    }

    /*************************************************************************/
    inline double lagrangian_coefficient(void) const noexcept {
        return m_extension->lagrangian_coefficient;
    }

    /*************************************************************************/
    inline void set_selection_ptr(
        Selection<T_Variable, T_Expression> *a_SELECTION) {
        m_extension->selection_ptr = a_SELECTION;
        m_extension->sense         = VariableSense::Selection;
    }

    /*************************************************************************/
    inline Selection<T_Variable, T_Expression> *selection_ptr(void) const {
        return m_extension->selection_ptr;
    }

    /*************************************************************************/
    inline void select(void) const {
        m_extension->selection_ptr->selected_variable_ptr = this->reference();
    }

    /*************************************************************************/
    inline void update(void) {
        m_value = m_extension->dependent_expression_ptr->value();
    }

    /*************************************************************************/
    inline void register_related_constraint_ptr(
        Constraint<T_Variable, T_Expression> *a_constraint_ptr) {
        m_extension->related_constraint_ptrs.push_back(a_constraint_ptr);
    }

    /*************************************************************************/
    inline void reset_related_constraint_ptrs(void) {
        m_extension->related_constraint_ptrs.clear();
    }

    /*************************************************************************/
    inline void sort_and_unique_related_constraint_ptrs(void) {
        std::stable_sort(m_extension->related_constraint_ptrs.begin(),
                         m_extension->related_constraint_ptrs.end(),
                         [](const auto &a_FIRST, const auto &a_SECOND) {
                             return a_FIRST->name() < a_SECOND->name();
                         });

        m_extension->related_constraint_ptrs.erase(
            std::unique(m_extension->related_constraint_ptrs.begin(),  //
                        m_extension->related_constraint_ptrs.end()),
            m_extension->related_constraint_ptrs.end());
    }

    /*************************************************************************/
    inline std::vector<Constraint<T_Variable, T_Expression> *> &
    related_constraint_ptrs(void) {
        return m_extension->related_constraint_ptrs;
    }

    /*************************************************************************/
    inline const std::vector<Constraint<T_Variable, T_Expression> *> &
    related_constraint_ptrs(void) const {
        return m_extension->related_constraint_ptrs;
    }

    /*************************************************************************/
    inline void setup_related_binary_coefficient_constraint_ptrs(void) {
        /**
         * NOTE: This method must be called after constraint categorization.
         */
        for (const auto &constraint_ptr :
             m_extension->related_constraint_ptrs) {
            if (constraint_ptr->is_set_partitioning() ||
                constraint_ptr->is_set_packing() ||
                constraint_ptr->is_set_covering() ||
                constraint_ptr->is_cardinality() ||
                constraint_ptr->is_invariant_knapsack() ||
                constraint_ptr->is_multiple_covering()) {
                m_extension->related_binary_coefficient_constraint_ptrs
                    .push_back(constraint_ptr);
            }
        }

        std::stable_sort(
            m_extension->related_binary_coefficient_constraint_ptrs.begin(),
            m_extension->related_binary_coefficient_constraint_ptrs.end(),
            [](const auto &a_FIRST, const auto &a_SECOND) {
                return a_FIRST->name() < a_SECOND->name();
            });

        m_extension->related_binary_coefficient_constraint_ptrs.erase(
            std::unique(
                m_extension->related_binary_coefficient_constraint_ptrs
                    .begin(),  //
                m_extension->related_binary_coefficient_constraint_ptrs.end()),
            m_extension->related_binary_coefficient_constraint_ptrs.end());
    }

    /*************************************************************************/
    inline void reset_related_binary_coefficient_constraint_ptrs(void) {
        m_extension->related_binary_coefficient_constraint_ptrs.clear();
    }

    /*************************************************************************/
    inline std::vector<Constraint<T_Variable, T_Expression> *> &
    related_binary_coefficient_constraint_ptrs(void) {
        return m_extension->related_binary_coefficient_constraint_ptrs;
    }

    /*************************************************************************/
    inline const std::vector<Constraint<T_Variable, T_Expression> *> &
    related_binary_coefficient_constraint_ptrs(void) const {
        return m_extension->related_binary_coefficient_constraint_ptrs;
    }

    /*************************************************************************/
    inline void set_dependent_expression_ptr(
        Expression<T_Variable, T_Expression> *a_expression_ptr) {
        m_extension->dependent_expression_ptr = a_expression_ptr;
        setup_sense_binary_or_integer();
        if (m_extension->sense == VariableSense::Binary) {
            m_extension->sense = VariableSense::DependentBinary;
        } else if (m_extension->sense == VariableSense::Integer) {
            m_extension->sense = VariableSense::DependentInteger;
        } else {
        }
    }

    /*************************************************************************/
    inline void reset_dependent_expression_ptr(void) {
        m_extension->dependent_expression_ptr = nullptr;
        this->setup_sense_binary_or_integer();
    }

    /*************************************************************************/
    inline Expression<T_Variable, T_Expression> *dependent_expression_ptr(
        void) const {
        return const_cast<Expression<T_Variable, T_Expression> *>(
            m_extension->dependent_expression_ptr);
    }

    /*************************************************************************/
    inline void register_constraint_sensitivity(
        Constraint<T_Variable, T_Expression> *a_constraint_ptr,
        const T_Expression                    a_SENSITIVITY) {
        m_extension->constraint_sensitivities.emplace_back(a_constraint_ptr,
                                                           a_SENSITIVITY);
    }

    /*************************************************************************/
    inline void reset_constraint_sensitivities(void) {
        m_extension->constraint_sensitivities.clear();
    }

    /*************************************************************************/
    inline void sort_constraint_sensitivities(void) {
        std::stable_sort(m_extension->constraint_sensitivities.begin(),
                         m_extension->constraint_sensitivities.end(),
                         [](const auto &a_FIRST, const auto &a_SECOND) {
                             return a_FIRST.first->name() <
                                    a_SECOND.first->name();
                         });
    }

    /*************************************************************************/
    inline std::vector<
        std::pair<Constraint<T_Variable, T_Expression> *, T_Expression>> &
    constraint_sensitivities(void) {
        return m_extension->constraint_sensitivities;
    }

    /*************************************************************************/
    inline const std::vector<
        std::pair<Constraint<T_Variable, T_Expression> *, T_Expression>> &
    constraint_sensitivities(void) const {
        return m_extension->constraint_sensitivities;
    }

    /*************************************************************************/
    inline void set_objective_sensitivity(const T_Expression a_SENSITIVITY) {
        m_extension->objective_sensitivity = a_SENSITIVITY;
    }

    /*************************************************************************/
    inline T_Expression objective_sensitivity(void) const noexcept {
        return m_extension->objective_sensitivity;
    }

    /*************************************************************************/
    inline void setup_hash(void) {
        /**
         * NOTE: This method is called in
         * preprocess::remove_redundant_set_variables().
         */
        std::uint64_t hash = 0;
        for (const auto &sensitivity : m_extension->constraint_sensitivities) {
            hash += reinterpret_cast<std::uint64_t>(sensitivity.first);
        }
        m_extension->hash = hash;
    }

    /*************************************************************************/
    inline std::uint64_t hash(void) const noexcept {
        return m_extension->hash;
    }

    /*************************************************************************/
    inline void reset_related_selection_constraint_ptr_index(void) noexcept {
        m_extension->related_selection_constraint_ptr_index_min = -1;
        m_extension->related_selection_constraint_ptr_index_max = -1;
    }

    /*************************************************************************/
    inline void setup_related_selection_constraint_ptr_index(void) noexcept {
        if (m_extension->selection_ptr == nullptr) {
            return;
        }

        const auto &RELATED_SELECTION_CONSTRAINT_PTRS =
            m_extension->selection_ptr->related_constraint_ptrs;

        const int CONSTRAINTS_SIZE = RELATED_SELECTION_CONSTRAINT_PTRS.size();

        for (auto i = 0; i < CONSTRAINTS_SIZE; i++) {
            if (RELATED_SELECTION_CONSTRAINT_PTRS[i]->is_enabled() &&
                std::find(m_extension->related_constraint_ptrs.begin(),
                          m_extension->related_constraint_ptrs.end(),
                          RELATED_SELECTION_CONSTRAINT_PTRS[i]) !=
                    m_extension->related_constraint_ptrs.end()) {
                m_extension->related_selection_constraint_ptr_index_min = i;
                break;
            }
        }

        for (auto i = CONSTRAINTS_SIZE - 1; i >= 0; i--) {
            if (RELATED_SELECTION_CONSTRAINT_PTRS[i]->is_enabled() &&
                std::find(m_extension->related_constraint_ptrs.begin(),
                          m_extension->related_constraint_ptrs.end(),
                          RELATED_SELECTION_CONSTRAINT_PTRS[i]) !=
                    m_extension->related_constraint_ptrs.end()) {
                m_extension->related_selection_constraint_ptr_index_max = i;
                break;
            }
        }
    }

    /*************************************************************************/
    inline int related_selection_constraint_ptr_index_min(void) const noexcept {
        return m_extension->related_selection_constraint_ptr_index_min;
    }

    /*************************************************************************/
    inline int related_selection_constraint_ptr_index_max(void) const noexcept {
        return m_extension->related_selection_constraint_ptr_index_max;
    }

    /*************************************************************************/
    inline Expression<T_Variable, T_Expression> to_expression(void) const {
        std::unordered_map<Variable<T_Variable, T_Expression> *, T_Expression>
            sensitivity;

        sensitivity[this->reference()] = 1;
        return Expression<T_Variable, T_Expression>::create_instance(
            sensitivity, 0);
    }

    /*************************************************************************/
    inline Variable<T_Variable, T_Expression> *reference(void) const {
        return const_cast<Variable<T_Variable, T_Expression> *>(this);
    }

    /*************************************************************************/
    inline Expression<T_Variable, T_Expression> operator+(void) const {
        return this->to_expression();
    }

    /*************************************************************************/
    inline Expression<T_Variable, T_Expression> operator-(void) const {
        return -this->to_expression();
    }

    /*************************************************************************/
    inline Variable<T_Variable, T_Expression> &operator=(
        const T_Variable a_VALUE) {
        if (m_value != a_VALUE && m_is_fixed) {
            throw std::runtime_error(utility::format_error_location(
                __FILE__, __LINE__, __func__,
                "A fixed variable was attempted to be changed."));
        }
        m_value = a_VALUE;
        this->update_margin();
        return *this;
    }
};

using IPVariable = Variable<int, double>;
}  // namespace printemps::model_component
#endif
/*****************************************************************************/
// END
/*****************************************************************************/
