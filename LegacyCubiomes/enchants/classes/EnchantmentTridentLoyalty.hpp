#pragma once

#include "LegacyCubiomes/enchants/enchantment.hpp"


class EnchantmentTridentLoyalty : public Enchantment {
public:
    EnchantmentTridentLoyalty(std::string name, const Rarity *rarity) :
            Enchantment(std::move(name), rarity, &Type::TRIDENT,
                        EnumName::TRIDENT_LOYALTY, 3) {};

    int getMinCost(int enchantmentLevel) override {
        return enchantmentLevel * 7 + 5;
    }

    int getMaxCost(int enchantmentLevel) override {
        return 50;
    }

    // checkCompatability
    ND bool canApplyTogether(const Enchantment *enchantment) const override {
        return enchantment->enumID != EnumName::TRIDENT_RIPTIDE;
    }
};

