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
    extern const EnchantNormalProtection s_protection;

    /**
     * @brief Pre-constructed enchantment for Fire Protection.
     */
    extern const EnchantFireProtection s_fireProtection;

    /**
     * @brief Pre-constructed enchantment for Feather Falling.
     */
    extern const EnchantFeatherFalling s_featherFalling;

    /**
     * @brief Pre-constructed enchantment for Blast Protection.
     */
    extern const EnchantBlastProtection s_blastProtection;

    /**
     * @brief Pre-constructed enchantment for Projectile Protection.
     */
    extern const EnchantProjectileProtection s_projectileProtection;

    /**
     * @brief Pre-constructed enchantment for Respiration.
     */
    extern const EnchantOxygen s_respiration;

    /**
     * @brief Pre-constructed enchantment for Aqua Affinity.
     */
    extern const EnchantWaterWorker s_aquaAffinity;

    /**
     * @brief Pre-constructed enchantment for Thorns.
     */
    extern const EnchantThorns s_thorns;

    /**
     * @brief Pre-constructed enchantment for Depth Strider.
     */
    extern const EnchantWaterWalker s_depthStrider;

    /**
     * @brief Pre-constructed enchantment for Frost Walker.
     */
    extern const EnchantFrostWalker s_frostWalker;

    /**
     * @brief Pre-constructed enchantment for Curse Of Binding.
     */
    extern const EnchantBindingCurse s_bindingCurse;

    /**
     * @brief Pre-constructed enchantment for Sharpness.
     */
    extern const EnchantSharpness s_sharpness;

    /**
     * @brief Pre-constructed enchantment for Smite.
     */
    extern const EnchantSmite s_smite;

    /**
     * @brief Pre-constructed enchantment for Bane of Arthropods.
     */
    extern const EnchantBaneOfArthropods s_baneOfArthropods;

    /**
     * @brief Pre-constructed enchantment for Knockback.
     */
    extern const EnchantKnockback s_knockback;

    /**
     * @brief Pre-constructed enchantment for Fire Aspect.
     */
    extern const EnchantFireAspect s_fireAspect;

    /**
     * @brief Pre-constructed enchantment for Looting.
     */
    extern const EnchantLootBonus s_looting;

    /**
     * @brief Pre-constructed enchantment for Efficiency.
     */
    extern const EnchantDigging s_efficiency;

    /**
     * @brief Pre-constructed enchantment for Silk Touch.
     */
    extern const EnchantUntouching s_silkTouch;

    /**
     * @brief Pre-constructed enchantment for Unbreaking.
     */
    extern const EnchantDurability s_unbreaking;

    /**
     * @brief Pre-constructed enchantment for Fortune.
     */
    extern const EnchantLootBonus s_fortune;

    /**
     * @brief Pre-constructed enchantment for Power.
     */
    extern const EnchantArrowDamage s_power;

    /**
     * @brief Pre-constructed enchantment for Punch.
     */
    extern const EnchantArrowKnockback s_punch;

    /**
     * @brief Pre-constructed enchantment for Flame.
     */
    extern const EnchantArrowFire s_flame;

    /**
     * @brief Pre-constructed enchantment for Infinity.
     */
    extern const EnchantArrowInfinite s_infinity;

    /**
     * @brief Pre-constructed enchantment for Luck of the Sea.
     */
    extern const EnchantLootBonus s_luckOfTheSea;

    /**
     * @brief Pre-constructed enchantment for Lure.
     */
    extern const EnchantFishingSpeed s_lure;

    /**
     * @brief Pre-constructed enchantment for Mending.
     */
    extern const EnchantMending s_mending;

    /**
     * @brief Pre-constructed enchantment for Curse of Vanishing.
     */
    extern const EnchantVanishingCurse s_vanishingCurse;

    /**
     * @brief Pre-constructed enchantment for Impaling.
     */
    extern const EnchantTridentImpaler s_impaling;

    /**
     * @brief Pre-constructed enchantment for Riptide.
     */
    extern const EnchantTridentRiptide s_riptide;

    /**
     * @brief Pre-constructed enchantment for Loyalty.
     */
    extern const EnchantTridentLoyalty s_loyalty;

    /**
     * @brief Pre-constructed enchantment for Channeling.
     */
    extern const EnchantTridentChanneling s_channeling;

} // namespace enchants
