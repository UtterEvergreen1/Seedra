#pragma once

#include "LegacyCubiomes/enchants/enchantment.hpp"


class EnchantmentVanishingCurse : public Enchantment {
public:

    EnchantmentVanishingCurse(std::string name, const Rarity *rarity) :
            Enchantment(std::move(name), rarity, &Type::ALL, EnumName::VANISHING_CURSE, 1, true) {};

    int getMinCost(int enchantmentLevel) override {
        return 25;
    }

    int getMaxCost(int enchantmentLevel) override {
        return 50;
    }

    static bool func_190936_d() {
        return true;
    }
};

