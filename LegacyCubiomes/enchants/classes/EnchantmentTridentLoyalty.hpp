#pragma once

#include "LegacyCubiomes/enchants/enchantment.hpp"


class EnchantmentTridentLoyalty final : public Enchantment {
    static constexpr inline int8_t LEVELS_MIN[3] = {7, 14, 21};
public:
    EnchantmentTridentLoyalty(std::string name, const Rarity *rarity) :
            Enchantment(std::move(name), rarity, &Type::TRIDENT,
                        EnumName::TRIDENT_LOYALTY, 3) {};

    int getMinCost(const int enchantmentLevel) override {
        return LEVELS_MIN[enchantmentLevel];
    }

    int getMaxCost(int enchantmentLevel) override {
        return 50;
    }

    ND bool canApplyTogether(const Enchantment *enchantment) const override {
        return enchantment->enumID != EnumName::TRIDENT_RIPTIDE;
    }
};

