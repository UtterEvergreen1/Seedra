#pragma once

#include "enchants/enchantment.hpp"


class EnchantmentProtection final : public Enchantment {
public:
    struct ArmorType {
        const std::string typeName;
        c_i8 id{};
        c_i8 minEnchantability{};
        c_i8 levelCost{};

        ArmorType() = default;
        ArmorType(std::string typeName, c_i8 id, c_i8 minEnchantability, c_i8 levelCost)
            : typeName(std::move(typeName)), id(id), minEnchantability(minEnchantability), levelCost(levelCost){}

        ND i8 getEnchantIncreasePerLevel() const { return this->levelCost; }
        ND i8 getMinimalEnchantability() const { return this->minEnchantability; }
    };

    static const ArmorType ALL;
    static const ArmorType FIRE;
    static const ArmorType FALL;
    static const ArmorType EXPLOSION;
    static const ArmorType PROJECTILE;

    const ArmorType* protType = &ALL;

    EnchantmentProtection(std::string name, const eRarity rarity, c_int type)
        : Enchantment(std::move(name), rarity, EnumName::PROTECTION, 4) {
        switch (type) {
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

    int getMinCost(c_int enchantmentLevel) override {
        return protType->getMinimalEnchantability() + (enchantmentLevel - 1) * protType->getEnchantIncreasePerLevel();
    }

    int getMaxCost(c_int enchantmentLevel) override {
        return getMinCost(enchantmentLevel) + protType->getEnchantIncreasePerLevel();
    }

    ND bool canApplyTogether(const Enchantment* enchantment) const override {
        if (enchantment->enumID == EnumName::PROTECTION) {
            c_auto enchantmentIn = dynamic_cast<const EnchantmentProtection*>(enchantment);

            if (protType == enchantmentIn->protType) { return false; }
            return protType == &EnchantmentProtection::FALL || enchantmentIn->protType == &EnchantmentProtection::FALL;
        }
        return Enchantment::canApplyTogether(enchantment);
    }
};
