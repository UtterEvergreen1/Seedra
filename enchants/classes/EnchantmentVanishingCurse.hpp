#pragma once

#include "enchants/enchantment.hpp"


class EnchantmentVanishingCurse final : public Enchantment {
public:
    EnchantmentVanishingCurse(std::string name, const eRarity rarity) :
            Enchantment(std::move(name), rarity, &Type::ALL, EnumName::VANISHING_CURSE, 1, true) {};

    int getMinCost(MU c_int enchantmentLevel) override { return 25; }

    int getMaxCost(MU c_int enchantmentLevel) override { return 50; }
};

