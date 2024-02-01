#pragma once

#include "LegacyCubiomes/enchants/enchantment.hpp"


class EnchantmentArrowDamage final : public Enchantment {
    static constexpr inline int8_t LEVELS_MIN[5] = {9, 19, 29, 39, 49};
    static constexpr inline int8_t LEVELS_MAX[5] = {16, 26, 36, 46, 56};

public:
    EnchantmentArrowDamage(const std::string& name, const Rarity* rarity)
        : Enchantment(name, rarity, &Type::BOW, EnumName::ARROW_DAMAGE, 5){};

    int getMinCost(const int enchantmentLevel) override { return LEVELS_MIN[enchantmentLevel - 1]; }

    int getMaxCost(const int enchantmentLevel) override { return LEVELS_MAX[enchantmentLevel - 1]; }
};
