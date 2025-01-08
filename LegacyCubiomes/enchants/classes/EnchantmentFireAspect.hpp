#pragma once

#include "LegacyCubiomes/enchants/enchantment.hpp"


class EnchantmentFireAspect final : public Enchantment {
    static constexpr i8 LEVELS_MIN[2] = {10, 30};
    static constexpr i8 LEVELS_MAX[2] = {61, 71};

public:
    EnchantmentFireAspect(std::string name, const eRarity rarity)
        : Enchantment(std::move(name), rarity, &Type::WEAPON, EnumName::FIRE_ASPECT, 2){};

    int getMinCost(c_int enchantmentLevel) override { return LEVELS_MIN[enchantmentLevel - 1]; }

    int getMaxCost(c_int enchantmentLevel) override { return LEVELS_MAX[enchantmentLevel - 1]; }
};
