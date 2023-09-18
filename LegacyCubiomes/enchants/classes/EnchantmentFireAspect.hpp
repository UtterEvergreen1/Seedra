#pragma once

#include "LegacyCubiomes/enchants/enchantment.hpp"


class EnchantmentFireAspect : public Enchantment {
public:

    EnchantmentFireAspect(std::string name, const Rarity *rarity) :
        Enchantment(std::move(name), rarity, &Type::WEAPON, EnumName::FIRE_ASPECT, 2) {
    };

    int getMinEnchantability(int enchantmentLevel) override {
        return enchantmentLevel * 20 - 10;
    }

    int getMaxEnchantability(int enchantmentLevel) override {
        return Enchantment::getMinEnchantability(enchantmentLevel) + 50;
    }
};

