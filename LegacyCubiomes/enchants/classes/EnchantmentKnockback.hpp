#pragma once

#include "LegacyCubiomes/enchants/enchantment.hpp"


class EnchantmentKnockback : public Enchantment {
private:
    static constexpr inline int8_t LEVELS_MIN[2] = {5, 25};
    static constexpr inline int8_t LEVELS_MAX[2] = {61, 71};
public:

    EnchantmentKnockback(std::string name, const Rarity *rarity) :
        Enchantment(std::move(name), rarity, &Type::WEAPON, EnumName::KNOCKBACK, 2) {};

    int getMinCost(int enchantmentLevel) override {
        return LEVELS_MIN[enchantmentLevel - 1];
    }

    int getMaxCost(int enchantmentLevel) override {
        return LEVELS_MAX[enchantmentLevel - 1];
    }
};

