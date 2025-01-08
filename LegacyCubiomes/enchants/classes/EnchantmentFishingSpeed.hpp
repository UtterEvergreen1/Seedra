#pragma once

#include "LegacyCubiomes/enchants/enchantment.hpp"


class EnchantmentFishingSpeed final : public Enchantment {
    static constexpr i8 LEVELS_MIN[3] = {15, 24, 33};
    static constexpr i8 LEVELS_MAX[3] = {61, 71, 81};

public:
    EnchantmentFishingSpeed(std::string name, const eRarity rarity)
        : Enchantment(std::move(name), rarity, &Type::FISHING_ROD, EnumName::FISHING_SPEED, 3){};

    int getMinCost(c_int enchantmentLevel) override { return LEVELS_MIN[enchantmentLevel - 1]; }

    int getMaxCost(c_int enchantmentLevel) override { return LEVELS_MAX[enchantmentLevel - 1]; }
};
