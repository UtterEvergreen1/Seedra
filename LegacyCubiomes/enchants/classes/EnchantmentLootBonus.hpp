#pragma once

#include "LegacyCubiomes/enchants/enchantment.hpp"


class EnchantmentLootBonus : public Enchantment {
private:
    static constexpr inline int8_t LEVELS_MIN[3] = {15, 24, 33};
    static constexpr inline int8_t LEVELS_MAX[3] = {61, 71, 81};
public:

    EnchantmentLootBonus(const std::string& name, const Rarity *rarity, const Type::Base* type) :
        Enchantment(name, rarity, type, EnumName::LOOT_BONUS, 3) {};

    int getMinCost(int enchantmentLevel) override {
        return LEVELS_MIN[enchantmentLevel - 1];
    }

    int getMaxCost(int enchantmentLevel) override {
        return LEVELS_MAX[enchantmentLevel - 1];
    }

    ND bool canApplyTogether(const Enchantment *enchantment) const override {
        return Enchantment::canApplyTogether(enchantment)
        && enchantment->enumID != EnumName::UNTOUCHING;
    }
};














