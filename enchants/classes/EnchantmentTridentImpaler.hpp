#pragma once

#include "enchants/enchantment.hpp"


class EnchantmentTridentImpaler final : public Enchantment {
    static constexpr i8 LEVELS_MIN[5] = {17, 24, 31, 38, 45};

public:
    EnchantmentTridentImpaler(std::string name, const eRarity rarity) :
            Enchantment(std::move(name), rarity, &Type::TRIDENT, EnumName::TRIDENT_IMPALER, 5) {};

    int getMinCost(c_int enchantmentLevel) override { return LEVELS_MIN[enchantmentLevel - 1]; }

    int getMaxCost(MU c_int enchantmentLevel) override { return 50; }
};

