#pragma once

#include "LegacyCubiomes/enchants/enchantment.hpp"


class EnchantmentArrowFire : public Enchantment {
public:

    EnchantmentArrowFire(std::string name, const Rarity *rarity) :
        Enchantment(std::move(name), rarity, &Type::BOW, EnumName::ARROW_FIRE, 1) {};

    int getMinEnchantability(int enchantmentLevel) override {
        return 20;
    }

    int getMaxEnchantability(int enchantmentLevel) override {
        return 50;
    }
};

