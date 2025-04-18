#pragma once

/**
 * @file enchantController.hpp
 * @brief Provides a centralized interface for managing enchantments.
 *
 * @details
 * The EnchantController class coordinates the initialization and operation of the enchantment
 * system. It uses an EnchantRegistry to maintain a list of all registered enchantments and an
 * EnchantLookupTable to organize enchantments for enchanted books based on a given console/version
 * ordering. The controller allows retrieval of active enchantments, enchantment lookup by ID,
 * and provides methods to set up and free resources for the enchantment system.
 */

#include <span>
#include "enchantRegistry.hpp"
#include "fastBookEnchantArray.hpp"
#include "common/enums.hpp"

namespace enchants {

    /**
     * @brief Controls the initialization and access of enchantments.
     *
     * @details
     * EnchantController is a static class that provides methods for setting up the enchantment system,
     * retrieving active enchantments, and obtaining enchantment level arrays for enchanted books.
     * It ensures that the registry is initialized and ordered according to the current console and version.
     */
    class EnchantController {
    public:

        /**
         * @brief Sets up the enchantment system for a given console and version.
         *
         * @param console The console for which the enchantment system is configured.
         * @param version The game version for which the system is configured.
         *
         * @details
         * This function first initializes the registry if needed, then checks if the current console/version
         * combination is supported. If so, it reorders the active enchantments according to the global lookup
         * table and sets up the book enchantment lookup table.
         *
         * @warning If the provided console/version combination is not supported, a std::range_error is thrown.
         */
        static void setup(lce::CONSOLE console, LCEVERSION version);

        /**
         * @brief Retrieves the number of active (ordered) enchantments.
         *
         * @return The number of active enchantments.
         */
        static size_t getActiveSize();

        /**
         * @brief Retrieves a read-only view of the active enchantments.
         *
         * @return A std::span containing pointers to active Enchant objects.
         */
        static std::span<const Enchant* const> getActiveEnchants();

        /**
         * @brief Retrieves an enchantment by its ID.
         *
         * @param id The unique identifier for the enchantment.
         * @return A pointer to the Enchant object associated with the given ID.
         *
         * @note This is a convenience function that wraps a call to the internal registry.
         */
        ND static const Enchant* getEnchantFromID(c_int id) {
            return s_registry.getEnchantFromID(id);
        }

        /**
         * @brief Retrieves the weight (rarity) associated with an enchantment ID.
         *
         * @param id The unique identifier for the enchantment.
         * @return The weight value for the enchantment.
         *
         * @note This value is used for weighted random selection of enchantments.
         */
        ND static int getWeightFromID(c_int id) {
            return s_registry.getWeightFromID(id);
        }

        /**
         * @brief Retrieves the EnchantLevelArray for a given level.
         *
         * @param level The base level for which to retrieve the enchantment level array.
         * @return A pointer to the EnchantLevelArray corresponding to the given level.
         *
         * @details This is used primarily for enchanted books.
         */
        static EnchantLevelArray* getLevelArray(c_int level);

        /**
         * @brief Frees memory and resets the enchantment system.
         *
         * @details This function clears the registry, effectively resetting the state of the enchantment system.
         */
        static void freeMemory();

    private:
        static EnchantLookupTable s_bookEnchantLevelTable;  ///< Lookup table for book enchantments.
        static EnchantRegistry s_registry;                  ///< Registry for managing all enchantments.
        static bool s_initialized;      ///< Flag indicating whether the system has been initialized.
        static lce::CONSOLE s_console;  ///< The current console.
        static LCEVERSION s_version;    ///< The current game version.
    };

} // namespace enchants
