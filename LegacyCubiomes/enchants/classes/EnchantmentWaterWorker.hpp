#pragma once

#include "LegacyCubiomes/enchants/enchantment.hpp"


class EnchantmentWaterWorker : public Enchantment {
public:

    EnchantmentWaterWorker(const std::string& name, const Rarity *rarity) :
        Enchantment(name, rarity, &Type::ARMOR_HEAD, EnumName::WATER_WORKER, 1) {};

    int getMinEnchantability(int enchantmentLevel) override {
        return 1;
    }

    int getMaxEnchantability(int enchantmentLevel) override {
        return getMinEnchantability(enchantmentLevel) + 40;
    }
};

