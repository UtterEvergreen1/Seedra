#pragma once

#include "enchants/enchantment.hpp"


class EnchantmentArrowInfinite final : public Enchantment {
public:
    EnchantmentArrowInfinite(std::string name, const eRarity rarity)
        : Enchantment(std::move(name), rarity, &Type::BOW, EnumName::ARROW_INFINITE, 1){};

    int getMinCost(c_int enchantmentLevel) override { return 20; }

    int getMaxCost(c_int enchantmentLevel) override { return 50; }

    ND bool canApplyTogether(const Enchantment* enchantment) const override {
        return enchantment->enumID != EnumName::MENDING
               && Enchantment::canApplyTogether(enchantment);
    }
};
