#pragma once

/**
 * @file enchantEnums.hpp
 * @brief Defines enumeration types for enchantment identifiers and rarities.
 *
 * @details
 * This file declares the enumerations used throughout the enchantment system.
 * The @c eEnchantId enum defines a unique identifier for each enchantment type,
 * and the @c eEnchantRarity enum represents the rarity of an enchantment, which is
 * used to influence selection probabilities and cost calculations.
 */

#include "lce/processor.hpp"

namespace enchants {

    /**
     * @brief Unique identifiers for each enchantment.
     *
     * @details
     * The @c eEnchantId enum assigns a unique integer value to each enchantment type.
     * These IDs are used for comparisons, lookups, and in weighted selection algorithms.
     * The last value, @c MAX_ENCHANTMENT_COUNT, defines the total number of enchantments.
     */
    enum eEnchantId : i8 {
        protection = 0,            ///< Standard protection enchantment.
        fire_protection = 1,       ///< Enchantment for protection against fire.
        feather_falling = 2,       ///< Enchantment to reduce fall damage.
        blast_protection = 3,      ///< Enchantment for protection against explosions.
        projectile_protection = 4, ///< Enchantment for protection against projectiles.
        respiration = 5,           ///< Enchantment to extend underwater breathing.
        aqua_affinity = 6,         ///< Enchantment to speed up underwater mining.
        thorns = 7,                ///< Enchantment that damages attackers.
        depth_strider = 8,         ///< Enchantment to increase underwater movement speed.
        frost_walker = 9,          ///< Enchantment to create ice blocks when walking on water.
        binding_curse = 10,        ///< Curse that prevents removal of equipped items.
        sharpness = 11,            ///< Enchantment to increase melee damage.
        smite = 12,                ///< Enchantment to increase damage against undead.
        bane_of_arthropods = 13,   ///< Enchantment to increase damage against arthropods.
        knockback = 14,            ///< Enchantment to increase knockback effect.
        fire_aspect = 15,          ///< Enchantment that sets targets on fire.
        looting = 16,              ///< Enchantment to increase item drops.
        efficiency = 17,           ///< Enchantment to increase mining speed.
        silk_touch = 18,           ///< Enchantment to drop blocks themselves instead of items.
        unbreaking = 19,           ///< Enchantment to improve durability.
        fortune = 20,              ///< Enchantment to increase ore drops.
        power = 21,                ///< Enchantment to increase bow damage.
        punch = 22,                ///< Enchantment to increase bow knockback.
        flame = 23,                ///< Enchantment for setting arrows on fire.
        infinity = 24,             ///< Enchantment to allow infinite arrows.
        luck_of_the_sea = 25,      ///< Enchantment to increase treasure chance while fishing.
        lure = 26,                 ///< Enchantment to decrease fish bite time.
        mending = 27,              ///< Enchantment to repair items using experience.
        vanishing_curse = 28,      ///< Curse that causes items to vanish on death.
        impaling = 29,             ///< Enchantment to increase damage against sea creatures.
        riptide = 30,              ///< Enchantment for propelling the player when in water.
        loyalty = 31,              ///< Enchantment for returning thrown tridents.
        channeling = 32,           ///< Enchantment to summon lightning with tridents.
        MAX_ENCHANTMENT_COUNT = 33 ///< Total number of enchantments.
    };

    /**
     * @brief Rarity levels for enchantments.
     *
     * @details
     * The @c eEnchantRarity enum defines the relative rarity of enchantments.
     * Lower numeric values indicate a higher rarity (i.e., EPIC is rarer than COMMON).
     * These rarity values are used in cost calculations and for weighted random selection.
     */
    enum eEnchantRarity : u8 {
        NONE = 0,     ///< No rarity specified.
        COMMON = 10,  ///< Common enchantments.
        UNCOMMON = 5, ///< Uncommon enchantments.
        RARE = 2,     ///< Rare enchantments.
        EPIC = 1      ///< Epic enchantments.
    };

} // namespace enchants
