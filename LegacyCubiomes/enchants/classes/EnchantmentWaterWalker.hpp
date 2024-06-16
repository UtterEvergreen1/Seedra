#pragma once

#include "LegacyCubiomes/enchants/enchantment.hpp"


class EnchantmentWaterWalker final : public Enchantment {
    static constexpr inline int8_t LEVELS_MIN[3] = {10, 20, 30};
    static constexpr inline int8_t LEVELS_MAX[3] = {26, 36, 46};

public:
    EnchantmentWaterWalker(std::string name, const Rarity* rarity)
        : Enchantment(std::move(name), rarity, &Type::ARMOR_FEET, EnumName::WATER_WALKER, 3){};

    int getMinCost(const int enchantmentLevel) override { return LEVELS_MIN[enchantmentLevel - 1]; }

    int getMaxCost(const int enchantmentLevel) override { return LEVELS_MAX[enchantmentLevel - 1]; }

    ND bool canApplyTogether(const Enchantment* enchantment) const override {
        return Enchantment::canApplyTogether(enchantment) && enchantment->enumID != EnumName::FROST_WALKER;
    }
};
