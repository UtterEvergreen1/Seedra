#pragma once

#include "LegacyCubiomes/enchants/enchantment.hpp"


class EnchantmentFireAspect : public Enchantment {
public:

    EnchantmentFireAspect(std::string name, const Rarity *rarity) :
        Enchantment(std::move(name), rarity, &Type::WEAPON, EnumName::FIRE_ASPECT, 2) {
    };

    int getMinCost(int enchantmentLevel) override {
        return enchantmentLevel * 20 - 10;
    }

    int getMaxCost(int enchantmentLevel) override {
        return Enchantment::getMinCost(enchantmentLevel) + 50;
    }
};

