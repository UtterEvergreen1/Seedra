#pragma once

#include "LegacyCubiomes/enchants/enchantment.hpp"

class EnchantmentBindingCurse : public Enchantment {
public:

    EnchantmentBindingCurse(const std::string& name, const Rarity *rarity) :
        Enchantment(name, rarity, &Type::WEARABLE, EnumName::BINDING_CURSE, 1, true) {};

    int getMinEnchantability(int enchantmentLevel) override {
        return 25;
    }

    int getMaxEnchantability(int enchantmentLevel) override {
        return 50;
    }

    static bool func_190936_d() {
        return true;
    }
};














