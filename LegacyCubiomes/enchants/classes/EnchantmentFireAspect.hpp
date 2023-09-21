#pragma once

#include "LegacyCubiomes/enchants/enchantment.hpp"


class EnchantmentFireAspect : public Enchantment {
private:
    static constexpr inline int LEVELS_MIN[2] = {};
    static constexpr inline int LEVELS_MAX[2] = {};
public:

    EnchantmentFireAspect(std::string name, const Rarity *rarity) :
        Enchantment(std::move(name), rarity, &Type::WEAPON, EnumName::FIRE_ASPECT, 2) {
    };

    int getMinCost(int enchantmentLevel) override {
        return enchantmentLevel * 20 - 10;
    }

    int getMaxCost(int enchantmentLevel) override {
        return enchantmentLevel * 10 + 51;
    }
};

