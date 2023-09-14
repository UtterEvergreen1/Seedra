#pragma once

#include "LegacyCubiomes/enchants/enchantment.hpp"


class EnchantmentKnockback : public Enchantment {
public:

    EnchantmentKnockback(std::string name, const Rarity *rarity) :
        Enchantment(std::move(name), rarity, &Type::WEAPON, EnumName::KNOCKBACK, 2) {};

    int getMinEnchantability(int enchantmentLevel) override {
        return 5 + 20 * (enchantmentLevel - 1);
    }

    int getMaxEnchantability(int enchantmentLevel) override {
        return Enchantment::getMinEnchantability(enchantmentLevel) + 50;
    }
};

