#pragma once

/**
 * @file enchantType.hpp
 * @brief Provides functions and types for determining whether an item can be enchanted.
 *
 * @details
 * This file defines the @c eEnchantType enumeration which categorizes enchantments based on the type
 * of item they affect (e.g., armor, weapon, digger). It also defines a function pointer type
 * (@c CanEnchantItemFn) and a set of helper functions for determining whether a given item (represented
 * by an @c lce::ItemState) can be enchanted with a particular type. Additionally, a constant array,
 * @c ALL_ITERABLE, is provided to facilitate iterating over all applicable enchantment-checking functions.
 * Finally, overloaded versions of @c canEnchantItem are provided to check whether an item can be enchanted
 * for a specific @c eEnchantType.
 */

#include "lce/items/Item.hpp"
#include <array>

namespace enchants {

    /**
     * @brief Enumeration of enchantment types.
     *
     * @details
     * The @c eEnchantType enumeration specifies the different categories of enchantments.
     * For example, @c ARMOR specifies enchantments that apply to all armor, while
     * @c ARMOR_FEET and @c ARMOR_LEGS target specific armor pieces. The special value @c ALL
     * is used to indicate that an item can be enchanted with any enchantment (subject to additional rules).
     */
    enum class MU eEnchantType : i8 {
        ALL,           ///< Applies to all items.
        ARMOR,         ///< Applies to any armor.
        ARMOR_FEET,    ///< Applies specifically to footwear.
        ARMOR_LEGS,    ///< Applies specifically to leg armor.
        ARMOR_CHEST,   ///< Applies specifically to chest armor.
        ARMOR_HEAD,    ///< Applies specifically to head armor.
        WEAPON,        ///< Applies to melee weapons (e.g., swords).
        DIGGER,        ///< Applies to tools (e.g., axes, pickaxes).
        FISHING_ROD,   ///< Applies to fishing rods.
        BREAKABLE,     ///< Applies to damageable items.
        BOW,           ///< Applies to bows.
        WEARABLE,      ///< Applies to wearable items (beyond armor).
        TRIDENT        ///< Applies to tridents.
    };

    /**
     * @brief Function pointer type for checking if an item can be enchanted.
     *
     * @param itemIn A constant reference to an @c lce::ItemState.
     * @return @c true if the item can be enchanted with the given criteria; otherwise, @c false.
     */
    using CanEnchantItemFn = bool (*)(const lce::ItemState&);

    /**
     * @brief Checks if an armor item can be enchanted.
     *
     * @param itemIn The item state to check.
     * @return @c true if the item's type is @c ItemArmor; otherwise, @c false.
     */
    static bool Armor_canEnchantItem(const lce::ItemState& itemIn) {
        return itemIn.getItemType() == lce::items::ItemType::ItemArmor;
    }

    /**
     * @brief Checks if an item can be enchanted as footwear.
     *
     * @param itemIn The item state to check.
     * @return @c true if the item's armor type is @c FEET; otherwise, @c false.
     */
    static bool ArmorFeet_canEnchantItem(const lce::ItemState& itemIn) {
        return itemIn.getArmorType() == lce::items::ItemEquipSlot::FEET;
    }

    /**
     * @brief Checks if an item can be enchanted as leg armor.
     *
     * @param itemIn The item state to check.
     * @return @c true if the item's armor type is @c LEGS; otherwise, @c false.
     */
    static bool ArmorLegs_canEnchantItem(const lce::ItemState& itemIn) {
        return itemIn.getArmorType() == lce::items::ItemEquipSlot::LEGS;
    }

    /**
     * @brief Checks if an item can be enchanted as chest armor.
     *
     * @param itemIn The item state to check.
     * @return @c true if the item's armor type is @c CHEST; otherwise, @c false.
     */
    static bool ArmorChest_canEnchantItem(const lce::ItemState& itemIn) {
        return itemIn.getArmorType() == lce::items::ItemEquipSlot::CHEST;
    }

    /**
     * @brief Checks if an item can be enchanted as head armor.
     *
     * @param itemIn The item state to check.
     * @return @c true if the item's armor type is @c HEAD; otherwise, @c false.
     */
    static bool ArmorHead_canEnchantItem(const lce::ItemState& itemIn) {
        return itemIn.getArmorType() == lce::items::ItemEquipSlot::HEAD;
    }

    /**
     * @brief Checks if a weapon can be enchanted.
     *
     * @param itemIn The item state to check.
     * @return @c true if the item type is @c ItemSword; otherwise, @c false.
     */
    static bool Weapon_canEnchantItem(const lce::ItemState& itemIn) {
        return itemIn.getItemType() == lce::items::ItemType::ItemSword;
    }

    /**
     * @brief Checks if a tool can be enchanted.
     *
     * @param itemIn The item state to check.
     * @return @c true if the item type is @c ItemTool; otherwise, @c false.
     */
    static bool Digger_canEnchantItem(const lce::ItemState& itemIn) {
        return itemIn.getItemType() == lce::items::ItemType::ItemTool;
    }

    /**
     * @brief Checks if a fishing rod can be enchanted.
     *
     * @param itemIn The item state to check.
     * @return @c true if the item type is @c ItemFishingRod; otherwise, @c false.
     */
    static bool FishingRod_canEnchantItem(const lce::ItemState& itemIn) {
        return itemIn.getItemType() == lce::items::ItemType::ItemFishingRod;
    }

    /**
     * @brief Checks if a breakable (damageable) item can be enchanted.
     *
     * @param itemIn The item state to check.
     * @return @c true if the item is damageable; otherwise, @c false.
     */
    static bool Breakable_canEnchantItem(const lce::ItemState& itemIn) {
        return itemIn.isDamageable();
    }

    /**
     * @brief Checks if a bow can be enchanted.
     *
     * @param itemIn The item state to check.
     * @return @c true if the item type is @c ItemBow; otherwise, @c false.
     */
    static bool Bow_canEnchantItem(const lce::ItemState& itemIn) {
        return itemIn.getItemType() == lce::items::ItemType::ItemBow;
    }

    /**
     * @brief Checks if a trident can be enchanted.
     *
     * @param itemIn The item state to check.
     * @return @c true if the item type is @c ItemTrident; otherwise, @c false.
     */
    static bool Trident_canEnchantItem(const lce::ItemState& itemIn) {
        return itemIn.getItemType() == lce::items::ItemType::ItemTrident;
    }

    /**
     * @brief Checks if a wearable item can be enchanted.
     *
     * @param itemIn The item state to check.
     * @return @c true if the item has an armor type (other than NONE) or is a skull or elytra; otherwise, @c false.
     *
     * @details This function checks if the item is wearable by determining if it has a valid armor type or if it is one of
     * the specific items (skull or elytra) that can be enchanted despite not being standard armor.
     */
    static bool Wearable_canEnchantItem(const lce::ItemState& itemIn) {
        return itemIn.getArmorType() != lce::items::ItemEquipSlot::NONE ||
               itemIn.getItemType() == lce::items::ItemType::ItemSkull ||
               itemIn.getItemType() == lce::items::ItemType::ItemElytra;
    }

    /**
     * @brief An array of function pointers for checking if an item can be enchanted.
     *
     * @details
     * The array @c ALL_ITERABLE holds functions that correspond to each enchantment type.
     * The index in the array corresponds to the integer value of an @c eEnchantType, with the
     * first element (index 0) reserved (set to nullptr) for @c eEnchantType::ALL.
     */
    static constexpr std::array<CanEnchantItemFn, 13> ALL_ITERABLE = {
        nullptr,
        Armor_canEnchantItem,
        ArmorFeet_canEnchantItem,
        ArmorLegs_canEnchantItem,
        ArmorChest_canEnchantItem,
        ArmorHead_canEnchantItem,
        Weapon_canEnchantItem,
        Digger_canEnchantItem,
        FishingRod_canEnchantItem,
        Breakable_canEnchantItem,
        Bow_canEnchantItem,
        Wearable_canEnchantItem,
        Trident_canEnchantItem
    };

    /**
     * @brief Determines if an item can be enchanted with a specific type.
     *
     * @param itemIn The item state to check.
     * @param theType The enchantment type.
     * @return @c true if the item can be enchanted with the specified type; otherwise, @c false.
     *
     * @details
     * When @c theType is @c eEnchantType::ALL, the function iterates over all enchantment-checking functions
     * (skipping index 0) and returns @c true if any function returns @c true. Otherwise, it directly calls the function
     * corresponding to the given type.
     */
    constexpr bool canEnchantItem(const lce::ItemState& itemIn, const eEnchantType theType) {
        if (theType == eEnchantType::ALL) {
            for (size_t i = 1; i < ALL_ITERABLE.size(); ++i) {
                if (ALL_ITERABLE[i](itemIn)) {
                    return true;
                }
            }
            return false;
        }
        return ALL_ITERABLE[static_cast<int>(theType)](itemIn);
    }

    /**
     * @brief Template overload to determine if an item can be enchanted for a specific enchantment type.
     *
     * @tparam THE_TYPE The enchantment type.
     * @param itemIn The item state to check.
     * @return @c true if the item can be enchanted with the specified type; otherwise, @c false.
     *
     * @details
     * This template overload performs a compile-time check for the enchantment type.
     * For the special type @c eEnchantType::ALL, it iterates over all functions (skipping index 0).
     * Otherwise, it calls the function at the index corresponding to the type.
     */
    template<eEnchantType THE_TYPE>
    MU constexpr bool canEnchantItem(const lce::ItemState& itemIn) {
        if constexpr (THE_TYPE == eEnchantType::ALL) {
            for (size_t i = 1; i < ALL_ITERABLE.size(); ++i) {
                if (ALL_ITERABLE[i](itemIn)) {
                    return true;
                }
            }
            return false;
        }
        return ALL_ITERABLE[static_cast<int>(THE_TYPE)](itemIn);
    }

} // namespace enchants
