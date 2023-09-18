#pragma once

#include "LegacyCubiomes/enchants/enchantment.hpp"


class EnchantmentArrowDamage : public Enchantment {
public:

    EnchantmentArrowDamage(const std::string& name, const Rarity *rarity) :
        Enchantment(name, rarity, &Type::BOW, EnumName::ARROW_DAMAGE, 5) {
    };

    int getMinEnchantability(int enchantmentLevel) override {
        return enchantmentLevel * 10 - 9;
    }

    int getMaxEnchantability(int enchantmentLevel) override {
        return this->getMinEnchantability(enchantmentLevel) + 15;
    }
};

