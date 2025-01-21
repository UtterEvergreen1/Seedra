#pragma once

#include "enchants/enchantment.hpp"


class EnchantmentArrowKnockback final : public Enchantment {
    static constexpr i8 LEVELS_MIN[2] = {12, 32};
    static constexpr i8 LEVELS_MAX[2] = {37, 57};

public:
    EnchantmentArrowKnockback(std::string name, const eRarity rarity)
        : Enchantment(std::move(name), rarity, &Type::BOW, EnumName::ARROW_KNOCKBACK, 2){};

    int getMinCost(c_int enchantmentLevel) override { return LEVELS_MIN[enchantmentLevel - 1]; }

    int getMaxCost(c_int enchantmentLevel) override { return LEVELS_MAX[enchantmentLevel - 1]; }
};
