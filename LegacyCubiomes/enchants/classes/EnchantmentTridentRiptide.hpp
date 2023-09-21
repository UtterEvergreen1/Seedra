#pragma once

#include "LegacyCubiomes/enchants/enchantment.hpp"


class EnchantmentTridentRiptide : public Enchantment {
public:
    EnchantmentTridentRiptide(std::string name, const Rarity *rarity) :
            Enchantment(std::move(name), rarity, &Type::TRIDENT,
                        EnumName::TRIDENT_RIPTIDE, 3) {};

    int getMinCost(int enchantmentLevel) override {
        return enchantmentLevel * 7 + 10;
    }

    int getMaxCost(int enchantmentLevel) override {
        return 50;
    }

    ND bool canApplyTogether(const Enchantment *enchantment) const override {
        return enchantment->enumID != EnumName::TRIDENT_RIPTIDE;
    }
};

