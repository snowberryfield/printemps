/*****************************************************************************/
// Copyright (c) 2020-2021 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_PREPROCESS_GF2_H__
#define PRINTEMPS_PREPROCESS_GF2_H__

namespace printemps {
namespace preprocess {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class GF2Solver {
   private:
    model::Model<T_Variable, T_Expression> *m_model_ptr;

   public:
    /*************************************************************************/
    GF2Solver(void) {
        this->initialize();
    }

    /*************************************************************************/
    GF2Solver(model::Model<T_Variable, T_Expression> *a_model_ptr) {
        this->setup(a_model_ptr);
    }

    /*************************************************************************/
    inline void setup(model::Model<T_Variable, T_Expression> *a_model_ptr) {
        this->initialize();
        this->m_model_ptr = a_model_ptr;
    }

    /*************************************************************************/
    inline void initialize(void) {
        m_model_ptr = nullptr;
    }

    /*************************************************************************/
    inline constexpr bool solve(const bool a_IS_ENABLED_PRINT) {
        utility::print_single_line(a_IS_ENABLED_PRINT);
        utility::print_message("Attempting to solve GF(2) equations...",
                               a_IS_ENABLED_PRINT);

        /**
         * Set up number of GF(2) equations.
         */
        auto &    gf2s      = m_model_ptr->constraint_type_reference().gf2_ptrs;
        const int GF2S_SIZE = gf2s.size();

        /**
         * Set up binary/auxiliary variables sets.
         */
        std::unordered_set<
            model_component::Variable<T_Variable, T_Expression> *>
            binary_variable_ptrs;
        std::unordered_set<
            model_component::Variable<T_Variable, T_Expression> *>
            aux_variable_ptrs;

        for (const auto &constraint_ptr : gf2s) {
            auto &expression       = constraint_ptr->expression();
            auto  aux_variable_ptr = constraint_ptr->aux_variable_ptr();
            for (const auto &sensitivity : expression.sensitivities()) {
                if (sensitivity.first != aux_variable_ptr) {
                    binary_variable_ptrs.insert(sensitivity.first);
                } else {
                    aux_variable_ptrs.insert(sensitivity.first);
                }
            }
        }

        /**
         * If the number of binary/auxiliary variables do not match the that of
         * the GF(2) equations, skip the following processes.
         */
        if (static_cast<int>(binary_variable_ptrs.size()) != GF2S_SIZE) {
            utility::print_message("Failed.", a_IS_ENABLED_PRINT);
            return false;
        }

        if (static_cast<int>(aux_variable_ptrs.size()) != GF2S_SIZE) {
            utility::print_message("Failed.", a_IS_ENABLED_PRINT);
            return false;
        }

        /**
         * Set up the bidirectional maps between variable pointers and indices.
         */
        utility::BidirectionalMap<
            model_component::Variable<T_Variable, T_Expression> *, int>
            variable_map;

        utility::BidirectionalMap<
            model_component::Constraint<T_Variable, T_Expression> *, int>
            constraint_map;

        int variable_index = 0;
        for (const auto &variable_ptr : binary_variable_ptrs) {
            variable_map.insert(variable_ptr, variable_index++);
        }

        int constraint_index = 0;
        for (const auto &constraint_ptr : gf2s) {
            constraint_map.insert(constraint_ptr, constraint_index++);
        }

        /**
         * Solve GF(2) equations.
         */
        utility::GF2Matrix gf2_matrix(GF2S_SIZE, GF2S_SIZE);
        std::vector<int>   constant_values(GF2S_SIZE, 0);

        for (const auto &constraint_ptr : gf2s) {
            auto &expression       = constraint_ptr->expression();
            auto  aux_variable_ptr = constraint_ptr->aux_variable_ptr();
            int   row              = constraint_map[constraint_ptr];
            for (const auto &sensitivity : expression.sensitivities()) {
                if (sensitivity.first != aux_variable_ptr) {
                    int column              = variable_map[sensitivity.first];
                    gf2_matrix[row][column] = 1;
                }
            }
            constant_values[row] =
                static_cast<int>(expression.constant_value()) & 1;
        }

        auto  result  = gf2_matrix.inverse_and_rank();
        auto &inverse = result.first;
        int   rank    = result.second;

        /**
         * If the GF(2) matrix is not full-rank, skip the following processes.
         */
        if (rank != GF2S_SIZE) {
            utility::print_message("Failed.", a_IS_ENABLED_PRINT);
            return false;
        }

        /**
         * Fix the values of variables in GF(2) equations based on the result
         * above.
         */
        auto solution = inverse.dot(constant_values);

        for (auto i = 0; i < GF2S_SIZE; i++) {
            variable_map[i]->fix_by(solution[i]);
        }

        for (const auto &constraint_ptr : gf2s) {
            auto & expression       = constraint_ptr->expression();
            auto   aux_variable_ptr = constraint_ptr->aux_variable_ptr();
            double value            = expression.constant_value();
            for (const auto &sensitivity : expression.sensitivities()) {
                if (sensitivity.first != aux_variable_ptr) {
                    value += sensitivity.first->value();
                }
            }
            double aux_coefficient =
                expression.sensitivities().at(aux_variable_ptr);
            aux_variable_ptr->fix_by(
                static_cast<int>(-value / aux_coefficient));
        }

        for (const auto &variable_ptr : binary_variable_ptrs) {
            utility::print_message("The value of variable " +
                                       variable_ptr->name() + " was fixed at " +
                                       std::to_string(variable_ptr->value()) +
                                       ".",
                                   a_IS_ENABLED_PRINT);
        }

        for (const auto &variable_ptr : aux_variable_ptrs) {
            utility::print_message("The value of variable " +
                                       variable_ptr->name() + " was fixed at " +
                                       std::to_string(variable_ptr->value()) +
                                       ".",
                                   a_IS_ENABLED_PRINT);
        }

        utility::print_message("Done.", a_IS_ENABLED_PRINT);
        return true;
    }
};
}  // namespace preprocess
}  // namespace printemps

#endif
/*****************************************************************************/
// END
/*****************************************************************************/