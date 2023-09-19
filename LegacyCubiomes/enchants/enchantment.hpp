#pragma once

#include <string>
#include <utility>
#include <map>
#include <iostream>
#include <unordered_map>
#include <vector>

#include "rarity.hpp"
#include "LegacyCubiomes/mc/items.hpp"
#include "LegacyCubiomes/mc/itemID.hpp"


// #include "LegacyCubiomes/utils/weightedRandom.hpp"


class EnchantmentArrowDamage;
class EnchantmentArrowFire;
class EnchantmentArrowInfinite;
class EnchantmentArrowKnockback;
class EnchantmentBindingCurse;
class EnchantmentDamage;
class EnchantmentDigging;
class EnchantmentDurability;
class EnchantmentFireAspect;
class EnchantmentFishingSpeed;
class EnchantmentFrostWalker;
class EnchantmentKnockback;
class EnchantmentLootBonus;
class EnchantmentMending;
class EnchantmentOxygen;
class EnchantmentProtection;
class EnchantmentThorns;
class EnchantmentUntouching;
class EnchantmentVanishingCurse;
class EnchantmentWaterWalker;
class EnchantmentWaterWorker;


class Enchantment {
public:
    static std::map<int, Enchantment*> REGISTRY;

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
        NONE
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

    ~Enchantment();

    virtual int getMinCost(int enchantmentLevel);
    virtual int getMaxCost(int enchantmentLevel);

    ND virtual bool canApplyTogether(const Enchantment *enchantment) const;
    ND virtual bool canApply(const Items::Item *item) const;

    static void registerEnchantments();

private:
    static void registerEnchantment(int id, Enchantment* enchantment);
    static void initializeTypeIterable();

};










