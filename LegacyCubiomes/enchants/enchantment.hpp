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
class EnchantmentTridentChanneling;
class EnchantmentTridentImpaler;
class EnchantmentTridentLoyalty;
class EnchantmentTridentRiptide;

template<bool> class EnchantmentHelper;

class Enchantment {
private:
    static inline bool isSetup = false;
public:
    friend class EnchantmentHelper<true>;
    friend class EnchantmentHelper<false>;

    static constexpr int MAX_ENCHANTMENT_COUNT = 33;

    static inline std::map<int, Enchantment*> REGISTRY = {};
    static inline int count = 0;

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

        static inline const All ALL;
        static inline const Armor ARMOR;
        static inline const ArmorFeet ARMOR_FEET;
        static inline const ArmorLegs ARMOR_LEGS;
        static inline const ArmorChest ARMOR_CHEST;
        static inline const ArmorHead ARMOR_HEAD;
        static inline const Weapon WEAPON;
        static inline const Digger DIGGER;
        static inline const FishingRod FISHING_ROD;
        static inline const Breakable BREAKABLE;
        static inline const Bow BOW;
        static inline const Wearable WEARABLE;
        static inline const Trident TRIDENT;

        static inline std::vector<const Base*> ALL_ITERABLE = {
                &Type::ALL,
                &Type::ARMOR,
                &Type::ARMOR_FEET,
                &Type::ARMOR_LEGS,
                &Type::ARMOR_CHEST,
                &Type::ARMOR_HEAD,
                &Type::WEAPON,
                &Type::DIGGER,
                &Type::FISHING_ROD,
                &Type::BREAKABLE,
                &Type::BOW,
                &Type::WEARABLE,
                &Type::TRIDENT
        };
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

    ~Enchantment() {
        for (auto &pair : REGISTRY) {
            delete pair.second;
        }
        REGISTRY.clear();
    }

    virtual int getMinCost(int enchantmentLevel);
    virtual int getMaxCost(int enchantmentLevel);

    ND virtual bool canApplyTogether(const Enchantment *enchantment) const;
    ND virtual bool canApply(const Items::Item *item) const;

    template<bool isAquatic>
    static void registerEnchantments();

private:
    static void registerEnch(int *id, Enchantment* enchantment);

};










