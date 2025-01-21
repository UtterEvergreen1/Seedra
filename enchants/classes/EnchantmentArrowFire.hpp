#pragma once

#include "enchants/enchantment.hpp"


class EnchantmentArrowFire final : public Enchantment {
public:
    EnchantmentArrowFire(std::string name, const eRarity rarity)
        : Enchantment(std::move(name), rarity, &Type::BOW, EnumName::ARROW_FIRE, 1){};

    int getMinCost(MU c_int enchantmentLevel) override { return 20; }

    int getMaxCost(MU c_int enchantmentLevel) override { return 50; }
};
