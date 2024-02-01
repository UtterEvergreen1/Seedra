#pragma once

#include "LegacyCubiomes/enchants/enchantment.hpp"


class EnchantmentTridentRiptide final : public Enchantment {
    static constexpr inline int8_t LEVELS_MIN[3] = {17, 24, 31};

public:
    EnchantmentTridentRiptide(std::string name, const Rarity *rarity) :
            Enchantment(std::move(name), rarity, &Type::TRIDENT,
                        EnumName::TRIDENT_RIPTIDE, 3) {};

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

