#pragma once

#include "enchants/enchantment.hpp"


class EnchantmentUntouching final : public Enchantment {
public:
    EnchantmentUntouching(std::string name, const eRarity rarity)
        : Enchantment(std::move(name), rarity, &Type::DIGGER, EnumName::UNTOUCHING, 1){};

    int getMinCost(MU c_int enchantmentLevel) override { return 15; }

    int getMaxCost(MU c_int enchantmentLevel) override { return 61; }

    ND bool canApplyTogether(const Enchantment* enchantment) const override {
        return Enchantment::canApplyTogether(enchantment) && enchantment->name != "Fortune";
    }
};
