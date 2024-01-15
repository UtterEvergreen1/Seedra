#pragma once

#include "LegacyCubiomes/enchants/enchantment.hpp"


class EnchantmentOxygen final : public Enchantment {
    static constexpr inline int8_t LEVELS_MIN[3] = {10, 20, 30};
    static constexpr inline int8_t LEVELS_MAX[3] = {40, 50, 60};

public:
    EnchantmentOxygen(std::string name, const Rarity* rarity)
        : Enchantment(std::move(name), rarity, &Type::ARMOR_HEAD, EnumName::OXYGEN, 3){};

    int getMinCost(const int enchantmentLevel) override { return LEVELS_MIN[enchantmentLevel - 1]; }

    int getMaxCost(const int enchantmentLevel) override { return LEVELS_MAX[enchantmentLevel - 1]; }
};
