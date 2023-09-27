#pragma once

#include <string>
#include <utility>
#include <iostream>
#include <vector>

#include "rarity.hpp"
#include "LegacyCubiomes/mc/items.hpp"
#include "LegacyCubiomes/mc/itemID.hpp"
#include "LegacyCubiomes/utils/RegistryNamespaced.hpp"
#include "LegacyCubiomes/cubiomes/generator.hpp"

class EnchantmentHelper;

class Enchantment {
private:
    static bool isSetup;
public:
    friend class EnchantmentHelper;
    static RegistryNamespaced<Enchantment> REGISTRY;
    static CONSOLE currentConsole;
    static LCEVERSION currentVersion;

    /// the order is: [console][version][pointer]
    static const std::vector<std::vector<std::vector<int>>> tableOfOrders;

    static constexpr int MAX_ENCHANTMENT_COUNT = 33;
    static int count;

    class Type {
    public:
        class Base { public: virtual bool canEnchantItem(const Items::Item *itemIn) const;};
        class All        : public Base {public: bool canEnchantItem(const Items::Item *itemIn) const override;};
        class Armor      : public Base {public: bool canEnchantItem(const Items::Item *itemIn) const override;};
        class ArmorFeet  : public Base {public: bool canEnchantItem(const Items::Item *itemIn) const override;};
        class ArmorLegs  : public Base {public: bool canEnchantItem(const Items::Item *itemIn) const override;};
        class ArmorChest : public Base {public: bool canEnchantItem(const Items::Item *itemIn) const override;};
        class ArmorHead  : public Base {public: bool canEnchantItem(const Items::Item *itemIn) const override;};
        class Weapon     : public Base {public: bool canEnchantItem(const Items::Item *itemIn) const override;};
        class Digger     : public Base {public: bool canEnchantItem(const Items::Item *itemIn) const override;};
        class FishingRod : public Base {public: bool canEnchantItem(const Items::Item *itemIn) const override;};
        class Breakable  : public Base {public: bool canEnchantItem(const Items::Item *itemIn) const override;};
        class Bow        : public Base {public: bool canEnchantItem(const Items::Item *itemIn) const override;};
        class Wearable   : public Base {public: bool canEnchantItem(const Items::Item *itemIn) const override;};
        class Trident    : public Base {public: bool canEnchantItem(const Items::Item *itemIn) const override;};

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

    enum EnumName {
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

    enum EnchantmentOrder {
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


    // attributes of each class
    const std::string name;
    const Rarity* const rarity = &Rarities::NONE;
    const Type::Base* type = nullptr;
    EnumName enumID = EnumName::NONE;
    bool isTreasure = false;
    const int minLevel = 1;
    const int maxLevel = 1;


    Enchantment() = default;

    Enchantment(std::string name, const Rarity *rarity, EnumName enchantName, int maxLevel, bool isTreasure = false) :
            name(std::move(name)), rarity(rarity), type(&Type::ALL), enumID(enchantName), maxLevel(maxLevel), isTreasure(isTreasure) {};

    Enchantment(std::string name, const Rarity *rarity, const Type::Base* type, EnumName enchantName, int maxLevel, bool isTreasure = false) :
            name(std::move(name)), rarity(rarity), type(type), enumID(enchantName), maxLevel(maxLevel), isTreasure(isTreasure) {};

    virtual int getMinCost(int enchantmentLevel);
    virtual int getMaxCost(int enchantmentLevel);

    ND virtual bool canApplyTogether(const Enchantment *enchantment) const;
    ND virtual bool canApply(const Items::Item *item) const;


    /**
     * Registers all the enchantments
     */
    static void registerEnchantments();

private:
    static void registerEnchantment(Enchantment *enchantment);
};










