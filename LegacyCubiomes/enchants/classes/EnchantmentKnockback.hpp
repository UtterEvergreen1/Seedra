#pragma once

#include "LegacyCubiomes/enchants/enchantment.hpp"


class EnchantmentKnockback : public Enchantment {
public:

    EnchantmentKnockback(std::string name, const Rarity *rarity) :
        Enchantment(std::move(name), rarity, &Type::WEAPON, EnumName::KNOCKBACK, 2) {};

    int getMinCost(int enchantmentLevel) override {
        return 20 * enchantmentLevel - 15;
    }

    int getMaxCost(int enchantmentLevel) override {
        return Enchantment::getMinCost(enchantmentLevel) + 50;
    }
};

