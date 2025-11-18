#pragma once

/**
 * @file enchant.hpp
 * @brief Contains the declaration of the Enchant base class for enchantments.
 *
 * @details
 * The Enchant class defines the interface and basic data for all enchantments. It includes
 * properties such as name, ID, type, rarity, and maximum level. Additionally, it provides
 * virtual functions for cost calculation and compatibility checks.
 *
 * The design assumes that derived classes override cost functions and compatibility checks
 * to implement specific enchantment behaviors.
 */

#include <string>
#include "enchantEnums.hpp"
#include "enchantType.hpp"

namespace enchants {

    /*
     // The following commented-out code shows an alternative approach using a custom vtable.
     // It is left here for reference and potential future use.
     //
     // using CostFunction = int (*)(const Enchant*, int);
     //
     // struct EnchantVTable {
     //     CostFunction getMinCost;
     //     CostFunction getMaxCost;
     // };
     //
     // template <typename Derived>
     // constexpr EnchantVTable makeVTable() {
     //     return {
     //             // getMinCost dispatcher
     //             [](const Enchant* self, int level) {
     //                 return static_cast<const Derived*>(self)->getMinCostImpl(level);
     //             },
     //             // getMaxCost dispatcher
     //             [](const Enchant* self, int level) {
     //                 return static_cast<const Derived*>(self)->getMaxCostImpl(level);
     //             }
     //     };
     // }
     */

    /**
     * @brief Base class for all enchantments.
     *
     * @details
     * The Enchant class encapsulates the common properties and behaviors of an enchantment,
     * including its name, ID, type, rarity, and maximum level. Derived classes are expected to
     * override the virtual functions to provide specific cost calculations and compatibility
     * checks. Note that the cost functions provided here are simple linear formulas which may be
     * overridden by derived classes for more complex behavior.
     *
     * @note The getName() function currently returns a std::string constructed from a C-string.
     * This could be optimized by using std::string_view or by directly returning the C-string.
     */
    class Enchant {
        const char* m_Name;              ///< The name of the enchantment.
        const eEnchantId m_Id;           ///< The unique identifier of the enchantment.
        const eEnchantType m_Type;       ///< The type/category of the enchantment.
        const eEnchantRarity m_Rarity;   ///< The rarity level of the enchantment.
        const u8 m_MaxLevel = 1;         ///< The maximum level for this enchantment.
    public:

        /**
         * @brief Deleted default constructor.
         *
         * Enchantments must be initialized with specific parameters.
         */
        Enchant() = delete;

        /**
         * @brief Constructs an Enchant with the given properties.
         *
         * @param name The name of the enchantment.
         * @param id The unique identifier for the enchantment.
         * @param type The type of the enchantment.
         * @param rarity The rarity of the enchantment.
         * @param maxLevel The maximum level of the enchantment.
         */
        constexpr Enchant(const char* name, const eEnchantId id,
                          const eEnchantType type, const eEnchantRarity rarity, c_u8 maxLevel) :
            m_Name(name), m_Id(id), m_Type(type), m_Rarity(rarity), m_MaxLevel(maxLevel) {}

        /**
         * @brief Virtual destructor.
         */
        virtual ~Enchant() noexcept;

        /**
         * @brief Returns the name of the enchantment.
         *
         * @return The enchantment name as a std::string.
         *
         * @note This currently converts the stored C-string into a std::string. Consider using std::string_view for efficiency.
         */
        ND std::string getName() const { return m_Name; }

        /**
         * @brief Returns the unique identifier of the enchantment.
         *
         * @return The enchantment's ID.
         */
        ND eEnchantId getID() const { return m_Id; }

        /**
         * @brief Returns the type of the enchantment.
         *
         * @return The enchantment type.
         */
        ND eEnchantType getType() const { return m_Type; }

        /**
         * @brief Returns the rarity of the enchantment.
         *
         * @return The rarity level.
         */
        ND eEnchantRarity getRarity() const { return m_Rarity; }

        /**
         * @brief Returns the maximum level of the enchantment.
         *
         * @return The maximum level.
         */
        ND u8 getMaxLevel() const { return m_MaxLevel; }

        /**
         * @brief Calculates the minimum cost for a given enchantment level.
         *
         * @param enchantLevel The level for which to calculate the minimum cost.
         * @return The minimum cost.
         *
         * @details This function provides a basic linear cost calculation: <c>1 + enchantLevel * 10</c>.
         * Derived classes may override this function to implement different cost formulas.
         */
        ND virtual int getMinCost(const int enchantLevel) const {
            return 1 + enchantLevel * 10;
        }

        /**
         * @brief Calculates the maximum cost for a given enchantment level.
         *
         * @param enchantLevel The level for which to calculate the maximum cost.
         * @return The maximum cost.
         *
         * @details This function provides a basic linear cost calculation: <c>6 + enchantLevel * 10</c>.
         * Derived classes may override this function to implement different cost formulas.
         */
        ND virtual int getMaxCost(const int enchantLevel) const {
            return 6 + enchantLevel * 10;
        }

        /**
         * @brief Checks if the provided cost is valid for a given enchantment level.
         *
         * @param level The enchantment level.
         * @param cost The cost to validate.
         * @return @c true if the cost is between the minimum and maximum cost (inclusive); otherwise, @c false.
         */
        MU ND bool isValidCost(const int level, const int cost) const {
            return cost >= getMinCost(level) && cost <= getMaxCost(level);
        }

        /**
         * @brief Determines if this enchantment can be applied to the given item.
         *
         * @param item The item state to check.
         * @return @c true if the enchantment can be applied; otherwise, @c false.
         *
         * @details By default, this function calls a helper function @c canEnchantItem with the item and
         * the enchantment type. Derived classes may override this behavior.
         */
        ND virtual bool canApply(const lce::ItemState& item) const {
            return canEnchantItem(item, m_Type);
        }

        /**
         * @brief Checks whether this enchantment can be applied together with another.
         *
         * @param enchant A pointer to another enchantment.
         * @return @c true if the two enchantments are compatible; otherwise, @c false.
         *
         * @details The default implementation considers two enchantments incompatible if they have the same ID.
         * Derived classes may override this function for more specific compatibility rules.
         */
        ND virtual bool canApplyTogether(const Enchant* enchant) const {
            return getID() != enchant->getID();
        }
    };
} // namespace enchants
