#pragma once

#include "LegacyCubiomes/enchants/enchantment.hpp"


class EnchantmentArrowDamage : public Enchantment {
public:

    EnchantmentArrowDamage(const std::string& name, const Rarity *rarity) :
        Enchantment(name, rarity, &Type::BOW, EnumName::ARROW_DAMAGE, 5) {
    };

    int getMinCost(int enchantmentLevel) override {
        return enchantmentLevel * 10 - 9;
    }

    int getMaxCost(int enchantmentLevel) override {
        return this->getMinCost(enchantmentLevel) + 15;
    }
};

