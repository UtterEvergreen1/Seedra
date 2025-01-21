#pragma once

#include "enchants/enchantment.hpp"


class EnchantmentBindingCurse final : public Enchantment {
public:
    EnchantmentBindingCurse(const std::string& name, const eRarity rarity)
        : Enchantment(name, rarity, &Type::WEARABLE, EnumName::BINDING_CURSE, 1, true){};

    int getMinCost(c_int enchantmentLevel) override { return 25; }

    int getMaxCost(c_int enchantmentLevel) override { return 50; }
};
