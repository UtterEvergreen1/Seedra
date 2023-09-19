#pragma once

#include "LegacyCubiomes/enchants/enchantment.hpp"


class EnchantmentUntouching : public Enchantment {
public:

    EnchantmentUntouching(std::string name, const Rarity *rarity) :
            Enchantment(std::move(name), rarity, &Type::DIGGER, EnumName::UNTOUCHING, 1) {};

    int getMinCost(int enchantmentLevel) override {
        return 15;
    }

    int getMaxCost(int enchantmentLevel) override {
        return Enchantment::getMinCost(enchantmentLevel) + 50;
    }

    ND bool canApplyTogether(const Enchantment *enchantment) const override {
        return Enchantment::canApplyTogether(enchantment) && enchantment->name != "Fortune";
    }
};

