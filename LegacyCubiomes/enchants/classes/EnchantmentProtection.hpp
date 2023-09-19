#pragma once

#include "LegacyCubiomes/enchants/enchantment.hpp"

class EnchantmentProtection : public Enchantment {
public:

    struct ArmorType {
        const std::string typeName;
        const int id{};
        const int minEnchantability{};
        const int levelCost{};
        const int levelCostSpan{};

        ArmorType() = default;
        ArmorType(std::string typeName, int id, int minEnchantability, int levelCost, int levelCostSpan) :
        typeName(std::move(typeName)), id(id), minEnchantability(minEnchantability), levelCost(levelCost), levelCostSpan(levelCostSpan) {}

        ND int getEnchantIncreasePerLevel() const {return this->levelCost;}
        ND int getMinimalEnchantability() const {return this->minEnchantability;}
    };

    static const ArmorType ALL;
    static const ArmorType FIRE;
    static const ArmorType FALL;
    static const ArmorType EXPLOSION;
    static const ArmorType PROJECTILE;

    const EnchantmentProtection::ArmorType* protType = &ALL;

    EnchantmentProtection(std::string name, const Rarity *rarity, int type) :
            Enchantment(std::move(name), rarity, EnumName::PROTECTION, 4) {
        switch(type) {
            case 0:
                protType = &ALL;
                break;
            case 1:
                protType = &FIRE;
                break;
            case 2:
                protType = &FALL;
                break;
            case 3:
                protType = &EXPLOSION;
                break;
            case 4:
                protType = &PROJECTILE;
                break;
            default:
                protType = &ALL;
                break;
        }

        if (protType == &FALL) {
            this->type = &Type::ARMOR_FEET;
        } else {
            this->type = &Type::ARMOR;
        }


    };

    int getMinCost(int enchantmentLevel) override {
        return protType->getMinimalEnchantability() +
               (enchantmentLevel - 1) * protType->getEnchantIncreasePerLevel();
    }

    int getMaxCost(int enchantmentLevel) override {
        return getMinCost(enchantmentLevel) +
               protType->getEnchantIncreasePerLevel();
    }

    ND bool canApplyTogether(const Enchantment *enchantment) const override {
        if (enchantment->enumID == EnumName::PROTECTION) {
            auto enchantmentIn = dynamic_cast<const EnchantmentProtection*>(enchantment);

            if (protType == enchantmentIn->protType) {
                return false;
            } else {
                return protType == &EnchantmentProtection::FALL ||
                       enchantmentIn->protType == &EnchantmentProtection::FALL;
            }
        } else {
            return Enchantment::canApplyTogether(enchantment);
        }
    }
};
