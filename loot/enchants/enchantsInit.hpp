#pragma once

/**
 * @file enchantsInit.hpp
 * @brief Contains static, pre-constructed enchantment objects.
 *
 * @details
 * This file declares a set of pre-constructed enchantment objects for use in the enchanting
 * system. These objects represent the various enchantments available in the system and are
 * stored as static constants. Each object is initialized with its name, unique identifier,
 * type, rarity, and maximum level. By creating these objects statically, the system avoids
 * dynamic allocation and can improve cache locality.
 */

#include "enchants.hpp"

namespace enchants {

    /**
     * @brief Pre-constructed enchantment for Protection.
     */
    static const EnchantNormalProtection s_protection("Protection", protection, eEnchantType::ARMOR, COMMON, 4);

    /**
     * @brief Pre-constructed enchantment for Fire Protection.
     */
    static const EnchantFireProtection s_fireProtection("Fire Protection", fire_protection, eEnchantType::ARMOR, UNCOMMON, 4);

    /**
     * @brief Pre-constructed enchantment for Feather Falling.
     */
    static const EnchantFeatherFalling s_featherFalling("Feather Falling", feather_falling, eEnchantType::ARMOR_FEET, UNCOMMON, 4);

    /**
     * @brief Pre-constructed enchantment for Blast Protection.
     */
    static const EnchantBlastProtection s_blastProtection("Blast Protection", blast_protection, eEnchantType::ARMOR, RARE, 4);

    /**
     * @brief Pre-constructed enchantment for Projectile Protection.
     */
    static const EnchantProjectileProtection s_projectileProtection("Projectile Protection", projectile_protection, eEnchantType::ARMOR, UNCOMMON, 4);

    /**
     * @brief Pre-constructed enchantment for Respiration.
     */
    static const EnchantOxygen s_respiration("Respiration", respiration, eEnchantType::ARMOR_HEAD, RARE, 3);

    /**
     * @brief Pre-constructed enchantment for Aqua Affinity.
     */
    static const EnchantWaterWorker s_aquaAffinity("Aqua Affinity", aqua_affinity, eEnchantType::ARMOR_HEAD, RARE, 1);

    /**
     * @brief Pre-constructed enchantment for Thorns.
     */
    static const EnchantThorns s_thorns("Thorns", thorns, eEnchantType::ARMOR_CHEST, EPIC, 3);

    /**
     * @brief Pre-constructed enchantment for Depth Strider.
     */
    static const EnchantWaterWalker s_depthStrider("Depth Strider", depth_strider, eEnchantType::ARMOR_FEET, RARE, 3);

    /**
     * @brief Pre-constructed enchantment for Frost Walker.
     */
    static const EnchantFrostWalker s_frostWalker("Frost Walker", frost_walker, eEnchantType::ARMOR_FEET, RARE, 2);

    /**
     * @brief Pre-constructed enchantment for Curse Of Binding.
     */
    static const EnchantBindingCurse s_bindingCurse("Curse Of Binding", binding_curse, eEnchantType::WEARABLE, EPIC, 1);

    /**
     * @brief Pre-constructed enchantment for Sharpness.
     */
    static const EnchantSharpness s_sharpness("Sharpness", sharpness, eEnchantType::WEAPON, COMMON, 5);

    /**
     * @brief Pre-constructed enchantment for Smite.
     */
    static const EnchantSmite s_smite("Smite", smite, eEnchantType::WEAPON, UNCOMMON, 5);

    /**
     * @brief Pre-constructed enchantment for Bane of Arthropods.
     */
    static const EnchantBaneOfArthropods s_baneOfArthropods("Bane of Arthropods", bane_of_arthropods, eEnchantType::WEAPON, UNCOMMON, 5);

    /**
     * @brief Pre-constructed enchantment for Knockback.
     */
    static const EnchantKnockback s_knockback("Knockback", knockback, eEnchantType::WEAPON, UNCOMMON, 2);

    /**
     * @brief Pre-constructed enchantment for Fire Aspect.
     */
    static const EnchantFireAspect s_fireAspect("Fire Aspect", fire_aspect, eEnchantType::WEAPON, RARE, 2);

    /**
     * @brief Pre-constructed enchantment for Looting.
     */
    static const EnchantLootBonus s_looting("Looting", looting, eEnchantType::WEAPON, RARE, 3);

    /**
     * @brief Pre-constructed enchantment for Efficiency.
     */
    static const EnchantDigging s_efficiency("Efficiency", efficiency, eEnchantType::DIGGER, COMMON, 5);

    /**
     * @brief Pre-constructed enchantment for Silk Touch.
     */
    static const EnchantUntouching s_silkTouch("Silk Touch", silk_touch, eEnchantType::DIGGER, EPIC, 1);

    /**
     * @brief Pre-constructed enchantment for Unbreaking.
     */
    static const EnchantDurability s_unbreaking("Unbreaking", unbreaking, eEnchantType::BREAKABLE, UNCOMMON, 3);

    /**
     * @brief Pre-constructed enchantment for Fortune.
     */
    static const EnchantLootBonus s_fortune("Fortune", fortune, eEnchantType::DIGGER, RARE, 3);

    /**
     * @brief Pre-constructed enchantment for Power.
     */
    static const EnchantArrowDamage s_power("Power", power, eEnchantType::BOW, COMMON, 5);

    /**
     * @brief Pre-constructed enchantment for Punch.
     */
    static const EnchantArrowKnockback s_punch("Punch", punch, eEnchantType::BOW, UNCOMMON, 2);

    /**
     * @brief Pre-constructed enchantment for Flame.
     */
    static const EnchantArrowFire s_flame("Flame", flame, eEnchantType::BOW, RARE, 1);

    /**
     * @brief Pre-constructed enchantment for Infinity.
     */
    static const EnchantArrowInfinite s_infinity("Infinity", infinity, eEnchantType::BOW, EPIC, 1);

    /**
     * @brief Pre-constructed enchantment for Luck of the Sea.
     */
    static const EnchantLootBonus s_luckOfTheSea("Luck of the Sea", luck_of_the_sea, eEnchantType::FISHING_ROD, RARE, 3);

    /**
     * @brief Pre-constructed enchantment for Lure.
     */
    static const EnchantFishingSpeed s_lure("Lure", lure, eEnchantType::FISHING_ROD, RARE, 3);

    /**
     * @brief Pre-constructed enchantment for Mending.
     */
    static const EnchantMending s_mending("Mending", mending, eEnchantType::BREAKABLE, RARE, 1);

    /**
     * @brief Pre-constructed enchantment for Curse of Vanishing.
     */
    static const EnchantVanishingCurse s_vanishingCurse("Curse of Vanishing", vanishing_curse, eEnchantType::ALL, EPIC, 1);

    /**
     * @brief Pre-constructed enchantment for Impaling.
     */
    static const EnchantTridentImpaler s_impaling("Impaling", impaling, eEnchantType::TRIDENT, UNCOMMON, 5);

    /**
     * @brief Pre-constructed enchantment for Riptide.
     */
    static const EnchantTridentRiptide s_riptide("Riptide", riptide, eEnchantType::TRIDENT, RARE, 3);

    /**
     * @brief Pre-constructed enchantment for Loyalty.
     */
    static const EnchantTridentLoyalty s_loyalty("Loyalty", loyalty, eEnchantType::TRIDENT, COMMON, 3);

    /**
     * @brief Pre-constructed enchantment for Channeling.
     */
    static const EnchantTridentChanneling s_channeling("Channeling", channeling, eEnchantType::TRIDENT, EPIC, 1);

} // namespace enchants
