#pragma once

#include "LegacyCubiomes/enchants/enchantment.hpp"


class EnchantmentMending : public Enchantment {
public:

    EnchantmentMending(std::string name, const Rarity *rarity) :
        Enchantment(std::move(name), rarity, &Type::BREAKABLE, EnumName::MENDING, 1, true) {};

    int getMinCost(int enchantmentLevel) override {
        return enchantmentLevel * 25;
    }

    int getMaxCost(int enchantmentLevel) override {
        return this->getMinCost(enchantmentLevel) + 50;
    }

};

