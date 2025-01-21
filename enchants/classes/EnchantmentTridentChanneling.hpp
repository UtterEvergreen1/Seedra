#pragma once

#include "enchants/enchantment.hpp"


class EnchantmentTridentChanneling final : public Enchantment {
public:
    EnchantmentTridentChanneling(std::string name, const eRarity rarity) :
            Enchantment(std::move(name), rarity, &Type::TRIDENT,
                        EnumName::TRIDENT_CHANNELING, 1) {};

    int getMinCost(c_int enchantmentLevel) override { return 25; }

    int getMaxCost(c_int enchantmentLevel) override { return 50; }
};

