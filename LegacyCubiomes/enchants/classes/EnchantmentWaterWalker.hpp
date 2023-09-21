#pragma once

#include "LegacyCubiomes/enchants/enchantment.hpp"


class EnchantmentWaterWalker : public Enchantment {
private:
    static constexpr inline int LEVELS_MIN[3] = {10, 20, 30};
    static constexpr inline int LEVELS_MAX[3] = {25, 35, 45};
public:

    EnchantmentWaterWalker(std::string name, const Rarity *rarity) :
        Enchantment(std::move(name), rarity, &Type::ARMOR_FEET, EnumName::WATER_WALKER, 3) {};

    int getMinCost(int enchantmentLevel) override {
        return LEVELS_MIN[enchantmentLevel];
    }

    int getMaxCost(int enchantmentLevel) override {
        return LEVELS_MAX[enchantmentLevel];
    }

    ND bool canApplyTogether(const Enchantment *enchantment) const override {
        return Enchantment::canApplyTogether(enchantment)
        && enchantment->enumID != EnumName::FROST_WALKER;
    }
};

