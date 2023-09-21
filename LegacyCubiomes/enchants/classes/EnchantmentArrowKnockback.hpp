#pragma once

#include "LegacyCubiomes/enchants/enchantment.hpp"


class EnchantmentArrowKnockback : public Enchantment {
private:
    static constexpr inline int LEVELS_MIN[2] = {12, 32};
    static constexpr inline int LEVELS_MAX[2] = {37, 57};

public:

    EnchantmentArrowKnockback(std::string name, const Rarity *rarity) :
        Enchantment(std::move(name), rarity, &Type::BOW, EnumName::ARROW_KNOCKBACK, 2) {};

    int getMinCost(int enchantmentLevel) override {
        return LEVELS_MIN[enchantmentLevel];
    }

    int getMaxCost(int enchantmentLevel) override {
        return LEVELS_MAX[enchantmentLevel];
    }
};

