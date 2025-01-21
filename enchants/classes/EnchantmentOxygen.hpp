#pragma once

#include "enchants/enchantment.hpp"


class EnchantmentOxygen final : public Enchantment {
    static constexpr i8 LEVELS_MIN[3] = {10, 20, 30};
    static constexpr i8 LEVELS_MAX[3] = {40, 50, 60};

public:
    EnchantmentOxygen(std::string name, const eRarity rarity)
        : Enchantment(std::move(name), rarity, &Type::ARMOR_HEAD, EnumName::OXYGEN, 3){};

    int getMinCost(c_int enchantmentLevel) override { return LEVELS_MIN[enchantmentLevel - 1]; }

    int getMaxCost(c_int enchantmentLevel) override { return LEVELS_MAX[enchantmentLevel - 1]; }
};
