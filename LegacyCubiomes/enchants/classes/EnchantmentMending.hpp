#pragma once

#include "LegacyCubiomes/enchants/enchantment.hpp"


class EnchantmentMending final : public Enchantment {
public:
    EnchantmentMending(std::string name, const eRarity rarity)
        : Enchantment(std::move(name), rarity, &Type::BREAKABLE, EnumName::MENDING, 1, true){};

    int getMinCost(c_int enchantmentLevel) override { return 25; }

    int getMaxCost(c_int enchantmentLevel) override { return 75; }
};
