#pragma once

#include "LegacyCubiomes/enchants/enchantment.hpp"


class EnchantmentArrowFire : public Enchantment {
public:

    EnchantmentArrowFire(std::string name, const Rarity *rarity) :
        Enchantment(std::move(name), rarity, &Type::BOW, EnumName::ARROW_FIRE, 1) {};

    int getMinCost(int enchantmentLevel) override {
        return 20;
    }

    int getMaxCost(int enchantmentLevel) override {
        return 50;
    }
};

