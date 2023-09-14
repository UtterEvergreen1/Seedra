#pragma once

#include "LegacyCubiomes/enchants/enchantment.hpp"


class EnchantmentWaterWalker : public Enchantment {
public:

    EnchantmentWaterWalker(std::string name, const Rarity *rarity) :
        Enchantment(std::move(name), rarity, &Type::ARMOR_FEET, EnumName::WATER_WALKER, 3) {};

    int getMinEnchantability(int enchantmentLevel) override {
        return enchantmentLevel * 10;
    }

    int getMaxEnchantability(int enchantmentLevel) override {
        return getMinEnchantability(enchantmentLevel) + 15;
    }

    ND bool canApplyTogether(const Enchantment *enchantment) const override {
        return Enchantment::canApplyTogether(enchantment)
        && enchantment->enumID != EnumName::FROST_WALKER;
    }
};

