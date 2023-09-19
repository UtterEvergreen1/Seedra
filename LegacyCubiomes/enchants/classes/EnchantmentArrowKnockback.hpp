#pragma once

#include "LegacyCubiomes/enchants/enchantment.hpp"


class EnchantmentArrowKnockback : public Enchantment {
public:

    EnchantmentArrowKnockback(std::string name, const Rarity *rarity) :
        Enchantment(std::move(name), rarity, &Type::BOW, EnumName::ARROW_KNOCKBACK, 2) {};

    int getMinCost(int enchantmentLevel) override {
        return enchantmentLevel * 20 - 8;
    }

    int getMaxCost(int enchantmentLevel) override {
        return this->getMinCost(enchantmentLevel) + 25;
    }
};

