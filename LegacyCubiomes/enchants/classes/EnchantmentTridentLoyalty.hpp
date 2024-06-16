#pragma once

#include "LegacyCubiomes/enchants/enchantment.hpp"


class EnchantmentTridentLoyalty final : public Enchantment {
    static constexpr inline i8 LEVELS_MIN[3] = {12, 19, 26};

public:
    EnchantmentTridentLoyalty(std::string name, const Rarity *rarity) :
            Enchantment(std::move(name), rarity, &Type::TRIDENT,
                        EnumName::TRIDENT_LOYALTY, 3) {};

    int getMinCost(c_int enchantmentLevel) override { return LEVELS_MIN[enchantmentLevel - 1]; }

    int getMaxCost(c_int enchantmentLevel) override { return 50; }
};

