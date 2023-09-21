#pragma once

#include "LegacyCubiomes/enchants/enchantment.hpp"


class EnchantmentTridentChanneling : public Enchantment {
public:
    EnchantmentTridentChanneling(std::string name, const Rarity *rarity) :
            Enchantment(std::move(name), rarity, &Type::TRIDENT,
                        EnumName::TRIDENT_CHANNELING, 1) {};

    int getMinCost(int enchantmentLevel) override {
        return 25;
    }

    int getMaxCost(int enchantmentLevel) override {
        return 50;
    }

    // checkCompatability
    ND bool canApplyTogether(const Enchantment *enchantment) const override {
        return enchantment->enumID != EnumName::TRIDENT_RIPTIDE;
    }

};

