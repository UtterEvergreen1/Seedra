#pragma once

/**
 * @file enchantRegistry.hpp
 * @brief Declares the EnchantRegistry class which manages and orders available enchantments.
 *
 * @details
 * The EnchantRegistry class is responsible for managing all available enchantments. It stores both the
 * complete list of registered enchantments and a subset of "active" enchantments sorted according to a given
 * console/version ordering. The registry provides methods to:
 * - Retrieve the active enchantments and their weights.
 * - Look up enchantments and weights by their unique identifier.
 * - Apply an ordering based on a lookup table.
 * - Initialize (and reinitialize) the registry as needed.
 *
 * The registry relies on static pre-constructed enchantment objects defined in enchantsInit.hpp.
 */

#include <array>
#include <map>
#include <span>
#include <stdexcept>
#include <algorithm>
#include "enchantsInit.hpp"
#include "common/enums.hpp"
#include "lce/enums.hpp"
#include "lce/processor.hpp"

namespace enchants {

    /**
     * @brief Type alias representing a key for ordering enchantments.
     *
     * @details The key is a pair consisting of a console type and a version.
     */
    using KeyType_t = std::pair<lce::CONSOLE, LCEVERSION>;

    /**
     * @brief Manages the collection and ordering of enchantments.
     *
     * @details
     * EnchantRegistry maintains two arrays:
     *  - A complete list of registered enchantments and their weights.
     *  - An active (sorted) list of enchantments and their weights, as defined by a lookup table.
     *
     * The registry can be initialized on demand, reordering the active enchantments based on a specific
     * console/version key. It also provides methods to access enchantments and their weights by their ID.
     *
     * @note The registry is designed to be initialized only once; subsequent calls to initializeIfNeeded()
     * will have no effect unless the registry is cleared.
     */
    class EnchantRegistry {

        // All registered enchantments (indexed order)
        std::array<const Enchant*, MAX_ENCHANTMENT_COUNT> m_registeredEnchants;

        // Active (sorted) enchantments for the current console/version
        std::array<const Enchant*, MAX_ENCHANTMENT_COUNT> m_activeEnchants;

        // All registered enchant weights (indexed order)
        std::array<int, MAX_ENCHANTMENT_COUNT> m_registeredWeights{};

        // Active (sorted) enchant weights for the current console/version
        std::array<int, MAX_ENCHANTMENT_COUNT> m_activeWeights{};

        // Count of registered enchantments/weights
        size_t m_numRegistered;

        // Count of active enchantments/weights
        size_t m_numActive;

        // Indicates whether the registry has been initialized
        bool m_initialized;

    public:
        /**
         * @brief Constructs an empty EnchantRegistry.
         *
         * @details All counters are set to zero and the registry is marked uninitialized.
         */
        EnchantRegistry()
            : m_registeredEnchants{}, m_activeEnchants{},
              m_numRegistered(0), m_numActive(0), m_initialized(false) {}

        /**
         * @brief Provides read-only access to an active enchantment by index.
         *
         * @param index The index into the active enchantments array.
         * @return A pointer to the Enchant object at the specified index.
         *
         * @note It is the caller's responsibility to ensure that the index is within bounds.
         */
        const Enchant* operator[](const size_t index) const {
            return m_activeEnchants[index];
        }

        /**
         * @brief Retrieves a read-only view of the active enchantments.
         *
         * @return A std::span containing pointers to active Enchant objects.
         */
        ND std::span<const Enchant* const> getActiveEnchants() const {
            return {m_activeEnchants.data(), m_numActive};
        }

        /**
         * @brief Retrieves a registered enchantment by its ID.
         *
         * @param id The enchantment's unique identifier.
         * @return A pointer to the corresponding Enchant object.
         */
        ND const Enchant* getEnchantFromID(const size_t id) const {
            return m_registeredEnchants[id];
        }

        /**
         * @brief Retrieves the weight associated with a registered enchantment.
         *
         * @param id The enchantment's unique identifier.
         * @return The weight of the enchantment.
         */
        ND int getWeightFromID(const size_t id) const {
            return m_registeredWeights[id];
        }

        /**
         * @brief Checks if the global order lookup table contains an entry for a given key.
         *
         * @param orderKey The key, consisting of a console and version.
         * @return @c true if the key exists in the lookup table; otherwise, @c false.
         */
        static bool containsKey(const KeyType_t& orderKey) {
            return getTableOfOrders().contains(orderKey);
        }

        /**
         * @brief Applies a specific ordering to the active enchantments.
         *
         * @param orderKey The key corresponding to the desired order (console and version).
         *
         * @details
         * This function retrieves the lookup values from the global table and calls @c orderValues()
         * to reorder the active enchantments accordingly.
         *
         * @warning If the key is not found in the lookup table, a std::range_error is thrown.
         */
        void applyOrder(const KeyType_t& orderKey) {
            if (!containsKey(orderKey)) {
                throw std::range_error("Unsupported console/version combination!");
            }
            orderValues(getTableOfOrders().at(orderKey));
        }

        /**
         * @brief Returns the number of active enchantments.
         *
         * @return The count of active enchantments.
         */
        ND size_t activeSize() const { return m_numActive; }

        /**
         * @brief Clears the registry.
         *
         * @details
         * This function resets the registry to an uninitialized state by clearing the counts for both
         * registered and active enchantments.
         */
        void clear() {
            m_initialized = false;
            m_numRegistered = 0;
            m_numActive = 0;
        }

        /**
         * @brief Initializes the registry if it has not been initialized.
         *
         * @details
         * If the registry is not yet initialized, this function calls @c registerEnchants() to register
         * all available enchantments and marks the registry as initialized.
         */
        void initializeIfNeeded() {
            if (!m_initialized) {
                registerEnchants();
                m_initialized = true;
            }
        }

    private:
        /**
         * @brief Retrieves the global lookup table for ordering enchantments.
         *
         * @return A reference to a std::map mapping a KeyType_t to a span of enchantment IDs.
         */
        static const std::map<KeyType_t, std::span<const eEnchantId>>& getTableOfOrders();

        /**
         * @brief Registers a single enchantment.
         *
         * @param enchant A pointer to the enchantment to register.
         *
         * @details
         * This function stores the enchantment pointer along with its rarity as weight.
         */
        void registerEnchant(const Enchant* enchant) {
            m_registeredEnchants[m_numRegistered] = enchant;
            m_registeredWeights[m_numRegistered] = enchant->getRarity();
            m_numRegistered++;
        }

        /**
         * @brief Registers all available enchantments.
         *
         * @details
         * This function registers all static enchantment objects defined in the initialization file (enchantsInit.hpp)
         * by calling @c registerEnchant() for each one.
         */
        void registerEnchants() {
            registerEnchant(&s_protection);
            registerEnchant(&s_fireProtection);
            registerEnchant(&s_featherFalling);
            registerEnchant(&s_blastProtection);
            registerEnchant(&s_projectileProtection);
            registerEnchant(&s_respiration);
            registerEnchant(&s_aquaAffinity);
            registerEnchant(&s_thorns);
            registerEnchant(&s_depthStrider);
            registerEnchant(&s_frostWalker);
            registerEnchant(&s_bindingCurse);
            registerEnchant(&s_sharpness);
            registerEnchant(&s_smite);
            registerEnchant(&s_baneOfArthropods);
            registerEnchant(&s_knockback);
            registerEnchant(&s_fireAspect);
            registerEnchant(&s_looting);
            registerEnchant(&s_efficiency);
            registerEnchant(&s_silkTouch);
            registerEnchant(&s_unbreaking);
            registerEnchant(&s_fortune);
            registerEnchant(&s_power);
            registerEnchant(&s_punch);
            registerEnchant(&s_flame);
            registerEnchant(&s_infinity);
            registerEnchant(&s_luckOfTheSea);
            registerEnchant(&s_lure);
            registerEnchant(&s_mending);
            registerEnchant(&s_vanishingCurse);
            registerEnchant(&s_impaling);
            registerEnchant(&s_riptide);
            registerEnchant(&s_loyalty);
            registerEnchant(&s_channeling);
        }

        /**
         * @brief Reorders the registered enchantments into the active list.
         *
         * @param lookupValues A span of enchantment IDs that specify the desired order.
         *
         * @details
         * The function sets the active count to the minimum of the number of registered enchantments and
         * the size of the lookup values. Then, for each index, it copies the corresponding enchantment and weight
         * from the registered arrays into the active arrays.
         */
        MU void orderValues(const std::span<const eEnchantId>& lookupValues) {
            m_numActive = std::min(m_numRegistered, lookupValues.size());
            for (size_t i = 0; i < m_numActive; ++i) {
                const auto idx = static_cast<size_t>(lookupValues[i]);
                m_activeEnchants[i] = m_registeredEnchants[idx];
                m_activeWeights[i] = m_registeredWeights[idx];
            }
        }
    };

} // namespace enchants
