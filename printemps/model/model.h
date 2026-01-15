/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_MODEL_MODEL_H__
#define PRINTEMPS_MODEL_MODEL_H__

namespace printemps::model {

/*****************************************************************************/
template <class T_Variable, class T_Expression>
class Model {
    friend class model_handler::ModelBuilder<T_Variable, T_Expression>;
    friend class model_handler::ModelComponentCreator<T_Variable, T_Expression>;
    friend class model_handler::ModelEvaluator<T_Variable, T_Expression>;
    friend class model_handler::ModelUpdater<T_Variable, T_Expression>;

   private:
    std::string m_name;

    std::vector<model_component::VariableProxy<T_Variable, T_Expression>>
        m_variable_proxies;
    std::vector<model_component::ExpressionProxy<T_Variable, T_Expression>>
        m_expression_proxies;
    std::vector<model_component::ConstraintProxy<T_Variable, T_Expression>>
        m_constraint_proxies;

    model_component::Objective<T_Variable, T_Expression> m_objective;

    std::unordered_map<model_component::Variable<T_Variable, T_Expression> *,
                       model_component::Expression<T_Variable, T_Expression> *>
        m_dependent_expression_map;

    std::vector<model_component::ConstraintCompact<T_Variable, T_Expression>>
        m_constraint_compacts;

    std::vector<std::string> m_variable_names;
    std::vector<std::string> m_expression_names;
    std::vector<std::string> m_constraint_names;

    bool m_is_defined_objective;
    bool m_is_minimization;
    bool m_is_solved;
    bool m_is_integer;
    bool m_current_is_feasible;
    bool m_previous_is_feasible;

    double m_global_penalty_coefficient;
    double m_sign;

    std::vector<model_component::Selection<T_Variable, T_Expression>>
        m_selections;

    std::vector<model_component::Constraint<T_Variable, T_Expression> *>
        m_current_violative_constraint_ptrs;
    std::vector<model_component::Constraint<T_Variable, T_Expression> *>
        m_previous_violative_constraint_ptrs;

    std::vector<
        std::pair<model_component::Variable<T_Variable, T_Expression> *,
                  model_component::Variable<T_Variable, T_Expression> *>>
        m_flippable_variable_ptr_pairs;

    neighborhood::Neighborhood<T_Variable, T_Expression> m_neighborhood;

    preprocess::ProblemSizeReducer<T_Variable, T_Expression>
        m_problem_size_reducer;

    preprocess::Verifier<T_Variable, T_Expression> m_verifier;

    model_handler::Reference<T_Variable, T_Expression>    m_reference_original;
    model_handler::Reference<T_Variable, T_Expression>    m_reference;
    model_handler::ModelBuilder<T_Variable, T_Expression> m_builder;
    model_handler::ModelComponentCreator<T_Variable, T_Expression>
                                                            m_component_creator;
    model_handler::ModelEvaluator<T_Variable, T_Expression> m_evaluator;
    model_handler::ModelUpdater<T_Variable, T_Expression>   m_updater;
    model_handler::ModelPrinter<T_Variable, T_Expression>   m_printer;
    model_handler::ModelMPSHandler<T_Variable, T_Expression>  m_mps_handler;
    model_handler::ModelOPBHandler<T_Variable, T_Expression>  m_opb_handler;
    model_handler::ModelJSONHandler<T_Variable, T_Expression> m_json_handler;
    model_handler::ModelLinearProgrammingHandler<T_Variable, T_Expression>
        m_linear_programming_handler;
    model_handler::ModelInitialSolutionHandler<T_Variable, T_Expression>
        m_initial_solution_handler;
    model_handler::ModelStateInspector<T_Variable, T_Expression>
        m_state_inspector;

    /*************************************************************************/
    Model(const Model &) = default;

    /*************************************************************************/
    Model &operator=(const Model &) = default;

   public:
    /*************************************************************************/
    Model(void) {
        this->initialize();
    }

    /*************************************************************************/
    Model(const std::string &a_NAME) {
        this->initialize();
        this->set_name(a_NAME);
    }

    /*************************************************************************/
    void initialize(void) {
        m_name = "";

        m_variable_proxies.reserve(
            model_handler::ModelComponentCreatorConstant::
                MAX_NUMBER_OF_VARIABLE_PROXIES);
        m_expression_proxies.reserve(
            model_handler::ModelComponentCreatorConstant::
                MAX_NUMBER_OF_EXPRESSION_PROXIES);
        m_constraint_proxies.reserve(
            model_handler::ModelComponentCreatorConstant::
                MAX_NUMBER_OF_CONSTRAINT_PROXIES);
        m_objective.initialize();
        m_dependent_expression_map.clear();
        m_constraint_compacts.clear();

        m_variable_names.clear();
        m_expression_names.clear();
        m_constraint_names.clear();

        m_is_defined_objective = false;
        m_is_minimization      = true;
        m_is_solved            = false;
        m_is_integer           = false;
        m_current_is_feasible  = false;
        m_previous_is_feasible = false;

        m_global_penalty_coefficient = 0.0;
        m_sign                       = 1.0;

        m_selections.clear();
        m_current_violative_constraint_ptrs.clear();
        m_previous_violative_constraint_ptrs.clear();
        m_flippable_variable_ptr_pairs.clear();

        m_neighborhood.initialize();
        m_problem_size_reducer.setup(this);
        m_verifier.setup(this);

        m_reference_original.setup(this);
        m_reference.setup(this);
        m_builder.setup(this);
        m_component_creator.setup(this);
        m_evaluator.setup(this);
        m_updater.setup(this);
        m_printer.setup(this);
        m_mps_handler.setup(this);
        m_opb_handler.setup(this);
        m_json_handler.setup(this);
        m_linear_programming_handler.setup(this);
        m_initial_solution_handler.setup(this);
        m_state_inspector.setup(this);
    }

    /*************************************************************************/
    inline void build(const option::Option &a_OPTION,
                      const bool            a_IS_ENABLED_PRINT) {
        m_builder.build(a_OPTION, a_IS_ENABLED_PRINT);
    }

    /*************************************************************************/
    inline void set_name(const std::string &a_NAME) {
        m_name = a_NAME;
    }

    /*************************************************************************/
    inline const std::string &name(void) const {
        return m_name;
    }

    /*************************************************************************/
    inline model_component::VariableProxy<T_Variable, T_Expression> &
    create_variable(const std::string &a_NAME) {
        return m_component_creator.create_variable(a_NAME);
    }

    /*************************************************************************/
    inline model_component::VariableProxy<T_Variable, T_Expression> &
    create_variable(const std::string &a_NAME,         //
                    const T_Variable   a_LOWER_BOUND,  //
                    const T_Variable   a_UPPER_BOUND) {
        return m_component_creator.create_variable(  //
            a_NAME, a_LOWER_BOUND, a_UPPER_BOUND);
    }

    /*************************************************************************/
    inline model_component::VariableProxy<T_Variable, T_Expression> &
    create_variables(const std::string &a_NAME,  //
                     const int          a_NUMBER_OF_ELEMENTS) {
        return m_component_creator.create_variables(  //
            a_NAME, a_NUMBER_OF_ELEMENTS);
    }

    /*************************************************************************/
    inline model_component::VariableProxy<T_Variable, T_Expression> &
    create_variables(const std::string &a_NAME,                //
                     const int          a_NUMBER_OF_ELEMENTS,  //
                     const T_Variable   a_LOWER_BOUND,         //
                     const T_Variable   a_UPPER_BOUND) {
        return m_component_creator.create_variables(  //
            a_NAME, a_NUMBER_OF_ELEMENTS, a_LOWER_BOUND, a_UPPER_BOUND);
    }

    /*************************************************************************/
    inline model_component::VariableProxy<T_Variable, T_Expression> &
    create_variables(const std::string      &a_NAME,  //
                     const std::vector<int> &a_SHAPE) {
        return m_component_creator.create_variables(  //
            a_NAME, a_SHAPE);
    }

    /*************************************************************************/
    inline model_component::VariableProxy<T_Variable, T_Expression> &
    create_variables(const std::string      &a_NAME,         //
                     const std::vector<int> &a_SHAPE,        //
                     const T_Variable        a_LOWER_BOUND,  //
                     const T_Variable        a_UPPER_BOUND) {
        return m_component_creator.create_variables(  //
            a_NAME, a_SHAPE, a_LOWER_BOUND, a_UPPER_BOUND);
    }

    /*************************************************************************/
    inline model_component::ExpressionProxy<T_Variable, T_Expression> &
    create_expression(const std::string &a_NAME) {
        return m_component_creator.create_expression(a_NAME);
    }

    /*************************************************************************/
    inline model_component::ExpressionProxy<T_Variable, T_Expression> &
    create_expressions(const std::string &a_NAME,  //
                       int                a_NUMBER_OF_ELEMENTS) {
        return m_component_creator.create_expressions(  //
            a_NAME, a_NUMBER_OF_ELEMENTS);
    }

    /*************************************************************************/
    inline model_component::ExpressionProxy<T_Variable, T_Expression> &
    create_expressions(const std::string      &a_NAME,  //
                       const std::vector<int> &a_SHAPE) {
        return m_component_creator.create_expressions(  //
            a_NAME, a_SHAPE);
    }

    /*************************************************************************/
    template <template <class, class> class T_ExpressionLike>
    inline model_component::ExpressionProxy<T_Variable, T_Expression> &
    create_expression(
        const std::string                                &a_NAME,  //
        const T_ExpressionLike<T_Variable, T_Expression> &a_EXPRESSION_LIKE) {
        return m_component_creator.create_expression(  //
            a_NAME, a_EXPRESSION_LIKE);
    }

    /*************************************************************************/
    inline model_component::ExpressionProxy<T_Variable, T_Expression> &
    create_expression(
        const std::string &a_NAME,  //
        const model_component::Expression<T_Variable, T_Expression>
            &a_EXPRESSION) {
        return m_component_creator.create_expression(  //
            a_NAME, a_EXPRESSION);
    }

    /*************************************************************************/
    inline model_component::ConstraintProxy<T_Variable, T_Expression> &
    create_constraint(const std::string &a_NAME) {
        return m_component_creator.create_constraint(a_NAME);
    }

    /*************************************************************************/
    inline model_component::ConstraintProxy<T_Variable, T_Expression> &
    create_constraints(const std::string &a_NAME,  //
                       int                a_NUMBER_OF_ELEMENTS) {
        return m_component_creator.create_constraints(  //
            a_NAME, a_NUMBER_OF_ELEMENTS);
    }

    /*************************************************************************/
    inline model_component::ConstraintProxy<T_Variable, T_Expression> &
    create_constraints(const std::string      &a_NAME,  //
                       const std::vector<int> &a_SHAPE) {
        return m_component_creator.create_constraints(  //
            a_NAME, a_SHAPE);
    }

    /*************************************************************************/
    inline model_component::ConstraintProxy<T_Variable, T_Expression> &
    create_constraint(
        const std::string &a_NAME,  //
        const model_component::Constraint<T_Variable, T_Expression>
            &a_CONSTRAINT) {
        return m_component_creator.create_constraint(  //
            a_NAME, a_CONSTRAINT);
    }

    /*************************************************************************/
    template <template <class, class> class T_ExpressionLike>
    inline void minimize(
        const T_ExpressionLike<T_Variable, T_Expression> &a_EXPRESSION_LIKE) {
        m_component_creator.minimize(a_EXPRESSION_LIKE);
    }

    /*************************************************************************/
    inline void minimize(
        const model_component::Expression<T_Variable, T_Expression>
            &a_EXPRESSION) {
        m_component_creator.minimize(a_EXPRESSION);
    }

    /*************************************************************************/
    template <template <class, class> class T_ExpressionLike>
    inline void maximize(
        const T_ExpressionLike<T_Variable, T_Expression> &a_EXPRESSION_LIKE) {
        m_component_creator.maximize(a_EXPRESSION_LIKE);
    }

    /*************************************************************************/
    inline void maximize(
        const model_component::Expression<T_Variable, T_Expression>
            &a_EXPRESSION) {
        m_component_creator.maximize(a_EXPRESSION);
    }

    /*************************************************************************/
    inline std::vector<
        model_component::VariableProxy<T_Variable, T_Expression>> &
    variable_proxies(void) noexcept {
        return m_variable_proxies;
    }

    /*************************************************************************/
    inline const std::vector<
        model_component::VariableProxy<T_Variable, T_Expression>> &
    variable_proxies(void) const noexcept {
        return m_variable_proxies;
    }

    /*************************************************************************/
    inline std::vector<
        model_component::ExpressionProxy<T_Variable, T_Expression>> &
    expression_proxies(void) noexcept {
        return m_expression_proxies;
    }

    /*************************************************************************/
    inline const std::vector<
        model_component::ExpressionProxy<T_Variable, T_Expression>> &
    expression_proxies(void) const noexcept {
        return m_expression_proxies;
    }

    /*************************************************************************/
    inline std::vector<
        model_component::ConstraintProxy<T_Variable, T_Expression>> &
    constraint_proxies(void) {
        return m_constraint_proxies;
    }

    /*************************************************************************/
    inline const std::vector<
        model_component::ConstraintProxy<T_Variable, T_Expression>> &
    constraint_proxies(void) const noexcept {
        return m_constraint_proxies;
    }

    /*************************************************************************/
    inline model_component::Objective<T_Variable, T_Expression> &objective(
        void) noexcept {
        return m_objective;
    }

    /*************************************************************************/
    inline const model_component::Objective<T_Variable, T_Expression> &
    objective(void) const noexcept {
        return m_objective;
    }

    /*************************************************************************/
    inline const std::unordered_map<
        model_component::Variable<T_Variable, T_Expression> *,
        model_component::Expression<T_Variable, T_Expression> *> &
    dependent_expression_map(void) const noexcept {
        return m_dependent_expression_map;
    }

    /*************************************************************************/
    inline std::unordered_map<
        model_component::Variable<T_Variable, T_Expression> *,
        model_component::Expression<T_Variable, T_Expression> *> &
    dependent_expression_map(void) noexcept {
        return m_dependent_expression_map;
    }

    /*************************************************************************/
    inline const std::vector<
        model_component::ConstraintCompact<T_Variable, T_Expression>> &
    constraint_compacts(void) const noexcept {
        return m_constraint_compacts;
    }

    /*************************************************************************/
    inline std::vector<
        model_component::ConstraintCompact<T_Variable, T_Expression>> &
    constraint_compacts(void) noexcept {
        return m_constraint_compacts;
    }

    /*************************************************************************/
    inline const std::vector<std::string> &variable_names(void) const noexcept {
        return m_variable_names;
    }

    /*************************************************************************/
    inline const std::vector<std::string> &expression_names(
        void) const noexcept {
        return m_expression_names;
    }

    /*************************************************************************/
    inline const std::vector<std::string> &constraint_names(
        void) const noexcept {
        return m_constraint_names;
    }

    /*************************************************************************/
    inline bool is_defined_objective(void) const noexcept {
        return m_is_defined_objective;
    }

    /*************************************************************************/
    inline void set_is_minimization(const bool a_IS_MINIMIZATION) noexcept {
        m_is_minimization = a_IS_MINIMIZATION;
        m_sign            = a_IS_MINIMIZATION ? 1.0 : -1.0;
    }

    /*************************************************************************/
    inline bool is_minimization(void) const noexcept {
        return m_is_minimization;
    }

    /*************************************************************************/
    inline void set_is_solved(const bool a_IS_SOLVED) noexcept {
        m_is_solved = a_IS_SOLVED;
    }

    /*************************************************************************/
    inline bool is_solved(void) const noexcept {
        return m_is_solved;
    }

    /*************************************************************************/
    inline bool is_integer(void) const noexcept {
        return m_is_integer;
    }

    /*************************************************************************/
    inline bool is_feasible(void) const noexcept {
        return m_current_is_feasible;
    }

    /*************************************************************************/
    inline bool current_is_feasible(void) const noexcept {
        return m_current_is_feasible;
    }

    /*************************************************************************/
    inline bool previous_is_feasible(void) const noexcept {
        return m_previous_is_feasible;
    }

    /*************************************************************************/
    inline void set_global_penalty_coefficient(
        const double a_GLOBAL_PENALTY_COEFFICIENT) noexcept {
        m_global_penalty_coefficient = a_GLOBAL_PENALTY_COEFFICIENT;
    }

    /*************************************************************************/
    inline double global_penalty_coefficient(void) const noexcept {
        return m_global_penalty_coefficient;
    }

    /*************************************************************************/
    inline double sign(void) const noexcept {
        /**
         * In this program, maximization problems are solved as minimization
         * problems by negating the objective function values. This method
         * is used to show objective function values for output.
         */
        return m_sign;
    }

    /*************************************************************************/
    inline const std::vector<
        model_component::Selection<T_Variable, T_Expression>> &
    selections(void) const noexcept {
        return m_selections;
    }

    /*************************************************************************/
    inline const std::vector<
        model_component::Constraint<T_Variable, T_Expression> *> &
    current_violative_constraint_ptrs(void) const noexcept {
        return m_current_violative_constraint_ptrs;
    }

    /*************************************************************************/
    inline const std::vector<
        model_component::Constraint<T_Variable, T_Expression> *> &
    previous_violative_constraint_ptrs(void) const noexcept {
        return m_previous_violative_constraint_ptrs;
    }

    /*************************************************************************/
    inline std::vector<
        std::pair<model_component::Variable<T_Variable, T_Expression> *,
                  model_component::Variable<T_Variable, T_Expression> *>> &
    flippable_variable_ptr_pairs(void) {
        return m_flippable_variable_ptr_pairs;
    }

    /*************************************************************************/
    inline neighborhood::Neighborhood<T_Variable, T_Expression> &neighborhood(
        void) noexcept {
        return m_neighborhood;
    }

    /*************************************************************************/
    inline preprocess::ProblemSizeReducer<T_Variable, T_Expression> &
    problem_size_reducer(void) noexcept {
        return m_problem_size_reducer;
    }

    /*************************************************************************/
    inline preprocess::Verifier<T_Variable, T_Expression> &verifier(
        void) noexcept {
        return m_verifier;
    }

    /*************************************************************************/
    inline model_handler::Reference<T_Variable, T_Expression> &reference(
        void) noexcept {
        return m_reference;
    }

    /*************************************************************************/
    inline const model_handler::Reference<T_Variable, T_Expression> &reference(
        void) const noexcept {
        return m_reference;
    }

    /*************************************************************************/
    inline model_handler::Reference<T_Variable, T_Expression> &
    reference_original(void) noexcept {
        return m_reference_original;
    }

    /*************************************************************************/
    inline const model_handler::Reference<T_Variable, T_Expression> &
    reference_original(void) const noexcept {
        return m_reference_original;
    }

    /*************************************************************************/
    inline model_handler::ModelBuilder<T_Variable, T_Expression> &builder(
        void) noexcept {
        return m_builder;
    }

    /*************************************************************************/
    inline model_handler::ModelComponentCreator<T_Variable, T_Expression> &
    component_creator(void) noexcept {
        return m_component_creator;
    }

    /*************************************************************************/
    inline model_handler::ModelEvaluator<T_Variable, T_Expression> &evaluator(
        void) noexcept {
        return m_evaluator;
    }

    /*************************************************************************/
    inline model_handler::ModelUpdater<T_Variable, T_Expression> &updater(
        void) noexcept {
        return m_updater;
    }

    /*************************************************************************/
    inline const model_handler::ModelPrinter<T_Variable, T_Expression> &printer(
        void) const noexcept {
        return m_printer;
    }

    /*************************************************************************/
    inline model_handler::ModelMPSHandler<T_Variable, T_Expression> &
    mps_handler(void) noexcept {
        return m_mps_handler;
    }

    /*************************************************************************/
    inline model_handler::ModelOPBHandler<T_Variable, T_Expression> &
    opb_handler(void) noexcept {
        return m_opb_handler;
    }

    /*************************************************************************/
    inline model_handler::ModelJSONHandler<T_Variable, T_Expression> &
    json_handler(void) {
        return m_json_handler;
    }

    /*************************************************************************/
    inline model_handler::ModelLinearProgrammingHandler<T_Variable,
                                                        T_Expression> &
    linear_programming_handler(void) noexcept {
        return m_linear_programming_handler;
    }

    /*************************************************************************/
    inline model_handler::ModelInitialSolutionHandler<T_Variable,
                                                      T_Expression> &
    initial_solution_handler(void) noexcept {
        return m_initial_solution_handler;
    }

    /*************************************************************************/
    inline model_handler::ModelStateInspector<T_Variable, T_Expression> &
    state_inspector(void) noexcept {
        return m_state_inspector;
    }
};
using IPModel         = Model<int, double>;
using InfeasibleError = printemps::model_component::InfeasibleError;
}  // namespace printemps::model
#endif
/*****************************************************************************/
// END
/*****************************************************************************/