/*****************************************************************************/
// Copyright (c) 2020-2025 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_MODEL_HANDLER_MODEL_INITIAL_SOLUTION_HANDLER_H__
#define PRINTEMPS_MODEL_HANDLER_MODEL_INITIAL_SOLUTION_HANDLER_H__

namespace printemps::model_handler {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class ModelInitialSolutionHandler {
   private:
    model::Model<T_Variable, T_Expression> *m_model_ptr;

   public:
    /*************************************************************************/
    ModelInitialSolutionHandler(void) {
        m_model_ptr = nullptr;
    }

    /*************************************************************************/
    ModelInitialSolutionHandler(
        model::Model<T_Variable, T_Expression> *a_model_ptr) {
        this->initialize();
        this->setup(a_model_ptr);
    }

    /*************************************************************************/
    inline void initialize(void) noexcept {
        m_model_ptr = nullptr;
    }

    /*************************************************************************/
    inline void setup(
        model::Model<T_Variable, T_Expression> *a_model_ptr) noexcept {
        m_model_ptr = a_model_ptr;
    }

    /*************************************************************************/
    inline void import_solution(
        const solution::DenseSolution<T_Variable, T_Expression> &a_SOLUTION,
        const bool a_IS_ENABLED_VERIFY) {
        auto &model = *m_model_ptr;

        for (auto &&proxy : model.variable_proxies()) {
            for (auto &&variable : proxy.flat_indexed_variables()) {
                const int PROXY_INDEX = variable.proxy_index();
                const int FLAT_INDEX  = variable.flat_index();
                variable.set_value_if_mutable(
                    a_SOLUTION.variable_value_proxies[PROXY_INDEX]
                        .flat_indexed_values(FLAT_INDEX));
            }
        }

        if (a_IS_ENABLED_VERIFY) {
            model.verifier().verify_and_correct(false, false);
        }
    }

    /*************************************************************************/
    inline void import_solution(
        const solution::SparseSolution<T_Variable, T_Expression> &a_SOLUTION,
        const bool a_IS_ENABLED_VERIFY) {
        auto &model = *m_model_ptr;

        const auto &VARIABLES = a_SOLUTION.variables;
        for (auto &&proxy : model.variable_proxies()) {
            for (auto &&variable : proxy.flat_indexed_variables()) {
                if (VARIABLES.find(variable.name()) != VARIABLES.end()) {
                    variable = VARIABLES.at(variable.name());
                } else {
                    variable = 0;
                }
            }
        }
        if (a_IS_ENABLED_VERIFY) {
            model.verifier().verify_and_correct(false, false);
        }
    }

    /*************************************************************************/
    inline void import_solution(
        const std::unordered_map<std::string, int> &a_VARIABLES,
        const bool                                  a_IS_ENABLED_VERIFY) {
        auto &model = *m_model_ptr;

        for (auto &&proxy : model.variable_proxies()) {
            for (auto &&variable : proxy.flat_indexed_variables()) {
                if (a_VARIABLES.find(variable.name()) != a_VARIABLES.end()) {
                    variable = a_VARIABLES.at(variable.name());
                } else {
                    variable = 0;
                }
            }
        }
        if (a_IS_ENABLED_VERIFY) {
            model.verifier().verify_and_correct(false, false);
        }
    }

    /*************************************************************************/
    inline void fix_variables(
        const std::unordered_map<std::string, int> &a_VARIABLES) {
        auto &model = *m_model_ptr;

        for (auto &&proxy : model.variable_proxies()) {
            for (auto &&variable : proxy.flat_indexed_variables()) {
                if (a_VARIABLES.find(variable.name()) != a_VARIABLES.end()) {
                    variable.fix_by(a_VARIABLES.at(variable.name()));
                }
            }
        }
    }

    /*************************************************************************/
    inline void unfix_variables(
        const std::unordered_set<std::string> &a_VARIABLE_NAMES) {
        auto &model = *m_model_ptr;

        for (auto &&proxy : model.variable_proxies()) {
            for (auto &&variable : proxy.flat_indexed_variables()) {
                variable.fix_by(0);
                if (a_VARIABLE_NAMES.find(variable.name()) !=
                    a_VARIABLE_NAMES.end()) {
                    variable.unfix();
                }
            }
        }
    }
};
}  // namespace printemps::model_handler
#endif
/*****************************************************************************/
// END
/*****************************************************************************/