#pragma once

#include "LegacyCubiomes/enchants/enchantment.hpp"


class EnchantmentFishingSpeed : public Enchantment {
public:

    EnchantmentFishingSpeed(std::string name, const Rarity *rarity) :
        Enchantment(std::move(name), rarity, &Type::FISHING_ROD, EnumName::FISHING_SPEED, 3) {};

    int getMinEnchantability(int enchantmentLevel) override {
        return enchantmentLevel * 9 + 6;
    }

    int getMaxEnchantability(int enchantmentLevel) override {
        return Enchantment::getMinEnchantability(enchantmentLevel) + 50;
    }
};

