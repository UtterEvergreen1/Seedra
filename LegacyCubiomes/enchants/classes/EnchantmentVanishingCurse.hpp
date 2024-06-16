#pragma once

#include "LegacyCubiomes/enchants/enchantment.hpp"


class EnchantmentVanishingCurse final : public Enchantment {
public:
    EnchantmentVanishingCurse(std::string name, const Rarity *rarity) :
            Enchantment(std::move(name), rarity, &Type::ALL, EnumName::VANISHING_CURSE, 1, true) {};

    int getMinCost(c_int enchantmentLevel) override { return 25; }

    int getMaxCost(c_int enchantmentLevel) override { return 50; }
};

