#pragma once

/**
 * @file enchants.hpp
 * @brief Provides concrete implementations for various enchantments.
 *
 * @details
 * This file defines a suite of classes derived from the base @c Enchant class, each representing
 * a specific enchantment type used in the system. These concrete classes override cost calculation
 * methods and, where applicable, the compatibility function (@c canApplyTogether) to implement the
 * behavior of different enchantments (e.g., Protection, Sharpness, Smite, etc.).
 *
 * The following enchantment classes are defined in this file:
 *  - @c EnchantArrowDamage: Handles the "Power" enchantment for bows.
 *  - @c EnchantArrowFire: Implements the "Flame" enchantment for bows.
 *  - @c EnchantArrowInfinite: Implements the "Infinity" enchantment for bows.
 *  - @c EnchantArrowKnockback: Implements the "Punch" enchantment for bows.
 *  - @c EnchantBindingCurse: Implements the "Curse of Binding" enchantment.
 *  - @c EnchantSharpness: Implements the "Sharpness" enchantment for melee weapons.
 *  - @c EnchantSmite: Implements the "Smite" enchantment for melee weapons.
 *  - @c EnchantBaneOfArthropods: Implements the "Bane of Arthropods" enchantment.
 *  - @c EnchantDigging: Implements the "Efficiency" enchantment for tools.
 *  - @c EnchantDurability: Implements the "Unbreaking" enchantment.
 *  - @c EnchantFireAspect: Implements the "Fire Aspect" enchantment for melee weapons.
 *  - @c EnchantFishingSpeed: Implements the "Lure" enchantment for fishing rods.
 *  - @c EnchantFrostWalker: Implements the "Frost Walker" enchantment for boots.
 *  - @c EnchantKnockback: Implements the "Knockback" enchantment for melee weapons.
 *  - @c EnchantLootBonus: Implements both the "Looting" and "Fortune" enchantments.
 *  - @c EnchantMending: Implements the "Mending" enchantment.
 *  - @c EnchantOxygen: Implements the "Respiration" enchantment for helmets.
 *  - @c EnchantThorns: Implements the "Thorns" enchantment for chest armor.
 *  - @c EnchantTridentChanneling: Implements the "Channeling" enchantment for tridents.
 *  - @c EnchantTridentImpaler: Implements the "Impaling" enchantment for tridents.
 *  - @c EnchantTridentLoyalty: Implements the "Loyalty" enchantment for tridents.
 *  - @c EnchantTridentRiptide: Implements the "Riptide" enchantment for tridents.
 *  - @c EnchantUntouching: Implements the "Silk Touch" enchantment for tools.
 *  - @c EnchantVanishingCurse: Implements the "Curse of Vanishing" enchantment.
 *  - @c EnchantWaterWalker: Implements the "Depth Strider" (or water walking) enchantment.
 *  - @c EnchantWaterWorker: Implements the "Aqua Affinity" enchantment.
 *  - @c EnchantNormalProtection: Implements the "Protection" enchantment for armor.
 *  - @c EnchantFireProtection: Implements the "Fire Protection" enchantment for armor.
 *  - @c EnchantFeatherFalling: Implements the "Feather Falling" enchantment for boots.
 *  - @c EnchantBlastProtection: Implements the "Blast Protection" enchantment for armor.
 *  - @c EnchantProjectileProtection: Implements the "Projectile Protection" enchantment for armor.
 *
 * @note A macro (@c ENCHANTMENT_CONSTRUCTOR) is used throughout this file to reduce boilerplate code
 * in the constructors for each enchantment class.
 */

#include "enchant.hpp"
#include "lce/items/itemID.hpp"

namespace enchants {

    // Cost arrays for various enchantments

    static constexpr i8 KNOCKBACK_OXYGEN_MAX[2] = {61, 71};
    static constexpr i8 FIRE_ASPECT_MAX[2] =      {61, 71};
    static constexpr i8 DURABILITY_MAX[3] =       {61, 71, 81};
    static constexpr i8 LOOT_BONUS_MAX[3] =       {61, 71, 81};
    static constexpr i8 THORNS_MAX[3] =           {61, 71, 81};
    static constexpr i8 FISHING_SPEED_MAX[3] =    {61, 71, 81};
    static constexpr i8 DIGGING_MAX[5] =          {61, 71, 81, 91, 101};

    static constexpr i8 NORMAL_PROT_MIN[4] =      { 1, 12, 23, 34 };
    static constexpr i8 SHARPNESS_MIN[5] = { 1, 12, 23, 34, 45 };
    static constexpr i8 NORMAL_PROT_MAX[4] =         { 12, 23, 34, 45 };

    static constexpr i8 DURABILITY_MIN[3] =      {5, 13, 21};
    static constexpr i8 SMITE_BANE_MIN[5] =      {5, 13, 21, 29, 37 };
    static constexpr i8 ARROW_KNOCKBACK_MAX[2] =                {37, 57};

    static constexpr i8 FROST_WALKER_MIN[2] = {10, 20};
    static constexpr i8 OXYGEN_MIN[3] =       {10, 20, 30};
    static constexpr i8 WATER_WALKER_MIN[3] = {10, 20, 30};

    static constexpr i8 ARROW_DAMAGE_MIN[5] = {1, 11, 21, 31, 41};
    static constexpr i8 DIGGING_MIN[5] =      {1, 11, 21, 31, 41};

    static constexpr i8 WATER_WALKER_MAX[3] =     {26, 36, 46};
    static constexpr i8 ARROW_DAMAGE_MAX[5] = {16, 26, 36, 46, 56};

    static constexpr i8 FIRE_ASPECT_MIN[2] = {10, 30};
    static constexpr i8 THORNS_MIN[3] =      {10, 30, 50};

    static constexpr i8 RIPTIDE_WATER_WALKER_MIN[3] = {17, 24, 31};
    static constexpr i8 TRIDENT_IMPALER_MIN[5] =      {17, 24, 31, 38, 45};

    static constexpr i8 FIRE_PROT_MIN[4] = { 10, 18, 26, 34 };
    static constexpr i8 FIRE_PROT_MAX[4] =     { 18, 26, 34, 42 };

    static constexpr i8 FEATHER_FALLING_MIN[4] = { 5, 11, 17, 23 };
    static constexpr i8 FEATHER_FALLING_MAX[4] =    { 11, 17, 23, 29 };

    static constexpr i8 BLAST_PROT_MIN[4] = { 5, 13, 21, 29 };
    static constexpr i8 BLAST_PROT_MAX[4] =    { 13, 21, 29, 37 };

    static constexpr i8 PROJECTILE_PROT_MIN[4] = { 3, 9, 15, 21 };
    static constexpr i8 PROJECTILE_PROT_MAX[4] =    { 9, 15, 21, 27 };

    static constexpr i8 KNOCKBACK_OXYGEN_MIN[2] = {5, 25};
    static constexpr i8 FROST_WALKER_MAX[2] = {25, 35};

    static constexpr i8 FISHING_SPEED_MIN[3] = {15, 24, 33};
    static constexpr i8 LOOT_BONUS_MIN[3] =    {15, 24, 33};

    static constexpr i8 SHARPNESS_MAX[5] = { 21, 32, 43, 54, 65 };

    static constexpr i8 TRIDENT_LOYALTY_MIN[3] = {12, 19, 26};

    static constexpr i8 SMITE_BANE_MAX[5] = { 25, 33, 41, 49, 57 };

    static constexpr i8 OXYGEN_MAX[3] = {40, 50, 60};

    static constexpr i8 ARROW_KNOCKBACK_MIN[2] = {12, 32};


    /// Macro to generate a constructor for an enchantment.
#define ENCHANTMENT_CONSTRUCTOR(func_name) \
    func_name(const char* name, const eEnchantId id, \
        const eEnchantType type, const eEnchantRarity rarity, c_u8 maxLevel) : \
        Enchant(name, id, type, rarity, maxLevel)

    /// Macro to generate a deconstructor for an enchantment.
#define ENCHANTMENT_DECONSTRUCTOR(func_name) \
    ~func_name() noexcept override

    /* Concrete enchantment classes follow. Each class provides an implementation for
     * cost functions and (if necessary) compatibility checks using canApplyTogether.
     */

    class EnchantArrowDamage final : public Enchant {
    public:
        ENCHANTMENT_CONSTRUCTOR(EnchantArrowDamage) {}
        ENCHANTMENT_DECONSTRUCTOR(EnchantArrowDamage);

        MU ND int getMinCost(c_int enchantLevel) const override { return ARROW_DAMAGE_MIN[enchantLevel - 1]; }
        MU ND int getMaxCost(c_int enchantLevel) const override { return ARROW_DAMAGE_MAX[enchantLevel - 1]; }
    };


    class EnchantArrowFire final : public Enchant {
    public:
        ENCHANTMENT_CONSTRUCTOR(EnchantArrowFire) {}
        ENCHANTMENT_DECONSTRUCTOR(EnchantArrowFire);

        MU ND int getMinCost(MU c_int enchantLevel) const override { return 20; }
        MU ND int getMaxCost(MU c_int enchantLevel) const override { return 50; }
    };


    class EnchantArrowInfinite final : public Enchant {
    public:
        ENCHANTMENT_CONSTRUCTOR(EnchantArrowInfinite) {}
        ENCHANTMENT_DECONSTRUCTOR(EnchantArrowInfinite);

        MU ND int getMinCost(MU c_int enchantLevel) const override { return 20; }
        MU ND int getMaxCost(MU c_int enchantLevel) const override { return 50; }

        ND bool canApplyTogether(const Enchant* enchant) const override {
            static constexpr u64 mask = ~(1ULL << infinity | 1ULL << mending);
            return (mask & 1ULL << enchant->getID()) != 0;
        }
    };


    class EnchantArrowKnockback final : public Enchant {
    public:
        ENCHANTMENT_CONSTRUCTOR(EnchantArrowKnockback) {}
        ENCHANTMENT_DECONSTRUCTOR(EnchantArrowKnockback);

        MU ND int getMinCost(c_int enchantLevel) const override { return ARROW_KNOCKBACK_MIN[enchantLevel - 1]; }
        MU ND int getMaxCost(c_int enchantLevel) const override { return ARROW_KNOCKBACK_MAX[enchantLevel - 1]; }
    };


    class EnchantBindingCurse final : public Enchant {
    public:
        ENCHANTMENT_CONSTRUCTOR(EnchantBindingCurse) {}
        ENCHANTMENT_DECONSTRUCTOR(EnchantBindingCurse);

        MU ND int getMinCost(MU c_int enchantLevel) const override { return 25; }
        MU ND int getMaxCost(MU c_int enchantLevel) const override { return 50; }
    };


    // canApply should be
    // item->getID() == ANY AXE ID || Enchant::canApply(item);
    // but loot tables never try to enchant axes
    class EnchantSharpness final : public Enchant {
    public:
        ENCHANTMENT_CONSTRUCTOR(EnchantSharpness) {}
        ENCHANTMENT_DECONSTRUCTOR(EnchantSharpness);

        MU ND int getMinCost(c_int enchantLevel) const override { return SHARPNESS_MIN[enchantLevel - 1]; }
        MU ND int getMaxCost(c_int enchantLevel) const override { return SHARPNESS_MAX[enchantLevel - 1]; }

        ND bool canApplyTogether(const Enchant* enchant) const override {
            static constexpr u64 mask = ~(1ULL << sharpness | 1ULL << smite | 1ULL << bane_of_arthropods);
            return (mask & 1ULL << enchant->getID()) != 0;
        }
    };

    // canApply should be
    // item->getID() == ANY AXE ID || Enchant::canApply(item);
    // but loot tables never try to enchant axes
    class EnchantSmite final : public Enchant {
    public:
        ENCHANTMENT_CONSTRUCTOR(EnchantSmite) {}
        ENCHANTMENT_DECONSTRUCTOR(EnchantSmite);

        MU ND int getMinCost(c_int enchantLevel) const override { return SMITE_BANE_MIN[enchantLevel - 1]; }
        MU ND int getMaxCost(c_int enchantLevel) const override { return SMITE_BANE_MAX[enchantLevel - 1]; }

        ND bool canApplyTogether(const Enchant* enchant) const override {
            static constexpr u64 mask = ~(1ULL << sharpness | 1ULL << smite | 1ULL << bane_of_arthropods);
            return (mask & 1ULL << enchant->getID()) != 0;
        }
    };


    // canApply should be
    // item->getID() == ANY AXE ID || Enchant::canApply(item);
    // but loot tables never try to enchant axes
    class EnchantBaneOfArthropods final : public Enchant {
    public:
        ENCHANTMENT_CONSTRUCTOR(EnchantBaneOfArthropods) {}
        ENCHANTMENT_DECONSTRUCTOR(EnchantBaneOfArthropods);

        MU ND int getMinCost(c_int enchantLevel) const override { return SMITE_BANE_MIN[enchantLevel - 1]; }
        MU ND int getMaxCost(c_int enchantLevel) const override { return SMITE_BANE_MAX[enchantLevel - 1]; }

        ND bool canApplyTogether(const Enchant* enchant) const override {
            static constexpr u64 mask = ~(1ULL << sharpness | 1ULL << smite | 1ULL << bane_of_arthropods);
            return (mask & 1ULL << enchant->getID()) != 0;
        }
    };


    class EnchantDigging final : public Enchant {
    public:
        ENCHANTMENT_CONSTRUCTOR(EnchantDigging) {}
        ENCHANTMENT_DECONSTRUCTOR(EnchantDigging);

        MU ND int getMinCost(c_int enchantLevel) const override { return DIGGING_MIN[enchantLevel - 1]; }
        MU ND int getMaxCost(c_int enchantLevel) const override { return DIGGING_MAX[enchantLevel - 1]; }
    };


    class EnchantDurability final : public Enchant {
    public:
        ENCHANTMENT_CONSTRUCTOR(EnchantDurability) {}
        ENCHANTMENT_DECONSTRUCTOR(EnchantDurability);

        MU ND int getMinCost(c_int enchantLevel) const override { return DURABILITY_MIN[enchantLevel - 1]; }
        MU ND int getMaxCost(c_int enchantLevel) const override { return DURABILITY_MAX[enchantLevel - 1]; }
    };


    class EnchantFireAspect final : public Enchant {
    public:
        ENCHANTMENT_CONSTRUCTOR(EnchantFireAspect) {}
        ENCHANTMENT_DECONSTRUCTOR(EnchantFireAspect);

        MU ND int getMinCost(c_int enchantLevel) const override { return FIRE_ASPECT_MIN[enchantLevel - 1]; }
        MU ND int getMaxCost(c_int enchantLevel) const override { return FIRE_ASPECT_MAX[enchantLevel - 1]; }
    };


    class EnchantFishingSpeed final : public Enchant {
    public:
        ENCHANTMENT_CONSTRUCTOR(EnchantFishingSpeed) {}
        ENCHANTMENT_DECONSTRUCTOR(EnchantFishingSpeed);

        MU ND int getMinCost(c_int enchantLevel) const override { return FISHING_SPEED_MIN[enchantLevel - 1]; }
        MU ND int getMaxCost(c_int enchantLevel) const override { return FISHING_SPEED_MAX[enchantLevel - 1]; }
    };


    class EnchantFrostWalker final : public Enchant {
    public:
        ENCHANTMENT_CONSTRUCTOR(EnchantFrostWalker) {}
        ENCHANTMENT_DECONSTRUCTOR(EnchantFrostWalker);

        MU ND int getMinCost(c_int enchantLevel) const override { return FROST_WALKER_MIN[enchantLevel - 1]; }
        MU ND int getMaxCost(c_int enchantLevel) const override { return FROST_WALKER_MAX[enchantLevel - 1]; }

        ND bool canApplyTogether(const Enchant* enchant) const override {
            static constexpr u64 mask = ~(1ULL << frost_walker | 1ULL << depth_strider);
            return (mask & 1ULL << enchant->getID()) != 0;
        }
    };


    class EnchantKnockback final : public Enchant {
    public:
        ENCHANTMENT_CONSTRUCTOR(EnchantKnockback) {}
        ENCHANTMENT_DECONSTRUCTOR(EnchantKnockback);

        MU ND int getMinCost(c_int enchantLevel) const override { return KNOCKBACK_OXYGEN_MIN[enchantLevel - 1]; }
        MU ND int getMaxCost(c_int enchantLevel) const override { return KNOCKBACK_OXYGEN_MAX[enchantLevel - 1]; }
    };


    class EnchantLootBonus final : public Enchant {
    public:
        ENCHANTMENT_CONSTRUCTOR(EnchantLootBonus) {}
        ENCHANTMENT_DECONSTRUCTOR(EnchantLootBonus);

        MU ND int getMinCost(c_int enchantLevel) const override { return LOOT_BONUS_MIN[enchantLevel - 1]; }
        MU ND int getMaxCost(c_int enchantLevel) const override { return LOOT_BONUS_MAX[enchantLevel - 1]; }

        ND bool canApplyTogether(const Enchant* enchant) const override {
            return getID() != enchant->getID() &&
                   enchant->getID() != silk_touch;
        }
    };


    class EnchantMending final : public Enchant {
    public:
        ENCHANTMENT_CONSTRUCTOR(EnchantMending) {}
        ENCHANTMENT_DECONSTRUCTOR(EnchantMending);

        MU ND int getMinCost(MU c_int enchantLevel) const override { return 25; }
        MU ND int getMaxCost(MU c_int enchantLevel) const override { return 75; }

        ND bool canApplyTogether(const Enchant* enchant) const override {
            static constexpr u64 mask = ~(1ULL << mending | 1ULL << infinity);
            return (mask & 1ULL << enchant->getID()) != 0;
        }
    };


    class EnchantOxygen final : public Enchant {
    public:
        ENCHANTMENT_CONSTRUCTOR(EnchantOxygen) {}
        ENCHANTMENT_DECONSTRUCTOR(EnchantOxygen);

        MU ND int getMinCost(c_int enchantLevel) const override { return OXYGEN_MIN[enchantLevel - 1]; }
        MU ND int getMaxCost(c_int enchantLevel) const override { return OXYGEN_MAX[enchantLevel - 1]; }
    };


    class EnchantThorns final : public Enchant {
    public:
        ENCHANTMENT_CONSTRUCTOR(EnchantThorns) {}
        ENCHANTMENT_DECONSTRUCTOR(EnchantThorns);

        MU ND int getMinCost(c_int enchantLevel) const override { return THORNS_MIN[enchantLevel - 1]; }
        MU ND int getMaxCost(c_int enchantLevel) const override { return THORNS_MAX[enchantLevel - 1]; }

        ND bool canApply(const lce::ItemState& item) const override {
            return item.getItemType() == lce::items::ItemType::ItemArmor
                   || ArmorChest_canEnchantItem(item);
        }
    };


    class EnchantTridentChanneling final : public Enchant {
    public:
        ENCHANTMENT_CONSTRUCTOR(EnchantTridentChanneling) {}
        ENCHANTMENT_DECONSTRUCTOR(EnchantTridentChanneling);

        MU ND int getMinCost(MU c_int enchantLevel) const override { return 25; }
        MU ND int getMaxCost(MU c_int enchantLevel) const override { return 50; }

        ND bool canApplyTogether(const Enchant* enchant) const override {
            static constexpr u64 mask = ~(1ULL << channeling | 1ULL << riptide);
            return (mask & 1ULL << enchant->getID()) != 0;
        }
    };


    class EnchantTridentImpaler final : public Enchant {
    public:
        ENCHANTMENT_CONSTRUCTOR(EnchantTridentImpaler) {}
        ENCHANTMENT_DECONSTRUCTOR(EnchantTridentImpaler);

        MU ND int getMinCost(c_int enchantLevel) const override { return TRIDENT_IMPALER_MIN[enchantLevel - 1]; }
        MU ND int getMaxCost(MU c_int enchantLevel) const override { return 50; }
    };


    class EnchantTridentLoyalty final : public Enchant {
    public:
        ENCHANTMENT_CONSTRUCTOR(EnchantTridentLoyalty) {}
        ENCHANTMENT_DECONSTRUCTOR(EnchantTridentLoyalty);

        MU ND int getMinCost(c_int enchantLevel) const override { return TRIDENT_LOYALTY_MIN[enchantLevel - 1]; }
        MU ND int getMaxCost(MU c_int enchantLevel) const override { return 50; }

        ND bool canApplyTogether(const Enchant* enchant) const override {
            static constexpr u64 mask = ~(1ULL << loyalty | 1ULL << riptide);
            return (mask & 1ULL << enchant->getID()) != 0;
        }
    };


    class EnchantTridentRiptide final : public Enchant {
    public:
        ENCHANTMENT_CONSTRUCTOR(EnchantTridentRiptide) {}
        ENCHANTMENT_DECONSTRUCTOR(EnchantTridentRiptide);

        MU ND int getMinCost(c_int enchantLevel) const override { return RIPTIDE_WATER_WALKER_MIN[enchantLevel - 1]; }
        MU ND int getMaxCost(MU c_int enchantLevel) const override { return 50; }

        ND bool canApplyTogether(const Enchant* enchant) const override {
            static constexpr u64 mask = ~(1ULL << riptide | 1ULL << loyalty | 1ULL << channeling);
            return (mask & 1ULL << enchant->getID()) != 0;
        }
    };


    class EnchantUntouching final : public Enchant {
    public:
        ENCHANTMENT_CONSTRUCTOR(EnchantUntouching) {}
        ENCHANTMENT_DECONSTRUCTOR(EnchantUntouching);

        MU ND int getMinCost(MU c_int enchantLevel) const override { return 15; }
        MU ND int getMaxCost(MU c_int enchantLevel) const override { return 61; }

        ND bool canApplyTogether(const Enchant* enchant) const override {
            static constexpr u64 mask = ~(1ULL << silk_touch | 1ULL << fortune |
                                          1ULL << looting | 1ULL << luck_of_the_sea);
            return (mask & 1ULL << enchant->getID()) != 0;
        }
    };


    class EnchantVanishingCurse final : public Enchant {
    public:
        ENCHANTMENT_CONSTRUCTOR(EnchantVanishingCurse) {}
        ENCHANTMENT_DECONSTRUCTOR(EnchantVanishingCurse);

        MU ND int getMinCost(MU c_int enchantLevel) const override { return 25; }
        MU ND int getMaxCost(MU c_int enchantLevel) const override { return 50; }
    };


    class EnchantWaterWalker final : public Enchant {
    public:
        ENCHANTMENT_CONSTRUCTOR(EnchantWaterWalker) {}
        ENCHANTMENT_DECONSTRUCTOR(EnchantWaterWalker);

        MU ND int getMinCost(c_int enchantLevel) const override { return WATER_WALKER_MIN[enchantLevel - 1]; }
        MU ND int getMaxCost(c_int enchantLevel) const override { return WATER_WALKER_MAX[enchantLevel - 1]; }

        ND bool canApplyTogether(const Enchant* enchant) const override {
            static constexpr u64 mask = ~(1ULL << depth_strider | 1ULL << frost_walker);
            return (mask & 1ULL << enchant->getID()) != 0;
        }
    };


    class EnchantWaterWorker final : public Enchant {
    public:
        ENCHANTMENT_CONSTRUCTOR(EnchantWaterWorker) {}
        ENCHANTMENT_DECONSTRUCTOR(EnchantWaterWorker);

        MU ND int getMinCost(MU c_int enchantLevel) const override { return 1; }
        MU ND int getMaxCost(MU c_int enchantLevel) const override { return 41; }
    };



    class EnchantNormalProtection final : public Enchant {
    public:
        ENCHANTMENT_CONSTRUCTOR(EnchantNormalProtection) {}
        ENCHANTMENT_DECONSTRUCTOR(EnchantNormalProtection);

        MU ND int getMinCost(c_int enchantLevel) const override { return NORMAL_PROT_MIN[enchantLevel - 1]; }
        MU ND int getMaxCost(c_int enchantLevel) const override { return NORMAL_PROT_MAX[enchantLevel - 1]; }


        ND bool canApplyTogether(const Enchant* enchant) const override {
            static constexpr u64 mask = ~(
                    1ULL << protection | 1ULL << fire_protection |
                    1ULL << projectile_protection | 1ULL << blast_protection);
            return (mask & 1ULL << enchant->getID()) != 0;
        }
    };


    class EnchantFireProtection final : public Enchant {
    public:
        ENCHANTMENT_CONSTRUCTOR(EnchantFireProtection) {}
        ENCHANTMENT_DECONSTRUCTOR(EnchantFireProtection);

        MU ND int getMinCost(c_int enchantLevel) const override { return FIRE_PROT_MIN[enchantLevel - 1]; }
        MU ND int getMaxCost(c_int enchantLevel) const override { return FIRE_PROT_MAX[enchantLevel - 1]; }

        ND bool canApplyTogether(const Enchant* enchant) const override {
            static constexpr u64 mask = ~(
                    1ULL << protection | 1ULL << fire_protection |
                    1ULL << projectile_protection | 1ULL << blast_protection);
            return (mask & 1ULL << enchant->getID()) != 0;
        }
    };


    class EnchantFeatherFalling final : public Enchant {
    public:
        ENCHANTMENT_CONSTRUCTOR(EnchantFeatherFalling) {}
        ENCHANTMENT_DECONSTRUCTOR(EnchantFeatherFalling);

        MU ND int getMinCost(c_int enchantLevel) const override { return FEATHER_FALLING_MIN[enchantLevel - 1]; }
        MU ND int getMaxCost(c_int enchantLevel) const override { return FEATHER_FALLING_MAX[enchantLevel - 1]; }

        ND bool canApplyTogether(const Enchant* enchant) const override {
            static constexpr u64 mask = ~(1ULL << feather_falling);
            return (mask & 1ULL << enchant->getID()) != 0;
        }
    };


    class EnchantBlastProtection final : public Enchant {
    public:
        ENCHANTMENT_CONSTRUCTOR(EnchantBlastProtection) {}
        ENCHANTMENT_DECONSTRUCTOR(EnchantBlastProtection);

        MU ND int getMinCost(c_int enchantLevel) const override { return BLAST_PROT_MIN[enchantLevel - 1]; }
        MU ND int getMaxCost(c_int enchantLevel) const override { return BLAST_PROT_MAX[enchantLevel - 1]; }

        ND bool canApplyTogether(const Enchant* enchant) const override {
            static constexpr u64 mask = ~(
                    1ULL << protection | 1ULL << fire_protection |
                    1ULL << projectile_protection | 1ULL << blast_protection);
            return (mask & 1ULL << enchant->getID()) != 0;
        }
    };


    class EnchantProjectileProtection final : public Enchant {
    public:
        ENCHANTMENT_CONSTRUCTOR(EnchantProjectileProtection) {}
        ENCHANTMENT_DECONSTRUCTOR(EnchantProjectileProtection);

        MU ND int getMinCost(c_int enchantLevel) const override { return PROJECTILE_PROT_MIN[enchantLevel - 1]; }
        MU ND int getMaxCost(c_int enchantLevel) const override { return PROJECTILE_PROT_MAX[enchantLevel - 1]; }

        ND bool canApplyTogether(const Enchant* enchant) const override {
            static constexpr u64 mask = ~(
                    1ULL << protection | 1ULL << fire_protection |
                    1ULL << projectile_protection | 1ULL << blast_protection);
            return (mask & 1ULL << enchant->getID()) != 0;
        }
    };


#undef ENCHANTMENT_CONSTRUCTOR

} // namespace enchants
