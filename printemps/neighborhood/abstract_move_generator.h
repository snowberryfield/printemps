/*****************************************************************************/
// Copyright (c) 2020 Yuji KOGUMA
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php
/*****************************************************************************/
#ifndef PRINTEMPS_NEIGHBORHOOD_ABSTRACT_MOVE_GENERATOR_H__
#define PRINTEMPS_NEIGHBORHOOD_ABSTRACT_MOVE_GENERATOR_H__

namespace printemps {
namespace neighborhood {
/*****************************************************************************/
template <class T_Variable, class T_Expression>
class Move;

/*****************************************************************************/
template <class T_Variable, class T_Expression>
class AbstractMoveGenerator {
   protected:
    std::function<void(std::vector<Move<T_Variable, T_Expression>> *,
                       std::vector<int> *, const bool, const bool, const bool,
                       const bool)>
        m_move_updater;

    std::vector<Move<T_Variable, T_Expression>> m_moves;
    std::vector<int>                            m_flags;
    bool                                        m_is_enabled;

   public:
    /*************************************************************************/
    AbstractMoveGenerator(void) {
        this->initialize();
    }

    /*************************************************************************/
    virtual ~AbstractMoveGenerator(void) {
        /// nothing to do
    }

    /*************************************************************************/
    void initialize(void) {
        m_move_updater = [](std::vector<Move<T_Variable, T_Expression>> *,  //
                            std::vector<int> *,                             //
                            const bool,                                     //
                            const bool,                                     //
                            const bool,                                     //
                            const bool) {};
        m_moves.clear();
        m_flags.clear();
        m_is_enabled = false;
    }

    /*************************************************************************/
    inline constexpr void update_moves(
        const bool a_ACCEPT_ALL,                     //
        const bool a_ACCEPT_OBJECTIVE_IMPROVABLE,    //
        const bool a_ACCEPT_FEASIBILITY_IMPROVABLE,  //
        const bool a_IS_ENABLED_PARALLEL) {
        m_move_updater(&m_moves,                         //
                       &m_flags,                         //
                       a_ACCEPT_ALL,                     //
                       a_ACCEPT_OBJECTIVE_IMPROVABLE,    //
                       a_ACCEPT_FEASIBILITY_IMPROVABLE,  //
                       a_IS_ENABLED_PARALLEL);
    }

    /*************************************************************************/
    inline constexpr std::vector<Move<T_Variable, T_Expression>> &moves(void) {
        return m_moves;
    }

    /*************************************************************************/
    inline constexpr const std::vector<Move<T_Variable, T_Expression>> &moves(
        void) const {
        return m_moves;
    }

    /*************************************************************************/
    inline constexpr void set_moves(
        const std::vector<Move<T_Variable, T_Expression>> &a_MOVES) {
        /**
         * NOTE: This method is for test.
         */
        m_moves = a_MOVES;
    }

    /*************************************************************************/
    inline constexpr std::vector<int> &flags(void) {
        return m_flags;
    }

    /*************************************************************************/
    inline constexpr const std::vector<int> &flags(void) const {
        return m_flags;
    }

    /*************************************************************************/
    inline constexpr void set_flags(const std::vector<int> &a_FLAGS) {
        /**
         * NOTE: This method is for test.
         */
        m_flags = a_FLAGS;
    }

    /*************************************************************************/
    inline constexpr bool is_enabled(void) const {
        return m_is_enabled;
    }

    /*************************************************************************/
    inline constexpr void enable(void) {
        m_is_enabled = true;
    }

    /*************************************************************************/
    inline constexpr void disable(void) {
        m_is_enabled = false;
    }

    /*************************************************************************/
    inline constexpr void reset_availability(void) {
        for (auto &&move : m_moves) {
            move.is_available = true;
        }
    }
};
}  // namespace neighborhood
}  // namespace printemps
#endif
/*****************************************************************************/
// END
/*****************************************************************************/