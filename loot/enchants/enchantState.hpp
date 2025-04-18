#pragma once

/**
 * @file enchantState.hpp
 * @brief Defines the EnchantState class and related types for tracking enchantment state.
 *
 * @details
 * The EnchantState class encapsulates the state of an enchantment applied to an item.
 * It stores the enchantment ID, the level of the enchantment, and an additional field (extra)
 * that can be used for various purposes (e.g., container index or other metadata). A vector type
 * alias (EnchDataVec_t) is also provided for convenience.
 */

#include "enchant.hpp"

namespace enchants {

    /**
     * @brief Represents the state of an enchantment applied to an item.
     *
     * @details
     * The EnchantState class stores three key pieces of information:
     * - @c id: The unique identifier of the enchantment.
     * - @c level: The level at which the enchantment is applied.
     * - @c extra: An additional value that can be used to store supplementary data.
     *
     * The class provides getter functions to retrieve these values and a member function to convert
     * the state into a human-readable string.
     */
    class EnchantState {
        u16 m_id = 0;    ///< The unique identifier for the enchantment.
        u8 m_level = 0;  ///< The level of the enchantment.
        u8 m_extra = 0;  ///< An extra field for additional data (e.g., container index).
    public:
        /**
         * @brief Default constructor.
         *
         * @details Initializes an EnchantState with default values (id, level, extra all set to zero).
         */
        constexpr EnchantState() = default;

        /**
         * @brief Constructs an EnchantState with specified values.
         *
         * @param id The unique identifier of the enchantment.
         * @param enchLevel The level at which the enchantment is applied.
         * @param extra (Optional) Additional data associated with the enchantment. Defaults to 0.
         */
        constexpr EnchantState(c_int id, c_int enchLevel, c_int extra = 0)
            : m_id(id), m_level(enchLevel), m_extra(extra) {}

        /**
         * @brief Retrieves the enchantment ID.
         *
         * @return The unique identifier of the enchantment.
         */
        MU ND int getID() const noexcept { return m_id; }

        /**
         * @brief Retrieves the enchantment level.
         *
         * @return The level at which the enchantment is applied.
         */
        MU ND int getLevel() const noexcept { return m_level; }

        /**
         * @brief Retrieves the extra value associated with the enchantment.
         *
         * @return The extra data value.
         */
        MU ND int getExtra() const noexcept { return m_extra; }

        /**
         * @brief Sets the extra value.
         *
         * @param extraIn The new value for the extra data.
         */
        void setExtra(c_u8 extraIn) noexcept { m_extra = extraIn; }

        /**
         * @brief Converts the enchantment state to a human-readable string.
         *
         * @return A std::string representing the enchantment state (name and level in Roman numerals).
         *
         * @details
         * This function retrieves the corresponding Enchant object (using EnchantController::getEnchantFromID)
         * and constructs a string by combining the enchant's name with its level in Roman numerals.
         * If the enchant has only one level, only the name is returned.
         *
         * @note If the Enchant object cannot be found (i.e. returns nullptr), the function returns "NULLPTR 0".
         */
        ND std::string toString() const;
    };

    /**
     * @brief Type alias for a vector of EnchantState objects.
     */
    typedef std::vector<EnchantState> EnchStateVec_t;

} // namespace enchants
