#pragma once

#include "LegacyCubiomes/enchants/enchantment.hpp"


class EnchantmentFireAspect final : public Enchantment {
    static constexpr inline int8_t LEVELS_MIN[2] = {10, 30};
    static constexpr inline int8_t LEVELS_MAX[2] = {61, 71};

public:
    EnchantmentFireAspect(std::string name, const Rarity* rarity)
        : Enchantment(std::move(name), rarity, &Type::WEAPON, EnumName::FIRE_ASPECT, 2){};

    int getMinCost(const int enchantmentLevel) override { return LEVELS_MIN[enchantmentLevel - 1]; }

    int getMaxCost(const int enchantmentLevel) override { return LEVELS_MAX[enchantmentLevel - 1]; }
};
