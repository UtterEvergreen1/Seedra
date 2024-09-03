#pragma once

#include "LegacyCubiomes/enchants/enchantment.hpp"


class EnchantmentFrostWalker : public Enchantment {
    static constexpr inline i8 LEVELS_MIN[2] = {10, 20};
    static constexpr inline i8 LEVELS_MAX[2] = {25, 35};

public:
    EnchantmentFrostWalker(std::string name, const eRarity rarity)
        : Enchantment(std::move(name), rarity, &Type::ARMOR_FEET, EnumName::FROST_WALKER, 2, true){};

    int getMinCost(c_int enchantmentLevel) override { return LEVELS_MIN[enchantmentLevel - 1]; }

    int getMaxCost(c_int enchantmentLevel) override { return LEVELS_MAX[enchantmentLevel - 1]; }

    ND bool canApplyTogether(const Enchantment* enchantment) const override {
        return Enchantment::canApplyTogether(enchantment) && enchantment->enumID != EnumName::WATER_WALKER;
    }
};
