#pragma once

#include "LegacyCubiomes/enchants/enchantment.hpp"


class EnchantmentOxygen : public Enchantment {
public:
    EnchantmentOxygen(std::string name, const Rarity *rarity) :
        Enchantment(std::move(name), rarity, &Type::ARMOR_HEAD, EnumName::OXYGEN, 3) {};

    int getMinCost(int enchantmentLevel) override {
        return 10 * enchantmentLevel;
    }

    int getMaxCost(int enchantmentLevel) override {
        return this->getMinCost(enchantmentLevel) + 30;
    }
};

