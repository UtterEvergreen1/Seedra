#pragma once

#include "LegacyCubiomes/enchants/enchantment.hpp"


class EnchantmentFishingSpeed : public Enchantment {
private:
    static constexpr inline int8_t LEVELS_MIN[3] = {};
    static constexpr inline int8_t LEVELS_MAX[3] = {};

public:
    EnchantmentFishingSpeed(std::string name, const Rarity* rarity)
        : Enchantment(std::move(name), rarity, &Type::FISHING_ROD, EnumName::FISHING_SPEED, 3){};

    int getMinCost(int enchantmentLevel) override { return enchantmentLevel * 9 + 6; }

    int getMaxCost(int enchantmentLevel) override { return enchantmentLevel * 10 + 51; }
};
