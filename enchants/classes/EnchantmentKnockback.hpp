#pragma once

#include "enchants/enchantment.hpp"


class EnchantmentKnockback final : public Enchantment {
    static constexpr i8 LEVELS_MIN[2] = {5, 25};
    static constexpr i8 LEVELS_MAX[2] = {61, 71};

public:
    EnchantmentKnockback(std::string name, const eRarity rarity)
        : Enchantment(std::move(name), rarity, &Type::WEAPON, EnumName::KNOCKBACK, 2){};

    int getMinCost(c_int enchantmentLevel) override { return LEVELS_MIN[enchantmentLevel - 1]; }

    int getMaxCost(c_int enchantmentLevel) override { return LEVELS_MAX[enchantmentLevel - 1]; }
};
