#pragma once

#include "LegacyCubiomes/enchants/enchantment.hpp"


class EnchantmentUntouching final : public Enchantment {
public:
    EnchantmentUntouching(std::string name, const Rarity* rarity)
        : Enchantment(std::move(name), rarity, &Type::DIGGER, EnumName::UNTOUCHING, 1){};

    int getMinCost(c_int enchantmentLevel) override { return 15; }

    int getMaxCost(c_int enchantmentLevel) override { return 61; }

    ND bool canApplyTogether(const Enchantment* enchantment) const override {
        return Enchantment::canApplyTogether(enchantment) && enchantment->name != "Fortune";
    }
};
