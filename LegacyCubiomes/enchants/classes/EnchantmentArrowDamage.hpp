#pragma once

#include "LegacyCubiomes/enchants/enchantment.hpp"


class EnchantmentArrowDamage final : public Enchantment {
    static constexpr inline i8 LEVELS_MIN[5] = {1, 11, 21, 31, 41};
    static constexpr inline i8 LEVELS_MAX[5] = {16, 26, 36, 46, 56};

public:
    EnchantmentArrowDamage(const std::string& name, const eRarity rarity)
        : Enchantment(name, rarity, &Type::BOW, EnumName::ARROW_DAMAGE, 5){};

    int getMinCost(c_int enchantmentLevel) override { return LEVELS_MIN[enchantmentLevel - 1]; }

    int getMaxCost(c_int enchantmentLevel) override { return LEVELS_MAX[enchantmentLevel - 1]; }
};
