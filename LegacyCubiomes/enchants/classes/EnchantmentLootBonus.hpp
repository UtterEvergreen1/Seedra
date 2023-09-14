#pragma once

#include "LegacyCubiomes/enchants/enchantment.hpp"


class EnchantmentLootBonus : public Enchantment {
public:

    EnchantmentLootBonus(const std::string& name, const Rarity *rarity, const Type::Base* type) :
        Enchantment(name, rarity, type, EnumName::LOOT_BONUS, 3) {};

    int getMinEnchantability(int enchantmentLevel) override {
        return 15 + (enchantmentLevel - 1) * 9;
    }

    int getMaxEnchantability(int enchantmentLevel) override {
        return Enchantment::getMinEnchantability(enchantmentLevel) + 50;
    }

    ND bool canApplyTogether(const Enchantment *enchantment) const override {
        return Enchantment::canApplyTogether(enchantment)
        && enchantment->enumID != EnumName::UNTOUCHING;
    }
};














