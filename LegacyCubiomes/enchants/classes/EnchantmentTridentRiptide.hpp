#pragma once

#include "LegacyCubiomes/enchants/enchantment.hpp"


class EnchantmentTridentRiptide final : public Enchantment {
    static constexpr i8 LEVELS_MIN[3] = {17, 24, 31};

public:
    EnchantmentTridentRiptide(std::string name, const eRarity rarity) :
            Enchantment(std::move(name), rarity, &Type::TRIDENT,
                        EnumName::TRIDENT_RIPTIDE, 3) {}

    int getMinCost(c_int enchantmentLevel) override { return LEVELS_MIN[enchantmentLevel - 1]; }

    int getMaxCost(c_int enchantmentLevel) override { return 50; }

    ND bool canApplyTogether(const Enchantment *enchantment) const override {
        return Enchantment::canApplyTogether(enchantment) &&
               enchantment->enumID != EnumName::TRIDENT_LOYALTY &&
               enchantment->enumID != EnumName::TRIDENT_CHANNELING;
    }
};

