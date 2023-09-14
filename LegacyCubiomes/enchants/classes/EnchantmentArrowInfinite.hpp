#pragma once

#include "LegacyCubiomes/enchants/enchantment.hpp"

class EnchantmentArrowInfinite : public Enchantment {
public:

    EnchantmentArrowInfinite(std::string name, const Rarity *rarity) :
        Enchantment(std::move(name), rarity, &Type::BOW, EnumName::ARROW_INFINITE, 1) {};

    int getMinEnchantability(int enchantmentLevel) override {
        return 20;
    }

    int getMaxEnchantability(int enchantmentLevel) override {
        return 50;
    }

    ND bool canApplyTogether(const Enchantment *enchantment) const override {
        return enchantment->enumID != EnumName::ARROW_INFINITE
        && Enchantment::canApplyTogether(enchantment);
    }

};

