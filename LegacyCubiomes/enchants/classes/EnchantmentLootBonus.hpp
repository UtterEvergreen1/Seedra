#pragma once

#include "LegacyCubiomes/enchants/enchantment.hpp"


class EnchantmentLootBonus : public Enchantment {
    static constexpr inline i8 LEVELS_MIN[3] = {15, 24, 33};
    static constexpr inline i8 LEVELS_MAX[3] = {61, 71, 81};

public:
    EnchantmentLootBonus(const std::string& name, const eRarity rarity, const Type::Base* type)
        : Enchantment(name, rarity, type, EnumName::LOOT_BONUS, 3){};

    int getMinCost(c_int enchantmentLevel) override { return LEVELS_MIN[enchantmentLevel - 1]; }

    int getMaxCost(c_int enchantmentLevel) override { return LEVELS_MAX[enchantmentLevel - 1]; }

    ND bool canApplyTogether(const Enchantment* enchantment) const override {
        return Enchantment::canApplyTogether(enchantment) && enchantment->enumID != EnumName::UNTOUCHING;
    }
};
