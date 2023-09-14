#pragma once

#include "LegacyCubiomes/enchants/enchantment.hpp"


class EnchantmentOxygen : public Enchantment {
public:
    EnchantmentOxygen(std::string name, const Rarity *rarity) :
        Enchantment(std::move(name), rarity, &Type::ARMOR_HEAD, EnumName::OXYGEN, 3) {};

    int getMinEnchantability(int enchantmentLevel) override {
        return 10 * enchantmentLevel;
    }

    int getMaxEnchantability(int enchantmentLevel) override {
        return this->getMinEnchantability(enchantmentLevel) + 30;
    }
};

