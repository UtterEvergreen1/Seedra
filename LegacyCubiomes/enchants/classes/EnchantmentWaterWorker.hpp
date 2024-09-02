#pragma once

#include "LegacyCubiomes/enchants/enchantment.hpp"


class EnchantmentWaterWorker final : public Enchantment {
public:
    EnchantmentWaterWorker(const std::string& name, const eRarity rarity)
        : Enchantment(name, rarity, &Type::ARMOR_HEAD, EnumName::WATER_WORKER, 1){};

    int getMinCost(c_int enchantmentLevel) override { return 1; }

    int getMaxCost(c_int enchantmentLevel) override { return 41; }
};
