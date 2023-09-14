#pragma once

#include "LegacyCubiomes/enchants/enchantment.hpp"


class EnchantmentFrostWalker : public Enchantment {
public:

    EnchantmentFrostWalker(std::string name, const Rarity *rarity) :
        Enchantment(std::move(name), rarity, &Type::ARMOR_FEET, FROST_WALKER, 2, true) {};

    int getMinEnchantability(int enchantmentLevel) override {
        return enchantmentLevel * 10;
    }

    int getMaxEnchantability(int enchantmentLevel) override {
        return this->getMinEnchantability(enchantmentLevel) + 15;
    }

    ND bool canApplyTogether(const Enchantment *enchantment) const override {
        return Enchantment::canApplyTogether(enchantment) && enchantment->enumID != EnumName::WATER_WALKER;
    }
};

