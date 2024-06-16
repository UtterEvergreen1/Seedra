#pragma once

#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "LegacyCubiomes/cubiomes/generator.hpp"
#include "enchantmentRegistry.hpp"
#include "lce/items/items.hpp"
#include "rarity.hpp"

class EnchantmentHelper;
class EnchantmentRegistry;

class Enchantment {
    static bool isSetup;

public:
    friend class EnchantmentHelper;

    class Type {
    public:
        class Base {
        public:
            virtual ~Base() = default;
            virtual bool canEnchantItem(const lce::items::Item* itemIn) const;
        };
        class All final : public Base {
        public:
            bool canEnchantItem(const lce::items::Item* itemIn) const override;
        };
        class Armor final : public Base {
        public:
            bool canEnchantItem(const lce::items::Item* itemIn) const override;
        };
        class ArmorFeet final : public Base {
        public:
            bool canEnchantItem(const lce::items::Item* itemIn) const override;
        };
        class ArmorLegs final : public Base {
        public:
            bool canEnchantItem(const lce::items::Item* itemIn) const override;
        };
        class ArmorChest final : public Base {
        public:
            bool canEnchantItem(const lce::items::Item* itemIn) const override;
        };
        class ArmorHead final : public Base {
        public:
            bool canEnchantItem(const lce::items::Item* itemIn) const override;
        };
        class Weapon final : public Base {
        public:
            bool canEnchantItem(const lce::items::Item* itemIn) const override;
        };
        class Digger final : public Base {
        public:
            bool canEnchantItem(const lce::items::Item* itemIn) const override;
        };
        class FishingRod final : public Base {
        public:
            bool canEnchantItem(const lce::items::Item* itemIn) const override;
        };
        class Breakable final : public Base {
        public:
            bool canEnchantItem(const lce::items::Item* itemIn) const override;
        };
        class Bow final : public Base {
        public:
            bool canEnchantItem(const lce::items::Item* itemIn) const override;
        };
        class Wearable final : public Base {
        public:
            bool canEnchantItem(const lce::items::Item* itemIn) const override;
        };
        class Trident final : public Base {
        public:
            bool canEnchantItem(const lce::items::Item* itemIn) const override;
        };

        static const All ALL;
        static const Armor ARMOR;
        static const ArmorFeet ARMOR_FEET;
        static const ArmorLegs ARMOR_LEGS;
        static const ArmorChest ARMOR_CHEST;
        static const ArmorHead ARMOR_HEAD;
        static const Weapon WEAPON;
        static const Digger DIGGER;
        static const FishingRod FISHING_ROD;
        static const Breakable BREAKABLE;
        static const Bow BOW;
        static const Wearable WEARABLE;
        static const Trident TRIDENT;

        static std::vector<const Base*> ALL_ITERABLE;
    };

    enum class EnumName : i8 {
        OXYGEN,
        PROTECTION,
        WATER_WORKER,
        THORNS,
        WATER_WALKER,
        FROST_WALKER,
        BINDING_CURSE,
        DAMAGE,
        KNOCKBACK,
        FIRE_ASPECT,
        LOOT_BONUS,
#ifdef INCLUDE_JAVA
        SWEEPING,
#endif
        DIGGING,
        UNTOUCHING,
        DURABILITY,
        ARROW_DAMAGE,
        ARROW_KNOCKBACK,
        ARROW_FIRE,
        ARROW_INFINITE,
        FISHING_SPEED,
        MENDING,
        VANISHING_CURSE,
        TRIDENT_IMPALER,
        TRIDENT_RIPTIDE,
        TRIDENT_LOYALTY,
        TRIDENT_CHANNELING,
        NONE
    };

    enum EnchantmentOrder : i8 {
        protection = 0,
        fire_protection = 1,
        feather_falling = 2,
        blast_protection = 3,
        projectile_protection = 4,
        respiration = 5,
        aqua_affinity = 6,
        thorns = 7,
        depth_strider = 8,
        frost_walker = 9,
        binding_curse = 10,
        sharpness = 11,
        smite = 12,
        bane_of_arthropods = 13,
        knockback = 14,
        fire_aspect = 15,
        looting = 16,
        efficiency = 17,
        silk_touch = 18,
        unbreaking = 19,
        fortune = 20,
        power = 21,
        punch = 22,
        flame = 23,
        infinity = 24,
        luck_of_the_sea = 25,
        lure = 26,
        mending = 27,
        vanishing_curse = 28,
        impaling = 29,
        riptide = 30,
        loyalty = 31,
        channeling = 32
    };

    static constexpr i8 MAX_ENCHANTMENT_COUNT = 33;

    static EnchantmentRegistry REGISTRY;
    static lce::CONSOLE currentConsole;
    static LCEVERSION currentVersion;

    /// the order is: [console][version][pointer]
    static const std::vector<std::vector<std::vector<int>>> tableOfOrders;

    static int count;

    // attributes of each class
    const std::string name;
    const Type::Base* type = nullptr;
    EnumName enumID = EnumName::NONE;
    bool isTreasure = false;
    c_i8 maxLevel = 1;
    const Rarity* rarity = &Rarities::NONE;


    Enchantment() = default;

    Enchantment(std::string name, const Rarity* rarity, const EnumName enchantName, c_i8 maxLevel,
                c_bool isTreasure = false)
        : name(std::move(name)), type(&Type::ALL), enumID(enchantName), isTreasure(isTreasure), maxLevel(maxLevel),
          rarity(rarity){}

    Enchantment(std::string name, const Rarity* rarity, const Type::Base* type, const EnumName enchantName,
                c_i8 maxLevel, c_bool isTreasure = false)
        : name(std::move(name)), type(type), enumID(enchantName), isTreasure(isTreasure), maxLevel(maxLevel),
          rarity(rarity){}

    virtual ~Enchantment() = default;

    virtual int getMinCost(int enchantmentLevel);
    virtual int getMaxCost(int enchantmentLevel);

    ND virtual bool canApplyTogether(const Enchantment* enchantment) const;
    ND bool isCompatibleWith(const Enchantment *enchantment) const;
    ND virtual bool canApply(const lce::items::Item* item) const;


    /// registers all the enchantments
    static void registerEnchantments();

private:
    static void registerEnchantment(Enchantment* enchantment);
};
