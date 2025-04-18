#include "enchantController.hpp"

namespace enchants {

    // Static member definitions.
    bool EnchantController::s_initialized = false;
    lce::CONSOLE EnchantController::s_console = lce::CONSOLE::NONE;
    LCEVERSION EnchantController::s_version = LCEVERSION::NONE;
    EnchantLookupTable EnchantController::s_bookEnchantLevelTable;
    EnchantRegistry EnchantController::s_registry;

    /**
     * @brief Retrieves the number of active enchantments.
     *
     * @return The count of active enchantments.
     */
    size_t EnchantController::getActiveSize() {
        return s_registry.activeSize();
    }

    /**
     * @brief Retrieves a read-only view of the active enchantments.
     *
     * @return A std::span of pointers to active Enchant objects.
     */
    std::span<const Enchant* const> EnchantController::getActiveEnchants() {
        return s_registry.getActiveEnchants();
    }

    /**
     * @brief Retrieves the enchantment level array for the given level.
     *
     * @param level The base level for which the array is requested.
     * @return A pointer to the corresponding EnchantLevelArray.
     */
    EnchantLevelArray* EnchantController::getLevelArray(c_int level) {
        return s_bookEnchantLevelTable.get(level);
    }

    /**
     * @brief Sets up the enchantment system for the specified console and version.
     *
     * @param console The console type.
     * @param version The game version.
     *
     * @details
     * This function first ensures that the registry is initialized. If the system has already been initialized
     * with the same console and version, it returns early. Otherwise, it updates the current console and version,
     * validates the key against the lookup table, applies the new ordering, sets up the book lookup table, and
     * marks the system as initialized.
     *
     * @warning If the console/version combination is unsupported, this function throws a std::range_error.
     */
    void EnchantController::setup(const lce::CONSOLE console, const LCEVERSION version) {
        s_registry.initializeIfNeeded();

        // If already initialized with the same parameters, nothing to do.
        if (s_initialized && s_console == console && s_version == version) {
            return;
        }

        s_console = console;
        s_version = version;
        const KeyType_t orderKey{ s_console, s_version };

        if (!EnchantRegistry::containsKey(orderKey)) {
            throw std::range_error("Unsupported console/version combination!");
        }

        // Reorder the active registry based on the given key.
        s_registry.applyOrder(orderKey);
        s_bookEnchantLevelTable.setup();
        s_initialized = true;
    }

    /**
     * @brief Frees resources and resets the enchantment system.
     *
     * @details
     * This function clears the registry and resets the initialization flag, effectively resetting
     * the enchantment system to an uninitialized state.
     */
    MU void EnchantController::freeMemory() {
        s_initialized = false;
        s_registry.clear();
    }

} // namespace enchants
